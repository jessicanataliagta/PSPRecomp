#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace vcs {

enum class DisplayResolutionMode : std::uint8_t {
    PspNative,
    Custom,
    Desktop,
};

enum class DisplayAspectMode : std::uint8_t {
    Preserve,
    Stretch,
};

enum class DisplayUpscaleFilter : std::uint8_t {
    Nearest,
    Bilinear,
};

enum class InternalResolutionMode : std::uint8_t {
    PspNative,
    Scale,
    Custom,
    Desktop,
};

enum class RenderingBackend : std::uint8_t {
    Software,
    DirectX12,
};

struct DisplayConfiguration {
    // The native PSP output remains the default so framebuffer hashes and
    // visual regressions are directly comparable with earlier stages.
    bool enabled{true};
    DisplayResolutionMode resolution_mode{DisplayResolutionMode::PspNative};
    std::uint32_t custom_width{480u};
    std::uint32_t custom_height{272u};
    bool fullscreen{false};
    DisplayAspectMode aspect_mode{DisplayAspectMode::Preserve};
    DisplayUpscaleFilter upscale_filter{DisplayUpscaleFilter::Nearest};
    bool integer_scale{false};
    // Native in-game overlay. This counts completed VCS render frames in wall
    // time (30 is full speed), rather than merely echoing the PSP's 60 Hz
    // vblank clock.
    bool show_fps{false};
    // Wait for the display's vertical blank when presenting (no tearing).
    bool vsync{false};
};

struct PresentationRectangle {
    std::int32_t x{};
    std::int32_t y{};
    std::int32_t width{};
    std::int32_t height{};
};

struct DisplaySurfaceDimensions {
    std::uint32_t width{480u};
    std::uint32_t height{272u};
};

[[nodiscard]] DisplaySurfaceDimensions resolve_display_surface_dimensions(
    const DisplayConfiguration &configuration) noexcept;

[[nodiscard]] PresentationRectangle calculate_presentation_rectangle(
    std::uint32_t client_width, std::uint32_t client_height,
    std::uint32_t source_width, std::uint32_t source_height,
    DisplayAspectMode aspect_mode, bool integer_scale) noexcept;

struct InternalResolutionDimensions {
    std::uint32_t width{480u};
    std::uint32_t height{272u};
};

struct RenderingConfiguration {
    // PSP-native remains authoritative by default. Higher modes allocate a
    // genuinely larger render target; they are not presentation upscalers.
    RenderingBackend backend{RenderingBackend::Software};
    InternalResolutionMode internal_resolution_mode{InternalResolutionMode::PspNative};
    std::uint32_t internal_scale{2u};
    std::uint32_t internal_width{960u};
    std::uint32_t internal_height{544u};
    // Anisotropic sampling of the game's own mip chain, 1 to 16. The PSP had
    // none, so 1 is what the parity presets keep. Only applies to the DirectX 12
    // backend, and only to mipmapped world textures.
    std::uint32_t anisotropic_filtering{1u};
    // Multisample count for the 3D render target: 1, 2, 4, 8 or 16. Unlike the
    // other image-quality options this defaults to on, because it is the one
    // that fixes geometric aliasing at the source rather than after the fact.
    // Clamped down to what the adapter reports for the colour and depth
    // formats actually in use.
    std::uint32_t msaa{4u};
    // Depth buffer precision in bits: 16, 24 or 32.
    //
    // 16 is what the PSP had, but the console rasterized at 480x272; at desktop
    // resolution far fewer surfaces fit in each of those 65536 steps and
    // distant geometry -- fences, billboards -- fights visibly. 24 fixes that
    // and is the default. 32 is float, which spends its precision next to the
    // near plane and is not recommended: it gave nearby bodywork enough
    // resolution to start resolving a small difference in Z between two passes
    // over the same panel, and the cars came out speckled.
    std::uint32_t depth_precision{24u};
    // Open-world texture working set.  A small entry limit makes the cache
    // repeatedly evict and re-decode city textures even while plenty of VRAM
    // remains available.
    std::uint32_t texture_cache_entries{8192u};
    std::uint32_t texture_cache_mb{256u};
    // Stage 39's hardware-transform frontend, which moves world/view/projection
    // and viewport onto the GPU for most projected draws.
    //
    // Generated/reflection UVs are supported by this path as well, keeping all
    // passes of vehicle bodywork on the same depth transform.
    bool hardware_transform{false};
    // Stage 44.6: enable the native Direct3D 12 GE path. It includes decoded
    // PSP textures, persistent framebuffer feedback, direct swapchain present,
    // depth precision selection and native MSAA/resolve.
    bool dx12_ge_color{false};
    // SMAA 1x, the spatial variant. The temporal ones need per-pixel motion
    // vectors, which this port has no way to produce. Off keeps PSP parity.
    bool smaa{false};
    bool experimental_gpu_color_preview{false};
    bool gpu_geometry_debug_colors{false};
    std::uint64_t dump_gpu_frame_vblank{0u};
};

