#pragma once

// Generic sceCtrl HLE shared by title profiles: sampling cycle/mode, buffered
// and peeked pad samples, latches. Live input comes from CtrlHooks; scripted
// presses for headless runs come from PSPRECOMP_CTRL_PULSE[n]_{BUTTONS,
// START_VBLANK,END_VBLANK,LX,LY,REPEAT_EVERY} environment variables.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <cstdint>
#include <utility>

namespace psprecomp::hle {

struct ControllerState {
    std::uint32_t sampling_cycle{};
    std::uint32_t sampling_mode{};
    std::uint32_t buttons{};
    std::uint8_t lx{128u};
    std::uint8_t ly{128u};
    std::uint8_t rx{128u};
    std::uint8_t ry{128u};
};

struct CtrlHooks {
    // PSP button mask currently held on the host (PSP_CTRL_* bits).
    std::uint32_t (*host_buttons)() = nullptr;
    // Host analog stick, 0..255 with 128 centred; leave both at 128 for none.
    void (*host_analog)(std::uint8_t &x, std::uint8_t &y) = nullptr;
};

extern CtrlHooks g_ctrl_hooks;
extern ControllerState controller_state;

std::uint32_t effective_controller_buttons();
std::pair<std::uint8_t, std::uint8_t> effective_controller_analog();

void reset_ctrl(const CtrlHooks &hooks = {});
void install_ctrl_hle(Runtime &runtime);

} // namespace psprecomp::hle
