#include "audio_output.hpp"
#include "audio_resampler.hpp"
#include "vcs_config.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>

#include <mutex>

namespace vcs {
namespace {

constexpr std::uint32_t kSampleRate = StreamingLinearResampler::kOutputRate;
constexpr std::uint32_t kOutputChannels = 2u;
// Smaller blocks reduce the time between vblank-driven queue refills.  A group
// of four is queued while waveOut is paused, then playback starts with ~46 ms
// already buffered.  That removes the periodic starvation clicks the old
// submit-driven sink produced when the guest had a long CPU frame.
constexpr std::size_t kBlockFrames = 512u;
constexpr std::size_t kBlockCount = 24u;
constexpr std::size_t kDefaultPrebufferBlocks = 6u;
// Do not seal the newest ~23 ms of the guest timeline.  Other PSP channels can
// still submit samples for that region before it is irreversibly handed to the
// device.  This replaces the old "furthest channel + four blocks" heuristic.
constexpr std::uint64_t kMixSafetyFrames = 1024u;
// Two seconds is enough to absorb a temporarily blocked host device without a
// channel lapping the ring during normal realtime play.
constexpr std::size_t kRingFrames = kSampleRate * 2u;
constexpr std::size_t kGuestChannels = 9u;
constexpr std::uint64_t kChannelDiscontinuityFrames = 64u;

struct Block {
    WAVEHDR header{};
    std::vector<std::int16_t> samples;
    std::uint64_t start_frame{};
};

struct ChannelStream {
    StreamingLinearResampler resampler;
    std::uint64_t cursor{};
    std::uint64_t last_guest_time_us{};
    std::uint32_t source_rate{kSampleRate};
    bool stereo{true};
    bool active{};
};

struct AudioState {
    std::mutex mutex;
    HWAVEOUT device{nullptr};
    std::vector<Block> blocks;
    std::size_t next_block{};
    std::vector<std::int32_t> ring;
    // First frame not yet handed to waveOut.
    std::uint64_t output_frame{};
    // Guest virtual-time -> output-frame anchor.
    std::uint64_t guest_anchor_us{};
    bool timeline_anchored{};
    std::array<ChannelStream, kGuestChannels> channels{};
    std::uint64_t late_frames_dropped{};
    std::uint64_t overrun_frames_dropped{};
    std::uint64_t queued_blocks{};
    std::uint64_t underrun_rebuffers{};
    std::uint64_t timeline_resyncs{};
    std::uint64_t latency_recoveries{};
    std::uint64_t stale_frames_skipped{};
    std::uint64_t output2_seal_clamps{};
    std::uint64_t submit_calls{};
    std::uint64_t submit_cpu_ns{};
    std::uint64_t submit_cpu_max_ns{};
    std::uint64_t last_summary_guest_us{};
    std::ofstream wav_capture;
    std::ofstream diagnostics_log;
    std::uint64_t wav_frames{};
    std::size_t prebuffer_blocks{kDefaultPrebufferBlocks};
    std::size_t recovery_prebuffer_blocks{kDefaultPrebufferBlocks * 2u};
    bool playback_started{};
    bool recovering_from_underrun{};
    bool opened{};
    bool failed{};
};

AudioState &audio_state() {
    static AudioState state;
    return state;
}

bool diagnostics_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr;
    return enabled;
}

bool summary_diagnostics_enabled() {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_AUDIO_SUMMARY");
        if (text != nullptr)
            return *text != '\0' && std::strcmp(text, "0") != 0;
        return vcs_configuration().audio.diagnostics;
    }();
    return enabled;
}

std::size_t configured_prebuffer_blocks() {
    const char *text = std::getenv("PSPRECOMP_AUDIO_PREBUFFER_BLOCKS");
    if (text == nullptr || *text == '\0')
        return std::clamp<std::size_t>(vcs_configuration().audio.prebuffer_blocks,
                                      2u, kBlockCount - 2u);
    char *end = nullptr;
    const unsigned long value = std::strtoul(text, &end, 0);
    if (end == text || *end != '\0') return kDefaultPrebufferBlocks;
    return std::clamp<std::size_t>(static_cast<std::size_t>(value), 2u, kBlockCount - 2u);
}

