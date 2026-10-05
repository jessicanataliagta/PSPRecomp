#include "audio_stream_decoder.hpp"

#include <algorithm>
#include <cstdio>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

namespace psprecomp::hle {
namespace {

// Both decoders pull one packet at a time and keep whatever the decoder hands
// back in a buffer, because the caller asks for byte counts that have nothing
// to do with frame boundaries -- it is draining a stream, exactly as it did
// from the pipe.
struct DecodeCommon {
    AVFormatContext *format{};
    AVCodecContext *codec{};
    AVPacket *packet{};
    AVFrame *frame{};
    int stream_index{-1};
    bool eof{};
    std::vector<std::uint8_t> pending;
    std::size_t pending_read{};

    ~DecodeCommon() { release(); }

    void release() noexcept {
        if (frame != nullptr) av_frame_free(&frame);
        if (packet != nullptr) av_packet_free(&packet);
        if (codec != nullptr) avcodec_free_context(&codec);
        if (format != nullptr) avformat_close_input(&format);
        stream_index = -1;
        eof = false;
        pending.clear();
        pending_read = 0u;
    }

    [[nodiscard]] bool open_stream(const std::filesystem::path &path, AVMediaType type) {
        release();
        if (avformat_open_input(&format, path.string().c_str(), nullptr, nullptr) < 0) return false;
        if (avformat_find_stream_info(format, nullptr) < 0) return false;
        const AVCodec *decoder = nullptr;
        stream_index = av_find_best_stream(format, type, -1, -1, &decoder, 0);
        if (stream_index < 0 || decoder == nullptr) return false;
        codec = avcodec_alloc_context3(decoder);
        if (codec == nullptr) return false;
        if (avcodec_parameters_to_context(codec, format->streams[stream_index]->codecpar) < 0)
            return false;
        if (avcodec_open2(codec, decoder, nullptr) < 0) return false;
        packet = av_packet_alloc();
        frame = av_frame_alloc();
        return packet != nullptr && frame != nullptr;
    }

    // Hands the next decoded frame to `consume`, which appends its bytes to
    // `pending`. Returns false once the file and the decoder are both drained.
    template <typename Consume>
    bool decode_one(Consume &&consume) {
        for (;;) {
            const int received = avcodec_receive_frame(codec, frame);
            if (received == 0) {
                consume(frame);
                av_frame_unref(frame);
                return true;
            }
            if (received != AVERROR(EAGAIN) && received != AVERROR_EOF) return false;
            if (received == AVERROR_EOF) return false;
            if (eof) {
                // Flush: a decoder can be holding frames after the last packet.
                if (avcodec_send_packet(codec, nullptr) < 0) return false;
                const int flushed = avcodec_receive_frame(codec, frame);
                if (flushed < 0) return false;
                consume(frame);
                av_frame_unref(frame);
                return true;
            }
            const int read = av_read_frame(format, packet);
            if (read < 0) {
                eof = true;
                continue;
            }
            if (packet->stream_index != stream_index) {
                av_packet_unref(packet);
                continue;
            }
            const int sent = avcodec_send_packet(codec, packet);
            av_packet_unref(packet);
            if (sent < 0 && sent != AVERROR(EAGAIN)) return false;
        }
    }

    // Serves `output` out of `pending`, refilling through `refill` as needed.
    template <typename Refill>
    std::size_t drain(std::span<std::uint8_t> output, Refill &&refill) {
        std::size_t written = 0u;
        while (written < output.size()) {
            if (pending_read >= pending.size()) {
                pending.clear();
                pending_read = 0u;
                if (!refill()) break;
                if (pending.empty()) break;
            }
            const std::size_t available = pending.size() - pending_read;
            const std::size_t take = std::min(available, output.size() - written);
            std::copy_n(pending.begin() + static_cast<std::ptrdiff_t>(pending_read), take,
                        output.begin() + static_cast<std::ptrdiff_t>(written));
            pending_read += take;
            written += take;
        }
        return written;
    }
};

} // namespace

// ---------------------------------------------------------------------------

struct AudioStreamDecoder::State {
    DecodeCommon common;
    SwrContext *resampler{};
    std::uint32_t sample_rate{};
    std::uint32_t channels{};

