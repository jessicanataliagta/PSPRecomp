#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

namespace psprecomp {
class Runtime;
class GuestMemory;
}

namespace psprecomp::hle {
struct GeGpuDrawDescriptor;
}

namespace vcs {
using psprecomp::hle::GeGpuDrawDescriptor;

// Installs the complete VCS Project2DFX host port. Must be called after
// register_generated_functions() so the optional guest-address hooks can replace
// generated AOT labels without modifying generated_unit_*.cpp.
void install_project2dfx(psprecomp::Runtime &runtime,
                         const std::filesystem::path &ini_path,
                         std::uint32_t guest_scratch_base);

// Cheap Stage 43 fast-path check used before decoding viewport registers.
// Returns true when the draw was consumed by the current camera snapshot (or
// Project2DFX is irrelevant for this draw), false when a full camera observation
// is required.
[[nodiscard]] bool project2dfx_observe_camera_hot(
    const GeGpuDrawDescriptor &draw,
    std::uint32_t vertex_weight,
    std::uint64_t camera_state_revision) noexcept;

// Called by ge_renderer for projected world draws. Project2DFX selects the
// dominant world render target/camera for the current vblank and draws its
// native overlay into that same target before the Vulkan frame is finalized.
void project2dfx_observe_camera(
    const std::array<float, 12> &view,
    const std::array<float, 16> &projection,
    float viewport_scale_x, float viewport_scale_y, float viewport_scale_z,
    float viewport_center_x, float viewport_center_y, float viewport_center_z,
    float viewport_offset_x, float viewport_offset_y,
    float clip_x_scale,
    const GeGpuDrawDescriptor &draw,
    std::uint32_t vertex_weight,
    std::uint64_t camera_state_revision = 0u) noexcept;

// Called once per display vblank immediately before ge_gpu_backend_finish_color_frame().
void project2dfx_render_frame(psprecomp::GuestMemory &memory,
                              std::uint32_t guest_gp,
                              std::uint64_t vblank,
                              std::uint32_t display_framebuffer) noexcept;

} // namespace vcs
