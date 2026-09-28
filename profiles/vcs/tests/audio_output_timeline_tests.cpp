// Exercise the production mixer against a deterministic waveOut device. Only
// the OS boundary is replaced: buffering, mixing and recovery are the real code.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <deque>
#include <stdexcept>
#include <iostream>
#include "vcs_config.hpp"

namespace fake {
std::deque<WAVEHDR *> queue;
bool paused = true;
bool fail_reset = false;
MMRESULT open(LPHWAVEOUT device, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD) {
    *device = reinterpret_cast<HWAVEOUT>(1); paused = true; return 0;
}
MMRESULT pause(HWAVEOUT) { paused = true; return 0; }
MMRESULT restart(HWAVEOUT) { paused = false; return 0; }
MMRESULT reset(HWAVEOUT) {
    if (fail_reset) return MMSYSERR_ERROR;
    for (auto *header : queue) { header->dwFlags |= WHDR_DONE; header->dwFlags &= ~WHDR_INQUEUE; }
    queue.clear(); return 0;
}
MMRESULT prepare(HWAVEOUT, WAVEHDR *h, UINT) { h->dwFlags |= WHDR_PREPARED; return 0; }
MMRESULT unprepare(HWAVEOUT, WAVEHDR *h, UINT) {
    if (h->dwFlags & WHDR_INQUEUE) throw std::runtime_error("unprepare in-flight header");
    h->dwFlags &= ~WHDR_PREPARED; return 0;
}
MMRESULT write(HWAVEOUT, WAVEHDR *h, UINT) {
    h->dwFlags &= ~WHDR_DONE; h->dwFlags |= WHDR_INQUEUE; queue.push_back(h); return 0;
}
MMRESULT close(HWAVEOUT) { return reset(nullptr); }
void consume(std::size_t blocks) {
    while (!paused && blocks-- && !queue.empty()) {
        auto *h = queue.front(); queue.pop_front();
        h->dwFlags |= WHDR_DONE; h->dwFlags &= ~WHDR_INQUEUE;
    }
}
}

#define waveOutOpen fake::open
#define waveOutPause fake::pause
#define waveOutRestart fake::restart
#define waveOutReset fake::reset
#define waveOutPrepareHeader fake::prepare
#define waveOutUnprepareHeader fake::unprepare
#define waveOutWrite fake::write
#define waveOutClose fake::close
#include "../host/audio_output.cpp"

namespace vcs {
const VcsConfiguration &vcs_configuration() {
    static const auto configuration = [] {
        VcsConfiguration c;
        c.audio.diagnostics = false;
        c.audio.prebuffer_blocks = 8;
        c.audio.recovery_prebuffer_blocks = 14;
        return c;
    }();
    return configuration;
}
}

void require(bool ok, const char *message) { if (!ok) throw std::runtime_error(message); }
std::uint64_t time_for(std::uint64_t frame) { return (frame * 1000000u + 44099u) / 44100u; }
void fresh() {
    vcs::audio_output_shutdown();
    auto &s = vcs::audio_state();
    require(vcs::ensure_device(s), "device initialization");
    s.guest_anchor_us = 0; s.timeline_anchored = true;
}

void test_future_submission_does_not_seal_other_channels() {
    fresh();
    const std::vector<std::int16_t> pcm(4096, 1200);
    vcs::audio_output_submit(pcm, 2048, true, 32768, 32768, 44100, 0, time_for(2048), 0);
    require(vcs::audio_state().output_frame == 0, "future scheduled buffer advanced actual guest clock");
    vcs::audio_output_submit(pcm, 2048, true, 32768, 32768, 44100, 1, 0, 0);
    vcs::audio_output_advance(time_for(2048));
    require(!fake::queue.empty(), "current-channel samples not queued");
    require(reinterpret_cast<std::int16_t *>(fake::queue.front()->lpData)[0] == 1200,
            "future submission sealed silence before second channel supplied PCM");
}

void test_full_queue_recovers_without_losing_retained_speech() {
    fresh();
    auto &s = vcs::audio_state();
    // Reproduce a saturated 24-block device with another ~125 ms in the mixer.
    for (std::uint64_t f = 0; f < 20000; ++f) {
        s.ring[f * 2] = s.ring[f * 2 + 1] = static_cast<std::int32_t>(f % 20000 + 1);
    }
    for (unsigned i = 0; i < 24; ++i) require(vcs::queue_one_block(s), "fill device");
    const auto guest = 17800u;
    require(vcs::recover_latency_locked(s, guest), "saturated queue never recovered");
    const auto resume = s.output_frame;
    require(guest - resume >= 5120 && guest - resume < 5632, "recovery missed normal delay budget");
    // Here resume exceeds submitted PCM: it must retain the new speech in ring.
    require(s.ring[(resume % vcs::kRingFrames) * 2] == resume % 20000 + 1,
            "recovery erased recent unsubmitted speech");
    vcs::advance_locked(s, time_for(guest));
    require(fake::queue.size() >= 8 && fake::queue.size() <= 9 && !fake::paused,
            "recovery failed to restart with bounded queue");
    require(reinterpret_cast<std::int16_t *>(fake::queue.front()->lpData)[0] == resume % 20000 + 1,
            "recovery played old scene instead of retained speech");
}

