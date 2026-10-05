#pragma once

#include <cstdint>

namespace psprecomp::hle {
struct GeGpuDrawDescriptor;
}

namespace vcs {

using psprecomp::hle::GeGpuDrawDescriptor;

// Records render targets touched during the current guest frame. The overlay
// is emitted only when that same frame actually touched the displayed target,
// so enabling it cannot turn VCS's intentionally repeated vblanks into new GPU
// frames.
void fps_overlay_observe_draw(const GeGpuDrawDescriptor &draw,
                              std::uint32_t vertex_weight) noexcept;

// Draws a small native 5x7 counter into the current displayed target. A full-
// speed VCS frame cadence reads 30 FPS; this is deliberately not vblank Hz.
void fps_overlay_render_frame(std::uint32_t selected_framebuffer) noexcept;

} // namespace vcs
