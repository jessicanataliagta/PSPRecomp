#pragma once

// Data exchanged between the shared GE software renderer and an optional GPU
// backend, plus the backend entry points the renderer calls. A profile that
// has a GPU backend defines the psprecomp::hle::gpu functions (VCS forwards
// them to its DX12 backend); one without links psprecomp_hle_gpu_null.

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace psprecomp::hle {

enum class GeGpuBackendKind : std::uint8_t {
    Software,
    DirectX12,
};

struct GeGpuDrawDescriptor {
    std::uint32_t primitive{};
    std::uint32_t vertex_count{};
    std::uint32_t vertex_type{};
    // GE render target for this draw. Stage 33 uses it to exclude prelight,
    // reflection and other offscreen passes from the presented framebuffer.
    std::uint32_t framebuffer_address{};
    std::uint32_t framebuffer_stride{};
    std::uint32_t framebuffer_format{};
    std::uint32_t texture_format{};
    std::uint32_t texture_address{};
    std::uint32_t texture_buffer_width{};
    std::uint32_t texture_width{};
    std::uint32_t texture_height{};
    std::uint32_t texture_function{};
    bool texture_use_alpha{};
    bool texture_double_color{};
    std::uint32_t texture_env{};
    std::uint32_t clut_address{};
    std::uint32_t clut_format{};
    std::uint32_t clut_shift{};
    std::uint32_t clut_mask{};
    std::uint32_t clut_start{};
    // Checksum of the palette bytes themselves. The cache keys carry the CLUT
    // *address*, and VCS recolours vehicles by loading a new palette into the
    // same address: two cars of different colours collided on one key, so
    // whichever decoded first decided the paint and the colour flipped as the
    // cache refreshed or evicted. That is the blinking bodywork, and the reason
    // the software rasterizer -- which decodes the palette on every draw and
    // caches nothing -- never showed it.
    std::uint32_t clut_checksum{};
    // Lightweight signature of the source texel bytes. It is deliberately NOT
    // part of the cache key: when a PSP title streams new pixels into the same
    // address/layout, the existing VkImage must be refreshed rather than minting
    // a permanently new cache identity.
    std::uint64_t texture_content_signature{};
    // Stage 42 host-only derived keys. The PSP state above is hashed once after
    // the per-draw CLUT checksum is known; texture lookup/signature/upload and
    // accumulation then reuse the key instead of re-hashing 8 mip descriptors
    // and sampler state several times for the same PRIM. Zero means "compute".
    std::uint64_t texture_cache_key_hint{};
    std::uint64_t texture_image_key_hint{};
    bool texture_swizzled{};
    // Full PSP GE filter/mipmap state. texture_linear remains the legacy alias
    // for magnification filtering so older probes and packages stay source-compatible.
    bool texture_linear{};
    bool texture_min_linear{};
    bool texture_mag_linear{};
    bool texture_mipmap_enabled{};
    bool texture_mipmap_linear{};
    std::uint32_t texture_max_level{};
    std::uint32_t texture_level_mode{};
    std::int32_t texture_level_offset16{};
    float texture_lod_slope{};
    std::uint32_t texture_selected_level{};
    std::array<std::uint32_t, 8> texture_level_addresses{};
    std::array<std::uint32_t, 8> texture_level_buffer_widths{};
    std::array<std::uint32_t, 8> texture_level_widths{};
    std::array<std::uint32_t, 8> texture_level_heights{};
    bool texture_clamp_u{};
    bool texture_clamp_v{};
    // GE scissor rectangle, in PSP 480x272 screen space, inclusive on both ends.
    // The Vulkan path used to scissor to the whole target, so every 2D element
    // the game clips with this register -- the radar map above all -- spilled
    // across the screen.
    std::int32_t scissor_x0{};
    std::int32_t scissor_y0{};
    std::int32_t scissor_x1{479};
    std::int32_t scissor_y1{271};
    bool through{};
    // Set by the GE renderer when it has already shrunk this draw's vertices
    // back into the 16:9 box. The backend reads it to move the scissor with
    // them. See ge_gpu_backend_widescreen_hud().
    bool widescreen_hud{};
    bool texture_enabled{};
    bool blend_enabled{};
    std::uint32_t blend_equation{};
    std::uint32_t blend_source_factor{};
    std::uint32_t blend_dest_factor{};
    std::uint32_t blend_fix_source{};
    std::uint32_t blend_fix_dest{};
    std::uint32_t color_write_mask{};
    bool alpha_test_enabled{};
    std::uint32_t alpha_function{};
    std::uint32_t alpha_reference{};
    std::uint32_t alpha_mask{};
    bool depth_test_enabled{};
    bool depth_write_enabled{};
    std::uint32_t depth_function{};
    bool fog_enabled{};
    std::uint32_t fog_color{};
    float fog_end{};
    float fog_slope{};
    bool clear_mode{};
    bool clear_color{};
    bool clear_alpha{};
    bool clear_depth{};
};

// Stable host-side vertex format for the first Vulkan bring-up path.  Values
// are the already decoded GE coordinates/colors produced by ge_renderer.cpp;
// no guest-memory format assumptions leak into the backend.
// Per-draw hardware-transform state for the Vulkan geometry frontend.
// PSP vertex format decode stays on the CPU in this hardware-transform path,
// but does not materialize clipped/screen-space
// triangles. The backend folds viewport/composition into model_to_clip once per
// draw and pushes the final constants to the vertex shader.
struct GeGpuHardwareTransform {
    std::array<float, 16> model_to_clip{};
    std::array<float, 4> model_to_view_z{};
    float viewport_scale_x{};
    float viewport_scale_y{};
    float viewport_scale_z{};
    float viewport_center_x{};
    float viewport_center_y{};
    float viewport_center_z{};
    float viewport_offset_x{};
    float viewport_offset_y{};
    float uv_scale_u{1.0f};
    float uv_scale_v{1.0f};
    float uv_offset_u{};
    float uv_offset_v{};
    float fog_end{};
    float fog_slope{};
    bool depth_clip_enabled{};
    bool cull_enabled{};
    bool accept_counter_clockwise{};
    // Stage 45.5: preserve the PSP primitive topology on the native GPU path.
    // 3 = triangle list, 4 = triangle strip, 5 = fan (fan is expanded).
    std::uint32_t primitive{3u};
    // Optional affine vertex-lighting transform. For the dominant 0x0115
    // layout the normal is constant per draw; when every enabled light is
    // directional the complete PSP lighting equation is affine in vertex
    // colour. Applying it in the VS keeps the original 10-byte stream on GPU.
    bool vertex_color_affine{};
    std::array<float, 4> vertex_color_mul{1.0f, 1.0f, 1.0f, 1.0f};
    std::array<float, 4> vertex_color_add{};
    std::uint32_t logical_prim_batches{1u};
    std::uint32_t unique_vertices_decoded{};
    std::uint32_t index_reuses{};
};

struct GeGpuVertex {
    float x{};
    float y{};
    float z{};
    float w{1.0f};
    std::uint32_t rgba{0xFFFFFFFFu};
    float u{};
    float v{};
    // Packed as R8G8B8A8_UINT: enable, function, reference, mask.
    std::uint32_t alpha_control{0xFF000100u};
    // Packed as R8G8B8A8_UINT: function, use-alpha, double-color, reserved.
    std::uint32_t texture_control{};
    // Packed as R8G8B8A8_UINT. RGB is the PSP GE texture-environment color;
    // A is reserved. Required by GU_TFX_BLEND and carried flat per primitive.
    std::uint32_t texture_env{};
    // Perspective-interpolated PSP fog coefficient in [0, 1]. 1 preserves the
    // fragment color; 0 selects the flat fog color.
    float fog_factor{1.0f};
    // Packed R8G8B8A8_UINT: RGB fog color, A nonzero when fog is enabled.
    std::uint32_t fog_control{};
    // PSP projective texture denominator. The hardware interpolates U, V and Q
    // perspective-correctly, then the fragment stage samples at (U/Q, V/Q).
    float q{1.0f};
    // bit 0 = model-space/HW transform, bit 1 = culling enabled,
    // bit 2 = PSP accepts counter-clockwise faces, bit 3 = depth clip enabled.
    std::uint32_t transform_control{};
};

struct GeGpuDecodedMipLevel {
    std::uint32_t width{};
    std::uint32_t height{};
    std::span<const std::byte> rgba8{};
};

struct GeGpuBackendReport {
    GeGpuBackendKind requested{GeGpuBackendKind::Software};
    GeGpuBackendKind active{GeGpuBackendKind::Software};
    bool strict{};
    bool loader_opened{};
    bool instance_created{};
    bool device_created{};
    bool transfer_buffer_created{};
    bool transfer_memory_mapped{};
    bool command_pool_created{};
    bool transfer_self_test_passed{};
    bool offscreen_image_created{};
    bool offscreen_image_memory_bound{};
    bool offscreen_image_view_created{};
    bool render_pass_created{};
    bool framebuffer_created{};
    bool shader_modules_created{};
    bool graphics_pipeline_created{};
    bool offscreen_self_test_passed{};
    std::uint32_t physical_device_count{};
    std::uint32_t graphics_queue_family{0xFFFFFFFFu};
    std::uint32_t memory_type_index{0xFFFFFFFFu};
    std::uint64_t draw_calls{};
    std::uint64_t vertices{};
    std::uint64_t textured_draw_calls{};
    std::uint64_t unique_pipeline_keys{};
    std::uint64_t unique_texture_keys{};
    // Distinct decoded images behind those keys. The gap between the two is the
    // share of the texture cache that is duplicate art differing only in sampler
    // state (filtering, wrap, LOD).
    std::uint64_t unique_texture_image_keys{};
    // Cache entries that adopted an image another entry had already decoded and
    // uploaded, instead of creating and filling a duplicate.
    std::uint64_t shared_texture_images{};
    std::uint64_t captured_draws{};
    std::uint64_t draw_ring_capacity{};
    std::uint64_t draw_ring_overwrites{};
    std::uint64_t upload_capacity_bytes{};
    // Complete host frame resource sets available for CPU/GPU overlap.
    std::uint32_t frames_in_flight_capacity{1u};
    std::uint64_t staged_draw_calls{};
    std::uint64_t staged_vertices{};
    std::uint64_t staged_bytes{};
    std::uint64_t upload_wraps{};
    std::uint64_t transfer_submissions{};
    std::uint64_t transfer_bytes{};
    // Performance diagnostics only. These counters are populated only when
    // PSPRECOMP_GPU_TIMING_DIAG is set; they do not change renderer policy.
    std::uint64_t perf_wait_for_frame_calls{};
    std::uint64_t perf_wait_for_frame_ns{};
    std::uint64_t perf_upload_flush_wait_calls{};
    std::uint64_t perf_upload_flush_wait_ns{};
    std::uint64_t perf_acquire_calls{};
    std::uint64_t perf_acquire_ns{};
    std::uint64_t perf_queue_submit_calls{};
    std::uint64_t perf_queue_submit_ns{};
    std::uint64_t perf_queue_present_calls{};
    std::uint64_t perf_queue_present_ns{};
    std::uint64_t perf_finish_frame_calls{};
    std::uint64_t perf_finish_frame_ns{};
    std::uint64_t rejected_gpu_draws{};
    std::uint32_t offscreen_width{};
    std::uint32_t offscreen_height{};
    std::uint32_t offscreen_center_rgba{};
    std::uint64_t offscreen_draw_calls{};
    std::uint64_t offscreen_readback_bytes{};
    std::uint64_t offscreen_changed_pixels{};
    std::uint64_t offscreen_checksum{};
    // Stage 24: actual VCS screen-space triangles rasterized into the configured
    // internal-resolution Vulkan target. Textures/depth are not authoritative
    // yet, so this remains a diagnostic color-geometry preview.
    std::uint64_t game_frames{};
    std::uint64_t game_draw_calls{};
    std::uint64_t game_triangles{};
    std::uint64_t game_vertices{};
    std::uint64_t hw_transform_draw_calls{};
    std::uint64_t hw_transform_vertices{};
    std::uint64_t hw_transform_prim_batches{};
    std::uint64_t hw_transform_unique_vertices_decoded{};
    std::uint64_t hw_transform_index_reuses{};
    std::uint64_t game_textured_draws_without_texture{};
    std::uint64_t game_vertex_overflows{};
    std::uint64_t game_frame_readback_bytes{};
    std::uint64_t game_frame_changed_pixels{};
    std::uint64_t game_frame_checksum{};
    std::uint64_t game_frame_vblank{};
    // Stage 25: exact CPU decode of PSP indexed textures before descriptor-backed
    // sampling lands. T4/T8 + CLUT data are cached as RGBA8 and staged into the
    // persistent upload ring so the next Vulkan stage can create images without
    // touching guest memory again.
    std::uint64_t texture_decode_requests{};
    std::uint64_t texture_cache_hits{};
    std::uint64_t decoded_texture_uploads{};
    std::uint64_t decoded_texture_bytes{};
    std::uint64_t decoded_t4_textures{};
    std::uint64_t decoded_t8_textures{};
    std::uint64_t texture_images_created{};
    std::uint64_t texture_image_uploads{};
    std::uint64_t texture_image_upload_bytes{};
    // Stage 26: descriptor-backed sampling of the Stage 25 RGBA8 VkImages.
    bool texture_descriptor_layout_created{};
    bool texture_descriptor_pool_created{};
    bool textured_shader_modules_created{};
    bool textured_pipeline_created{};
    // Stage 27: D16 depth attachment plus PSP GE compare/write state.
    bool depth_image_created{};
    bool depth_image_memory_bound{};
    bool depth_image_view_created{};
    bool depth_attachment_active{};
    std::uint64_t depth_pipeline_variants_created{};
    std::uint64_t depth_tested_game_draw_calls{};
    std::uint64_t depth_writing_game_draw_calls{};
    // Stage 28: exact PSP GE alpha compare in color and textured fragments.
    bool alpha_test_shader_active{};
    std::uint64_t alpha_tested_game_draw_calls{};
    // Stage 29: dominant PSP ADD/SRC_ALPHA/ONE_MINUS_SRC_ALPHA blend path.
    bool standard_alpha_blend_pipeline_active{};
    bool observed_blend_modes_pipeline_active{};
    std::uint64_t blend_pipeline_variants_created{};
    std::uint64_t standard_alpha_blended_game_draw_calls{};
    std::uint64_t fixed_replace_blended_game_draw_calls{};
    std::uint64_t additive_blended_game_draw_calls{};
    std::uint64_t unsupported_blend_game_draw_calls{};
    // Stage 31: complete PSP texture-function switch (MODULATE, DECAL, BLEND,
    // REPLACE and ADD), texture environment color, all uncompressed base
    // texture formats 0..7, and whole-channel color write masks.
    bool observed_texture_function_shader_active{};
    bool complete_texture_function_shader_active{};
    bool color_write_mask_pipeline_active{};
    bool base_texture_formats_active{};
    std::uint64_t modulate_texture_game_draw_calls{};
    std::uint64_t decal_texture_game_draw_calls{};
    std::uint64_t blend_texture_game_draw_calls{};
    std::uint64_t replace_texture_game_draw_calls{};
    std::uint64_t add_texture_game_draw_calls{};
    std::uint64_t double_color_texture_game_draw_calls{};
    std::uint64_t unsupported_texture_function_game_draw_calls{};
    std::uint64_t color_mask_pipeline_variants_created{};
    std::uint64_t masked_color_game_draw_calls{};
    std::uint64_t unsupported_partial_color_mask_game_draw_calls{};
    std::uint64_t decoded_direct16_textures{};
    std::uint64_t decoded_direct32_textures{};
    std::uint64_t decoded_indexed16_textures{};
    std::uint64_t decoded_indexed32_textures{};
    // Stage 32: PSP S3TC, explicit mip-level/filter state and per-fragment fog.
    bool compressed_texture_formats_active{};
    bool mipmap_state_active{};
    bool fog_shader_active{};
    std::uint64_t decoded_dxt1_textures{};
    std::uint64_t decoded_dxt3_textures{};
    std::uint64_t decoded_dxt5_textures{};
    std::uint64_t mipmapped_game_draw_calls{};
    std::uint64_t mip_linear_game_draw_calls{};
    std::uint64_t fixed_lod_game_draw_calls{};
    std::uint64_t selected_nonzero_mip_game_draw_calls{};
    bool full_mip_chain_active{};
    std::uint64_t uploaded_mip_levels{};
    std::uint64_t automatic_lod_game_draw_calls{};
    std::uint64_t slope_lod_game_draw_calls{};
    std::uint64_t fogged_game_draw_calls{};
    // Stage 33: persistent framebuffer-feedback bridge and display-target
    // selection for VCS's delayed prelight/bloom/final-composition chain.
    std::uint64_t framebuffer_targets_observed{};
    // Draws that sample the surface the guest is currently displaying.  This is
    // the empirical test for PSPRECOMP_GE_GPU_SKIP_DISPLAYED_RASTER: skipping
    // the CPU rasterization of that surface is only safe if nothing reads it
    // back, and a nonzero count here means something does.
    std::uint64_t display_framebuffer_sampled_draws{};
    // LRU eviction. evicted_textures rising while unbound_textured_draws stays
    // at zero is the cache working; unbound_textured_draws rising again means
    // the working set no longer fits and the ceilings need raising.
    std::uint64_t evicted_textures{};
    std::uint64_t recycled_texture_descriptor_sets{};
    std::uint64_t vram_feedback_refreshes{};
    // Stage 44.5 DX12: persistent EDRAM framebuffer surfaces live as native
    // D3D12 render-target/SRV resources. Feedback draws sample them GPU->GPU;
    // same-target feedback snapshots the pre-draw image on the GPU to avoid
    // the D3D12 RTV+SRV hazard without a CPU readback.
    std::uint64_t dx12_native_framebuffer_targets{};
    std::uint64_t dx12_gpu_feedback_draws{};
    std::uint64_t dx12_self_feedback_snapshots{};
    // Stage 44.6 Direct3D 12 release-candidate diagnostics.
    std::uint32_t dx12_msaa_samples{1u};
    std::uint32_t dx12_depth_bits{32u};
    std::uint64_t dx12_resolves{};
    std::uint64_t dx12_device_recoveries{};
    std::uint32_t presented_framebuffer_target{};
    // True once the rendered image goes straight to a native GPU swapchain instead
    // of being copied back to system memory and blitted with GDI.
    bool swapchain_active{};
    // Vblanks whose batches never touched the displayed framebuffer and were
    // therefore handed back to the software path instead of being presented.
    std::uint64_t frames_without_displayed_target{};
    bool gpu_frame_presented_to_window{};
    bool release_candidate_ready{};
    std::uint64_t texture_samplers_created{};
    std::uint64_t texture_descriptor_sets_allocated{};
    std::uint64_t textured_game_draw_calls{};
    std::uint64_t textured_game_triangles{};
    std::uint64_t textured_game_vertices{};
    std::uint64_t missing_texture_draw_calls{};
    std::uint64_t rejected_texture_decodes{};
    std::uint64_t texture_upload_wraps{};
    std::uint64_t last_texture_key{};
    std::uint64_t last_texture_checksum{};
    std::uint32_t last_texture_width{};
    std::uint32_t last_texture_height{};
    std::uint32_t last_texture_format{};
    std::string message;
};

struct GeGpuWidescreenHud {
    // Divide distances from source_center by this. 1 means the correction is
    // off, and every other field should be ignored.
    float shrink{1.0f};
    // Middle of the display, in this target's coordinates.
    float source_center{240.0f};
    // Target-to-display scale, so a caller can test a span against the real
    // 480 px screen width without knowing the target's size.
    float display_scale_x{1.0f};
};

namespace gpu {

void ge_gpu_backend_accumulate_color_triangles(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuVertex> triangle_vertices) noexcept;
[[nodiscard]] bool ge_gpu_backend_accumulate_hardware_packed_0115(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const std::byte> packed_vertices,
    std::uint32_t vertex_count,
    std::span<const std::uint32_t> triangle_indices) noexcept;
void ge_gpu_backend_accumulate_hardware_triangles(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const GeGpuVertex> vertices,
    std::span<const std::uint32_t> triangle_indices) noexcept;
[[nodiscard]] bool ge_gpu_backend_active() noexcept;
[[nodiscard]] bool ge_gpu_backend_adopt_shared_texture(
    const GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] std::uint32_t ge_gpu_backend_display_framebuffer() noexcept;
[[nodiscard]] bool ge_gpu_backend_graphics_ready() noexcept;
[[nodiscard]] bool ge_gpu_backend_is_framebuffer_feedback_texture(
    const GeGpuDrawDescriptor &draw) noexcept;
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &draw,
                                        float max_x, float max_y) noexcept;
void ge_gpu_backend_observe_camera(const std::array<float, 12> &view,
                                   const std::array<float, 16> &projection,
                                   const std::array<float, 6> &viewport,
                                   const std::array<float, 3> &camera_position,
                                   const GeGpuDrawDescriptor &draw,
                                   std::uint32_t vertex_weight) noexcept;
[[nodiscard]] std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept;
void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_presents_directly() noexcept;
void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_stage_vertices(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuVertex> vertices) noexcept;
[[nodiscard]] bool ge_gpu_backend_texture_available(
    const GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_texture_needed(
    const GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_texture_signature_needed(
    const GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_transfer_ready() noexcept;
[[nodiscard]] bool ge_gpu_backend_upload_decoded_texture(
    const GeGpuDrawDescriptor &draw,
    std::uint32_t width, std::uint32_t height,
    std::span<const std::byte> rgba8) noexcept;
[[nodiscard]] bool ge_gpu_backend_upload_decoded_texture_chain_packed(
    const GeGpuDrawDescriptor &draw,
    std::uint32_t base_width, std::uint32_t base_height,
    std::uint32_t mip_levels, std::vector<std::byte> rgba8) noexcept;
[[nodiscard]] GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(
    const GeGpuDrawDescriptor &draw) noexcept;

} // namespace gpu
} // namespace psprecomp::hle