    ~State() {
        if (resampler != nullptr) swr_free(&resampler);
    }
};

AudioStreamDecoder::AudioStreamDecoder() : state_(std::make_unique<State>()) {}
AudioStreamDecoder::~AudioStreamDecoder() = default;
AudioStreamDecoder::AudioStreamDecoder(AudioStreamDecoder &&) noexcept = default;
AudioStreamDecoder &AudioStreamDecoder::operator=(AudioStreamDecoder &&) noexcept = default;

bool AudioStreamDecoder::is_open() const noexcept { return state_->common.codec != nullptr; }

void AudioStreamDecoder::close() noexcept {
    if (state_->resampler != nullptr) swr_free(&state_->resampler);
    state_->common.release();
}

bool AudioStreamDecoder::open(const std::filesystem::path &path, std::uint32_t sample_rate,
                              std::uint32_t channels, std::uint64_t start_sample) {
    close();
    if (sample_rate == 0u || channels == 0u) return false;
    if (!state_->common.open_stream(path, AVMEDIA_TYPE_AUDIO)) return false;
    state_->sample_rate = sample_rate;
    state_->channels = channels;

    // Always resample: ATRAC3+ decodes to planar float, and the guest wants
    // interleaved signed 16-bit at the rate its own header declares.
    AVChannelLayout out_layout{};
    av_channel_layout_default(&out_layout, static_cast<int>(channels));
    if (swr_alloc_set_opts2(&state_->resampler, &out_layout, AV_SAMPLE_FMT_S16,
                            static_cast<int>(sample_rate), &state_->common.codec->ch_layout,
                            state_->common.codec->sample_fmt,
                            state_->common.codec->sample_rate, 0, nullptr) < 0) {
        av_channel_layout_uninit(&out_layout);
        return false;
    }
    av_channel_layout_uninit(&out_layout);
    if (swr_init(state_->resampler) < 0) return false;

    if (start_sample != 0u) {
        // Seeking is in the stream's time base, not in samples.
        const AVStream *stream = state_->common.format->streams[state_->common.stream_index];
        const std::int64_t timestamp = av_rescale_q(
            static_cast<std::int64_t>(start_sample), AVRational{1, static_cast<int>(sample_rate)},
            stream->time_base);
        if (av_seek_frame(state_->common.format, state_->common.stream_index, timestamp,
                          AVSEEK_FLAG_BACKWARD) >= 0) {
            avcodec_flush_buffers(state_->common.codec);
        }
    }
    return true;
}

std::size_t AudioStreamDecoder::read(std::span<std::uint8_t> output) {
    if (!is_open()) return 0u;
    State &state = *state_;
    return state.common.drain(output, [&state]() -> bool {
        return state.common.decode_one([&state](AVFrame *frame) {
            const int out_samples = static_cast<int>(av_rescale_rnd(
                swr_get_delay(state.resampler, frame->sample_rate) + frame->nb_samples,
                static_cast<std::int64_t>(state.sample_rate), frame->sample_rate, AV_ROUND_UP));
            const std::size_t bytes =
                static_cast<std::size_t>(out_samples) * state.channels * sizeof(std::int16_t);
            state.common.pending.resize(bytes);
            std::uint8_t *destination = state.common.pending.data();
            const int converted = swr_convert(state.resampler, &destination, out_samples,
                                              const_cast<const std::uint8_t **>(frame->data),
                                              frame->nb_samples);
            state.common.pending.resize(converted <= 0 ? 0u :
                static_cast<std::size_t>(converted) * state.channels * sizeof(std::int16_t));
        });
    });
}

} // namespace psprecomp::hle
