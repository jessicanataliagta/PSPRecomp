// VCS integration of the shared GE software renderer: forwards the renderer's
// GPU-backend calls to the DX12 backend and supplies the VCS configuration and
// per-draw camera/overlay taps.

#include "ge_gpu_backend.hpp"
#include "ge_renderer.hpp"
#include "vcs_config.hpp"
#include "vcs_fps_overlay.hpp"
#include "vcs_project2dfx.hpp"
#include "vcs_profile.hpp"

#include "psprecomp/hle/ge_renderer.hpp"

#include <bit>
#include <utility>

namespace psprecomp::hle::gpu {

void ge_gpu_backend_accumulate_color_triangles(const GeGpuDrawDescriptor &draw, std::span<const GeGpuVertex> triangle_vertices) noexcept {
    vcs::ge_gpu_backend_accumulate_color_triangles(draw, triangle_vertices);
}

bool ge_gpu_backend_accumulate_hardware_packed_0115(const GeGpuDrawDescriptor &draw, const GeGpuHardwareTransform &transform, std::span<const std::byte> packed_vertices, std::uint32_t vertex_count, std::span<const std::uint32_t> triangle_indices) noexcept {
    return vcs::ge_gpu_backend_accumulate_hardware_packed_0115(draw, transform, packed_vertices, vertex_count, triangle_indices);
}

void ge_gpu_backend_accumulate_hardware_triangles(const GeGpuDrawDescriptor &draw, const GeGpuHardwareTransform &transform, std::span<const GeGpuVertex> vertices, std::span<const std::uint32_t> triangle_indices) noexcept {
    vcs::ge_gpu_backend_accumulate_hardware_triangles(draw, transform, vertices, triangle_indices);
}

bool ge_gpu_backend_active() noexcept {
    return vcs::ge_gpu_backend_active();
}

bool ge_gpu_backend_adopt_shared_texture(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_adopt_shared_texture(draw);
}

std::uint32_t ge_gpu_backend_display_framebuffer() noexcept {
    return vcs::ge_gpu_backend_display_framebuffer();
}

bool ge_gpu_backend_graphics_ready() noexcept {
    return vcs::ge_gpu_backend_graphics_ready();
}

bool ge_gpu_backend_is_framebuffer_feedback_texture(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_is_framebuffer_feedback_texture(draw);
}

void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &draw, float max_x, float max_y) noexcept {
    vcs::ge_gpu_backend_note_through_extent(draw, max_x, max_y);
}

void ge_gpu_backend_observe_camera(const std::array<float, 12> &view, const std::array<float, 16> &projection, const std::array<float, 6> &viewport, const std::array<float, 3> &camera_position, const GeGpuDrawDescriptor &draw, std::uint32_t vertex_weight) noexcept {
    vcs::ge_gpu_backend_observe_camera(view, projection, viewport, camera_position, draw, vertex_weight);
}

std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept {
    return vcs::ge_gpu_backend_owned_framebuffer();
}

void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &draw) noexcept {
    vcs::ge_gpu_backend_prepare_texture_keys(draw);
}

bool ge_gpu_backend_presents_directly() noexcept {
    return vcs::ge_gpu_backend_presents_directly();
}

void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &draw) noexcept {
    vcs::ge_gpu_backend_record_draw(draw);
}

bool ge_gpu_backend_stage_vertices(const GeGpuDrawDescriptor &draw, std::span<const GeGpuVertex> vertices) noexcept {
    return vcs::ge_gpu_backend_stage_vertices(draw, vertices);
}

bool ge_gpu_backend_texture_available(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_texture_available(draw);
}

bool ge_gpu_backend_texture_needed(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_texture_needed(draw);
}

bool ge_gpu_backend_texture_signature_needed(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_texture_signature_needed(draw);
}

bool ge_gpu_backend_transfer_ready() noexcept {
    return vcs::ge_gpu_backend_transfer_ready();
}

bool ge_gpu_backend_upload_decoded_texture(const GeGpuDrawDescriptor &draw, std::uint32_t width, std::uint32_t height, std::span<const std::byte> rgba8) noexcept {
    return vcs::ge_gpu_backend_upload_decoded_texture(draw, width, height, rgba8);
}

