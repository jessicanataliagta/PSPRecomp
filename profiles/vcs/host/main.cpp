#include "psprecomp/common.hpp"
#include "psprecomp/elf32.hpp"
#include "psprecomp/runtime.hpp"
#include "psprecomp/sha256.hpp"
#include "audio_output.hpp"
#include "display_window.hpp"
#include "ge_gpu_backend.hpp"
#include "vcs_profile.hpp"
#include "vcs_config.hpp"
#include "vcs_texture_replacement.hpp"
#include "vcs_bootstrap_paths.hpp"
#include "vcs_project2dfx.hpp"
#include "vcs_draw_distance_patch.hpp"
#include "vcs_hdr_post.hpp"
#include "vcs_runtime_log.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>
#include <filesystem>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace {

#ifdef _WIN32
LONG WINAPI vcs_unhandled_exception_filter(EXCEPTION_POINTERS *info) noexcept {
    wchar_t module_path[32768]{};
    const DWORD module_len = GetModuleFileNameW(nullptr, module_path, 32768u);
    std::filesystem::path output = module_len != 0u
        ? std::filesystem::path(module_path).parent_path() / L"VCSNative_crash.txt"
        : std::filesystem::path(L"VCSNative_crash.txt");

    FILE *file = nullptr;
    _wfopen_s(&file, output.c_str(), L"wb");
    if (file != nullptr) {
        const unsigned long code = info != nullptr && info->ExceptionRecord != nullptr
            ? info->ExceptionRecord->ExceptionCode : 0ul;
        const void *address = info != nullptr && info->ExceptionRecord != nullptr
            ? info->ExceptionRecord->ExceptionAddress : nullptr;
        std::fprintf(file, "VCSNative (async GE + cross-unit hot-register cache) unhandled exception\r\n");
        const auto module_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
        const auto exception_pc = reinterpret_cast<std::uintptr_t>(address);
        const auto module_rva = exception_pc >= module_base ? exception_pc - module_base : 0u;
        std::fprintf(file, "code=0x%08lX address=%p host_thread=%lu\r\n",
                     code, address, static_cast<unsigned long>(GetCurrentThreadId()));
        std::fprintf(file, "module_base=%016llX module_rva=0x%llX\r\n",
                     static_cast<unsigned long long>(module_base),
                     static_cast<unsigned long long>(module_rva));
        if (info != nullptr && info->ExceptionRecord != nullptr &&
            info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
            info->ExceptionRecord->NumberParameters >= 2u) {
            const ULONG_PTR operation = info->ExceptionRecord->ExceptionInformation[0];
            const ULONG_PTR fault_address = info->ExceptionRecord->ExceptionInformation[1];
            const char *operation_name = operation == 0u ? "read" :
                (operation == 1u ? "write" : (operation == 8u ? "execute" : "unknown"));
            std::fprintf(file, "access=%s fault_address=%016llX\r\n", operation_name,
                         static_cast<unsigned long long>(fault_address));
        }
        std::fprintf(file, "guest_dispatch_pc=0x%08X guest_thread_uid=%d guest_thread_name=%s\r\n",
                     psprecomp::runtime_dispatch_pc(), psprecomp::runtime_thread_uid(),
                     psprecomp::runtime_thread_name());
#if defined(_M_X64)
        if (info != nullptr && info->ContextRecord != nullptr) {
            const CONTEXT &c = *info->ContextRecord;
            std::fprintf(file,
                         "RIP=%016llX RSP=%016llX RBP=%016llX RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX\r\n",
                         static_cast<unsigned long long>(c.Rip),
                         static_cast<unsigned long long>(c.Rsp),
                         static_cast<unsigned long long>(c.Rbp),
                         static_cast<unsigned long long>(c.Rax),
                         static_cast<unsigned long long>(c.Rbx),
                         static_cast<unsigned long long>(c.Rcx),
                         static_cast<unsigned long long>(c.Rdx));
        }
#endif
        std::fflush(file);
        std::fclose(file);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

std::uint64_t configured_max_dispatches() {
    // A direct double-click has no launcher script to override this. Keep the
    // old environment variable for diagnostics, but make the normal default
    // long-lived enough for an actual play session.
    constexpr std::uint64_t default_limit = 4'000'000'000ull;
    const char *text = std::getenv("PSPRECOMP_MAX_DISPATCHES");
    if (text == nullptr || *text == '\0') return default_limit;

    errno = 0;
    char *end = nullptr;
    const unsigned long long parsed = std::strtoull(text, &end, 0);
    if (errno == ERANGE || end == text || *end != '\0' || parsed == 0u ||
        parsed > std::numeric_limits<std::uint64_t>::max()) {
        throw psprecomp::Error(std::string("Invalid PSPRECOMP_MAX_DISPATCHES value: ") + text);
    }
    return static_cast<std::uint64_t>(parsed);
}

std::filesystem::path native_executable_directory(const char *argv0) {
#ifdef _WIN32
    std::wstring buffer(512u, L'\0');
    for (;;) {
        const DWORD length = GetModuleFileNameW(
            nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0u)
            throw psprecomp::Error("GetModuleFileNameW failed while locating VCSNative.exe");
        if (length < buffer.size() - 1u) {
            buffer.resize(length);
            return std::filesystem::path(buffer).parent_path();
        }
        buffer.resize(buffer.size() * 2u);
    }
#else
    return std::filesystem::absolute(argv0 != nullptr ? argv0 : "VCSNative").parent_path();
#endif
}

void validate_vcs_game_root(const std::filesystem::path &root) {
    if (std::getenv("PSPRECOMP_ALLOW_INCOMPLETE_ROOT") != nullptr) return;

    constexpr std::array<const char *, 2> required_boot_files{
        "PSP_GAME/USRDIR/RUNDATA/PSP/MOVIES/LOGO.PMF",
        "PSP_GAME/USRDIR/RUNDATA/PSP/MOVIES/TITLES.PMF",
    };

    std::ostringstream missing;
    std::size_t missing_count = 0u;
    for (const char *relative : required_boot_files) {
        const std::filesystem::path candidate = root / relative;
        std::error_code ec;
        if (!std::filesystem::is_regular_file(candidate, ec)) {
            missing << "\n  - " << relative;
            ++missing_count;
        }
    }
    if (missing_count != 0u) {
        throw psprecomp::Error(
            "Incomplete GTA VCS game root: missing required boot file(s):" + missing.str() +
            "\nRegenerate the compact root with tools/Gerar_pacote_minimo_VCS_v0_7.bat "
            "or provide a complete PSP_GAME/USRDIR tree. "
            "Set PSPRECOMP_ALLOW_INCOMPLETE_ROOT=1 only for low-level diagnostics.");
    }
}

bool environment_flag_enabled(const char *name) noexcept {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return false;
    return std::strcmp(text, "0") != 0 &&
           std::strcmp(text, "false") != 0 &&
           std::strcmp(text, "FALSE") != 0 &&
           std::strcmp(text, "off") != 0 &&
           std::strcmp(text, "OFF") != 0;
}

} // namespace

int main(int argc, char **argv) {
#ifdef _WIN32
    SetUnhandledExceptionFilter(&vcs_unhandled_exception_filter);
#endif
    // PSPRECOMP_V815_PRODUCTION_DEFAULTS
    // PSPRECOMP_V8151_FORCE_PRODUCTION_ON
    // PSPRECOMP_V8155_HARD_FORCE_ASYNC: the asynchronous GE path is available
    // for explicit benchmark runs, but stays off for the normal launcher until
    // its first post-intro frame is fully validated. Requiring a dedicated
    // opt-in also prevents an inherited PSPRECOMP_GE_ASYNC=1 from producing a
    // black frame on a normal launch. Parallel vertex decode remains available
    // for an explicit A/B run, but stays off by default until the first full
    // post-intro route proves that every guest vertex buffer lifetime is safe.
    // Set PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=1 to test it deliberately.
    const char *async_opt_in_text = std::getenv("PSPRECOMP_V8155_ENABLE_ASYNC");
    const bool async_opt_in = async_opt_in_text != nullptr && *async_opt_in_text != '\0' &&
        std::strcmp(async_opt_in_text, "0") != 0 &&
        std::strcmp(async_opt_in_text, "false") != 0 &&
        std::strcmp(async_opt_in_text, "FALSE") != 0 &&
        std::strcmp(async_opt_in_text, "off") != 0 &&
        std::strcmp(async_opt_in_text, "OFF") != 0;
    const char *parallel_decode_override =
        std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE");
    const bool parallel_decode_disabled = parallel_decode_override != nullptr &&
        *parallel_decode_override != '\0' &&
        (std::strcmp(parallel_decode_override, "0") == 0 ||
         std::strcmp(parallel_decode_override, "false") == 0 ||
         std::strcmp(parallel_decode_override, "FALSE") == 0 ||
         std::strcmp(parallel_decode_override, "off") == 0 ||
         std::strcmp(parallel_decode_override, "OFF") == 0);
    const bool parallel_decode_enabled = async_opt_in ||
        (parallel_decode_override != nullptr && !parallel_decode_disabled);
#ifdef _WIN32
    _putenv_s("PSPRECOMP_V8151_FORCE_SYNC", "");
    _putenv_s("PSPRECOMP_GE_ASYNC", async_opt_in ? "1" : "0");
    _putenv_s("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", parallel_decode_enabled ? "1" : "0");
#else
    unsetenv("PSPRECOMP_V8151_FORCE_SYNC");
    setenv("PSPRECOMP_GE_ASYNC", async_opt_in ? "1" : "0", 1);
    setenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE", parallel_decode_enabled ? "1" : "0", 1);
#endif
    try {
        const std::filesystem::path executable_directory =
            native_executable_directory(argc > 0 ? argv[0] : nullptr);
        const vcs::BootstrapPaths paths =
            vcs::resolve_bootstrap_paths(argc, argv, executable_directory);
        const std::filesystem::path &executable = paths.psp_executable;
        const std::filesystem::path &root = paths.game_root;
        vcs::initialize_vcs_configuration(executable_directory);
        const vcs::VcsConfiguration &configuration = vcs::vcs_configuration();
        vcs::runtime_log_initialize(configuration);
        vcs::runtime_log_line(std::string("v8155 hard_force_async=") +
                              (async_opt_in ? "1" : "0") +
                              " parallel_vertex=" + (parallel_decode_enabled ? "1" : "0") +
                              " opt_in=PSPRECOMP_V8155_ENABLE_ASYNC");
        // PSPRECOMP_V8151_ACTIVATION_LOG: prove the production defaults reached this executable.
        {
            const char *async_env = std::getenv("PSPRECOMP_GE_ASYNC");
            const char *decode_env = std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE");
            vcs::runtime_log_line(std::string("v8151 activation ge_async_env=") +
                                  (async_env != nullptr ? async_env : "<unset>") +
                                  " parallel_vertex_env=" +
                                  (decode_env != nullptr ? decode_env : "<unset>"));
            const char *indirect_env = std::getenv("PSPRECOMP_DX12_EXECUTE_INDIRECT");
            vcs::runtime_log_line(std::string("dx12 execute_indirect_env=") +
                                  (indirect_env != nullptr ? indirect_env : "<unset>"));
        }
        vcs::runtime_log_line(std::string("bootstrap executable=") + executable.string());
        vcs::runtime_log_line(std::string("bootstrap root=") + root.string());
        vcs::runtime_log_line(std::string("rendering backend=") +
                              vcs::rendering_backend_name(configuration.rendering.backend));
        validate_vcs_game_root(root);

        psprecomp::Elf32Image elf = psprecomp::Elf32Image::from_file(executable);
        psprecomp::Runtime runtime(32u * 1024u * 1024u);
        {
            std::ostringstream fastmem_line;
            fastmem_line << "aot direct fastmem enabled="
                         << (runtime.memory().direct_fastmem_enabled() ? 1 : 0);
            if (runtime.memory().direct_fastmem_enabled())
                fastmem_line << " base=0x" << std::hex
                             << runtime.memory().direct_fastmem_base_address() << std::dec
                             << " required=1 branchless_aot=1";
            else
                fastmem_line << " required=1 branchless_aot=1 unavailable=1";
            vcs::runtime_log_line(fastmem_line.str());
        }
        // V8.4 specializes the VCS automatic AOT corpus for the direct alias
        // map.  Do not limp into gameplay with null branchless accessors: fail
        // with an actionable message before any generated guest code executes.
        if (!runtime.memory().direct_fastmem_enabled()) {
            throw psprecomp::Error(
                "V8.4 aggressive CPU path requires Win64 direct fastmem, but the alias mapping could not be created. "
                "Ensure PSPRECOMP_AOT_DIRECT_FASTMEM is not disabled and no security/VA policy blocks MapViewOfFileEx. "
                "Use V8.2.7A if checked-memory fallback is required.");
        }
        // The heavy GUESTHOT sampler is opt-in. The rolling PERF telemetry stays on,
        // but normal gameplay does not pay a census/timestamp branch per cross-unit edge.
        const bool guest_hotspot_env = environment_flag_enabled("PSPRECOMP_GUEST_HOTSPOT");
        const bool guest_hotspot_enabled =
            configuration.diagnostics.guest_hotspot_profile || guest_hotspot_env;
        psprecomp::set_guest_hotspot_profile(guest_hotspot_enabled, 8u);
        vcs::runtime_log_line(std::string("guest_hotspot_runtime=") +
                              (guest_hotspot_enabled ? "1" : "0") +
                              " env=PSPRECOMP_GUEST_HOTSPOT");
        runtime.set_game_root(root);
        const auto relocations = elf.load_and_relocate(runtime.memory(), psprecomp::kDefaultPspUserLoadBase);
        std::uint64_t image_end = 0u;
        for (std::size_t index = 0; index < elf.segments().size(); ++index) {
            const auto &segment = elf.segments()[index];
            if (segment.type != 1u) continue;  // PT_LOAD
            const std::uint64_t start = elf.segment_runtime_address(index, psprecomp::kDefaultPspUserLoadBase);
            image_end = std::max(image_end, start + segment.memory_size);
        }
        if (image_end == 0u || image_end > 0x0A000000ull)
            throw psprecomp::Error("Invalid PSP ELF load image extent");
        const std::uint32_t user_arena_start =
            static_cast<std::uint32_t>((image_end + 0xFFu) & ~0xFFull);
        psprecomp::register_generated_functions(runtime);
        vcs::install_draw_distance_patch(runtime, configuration.source_path);
        vcs::install_project2dfx(runtime, configuration.source_path, 0u);
        // Legacy HDR configuration, if re-enabled later, belongs with the
        // optional shader layer rather than core VCSNative.ini.
        vcs::hdr_post_configure(executable_directory / "ProperShaders.ini");
        vcs::install_profile(runtime, user_arena_start);

        std::string gpu_backend_error;
        if (!vcs::initialize_ge_gpu_backend(gpu_backend_error))
            throw psprecomp::Error(gpu_backend_error);
        const vcs::GeGpuBackendReport gpu_start = vcs::ge_gpu_backend_report();
        vcs::runtime_log_line(std::string("ge backend requested=") +
                              vcs::ge_gpu_backend_name(gpu_start.requested) +
                              " active=" + vcs::ge_gpu_backend_name(gpu_start.active));
        vcs::runtime_log_line(std::string("ge backend status=") + gpu_start.message);

        std::cout << "VCSNative PSP bootstrap\n"
                  << "Launch mode: "
                  << (paths.discovered_from_psp_data
                          ? "PSP_DATA beside executable"
                          : "explicit command-line paths") << "\n"
                  << "Executable: " << executable.string() << "\n"
                  << "SHA-256:   " << psprecomp::sha256_file(executable) << "\n"
                  << "Entry:     " << psprecomp::hex32(elf.runtime_entry()) << "\n"
                  << "Relocs:    " << relocations.total << " (invalid " << relocations.invalid << ", unsupported " << relocations.unsupported << ")\n"
                  << "Functions: " << runtime.function_count() << "\n"
                  << "GE backend requested: " << vcs::ge_gpu_backend_name(gpu_start.requested) << "\n"
                  << "GE backend active:    " << vcs::ge_gpu_backend_name(gpu_start.active) << "\n"
                  << "GE backend status:    " << gpu_start.message << "\n"
                  << "Config:               " << configuration.source_path.string()
                  << (configuration.loaded_from_file ? " (loaded)" : " (defaults)") << "\n"
                  << "Display output:       "
                  << vcs::display_resolution_mode_name(configuration.display.resolution_mode)
                  << " / " << vcs::display_aspect_mode_name(configuration.display.aspect_mode)
                  << " / " << vcs::display_upscale_filter_name(configuration.display.upscale_filter)
                  << (configuration.display.fullscreen ? " / fullscreen" : " / windowed") << "\n";
        const vcs::InternalResolutionDimensions internal_resolution =
            vcs::resolve_internal_resolution(configuration.rendering);
        std::cout << "Rendering backend:    "
                  << vcs::rendering_backend_name(configuration.rendering.backend) << "\n"
                  << "Internal resolution:  " << internal_resolution.width << 'x'
                  << internal_resolution.height << " ("
                  << vcs::internal_resolution_mode_name(
                         configuration.rendering.internal_resolution_mode)
                  << ")\n";
        std::cout << "Multisampling:        ";
        if (configuration.rendering.msaa <= 1u)
            std::cout << "off\n";
        else
            std::cout << configuration.rendering.msaa
                      << "x requested (clamped to adapter support)\n";
        std::cout << "Antialiasing:         ";
        if (configuration.rendering.smaa && gpu_start.active == vcs::GeGpuBackendKind::DirectX12) {
            // The bundled SMAA shaders currently belong to the Vulkan postprocess
            // path.  Do not claim DX12 SMAA is active when the native GE backend
            // never executes those passes; use the real D3D12 MSAA path instead.
            std::cout << "SMAA requested but NOT active on native DX12 (use MSAA)\n";
        } else {
            std::cout << (configuration.rendering.smaa ? "SMAA 1x (Jimenez et al.)\n" : "off\n");
        }
        if (gpu_start.active == vcs::GeGpuBackendKind::DirectX12) {
            std::cout << "DX12 physical target: " << gpu_start.offscreen_width << 'x'
                      << gpu_start.offscreen_height << " | actual MSAA "
                      << gpu_start.dx12_msaa_samples << "x\n";
        }
        std::cout << "Anisotropic filter:   ";
        if (configuration.rendering.anisotropic_filtering <= 1u)
            std::cout << "off (PSP isotropic sampling)\n";
        else
            std::cout << configuration.rendering.anisotropic_filtering
                      << "x requested, mipmapped world textures only\n";
        // Reported so "did the widescreen option actually take effect" is a
        // fact on screen rather than something to infer from the picture.
        const vcs::DisplaySurfaceDimensions output_surface =
            vcs::resolve_display_surface_dimensions(configuration.display);
        const float widescreen_aspect = vcs::resolve_widescreen_aspect_ratio(
            configuration, output_surface.width, output_surface.height);
        const float widescreen_stretch = vcs::widescreen_stretch_factor(
            configuration, output_surface.width, output_surface.height);
        std::cout << "Widescreen:           ";
        if (widescreen_aspect <= 0.0f || widescreen_stretch == 1.0f) {
            std::cout << "off (PSP 16:9 projection)\n";
        } else {
            std::cout << "on, output " << output_surface.width << 'x' << output_surface.height
                      << ", aspect " << widescreen_aspect << " (frustum x"
                      << widescreen_stretch << ", DX12 HUD /" << widescreen_stretch
                      << ") -- ThirteenAG WidescreenFixesPack\n";
        }
        for (const std::string &warning : configuration.warnings)
            std::cerr << "VCSNative.ini warning: " << warning << "\n";

        if (runtime.function_count() == 0u) {
            std::cout << "No generated VCS functions are linked yet. Run psp_recomp after exporting functions.csv from Ghidra.\n";
            return 3;
        }
        if (const auto module = elf.find_module_info(runtime.memory(), psprecomp::kDefaultPspUserLoadBase)) {
            runtime.cpu().set_gpr(28, module->gp);
        } else {
            throw psprecomp::Error("PSP module info not found after relocation");
        }
        // install_profile() establishes the loader/module thread stack.
        runtime.cpu().set_gpr(31, 0u);
        runtime.cpu().set_gpr(4, 0u);
        runtime.cpu().set_gpr(5, 0u);
        std::string save_repro_restore_error;
        const bool save_repro_restored =
            vcs::restore_save_repro_checkpoint_if_requested(runtime, save_repro_restore_error);
        if (!save_repro_restore_error.empty())
            throw psprecomp::Error("SAVE REPRO restore failed: " + save_repro_restore_error);
        const std::uint32_t runtime_entry = save_repro_restored ? runtime.cpu().pc : elf.runtime_entry();
        const std::uint64_t max_dispatches = configured_max_dispatches();
        std::cout << "Dispatch cap: " << max_dispatches << "\n";
        vcs::display_window_start();

        vcs::install_display_heartbeat();
        vcs::install_starvation_preemption();
        runtime.run(runtime_entry, max_dispatches);
        const bool shutdown_diag = std::getenv("PSPRECOMP_SHUTDOWN_DIAG") != nullptr;
        if (shutdown_diag) std::cerr << "[shutdown] runtime-run-returned\n";
        std::cout << "Runtime stopped: " << runtime.stop_reason() << "\n";
        psprecomp::report_counted_pcs();
        if (shutdown_diag) std::cerr << "[shutdown] counted-pcs-reported\n";
        runtime.report_hle_histogram();
        psprecomp::report_unit_profile();
        if (shutdown_diag) std::cerr << "[shutdown] hle-histogram-reported\n";
        vcs::report_disc_read_stats();
        vcs::report_present_stats();
        if (shutdown_diag) std::cerr << "[shutdown] disc-stats-reported\n";
        const vcs::GeGpuBackendReport gpu_final = vcs::ge_gpu_backend_report();
        if (shutdown_diag) std::cerr << "[shutdown] gpu-report-copied\n";
        if (gpu_final.active == vcs::GeGpuBackendKind::DirectX12) {
            std::cout << "GE DirectX12: frames=" << gpu_final.game_frames
                      << " draws=" << gpu_final.game_draw_calls
                      << " triangles=" << gpu_final.game_triangles
                      << " vertices=" << gpu_final.game_vertices
                      << " hw_draws=" << gpu_final.hw_transform_draw_calls
                      << " pipelines=" << gpu_final.unique_pipeline_keys
                      << " target=" << gpu_final.offscreen_width << 'x' << gpu_final.offscreen_height
                      << " msaa=" << gpu_final.dx12_msaa_samples
                      << " depth=" << gpu_final.dx12_depth_bits
                      << " resolves=" << gpu_final.dx12_resolves
                      << " textured_pending=" << gpu_final.game_textured_draws_without_texture
                      << " readback_bytes=" << gpu_final.game_frame_readback_bytes
                      << "\n";
        }
        if (gpu_final.active == vcs::GeGpuBackendKind::DirectX12) {
            std::cout << "GE GPU backend: draws=" << gpu_final.draw_calls
                      << " vertices=" << gpu_final.vertices
                      << " textured=" << gpu_final.textured_draw_calls
                      << " pipelines=" << gpu_final.unique_pipeline_keys
                      << " textures=" << gpu_final.unique_texture_keys
                      << " texture_images=" << gpu_final.unique_texture_image_keys
                      << " shared_texture_images=" << gpu_final.shared_texture_images
                      << " ring=" << gpu_final.draw_ring_capacity
                      << " overwrites=" << gpu_final.draw_ring_overwrites
                      << " transfer_ready=" << gpu_final.transfer_self_test_passed
                      << " graphics_ready=" << gpu_final.offscreen_self_test_passed
                      << " offscreen=" << gpu_final.offscreen_width << "x" << gpu_final.offscreen_height
                      << " offscreen_pixels=" << gpu_final.offscreen_changed_pixels
                      << " offscreen_checksum=" << gpu_final.offscreen_checksum
                      << " upload_capacity=" << gpu_final.upload_capacity_bytes
                      << " frames_in_flight=" << gpu_final.frames_in_flight_capacity
                      << " staged_draws=" << gpu_final.staged_draw_calls
                      << " staged_vertices=" << gpu_final.staged_vertices
                      << " staged_bytes=" << gpu_final.staged_bytes
                      << " upload_wraps=" << gpu_final.upload_wraps
                      << " transfer_submissions=" << gpu_final.transfer_submissions
                      << " transfer_bytes=" << gpu_final.transfer_bytes
                      << " rejected=" << gpu_final.rejected_gpu_draws
                      << " game_frames=" << gpu_final.game_frames
                      << " game_draws=" << gpu_final.game_draw_calls
                      << " game_triangles=" << gpu_final.game_triangles
                      << " game_vertices=" << gpu_final.game_vertices
                      << " hw_transform_draws=" << gpu_final.hw_transform_draw_calls
                      << " hw_transform_vertices=" << gpu_final.hw_transform_vertices
                      << " hw_transform_prim_batches=" << gpu_final.hw_transform_prim_batches
                      << " hw_transform_unique_vertices_decoded=" << gpu_final.hw_transform_unique_vertices_decoded
                      << " hw_transform_index_reuses=" << gpu_final.hw_transform_index_reuses
                      << " game_changed_pixels=" << gpu_final.game_frame_changed_pixels
                      << " game_checksum=" << gpu_final.game_frame_checksum
                      << " texture_decode_requests=" << gpu_final.texture_decode_requests
                      << " texture_cache_hits=" << gpu_final.texture_cache_hits
                      << " decoded_textures=" << gpu_final.decoded_texture_uploads
                      << " decoded_texture_bytes=" << gpu_final.decoded_texture_bytes
                      << " decoded_t4=" << gpu_final.decoded_t4_textures
                      << " decoded_t8=" << gpu_final.decoded_t8_textures
                      << " texture_images=" << gpu_final.texture_images_created
                      << " texture_image_uploads=" << gpu_final.texture_image_uploads
                      << " texture_image_upload_bytes=" << gpu_final.texture_image_upload_bytes
                      << " descriptor_layout=" << gpu_final.texture_descriptor_layout_created
                      << " descriptor_pool=" << gpu_final.texture_descriptor_pool_created
                      << " textured_shaders=" << gpu_final.textured_shader_modules_created
                      << " textured_pipeline=" << gpu_final.textured_pipeline_created
                      << " depth_image=" << gpu_final.depth_image_created
                      << " depth_memory=" << gpu_final.depth_image_memory_bound
                      << " depth_view=" << gpu_final.depth_image_view_created
                      << " depth_attachment=" << gpu_final.depth_attachment_active
                      << " depth_pipeline_variants=" << gpu_final.depth_pipeline_variants_created
                      << " depth_tested_game_draws=" << gpu_final.depth_tested_game_draw_calls
                      << " depth_writing_game_draws=" << gpu_final.depth_writing_game_draw_calls
                      << " alpha_shader=" << gpu_final.alpha_test_shader_active
                      << " alpha_tested_game_draws=" << gpu_final.alpha_tested_game_draw_calls
                      << " blend_active=" << gpu_final.standard_alpha_blend_pipeline_active
                      << " blend_variants=" << gpu_final.blend_pipeline_variants_created
                      << " standard_blended_game_draws=" << gpu_final.standard_alpha_blended_game_draw_calls
                      << " fixed_replace_blended_game_draws=" << gpu_final.fixed_replace_blended_game_draw_calls
                      << " additive_blended_game_draws=" << gpu_final.additive_blended_game_draw_calls
                      << " unsupported_blend_game_draws=" << gpu_final.unsupported_blend_game_draw_calls
                      << " texture_function_active=" << gpu_final.observed_texture_function_shader_active
                      << " complete_texture_functions=" << gpu_final.complete_texture_function_shader_active
                      << " modulate_texture_game_draws=" << gpu_final.modulate_texture_game_draw_calls
                      << " decal_texture_game_draws=" << gpu_final.decal_texture_game_draw_calls
                      << " blend_texture_game_draws=" << gpu_final.blend_texture_game_draw_calls
                      << " replace_texture_game_draws=" << gpu_final.replace_texture_game_draw_calls
                      << " add_texture_game_draws=" << gpu_final.add_texture_game_draw_calls
                      << " double_color_texture_game_draws=" << gpu_final.double_color_texture_game_draw_calls
                      << " unsupported_texture_function_game_draws=" << gpu_final.unsupported_texture_function_game_draw_calls
                      << " color_mask_active=" << gpu_final.color_write_mask_pipeline_active
                      << " color_mask_variants=" << gpu_final.color_mask_pipeline_variants_created
                      << " masked_color_game_draws=" << gpu_final.masked_color_game_draw_calls
                      << " unsupported_partial_color_masks=" << gpu_final.unsupported_partial_color_mask_game_draw_calls
                      << " base_texture_formats=" << gpu_final.base_texture_formats_active
                      << " decoded_direct16=" << gpu_final.decoded_direct16_textures
                      << " decoded_direct32=" << gpu_final.decoded_direct32_textures
                      << " decoded_indexed16=" << gpu_final.decoded_indexed16_textures
                      << " decoded_indexed32=" << gpu_final.decoded_indexed32_textures
                      << " compressed_texture_formats=" << gpu_final.compressed_texture_formats_active
                      << " decoded_dxt1=" << gpu_final.decoded_dxt1_textures
                      << " decoded_dxt3=" << gpu_final.decoded_dxt3_textures
                      << " decoded_dxt5=" << gpu_final.decoded_dxt5_textures
                      << " mipmap_state=" << gpu_final.mipmap_state_active
                      << " mipmapped_game_draws=" << gpu_final.mipmapped_game_draw_calls
                      << " mip_linear_game_draws=" << gpu_final.mip_linear_game_draw_calls
                      << " full_mip_chain=" << gpu_final.full_mip_chain_active
                      << " uploaded_mip_levels=" << gpu_final.uploaded_mip_levels
                      << " automatic_lod_game_draws=" << gpu_final.automatic_lod_game_draw_calls
                      << " fixed_lod_game_draws=" << gpu_final.fixed_lod_game_draw_calls
                      << " slope_lod_game_draws=" << gpu_final.slope_lod_game_draw_calls
                      << " selected_nonzero_mip_game_draws=" << gpu_final.selected_nonzero_mip_game_draw_calls
                      << " framebuffer_targets=" << gpu_final.framebuffer_targets_observed
                      << " dx12_framebuffer_targets=" << gpu_final.dx12_native_framebuffer_targets
                      << " dx12_gpu_feedback=" << gpu_final.dx12_gpu_feedback_draws
                      << " dx12_self_feedback=" << gpu_final.dx12_self_feedback_snapshots
                      << " presented_target=0x" << std::hex << gpu_final.presented_framebuffer_target << std::dec
                      << " gpu_window_presented=" << gpu_final.gpu_frame_presented_to_window
                      << " release_candidate_ready=" << gpu_final.release_candidate_ready
                      << " fog_shader=" << gpu_final.fog_shader_active
                      << " fogged_game_draws=" << gpu_final.fogged_game_draw_calls
                      << " texture_samplers=" << gpu_final.texture_samplers_created
                      << " descriptor_sets=" << gpu_final.texture_descriptor_sets_allocated
                      << " textured_game_draws=" << gpu_final.textured_game_draw_calls
                      << " textured_game_triangles=" << gpu_final.textured_game_triangles
                      << " textured_game_vertices=" << gpu_final.textured_game_vertices
                      << " unbound_textured_draws=" << gpu_final.game_textured_draws_without_texture
                      << " rejected_texture_decodes=" << gpu_final.rejected_texture_decodes
                      << " evicted_textures=" << gpu_final.evicted_textures
                      << " recycled_descriptor_sets=" << gpu_final.recycled_texture_descriptor_sets
                      << " last_texture=" << gpu_final.last_texture_width << 'x'
                      << gpu_final.last_texture_height << " fmt="
                      << gpu_final.last_texture_format
                      << " texture_checksum=" << gpu_final.last_texture_checksum << "\n";
        }
        vcs::texture_replacement_log_summary();
        if (shutdown_diag) std::cerr << "[shutdown] before-display-shutdown\n";
        vcs::audio_output_shutdown();
        vcs::display_window_shutdown();
        if (shutdown_diag) std::cerr << "[shutdown] after-display-shutdown\n";
        vcs::shutdown_ge_gpu_backend();
        if (shutdown_diag) std::cerr << "[shutdown] after-gpu-shutdown\n";
        return runtime.stop_reason().empty() ? 0 : 4;
    } catch (const std::exception &e) {
        std::cerr << "VCSNative error: " << e.what() << "\n";
        return 1;
    }
}