std::size_t outstanding_blocks(const AudioState &state) {
    return static_cast<std::size_t>(std::count_if(
        state.blocks.begin(), state.blocks.end(), [](const Block &block) {
            return (block.header.dwFlags & WHDR_PREPARED) != 0u &&
                (block.header.dwFlags & WHDR_DONE) == 0u;
        }));
}

void wav_write_u16(std::ostream &out, std::uint16_t value) {
    const std::array<char, 2> bytes{
        static_cast<char>(value & 0xFFu), static_cast<char>((value >> 8u) & 0xFFu)};
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

void wav_write_u32(std::ostream &out, std::uint32_t value) {
    const std::array<char, 4> bytes{
        static_cast<char>(value & 0xFFu), static_cast<char>((value >> 8u) & 0xFFu),
        static_cast<char>((value >> 16u) & 0xFFu), static_cast<char>((value >> 24u) & 0xFFu)};
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

void wav_write_header(std::ostream &out, std::uint64_t frames) {
    const std::uint64_t payload64 = frames * kOutputChannels * sizeof(std::int16_t);
    const std::uint32_t payload = static_cast<std::uint32_t>(
        std::min<std::uint64_t>(payload64, 0xFFFFFFFFull - 44u));
    out.write("RIFF", 4); wav_write_u32(out, 36u + payload);
    out.write("WAVEfmt ", 8); wav_write_u32(out, 16u);
    wav_write_u16(out, 1u); wav_write_u16(out, static_cast<std::uint16_t>(kOutputChannels));
    wav_write_u32(out, kSampleRate);
    wav_write_u32(out, kSampleRate * kOutputChannels * sizeof(std::int16_t));
    wav_write_u16(out, static_cast<std::uint16_t>(kOutputChannels * sizeof(std::int16_t)));
    wav_write_u16(out, 16u);
    out.write("data", 4); wav_write_u32(out, payload);
}

void open_wav_capture(AudioState &state) {
    const char *path = std::getenv("PSPRECOMP_AUDIO_WAV");
    if (path == nullptr || *path == '\0') return;
    state.wav_capture.open(path, std::ios::binary | std::ios::trunc);
    if (!state.wav_capture) {
        if (diagnostics_enabled())
            std::cerr << "[audio-host] unable to create WAV capture: " << path << "\n";
        return;
    }
    wav_write_header(state.wav_capture, 0u);
    state.wav_frames = 0u;
    if (diagnostics_enabled())
        std::cerr << "[audio-host] WAV capture: " << path << "\n";
}

void close_wav_capture(AudioState &state) {
    if (!state.wav_capture.is_open()) return;
    state.wav_capture.flush();
    state.wav_capture.seekp(0, std::ios::beg);
    wav_write_header(state.wav_capture, state.wav_frames);
    state.wav_capture.close();
}

bool ensure_device(AudioState &state) {
    if (state.opened) return true;
    if (state.failed) return false;

    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = static_cast<WORD>(kOutputChannels);
    format.nSamplesPerSec = kSampleRate;
    format.wBitsPerSample = 16u;
    format.nBlockAlign = static_cast<WORD>(kOutputChannels * sizeof(std::int16_t));
    format.nAvgBytesPerSec = kSampleRate * format.nBlockAlign;

    const MMRESULT open_result =
        waveOutOpen(&state.device, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL);
    if (open_result != MMSYSERR_NOERROR) {
        if (diagnostics_enabled())
            std::cerr << "[audio-host] waveOutOpen failed code=" << open_result << "\n";
        state.failed = true;
        state.device = nullptr;
        return false;
    }

    // Pause before the first write so playback starts with a real prebuffer,
    // not one tiny buffer followed by an immediate underrun.
    (void)waveOutPause(state.device);
    state.blocks.resize(kBlockCount);
    state.ring.assign(kRingFrames * kOutputChannels, 0);
    state.next_block = 0u;
    state.output_frame = 0u;
    state.queued_blocks = 0u;
    state.prebuffer_blocks = configured_prebuffer_blocks();
    state.recovery_prebuffer_blocks = std::clamp<std::size_t>(
        vcs_configuration().audio.recovery_prebuffer_blocks,
        state.prebuffer_blocks, kBlockCount - 2u);
    state.playback_started = false;
    state.recovering_from_underrun = false;
    if (summary_diagnostics_enabled()) {
        const auto path = vcs_configuration().executable_directory / "VCSAudio.log";
        state.diagnostics_log.open(path, std::ios::out | std::ios::trunc);
        if (state.diagnostics_log)
            state.diagnostics_log << "[audio-log] block_frames=" << kBlockFrames
                                  << " startup_blocks=" << state.prebuffer_blocks
                                  << " recovery_blocks=" << state.recovery_prebuffer_blocks
                                  << "\n";
    }
    open_wav_capture(state);
    if (diagnostics_enabled())
        std::cerr << "[audio-host] waveOut 44100Hz stereo block_frames=" << kBlockFrames
                  << " blocks=" << kBlockCount
                  << " prebuffer_blocks=" << state.prebuffer_blocks
                  << " prebuffer_ms="
                  << (state.prebuffer_blocks * kBlockFrames * 1000u / kSampleRate) << "\n";
    state.opened = true;
    return true;
}

std::uint64_t guest_frame_for(const AudioState &state, std::uint64_t guest_time_us) {
    if (!state.timeline_anchored || guest_time_us <= state.guest_anchor_us) return 0u;
    const std::uint64_t delta = guest_time_us - state.guest_anchor_us;
    // Rounded to nearest output frame.  This keeps repeated ceil-rounded PSP
    // blocking durations from accumulating a frame of drift every few buffers.
    return (delta * kSampleRate + 500000u) / 1000000u;
}

bool queue_one_block(AudioState &state) {
    Block &block = state.blocks[state.next_block];
    if ((block.header.dwFlags & WHDR_PREPARED) != 0u) {
        if ((block.header.dwFlags & WHDR_DONE) == 0u) return false;
        (void)waveOutUnprepareHeader(state.device, &block.header, sizeof(WAVEHDR));
    }

    block.samples.resize(kBlockFrames * kOutputChannels);
    for (std::size_t frame = 0u; frame < kBlockFrames; ++frame) {
        const std::size_t slot =
            static_cast<std::size_t>((state.output_frame + frame) % kRingFrames) * kOutputChannels;
        for (std::size_t channel = 0u; channel < kOutputChannels; ++channel) {
            block.samples[frame * kOutputChannels + channel] = static_cast<std::int16_t>(
                std::clamp(state.ring[slot + channel], -32768, 32767));
            state.ring[slot + channel] = 0;
        }
    }

    if (state.wav_capture.is_open()) {
        state.wav_capture.write(reinterpret_cast<const char *>(block.samples.data()),
                                static_cast<std::streamsize>(block.samples.size() * sizeof(std::int16_t)));
        if (state.wav_capture) state.wav_frames += kBlockFrames;
    }

    block.header = WAVEHDR{};
    block.start_frame = state.output_frame;
    block.header.lpData = reinterpret_cast<LPSTR>(block.samples.data());
    block.header.dwBufferLength = static_cast<DWORD>(block.samples.size() * sizeof(std::int16_t));
    const MMRESULT prepare_result =
        waveOutPrepareHeader(state.device, &block.header, sizeof(WAVEHDR));
    if (prepare_result != MMSYSERR_NOERROR) {
        if (diagnostics_enabled())
            std::cerr << "[audio-host] waveOutPrepareHeader failed code="
                      << prepare_result << "\n";
        return false;
    }
    const MMRESULT write_result = waveOutWrite(state.device, &block.header, sizeof(WAVEHDR));
    if (write_result != MMSYSERR_NOERROR) {
        if (diagnostics_enabled())
            std::cerr << "[audio-host] waveOutWrite failed code=" << write_result << "\n";
        (void)waveOutUnprepareHeader(state.device, &block.header, sizeof(WAVEHDR));
        return false;
    }

    state.output_frame += kBlockFrames;
    state.next_block = (state.next_block + 1u) % state.blocks.size();
    ++state.queued_blocks;

    const std::size_t target_blocks = state.recovering_from_underrun
        ? state.recovery_prebuffer_blocks : state.prebuffer_blocks;
    if (!state.playback_started && outstanding_blocks(state) >= target_blocks) {
        if (waveOutRestart(state.device) == MMSYSERR_NOERROR) {
            state.playback_started = true;
            state.recovering_from_underrun = false;
            if (diagnostics_enabled())
                std::cerr << "[audio-host] waveOut started with " << state.queued_blocks
                          << " prebuffered blocks\n";
        }
    }
    return true;
}

// waveOut has no guest timestamps. After a loading stall it will happily play
// every stale block, permanently delaying speech even after FPS has recovered.
// Estimate the oldest audible guest frame from the outstanding headers (within
// one 512-frame block), and discard only the excess beyond our buffering budget.
bool recover_latency_locked(AudioState &state, std::uint64_t guest_frame) {
    const std::uint64_t queued_frames = outstanding_blocks(state) * kBlockFrames;
    const std::uint64_t played_frame = state.output_frame > queued_frames
        ? state.output_frame - queued_frames : 0u;
    const std::uint64_t target_delay = state.prebuffer_blocks * kBlockFrames + kMixSafetyFrames;
    // Hysteresis covers partial headers and normal vblank jitter. It must not
    // become another reserve added on every underrun.
    if (guest_frame <= played_frame ||
        guest_frame - played_frame <= target_delay + 4u * kBlockFrames) return false;
    const std::uint64_t resume_frame = ((guest_frame - target_delay) / kBlockFrames) * kBlockFrames;
    if (waveOutReset(state.device) != MMSYSERR_NOERROR) return false;
    (void)waveOutPause(state.device);

    // Already submitted PCM was removed from the mix ring. Restore the retained
    // suffix before returning headers, so recovery never replaces valid recent
    // dialogue with silence or restarts a decoder from sample zero.
    for (Block &block : state.blocks) {
        if ((block.header.dwFlags & WHDR_PREPARED) == 0u) continue;
        for (std::size_t frame = 0u; frame < kBlockFrames; ++frame) {
            const auto absolute = block.start_frame + frame;
            if (absolute < resume_frame || absolute >= state.output_frame) continue;
            const auto slot = static_cast<std::size_t>(absolute % kRingFrames) * kOutputChannels;
            state.ring[slot] = block.samples[frame * kOutputChannels];
            state.ring[slot + 1u] = block.samples[frame * kOutputChannels + 1u];
        }
    }
    // Clear any unsubmitted portion of the skipped interval, bounded by the
    // ring size even after a very long pause. Do not erase future-channel PCM.
    for (std::uint64_t frame = state.output_frame;
         frame < resume_frame && frame - state.output_frame < kRingFrames; ++frame) {
        const auto slot = static_cast<std::size_t>(frame % kRingFrames) * kOutputChannels;
        state.ring[slot] = state.ring[slot + 1u] = 0;
    }
    state.output_frame = resume_frame;
    // Keep prepared headers allocated. waveOutReset marks them DONE; the normal
    // queue path unprepares each safely before reusing it.
    state.playback_started = false;
    state.recovering_from_underrun = false;
    ++state.latency_recoveries;
    state.stale_frames_skipped += resume_frame - played_frame;
    if (state.diagnostics_log) {
        state.diagnostics_log << "[audio-recover] guest_frame=" << guest_frame
            << " old_played_frame=" << played_frame << " resume_frame=" << resume_frame
            << " skipped_ms=" << (resume_frame - played_frame) * 1000u / kSampleRate << "\n";
        state.diagnostics_log.flush();
    }
    return true;
}

void advance_locked(AudioState &state, std::uint64_t guest_time_us) {
    if (!state.timeline_anchored || !state.opened) return;
    const std::uint64_t guest_frame = guest_frame_for(state, guest_time_us);
    (void)recover_latency_locked(state, guest_frame);
    std::size_t outstanding = outstanding_blocks(state);
    if (state.playback_started) {
        if (outstanding == 0u) {
            // Once waveOut drains completely, immediately writing one block at
            // a time leaves a permanent train of audible gaps. Pause the empty
            // device, build a deeper reserve, then resume continuous playback.
            (void)waveOutPause(state.device);
            ++state.underrun_rebuffers;
            state.playback_started = false;
            // A deeper reserve adds permanent A/V latency on every stall.
            // Recovery uses the same delay budget as initial playback.
            state.recovering_from_underrun = false;
        }
    }
    // When only two native blocks remain, waiting another full 23 ms for every
    // PSP channel to contribute is more damaging than sealing the already
    // mixed samples. This emergency margin recovers up to two blocks before an
    // audible underrun without changing the normal multi-channel mix path.
    const std::uint64_t safety_frames = state.playback_started && outstanding <= 2u
        ? 0u : kMixSafetyFrames;
    std::uint64_t sealed_frame = guest_frame > safety_frames
        ? guest_frame - safety_frames : 0u;

    // Preserve the HEAD producer watermark: this prevents sealing silence
    // before the live Output2 mixer has supplied its samples.
    const ChannelStream &output2 = state.channels[8u];
    const std::uint64_t unclamped = sealed_frame;
    sealed_frame = audio_output_master_seal_frame(
        guest_frame, sealed_frame, output2.active, output2.cursor,
        state.recovery_prebuffer_blocks * kBlockFrames);
    if (unclamped != sealed_frame) ++state.output2_seal_clamps;

    while (sealed_frame >= state.output_frame + kBlockFrames) {
        if (!queue_one_block(state)) break;
    }
}

void reset_channel_locked(AudioState &state, std::uint32_t channel) {
    if (channel >= state.channels.size()) return;
    state.channels[channel] = ChannelStream{};
}

} // namespace

bool audio_output_enabled() {
    static const bool enabled = [] {
        if (const char *text = std::getenv("PSPRECOMP_AUDIO"))
            return *text != '\0' && std::string(text) != "0";
        const VcsConfiguration &configuration = vcs_configuration();
        return !configuration.initialized || configuration.audio.enabled;
    }();
    return enabled;
}

void audio_output_submit(std::span<const std::int16_t> pcm, std::uint32_t frames,
                         bool stereo, std::uint32_t left, std::uint32_t right,
                         std::uint32_t source_rate, std::uint32_t channel,
                         std::uint64_t scheduled_time_us, std::uint64_t current_time_us) {
    if (!audio_output_enabled() || frames == 0u || channel >= kGuestChannels) return;
    if (source_rate == 0u) source_rate = kSampleRate;
    const std::size_t needed = static_cast<std::size_t>(frames) * (stereo ? 2u : 1u);
    if (pcm.size() < needed) return;
    const bool measure_submit = summary_diagnostics_enabled();
    const auto submit_started = measure_submit
        ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};

    AudioState &state = audio_state();
    std::lock_guard<std::mutex> guard(state.mutex);
    if (!ensure_device(state)) return;

    if (!state.timeline_anchored) {
        state.guest_anchor_us = current_time_us;
        state.timeline_anchored = true;
        state.output_frame = 0u;
    }

    // Seal old timeline regions before adding the new buffer.  Once virtual
    // time has advanced past them no later PSP thread can legitimately submit
    // audio into those frames.
    advance_locked(state, current_time_us);

    ChannelStream &stream = state.channels[channel];
    const std::uint64_t scheduled = guest_frame_for(state, scheduled_time_us);
    const auto distance = [](std::uint64_t a, std::uint64_t b) {
        return a > b ? a - b : b - a;
    };
    const bool format_changed = stream.active &&
        (stream.source_rate != source_rate || stream.stereo != stereo);
    const bool discontinuity = stream.active &&
        distance(stream.cursor, scheduled) > kChannelDiscontinuityFrames;
    const std::uint64_t previous_cursor = stream.cursor;
    if (!stream.active || format_changed || discontinuity) {
        stream = ChannelStream{};
        stream.active = true;
        stream.source_rate = source_rate;
        stream.stereo = stereo;
        stream.resampler.reset(source_rate, stereo);
        stream.cursor = std::max(scheduled, state.output_frame);
        if (discontinuity) ++state.timeline_resyncs;
        if (diagnostics_enabled() && discontinuity)
            std::cerr << "[audio-host] channel " << channel << " timeline resync old="
                      << previous_cursor << " scheduled=" << scheduled << "\n";
    }

    if (stream.cursor < state.output_frame) {
        state.late_frames_dropped += state.output_frame - stream.cursor;
        stream.cursor = state.output_frame;
        stream.resampler.reset(source_rate, stereo);
    }

    const std::uint32_t master = vcs_configuration().audio.volume;
    const std::int64_t left_gain = (static_cast<std::int64_t>(left) * master) / 100;
    const std::int64_t right_gain = (static_cast<std::int64_t>(right) * master) / 100;
    const std::uint64_t ring_limit = state.output_frame + kRingFrames - kBlockFrames;

    stream.resampler.process(pcm, frames, stereo, source_rate,
        [&](std::int16_t source_left, std::int16_t source_right) {
            if (stream.cursor >= ring_limit) {
                ++state.overrun_frames_dropped;
                ++stream.cursor;
                return;
            }
            const std::size_t slot =
                static_cast<std::size_t>(stream.cursor % kRingFrames) * kOutputChannels;
            const std::int64_t mixed_left =
                (static_cast<std::int64_t>(source_left) * left_gain) >> 15;
            const std::int64_t mixed_right =
                (static_cast<std::int64_t>(source_right) * right_gain) >> 15;
            state.ring[slot] += static_cast<std::int32_t>(std::clamp<std::int64_t>(
                mixed_left, std::numeric_limits<std::int32_t>::min(),
                std::numeric_limits<std::int32_t>::max()));
            state.ring[slot + 1u] += static_cast<std::int32_t>(std::clamp<std::int64_t>(
                mixed_right, std::numeric_limits<std::int32_t>::min(),
                std::numeric_limits<std::int32_t>::max()));
            ++stream.cursor;
        });

    stream.last_guest_time_us = scheduled_time_us;
    // A submission can make enough older samples complete to fill another
    // device block, so try once more after mixing it.
    advance_locked(state, current_time_us);
    if (measure_submit) {
        const std::uint64_t submit_ns = static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - submit_started).count());
        ++state.submit_calls;
        state.submit_cpu_ns += submit_ns;
        state.submit_cpu_max_ns = std::max(state.submit_cpu_max_ns, submit_ns);
    }
}

