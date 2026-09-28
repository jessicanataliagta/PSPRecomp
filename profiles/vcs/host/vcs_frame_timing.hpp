#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace vcs {
bool unlocked_game_timing_enabled() noexcept;
// Keep the fractional millisecond discarded by the stock per-frame integer
// counters. Detect external resets/clamps from the next observed counter.
struct FractionalGameClock {
    double remainder{};
    std::uint32_t expected{};
    bool valid{};
    float increment(std::uint32_t current, float milliseconds, bool unlocked) noexcept {
        if (!unlocked || !std::isfinite(milliseconds) || milliseconds < 0.0f || milliseconds > 1000000.0f) {
            valid = false;
            remainder = 0.0;
            return milliseconds;
        }
        if (!valid || current != expected) remainder = 0.0;
        const double total = milliseconds + remainder;
        const auto whole = static_cast<std::uint32_t>(total);
        remainder = total - whole;
        expected = current + whole;
        valid = true;
        return static_cast<float>(whole);
    }
};
float game_clock_increment(unsigned clock, std::uint32_t current, float milliseconds) noexcept;

// CTimer uses 50 units per second. Its stock minimum of 0.5 means 10 ms,
// regardless of the real frame duration: above 100 FPS animation time outruns
// the sample clock. Keep the stock path at 30 FPS, and elapsed time otherwise.
inline float game_minimum_timestep(float elapsed, float stock_minimum, bool unlocked) noexcept {
    return unlocked ? std::max(0.0f, elapsed) : stock_minimum;
}

// Root motion is a displacement, not a velocity. The stock sub-0.5 paths
// multiply ped deltas by 0.5 and cutscene-object deltas by 2 instead of 1/dt.
// CTimer's stock minimum normally hides both paths. Once that floor is removed,
// both consumers must use the actual timestep as well. A paused frame has no
// velocity contribution; never produce an infinity from a zero timestep.
inline float game_root_motion_scale(float timestep, float stock_scale, bool unlocked) noexcept {
    return unlocked ? (timestep > 0.0f ? 1.0f / timestep : 0.0f) : stock_scale;
}
}
