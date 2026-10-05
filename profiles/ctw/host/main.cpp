// CTW boot harness: loads the decrypted ELF, registers the generated corpus and
// runs from the module entry until the runtime stops (normally on the first PSP
// import that has no HLE implementation yet).

#include "ctw_frontend.hpp"
#include "ctw_overlays.hpp"
#include "psprecomp/common.hpp"
#include "psprecomp/elf32.hpp"
#include "psprecomp/hle/atrac.hpp"
#include "psprecomp/hle/audio.hpp"
#include "psprecomp/hle/ctrl.hpp"
#include "psprecomp/hle/display.hpp"
#include "psprecomp/hle/ge.hpp"
#include "psprecomp/hle/ge_renderer.hpp"
#if defined(CTW_HAVE_VULKAN)
#include "psprecomp/hle/ge_gpu_vulkan.hpp"
#endif
#include "psprecomp/hle/io.hpp"
#include "psprecomp/hle/kernel.hpp"
#include "psprecomp/hle/system.hpp"
#include "psprecomp/hle/utility.hpp"
#include "psprecomp/runtime.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <deque>
#include <iostream>
#include <string_view>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

// CTW_PERMISSIVE_HLE=1: every import without an implementation logs its first
// call and returns 0 instead of stopping, to map the boot-time import sequence.
void install_permissive_imports(psprecomp::Runtime &runtime, const std::vector<psprecomp::PspImport> &imports) {
    for (const auto &import : imports) {
        const std::string name =
            runtime.nids().resolve(import.library, import.nid).value_or(psprecomp::hex32(import.nid));
        runtime.register_hle(import.library, import.nid,
            [label = import.library + "::" + name, seen = false](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) mutable {
                if (!seen) {
                    std::cout << "[hle stub] " << label << " a0=" << psprecomp::hex32(ctx.gpr[4])
                              << " a1=" << psprecomp::hex32(ctx.gpr[5]) << " ra=" << psprecomp::hex32(ctx.gpr[31])
                              << "\n";
                    seen = true;
                }
                ctx.set_gpr(2, 0u);
            });
    }
}

// CTW_DUMP_VBLANKS="60,120,..." writes the displayed framebuffer at those
// vblanks to captures/ctw_vblank_<n>.ppm (no window exists on headless hosts).
std::vector<std::uint64_t> dump_vblanks() {
    std::vector<std::uint64_t> result;
    if (const char *text = std::getenv("CTW_DUMP_VBLANKS")) {
        for (const char *cursor = text; *cursor != '\0';) {
            char *end = nullptr;
            const unsigned long long value = std::strtoull(cursor, &end, 10);
            if (end == cursor) break;
            result.push_back(value);
            cursor = *end == ',' ? end + 1 : end;
        }
    }
    return result;
}

// GPU renderer (CTW_RENDERER=vulkan, the default when built with Vulkan).
bool gpu_rendering = false;
// The software rasterizer also keeps drawing into guest memory (CTW_GPU_SHADOW=1):
// slower, but anything the game reads back from VRAM stays correct.
bool gpu_shadow = false;
// Last vblank the GPU produced the displayed framebuffer.
std::uint64_t last_gpu_display_vblank = 0u;
// How long the last GPU frame stays on screen while the game draws nothing
// (loading and streaming hitches). Only after that is the guest's own
// framebuffer assumed to hold a CPU-drawn picture; with the GPU drawing, it
// is otherwise black, so a short grace showed as black flicker.
constexpr std::uint64_t kGpuDisplayGraceVblanks = 300u;
// Vblanks presented from guest memory after the GPU had started producing frames.
std::uint64_t gpu_fallback_vblanks = 0u;

// Vertex transform on the GPU (default; CTW_HW_TRANSFORM=0 keeps it on the CPU).
// Output is identical either way; this only moves work off the CPU.
bool ctw_hardware_transform() {
    const char *text = std::getenv("CTW_HW_TRANSFORM");
    return gpu_rendering && (text == nullptr || *text == '\0' || *text != '0');
}
bool ctw_gpu_color_authoritative() { return gpu_rendering && !gpu_shadow; }

