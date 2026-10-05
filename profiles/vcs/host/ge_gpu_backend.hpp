#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "psprecomp/hle/ge_gpu.hpp"

namespace vcs {

// Shared with the GE software renderer (psprecomp/hle/ge_gpu.hpp).
using psprecomp::hle::GeGpuBackendKind;
using psprecomp::hle::GeGpuDrawDescriptor;
using psprecomp::hle::GeGpuHardwareTransform;
using psprecomp::hle::GeGpuVertex;
using psprecomp::hle::GeGpuDecodedMipLevel;
using psprecomp::hle::GeGpuBackendReport;
using psprecomp::hle::GeGpuWidescreenHud;


// Initializes the optional persistent GPU prototype. The default remains the
// bit-exact software rasterizer. Set PSPRECOMP_GE_BACKEND=vulkan to create a
// persistent Vulkan instance/device and transfer/upload path while software
// rendering remains authoritative during bring-up.
[[nodiscard]] bool initialize_ge_gpu_backend(std::string &error);
void shutdown_ge_gpu_backend() noexcept;

[[nodiscard]] bool ge_gpu_backend_active() noexcept;
[[nodiscard]] bool ge_gpu_backend_transfer_ready() noexcept;
[[nodiscard]] bool ge_gpu_backend_graphics_ready() noexcept;
void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &draw) noexcept;

// Captures the dominant projected world camera for optional native post effects.
// The matrices are observed with the draw, before the GE state advances.
void ge_gpu_backend_observe_camera(const std::array<float, 12> &view,
                                   const std::array<float, 16> &projection,
                                   const std::array<float, 6> &viewport,
                                   const std::array<float, 3> &camera_position,
                                   const GeGpuDrawDescriptor &draw,
                                   std::uint32_t vertex_weight) noexcept;

// Stages already-decoded vertices into a persistently mapped Vulkan buffer.
// This does not replace the software rasterizer yet; it proves that real VCS
// geometry can cross the host->Vulkan boundary with no per-draw allocation.
[[nodiscard]] bool ge_gpu_backend_stage_vertices(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuVertex> vertices) noexcept;

// Returns true when the Vulkan-side texture cache does not yet contain the
// complete PSP texture/CLUT state represented by this draw.
[[nodiscard]] bool ge_gpu_backend_texture_needed(
    const GeGpuDrawDescriptor &draw) noexcept;

// Content hashing is required only once per cache entry per display interval.
// Repeated draws of the same immutable texture in one vblank can reuse the
// signature already validated by the first draw. Framebuffer feedback remains
// conservative and always requests a fresh signature.
void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &draw) noexcept;
[[nodiscard]] bool ge_gpu_backend_texture_signature_needed(
    const GeGpuDrawDescriptor &draw) noexcept;

// True when the sampled texture aliases a render target observed by the GE.
// Renderer-side upload code uses this to force framebuffer feedback to one mip
// level; generated render targets do not have a valid PSP mip chain behind them.
[[nodiscard]] bool ge_gpu_backend_is_framebuffer_feedback_texture(
    const GeGpuDrawDescriptor &draw) noexcept;

// Widescreen interface correction, expressed in the coordinates the GE renderer
// actually has: the draw's own render target, before the game's composition
// pass shrinks it into the 480x272 display.
//
// The correction has to be applied once, in the renderer, and not here. VCS
// renders on a two-vblank cycle: one vblank draws the interface, the next only
// replays 64 composition quads that copy an already-finished surface. The
// software rasterizer keeps filling guest VRAM for targets the GPU does not
// own, so correcting only the Vulkan vertices left two copies of the HUD --
// corrected on drawing vblanks, stretched on composition vblanks, alternating
// at 30 Hz on the loading screen.
[[nodiscard]] GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(
    const GeGpuDrawDescriptor &draw) noexcept;

// Records a through-mode draw's reach *before* the widescreen correction moves
// it. Whether a target counts as oversized is decided from how far its draws
// go, so feeding it shrunk coordinates would let the correction argue the
// composition mapping it depends on out of existence.
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &draw,
                                        float max_x, float max_y) noexcept;

// Creates the cache entry for this draw out of an image another entry already
// decoded, when the two differ only in sampler state. Returns false when there
// is no such image and the caller must decode.
//
// The cache key includes filtering, wrap mode and LOD, none of which change the
// decoded pixels, so the same art appears under many keys -- measured at 17
// keys per distinct image. Without this the duplicates are decoded from guest
// memory every time even though the result is thrown away.
[[nodiscard]] bool ge_gpu_backend_adopt_shared_texture(
    const GeGpuDrawDescriptor &draw) noexcept;

// Returns true only after the complete texture state has a sampled VkImage,
// sampler and descriptor set ready for Stage 26 draw submission.
[[nodiscard]] bool ge_gpu_backend_texture_available(
    const GeGpuDrawDescriptor &draw) noexcept;

