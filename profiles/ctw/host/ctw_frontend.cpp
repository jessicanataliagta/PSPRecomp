#include "ctw_frontend.hpp"

#include "psprecomp/hle/display.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#if defined(CTW_HAVE_SDL3)
#include <SDL3/SDL.h>
#endif

namespace ctw {
namespace {

constexpr std::uint32_t kPspWidth = 480u;
constexpr std::uint32_t kPspHeight = 272u;

// PSP_CTRL_* button bits.
constexpr std::uint32_t kSelect = 0x0001u, kStart = 0x0008u, kUp = 0x0010u, kRight = 0x0020u,
                        kDown = 0x0040u, kLeft = 0x0080u, kLTrigger = 0x0100u, kRTrigger = 0x0200u,
                        kTriangle = 0x1000u, kCircle = 0x2000u, kCross = 0x4000u, kSquare = 0x8000u;

// Converts the displayed PSP framebuffer to RGBA bytes (R, G, B, A in memory).
void convert_framebuffer(const psprecomp::GuestMemory &memory, std::vector<std::uint8_t> &rgba) {
    const auto &display = psprecomp::hle::display_state;
    const std::uint32_t width = std::min(display.width, kPspWidth);
    const std::uint32_t height = std::min(display.height, kPspHeight);
    rgba.assign(static_cast<std::size_t>(kPspWidth) * kPspHeight * 4u, 0u);
    const std::uint32_t bytes_per_pixel = display.pixel_format == 3u ? 4u : 2u;
    const std::uint32_t row_bytes = display.buffer_width * bytes_per_pixel;
    if (display.frame_buffer == 0u || !memory.contains(display.frame_buffer, row_bytes * height)) return;
    for (std::uint32_t y = 0; y < height; ++y) {
        const std::uint8_t *row = memory.raw_pointer(display.frame_buffer + y * row_bytes, width * bytes_per_pixel);
        if (row == nullptr) continue;
        std::uint8_t *out = rgba.data() + static_cast<std::size_t>(y) * kPspWidth * 4u;
        for (std::uint32_t x = 0; x < width; ++x, out += 4) {
            std::uint32_t r, g, b;
            if (bytes_per_pixel == 4u) {
                r = row[x * 4u]; g = row[x * 4u + 1u]; b = row[x * 4u + 2u];
            } else {
                const std::uint32_t pixel = static_cast<std::uint32_t>(row[x * 2u]) | (row[x * 2u + 1u] << 8u);
                switch (display.pixel_format) {
                case 0u:  // 565
                    r = (pixel & 0x1Fu) << 3u; g = ((pixel >> 5u) & 0x3Fu) << 2u; b = ((pixel >> 11u) & 0x1Fu) << 3u;
                    break;
                case 1u:  // 5551
                    r = (pixel & 0x1Fu) << 3u; g = ((pixel >> 5u) & 0x1Fu) << 3u; b = ((pixel >> 10u) & 0x1Fu) << 3u;
                    break;
                default:  // 4444
                    r = (pixel & 0xFu) << 4u; g = ((pixel >> 4u) & 0xFu) << 4u; b = ((pixel >> 8u) & 0xFu) << 4u;
                    break;
                }
            }
            out[0] = static_cast<std::uint8_t>(r);
            out[1] = static_cast<std::uint8_t>(g);
            out[2] = static_cast<std::uint8_t>(b);
            out[3] = 0xFFu;
        }
    }
}

// CTW_AUDIO_DUMP=<file.wav>: every submitted channel mixed onto one 44.1 kHz
// stereo timeline by guest time and written at shutdown, so audio can be
// checked on a machine without a sound device (and without SDL).
struct AudioDump {
    static constexpr std::uint32_t kRate = 44100u;
    static constexpr std::size_t kMaxFrames = static_cast<std::size_t>(kRate) * 60u * 20u;
    std::string path;
    std::vector<std::int32_t> mix;  // interleaved L/R
};

AudioDump &audio_dump() {
    static AudioDump dump = [] {
        AudioDump value;
        if (const char *path = std::getenv("CTW_AUDIO_DUMP")) value.path = path;
        return value;
    }();
    return dump;
}

void audio_dump_submit(std::span<const std::int16_t> pcm, std::uint32_t frames, bool stereo, std::uint32_t left,
                       std::uint32_t right, std::uint32_t source_rate, std::uint64_t guest_time_us) {
    AudioDump &dump = audio_dump();
    const std::uint32_t rate = source_rate == 0u ? AudioDump::kRate : source_rate;
    const std::size_t first = static_cast<std::size_t>(guest_time_us * AudioDump::kRate / 1000000u);
    const std::size_t count = static_cast<std::size_t>(frames) * AudioDump::kRate / rate;
    if (first + count > AudioDump::kMaxFrames) return;
    if (dump.mix.size() < (first + count) * 2u) dump.mix.resize((first + count) * 2u, 0);
    for (std::size_t frame = 0; frame < count; ++frame) {
        const std::size_t source_frame = frame * rate / AudioDump::kRate;
        const std::size_t source = stereo ? source_frame * 2u : source_frame;
        if (source + (stereo ? 1u : 0u) >= pcm.size()) break;
        dump.mix[(first + frame) * 2u] += (pcm[source] * static_cast<std::int32_t>(std::min(left, 0x8000u))) >> 15;
        dump.mix[(first + frame) * 2u + 1u] +=
            (pcm[stereo ? source + 1u : source] * static_cast<std::int32_t>(std::min(right, 0x8000u))) >> 15;
    }
}

void write_audio_dump() {
    AudioDump &dump = audio_dump();
    if (dump.path.empty()) return;
    std::ofstream out(dump.path, std::ios::binary);
    const auto u32 = [&out](std::uint32_t value) { out.write(reinterpret_cast<const char *>(&value), 4); };
    const auto u16 = [&out](std::uint16_t value) { out.write(reinterpret_cast<const char *>(&value), 2); };
    const std::uint32_t data_bytes = static_cast<std::uint32_t>(dump.mix.size() * 2u);
    out.write("RIFF", 4); u32(36u + data_bytes); out.write("WAVEfmt ", 8);
    u32(16u); u16(1u); u16(2u); u32(AudioDump::kRate); u32(AudioDump::kRate * 4u); u16(4u); u16(16u);
    out.write("data", 4); u32(data_bytes);
    for (const std::int32_t sample : dump.mix) {
        const auto clipped = static_cast<std::int16_t>(std::clamp(sample, -32768, 32767));
        out.write(reinterpret_cast<const char *>(&clipped), 2);
    }
    std::cout << "[frontend] audio dump: " << dump.path << " (" << dump.mix.size() / 2u / AudioDump::kRate << " s)\n";
}

#if defined(CTW_HAVE_SDL3)

struct AudioChannel {
    SDL_AudioStream *stream{};
    std::uint32_t rate{};
};

struct Frontend {
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
    SDL_Texture *gpu_texture{};  // sized to the GPU backend's internal resolution
    std::uint32_t gpu_width{};
    std::uint32_t gpu_height{};
    SDL_ScaleMode scale_mode{SDL_SCALEMODE_LINEAR};
    SDL_Gamepad *gamepad{};
    SDL_AudioDeviceID audio_device{};
    SDL_AudioSpec device_spec{};
    std::array<AudioChannel, 9> channels{};
    std::vector<std::uint8_t> pixels;
    std::chrono::steady_clock::time_point next_frame{};
    bool limit_frame_rate{true};
    bool active{};
};

Frontend frontend;

void open_first_gamepad() {
    if (frontend.gamepad != nullptr) return;
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    if (ids != nullptr && count > 0) {
        frontend.gamepad = SDL_OpenGamepad(ids[0]);
        if (frontend.gamepad != nullptr)
            std::cout << "[frontend] gamepad: " << SDL_GetGamepadName(frontend.gamepad) << "\n";
    }
    SDL_free(ids);
}

std::uint32_t host_buttons() {
    if (!frontend.active) return 0u;
    std::uint32_t buttons = 0u;
    const bool *keys = SDL_GetKeyboardState(nullptr);
    const auto key = [keys](SDL_Scancode code) { return keys != nullptr && keys[code]; };
    // Arrow keys are the D-pad; WASD is the analog stick (host_analog).
    if (key(SDL_SCANCODE_UP)) buttons |= kUp;
    if (key(SDL_SCANCODE_DOWN)) buttons |= kDown;
    if (key(SDL_SCANCODE_LEFT)) buttons |= kLeft;
    if (key(SDL_SCANCODE_RIGHT)) buttons |= kRight;
    if (key(SDL_SCANCODE_K) || key(SDL_SCANCODE_SPACE)) buttons |= kCross;
    if (key(SDL_SCANCODE_L)) buttons |= kCircle;
    if (key(SDL_SCANCODE_J)) buttons |= kSquare;
    if (key(SDL_SCANCODE_I)) buttons |= kTriangle;
    if (key(SDL_SCANCODE_Q)) buttons |= kLTrigger;
    if (key(SDL_SCANCODE_E)) buttons |= kRTrigger;
    if (key(SDL_SCANCODE_RETURN)) buttons |= kStart;
    if (key(SDL_SCANCODE_BACKSPACE)) buttons |= kSelect;
    if (SDL_Gamepad *pad = frontend.gamepad) {
        const auto button = [pad](SDL_GamepadButton b) { return SDL_GetGamepadButton(pad, b); };
        // Positional mapping: the PSP's Cross sits where the Deck's A does.
        if (button(SDL_GAMEPAD_BUTTON_SOUTH)) buttons |= kCross;
        if (button(SDL_GAMEPAD_BUTTON_EAST)) buttons |= kCircle;
        if (button(SDL_GAMEPAD_BUTTON_WEST)) buttons |= kSquare;
        if (button(SDL_GAMEPAD_BUTTON_NORTH)) buttons |= kTriangle;
        if (button(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER) ||
            SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 16000) buttons |= kLTrigger;
        if (button(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) ||
            SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 16000) buttons |= kRTrigger;
        if (button(SDL_GAMEPAD_BUTTON_START)) buttons |= kStart;
        if (button(SDL_GAMEPAD_BUTTON_BACK)) buttons |= kSelect;
        if (button(SDL_GAMEPAD_BUTTON_DPAD_UP)) buttons |= kUp;
        if (button(SDL_GAMEPAD_BUTTON_DPAD_DOWN)) buttons |= kDown;
        if (button(SDL_GAMEPAD_BUTTON_DPAD_LEFT)) buttons |= kLeft;
        if (button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) buttons |= kRight;
    }
    return buttons;
}

void host_analog(std::uint8_t &x, std::uint8_t &y) {
    if (!frontend.active) return;
    // WASD pushes the stick fully; held Shift halves it (walk instead of run).
    if (const bool *keys = SDL_GetKeyboardState(nullptr)) {
        const int dx = (keys[SDL_SCANCODE_D] ? 1 : 0) - (keys[SDL_SCANCODE_A] ? 1 : 0);
        const int dy = (keys[SDL_SCANCODE_S] ? 1 : 0) - (keys[SDL_SCANCODE_W] ? 1 : 0);
        if (dx != 0 || dy != 0) {
            const int reach = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT] ? 64 : 127;
            x = static_cast<std::uint8_t>(128 + dx * reach);
            y = static_cast<std::uint8_t>(128 + dy * reach);
            return;
        }
    }
    if (frontend.gamepad == nullptr) return;
    const auto to_psp = [](Sint16 value) {
        // 8000/32767 dead zone around the centre, then 0..255 with 128 centred.
        if (value > -6000 && value < 6000) return static_cast<std::uint8_t>(128u);
        return static_cast<std::uint8_t>(std::clamp((static_cast<int>(value) + 32768) >> 8, 0, 255));
    };
    x = to_psp(SDL_GetGamepadAxis(frontend.gamepad, SDL_GAMEPAD_AXIS_LEFTX));
    y = to_psp(SDL_GetGamepadAxis(frontend.gamepad, SDL_GAMEPAD_AXIS_LEFTY));
}

bool audio_enabled() { return frontend.active && frontend.audio_device != 0u; }

void audio_submit(std::span<const std::int16_t> pcm, std::uint32_t frames, bool stereo, std::uint32_t left,
                  std::uint32_t right, std::uint32_t source_rate, std::uint32_t channel, std::uint64_t) {
    if (!audio_enabled() || channel >= frontend.channels.size() || frames == 0u) return;
    AudioChannel &output = frontend.channels[channel];
    const std::uint32_t rate = source_rate == 0u ? 44100u : source_rate;
    if (output.stream == nullptr || output.rate != rate) {
        if (output.stream != nullptr) SDL_DestroyAudioStream(output.stream);
        const SDL_AudioSpec source{SDL_AUDIO_S16, 2, static_cast<int>(rate)};
        output.stream = SDL_CreateAudioStream(&source, &frontend.device_spec);
        output.rate = rate;
        if (output.stream == nullptr || !SDL_BindAudioStream(frontend.audio_device, output.stream)) return;
    }
    // Keep latency bounded (about 250 ms of 16-bit stereo) while the game runs
    // slower than real time.
    if (SDL_GetAudioStreamQueued(output.stream) > static_cast<int>(rate)) return;
    std::vector<std::int16_t> samples(static_cast<std::size_t>(frames) * 2u);
    const auto scale = [](std::int16_t value, std::uint32_t volume) {
        return static_cast<std::int16_t>(std::clamp((static_cast<int>(value) * static_cast<int>(std::min(volume, 0x8000u))) >> 15,
                                                    -32768, 32767));
    };
    for (std::uint32_t frame = 0; frame < frames; ++frame) {
        const std::size_t source = stereo ? frame * 2u : frame;
        if (source + (stereo ? 1u : 0u) >= pcm.size()) break;
        samples[frame * 2u] = scale(pcm[source], left);
        samples[frame * 2u + 1u] = scale(pcm[stereo ? source + 1u : source], right);
    }
    SDL_PutAudioStreamData(output.stream, samples.data(), static_cast<int>(samples.size() * sizeof(std::int16_t)));
}

void audio_reset_channel(std::uint32_t channel) {
    if (channel < frontend.channels.size() && frontend.channels[channel].stream != nullptr)
        SDL_ClearAudioStream(frontend.channels[channel].stream);
}

#endif  // CTW_HAVE_SDL3

} // namespace

