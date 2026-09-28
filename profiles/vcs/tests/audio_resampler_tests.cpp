#include "audio_resampler.hpp"
#include "audio_output.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using Stereo = std::pair<std::int16_t, std::int16_t>;

void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

std::vector<std::int16_t> make_signal(std::size_t frames, bool stereo) {
    std::vector<std::int16_t> pcm(frames * (stereo ? 2u : 1u));
    for (std::size_t i = 0; i < frames; ++i) {
        const std::int16_t left = static_cast<std::int16_t>((static_cast<int>(i * 977u) % 50000) - 25000);
        const std::int16_t right = static_cast<std::int16_t>((static_cast<int>(i * 619u + 1234u) % 48000) - 24000);
        if (stereo) {
            pcm[i * 2u] = left;
            pcm[i * 2u + 1u] = right;
        } else {
            pcm[i] = left;
        }
    }
    return pcm;
}

std::vector<Stereo> run_one(const std::vector<std::int16_t> &pcm, std::uint32_t frames,
                            bool stereo, std::uint32_t rate) {
    vcs::StreamingLinearResampler resampler;
    resampler.reset(rate, stereo);
    std::vector<Stereo> output;
    resampler.process(pcm, frames, stereo, rate,
        [&](std::int16_t l, std::int16_t r) { output.emplace_back(l, r); });
    return output;
}

std::vector<Stereo> run_chunked(const std::vector<std::int16_t> &pcm, std::uint32_t frames,
                                bool stereo, std::uint32_t rate,
                                const std::vector<std::uint32_t> &chunks) {
    vcs::StreamingLinearResampler resampler;
    resampler.reset(rate, stereo);
    std::vector<Stereo> output;
    const std::size_t channels = stereo ? 2u : 1u;
    std::uint32_t offset_frames = 0u;
    std::size_t chunk_index = 0u;
    while (offset_frames < frames) {
        const std::uint32_t requested = chunks[chunk_index++ % chunks.size()];
        const std::uint32_t count = std::min(requested, frames - offset_frames);
        const std::span<const std::int16_t> view(
            pcm.data() + static_cast<std::size_t>(offset_frames) * channels,
            static_cast<std::size_t>(count) * channels);
        resampler.process(view, count, stereo, rate,
            [&](std::int16_t l, std::int16_t r) { output.emplace_back(l, r); });
        offset_frames += count;
    }
    return output;
}

void test_chunk_invariance(std::uint32_t rate, bool stereo) {
    constexpr std::uint32_t frames = 4096u;
    const auto input = make_signal(frames, stereo);
    const auto whole = run_one(input, frames, stereo, rate);
    const auto chunked = run_chunked(input, frames, stereo, rate, {17u, 63u, 256u, 511u, 37u});
    require(whole == chunked, "streaming resampler changed output at a PSP buffer boundary");
}

void test_unity_rate_exact() {
    constexpr std::uint32_t frames = 1024u;
    const auto input = make_signal(frames, true);
    const auto output = run_chunked(input, frames, true, 44100u, {64u, 128u, 257u});
    require(output.size() == frames, "44100 Hz resampler changed frame count");
    for (std::size_t i = 0; i < output.size(); ++i) {
        require(output[i].first == input[i * 2u] && output[i].second == input[i * 2u + 1u],
                "44100 Hz resampler was not bit-exact");
    }
}


void test_output2_master_watermark() {
    // While Output2 is a live producer inside the device prebuffer window, the
    // host must not commit frames beyond what the game has actually mixed.
    require(vcs::audio_output_master_seal_frame(4096u, 3072u, true, 2560u, 3072u) == 2560u,
            "Output2 watermark did not clamp speculative sealing");

    // Never move a seal point backwards when the producer is already ahead.
    require(vcs::audio_output_master_seal_frame(4096u, 2048u, true, 2560u, 3072u) == 2048u,
            "Output2 watermark moved an already-safe seal point");

    // A genuinely stalled/stopped Output2 producer must not freeze the other
    // PSP channels forever once it is beyond the bounded prebuffer grace.
    require(vcs::audio_output_master_seal_frame(8192u, 7168u, true, 2560u, 3072u) == 7168u,
            "stale Output2 producer incorrectly blocked the host timeline");
    require(vcs::audio_output_master_seal_frame(4096u, 3072u, false, 0u, 3072u) == 3072u,
            "inactive Output2 producer incorrectly changed the host timeline");

    // Recovery uses a deeper reserve than normal startup.  A producer that is
    // late by more than the normal reserve but still within recovery must keep
    // its watermark; otherwise a long frame is turned into committed silence.
    require(vcs::audio_output_master_seal_frame(8192u, 7168u, true, 2560u, 7168u) == 2560u,
            "recovery-sized Output2 grace did not protect a slow producer");
}

void test_mono_duplication() {
    const auto input = make_signal(1024u, false);
    const auto output = run_chunked(input, 1024u, false, 32000u, {127u, 129u});
    require(!output.empty(), "mono resampler produced no data");
    for (const auto &sample : output)
        require(sample.first == sample.second, "mono source was not duplicated to stereo");
}
}

int main() {
    try {
        test_unity_rate_exact();
        test_chunk_invariance(32000u, true);
        test_chunk_invariance(48000u, true);
        test_chunk_invariance(22050u, true);
        test_chunk_invariance(24000u, false);
        test_output2_master_watermark();
        test_mono_duplication();
        std::cout << "audio_resampler_tests: PASS\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "audio_resampler_tests: FAIL: " << e.what() << "\n";
        return 1;
    }
}