void audio_output_advance(std::uint64_t guest_time_us) {
    if (!audio_output_enabled()) return;
    AudioState &state = audio_state();
    std::lock_guard<std::mutex> guard(state.mutex);
    if (!state.opened) return;
    advance_locked(state, guest_time_us);
    if (summary_diagnostics_enabled() &&
        (state.last_summary_guest_us == 0u ||
         guest_time_us - state.last_summary_guest_us >= 2'000'000u)) {
        const std::uint64_t guest_frame = guest_frame_for(state, guest_time_us);
        const std::size_t outstanding = outstanding_blocks(state);
        const std::uint64_t average_submit_us = state.submit_calls == 0u ? 0u
            : state.submit_cpu_ns / state.submit_calls / 1000u;
        std::ostringstream line;
        line << "[audio-summary] guest_us=" << guest_time_us
             << " guest_frame=" << guest_frame
             << " output_frame=" << state.output_frame
             << " outstanding_blocks=" << outstanding
             << " playback=" << state.playback_started
             << " recovering=" << state.recovering_from_underrun
             << " underrun_rebuffers=" << state.underrun_rebuffers
             << " resyncs=" << state.timeline_resyncs
             << " output2_clamps=" << state.output2_seal_clamps
             << " latency_recoveries=" << state.latency_recoveries
             << " stale_frames_skipped=" << state.stale_frames_skipped
             << " lag_estimate_ms=" << (guest_frame > state.output_frame
                 ? (guest_frame - state.output_frame + outstanding * kBlockFrames) * 1000u / kSampleRate
                 : (outstanding * kBlockFrames > state.output_frame - guest_frame
                    ? (outstanding * kBlockFrames - (state.output_frame - guest_frame)) * 1000u / kSampleRate : 0u))
             << " late_frames=" << state.late_frames_dropped
             << " overrun_frames=" << state.overrun_frames_dropped
             << " submit_calls=" << state.submit_calls
             << " submit_avg_us=" << average_submit_us
             << " submit_max_us=" << state.submit_cpu_max_ns / 1000u << "\n";
        std::cerr << line.str();
        if (state.diagnostics_log) {
            state.diagnostics_log << line.str();
            state.diagnostics_log.flush();
        }
        state.last_summary_guest_us = guest_time_us;
    }
}