// Stores one exact native-resolution RGBA8 decode of a T4/T8 PSP texture in the
// persistent cache and stages the bytes through the mapped Vulkan upload ring.
// This stage does not sample the image in the fragment shader yet.
[[nodiscard]] bool ge_gpu_backend_upload_decoded_texture(
    const GeGpuDrawDescriptor &draw,
    std::uint32_t width, std::uint32_t height,
    std::span<const std::byte> rgba8) noexcept;

[[nodiscard]] bool ge_gpu_backend_upload_decoded_texture_chain(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuDecodedMipLevel> levels) noexcept;

// Stage 40 fast path: the renderer can decode a complete mip chain directly
// into one contiguous allocation and transfer ownership here. This avoids the
// old vector-per-mip allocation followed by a second full-size pack/copy.
[[nodiscard]] bool ge_gpu_backend_upload_decoded_texture_chain_packed(
    const GeGpuDrawDescriptor &draw,
    std::uint32_t base_width, std::uint32_t base_height,
    std::uint32_t mip_levels, std::vector<std::byte> rgba8) noexcept;

// Copies the most recently accepted decoded texture for proof/debug tooling.
[[nodiscard]] bool ge_gpu_backend_copy_last_texture_rgba(
    std::span<std::byte> destination) noexcept;

// Adds already-clipped, screen-space triangle-list vertices to the current
// high-resolution Vulkan frame. Coordinates are in the PSP 480x272 viewport;
// the backend maps them to the configured internal target.
void ge_gpu_backend_accumulate_color_triangles(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuVertex> triangle_vertices) noexcept;

// Model-space triangle-list submission used by Stage 39.
void ge_gpu_backend_accumulate_hardware_triangles(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const GeGpuVertex> vertices,
    std::span<const std::uint32_t> triangle_indices) noexcept;

// Stage 45.4 DX12 fast path for the dominant VCS world vertex format
// (vtype 0x000115: u8 UV + 5551 colour + s16 XYZ, 10-byte stride).
// The renderer snapshots the untouched PSP bytes and lets the DX12 vertex
// shader decode them, removing the float/color conversion loop from the CPU.
// Returns false on backends that do not implement this exact packed format so
// the caller can fall back to ge_gpu_backend_accumulate_hardware_triangles().
[[nodiscard]] bool ge_gpu_backend_accumulate_hardware_packed_0115(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const std::byte> packed_vertices,
    std::uint32_t vertex_count,
    std::span<const std::uint32_t> triangle_indices) noexcept;

// Stage 45.5 broad raw-vertex GPU decode. The source span contains a compact
// contiguous range of original PSP records described by `layout`. `indices`
// are local indices into that compact range; an empty span means non-indexed.
// Supplies the native host window used by a backend-owned swapchain. Passing
// nullptr detaches it. DX12 uses this to present the native GE render target
// directly without routing pixels back through the CPU presenter.
void ge_gpu_backend_set_native_window(void *native_window) noexcept;

// Publishes the framebuffer address the guest is currently displaying
// (sceDisplaySetFrameBuf). The frame assembler needs it to tell the displayed
// surface apart from the offscreen render targets VCS also draws into, which
// otherwise alternate into the window as unrelated images.
void ge_gpu_backend_set_display_framebuffer(std::uint32_t address) noexcept;

// Finishes one real VCS GPU preview frame at vblank, submits it once, waits for
// completion and stores an RGBA8 readback. Returns true when a new frame exists.
[[nodiscard]] bool ge_gpu_backend_finish_color_frame(std::uint64_t vblank) noexcept;

[[nodiscard]] bool ge_gpu_backend_copy_game_frame_rgba(
    std::span<std::byte> destination) noexcept;

// True when finished frames are presented by the backend itself through a
// native GPU swapchain. The caller must not present the window in that case.
[[nodiscard]] bool ge_gpu_backend_presents_directly() noexcept;

// Framebuffer address (masked to VRAM) whose image the GPU rendered and put on
// screen last, or 0 when the GPU owns nothing. Rasterizing that surface again
// on the CPU produces pixels no one reads.
[[nodiscard]] std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept;

// Framebuffer address (masked to VRAM) the guest last handed to
// sceDisplaySetFrameBuf, or 0 when the swapchain is not presenting. While the
// backend presents its own image this surface never reaches the screen, so the
// composition VCS rasterizes into it is only needed by whatever samples it back.
[[nodiscard]] std::uint32_t ge_gpu_backend_display_framebuffer() noexcept;

// Borrowed view of the last readback, valid until the next finished frame.
// Presenting through this avoids allocating and copying a desktop-sized RGBA
// buffer on every vblank.
[[nodiscard]] std::span<const std::byte> ge_gpu_backend_game_frame_rgba() noexcept;

// Copies the cached 64x64 RGBA8 offscreen self-test result. This is intended
// for validation/proof tooling; game output still comes from the software GE.
[[nodiscard]] bool ge_gpu_backend_copy_offscreen_rgba(
    std::span<std::byte> destination) noexcept;

void ge_gpu_backend_mark_window_presented() noexcept;

[[nodiscard]] GeGpuBackendReport ge_gpu_backend_report();
[[nodiscard]] const char *ge_gpu_backend_name(GeGpuBackendKind kind) noexcept;

} // namespace vcs
