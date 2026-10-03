#include "vcs_config.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {

void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

} // namespace

int main() {
    try {
        const std::filesystem::path root =
            std::filesystem::temp_directory_path() / "vcsnative_config_test";
        std::filesystem::create_directories(root);
        const std::filesystem::path path = root / "VCSNative.ini";
        {
            std::ofstream output(path, std::ios::trunc);
            output << "[Display]\n"
                   << "Enabled=yes\n"
                   << "ResolutionMode=Desktop\n"
                   << "Width=2560\n"
                   << "Height=1440\n"
                   << "Fullscreen=1\n"
                   << "AspectRatio=Preserve\n"
                   << "UpscaleFilter=Bilinear\n"
                   << "IntegerScale=no\n"
                   << "ShowFPS=true\n"
                   << "[Rendering]\n"
                   << "Backend=DirectX12\n"
                   << "DX12GEColor=true\n"
                   << "InternalResolutionMode=Scale\n"
                   << "InternalScale=2\n"
                   << "InternalWidth=2560\n"
                   << "InternalHeight=1440\n"
                   << "TextureCacheEntries=8192\n"
                   << "TextureCacheMB=256\n"
                   << "ExperimentalGpuColorPreview=true\nGeometryDebugColors=true\n"
                   << "DumpGpuFrameVblank=2200\n"
                   << "[Timing]\n"
                   << "FrameRate=200\n"
                   << "RealtimeSpeedDiagnostics=true\n"
                   << "RealtimeSpeedIntervalVblanks=90\n"
                   << "[Diagnostics]\n"
                   << "LogToFile=true\n"
                   << "LogFile=vcs-config-test.log\n"
                   << "FlushEveryLine=false\n"
                   << "[Controls]\n"
                   << "CameraStick=true\n"
                   << "MouseSensitivity=17\n"
                   << "InvertCameraY=true\n"
                   << "PedCameraUpLimitDegrees=40\n"
                   << "[Disc]\n"
                   << "Prefetch=false\n"
                   << "[Widescreen]\n"
                   << "Enabled=true\n"
                   << "AspectRatio=21:9\n"
                   << "[Project2DFX]\n"
                   << "Enabled=true\n"
                   << "[LodLights]\n"
                   << "FarClip=500\n"
                   << "[TrafficLights]\n"
                   << "Enabled=true\n"
                   << "[BlinkingLights]\n"
                   << "Enabled=true\n"
                   << "[SkyGfx]\n"
                   << "Enabled=true\n"
                   << "[HeliHeight]\n"
                   << "Height=800\n"
                   << "[DrawDistance]\n"
                   << "World=1.5\n";
        }
        const vcs::VcsConfiguration config = vcs::load_vcs_configuration(path);
        require(config.loaded_from_file, "INI was not loaded");
        require(config.warnings.empty(), "valid INI produced warnings");
        require(config.display.enabled, "Display.Enabled was not parsed");
        require(config.display.resolution_mode == vcs::DisplayResolutionMode::Desktop,
                "Desktop resolution mode was not parsed");
        require(config.display.custom_width == 2560u && config.display.custom_height == 1440u,
                "custom dimensions were not parsed");
        require(config.display.fullscreen, "fullscreen was not parsed");
        require(config.display.aspect_mode == vcs::DisplayAspectMode::Preserve,
                "aspect mode was not parsed");
        require(config.display.upscale_filter == vcs::DisplayUpscaleFilter::Bilinear,
                "bilinear filter was not parsed");
        require(!config.display.integer_scale, "integer scale was not parsed");
        require(config.display.show_fps, "Display.ShowFPS was not parsed");
        require(config.rendering.backend == vcs::RenderingBackend::DirectX12,
                "DirectX 12 backend was not parsed");
        require(config.rendering.dx12_ge_color,
                "DX12 GE color path was not parsed");
        require(config.rendering.internal_resolution_mode == vcs::InternalResolutionMode::Scale,
                "internal scale mode was not parsed");
        require(config.rendering.internal_scale == 2u,
                "internal scale value was not parsed");
        require(config.rendering.internal_width == 2560u &&
                config.rendering.internal_height == 1440u,
                "internal custom dimensions were not parsed");
        require(config.rendering.texture_cache_entries == 8192u &&
                config.rendering.texture_cache_mb == 256u,
                "texture cache limits were not parsed");
        require(config.rendering.experimental_gpu_color_preview,
                "GPU color preview was not parsed");
        require(config.rendering.gpu_geometry_debug_colors,
                "GPU geometry debug colors were not parsed");
        require(config.rendering.dump_gpu_frame_vblank == 2200u,
                "GPU dump vblank was not parsed");
        const vcs::InternalResolutionDimensions scaled =
            vcs::resolve_internal_resolution(config.rendering);
        require(scaled.width == 960u && scaled.height == 544u,
                "2x internal resolution was computed incorrectly");
        vcs::RenderingConfiguration custom_rendering = config.rendering;
        custom_rendering.internal_resolution_mode = vcs::InternalResolutionMode::Custom;
        const vcs::InternalResolutionDimensions custom =
            vcs::resolve_internal_resolution(custom_rendering);
        require(custom.width == 2560u && custom.height == 1440u,
                "custom internal resolution was computed incorrectly");
        require(config.timing.realtime_speed_diagnostics,
                "timing diagnostics were not parsed");
        require(config.timing.frame_rate == 200u,
                "Timing.FrameRate was not parsed");
        require(config.timing.realtime_speed_interval_vblanks == 90u,
                "timing interval was not parsed");
        require(config.diagnostics.log_to_file,
                "Diagnostics.LogToFile was not parsed");
        require(config.diagnostics.log_file == "vcs-config-test.log",
                "Diagnostics.LogFile was not parsed");
        require(!config.diagnostics.flush_every_line,
                "Diagnostics.FlushEveryLine was not parsed");
        require(config.controls.camera_stick, "camera stick was not parsed");
        require(config.controls.mouse_sensitivity == 17u,
                "mouse sensitivity was not parsed");
        require(config.controls.invert_camera_y, "camera inversion was not parsed");
        require(config.controls.ped_camera_up_limit_degrees == 40u,
                "on-foot camera upper limit was not parsed");

        const vcs::PresentationRectangle fit = vcs::calculate_presentation_rectangle(
            1920u, 1080u, 480u, 272u, vcs::DisplayAspectMode::Preserve, false);
        require(fit.x == 7 && fit.y == 0 && fit.width == 1906 && fit.height == 1080,
                "aspect-preserving desktop fit was computed incorrectly");
        const vcs::PresentationRectangle integer = vcs::calculate_presentation_rectangle(
            1920u, 1080u, 480u, 272u, vcs::DisplayAspectMode::Preserve, true);
        require(integer.x == 240 && integer.y == 132 && integer.width == 1440 && integer.height == 816,
                "integer-scale desktop fit was computed incorrectly");
        const vcs::PresentationRectangle stretch = vcs::calculate_presentation_rectangle(
            1920u, 1080u, 480u, 272u, vcs::DisplayAspectMode::Stretch, false);
        require(stretch.x == 0 && stretch.y == 0 && stretch.width == 1920 && stretch.height == 1080,
                "stretch output rectangle was computed incorrectly");

        const vcs::VcsConfiguration missing =
            vcs::load_vcs_configuration(root / "missing.ini");
        {
            std::ofstream proper(root / "ProperShaders.ini", std::ios::trunc);
            proper << "[VolumetricClouds]\n"
                   << "Enabled=true\n"
                   << "DownscaleDiv=4\n"
                   << "Layers=3\n"
                   << "ShadowSteps=6\n"
                   << "CoverageLow=0.61\n"
                   << "CoverageMid=0.42\n"
                   << "CoverageHigh=0.23\n"
                   << "Opacity=0.72\n"
                   << "Speed=150.0\n"
                   << "Mist=0.67\n"
                   << "DayProgression=-0.12\n";
        }
        vcs::initialize_vcs_configuration(root);
        const auto &clouds = vcs::vcs_configuration().volumetric_clouds;
        require(clouds.enabled, "ProperShaders.ini VolumetricClouds.Enabled was not parsed");
        require(clouds.downscale_div == 4u && clouds.layers == 3u &&
                clouds.shadow_steps == 6u,
                "ProperShaders.ini cloud quality controls were not parsed");
        require(std::abs(clouds.coverage_low - 0.61f) < 0.0001f &&
                std::abs(clouds.coverage_mid - 0.42f) < 0.0001f &&
                std::abs(clouds.coverage_high - 0.23f) < 0.0001f &&
                std::abs(clouds.opacity - 0.72f) < 0.0001f &&
                std::abs(clouds.speed - 150.0f) < 0.0001f &&
                std::abs(clouds.mist - 0.67f) < 0.0001f &&
                std::abs(clouds.day_progression + 0.12f) < 0.0001f,
                "ProperShaders.ini cloud parameters were not parsed");
        // Widescreen: explicit ratio, "auto", and off.  The correction must be
        // exactly neutral when disabled -- PSP parity stays the baseline.
        require(!config.disc.prefetch, "Disc.Prefetch was not parsed");
        require(config.widescreen.enabled, "Widescreen.Enabled was not parsed");
        require(config.widescreen.aspect_x == 21u && config.widescreen.aspect_y == 9u,
                "Widescreen.AspectRatio 'x:y' was not parsed");
        {
            const float forced = vcs::resolve_widescreen_aspect_ratio(config, 1920u, 1080u);
            require(std::abs(forced - 21.0f / 9.0f) < 0.0001f,
                    "an explicit widescreen ratio must ignore the surface size");

            vcs::VcsConfiguration automatic = config;
            automatic.widescreen.aspect_x = 0u;
            automatic.widescreen.aspect_y = 0u;
            require(std::abs(vcs::resolve_widescreen_aspect_ratio(automatic, 3440u, 1440u) -
                             3440.0f / 1440.0f) < 0.0001f,
                    "'auto' must resolve against the presentation surface");

            vcs::DisplayConfiguration custom_display = config.display;
            custom_display.resolution_mode = vcs::DisplayResolutionMode::Custom;
            custom_display.custom_width = 3440u;
            custom_display.custom_height = 1440u;
            const vcs::DisplaySurfaceDimensions display_surface =
                vcs::resolve_display_surface_dimensions(custom_display);
            require(display_surface.width == 3440u && display_surface.height == 1440u,
                    "display surface dimensions must stay independent of internal resolution");

            vcs::VcsConfiguration off = config;
            off.widescreen.enabled = false;
            require(vcs::resolve_widescreen_aspect_ratio(off, 3440u, 1440u) == 0.0f,
                    "a disabled widescreen correction must resolve to zero");

            // The stretch factor is what both halves of the correction read: the
            // guest widens its frustum by it and the interface is shrunk by it.
            // Disagreement between the two is what a stretched HUD looks like.
            require(vcs::widescreen_stretch_factor(off, 3440u, 1440u) == 1.0f,
                    "a disabled widescreen correction must not stretch anything");
            require(std::abs(vcs::widescreen_stretch_factor(config, 1920u, 1080u) -
                             (21.0f / 9.0f) / (16.0f / 9.0f)) < 0.0001f,
                    "the stretch factor must be the ratio against the game's own 16:9");
            require(vcs::widescreen_stretch_factor(automatic, 1920u, 1080u) == 1.0f,
                    "a 16:9 surface must need no correction at all");
            vcs::VcsConfiguration absurd = config;
            absurd.widescreen.aspect_x = 1000u;
            absurd.widescreen.aspect_y = 1u;
            require(vcs::widescreen_stretch_factor(absurd, 1920u, 1080u) == 4.0f,
                    "an out-of-range ratio must be clamped, not obeyed");
        }

        require(!missing.loaded_from_file, "missing INI was reported as loaded");
        require(missing.disc.prefetch, "missing INI must keep disc prefetch enabled");
        require(missing.display.resolution_mode == vcs::DisplayResolutionMode::PspNative,
                "missing INI did not preserve PSP-native default");
        require(missing.display.custom_width == 480u && missing.display.custom_height == 272u,
                "missing INI did not preserve 480x272 defaults");
        require(missing.display.upscale_filter == vcs::DisplayUpscaleFilter::Nearest,
                "missing INI did not preserve nearest-neighbour default");
        require(missing.rendering.backend == vcs::RenderingBackend::Software,
                "missing INI did not preserve software backend default");
        require(missing.rendering.internal_resolution_mode ==
                    vcs::InternalResolutionMode::PspNative,
                "missing INI did not preserve PSP internal-resolution default");
        require(missing.controls.ped_camera_up_limit_degrees == 45u,
                "missing INI did not preserve the stock on-foot camera upper limit");
        const vcs::InternalResolutionDimensions native =
            vcs::resolve_internal_resolution(missing.rendering);
        require(native.width == 480u && native.height == 272u,
                "missing INI did not preserve 480x272 internal resolution");

        std::filesystem::remove_all(root);
        std::cout << "vcs_config_tests: PASS\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "vcs_config_tests: FAIL: " << error.what() << "\n";
        return 1;
    }
}