bool frontend_start() {
#if defined(CTW_HAVE_SDL3)
    if (std::getenv("CTW_HEADLESS") != nullptr) return false;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
        std::cerr << "[frontend] SDL unavailable (" << SDL_GetError() << "); running headless\n";
        return false;
    }
    const char *scale_text = std::getenv("CTW_SCALE");
    const int scale = std::clamp(scale_text != nullptr ? std::atoi(scale_text) : 2, 1, 8);
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
    if (std::getenv("CTW_FULLSCREEN") != nullptr) flags |= SDL_WINDOW_FULLSCREEN;
    if (!SDL_CreateWindowAndRenderer("Grand Theft Auto: Chinatown Wars (PSPRecomp)", static_cast<int>(kPspWidth) * scale,
                                     static_cast<int>(kPspHeight) * scale, flags, &frontend.window, &frontend.renderer)) {
        std::cerr << "[frontend] no window (" << SDL_GetError() << "); running headless\n";
        SDL_Quit();
        return false;
    }
    SDL_SetRenderVSync(frontend.renderer, 0);
    SDL_SetRenderLogicalPresentation(frontend.renderer, kPspWidth, kPspHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    frontend.texture = SDL_CreateTexture(frontend.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                         kPspWidth, kPspHeight);
    SDL_SetTextureBlendMode(frontend.texture, SDL_BLENDMODE_NONE);
    const char *filter = std::getenv("CTW_FILTER");
    frontend.scale_mode = filter != nullptr && std::strcmp(filter, "nearest") == 0 ? SDL_SCALEMODE_NEAREST
                                                                                   : SDL_SCALEMODE_LINEAR;
    SDL_SetTextureScaleMode(frontend.texture, frontend.scale_mode);
    frontend.device_spec = SDL_AudioSpec{SDL_AUDIO_S16, 2, 44100};
    frontend.audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &frontend.device_spec);
    if (frontend.audio_device == 0u) std::cerr << "[frontend] no audio device (" << SDL_GetError() << ")\n";
    frontend.limit_frame_rate = std::getenv("CTW_UNLIMITED") == nullptr;
    open_first_gamepad();
    frontend.active = true;
    std::cout << "[frontend] SDL " << SDL_GetVersion() / 1000000 << "." << SDL_GetVersion() / 1000 % 1000
              << " video=" << SDL_GetCurrentVideoDriver() << " renderer=" << SDL_GetRendererName(frontend.renderer)
              << " audio=" << (frontend.audio_device != 0u ? SDL_GetCurrentAudioDriver() : "none") << "\n";
    return true;
