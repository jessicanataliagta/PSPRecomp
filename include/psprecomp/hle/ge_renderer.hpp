#pragma once

// Shared PSP GE software rasterizer: executes PRIM and BBOX for the GE
// interpreter (pass render_ge_primitive / test_ge_bounding_box as the
// GeRenderer), writing into guest VRAM, with an optional GPU backend
// (psprecomp/hle/ge_gpu.hpp) taking over draws it owns.

#include "psprecomp/guest_memory.hpp"
#include "psprecomp/hle/ge.hpp"
#include "psprecomp/hle/ge_gpu.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace psprecomp::hle {

struct GeRendererHooks {
    // GPU hardware-transform frontend (PSPRECOMP_GE_GPU_HW_TRANSFORM overrides).
    bool (*hardware_transform_enabled)() = nullptr;
    // The GPU backend owns colour output, so software rasterization of every
    // target may be skipped (PSPRECOMP_GE_GPU_SKIP_SOFTWARE_RASTER overrides).
    bool (*gpu_color_authoritative)() = nullptr;
    // Called for each draw recorded with an active GPU backend, after
    // gpu::ge_gpu_backend_record_draw: title-specific camera/overlay taps.
    void (*observe_gpu_draw)(const GuestMemory &memory, const std::array<std::uint32_t, 256> &commands,
                             const GeTransformState &transform, const GeGpuDrawDescriptor &draw,
                             std::uint32_t primitive, std::uint32_t count,
                             std::uint64_t camera_state_revision) = nullptr;
    // Face culling on the GPU for hardware-transform draws (off by default,
    // see gpu_hardware_cull_enabled(); PSPRECOMP_GE_GPU_HW_CULL overrides).
    // Without it those draws keep their back faces, which shows wherever a
    // game relies on culling (CTW's cloud layer).
    bool (*hardware_cull_enabled)() = nullptr;
};

// Read lazily on first use; set before the first draw.
extern GeRendererHooks g_ge_renderer_hooks;

// Executes the PSP GE BBOX visibility test conservatively. A false result means
// every control point lies outside at least one common clip plane, so BJUMP may
// safely skip the following draw block. The vertex/index streams advance with
// the same rules as PRIM/BBOX on the PSP.
bool test_ge_bounding_box(const GuestMemory &memory,
                          const std::array<std::uint32_t, 256> &commands,
                          const GeTransformState &transform,
                          std::uint32_t vertex_address,
                          std::uint32_t index_address,
                          std::uint32_t count,
                          GeBoundingBoxResult &result,
                          std::string &error);

// Executes one GE PRIM command using the supplied command-memory snapshot and
// persistent matrix state. Returns false only when the guest stream is malformed.
// Vertex formats not implemented by this checkpoint are skipped while still
// advancing the GE stream, so display-list execution remains synchronized.
bool render_ge_primitive(GuestMemory &memory,
                         const std::array<std::uint32_t, 256> &commands,
                         const GeTransformState &transform,
                         std::uint32_t vertex_address,
                         std::uint32_t index_address,
                         std::uint32_t primitive_data,
                         GeRenderStats &stats,
                         std::string &error,
                         std::uint32_t logical_primitive_count = 1u,
                         std::uint64_t draw_state_revision = 0u,
                         std::uint64_t camera_state_revision = 0u,
                         std::uint64_t lighting_state_revision = 0u,
                         bool collect_diagnostic_stats = true);

// Time spent inside the per-fragment pixel loop, and the triangles that reached
// it, since the last reset.  Only accumulated when PSPRECOMP_GE_PHASE_DIAG is
// set.  The rest of ge_us is per-triangle geometry: clipping, viewport
// transform, culling and bounding box.
// Stage 35 adds a breakdown of what the old report lumped into "geometry".  On
// the Vulkan path the pixel loop is skipped for the framebuffer the GPU owns, so
// "geometry" became almost the whole heavy frame without saying which part of it
// -- per-draw register decoding, per-vertex transform, clipping, texture upload
// or the hand-off to Vulkan -- was responsible.  Every counter below is taken
// once per draw call, never per vertex: a clock read costs about as much as
// transforming a vertex, so per-vertex timing would measure itself.
struct GePhaseTotals {
    std::uint64_t pixel_loop_ns{};
    std::uint64_t triangles{};
    std::uint64_t draw_setup_ns{};      // GE register decode + fragment setup, per draw
    std::uint64_t texture_upload_ns{};  // PSP mip chain decode and Vulkan upload
    std::uint64_t vertex_decode_ns{};   // index read + decode_vertex, whole draw
    std::uint64_t gpu_stage_ns{};       // legacy Stage 22 staging into the upload ring
    std::uint64_t triangle_prep_ns{};   // clip, viewport transform, cull
    std::uint64_t gpu_accumulate_ns{};  // prepared triangles -> Vulkan frame buffer
    std::uint64_t primitives{};
    std::uint64_t vertices{};
};
[[nodiscard]] GePhaseTotals ge_phase_totals() noexcept;
void reset_ge_phase_totals() noexcept;

} // namespace psprecomp::hle
