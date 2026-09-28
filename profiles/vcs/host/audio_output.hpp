#pragma once

#include <cstdint>
#include <span>

namespace vcs {

// sceAudioOutput2 is the game's continuous final music/radio producer.  When
// virtual time runs ahead of that producer, sealing future host frames would
// make the gap irreversible.  Clamp only while the producer is within the
// bounded device-prebuffer grace window; if it truly stalls beyond that, the
// rest of the PSP channels may advance normally.
[[nodiscard]] constexpr std::uint64_t audio_output_master_seal_frame(
    std::uint64_t guest_frame, std::uint64_t sealed_frame, bool producer_active,
    std::uint64_t producer_cursor, std::uint64_t grace_frames) noexcept {
    if (!producer_active) return sealed_frame;
    if (guest_frame > producer_cursor + grace_frames) return sealed_frame;
    return sealed_frame > producer_cursor ? producer_cursor : sealed_frame;
}

// Host audio sink for the sceAudio HLE.  The PSP exposes eight regular PCM
// channels plus one SRC/Output2 channel; submissions are mixed on the guest's
// virtual-time line before they are handed to the native audio device.
[[nodiscard]] bool audio_output_enabled();

// Mix a buffer at scheduled_time_us on the guest timeline. current_time_us is
// the actual PSP clock at submission: a blocking call can enqueue a FUTURE
// buffer, which must not advance/seal the timeline for other channels.
void audio_output_submit(std::span<const std::int16_t> pcm, std::uint32_t frames,
                         bool stereo, std::uint32_t left, std::uint32_t right,
                         std::uint32_t source_rate, std::uint32_t channel,
                         std::uint64_t scheduled_time_us, std::uint64_t current_time_us);

// Seal and queue audio whose guest time is safely in the past.  Call this from
// the vblank path even on frames where the game submitted no new audio so the
// native device keeps receiving silence rather than underrunning.
void audio_output_advance(std::uint64_t guest_time_us);

// Forget stream/resampler continuity for one PSP channel (release/re-reserve).
void audio_output_reset_channel(std::uint32_t channel);

// Releases the device. Safe to call when nothing was ever opened.
void audio_output_shutdown();

} // namespace vcs
