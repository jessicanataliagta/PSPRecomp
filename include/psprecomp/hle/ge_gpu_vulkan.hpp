#pragma once

// Vulkan GE backend. Linking psprecomp_hle_gpu_vulkan (instead of
// psprecomp_hle_gpu_null) supplies the psprecomp::hle::gpu entry points the GE
// renderer calls: every PSP draw is replayed on the GPU at an internal
// resolution of 480x272 times `scale`, render-to-texture included, and the
// displayed framebuffer is read back for the host frontend to present.
//
// Until initialize() succeeds the backend reports itself inactive and the
// software rasterizer draws everything, exactly as with the null backend.
// Whether the software rasterizer still runs alongside it is the profile's
// choice (GeRendererHooks::gpu_color_authoritative).

#include "psprecomp/hle/ge_gpu.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace psprecomp::hle::vulkan {

struct Config {
    // Internal resolution multiplier (1 = native 480x272).
    std::uint32_t scale{2u};
    // Enable VK_LAYER_KHRONOS_validation when it is installed.
    bool validation{};
};

bool initialize(const Config &config, std::string &error);
void shutdown();
[[nodiscard]] bool active();

// Address of the framebuffer the display scans out (sceDisplaySetFrameBuf).
void set_display_framebuffer(std::uint32_t address);

// Records and submits everything drawn since the previous call. Returns true
// when the displayed framebuffer was rendered during this interval; false
// means the guest's own framebuffer (for example a CPU-drawn movie frame) is
// the image to show.
bool finish_frame(std::uint64_t vblank);

struct Frame {
    std::span<const std::byte> rgba;  // tightly packed RGBA8, top row first
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint64_t vblank{};
};

// The newest display image the GPU has finished. With `wait` it is the frame
// finish_frame() just submitted (exact, for captures); without, presenting
// lags at most one frame and never stalls on the GPU.
[[nodiscard]] Frame latest_frame(bool wait);

[[nodiscard]] const GeGpuBackendReport &report();
[[nodiscard]] std::string device_name();

} // namespace psprecomp::hle::vulkan
