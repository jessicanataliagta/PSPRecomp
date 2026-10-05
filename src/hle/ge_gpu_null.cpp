// GPU-backend entry points for profiles without a GPU backend. Every query
// reports "inactive", so the GE software rasterizer handles every draw.

#include "psprecomp/hle/ge_gpu.hpp"

#include <vector>

namespace psprecomp::hle::gpu {

void ge_gpu_backend_accumulate_color_triangles(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex>) noexcept { }
bool ge_gpu_backend_accumulate_hardware_packed_0115(const GeGpuDrawDescriptor &, const GeGpuHardwareTransform &, std::span<const std::byte>, std::uint32_t, std::span<const std::uint32_t>) noexcept { return false; }
void ge_gpu_backend_accumulate_hardware_triangles(const GeGpuDrawDescriptor &, const GeGpuHardwareTransform &, std::span<const GeGpuVertex>, std::span<const std::uint32_t>) noexcept { }
bool ge_gpu_backend_active() noexcept { return false; }
bool ge_gpu_backend_adopt_shared_texture(const GeGpuDrawDescriptor &) noexcept { return false; }
std::uint32_t ge_gpu_backend_display_framebuffer() noexcept { return 0u; }
bool ge_gpu_backend_graphics_ready() noexcept { return false; }
bool ge_gpu_backend_is_framebuffer_feedback_texture(const GeGpuDrawDescriptor &) noexcept { return false; }
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &, float, float) noexcept { }
void ge_gpu_backend_observe_camera(const std::array<float, 12> &, const std::array<float, 16> &, const std::array<float, 6> &, const std::array<float, 3> &, const GeGpuDrawDescriptor &, std::uint32_t) noexcept { }
std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept { return 0u; }
void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &) noexcept { }
bool ge_gpu_backend_presents_directly() noexcept { return false; }
void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &) noexcept { }
bool ge_gpu_backend_stage_vertices(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex>) noexcept { return false; }
bool ge_gpu_backend_texture_available(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_texture_needed(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_texture_signature_needed(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_transfer_ready() noexcept { return false; }
bool ge_gpu_backend_upload_decoded_texture(const GeGpuDrawDescriptor &, std::uint32_t, std::uint32_t, std::span<const std::byte>) noexcept { return false; }
bool ge_gpu_backend_upload_decoded_texture_chain_packed(const GeGpuDrawDescriptor &, std::uint32_t, std::uint32_t, std::uint32_t, std::vector<std::byte>) noexcept { return false; }
GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(const GeGpuDrawDescriptor &) noexcept { return {}; }

} // namespace psprecomp::hle::gpu
