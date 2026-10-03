#include "display_window.hpp"
#include "dx12_presenter.hpp"
#include "ge_gpu_backend.hpp"
#include "vcs_config.hpp"
#include "vcs_runtime_log.hpp"
#include "vcs_vehicle_input.hpp"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <chrono>
#include <condition_variable>
#include <thread>

namespace vcs {
namespace {

constexpr std::uint32_t kPspSelect = 0x000001u;
constexpr std::uint32_t kPspStart = 0x000008u;
constexpr std::uint32_t kPspUp = 0x000010u;
constexpr std::uint32_t kPspRight = 0x000020u;
constexpr std::uint32_t kPspDown = 0x000040u;
constexpr std::uint32_t kPspLeft = 0x000080u;
constexpr std::uint32_t kPspLTrigger = 0x000100u;
constexpr std::uint32_t kPspRTrigger = 0x000200u;
constexpr std::uint32_t kPspTriangle = 0x001000u;
constexpr std::uint32_t kPspCircle = 0x002000u;
constexpr std::uint32_t kPspCross = 0x004000u;
constexpr std::uint32_t kPspSquare = 0x008000u;

constexpr UINT kMessagePresent = WM_APP + 1u;

struct KeyBinding {
    int virtual_key;
    std::uint32_t psp_button;
};

// GTA San Andreas' own PC defaults, mapped onto the PSP pad.
//
// VCS is a Vice City-era game on a console with four faces and two shoulders,
// so it cannot take San Andreas' full set; what it shares, it shares on the
// same key. Verified against the published SA control list rather than
// recalled: Sprint is Space and Jump is Left Shift (not the other way round),
// vehicles enter on F or Return, and weapons cycle on Q and E.
//
// The two collisions worth naming:
//
//   * San Andreas fires and accelerates both on the left mouse button. VCS
//     splits them, but ThirteenAG's modern control scheme already routes both
//     through R -- R fires on foot and accelerates in a vehicle -- so one
//     binding reproduces San Andreas in both contexts.
//   * Crouch (C) has no counterpart. Vice City-era protagonists do not crouch,
//     so it stays unbound rather than being given something to do.
//
// Movement is deliberately absent here: WASD drives the analog stick, because
// the digital D-pad makes the character walk in eight directions.
constexpr KeyBinding kKeyBindings[] = {
    // On foot: sprint, jump, enter/exit, weapon cycling.
    {VK_SPACE, kPspCross},     // Sprint on foot, handbrake in a vehicle
    {VK_LSHIFT, kPspSquare},   // Jump on foot, brake/reverse in a vehicle
    {VK_RSHIFT, kPspSquare},
    {'F', kPspTriangle},       // Enter/exit vehicle
    {VK_RETURN, kPspTriangle},
    // Weapon select and radio both live on the D-pad in VCS, so Q/E and the
    // wheel all land there -- the same one binding covers both contexts, which
    // is how San Andreas' wheel behaves.
    {'Q', kPspLeft},           // Previous weapon / radio station
    {'E', kPspRight},          // Next weapon / radio station
    {'H', kPspLTrigger},       // Horn
    // The arrow keys stay on the D-pad: menus are navigated with them, and the
    // PSP D-pad is what the game's own front end reads.
    {VK_UP, kPspUp},           {VK_DOWN, kPspDown},
    {VK_LEFT, kPspLeft},       {VK_RIGHT, kPspRight},
    {VK_ESCAPE, kPspStart},    // Pause. Closing the window moved to Alt+F4.
    {VK_TAB, kPspSelect},
};

// Mouse buttons follow San Andreas: fire left, aim right, look behind on the
// middle button.
//
// Measured out of the game rather than assumed. ThirteenAG's plugin replaces
// three pad accessors wholesale, so disassembling the stock versions at the
// addresses it patches says which field each one reads:
//
//   CPad::GetWeapon           (fire)        loads 0x2C -> circle
//   CPad::GetTarget           (aim)         loads 0x0E -> R1
//   CPad::GetLookBehindForPed               loads 0x0E and 0x0A -> R1, L1
//
// Worth stating because two earlier attempts here were guesses. The accessors
// in the plugin's own source describe its ModernControlScheme, not the shipped
// game, and reading them as documentation put fire on the wrong button twice.
constexpr KeyBinding kMouseBindings[] = {
    {VK_LBUTTON, kPspCircle},    // Fire / punch
    {VK_RBUTTON, kPspRTrigger},  // Target
    {VK_MBUTTON, kPspLTrigger},  // Look behind
};

// WASD drives the analog stick rather than the D-pad, matching San Andreas.
constexpr int kMoveForward = 'W';
constexpr int kMoveBack = 'S';
constexpr int kMoveLeft = 'A';
constexpr int kMoveRight = 'D';


struct WindowState {
    std::thread thread;
    std::mutex mutex;
    std::condition_variable ready_signal;
    std::vector<std::uint32_t> pixels;  // 0x00RRGGBB, top-down
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint64_t frame_index{};
    std::atomic<HWND> window{nullptr};
    std::atomic<bool> ready{false};
    std::atomic<bool> focused{false};
    std::atomic<bool> close_requested{false};
    // Raw mouse motion accumulated by the window thread and drained by the
    // guest's controller poll. Raw input rather than cursor position: the
    // cursor stops at the screen edge, and a camera that stops turning when
    // the pointer reaches the edge of a 3440-wide monitor is unusable.
    std::atomic<std::int32_t> mouse_dx{0};
    std::atomic<std::int32_t> mouse_dy{0};
    std::atomic<std::int32_t> wheel{0};
    // Set while a movie is on screen; see display_window_set_aspect_lock.
    // Atomic because the guest thread raises it and the window thread paints.
    std::atomic<bool> aspect_lock{false};
    bool mouse_captured{false};
    std::string status{"booting"};
    DisplayConfiguration configuration{};
    int client_width{480};
    int client_height{272};
    // Back buffer for WM_PAINT. Filling the window black and then stretching
    // into it directly made the whole client flash black every frame, which is
    // very visible once the client is desktop sized.
    HDC back_buffer_dc{nullptr};
    HBITMAP back_buffer_bitmap{nullptr};
    HGDIOBJ back_buffer_previous{nullptr};
    int back_buffer_width{};
    int back_buffer_height{};
};

constexpr UINT_PTR kStatusTimer = 1u;

WindowState &window_state() {
    static WindowState state;
    return state;
}

bool key_down(int virtual_key) noexcept {
    return (GetAsyncKeyState(virtual_key) & 0x8000) != 0;
}

// XInput, loaded at run time. Linking it would make the executable refuse to
// start on a machine without the redistributable, for a feature that is
// optional by definition -- a missing gamepad is not an error.
struct XInputGamepad {
    std::uint16_t buttons;
    std::uint8_t left_trigger;
    std::uint8_t right_trigger;
    std::int16_t lx, ly, rx, ry;
};
struct XInputStatePacket {
    std::uint32_t packet;
    XInputGamepad gamepad;
};
using PfnXInputGetState = std::uint32_t(WINAPI *)(std::uint32_t, XInputStatePacket *);

constexpr std::uint16_t kPadDpadUp = 0x0001u;
constexpr std::uint16_t kPadDpadDown = 0x0002u;
constexpr std::uint16_t kPadDpadLeft = 0x0004u;
constexpr std::uint16_t kPadDpadRight = 0x0008u;
constexpr std::uint16_t kPadStart = 0x0010u;
constexpr std::uint16_t kPadBack = 0x0020u;
constexpr std::uint16_t kPadLeftShoulder = 0x0100u;
constexpr std::uint16_t kPadRightShoulder = 0x0200u;
constexpr std::uint16_t kPadA = 0x1000u;
constexpr std::uint16_t kPadB = 0x2000u;
constexpr std::uint16_t kPadX = 0x4000u;
constexpr std::uint16_t kPadY = 0x8000u;

[[nodiscard]] PfnXInputGetState xinput_get_state() noexcept {
    // Newest first: 1_4 ships with Windows 8 and later, 9_1_0 is the version
    // present on every machine since Vista, and 1_3 covers the old SDK
    // redistributable.
    static PfnXInputGetState resolved = [] () -> PfnXInputGetState {
        for (const wchar_t *name : {L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll"}) {
            if (HMODULE module = LoadLibraryW(name)) {
                if (auto function = reinterpret_cast<PfnXInputGetState>(
                        reinterpret_cast<void *>(GetProcAddress(module, "XInputGetState"))))
                    return function;
            }
        }
        return nullptr;
    }();
    return resolved;
}

// Analog sticks arrive as signed 16-bit and leave as the PSP's 0..255 with 128
// at rest. The dead zone is applied before the rescale so the rest position is
// exactly 128 rather than a value that drifts by a unit or two.
[[nodiscard]] std::uint8_t stick_to_psp(std::int16_t value, bool invert) noexcept {
    constexpr int kDeadZone = 7849;  // XInput's own documented left-stick figure
    int magnitude = std::abs(static_cast<int>(value));
    if (magnitude <= kDeadZone) return 128u;
    magnitude = (magnitude - kDeadZone) * 32767 / (32767 - kDeadZone);
    int signed_value = value < 0 ? -magnitude : magnitude;
    if (invert) signed_value = -signed_value;
    return static_cast<std::uint8_t>(std::clamp(128 + signed_value * 127 / 32767, 0, 255));
}

int legacy_configured_scale() {
    const char *text = std::getenv("PSPRECOMP_WINDOW_SCALE");
    if (text == nullptr || *text == '\0') return 0;
    char *end = nullptr;
    const long value = std::strtol(text, &end, 10);
    if (end == text || *end != '\0' || value < 1 || value > 16) return 0;
    return static_cast<int>(value);
}

void resolve_client_size(WindowState &state) {
    const int desktop_width = std::max(1, GetSystemMetrics(SM_CXSCREEN));
    const int desktop_height = std::max(1, GetSystemMetrics(SM_CYSCREEN));

    if (state.configuration.fullscreen ||
        state.configuration.resolution_mode == DisplayResolutionMode::Desktop) {
        state.client_width = desktop_width;
        state.client_height = desktop_height;
    } else if (state.configuration.resolution_mode == DisplayResolutionMode::Custom) {
        state.client_width = static_cast<int>(state.configuration.custom_width);
        state.client_height = static_cast<int>(state.configuration.custom_height);
    } else {
        state.client_width = 480;
        state.client_height = 272;
    }

    // Backwards compatibility with the stage scripts that predate VCSNative.ini.
    // An explicit legacy scale overrides the windowed client dimensions only.
    if (!state.configuration.fullscreen) {
        const int legacy_scale = legacy_configured_scale();
        if (legacy_scale != 0) {
            state.client_width = 480 * legacy_scale;
            state.client_height = 272 * legacy_scale;
        }
    }
}

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    WindowState &state = window_state();
    switch (message) {
    case kMessagePresent:
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_SETFOCUS:
        state.focused.store(true, std::memory_order_relaxed);
        return 0;
    case WM_KILLFOCUS:
        state.focused.store(false, std::memory_order_relaxed);
        return 0;
    case WM_KEYDOWN:
        // Escape is the pause button now that it is bound to Start, the way it
        // is in San Andreas. Alt+F4 and the window's close box still close.
        return 0;
    case WM_INPUT: {
        // Raw mouse deltas. Sized from the message rather than assumed: the
        // header is followed by a union whose size differs between builds.
        UINT size = 0u;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr,
                        &size, sizeof(RAWINPUTHEADER));
        if (size != 0u && size <= 256u) {
            alignas(8) std::byte buffer[256];
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, buffer,
                                &size, sizeof(RAWINPUTHEADER)) == size) {
                const RAWINPUT *raw = reinterpret_cast<const RAWINPUT *>(buffer);
                if (raw->header.dwType == RIM_TYPEMOUSE &&
                    (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0) {
                    state.mouse_dx.fetch_add(raw->data.mouse.lLastX, std::memory_order_relaxed);
                    state.mouse_dy.fetch_add(raw->data.mouse.lLastY, std::memory_order_relaxed);
                }
            }
        }
        return 0;
    }
    case WM_MOUSEWHEEL:
        state.wheel.fetch_add(GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA,
                              std::memory_order_relaxed);
        return 0;
    case WM_SETCURSOR:
        // Hide the pointer over the client area: the mouse is aiming the
        // camera, not pointing at anything. Answering WM_SETCURSOR rather than
        // calling ShowCursor avoids its counter, which has to be balanced
        // exactly and leaves the cursor invisible everywhere if it is not.
        // The non-client area keeps its arrow so the title bar stays usable.
        if (LOWORD(lparam) == HTCLIENT) {
            SetCursor(nullptr);
            return TRUE;
        }
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_TIMER: {
        if (wparam != kStatusTimer) break;
        std::wstring title = L"VCSNative — GTA: Vice City Stories — ";
        {
            std::lock_guard<std::mutex> guard(state.mutex);
            title.append(state.status.begin(), state.status.end());
        }
        title += L" — ";
        title += std::to_wstring(state.client_width);
        title += L"x";
        title += std::to_wstring(state.client_height);
        title += state.configuration.upscale_filter == DisplayUpscaleFilter::Bilinear
                     ? L" bilinear" : L" nearest";
        SetWindowTextW(window, title.c_str());
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT paint{};
        HDC window_context = BeginPaint(window, &paint);
        RECT client{};
        GetClientRect(window, &client);
        // Once DirectX 12 owns presentation there is no GDI back buffer to
        // repaint. Begin/EndPaint still validates the update region; DWM keeps
        // the last flip-model swapchain image visible between presents.
        if (dx12_presenter_active()) {
            EndPaint(window, &paint);
            return 0;
        }
        const int client_w = std::max(1L, client.right - client.left);
        const int client_h = std::max(1L, client.bottom - client.top);
        std::lock_guard<std::mutex> guard(state.mutex);
        if (state.back_buffer_dc == nullptr || state.back_buffer_width != client_w ||
            state.back_buffer_height != client_h) {
            if (state.back_buffer_dc != nullptr) {
                SelectObject(state.back_buffer_dc, state.back_buffer_previous);
                DeleteObject(state.back_buffer_bitmap);
                DeleteDC(state.back_buffer_dc);
            }
            state.back_buffer_dc = CreateCompatibleDC(window_context);
            state.back_buffer_bitmap =
                CreateCompatibleBitmap(window_context, client_w, client_h);
            state.back_buffer_previous =
                SelectObject(state.back_buffer_dc, state.back_buffer_bitmap);
            state.back_buffer_width = client_w;
            state.back_buffer_height = client_h;
        }
        HDC context = state.back_buffer_dc != nullptr ? state.back_buffer_dc : window_context;
        if (state.width != 0u && state.height != 0u && !state.pixels.empty()) {
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = static_cast<LONG>(state.width);
            // Negative height selects a top-down DIB, matching our row order.
            info.bmiHeader.biHeight = -static_cast<LONG>(state.height);
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            FillRect(context, &client, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            const PresentationRectangle output = calculate_presentation_rectangle(
                static_cast<std::uint32_t>(std::max(0L, client.right - client.left)),
                static_cast<std::uint32_t>(std::max(0L, client.bottom - client.top)),
                state.width, state.height,
                state.aspect_lock.load(std::memory_order_relaxed)
                    ? DisplayAspectMode::Preserve
                    : state.configuration.aspect_mode,
                state.configuration.integer_scale);
            // HALFTONE is expensive and buys nothing when the blit is 1:1,
            // which is the normal case once the internal target matches the
            // client size.
            const bool scaling = output.width != static_cast<int>(state.width) ||
                                 output.height != static_cast<int>(state.height);
            if (scaling && state.configuration.upscale_filter == DisplayUpscaleFilter::Bilinear) {
                SetStretchBltMode(context, HALFTONE);
                SetBrushOrgEx(context, 0, 0, nullptr);
            } else {
                SetStretchBltMode(context, COLORONCOLOR);
            }
            StretchDIBits(context,
                          output.x, output.y, output.width, output.height,
                          0, 0, static_cast<int>(state.width), static_cast<int>(state.height),
                          state.pixels.data(), &info, DIB_RGB_COLORS, SRCCOPY);
        } else {
            FillRect(context, &client, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            const std::wstring message(state.status.begin(), state.status.end());
            SetBkMode(context, TRANSPARENT);
            SetTextColor(context, RGB(200, 200, 200));
            DrawTextW(context, message.c_str(), -1, &client,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        if (context != window_context)
            BitBlt(window_context, 0, 0, client_w, client_h, context, 0, 0, SRCCOPY);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_CLOSE:
        state.close_requested.store(true, std::memory_order_relaxed);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

void window_thread_main() {
    WindowState &state = window_state();
    const HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance;
    // The project does not define UNICODE, so IDC_ARROW expands to the ANSI
    // MAKEINTRESOURCE form; select the wide one explicitly.
    window_class.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    window_class.lpszClassName = L"VCSNativeDisplay";
    RegisterClassExW(&window_class);

    resolve_client_size(state);
    const DWORD style = state.configuration.fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    RECT bounds{0, 0, state.client_width, state.client_height};
    int window_x = 0;
    int window_y = 0;
    if (!state.configuration.fullscreen) {
        AdjustWindowRect(&bounds, style, FALSE);
        // Desktop resolution mode asks for a client as large as the monitor, and
        // the frame AdjustWindowRect adds on top of it made the window larger
        // than the screen. Placed at CW_USEDEFAULT it was also offset, so the
        // right and bottom of the client sat outside the monitor: the radar was
        // halved and the money/weapon icons were clipped by the screen edge, not
        // by the renderer. Fit the whole window inside the work area instead and
        // shrink the client by whatever the frame costs.
        RECT work{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
        const int frame_width = (bounds.right - bounds.left) - state.client_width;
        const int frame_height = (bounds.bottom - bounds.top) - state.client_height;
        const int available_width =
            static_cast<int>(std::max(1L, work.right - work.left)) - frame_width;
        const int available_height =
            static_cast<int>(std::max(1L, work.bottom - work.top)) - frame_height;
        state.client_width = std::clamp(state.client_width, 1, std::max(1, available_width));
        state.client_height = std::clamp(state.client_height, 1, std::max(1, available_height));
        bounds = RECT{0, 0, state.client_width, state.client_height};
        AdjustWindowRect(&bounds, style, FALSE);
        window_x = static_cast<int>(work.left);
        window_y = static_cast<int>(work.top);
    }
    const HWND window = CreateWindowExW(
        0, window_class.lpszClassName, L"VCSNative — GTA: Vice City Stories",
        style, window_x, window_y,
        bounds.right - bounds.left, bounds.bottom - bounds.top,
        nullptr, nullptr, instance, nullptr);
    state.window.store(window, std::memory_order_release);
    {
        std::lock_guard<std::mutex> guard(state.mutex);
        state.ready.store(true, std::memory_order_release);
    }
    state.ready_signal.notify_all();
    if (window == nullptr) return;

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    SetForegroundWindow(window);
    state.focused.store(true, std::memory_order_relaxed);
    SetTimer(window, kStatusTimer, 250u, nullptr);

    // Raw mouse input for the camera. Registered on this window rather than
    // with RIDEV_INPUTSINK, so the game stops turning when you tab away.
    const RAWINPUTDEVICE mouse{0x01u, 0x02u, 0u, window};
    RegisterRawInputDevices(&mouse, 1u, sizeof(mouse));

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    state.window.store(nullptr, std::memory_order_release);
}

void ensure_window_started() {
    WindowState &state = window_state();
    static std::once_flag once;
    std::call_once(once, [&state] {
        state.configuration = vcs_configuration().display;
        // Detached: the UI thread outlives every emulation stop path, and a
        // function-local static std::thread must never be destroyed joinable.
        state.thread = std::thread(window_thread_main);
        state.thread.detach();
        std::unique_lock<std::mutex> guard(state.mutex);
        state.ready_signal.wait(guard, [&state] { return state.ready.load(std::memory_order_acquire); });
    });
}

} // namespace

bool display_window_enabled() {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_WINDOW");
        if (text != nullptr && *text != '\0') return std::string(text) != "0";
        const VcsConfiguration &configuration = vcs_configuration();
        return configuration.initialized && configuration.display.enabled;
    }();
    return enabled;
}

void display_window_start() {
    if (!display_window_enabled()) return;
    ensure_window_started();
    if (vcs_configuration().rendering.backend != RenderingBackend::DirectX12) return;

    WindowState &state = window_state();
    HWND window = state.window.load(std::memory_order_acquire);
    if (ge_gpu_backend_active()) {
        // Native GE owns the D3D12 queue/swapchain. Avoid creating a second
        // presenter/device for the same HWND.
        ge_gpu_backend_set_native_window(window);
        return;
    }

    // Native GE may have been intentionally disabled or may have failed its
    // feature probe. In non-strict play mode the stable software GE can still
    // be shown through the standalone D3D12 presenter.
    std::string error;
    if (window == nullptr || !dx12_presenter_initialize(window, error)) {
        runtime_log_error("dx12 initialize", error.empty() ? "window unavailable" : error);
        std::cerr << "[dx12] initialize failed at startup: "
                  << (error.empty() ? "window unavailable" : error) << "\n";
        return;
    }
    const Dx12PresenterStatus dx = dx12_presenter_status();
    std::cout << "DirectX 12 presenter:  " << dx.adapter_name
              << " / " << dx.frames_in_flight << " frames in flight"
              << (dx.tearing_supported ? " / tearing" : "") << "\n";
}

void display_window_set_aspect_lock(bool locked) noexcept {
    if (!display_window_enabled()) return;
    window_state().aspect_lock.store(locked, std::memory_order_relaxed);
}

void display_window_set_status(const char *status) {
    if (!display_window_enabled() || status == nullptr) return;
    WindowState &state = window_state();
    if (!state.ready.load(std::memory_order_acquire)) return;
    std::lock_guard<std::mutex> guard(state.mutex);
    state.status = status;
}

void display_window_present(const psprecomp::GuestMemory &memory,
                            const FramebufferDescription &description) {
    if (!display_window_enabled()) return;
    ensure_window_started();
    if (description.address == 0u || description.width == 0u || description.height == 0u ||
        description.stride == 0u) {
        return;
    }
    WindowState &state = window_state();
    const HWND window = state.window.load(std::memory_order_acquire);
    if (window == nullptr) return;

    if (vcs_configuration().rendering.backend == RenderingBackend::DirectX12 &&
        !vcs_configuration().rendering.dx12_ge_color) {
        if (!dx12_presenter_active()) {
            std::string error;
            if (!dx12_presenter_initialize(window, error)) {
                runtime_log_error("dx12 initialize", error);
                std::cerr << "[dx12] initialize failed: " << error << "\n";
            }
        }
        if (dx12_presenter_active()) {
            try {
                std::vector<std::byte> rgba = decode_framebuffer_rgba(memory, description);
                std::string error;
                if (!dx12_presenter_present_rgba(
                        rgba, description.width, description.height, state.configuration,
                        state.aspect_lock.load(std::memory_order_relaxed), error)) {
                    runtime_log_error("dx12 present", error);
                    std::cerr << "[dx12] present failed: " << error << "\n";
                    dx12_presenter_shutdown();
                } else {
                    return;
                }
            } catch (const std::exception &error) {
                runtime_log_error("dx12 framebuffer decode", error.what());
                std::cerr << "[dx12] framebuffer decode failed: " << error.what() << "\n";
                return;
            }
        }
    }

    std::vector<std::uint8_t> rgb;
    try {
        rgb = decode_framebuffer_rgb(memory, description);
    } catch (const std::exception &) {
        // A transient framebuffer pointer outside EDRAM must never take the
        // host down; the previous frame simply stays on screen.
        return;
    }

    const std::size_t pixel_count = static_cast<std::size_t>(description.width) * description.height;
    {
        std::lock_guard<std::mutex> guard(state.mutex);
        state.width = description.width;
        state.height = description.height;
        state.pixels.resize(pixel_count);
        for (std::size_t index = 0; index < pixel_count; ++index) {
            const std::uint32_t red = rgb[index * 3u + 0u];
            const std::uint32_t green = rgb[index * 3u + 1u];
            const std::uint32_t blue = rgb[index * 3u + 2u];
            state.pixels[index] = (red << 16u) | (green << 8u) | blue;
        }
        ++state.frame_index;
    }
    PostMessageW(window, kMessagePresent, 0, 0);
}

void display_window_present_rgba(std::span<const std::byte> rgba,
                                 std::uint32_t width,
                                 std::uint32_t height) {
    if (!display_window_enabled() || width == 0u || height == 0u) return;
    const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
    if (rgba.size() < pixel_count * 4u) return;
    ensure_window_started();
    WindowState &state = window_state();
    const HWND window = state.window.load(std::memory_order_acquire);
    if (window == nullptr) return;
    if (vcs_configuration().rendering.backend == RenderingBackend::DirectX12 &&
        !vcs_configuration().rendering.dx12_ge_color) {
        if (!dx12_presenter_active()) {
            std::string error;
            if (!dx12_presenter_initialize(window, error)) {
                runtime_log_error("dx12 initialize", error);
                std::cerr << "[dx12] initialize failed: " << error << "\n";
            }
        }
        if (dx12_presenter_active()) {
            std::string error;
            if (!dx12_presenter_present_rgba(
                    rgba, width, height, state.configuration,
                    state.aspect_lock.load(std::memory_order_relaxed), error)) {
                runtime_log_error("dx12 present", error);
                std::cerr << "[dx12] present failed: " << error << "\n";
                dx12_presenter_shutdown();
            } else {
                return;
            }
        }
    }
    {
        std::lock_guard<std::mutex> guard(state.mutex);
        state.width = width;
        state.height = height;
        state.pixels.resize(pixel_count);
        // One 32-bit load and a red/blue swap per pixel. The byte-at-a-time
        // version cost several milliseconds per frame once the internal target
        // reached desktop resolution (a 3440x1440 frame is 4.95M pixels).
        for (std::size_t index = 0u; index < pixel_count; ++index) {
            std::uint32_t source = 0u;
            std::memcpy(&source, rgba.data() + index * 4u, sizeof(source));
            state.pixels[index] = (source & 0x0000FF00u) |
                                  ((source & 0x000000FFu) << 16u) |
                                  ((source >> 16u) & 0x000000FFu);
        }
        ++state.frame_index;
    }
    PostMessageW(window, kMessagePresent, 0, 0);
}

DisplayWindowSurface display_window_surface() {
    if (!display_window_enabled()) return {};
    ensure_window_started();
    WindowState &state = window_state();
    HWND window = state.window.load(std::memory_order_acquire);
    if (window == nullptr) return {};
    RECT client{};
    GetClientRect(window, &client);
    return {window, GetModuleHandleW(nullptr),
            static_cast<std::uint32_t>(std::max(0L, client.right - client.left)),
            static_cast<std::uint32_t>(std::max(0L, client.bottom - client.top))};
}

std::uint32_t display_window_buttons() {
    // Shares one reading with the analog path. Doing its own pass over
    // kKeyBindings is what left the mouse out entirely: the mouse buttons and
    // the wheel live in the reading below, so punching and tuning the radio
    // never reached the guest while the keyboard worked fine.
    return display_window_input().buttons;
}

void display_window_analog(std::uint8_t &x, std::uint8_t &y) {
    const HostInputState input = display_window_input();
    x = input.analog_x;
    y = input.analog_y;
}

HostInputState display_window_input() {
    // The buttons and the analog stick are fetched by separate callers within
    // one controller poll, and the mouse deltas can only be drained once -- so
    // the reading is cached for a few milliseconds and both callers get the
    // same one. Shorter than a frame, so nothing here is ever perceptibly old.
    static std::mutex cache_mutex;
    static HostInputState cached{};
    static std::chrono::steady_clock::time_point cached_at{};
    const std::lock_guard<std::mutex> guard(cache_mutex);
    const auto now = std::chrono::steady_clock::now();
    if (cached_at.time_since_epoch().count() != 0 &&
        now - cached_at < std::chrono::milliseconds(4))
        return cached;
    cached_at = now;
    cached = HostInputState{};

    HostInputState input{};
    const auto publish = [&]() -> HostInputState & { cached = input; return cached; };
    if (!display_window_enabled()) return publish();
    WindowState &state = window_state();
    if (!state.ready.load(std::memory_order_acquire)) return publish();

    // The mouse keeps accumulating while the window is not focused, so the
    // deltas are drained either way. Leaving them to pile up made the camera
    // whip round on the frame focus came back.
    const std::int32_t mouse_dx = state.mouse_dx.exchange(0, std::memory_order_relaxed);
    const std::int32_t mouse_dy = state.mouse_dy.exchange(0, std::memory_order_relaxed);
    const std::int32_t wheel = state.wheel.exchange(0, std::memory_order_relaxed);
    if (!state.focused.load(std::memory_order_relaxed)) return publish();

    for (const KeyBinding &binding : kKeyBindings)
        if (key_down(binding.virtual_key)) input.buttons |= binding.psp_button;
    for (const KeyBinding &binding : kMouseBindings)
        if (key_down(binding.virtual_key)) input.buttons |= binding.psp_button;

    // Driving and walking want opposite things from the same keys, and the
    // guest tells us which one is happening: only vehicle code reads the
    // throttle accessors, so their being read is the signal. See
    // vcs_vehicle_input.hpp.
    const bool driving = vcs_player_in_vehicle();

    int move_x = 0;
    int move_y = 0;
    if (key_down(kMoveLeft)) move_x -= 1;
    if (key_down(kMoveRight)) move_x += 1;
    if (!driving) {
        if (key_down(kMoveForward)) move_y -= 1;
        if (key_down(kMoveBack)) move_y += 1;
    } else {
        // In a vehicle the stick's Y axis is lean, not throttle, so W and S
        // must keep out of it -- feeding it made the bike wheelie every time
        // the player accelerated. San Andreas leans with the arrow keys, and
        // that is where it goes.
        if (key_down(VK_UP)) move_y -= 1;
        if (key_down(VK_DOWN)) move_y += 1;
    }
    // W and S drive whatever the context: the accessors they reach are the
    // vehicle's own, so on foot the guest never asks and nothing happens.
    input.accelerate = key_down(kMoveForward);
    input.brake = key_down(kMoveBack);
    // Left Alt is San Andreas' walk modifier: half deflection instead of full.
    const int reach = key_down(VK_LMENU) ? 60 : 127;
    input.analog_x = static_cast<std::uint8_t>(std::clamp(128 + move_x * reach, 0, 255));
    input.analog_y = static_cast<std::uint8_t>(std::clamp(128 + move_y * reach, 0, 255));

    // San Andreas puts weapon cycling and radio tuning on the same wheel, and
    // VCS puts both on the D-pad -- so one binding reproduces both, without the
    // host needing to know whether you are on foot or driving.
    //
    // Held across several polls rather than pulsed for one. A wheel notch is
    // instantaneous, and a button that goes down and up inside a single poll is
    // a press the game never sees: it compares this poll against the last one
    // to find edges, and both of them can miss the middle.
    static int wheel_hold = 0;
    static std::uint32_t wheel_button = 0u;
    if (wheel != 0) {
        wheel_button = wheel > 0 ? kPspLeft : kPspRight;
        wheel_hold = 4;
    }
    if (wheel_hold > 0) {
        --wheel_hold;
        input.buttons |= wheel_button;
    }

    const ControlsConfiguration &controls = vcs_configuration().controls;
    const int sensitivity = static_cast<int>(controls.mouse_sensitivity);
    // A curve rather than a multiply-and-clamp.
    //
    // The axis is a stick deflection, so it is a turn *rate* with a hard
    // ceiling at 127, and `delta * 12` reaches that ceiling at eleven counts of
    // mouse movement. An ordinary flick produces many times that, so the value
    // sat pinned at the ceiling nearly all the time: every difference between
    // a nudge and a sweep was discarded before the game saw it, which is what
    // made aiming feel like it moved in steps.
    //
    // x/(x+k) keeps small movements proportional -- a slow drag still maps
    // almost linearly -- while approaching the ceiling asymptotically instead
    // of slamming into it, so a fast flick stays faster than a slow one all the
    // way up. Sensitivity now scales the curve rather than the clamp.
    // Full range, and the curve rises quickly to reach it.
    //
    // Capping the peak at 63 was tried, on the theory that the rate was double
    // what the camera code expects -- the accessor being replaced does shift
    // its result right by one. It is not the answer: the pad's right stick goes
    // to the full 127 through a different path, turns at a speed the player
    // likes, and stalls no more than the capped mouse did. All the cap achieved
    // was a mouse that crawled.
    //
    // The stalling itself tracks something else. It is worst just after the
    // game starts and clears on its own, and emulation speed was measured at
    // 69-75% through boot, reaching 100% later -- see the intro-audio work.
    // A camera integrating at seven tenths of the intended rate reads as stuck.
    const auto camera_response = [sensitivity](std::int32_t delta) {
        const double scaled = std::abs(delta) * (sensitivity / 12.0);
        const double magnitude = 127.0 * scaled / (scaled + 12.0);
        return static_cast<int>(std::lround(delta < 0 ? -magnitude : magnitude));
    };
    input.camera_x = camera_response(mouse_dx);
    // Negated: raw mouse Y grows downwards, and the axis the game reads treats
    // positive as looking up. Pushing the mouse forward has to raise the view.
    input.camera_y = camera_response(-mouse_dy);
    if (controls.invert_camera_y) input.camera_y = -input.camera_y;

    if (const PfnXInputGetState get_state = xinput_get_state()) {
        // Merge every connected slot instead of trusting slot 0: one physical
        // pad can surface twice (e.g. a Steam/xpad virtual mirror under
        // Proton), and the live one is not necessarily the first.
        XInputStatePacket pad{};
        bool any_connected = false;
        const auto stronger = [](std::int16_t a, std::int16_t b) {
            return std::abs(static_cast<int>(b)) > std::abs(static_cast<int>(a)) ? b : a;
        };
        for (std::uint32_t slot = 0u; slot < 4u; ++slot) {
            XInputStatePacket slot_state{};
            if (get_state(slot, &slot_state) != 0u) continue;
            any_connected = true;
            const XInputGamepad &g = slot_state.gamepad;
            pad.gamepad.buttons |= g.buttons;
            pad.gamepad.left_trigger = std::max(pad.gamepad.left_trigger, g.left_trigger);
            pad.gamepad.right_trigger = std::max(pad.gamepad.right_trigger, g.right_trigger);
            pad.gamepad.lx = stronger(pad.gamepad.lx, g.lx);
            pad.gamepad.ly = stronger(pad.gamepad.ly, g.ly);
            pad.gamepad.rx = stronger(pad.gamepad.rx, g.rx);
            pad.gamepad.ry = stronger(pad.gamepad.ry, g.ry);
        }
        if (any_connected) {
            const std::uint16_t b = pad.gamepad.buttons;
            // The pad follows San Andreas' console layout, which is also the
            // scheme ThirteenAG's plugin assumes: cross accelerates and
            // sprints, square brakes and jumps, triangle enters vehicles.
            if (b & kPadA) input.buttons |= kPspCross;
            if (b & kPadX) input.buttons |= kPspSquare;
            if (b & kPadY) input.buttons |= kPspTriangle;
            if (b & kPadB) input.buttons |= kPspCircle;
            if (b & kPadLeftShoulder) input.buttons |= kPspLTrigger;
            if (b & kPadRightShoulder) input.buttons |= kPspRTrigger;
            if (b & kPadStart) input.buttons |= kPspStart;
            if (b & kPadBack) input.buttons |= kPspSelect;
            if (b & kPadDpadUp) input.buttons |= kPspUp;
            if (b & kPadDpadDown) input.buttons |= kPspDown;
            if (b & kPadDpadLeft) input.buttons |= kPspLeft;
            if (b & kPadDpadRight) input.buttons |= kPspRight;
            // On foot the triggers aim and look behind, which is where San
            // Andreas puts them. In a vehicle they must not: R is the aim
            // button, and holding aim suppresses the throttle, so a trigger
            // that sent R while driving accelerated and immediately undid it.
            // That is why the brake worked from LT and the accelerator did not
            // from RT -- L is look-behind and blocks nothing.
            if (!driving) {
                if (pad.gamepad.left_trigger > 64u) input.buttons |= kPspLTrigger;
                if (pad.gamepad.right_trigger > 64u) input.buttons |= kPspRTrigger;
            }
            // ...and they drive, the way every GTA on a modern pad does.
            //
            // Through the vehicle accessors rather than by moving them onto the
            // pad's R and L: those two are aim and look-behind on foot, so a
            // trigger wired straight to them would aim every time the player
            // walked with a finger resting on it. Routed this way the trigger
            // accelerates in a car and still aims out of one, and it needs no
            // help from ModernControlScheme -- that option is about which pad
            // button the game itself reads, which is a different question.
            if (pad.gamepad.right_trigger > 64u) input.accelerate = true;
            if (pad.gamepad.left_trigger > 64u) input.brake = true;

            const std::uint8_t pad_x = stick_to_psp(pad.gamepad.lx, false);
            // PSP Y grows downwards, the stick's grows upwards.
            const std::uint8_t pad_y = stick_to_psp(pad.gamepad.ly, true);
            if (pad_x != 128u || pad_y != 128u) {
                input.analog_x = pad_x;
                input.analog_y = pad_y;
            }
            // Not inverted, unlike the left stick: the camera axis reads
            // positive as up, which is the direction the stick already gives.
            const int camera_x = stick_to_psp(pad.gamepad.rx, false) - 128;
            int camera_y = stick_to_psp(pad.gamepad.ry, false) - 128;
            if (controls.invert_camera_y) camera_y = -camera_y;
            if (camera_x != 0 || camera_y != 0) {
                input.camera_x = std::clamp(camera_x, -127, 127);
                input.camera_y = std::clamp(camera_y, -127, 127);
            }
        }
    }
    return publish();
}

bool display_window_close_requested() {
    if (!display_window_enabled()) return false;
    return window_state().close_requested.load(std::memory_order_relaxed);
}

// Keeps the last rendered frame on screen after the guest stops so the run can
// be inspected.  PSPRECOMP_WINDOW_HOLD=0 closes immediately instead.
void display_window_shutdown() {
    if (!display_window_enabled()) {
        dx12_presenter_shutdown();
        return;
    }
    WindowState &state = window_state();
    const HWND window = state.window.load(std::memory_order_acquire);
    if (window == nullptr) return;
    const char *hold = std::getenv("PSPRECOMP_WINDOW_HOLD");
    if (hold != nullptr && std::string(hold) == "0") {
        dx12_presenter_shutdown();
        PostMessageW(window, WM_CLOSE, 0, 0);
        return;
    }
    SetWindowTextW(window, L"VCSNative — stopped (close this window)");
    while (!state.close_requested.load(std::memory_order_relaxed) &&
           state.window.load(std::memory_order_acquire) != nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    dx12_presenter_shutdown();
}

} // namespace vcs

#else

namespace vcs {

bool display_window_enabled() { return false; }
void display_window_start() {}
void display_window_set_status(const char *) {}
void display_window_set_aspect_lock(bool) noexcept {}
void display_window_present(const psprecomp::GuestMemory &, const FramebufferDescription &) {}
void display_window_present_rgba(std::span<const std::byte>, std::uint32_t, std::uint32_t) {}
DisplayWindowSurface display_window_surface() { return {}; }
std::uint32_t display_window_buttons() { return 0u; }
void display_window_analog(std::uint8_t &x, std::uint8_t &y) { x = 128u; y = 128u; }
HostInputState display_window_input() { return {}; }
bool display_window_close_requested() { return false; }
void display_window_shutdown() {}

} // namespace vcs

#endif