void test_recovery_preserves_already_queued_suffix() {
    fresh(); auto &s = vcs::audio_state();
    for (std::uint64_t f = 0; f < 12288; ++f) s.ring[f * 2] = s.ring[f * 2 + 1] = 2345;
    for (unsigned i = 0; i < 24; ++i) require(vcs::queue_one_block(s), "fill suffix");
    require(vcs::recover_latency_locked(s, 13000), "queued suffix not recovered");
    require(s.output_frame < 12288, "test must retain already queued samples");
    require(vcs::queue_one_block(s), "suffix requeue");
    require(reinterpret_cast<std::int16_t *>(fake::queue.front()->lpData)[0] == 2345,
            "reset lost valid PCM already handed to Windows");
}

void test_long_and_repeated_stalls_do_not_accumulate_latency() {
    fresh(); auto &s = vcs::audio_state();
    std::uint64_t guest = 6000;
    vcs::advance_locked(s, time_for(guest));
    for (unsigned i = 0; i < 10; ++i) {
        fake::consume(100);
        guest += (i == 0 ? 441000 : 22050); // 10 s loading stall, then 500 ms stalls
        vcs::advance_locked(s, time_for(guest));
        const auto queued = fake::queue.size() * 512u;
        const auto lag = guest - (s.output_frame - queued);
        require(lag >= 5120 && lag < 5632, "stall permanently accumulated extra A/V delay");
        require(fake::queue.size() <= 9 && !fake::paused, "stall left playback stopped or saturated");
    }
}

void test_stable_playback_does_not_reset_and_device_failure_is_safe() {
    fresh(); auto &s = vcs::audio_state();
    for (std::uint64_t frame = 0; frame < 44100 * 20; frame += 512) {
        fake::consume(1);
        vcs::advance_locked(s, time_for(frame));
    }
    require(s.latency_recoveries == 0, "normal playback repeatedly reset device");
    fake::fail_reset = true;
    const auto before = s.output_frame;
    require(!vcs::recover_latency_locked(s, before + 44100), "failed reset reported success");
    require(s.output_frame == before, "failed reset corrupted guest timeline");
    fake::fail_reset = false;
}

void test_live_output2_producer_recovers_after_loading() {
    fresh(); auto &s = vcs::audio_state();
    const std::vector<std::int16_t> pcm(1024, 3456);
    std::uint64_t now = 0;
    auto step = [&] {
        fake::consume(1);
        vcs::audio_output_submit(pcm, 512, true, 32768, 32768, 44100, 8,
                                 time_for(now), time_for(now));
        vcs::audio_output_advance(time_for(now));
        now += 512;
    };
    for (unsigned i = 0; i < 300; ++i) step();
    require(s.latency_recoveries == 0, "continuous music caused needless resets");
    fake::consume(100); now += 22050;
    for (unsigned i = 0; i < 300; ++i) step();
    require(s.latency_recoveries == 1, "loading recovery failed with live Output2 watermark");
    require(!fake::paused && !fake::queue.empty(), "Output2 stayed silent after loading");
    require(fake::queue.size() <= 10, "Output2 queue saturated again after recovery");
    require(reinterpret_cast<std::int16_t *>(fake::queue.front()->lpData)[0] == 3456,
            "recovered Output2 lost continuous PCM");
}

int main() {
    try {
        _putenv_s("PSPRECOMP_AUDIO", "1");
        _putenv_s("PSPRECOMP_AUDIO_SUMMARY", "0");
        _putenv_s("PSPRECOMP_AUDIO_PREBUFFER_BLOCKS", "8");
        test_future_submission_does_not_seal_other_channels();
        test_full_queue_recovers_without_losing_retained_speech();
        test_recovery_preserves_already_queued_suffix();
        test_long_and_repeated_stalls_do_not_accumulate_latency();
        test_stable_playback_does_not_reset_and_device_failure_is_safe();
        test_live_output2_producer_recovers_after_loading();
        vcs::audio_output_shutdown();
        std::cout << "audio_output_timeline_tests: PASS (production mixer, simulated waveOut)\n";
    } catch (const std::exception &e) {
        std::cerr << "audio_output_timeline_tests: FAIL: " << e.what() << '\n'; return 1;
    }
}