void audio_output_reset_channel(std::uint32_t channel) {
    AudioState &state = audio_state();
    std::lock_guard<std::mutex> guard(state.mutex);
    reset_channel_locked(state, channel);
}

void audio_output_shutdown() {
    AudioState &state = audio_state();
    std::lock_guard<std::mutex> guard(state.mutex);
    if (!state.opened || state.device == nullptr) return;

    // Start a paused device before reset on drivers that otherwise leave queued
    // WAVEHDRs in an indeterminate state during teardown.
    if (!state.playback_started) (void)waveOutRestart(state.device);
    (void)waveOutReset(state.device);
    for (Block &block : state.blocks) {
        if ((block.header.dwFlags & WHDR_PREPARED) != 0u)
            (void)waveOutUnprepareHeader(state.device, &block.header, sizeof(WAVEHDR));
    }
    (void)waveOutClose(state.device);
    close_wav_capture(state);
    if (state.diagnostics_log.is_open()) state.diagnostics_log.close();

    state.device = nullptr;
    state.opened = false;
    state.blocks.clear();
    state.ring.clear();
    state.timeline_anchored = false;
    state.playback_started = false;
    state.recovering_from_underrun = false;
    state.queued_blocks = 0u;
    state.output_frame = 0u;
    state.last_summary_guest_us = 0u;
    for (std::uint32_t channel = 0u; channel < kGuestChannels; ++channel)
        reset_channel_locked(state, channel);

    if (diagnostics_enabled() && (state.late_frames_dropped != 0u || state.overrun_frames_dropped != 0u)) {
        std::cerr << "[audio-host] shutdown late_frames=" << state.late_frames_dropped
                  << " overrun_frames=" << state.overrun_frames_dropped << "\n";
    }
    state.late_frames_dropped = 0u;
    state.overrun_frames_dropped = 0u;
    state.latency_recoveries = 0u;
    state.stale_frames_skipped = 0u;
    state.output2_seal_clamps = 0u;
}

} // namespace vcs

#else

namespace vcs {

bool audio_output_enabled() { return false; }
void audio_output_submit(std::span<const std::int16_t>, std::uint32_t, bool,
                         std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t,
                         std::uint64_t, std::uint64_t) {}
void audio_output_advance(std::uint64_t) {}
void audio_output_reset_channel(std::uint32_t) {}
void audio_output_shutdown() {}

} // namespace vcs

#endif