// The image to present: the GPU's while it keeps producing the displayed
// framebuffer, otherwise (e.g. CPU-drawn frames) the guest's own.
bool current_gpu_image(ctw::GpuImage &image, std::uint64_t vblank, bool wait) {
#if defined(CTW_HAVE_VULKAN)
    if (!gpu_rendering || last_gpu_display_vblank == 0u || vblank > last_gpu_display_vblank + kGpuDisplayGraceVblanks)
        return false;
    const psprecomp::hle::vulkan::Frame frame = psprecomp::hle::vulkan::latest_frame(wait);
    if (frame.rgba.empty()) return false;
    image = ctw::GpuImage{frame.rgba.data(), frame.width, frame.height};
    return true;
#else
    (void)image;
    (void)vblank;
    (void)wait;
    return false;
#endif
}

void dump_gpu_image(const ctw::GpuImage &image, std::uint64_t vblank) {
    std::filesystem::create_directories("captures");
    const std::string path = "captures/ctw_vblank_" + std::to_string(vblank) + ".ppm";
    std::ofstream out(path, std::ios::binary);
    out << "P6\n" << image.width << ' ' << image.height << "\n255\n";
    for (std::size_t pixel = 0; pixel < static_cast<std::size_t>(image.width) * image.height; ++pixel)
        out.write(reinterpret_cast<const char *>(image.rgba) + pixel * 4u, 3);
    std::cout << "[capture] " << path << " (gpu " << image.width << "x" << image.height << ")\n";
}

void dump_framebuffer(psprecomp::Runtime &runtime, std::uint64_t vblank) {
    const auto &display = psprecomp::hle::display_state;
    const std::uint32_t bytes_per_pixel = display.pixel_format == 3u ? 4u : 2u;
    const std::uint32_t row_bytes = display.buffer_width * bytes_per_pixel;
    if (display.frame_buffer == 0u ||
        !runtime.memory().contains(display.frame_buffer, row_bytes * display.height)) return;
    std::filesystem::create_directories("captures");
    const std::string path = "captures/ctw_vblank_" + std::to_string(vblank) + ".ppm";
    std::ofstream out(path, std::ios::binary);
    out << "P6\n" << display.width << ' ' << display.height << "\n255\n";
    for (std::uint32_t y = 0; y < display.height; ++y) {
        for (std::uint32_t x = 0; x < display.width; ++x) {
            const std::uint32_t address = display.frame_buffer + y * row_bytes + x * bytes_per_pixel;
            std::uint8_t r = 0, g = 0, b = 0;
            if (bytes_per_pixel == 4u) {
                const std::uint32_t pixel = runtime.memory().load32(address);
                r = static_cast<std::uint8_t>(pixel);
                g = static_cast<std::uint8_t>(pixel >> 8u);
                b = static_cast<std::uint8_t>(pixel >> 16u);
            } else {
                const std::uint32_t pixel = runtime.memory().load16(address);
                switch (display.pixel_format) {
                case 0u:  // 565
                    r = static_cast<std::uint8_t>((pixel & 0x1Fu) << 3u);
                    g = static_cast<std::uint8_t>(((pixel >> 5u) & 0x3Fu) << 2u);
                    b = static_cast<std::uint8_t>(((pixel >> 11u) & 0x1Fu) << 3u);
                    break;
                case 1u:  // 5551
                    r = static_cast<std::uint8_t>((pixel & 0x1Fu) << 3u);
                    g = static_cast<std::uint8_t>(((pixel >> 5u) & 0x1Fu) << 3u);
                    b = static_cast<std::uint8_t>(((pixel >> 10u) & 0x1Fu) << 3u);
                    break;
                default:  // 4444
                    r = static_cast<std::uint8_t>((pixel & 0xFu) << 4u);
                    g = static_cast<std::uint8_t>(((pixel >> 4u) & 0xFu) << 4u);
                    b = static_cast<std::uint8_t>(((pixel >> 8u) & 0xFu) << 4u);
                    break;
                }
            }
            const char rgb[3] = {static_cast<char>(r), static_cast<char>(g), static_cast<char>(b)};
            out.write(rgb, 3);
        }
    }
    std::cout << "[capture] " << path << "\n";
}

