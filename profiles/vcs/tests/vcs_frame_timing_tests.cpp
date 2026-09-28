#include "vcs_frame_timing.hpp"
#include "vcs_audio_commands.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main() {
    // Replay the READY-source branch for radio/cutscene transitions, including
    // preload-only calls. An old READY bit must not turn a new file into a
    // seek on the existing decoder, or silently ignore its preload.
    for (std::uint32_t current : {0u, 1u, 8u, 9u, 50u, 110u, 111u, ~0u}) {
        for (std::uint32_t requested : {0u, 1u, 8u, 9u, 50u, 110u, 111u}) {
            for (bool preload : {false, true}) {
                for (bool refilling : {false, true}) {
                    std::uint32_t events = 0;
                    if (vcs::stream_request_needs_new_source(current, requested))
                        events = vcs::stream_open_event_bits(current, requested);
                    else if (!preload) events = 2u; // stock same-source seek
                    auto decoder_source = current;
                    bool loading = refilling;
                    if (events & 4u) loading = false;
                    if (!loading && (events & 1u)) decoder_source = requested;
                    // A seek changes position, never the decoder's source.
                    if (decoder_source != requested) {
                        std::cerr << "New cutscene request retained the previous audio source\n";
                        return 1;
                    }
                    if (current == requested && events != (preload ? 0u : 2u)) return 1;
                }
            }
        }
    }
    {
        std::ifstream input(std::filesystem::path(__FILE__).parent_path().parent_path()
                            / "generated/generated_unit_0169.cpp");
        const std::string code{std::istreambuf_iterator<char>(input), {}};
        const auto begin = code.find("L_08AABAA0:\n");
        const auto end = code.find("L_08AABAD8:\n", begin);
        if (begin == std::string::npos || end == std::string::npos ||
            code.substr(begin, end - begin).find("vcs::stream_request_needs_new_source(") == std::string::npos ||
            code.substr(begin, end - begin).find("vcs::stream_open_event_bits(") == std::string::npos) return 1;
    }
    // Multiple render frames can enqueue commands before one audio batch.
    // Exhaust every start/stop order up to eight commands; last request wins,
    // including STOP->START (a legitimate retrigger) and START->STOP.
    for (unsigned length = 1; length <= 8; ++length) {
        for (unsigned sequence = 0; sequence < (1u << length); ++sequence) {
            std::uint32_t starts = 0, stops = 0;
            bool requested = false;
            for (unsigned n = 0; n < length; ++n) {
                requested = ((sequence >> n) & 1u) != 0;
                if (requested) starts |= 1u;
                else {
                    stops |= 1u;
                    starts = vcs::stop_queued_audio_start(starts, 1u);
                }
            }
            bool playing = false;
            if (stops & 1u) playing = false;
            if (starts & 1u) playing = true;
            if (playing != requested) {
                std::cerr << "Audio command order restarted a stopped voice\n";
                return 1;
            }
        }
    }
    for (unsigned voice = 0; voice < 28; ++voice) {
        const auto bit = 1u << voice;
        const auto low = voice < 24 ? bit : 0u;
        const auto high = voice >= 24 ? 1u << (voice - 24u) : 0u;
        for (unsigned extra_frames = 1; extra_frames <= 8; ++extra_frames) {
            for (unsigned frame = 0; frame < extra_frames; ++frame) {
                if (vcs::audio_end_flags_with_pending_starts(~0u, low, high, 0, 0, false) != ~bit ||
                    vcs::audio_end_flags_with_pending_starts(~0u, 0, 0, low, high, true) != ~bit)
                    return 1;
            }
        }
        if (vcs::audio_end_flags_with_pending_starts(~0u, 0, 0, low, high, false) != ~0u ||
            vcs::audio_end_flags_with_pending_starts(~0u, 0, 0, 0, 0, true) != ~0u)
            return 1; // consumed/aborted batches must not hide a real EOF
    }
    for (unsigned fps : {30u, 60u, 100u, 120u, 144u, 200u, 240u}) {
        vcs::FractionalGameClock clock;
        std::uint32_t counter = 0;
        double measured = 0;
        for (unsigned frame = 0; frame < fps * 60; ++frame) {
            const float ms = 1000.0f / fps;
            measured += ms;
            counter += static_cast<std::uint32_t>(clock.increment(counter, ms, true));
        }
        if (std::abs(counter - measured) >= 1.001) {
            std::cerr << "Integer game clock drift at FPS " << fps << '\n';
            return 1;
        }
        // Pause preserves the fraction; external reset discards it.
        if (clock.increment(counter, 0, true) != 0 || clock.increment(123u, 0.25f, true) != 0 ||
            clock.increment(123u, 0.75f, true) != 1 || clock.increment(124u, 0.5f, false) != 0.5f)
            return 1;
    }
    for (const unsigned fps : {30u, 60u, 100u, 120u, 144u, 200u, 240u}) {
        double seconds = 0.0;
        for (unsigned frame = 0; frame < fps * 60u; ++frame) {
            const float measured = 50.0f / fps;
            const float dt = measured < 0.5f
                ? vcs::game_minimum_timestep(measured, 0.5f, fps > 30u) : measured;
            seconds += dt / 50.0;
        }
        if (std::abs(seconds - 60.0) > 0.001) {
            std::cerr << "Animation drift at " << fps << " FPS: " << seconds << '\n';
            return 1;
        }
    }
    if (vcs::game_minimum_timestep(0.1f, 0.5f, false) != 0.5f ||
        vcs::game_minimum_timestep(0.0f, 0.5f, true) != 0.0f) return 1;
    double elapsed = 0.0, animation = 0.0;
    for (unsigned frame = 0; frame < 10000; ++frame) {
        const float milliseconds = frame % 3 == 0 ? 3.0f : 12.0f;
        const float measured = milliseconds / 20.0f;
        elapsed += milliseconds;
        animation += (measured < 0.5f
            ? vcs::game_minimum_timestep(measured, 0.5f, true) : measured) * 20.0;
    }
    if (std::abs(elapsed - animation) > 0.01) return 1;

    // Replay the root-displacement -> velocity -> physics-displacement chain.
    // Test both stock fallback factors: ped 0.5, cutscene object 2.0.
    for (float stock_scale : {0.5f, 2.0f}) {
        for (unsigned fps : {30u, 60u, 100u, 120u, 144u, 200u, 240u}) {
            double distance = 0.0;
            for (unsigned frame = 0; frame < fps * 10u; ++frame) {
                const float dt = 50.0f / fps;
                const float root_delta = 3.0f / fps; // 3 metres/second
                const float scale = dt < 0.5f
                    ? vcs::game_root_motion_scale(dt, stock_scale, true) : 1.0f / dt;
                distance += root_delta * scale * dt;
            }
            if (std::abs(distance - 30.0) > 0.001) {
                std::cerr << "Root-motion distance changed with FPS: " << fps << ' ' << distance << '\n';
                return 1;
            }
        }
        if (vcs::game_root_motion_scale(0.25f, stock_scale, false) != stock_scale ||
            vcs::game_root_motion_scale(0.0f, stock_scale, true) != 0.0f) return 1;

        double expected = 0.0, integrated = 0.0;
        for (unsigned frame = 0; frame < 10000u; ++frame) {
            // Cross the old threshold repeatedly, including paused frames.
            const float milliseconds = frame % 5 == 0 ? 0.0f : (frame % 2 == 0 ? 4.0f : 16.0f);
            const float dt = milliseconds / 20.0f;
            const float delta = -2.0f * milliseconds / 1000.0f;
            const float scale = dt < 0.5f
                ? vcs::game_root_motion_scale(dt, stock_scale, true) : 1.0f / dt;
            expected += delta;
            integrated += delta * scale * dt;
        }
        if (!std::isfinite(integrated) || std::abs(expected - integrated) > 0.001) return 1;
    }

    // Generated intra-unit jumps bypass Runtime::register_function. Catch a
    // regeneration that silently removes the actual CTimer patch, not just a
    // regression in the standalone arithmetic helper.
    std::ifstream generated(std::filesystem::path(__FILE__).parent_path().parent_path()
                            / "generated/generated_unit_0131.cpp");
    const std::string source{std::istreambuf_iterator<char>(generated), {}};
    const auto begin = source.find("L_08A11438:\n");
    const auto end = source.find("L_08A11440:\n", begin);
    if (source.find("vcs::game_clock_increment(0u, ctx.gpr[6], aot_fpr_16)") == std::string::npos ||
        source.find("vcs::game_clock_increment(1u, aot_gpr_5, aot_fpr_12)") == std::string::npos)
        return 1;
    if (begin == std::string::npos || end == std::string::npos ||
        source.substr(begin, end - begin).find("vcs::game_minimum_timestep(") == std::string::npos) {
        std::cerr << "Generated CTimer high-FPS patch is missing\n";
        return 1;
    }
    for (const auto &unit : {"generated_unit_0069.cpp", "generated_unit_0133.cpp"}) {
        std::ifstream input(std::filesystem::path(__FILE__).parent_path().parent_path() / "generated" / unit);
        const std::string code{std::istreambuf_iterator<char>(input), {}};
        const char *label = std::string(unit) == "generated_unit_0069.cpp" ? "L_0891997C:\n" : "L_08A199DC:\n";
        const auto at = code.find(label);
        if (at == std::string::npos || code.substr(at, 600).find("vcs::game_root_motion_scale(") == std::string::npos) {
            std::cerr << "Generated root-motion patch missing: " << unit << '\n';
            return 1;
        }
    }
    {
        std::ifstream input(std::filesystem::path(__FILE__).parent_path().parent_path()
                            / "generated/generated_unit_0001.cpp");
        const std::string code{std::istreambuf_iterator<char>(input), {}};
        const auto at = code.find("L_0880AB48:\n");
        if (code.find("vcs::audio_end_flags_with_pending_starts(") == std::string::npos ||
            code.find("vcs::stop_queued_audio_start(aot_mem.aot_direct_load32(aot_gpr_4), aot_gpr_6)") == std::string::npos)
            return 1;
        if (at == std::string::npos ||
            code.substr(at, 500).find("vcs::finish_aborted_audio_stops(") == std::string::npos) {
            std::cerr << "Generated audio abort KeyOff drain is missing\n";
            return 1;
        }
    }
    std::cout << "VCS frame timing, root motion and audio abort hook: PASS\n";
}
