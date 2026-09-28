#pragma once
#include <cstdint>

namespace vcs {
inline bool stream_request_needs_new_source(std::uint32_t current, std::uint32_t requested) noexcept {
    return current != requested;
}
inline std::uint32_t stream_open_event_bits(std::uint32_t current, std::uint32_t requested) noexcept {
    // OPEN=1, STOP=4. STOP also cancels an old in-progress refill before OPEN.
    return stream_request_needs_new_source(current, requested) ? 5u : 1u;
}
void trace_stream_request(std::uint32_t current, std::uint32_t requested, std::uint32_t events);
// Host-only diagnostics; never used to decide guest audio state.
inline std::uint64_t audio_pending_start_cancellations{};
inline std::uint64_t audio_pending_owner_protections{};
inline std::uint32_t cancel_pending_audio_start(std::uint32_t starts, std::uint32_t stops) noexcept {
    return starts & ~stops;
}

inline std::uint32_t stop_queued_audio_start(std::uint32_t starts, std::uint32_t stops) noexcept {
    if ((starts & stops) != 0u) ++audio_pending_start_cancellations;
    return cancel_pending_audio_start(starts, stops);
}

inline std::uint32_t audio_end_flags_with_pending_starts(
    std::uint32_t end_flags, std::uint32_t producer_low, std::uint32_t producer_high,
    std::uint32_t consumer_low, std::uint32_t consumer_high, bool batch_ready) noexcept {
    const auto pending_low = producer_low | (batch_ready ? consumer_low : 0u);
    const auto pending_high = producer_high | (batch_ready ? consumer_high : 0u);
    const auto pending = (pending_low & 0xFFFFFFu) | ((pending_high & 0xFu) << 24u);
    if ((end_flags & pending) != 0u) ++audio_pending_owner_protections;
    return end_flags & ~pending;
}

// VCS packs its 28 hardware channels into two 24-bit command words, not
// two consecutive 32-bit words. Only channels after the failing channel have
// not reached the normal KeyOff call when 0x0880AB48 aborts the batch.
inline std::uint32_t remaining_audio_stops(std::uint32_t low, std::uint32_t high,
                                         std::uint32_t failed_channel) noexcept {
    if (failed_channel >= 27u) return 0u;
    const auto mask = (low & 0x00FFFFFFu) | ((high & 0xFu) << 24u);
    return mask & (0x0FFFFFFFu << (failed_channel + 1u));
}
void finish_aborted_audio_stops(std::uint32_t low, std::uint32_t high,
                                std::uint32_t failed_channel);
}
