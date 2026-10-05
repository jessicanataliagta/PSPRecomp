#include "psprecomp/hle/ctrl.hpp"

#include "psprecomp/hle/kernel.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp);
// live input is reached through g_ctrl_hooks.

namespace psprecomp::hle {
namespace {

std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}

struct ControllerPulseConfig {
    std::uint32_t buttons{};
    std::uint64_t start_vblank{};
    std::uint64_t end_vblank{};
    std::uint8_t lx{128u};
    std::uint8_t ly{128u};
    bool has_lx{};
    bool has_ly{};
    // _REPEAT_EVERY: repeat the [start, end] window every this many vblanks.
    std::uint64_t repeat_every{};
};

const std::array<ControllerPulseConfig, 8> &controller_pulse_configs() {
    static const std::array<ControllerPulseConfig, 8> configs = [] {
        std::array<ControllerPulseConfig, 8> values{};
        constexpr std::array<const char *, 8> suffixes{"", "2", "3", "4", "5", "6", "7", "8"};
        for (std::size_t index = 0; index < values.size(); ++index) {
            const std::string suffix = suffixes[index];
            const std::string prefix = "PSPRECOMP_CTRL_PULSE" + suffix;
            const std::string buttons_name = prefix + "_BUTTONS";
            const std::string start_name = prefix + "_START_VBLANK";
            const std::string end_name = prefix + "_END_VBLANK";
            const std::string lx_name = prefix + "_LX";
            const std::string ly_name = prefix + "_LY";
            const std::string repeat_name = prefix + "_REPEAT_EVERY";
            ControllerPulseConfig &value = values[index];
            value.buttons = static_cast<std::uint32_t>(parse_environment_u64(buttons_name.c_str()));
            value.start_vblank = parse_environment_u64(start_name.c_str());
            value.end_vblank = parse_environment_u64(end_name.c_str(), value.start_vblank);
            if (value.end_vblank < value.start_vblank) value.end_vblank = value.start_vblank;
            value.repeat_every = parse_environment_u64(repeat_name.c_str());
            if (std::getenv(lx_name.c_str()) != nullptr) {
                value.lx = static_cast<std::uint8_t>(std::min<std::uint64_t>(255u, parse_environment_u64(lx_name.c_str(), 128u)));
                value.has_lx = true;
            }
            if (std::getenv(ly_name.c_str()) != nullptr) {
                value.ly = static_cast<std::uint8_t>(std::min<std::uint64_t>(255u, parse_environment_u64(ly_name.c_str(), 128u)));
                value.has_ly = true;
            }
        }
        return values;
    }();
    return configs;
}

} // namespace

CtrlHooks g_ctrl_hooks{};
ControllerState controller_state{};

static bool controller_pulse_active(const ControllerPulseConfig &pulse) {
    if (pulse.buttons == 0u && !pulse.has_lx && !pulse.has_ly) return false;
    if (display_vblank_index < pulse.start_vblank) return false;
    if (pulse.repeat_every != 0u)
        return (display_vblank_index - pulse.start_vblank) % pulse.repeat_every <= pulse.end_vblank - pulse.start_vblank;
    return display_vblank_index <= pulse.end_vblank;
}

std::uint32_t effective_controller_buttons() {
    // Live keyboard input is ORed in; deterministic vblank pulses keep scripted
    // validation runs reproducible whether or not a window is open.
    std::uint32_t buttons = controller_state.buttons |
        (g_ctrl_hooks.host_buttons != nullptr ? g_ctrl_hooks.host_buttons() : 0u);
    for (const ControllerPulseConfig &pulse : controller_pulse_configs()) {
        if (controller_pulse_active(pulse)) buttons |= pulse.buttons;
    }
    return buttons;
}