bool ctw_on_vblank(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    static const std::vector<std::uint64_t> targets = dump_vblanks();
    const std::uint64_t vblank = psprecomp::hle::display_vblank_index;
#if defined(CTW_HAVE_VULKAN)
    if (gpu_rendering) {
        psprecomp::hle::vulkan::set_display_framebuffer(psprecomp::hle::display_state.frame_buffer);
        if (psprecomp::hle::vulkan::finish_frame(vblank)) last_gpu_display_vblank = vblank;
    }
#endif
    const bool dump = std::find(targets.begin(), targets.end(), vblank) != targets.end();
    ctw::GpuImage gpu_image{};
    const bool have_gpu_image = current_gpu_image(gpu_image, vblank, dump);
    if (gpu_rendering && last_gpu_display_vblank != 0u && !have_gpu_image) {
        if (gpu_fallback_vblanks++ < 20u || std::getenv("CTW_RENDER_LOG") != nullptr)
            std::cout << "[renderer] vblank " << vblank << " shown from guest memory (last GPU frame at "
                      << last_gpu_display_vblank << ")\n";
    }
    if (dump) {
        if (have_gpu_image) dump_gpu_image(gpu_image, vblank);
        else dump_framebuffer(runtime, vblank);
        // CTW_DUMP_VRAM=address,stride,width,height: also save that 32-bit
        // region of guest memory (render-to-texture diagnostics).
        if (const char *spec = std::getenv("CTW_DUMP_VRAM")) {
            unsigned address = 0, stride = 0, width = 0, height = 0;
            if (std::sscanf(spec, "%x,%u,%u,%u", &address, &stride, &width, &height) == 4 &&
                runtime.memory().contains(address, stride * height * 4u)) {
                const std::string path = "captures/ctw_vram_" + std::to_string(vblank) + ".ppm";
                std::ofstream out(path, std::ios::binary);
                out << "P6\n" << width << ' ' << height << "\n255\n";
                for (unsigned y = 0; y < height; ++y)
                    for (unsigned x = 0; x < width; ++x) {
                        const std::uint32_t pixel = runtime.memory().load32(address + (y * stride + x) * 4u);
                        const char rgb[3] = {static_cast<char>(pixel), static_cast<char>(pixel >> 8u),
                                             static_cast<char>(pixel >> 16u)};
                        out.write(rgb, 3);
                    }
                std::cout << "[capture] " << path << "\n";
            }
        }
    }
    // CTW_SPEED_REPORT=1: emulated frames per wall-clock second, every 300 vblanks
    // (60 means real-time speed for a 60 Hz PSP display).
    static const bool speed_report = std::getenv("CTW_SPEED_REPORT") != nullptr;
    if (speed_report && vblank % 300u == 0u) {
        static auto last = std::chrono::steady_clock::now();
        const auto now = std::chrono::steady_clock::now();
        const double seconds = std::chrono::duration<double>(now - last).count();
        last = now;
        std::cout << "[speed] vblank=" << vblank << " vblanks_per_second=" << (seconds > 0.0 ? 300.0 / seconds : 0.0)
                  << "\n" << std::flush;
    }
    // CTW_PRESENT_LOG=1: source and average brightness of every presented image.
    static const bool present_log = std::getenv("CTW_PRESENT_LOG") != nullptr;
    if (present_log && have_gpu_image) {
        std::uint64_t sum = 0u, count = 0u;
        for (std::size_t pixel = 0; pixel < static_cast<std::size_t>(gpu_image.width) * gpu_image.height; pixel += 97u) {
            sum += static_cast<std::uint8_t>(gpu_image.rgba[pixel * 4u + 1u]);
            ++count;
        }
        std::cout << "[present] vblank=" << vblank << " gpu brightness=" << (count ? sum / count : 0u) << " age=" << (vblank - last_gpu_display_vblank) << "\n";
    } else if (present_log) {
        std::cout << "[present] vblank=" << vblank << " guest\n";
    }
    if (!ctw::frontend_present(runtime.memory(), have_gpu_image ? &gpu_image : nullptr)) {
        ctx.set_gpr(2, 0u);
        runtime.stop("Window closed");
        return false;
    }
    return true;
}

