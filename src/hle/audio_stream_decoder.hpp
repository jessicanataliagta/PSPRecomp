#pragma once

// In-process FFmpeg audio decoding for the ATRAC HLE: open a file once, then
// pull interleaved signed 16-bit PCM until the stream ends. Copied from the VCS
// profile's media decoder (profiles/vcs/host/vcs_media_decoder.cpp), which keeps
// its own copy for PMF audio and video.

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>

namespace psprecomp::hle {

class AudioStreamDecoder {
public:
    AudioStreamDecoder();
    ~AudioStreamDecoder();
    AudioStreamDecoder(const AudioStreamDecoder &) = delete;
    AudioStreamDecoder &operator=(const AudioStreamDecoder &) = delete;
    // Movable: the HLE resets a context with `state = AtracContextState{}`.
    AudioStreamDecoder(AudioStreamDecoder &&) noexcept;
    AudioStreamDecoder &operator=(AudioStreamDecoder &&) noexcept;

    // start_sample seeks before the first read.
    [[nodiscard]] bool open(const std::filesystem::path &path, std::uint32_t sample_rate,
                            std::uint32_t channels, std::uint64_t start_sample);
    // Returns bytes written; less than the span means the stream ended.
    [[nodiscard]] std::size_t read(std::span<std::uint8_t> output);
    [[nodiscard]] bool is_open() const noexcept;
    void close() noexcept;

private:
    struct State;
    std::unique_ptr<State> state_;
};

} // namespace psprecomp::hle