#else
    return false;
#endif
}

void frontend_shutdown() {
    write_audio_dump();
#if defined(CTW_HAVE_SDL3)
    if (!frontend.active) return;
    for (AudioChannel &channel : frontend.channels)
        if (channel.stream != nullptr) SDL_DestroyAudioStream(channel.stream);
    if (frontend.audio_device != 0u) SDL_CloseAudioDevice(frontend.audio_device);
    if (frontend.gamepad != nullptr) SDL_CloseGamepad(frontend.gamepad);
    SDL_DestroyTexture(frontend.texture);
    if (frontend.gpu_texture != nullptr) SDL_DestroyTexture(frontend.gpu_texture);
    SDL_DestroyRenderer(frontend.renderer);
    SDL_DestroyWindow(frontend.window);
    SDL_Quit();
    frontend = Frontend{};
#endif
}

bool frontend_active() {
#if defined(CTW_HAVE_SDL3)
    return frontend.active;
#else
    return false;
#endif
}

bool frontend_present(const psprecomp::GuestMemory &memory, const GpuImage *gpu) {
#if defined(CTW_HAVE_SDL3)
    if (!frontend.active) return true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) return false;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_F11)
            SDL_SetWindowFullscreen(frontend.window, (SDL_GetWindowFlags(frontend.window) & SDL_WINDOW_FULLSCREEN) == 0);
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) open_first_gamepad();
        if (event.type == SDL_EVENT_GAMEPAD_REMOVED && frontend.gamepad != nullptr &&
            SDL_GetGamepadID(frontend.gamepad) == event.gdevice.which) {
            SDL_CloseGamepad(frontend.gamepad);
            frontend.gamepad = nullptr;
            open_first_gamepad();
        }
    }
    SDL_Texture *shown = frontend.texture;
    if (gpu != nullptr && gpu->rgba != nullptr && gpu->width != 0u && gpu->height != 0u) {
        if (frontend.gpu_texture == nullptr || frontend.gpu_width != gpu->width || frontend.gpu_height != gpu->height) {
            if (frontend.gpu_texture != nullptr) SDL_DestroyTexture(frontend.gpu_texture);
            frontend.gpu_texture = SDL_CreateTexture(frontend.renderer, SDL_PIXELFORMAT_RGBA32,
                                                     SDL_TEXTUREACCESS_STREAMING, static_cast<int>(gpu->width),
                                                     static_cast<int>(gpu->height));
            frontend.gpu_width = gpu->width;
            frontend.gpu_height = gpu->height;
            if (frontend.gpu_texture != nullptr) {
                SDL_SetTextureScaleMode(frontend.gpu_texture, frontend.scale_mode);
                // The image's alpha is the PSP framebuffer's alpha/stencil byte,
                // not coverage: blending by it blacks out most 3D scenes.
                SDL_SetTextureBlendMode(frontend.gpu_texture, SDL_BLENDMODE_NONE);
            }
        }
        if (frontend.gpu_texture != nullptr) {
            SDL_UpdateTexture(frontend.gpu_texture, nullptr, gpu->rgba, static_cast<int>(gpu->width * 4u));
            shown = frontend.gpu_texture;
        }
    }
    if (shown == frontend.texture) {
        convert_framebuffer(memory, frontend.pixels);
        SDL_UpdateTexture(frontend.texture, nullptr, frontend.pixels.data(), static_cast<int>(kPspWidth * 4u));
    }
    SDL_SetRenderDrawColor(frontend.renderer, 0, 0, 0, 255);
    SDL_RenderClear(frontend.renderer);
    SDL_RenderTexture(frontend.renderer, shown, nullptr, nullptr);
    // CTW_WINDOW_DUMP=n,file.bmp: save what the window shows at the n-th
    // present (after SDL's compositing, unlike CTW_DUMP_VBLANKS).
    static const char *window_dump = std::getenv("CTW_WINDOW_DUMP");
    if (window_dump != nullptr) {
        static std::uint64_t presents = 0u;
        char *end = nullptr;
        const std::uint64_t target = std::strtoull(window_dump, &end, 10);
        if (++presents == target && end != nullptr && *end == ',') {
            if (SDL_Surface *surface = SDL_RenderReadPixels(frontend.renderer, nullptr)) {
                SDL_SaveBMP(surface, end + 1);
                SDL_DestroySurface(surface);
                std::cout << "[frontend] window dump: " << (end + 1) << "\n";
            }
        }
    }
    SDL_RenderPresent(frontend.renderer);
    // Never run faster than the PSP's display: one vblank of wall time per vblank.
    if (frontend.limit_frame_rate) {
        const auto period = std::chrono::microseconds(psprecomp::hle::display_vblank_period_us());
        const auto now = std::chrono::steady_clock::now();
        if (frontend.next_frame < now - period * 4) frontend.next_frame = now;  // running behind: don't catch up
        frontend.next_frame += period;
        if (frontend.next_frame > now) std::this_thread::sleep_until(frontend.next_frame);
    }
    return true;