std::pair<std::uint8_t, std::uint8_t> effective_controller_analog() {
    std::uint8_t analog_x = controller_state.lx;
    std::uint8_t analog_y = controller_state.ly;
    std::uint8_t window_x = 128u;
    std::uint8_t window_y = 128u;
    if (g_ctrl_hooks.host_analog != nullptr) g_ctrl_hooks.host_analog(window_x, window_y);
    if (window_x != 128u || window_y != 128u) {
        analog_x = window_x;
        analog_y = window_y;
    }
    // Later pulses intentionally win, allowing a scripted route to replace one
    // steering segment with the next while buttons remain independently ORed.
    for (const ControllerPulseConfig &pulse : controller_pulse_configs()) {
        if (!controller_pulse_active(pulse)) continue;
        if (pulse.has_lx) analog_x = pulse.lx;
        if (pulse.has_ly) analog_y = pulse.ly;
    }
    return {analog_x, analog_y};
}

void reset_ctrl(const CtrlHooks &hooks) {
    g_ctrl_hooks = hooks;
    controller_state = ControllerState{};
}

void install_ctrl_hle(psprecomp::Runtime &runtime) {
    runtime.register_hle("sceCtrl", 0x6A2774F3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t previous = controller_state.sampling_cycle;
            controller_state.sampling_cycle = ctx.gpr[4];
            ctx.set_gpr(2, previous);
        });
    runtime.register_hle("sceCtrl", 0x02BAAD91u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 4u)) {
                ctx.set_gpr(2, 0x80000103u);
                return;
            }
            rt.memory().store32(ctx.gpr[4], controller_state.sampling_cycle);
            set_success(ctx);
        });
    runtime.register_hle("sceCtrl", 0x1F4011E6u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] > 1u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            const std::uint32_t previous = controller_state.sampling_mode;
            controller_state.sampling_mode = ctx.gpr[4];
            ctx.set_gpr(2, previous);
        });
    runtime.register_hle("sceCtrl", 0xDA6B76A1u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 4u)) {
                ctx.set_gpr(2, 0x80000103u);
                return;
            }
            rt.memory().store32(ctx.gpr[4], controller_state.sampling_mode);
            set_success(ctx);
        });

    auto write_controller_samples = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx, bool negative) {
        const std::uint32_t destination = ctx.gpr[4];
        const std::uint32_t count = ctx.gpr[5];
        constexpr std::uint32_t sample_size = 16u;
        if (count == 0u) {
            ctx.set_gpr(2, 0u);
            return;
        }
        if (count > 64u || !rt.memory().contains(destination, static_cast<std::size_t>(count) * sample_size)) {
            ctx.set_gpr(2, 0x80000103u);
            return;
        }
        for (std::uint32_t index = 0u; index < count; ++index) {
            const std::uint32_t sample = destination + index * sample_size;
            rt.memory().store32(sample, static_cast<std::uint32_t>(system_time_microseconds()));
            const std::uint32_t buttons = negative ? ~effective_controller_buttons() : effective_controller_buttons();
            rt.memory().store32(sample + 4u, buttons);
            const auto [analog_x, analog_y] = effective_controller_analog();
            rt.memory().store8(sample + 8u, controller_state.sampling_mode != 0u ? analog_x : 128u);
            rt.memory().store8(sample + 9u, controller_state.sampling_mode != 0u ? analog_y : 128u);
            rt.memory().store8(sample + 10u, controller_state.rx);
            rt.memory().store8(sample + 11u, controller_state.ry);
            rt.memory().zero(sample + 12u, 4u);
        }
        ctx.set_gpr(2, count);
    };
    runtime.register_hle("sceCtrl", 0x3A622550u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, false); });
    runtime.register_hle("sceCtrl", 0x1F803938u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, false); });
    runtime.register_hle("sceCtrl", 0xC152080Au,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, true); });
    runtime.register_hle("sceCtrl", 0x60B81F86u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, true); });
    auto controller_latch = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 16u)) {
            ctx.set_gpr(2, 0x80000103u);
            return;
        }
        rt.memory().store32(ctx.gpr[4] + 0u, 0u);
        rt.memory().store32(ctx.gpr[4] + 4u, 0u);
        rt.memory().store32(ctx.gpr[4] + 8u, effective_controller_buttons());
        rt.memory().store32(ctx.gpr[4] + 12u, ~effective_controller_buttons());
        ctx.set_gpr(2, 0u);
    };
    runtime.register_hle("sceCtrl", 0xB1D0E5CDu, controller_latch);
    runtime.register_hle("sceCtrl", 0x0B588501u, controller_latch);

}

} // namespace psprecomp::hle