[[nodiscard]] InternalResolutionDimensions resolve_internal_resolution(
    const RenderingConfiguration &configuration) noexcept;

struct AudioConfiguration {
    bool enabled{true};
    // Percent of the guest's own volume. 100 reproduces the PSP mix.
    std::uint32_t volume{100u};
    // Keep a compact queue-health log beside the executable. This is cheap
    // (one line every two seconds) and makes intermittent underruns observable.
    bool diagnostics{true};
    std::uint32_t prebuffer_blocks{6u};
    std::uint32_t recovery_prebuffer_blocks{12u};
};

struct TimingConfiguration {
    // Native VCS renders every other 59.94 Hz vblank (30 FPS). 60 removes that
    // skip; 120/240 also raise the virtual display cadence so they are real
    // game-frame targets rather than duplicated presentation frames.
    std::uint32_t frame_rate{240u};
    bool realtime_speed_diagnostics{false};
    std::uint64_t realtime_speed_interval_vblanks{120u};
};

struct DiagnosticsConfiguration {
    // Optional text log beside the executable. Used for startup failures,
    // DirectX 12 device/present errors, missing-asset reports and other host
    // faults that may not stay visible on a closed console window.
    bool log_to_file{false};
    std::string log_file{"VCSNative.log"};
    bool flush_every_line{true};
};

// Widescreen / ultrawide frustum, after ThirteenAG's WidescreenFixesPack
// (MIT, Copyright (c) 2018 ThirteenAG) -- specifically the VCS WidescreenFix
// plugin. See host/ge_renderer.cpp for how the same correction
// is expressed here and why it cannot be a memory patch in this port.
//
// Off by default: PSP parity stays the baseline for visual regressions.
struct WidescreenConfiguration {
    bool enabled{false};
    // 0/0 means "auto": derive the ratio from the presentation surface.
    std::uint32_t aspect_x{0u};
    std::uint32_t aspect_y{0u};
};

enum class HudTextureFilter : std::uint8_t {
    Off,
    Smooth,
    Sharp,
};

// [HudTextures]: through-mode (2D interface) texture enhancement in the
// DirectX 12 backend. See host/vcs_hud_texture_enhance.hpp.
//
// Off by default so a missing INI keeps PSP parity; the shipped VCSNative.ini
// turns the filter on.
struct HudTexturesConfiguration {
    // Off: upload the PSP art as decoded. Smooth: alpha bleeding + Catmull-Rom
    // upscale + mip chain, sampled linearly. Sharp: Smooth plus edge-only ramp
    // narrowing, which is what makes glyphs and icon outlines read as HD.
    HudTextureFilter filter{HudTextureFilter::Off};
    // Largest integer enlargement (2-4). Also limited by the internal
    // resolution (no point enlarging past what the target can show) and by a
    // 1024-texel cap per side.
    std::uint32_t max_scale{4u};
    // Write every distinct 2D texture once to <Directory>/dump/<hash>.dds.
    bool dump{false};
    // Use <Directory>/**/<hash>.dds in place of the game's texture when present.
    // Any size with the original aspect ratio is accepted.
    bool replace{false};
    // Relative paths are resolved against the executable's directory.
    std::string directory{"TexturesHD"};
    // LRU cache of finished (enhanced + mip-chained) results keyed by guest
    // content. VCS cycles some HUD images through fresh texture keys every
    // frame; without it each recurrence pays the full upscale again. The
    // least recently used entries are dropped once either limit is exceeded.
    std::uint32_t cache_entries{256u};
    std::uint32_t cache_mb{128u};
};

// Standalone ProperShaders.ini feature. Values normally supplied by the San
// Andreas timecycle/weather integration remain explicit placeholders until the
// equivalent VCS guest hooks exist. Keeping the inputs separate is important:
// collapsing the three decks and lighting state into one "coverage" value
// materially changes CloudWorks' appearance.
struct VolumetricCloudsConfiguration {
    bool enabled{false};
    std::uint32_t downscale_div{2u};
    std::uint32_t layers{2u};
    std::uint32_t shadow_steps{8u};
    float coverage_low{0.35f};
    float coverage_mid{0.25f};
    float coverage_high{0.18f};
    float opacity{1.0f};
    float speed{0.0f};
    float brightness{1.0f};
    float random_seed{0.0f};
    float sun_direction_x{0.38f};
    float sun_direction_y{-0.28f};
    float sun_direction_z{0.88f};
    float sun_color_r{1.0f};
    float sun_color_g{0.97f};
    float sun_color_b{0.88f};
    float cloud_base_color_r{0.70f};
    float cloud_base_color_g{0.70f};
    float cloud_base_color_b{0.70f};
    float atmosphere_density{0.0f};
    float mist{0.50f};
    float fog_color_r{0.58f};
    float fog_color_g{0.68f};
    float fog_color_b{0.78f};
    float fog_start{4500.0f};
    float day_progression{0.88f};
    float temporal_blend{0.50f};
    float temporal_denoise{1.0f};
    float temporal_clamp{1.0f};
};