#else
    (void)memory;
    (void)gpu;
    return true;
#endif
}

psprecomp::hle::CtrlHooks frontend_ctrl_hooks() {
#if defined(CTW_HAVE_SDL3)
    return psprecomp::hle::CtrlHooks{&host_buttons, &host_analog};
#else
    return {};
#endif
}

psprecomp::hle::AudioHooks frontend_audio_hooks() {
    const auto enabled = []() {
#if defined(CTW_HAVE_SDL3)
        if (audio_enabled()) return true;
#endif
        return !audio_dump().path.empty();
    };
    const auto submit = [](std::span<const std::int16_t> pcm, std::uint32_t frames, bool stereo, std::uint32_t left,
                           std::uint32_t right, std::uint32_t source_rate, std::uint32_t channel,
                           std::uint64_t guest_time_us) {
        if (!audio_dump().path.empty())
            audio_dump_submit(pcm, frames, stereo, left, right, source_rate, guest_time_us);
#if defined(CTW_HAVE_SDL3)
        audio_submit(pcm, frames, stereo, left, right, source_rate, channel, guest_time_us);
#else
        (void)channel;
#endif
    };
    const auto reset_channel = [](std::uint32_t channel) {
#if defined(CTW_HAVE_SDL3)
        audio_reset_channel(channel);
#else
        (void)channel;
#endif
    };
    return psprecomp::hle::AudioHooks{+enabled, +submit, +reset_channel};
}

} // namespace ctw
