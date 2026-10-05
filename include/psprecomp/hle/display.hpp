#pragma once

// Generic sceDisplay HLE shared by title profiles: display mode and framebuffer
// state, vcount/vblank queries, and the vblank waits that advance virtual time
// to the next vblank and deliver the vblank sub-interrupt (30/15). Presentation
// and frame pacing are the profile's job, reached through DisplayHooks.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <cstdint>

namespace psprecomp::hle {

struct DisplayState {
    std::uint32_t mode{};
    std::uint32_t width{480u};
    std::uint32_t height{272u};
    std::uint32_t frame_buffer{};
    std::uint32_t buffer_width{512u};
    std::uint32_t pixel_format{3u};
    std::uint32_t sync_mode{};
};

struct DisplayHooks {
    // Virtual display refresh rate; 60 Hz (the PSP's 59.94 Hz period) when unset.
    std::uint32_t (*refresh_hz)() = nullptr;
    // Runs first in every vblank wait; returning false leaves the wait (the
    // hook has already set $v0 or stopped the runtime).
    bool (*before_vblank_wait)(Runtime &runtime, AllegrexContext &ctx) = nullptr;
    // Runs once per vblank after display_vblank_index advanced: present the
    // framebuffer, pace the frame. Returning false leaves the wait.
    bool (*on_vblank)(Runtime &runtime, AllegrexContext &ctx) = nullptr;
};

extern DisplayHooks g_display_hooks;
// When set, reset_display() makes the vblank sub-interrupt (30/15) periodic on
// PSP time (see set_periodic_sub_interrupt) and vblank waits only wait. Off by
// default: the handler runs only on a thread that waits for vblank.
extern bool periodic_vblank_interrupts;
extern DisplayState display_state;

std::uint32_t display_refresh_hz();
std::uint64_t display_vblank_period_us();

void reset_display(const DisplayHooks &hooks = {});
void install_display_hle(Runtime &runtime);

} // namespace psprecomp::hle