// The aspect the game itself builds its projection with.  VCS loads the
// constant 0x3FE38E39 -- exactly 16/9 -- and everything is relative to it.
inline constexpr float kGameNativeAspectRatio = 16.0f / 9.0f;

struct ControlsConfiguration {
    // Mouse and right-stick camera. Needs the guest-side hook, which bypasses
    // the game's own camera conditions, so it is opt-in.
    bool camera_stick{true};
    // Stick deflection per raw mouse count. The axis saturates at 127, so this
    // sets how far the mouse travels before the camera is already turning as
    // fast as the game will turn it -- past that point the value is pinned and
    // every difference in how fast you moved is thrown away, which reads as the
    // camera stepping rather than sweeping. 12 keeps a normal flick
    // proportional; the ceiling on turn speed is the game's, not this number's.
    std::uint32_t mouse_sensitivity{12u};
    bool invert_camera_y{false};
    // Safe stop for upward input on the outdoor on-foot camera. Stock VCS's
    // actual clamp remains at 45 degrees; this prevents reaching it without
    // changing vehicle or interior cameras.
    std::uint32_t ped_camera_up_limit_degrees{45u};
    // ThirteenAG's ModernControlScheme: accelerate on R, brake on L.
    //
    // Off, and not merely as a conservative default. Stock VCS accelerates on
    // Cross and brakes on Square -- measured, the accessors load pad offsets
    // 0x2A and 0x26 -- which is what San Andreas does on a pad, so leaving this
    // off is the San Andreas-parity setting and turning it on is the GTA IV/V
    // one. The original PSP configuration also leaves it disabled.
    bool modern_control_scheme{false};
};

struct VcsConfiguration {
    ControlsConfiguration controls{};
    DisplayConfiguration display{};
    RenderingConfiguration rendering{};
    AudioConfiguration audio{};
    TimingConfiguration timing{};
    DiagnosticsConfiguration diagnostics{};
    WidescreenConfiguration widescreen{};
    HudTexturesConfiguration hud_textures{};
    VolumetricCloudsConfiguration volumetric_clouds{};
    std::filesystem::path source_path{};
    // Where the executable lives. Saves go beside it rather than into the game
    // data, so a player who points the runtime at a read-only or shared copy of
    // the game still has somewhere to write, and so nobody has to go looking
    // inside the extracted disc for their progress.
    std::filesystem::path executable_directory{};
    bool initialized{false};
    bool loaded_from_file{false};
    std::vector<std::string> warnings{};
};

// Parses one INI without modifying the process-global configuration. Exposed
// for deterministic tests and small configuration tools.
[[nodiscard]] VcsConfiguration load_vcs_configuration(const std::filesystem::path &path);

// Loads PSPRECOMP_CONFIG when set, otherwise <executable_dir>/VCSNative.ini.
// Missing files are not errors: defaults deliberately reproduce PSP 480x272.
void initialize_vcs_configuration(const std::filesystem::path &executable_directory);

[[nodiscard]] const VcsConfiguration &vcs_configuration();
[[nodiscard]] const char *display_resolution_mode_name(DisplayResolutionMode mode) noexcept;
[[nodiscard]] const char *display_aspect_mode_name(DisplayAspectMode mode) noexcept;
[[nodiscard]] const char *display_upscale_filter_name(DisplayUpscaleFilter filter) noexcept;
[[nodiscard]] const char *internal_resolution_mode_name(InternalResolutionMode mode) noexcept;
[[nodiscard]] const char *rendering_backend_name(RenderingBackend backend) noexcept;

// Resolved widescreen ratio, or 0 when the correction is off.  "auto" resolves
// against the surface the game is actually presented on.
[[nodiscard]] float resolve_widescreen_aspect_ratio(
    const VcsConfiguration &configuration,
    std::uint32_t surface_width, std::uint32_t surface_height) noexcept;

// How much wider than its own 16:9 projection the game is being shown at.
// 1 when the correction is off or the target already is 16:9, in which case
// nothing anywhere has to compensate for anything.
//
// The world is widened by this much (the guest builds its projection with the
// resolved aspect instead of 16/9), and the 2D interface -- which is authored
// in a 480-wide screen space that knows nothing about any of this -- is shrunk
// by the same amount so the stretch to the window cancels out.
[[nodiscard]] float widescreen_stretch_factor(
    const VcsConfiguration &configuration,
    std::uint32_t surface_width, std::uint32_t surface_height) noexcept;

} // namespace vcs