// CTW_IMPORT_TRACE=N: keep the last N import calls (thread, name, arguments)
// and print them when the run stops.
struct ImportTraceEntry {
    std::uint64_t vblank;
    std::int32_t thread;
    std::uint32_t stub;
    std::uint32_t a0, a1, a2, a3, ra;
};
std::map<std::uint32_t, std::string> import_names;
std::deque<ImportTraceEntry> import_trace;
std::size_t import_trace_limit = 0;

// CTW_BREAK_NULL_READ=1: on the first sceIoRead into a null buffer, print the
// registers and a stack-scan backtrace (stack words that follow a JAL).
void print_stack_backtrace(psprecomp::Runtime &runtime, const psprecomp::AllegrexContext &ctx) {
    std::cout << "  [backtrace] pc=" << psprecomp::hex32(ctx.pc) << " ra=" << psprecomp::hex32(ctx.gpr[31])
              << " sp=" << psprecomp::hex32(ctx.gpr[29]) << "\n";
    for (int r = 4; r < 32; ++r)
        std::cout << (r % 8 == 4 ? "   " : "") << " r" << r << "=" << psprecomp::hex32(ctx.gpr[r])
                  << ((r % 8 == 3) ? "\n" : "");
    std::cout << "\n";
    for (std::uint32_t offset = 0; offset < 0x800u; offset += 4u) {
        const std::uint32_t address = ctx.gpr[29] + offset;
        if (!runtime.memory().contains(address, 4u)) break;
        const std::uint32_t value = runtime.memory().load32(address);
        if (value < 0x08804048u || value >= 0x08BC8000u || (value & 3u) != 0u) continue;
        if ((runtime.memory().load32(value - 8u) >> 26) != 3u) continue;  // previous-but-one word is a JAL
        std::cout << "    sp+" << psprecomp::hex32(offset) << " -> " << psprecomp::hex32(value) << "\n";
    }
}

void trace_import(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx, std::uint32_t dispatch_pc,
                  std::int32_t dispatch_thread_uid) {
    const auto name = import_names.find(dispatch_pc);
    if (name == import_names.end()) return;
    static const bool break_null_read = std::getenv("CTW_BREAK_NULL_READ") != nullptr;
    static bool reported = false;
    if (break_null_read && !reported && name->second.ends_with("sceIoRead") && ctx.gpr[5] == 0u) {
        reported = true;
        print_stack_backtrace(runtime, ctx);
    }
    // Skip the polling calls that otherwise flood the buffer.
    for (const char *noisy : {"DelayThread", "GetSystemTime", "SysClock2USec", "0x58B1F937", "0xD7763699",
                              "PowerTick"})
        if (name->second.find(noisy) != std::string::npos) return;
    // CTW_TRACE_SKIP="name,name": further import-name substrings to leave out.
    static const std::vector<std::string> extra_skip = [] {
        std::vector<std::string> skip;
        if (const char *text = std::getenv("CTW_TRACE_SKIP")) {
            std::string item;
            for (const char *c = text;; ++c) {
                if (*c == ',' || *c == '\0') { if (!item.empty()) skip.push_back(item); item.clear(); }
                else item.push_back(*c);
                if (*c == '\0') break;
            }
        }
        return skip;
    }();
    for (const std::string &noisy : extra_skip)
        if (name->second.find(noisy) != std::string::npos) return;
    import_trace.push_back({psprecomp::hle::display_vblank_index, dispatch_thread_uid, dispatch_pc,
                            ctx.gpr[4], ctx.gpr[5], ctx.gpr[6], ctx.gpr[7], ctx.gpr[31]});
    if (import_trace.size() > import_trace_limit) import_trace.pop_front();
}

std::uint64_t dispatch_cap() {
    if (const char *value = std::getenv("CTW_MAX_DISPATCHES")) return std::strtoull(value, nullptr, 0);
    return 50'000'000u;
}

}  // namespace

