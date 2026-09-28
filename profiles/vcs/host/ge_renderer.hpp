#pragma once

#include "psprecomp/guest_memory.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace vcs {

// Matrix state is separate from the 256 GE command registers. Matrix DATA commands
// auto-increment an internal cursor, so retaining only the latest command word loses
// almost the entire matrix. Keep the expanded state here and feed it to each PRIM.
struct GeTransformState {
    std::array<float, 96> bones{};
    std::array<float, 12> world{};
    std::array<float, 12> view{};
    std::array<float, 16> projection{};
    std::array<float, 12> texture{};
    std::array<float, 8> morph_weights{};
    std::uint32_t bone_cursor{};
    std::uint32_t world_cursor{};
    std::uint32_t view_cursor{};
    std::uint32_t projection_cursor{};
    std::uint32_t texture_cursor{};
};

void reset_ge_transform_state(GeTransformState &state) noexcept;
void update_ge_transform_state(GeTransformState &state, std::uint32_t command,
                               std::uint32_t data) noexcept;

struct GeBoundingBoxResult {
    bool visible{};
    std::uint32_t next_vertex_address{};
    std::uint32_t next_index_address{};
};

struct GeRenderStats {
    std::uint64_t primitives{};
    std::uint64_t points{};
    std::uint64_t lines{};
    std::uint64_t triangles{};
    std::uint64_t rectangles{};
    std::uint64_t skinned_vertices{};
    std::uint64_t morphed_vertices{};
    std::uint64_t lit_vertices{};
    std::uint64_t generated_uv_vertices{};
    std::uint64_t culled_triangles{};
    std::uint64_t flat_shaded_primitives{};
    std::uint64_t pixels_tested{};
    std::uint64_t pixels_written{};
    std::uint64_t unsupported_primitives{};
    std::uint64_t decoded_vertices{};
    std::uint64_t nonfinite_clip_vertices{};
    std::uint64_t screen_vertices{};
    bool has_clip_bounds{};
    bool has_screen_bounds{};
    float clip_min_x{};
    float clip_min_y{};
    float clip_min_z{};
    float clip_min_w{};
    float clip_max_x{};
    float clip_max_y{};
    float clip_max_z{};
    float clip_max_w{};
    float screen_min_x{};
    float screen_min_y{};
    float screen_max_x{};
    float screen_max_y{};
    float min_abs_w{};
    float max_abs_screen_coordinate{};
    std::uint32_t next_vertex_address{};
    std::uint32_t next_index_address{};
};


// Executes the PSP GE BBOX visibility test conservatively. A false result means
// every control point lies outside at least one common clip plane, so BJUMP may
// safely skip the following draw block. The vertex/index streams advance with
// the same rules as PRIM/BBOX on the PSP.
bool test_ge_bounding_box(const psprecomp::GuestMemory &memory,
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
bool render_ge_primitive(psprecomp::GuestMemory &memory,
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
    // Diagnostic census for the hardware-transform vertex path. These explain
    // whether vertex_decode_ns is index-stream bookkeeping, CPU conversion, or
    // draws that could not use the packed 0x0115 DX12 path.
    std::uint64_t hardware_draws{};
    std::uint64_t hardware_vertices{};
    std::uint64_t fast_0115_draws{};
    std::uint64_t fast_0115_vertices{};
    std::uint64_t packed_0115_candidates{};
    std::uint64_t packed_0115_accepted{};
    std::uint64_t packed_0115_vertices{};
    std::uint64_t cpu_decode_draws{};
    std::uint64_t cpu_decode_vertices{};
    std::uint64_t flat_shaded_draws{};
};
[[nodiscard]] GePhaseTotals ge_phase_totals() noexcept;
void reset_ge_phase_totals() noexcept;

} // namespace vcs