bool ge_gpu_backend_upload_decoded_texture_chain_packed(const GeGpuDrawDescriptor &draw, std::uint32_t base_width, std::uint32_t base_height, std::uint32_t mip_levels, std::vector<std::byte> rgba8) noexcept {
    return vcs::ge_gpu_backend_upload_decoded_texture_chain_packed(draw, base_width, base_height, mip_levels, std::move(rgba8));
}

GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(const GeGpuDrawDescriptor &draw) noexcept {
    return vcs::ge_gpu_backend_widescreen_hud(draw);
}

} // namespace psprecomp::hle::gpu

namespace vcs {
namespace {

constexpr std::uint32_t data24(std::uint32_t command) noexcept { return command & 0x00FFFFFFu; }
float decode_float24(std::uint32_t data) noexcept {
    return std::bit_cast<float>((data & 0x00FFFFFFu) << 8u);
}

bool vcs_hardware_transform_enabled() {
    const VcsConfiguration &config = vcs_configuration();
    return config.initialized && config.rendering.hardware_transform;
}

bool vcs_gpu_color_authoritative() {
    // Native DX12 GE is authoritative. The first physical Stage 44.6 run
    // exposed that the old conservative policy still CPU-rasterized most
    // offscreen targets as well as submitting the same geometry to D3D12.
    // That double raster is unnecessary once DX12GEColor is enabled.
    const VcsConfiguration &cfg = vcs_configuration();
    return cfg.initialized && cfg.rendering.backend == RenderingBackend::DirectX12 &&
           cfg.rendering.dx12_ge_color;
}

void vcs_observe_gpu_draw(const psprecomp::GuestMemory &memory, const std::array<std::uint32_t, 256> &commands,
                          const GeTransformState &transform, const GeGpuDrawDescriptor &gpu_draw,
                          std::uint32_t primitive, std::uint32_t count, std::uint64_t camera_state_revision) {
    if (!gpu_draw.through && !gpu_draw.clear_mode &&
        primitive >= 3u && primitive <= 5u) {
        const std::array<float, 6> cloud_viewport{
            decode_float24(data24(commands[0x42u])),
            decode_float24(data24(commands[0x43u])),
            decode_float24(data24(commands[0x45u])),
            decode_float24(data24(commands[0x46u])),
            static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f,
            static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f};
        // VCS' authoritative camera origin. The affine GE view used by
        // individual passes is not guaranteed to encode this position as
        // a rigid inverse (reflections and camera-relative passes do not),
        // which made a world-space cloud slab orbit while only turning.
        constexpr std::uint32_t kVcsCameraPosition = 0x08BC87E0u;
        std::array<float, 3> cloud_camera_position{};
        if (memory.contains(kVcsCameraPosition, 12u)) {
            cloud_camera_position = {
                std::bit_cast<float>(memory.load32(kVcsCameraPosition + 0u)),
                std::bit_cast<float>(memory.load32(kVcsCameraPosition + 4u)),
                std::bit_cast<float>(memory.load32(kVcsCameraPosition + 8u))};
        }
        ge_gpu_backend_observe_camera(transform.view, transform.projection,
                                      cloud_viewport, cloud_camera_position,
                                      gpu_draw, count);
    }
    if (!gpu_draw.clear_mode && primitive >= 3u && primitive <= 6u)
        fps_overlay_observe_draw(gpu_draw, count);
    if (!gpu_draw.through && !gpu_draw.clear_mode &&
        primitive >= 3u && primitive <= 5u &&
        !project2dfx_observe_camera_hot(gpu_draw, count, camera_state_revision)) {
        project2dfx_observe_camera(
            transform.view, transform.projection,
            decode_float24(data24(commands[0x42u])),
            decode_float24(data24(commands[0x43u])),
            decode_float24(data24(commands[0x44u])),
            decode_float24(data24(commands[0x45u])),
            decode_float24(data24(commands[0x46u])),
            decode_float24(data24(commands[0x47u])),
            static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f,
            static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f,
            1.0f, gpu_draw, count, camera_state_revision);
    }
}

} // namespace

void install_ge_renderer_hooks() {
    psprecomp::hle::g_ge_renderer_hooks = psprecomp::hle::GeRendererHooks{
        &vcs_hardware_transform_enabled, &vcs_gpu_color_authoritative, &vcs_observe_gpu_draw};
}

} // namespace vcs