int main(int argc, char **argv) {
    try {
        const std::filesystem::path game_root = argc >= 2 ? argv[1] : "profiles/ctw/game";
        const std::filesystem::path executable = game_root / "PSP_GAME/SYSDIR/EBOOT_DECRYPTED.ELF";

        const bool windowed = ctw::frontend_start();
#if defined(CTW_HAVE_VULKAN)
        // CTW_RENDERER=software|vulkan, CTW_RENDER_SCALE=N (internal resolution
        // 480x272 * N, default 2), CTW_GPU_SHADOW=1, CTW_HW_TRANSFORM=0.
        {
            const char *renderer = std::getenv("CTW_RENDERER");
            if (renderer == nullptr || std::string_view(renderer) != "software") {
                psprecomp::hle::vulkan::Config config;
                if (const char *scale = std::getenv("CTW_RENDER_SCALE")) config.scale = static_cast<std::uint32_t>(std::atoi(scale));
                config.validation = std::getenv("CTW_VK_VALIDATION") != nullptr;
                std::string error;
                gpu_rendering = psprecomp::hle::vulkan::initialize(config, error);
                gpu_shadow = std::getenv("CTW_GPU_SHADOW") != nullptr;
                if (gpu_rendering)
                    std::cout << "[renderer] vulkan " << psprecomp::hle::vulkan::device_name() << " "
                              << psprecomp::hle::vulkan::report().offscreen_width << "x"
                              << psprecomp::hle::vulkan::report().offscreen_height
                              << (gpu_shadow ? " (software shadow)" : "")
                              << (ctw_hardware_transform() ? " hw-transform" : "") << "\n";
                else
                    std::cout << "[renderer] vulkan unavailable (" << error << "); software\n";
            }
        }
#endif
        psprecomp::hle::g_ge_renderer_hooks = psprecomp::hle::GeRendererHooks{
            &ctw_hardware_transform, &ctw_gpu_color_authoritative, nullptr, &ctw_hardware_transform};
        psprecomp::Runtime runtime;
        runtime.set_game_root(game_root);
        const auto elf = psprecomp::Elf32Image::from_file(executable);
        (void)elf.load_and_relocate(runtime.memory(), psprecomp::kDefaultPspUserLoadBase);
        std::uint64_t image_end = 0u;
        for (std::size_t index = 0; index < elf.segments().size(); ++index) {
            const auto &segment = elf.segments()[index];
            if (segment.type != 1u) continue;  // PT_LOAD
            const std::uint64_t start = elf.segment_runtime_address(index, psprecomp::kDefaultPspUserLoadBase);
            image_end = std::max(image_end, start + segment.memory_size);
        }
        const auto user_arena_start = static_cast<std::uint32_t>((image_end + 0xFFu) & ~0xFFull);
        psprecomp::register_generated_functions(runtime);
        ctw::install_overlay_manager(runtime);
        runtime.nids().load_csv("configs/nids.csv");

        const auto module = elf.find_module_info(runtime.memory(), psprecomp::kDefaultPspUserLoadBase);
        if (!module) throw psprecomp::Error("PSP module info not found after relocation");
        // Permissive stubs go in first so the real kernel HLE replaces them.
        if (std::getenv("CTW_PERMISSIVE_HLE") != nullptr)
            install_permissive_imports(runtime, elf.scan_imports(runtime.memory(), *module));
        if (const char *trace = std::getenv("CTW_IMPORT_TRACE"); trace != nullptr || std::getenv("CTW_BREAK_NULL_READ")) {
            import_trace_limit = trace != nullptr ? std::strtoull(trace, nullptr, 0) : 0u;
            for (const auto &import : elf.scan_imports(runtime.memory(), *module))
                import_names[import.stub_address] = import.library + "::" +
                    runtime.nids().resolve(import.library, import.nid).value_or(psprecomp::hex32(import.nid));
            psprecomp::set_runtime_pre_dispatch_hook(&trace_import);
        }
        // reset_kernel() creates the module_start thread and points $sp at its stack.
        psprecomp::hle::reset_kernel(runtime, user_arena_start);
        // Kernel behaviours CTW relies on (all off by default, see kernel.hpp / display.hpp).
        // CTW's thread loops poll with sceKernelDelayThread(0) and rely on it
        // letting the lower-priority SysManager/loader threads run.
        psprecomp::hle::minimum_thread_delay_us = 200u;
        // SysManager registers power/UMD/memory-stick callbacks and waits for them.
        psprecomp::hle::notify_device_callbacks = true;
        // The resource manager relies on a 15 ms WaitSema timeout to mark itself idle.
        psprecomp::hle::honor_wait_timeouts = true;
        // The game sizes its main heap from sceKernelMaxFreeMemSize after its
        // bootstrap threads exit-delete themselves.
        psprecomp::hle::release_deleted_thread_stacks = true;
        // Its vblank interrupt handler wakes worker threads (e.g. the memstick
        // thread) even while no thread waits for vblank.
        psprecomp::hle::periodic_vblank_interrupts = true;
        psprecomp::hle::install_kernel_hle(runtime);
        psprecomp::hle::reset_io();
        psprecomp::hle::install_io_hle(runtime);
        psprecomp::hle::install_system_hle(runtime);
        psprecomp::hle::reset_display(psprecomp::hle::DisplayHooks{nullptr, nullptr, &ctw_on_vblank});
        psprecomp::hle::install_display_hle(runtime);
        // Shared software rasterizer; no GPU backend (psprecomp_hle_gpu_null).
        psprecomp::hle::reset_ge(psprecomp::hle::GeRenderer{&psprecomp::hle::render_ge_primitive,
                                                            &psprecomp::hle::test_ge_bounding_box});
        psprecomp::hle::install_ge_hle(runtime);
        psprecomp::hle::reset_audio(ctw::frontend_audio_hooks());  // silent when headless; paced on PSP time
        psprecomp::hle::install_audio_hle(runtime);
        psprecomp::hle::reset_atrac();  // music: the streamed AT3 file is decoded with FFmpeg
        psprecomp::hle::install_atrac_hle(runtime);
        psprecomp::hle::reset_ctrl(ctw::frontend_ctrl_hooks());  // plus PSPRECOMP_CTRL_PULSE* scripted presses
        psprecomp::hle::install_ctrl_hle(runtime);
        psprecomp::hle::reset_utility();  // saves under <game>/PSP/SAVEDATA
        psprecomp::hle::install_utility_hle(runtime);
        // Guest threads that spin without kernel calls still have to see time pass.
        const char *tick = std::getenv("PSPRECOMP_TIME_TICK_DISPATCHES");
        psprecomp::hle::install_execution_clock(tick != nullptr ? std::strtoull(tick, nullptr, 0) : 256u);
        auto &cpu = runtime.cpu();
        cpu.set_gpr(28, module->gp);
        cpu.set_gpr(31, 0u);
        cpu.set_gpr(4, 0u);
        cpu.set_gpr(5, 0u);

        const std::uint32_t entry = elf.runtime_entry();
        std::cout << "CTW boot\n"
                  << "  executable: " << executable.string() << "\n"
                  << "  entry:      " << psprecomp::hex32(entry) << "\n"
                  << "  gp:         " << psprecomp::hex32(module->gp) << "\n"
                  << "  functions:  " << runtime.function_count() << "\n";

        std::string fault;
        try {
            runtime.run(entry, dispatch_cap());
        } catch (const std::exception &error) {
            fault = error.what();  // reported below, after the usual diagnostics
        }

        std::cout << "Stopped at pc " << psprecomp::hex32(cpu.pc) << ": "
                  << (runtime.stopped() ? runtime.stop_reason() : std::string("dispatch cap reached")) << "\n"
                  << "  vblanks:    " << psprecomp::hle::display_vblank_index << "\n"
                  << "  guest time: " << psprecomp::hle::virtual_time_us / 1000u << " ms\n";
        if (const char *dump = std::getenv("CTW_DUMP_RANGE")) {  // "start,end,path"
            std::uint32_t start = 0, end = 0;
            char path[256]{};
            if (std::sscanf(dump, "%x,%x,%255s", &start, &end, path) == 3 && end > start &&
                runtime.memory().contains(start, end - start)) {
                std::vector<std::uint8_t> bytes(end - start);
                runtime.memory().copy_out(start, bytes);
                std::ofstream(path, std::ios::binary).write(reinterpret_cast<const char *>(bytes.data()),
                                                            static_cast<std::streamsize>(bytes.size()));
                std::cout << "  dumped " << psprecomp::hex32(start) << "-" << psprecomp::hex32(end) << " to " << path << "\n";
            }
        }
        if (runtime.memory().contains(cpu.pc, 16u)) {
            std::cout << "  code at stop pc:";
            for (std::uint32_t i = 0; i < 4u; ++i) std::cout << " " << psprecomp::hex32(runtime.memory().load32(cpu.pc + i * 4u));
            std::cout << "\n";
        }
        if (std::getenv("PSPRECOMP_HLE_HISTOGRAM") != nullptr) runtime.report_hle_histogram(40u);
        for (const auto &entry : import_trace) {
            std::cout << "  [import] vblank=" << entry.vblank << " thread=" << entry.thread << " "
                      << import_names[entry.stub] << " a0=" << psprecomp::hex32(entry.a0)
                      << " a1=" << psprecomp::hex32(entry.a1) << " a2=" << psprecomp::hex32(entry.a2)
                      << " a3=" << psprecomp::hex32(entry.a3)
                      << " ra=" << psprecomp::hex32(entry.ra) << "\n";
        }
        for (const auto &[uid, sema] : psprecomp::hle::semaphore_table.semaphores) {
            std::cout << "  sema " << psprecomp::hex32(static_cast<std::uint32_t>(uid)) << " \"" << sema.name
                      << "\" count=" << sema.count << "/" << sema.maximum << " waiters=";
            for (const auto &waiter : sema.waiters) std::cout << waiter.uid << " ";
            std::cout << "\n";
        }
        std::vector<std::int32_t> uids;
        for (const auto &[uid, thread] : psprecomp::hle::thread_table.threads) uids.push_back(uid);
        std::sort(uids.begin(), uids.end());
        for (const std::int32_t uid : uids) {
            const auto &thread = psprecomp::hle::thread_table.threads.at(uid);
            std::cout << "  thread " << uid << " " << thread.name << " prio=" << thread.priority << " "
                      << psprecomp::hle::thread_state_name(thread.state)
                      << (uid == psprecomp::hle::thread_table.current_uid ? " (current)" : "")
                      << " resume=" << psprecomp::hex32(thread.suspended_context.pc)
                      << " ra=" << psprecomp::hex32(thread.suspended_context.gpr[31]) << "\n";
        }
        ctw::frontend_shutdown();
#if defined(CTW_HAVE_VULKAN)
        if (gpu_rendering) {
            const auto &report = psprecomp::hle::vulkan::report();
            std::cout << "[renderer] frames=" << report.game_frames << " draws=" << report.draw_calls
                      << " gpu_draws=" << report.game_draw_calls << " hw_transform_draws=" << report.hw_transform_draw_calls
                      << " textures=" << report.texture_images_created << " uploads=" << report.decoded_texture_uploads
                      << " evicted=" << report.evicted_textures << " targets=" << report.framebuffer_targets_observed
                      << " pipelines=" << report.unique_pipeline_keys << " overflows=" << report.game_vertex_overflows
                      << " frames_without_display=" << report.frames_without_displayed_target
                      << " guest_memory_fallback_vblanks=" << gpu_fallback_vblanks << "\n";
        }
        psprecomp::hle::vulkan::shutdown();
#endif
        (void)windowed;
        if (!fault.empty()) {
            std::cerr << "ctw_boot: " << fault << '\n';
            return 1;
        }
        return runtime.stopped() ? 2 : 0;
    } catch (const std::exception &error) {
        std::cerr << "ctw_boot: " << error.what() << '\n';
        return 1;
    }
}
