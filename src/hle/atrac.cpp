#include "psprecomp/hle/atrac.hpp"

#include "audio_stream_decoder.hpp"
#include "psprecomp/common.hpp"
#include "psprecomp/hle/io.hpp"
#include "psprecomp/hle/kernel.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <string>
#include <utility>
#include <vector>

// sceAtrac3plus moved from the VCS profile host (profiles/vcs/host/vcs_profile.cpp).
// The guest's stream buffer is only book-kept: the RIFF header it hands over is
// matched against the .AT3/.AA3/.OMA files on the virtual disc, and that file is
// decoded with FFmpeg. Both Rockstar Leeds titles (VCS, CTW) stream music from
// plain AT3 files this way.

namespace psprecomp::hle {
namespace {

struct ParsedAtracHeader {
    std::uint16_t format_tag{};
    std::uint16_t channels{};
    std::uint32_t sample_rate{};
    std::uint32_t average_bytes_per_second{};
    std::uint16_t block_align{};
    std::uint16_t bits_per_sample{};
    std::uint32_t data_offset{};
    std::uint32_t data_size{};
    std::uint32_t file_size{};
    std::uint32_t total_samples{};
    std::int32_t loop_start{-1};
    std::int32_t loop_end{-1};
    bool atrac3plus{};
};

struct AtracContextState {
    bool allocated{};
    ParsedAtracHeader header{};
    std::uint32_t buffer_address{};
    std::uint32_t initial_read_size{};
    std::uint32_t buffer_size{};
    std::uint32_t buffered_encoded_bytes{};
    std::uint32_t next_file_offset{};
    std::uint32_t write_offset{};
    std::uint32_t last_writable_bytes{};
    std::uint64_t sample_position{};
    std::int32_t loop_num{};
    std::uint32_t internal_error{};
    std::filesystem::path source_path;
    AudioStreamDecoder decoder;
    bool decoder_eof{};
};

std::uint16_t read_le16(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint16_t>(bytes[offset]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

std::uint32_t read_le32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint32_t>(bytes[offset]) |
        (static_cast<std::uint32_t>(bytes[offset + 1u]) << 8u) |
        (static_cast<std::uint32_t>(bytes[offset + 2u]) << 16u) |
        (static_cast<std::uint32_t>(bytes[offset + 3u]) << 24u);
}

bool parse_atrac_header(std::span<const std::uint8_t> bytes, ParsedAtracHeader &header) {
    if (bytes.size() < 12u || std::memcmp(bytes.data(), "RIFF", 4u) != 0 ||
        std::memcmp(bytes.data() + 8u, "WAVE", 4u) != 0) return false;
    const std::uint64_t declared_file_size = static_cast<std::uint64_t>(read_le32(bytes, 4u)) + 8u;
    if (declared_file_size > 0xFFFFFFFFull) return false;
    header = ParsedAtracHeader{};
    header.file_size = static_cast<std::uint32_t>(declared_file_size);
    bool have_fmt = false;
    bool have_data = false;
    for (std::size_t offset = 12u; offset + 8u <= bytes.size();) {
        const std::uint32_t chunk_size = read_le32(bytes, offset + 4u);
        const std::size_t payload = offset + 8u;
        const std::uint64_t next64 = static_cast<std::uint64_t>(payload) + chunk_size + (chunk_size & 1u);
        if (next64 > bytes.size()) {
            // A partial streaming buffer is valid as long as the chunk header is present.
            if (std::memcmp(bytes.data() + offset, "data", 4u) == 0) {
                header.data_offset = static_cast<std::uint32_t>(payload);
                header.data_size = chunk_size;
                have_data = true;
            }
            break;
        }
        if (std::memcmp(bytes.data() + offset, "fmt ", 4u) == 0 && chunk_size >= 16u) {
            header.format_tag = read_le16(bytes, payload + 0u);
            header.channels = read_le16(bytes, payload + 2u);
            header.sample_rate = read_le32(bytes, payload + 4u);
            header.average_bytes_per_second = read_le32(bytes, payload + 8u);
            header.block_align = read_le16(bytes, payload + 12u);
            header.bits_per_sample = read_le16(bytes, payload + 14u);
            have_fmt = true;
        } else if (std::memcmp(bytes.data() + offset, "fact", 4u) == 0 && chunk_size >= 4u) {
            header.total_samples = read_le32(bytes, payload);
        } else if (std::memcmp(bytes.data() + offset, "smpl", 4u) == 0 && chunk_size >= 60u) {
            const std::uint32_t loop_count = read_le32(bytes, payload + 28u);
            if (loop_count != 0u && chunk_size >= 60u) {
                header.loop_start = static_cast<std::int32_t>(read_le32(bytes, payload + 44u));
                header.loop_end = static_cast<std::int32_t>(read_le32(bytes, payload + 48u));
            }
        } else if (std::memcmp(bytes.data() + offset, "data", 4u) == 0) {
            header.data_offset = static_cast<std::uint32_t>(payload);
            header.data_size = chunk_size;
            have_data = true;
        }
        offset = static_cast<std::size_t>(next64);
    }
    if (!have_fmt || !have_data || header.channels == 0u || header.channels > 2u ||
        header.sample_rate == 0u || header.block_align == 0u) return false;
    // PSP ATRAC files use WAVE_FORMAT_EXTENSIBLE (0xFFFE) or the legacy ATRAC3 tag.
    header.atrac3plus = header.format_tag == 0xFFFEu && header.block_align >= 0x180u;
    if (!header.atrac3plus && header.format_tag != 0x0270u && header.format_tag != 0xFFFEu) return false;
    if (header.total_samples == 0u) {
        const std::uint32_t samples_per_frame = header.atrac3plus ? 2048u : 1024u;
        header.total_samples = (header.data_size / header.block_align) * samples_per_frame;
    }
    return true;
}

bool is_atrac_file(const std::filesystem::path &path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
    return extension == ".AT3" || extension == ".AA3" || extension == ".OMA";
}

// Every ATRAC file under the game root, found once. The virtual-disc table only
// lists files the guest opened by sector (VCS); CTW opens its music by path.
const std::vector<std::pair<std::filesystem::path, std::uint64_t>> &game_atrac_files(
    const std::filesystem::path &game_root) {
    static std::filesystem::path scanned_root;
    static std::vector<std::pair<std::filesystem::path, std::uint64_t>> files;
    if (scanned_root != game_root) {
        scanned_root = game_root;
        files.clear();
        std::error_code error;
        for (std::filesystem::recursive_directory_iterator it(game_root, error), end; !error && it != end;
             it.increment(error)) {
            if (!it->is_regular_file(error) || !is_atrac_file(it->path())) continue;
            const std::uint64_t size = it->file_size(error);
            if (!error) files.emplace_back(it->path(), size);
        }
    }
    return files;
}

// Matches the bytes the guest loaded (the whole first read, which also tells
// apart same-size variants such as CTW's CITY_LONGLOOP8BIT and _WITHRAIN)
// against the files of the declared size.
std::filesystem::path identify_atrac_source(const psprecomp::Runtime &runtime, std::span<const std::uint8_t> header,
                                            const ParsedAtracHeader &parsed) {
    const auto matches = [&](const std::filesystem::path &path) {
        std::vector<std::uint8_t> candidate(header.size());
        std::ifstream input(path, std::ios::binary);
        if (!input) return false;
        input.read(reinterpret_cast<char *>(candidate.data()), static_cast<std::streamsize>(candidate.size()));
        return input.gcount() == static_cast<std::streamsize>(candidate.size()) &&
            std::equal(candidate.begin(), candidate.end(), header.begin());
    };
    for (const auto &[key, file] : file_table.virtual_files_by_path) {
        if (file.size == parsed.file_size && is_atrac_file(file.native_path) && matches(file.native_path))
            return file.native_path;
    }
    if (runtime.game_root().empty()) return {};
    for (const auto &[path, size] : game_atrac_files(runtime.game_root())) {
        if (size == parsed.file_size && matches(path)) return path;
    }
    return {};
}

// sceAtracDecodeData always hands the caller two interleaved channels: the PSP
// decoder upmixes a mono stream instead of returning half-width frames, and
// nothing in the API lets a game ask for anything else (this EBOOT does not
// even import sceAtracGetOutputChannel).  Emitting mono PCM for a mono file
// made the game read a stereo-sized buffer out of a half-filled one, so every
// mono stream -- VCPR and the NEWS_* bulletins, i.e. exactly the spoken
// stations -- ran at double speed while the stereo music stations were fine.
constexpr std::uint32_t kAtracOutputChannels = 2u;

void close_atrac_decoder(AtracContextState &state) {
    state.decoder.close();
    state.decoder_eof = false;
}

bool open_atrac_decoder(AtracContextState &state) {
    if (state.decoder.is_open()) return true;
    if (state.source_path.empty()) return false;
    // Clamp the seek to the stream. A reopen was observed at sample 159,114,619
    // on EMOTION.AT3 -- an hour of audio into a track a few minutes long -- which
    // sends the demuxer hunting past end of file for a position that cannot
    // exist. sample_position accumulates across decodes and nothing bounded it
    // here; whatever lets it run away is a separate bug, but the seek itself
    // must stay inside the file.
    std::uint64_t seek = state.sample_position;
    if (state.header.total_samples != 0u && seek > state.header.total_samples) {
        if (std::getenv("PSPRECOMP_ATRAC_DIAG") != nullptr)
            std::cerr << "[atrac] seek " << seek << " fora do stream (total="
                      << state.header.total_samples << "), limitado\n";
        seek = state.header.total_samples;
    }
    if (!state.decoder.open(state.source_path, state.header.sample_rate,
                            kAtracOutputChannels, seek))
        return false;
    state.decoder_eof = false;
    if (std::getenv("PSPRECOMP_ATRAC_DIAG") != nullptr)
        std::cerr << "[atrac] decoder opened id-source=\"" << state.source_path.string()
                  << "\" sample=" << state.sample_position << "\n";
    return true;
}

std::size_t read_atrac_pcm(AtracContextState &state, std::span<std::uint8_t> output) {
    if (!open_atrac_decoder(state)) return 0u;
    const std::size_t total = state.decoder.read(output);
    if (total < output.size()) state.decoder_eof = true;
    return total;
}

std::uint32_t atrac_samples_per_frame(const AtracContextState &state) {
    return state.header.atrac3plus ? 2048u : 1024u;
}

std::uint32_t atrac_bitrate_kbps(const AtracContextState &state) {
    if (state.header.atrac3plus) {
        const std::uint32_t raw = (static_cast<std::uint32_t>(state.header.block_align) * 352800u) / 1000u;
        return ((raw >> 11u) + 8u) & 0xFFFFFFF0u;
    }
    return (static_cast<std::uint32_t>(state.header.block_align) * 352800u / 1000u + 511u) >> 10u;
}

std::array<AtracContextState, 6> atrac_contexts{};

} // namespace

void reset_atrac() {
    for (auto &state : atrac_contexts) close_atrac_decoder(state);
    atrac_contexts = {};
}

void install_atrac_hle(psprecomp::Runtime &runtime) {
    constexpr std::uint32_t kAtracErrorApiFail = 0x80630002u;
    constexpr std::uint32_t kAtracErrorNoId = 0x80630003u;
    constexpr std::uint32_t kAtracErrorBadId = 0x80630005u;
    constexpr std::uint32_t kAtracErrorUnknownFormat = 0x80630006u;
    constexpr std::uint32_t kAtracErrorAllDataLoaded = 0x80630009u;
    constexpr std::uint32_t kAtracErrorNoData = 0x80630010u;
    constexpr std::uint32_t kAtracErrorIncorrectReadSize = 0x80630013u;
    constexpr std::uint32_t kAtracErrorBadAddress = 0x800200D3u;

    const auto get_atrac = [](std::uint32_t id) -> AtracContextState * {
        if (id >= atrac_contexts.size() || !atrac_contexts[id].allocated) return nullptr;
        return &atrac_contexts[id];
    };

    runtime.register_hle("sceAtrac3plus", 0x0FAE370Eu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t buffer = ctx.gpr[4];
            const std::uint32_t read_size = ctx.gpr[5];
            const std::uint32_t buffer_size = ctx.gpr[6];
            if (read_size > buffer_size) { ctx.set_gpr(2, kAtracErrorIncorrectReadSize); return; }
            if (read_size < 12u || !rt.memory().contains(buffer, read_size)) {
                ctx.set_gpr(2, kAtracErrorUnknownFormat); return;
            }
            std::vector<std::uint8_t> header_bytes(read_size);
            rt.memory().copy_out(buffer, header_bytes);
            ParsedAtracHeader parsed{};
            if (!parse_atrac_header(header_bytes, parsed)) {
                ctx.set_gpr(2, kAtracErrorUnknownFormat); return;
            }
            std::size_t id = atrac_contexts.size();
            for (std::size_t i = 0u; i < atrac_contexts.size(); ++i) {
                if (!atrac_contexts[i].allocated) { id = i; break; }
            }
            if (id == atrac_contexts.size()) { ctx.set_gpr(2, kAtracErrorNoId); return; }
            auto &state = atrac_contexts[id];
            close_atrac_decoder(state);
            state = AtracContextState{};
            state.allocated = true;
            state.header = parsed;
            state.buffer_address = buffer;
            state.initial_read_size = read_size;
            state.buffer_size = buffer_size;
            state.buffered_encoded_bytes = read_size > parsed.data_offset ? read_size - parsed.data_offset : 0u;
            state.buffered_encoded_bytes = std::min(state.buffered_encoded_bytes, parsed.data_size);
            state.next_file_offset = std::min(read_size, parsed.file_size);
            state.write_offset = buffer_size == 0u ? 0u : read_size % buffer_size;
            state.source_path = identify_atrac_source(rt, header_bytes, parsed);
            if (std::getenv("PSPRECOMP_ATRAC_DIAG") != nullptr) {
                std::cerr << "[atrac] set-halfway id=" << id
                          << " buffer=" << psprecomp::hex32(buffer)
                          << " read=" << read_size << " capacity=" << buffer_size
                          << " file=" << parsed.file_size << " frame=" << parsed.block_align
                          << " samples=" << parsed.total_samples
                          << " source=\"" << state.source_path.string() << "\"\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(id));
        });

    runtime.register_hle("sceAtrac3plus", 0x61EB33F5u,
        [get_atrac](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            close_atrac_decoder(*state);
            *state = AtracContextState{};
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x5D268707u,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::uint32_t write_ptr_addr = ctx.gpr[5];
            const std::uint32_t writable_addr = ctx.gpr[6];
            const std::uint32_t read_offset_addr = ctx.gpr[7];
            for (const std::uint32_t address : {write_ptr_addr, writable_addr, read_offset_addr}) {
                if (address != 0u && !rt.memory().contains(address, 4u)) {
                    ctx.set_gpr(2, kAtracErrorBadAddress); return;
                }
            }
            const std::uint32_t remaining_file = state->next_file_offset < state->header.file_size ?
                state->header.file_size - state->next_file_offset : 0u;
            const std::uint32_t free_bytes = state->buffer_size > state->buffered_encoded_bytes ?
                state->buffer_size - state->buffered_encoded_bytes : 0u;
            const std::uint32_t contiguous = state->buffer_size == 0u ? 0u : state->buffer_size - state->write_offset;
            const std::uint32_t writable = std::min({remaining_file, free_bytes, contiguous});
            state->last_writable_bytes = writable;
            if (write_ptr_addr != 0u) rt.memory().store32(write_ptr_addr, state->buffer_address + state->write_offset);
            if (writable_addr != 0u) rt.memory().store32(writable_addr, writable);
            if (read_offset_addr != 0u) rt.memory().store32(read_offset_addr, state->next_file_offset);
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x7DB31251u,
        [get_atrac](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::uint32_t bytes = ctx.gpr[5];
            if (state->next_file_offset >= state->header.file_size) {
                ctx.set_gpr(2, bytes == 0u ? 0u : kAtracErrorAllDataLoaded); return;
            }
            if (bytes > state->last_writable_bytes) {
                ctx.set_gpr(2, kAtracErrorIncorrectReadSize); return;
            }
            state->buffered_encoded_bytes = std::min(state->buffer_size, state->buffered_encoded_bytes + bytes);
            state->next_file_offset = std::min(state->header.file_size, state->next_file_offset + bytes);
            if (state->buffer_size != 0u) state->write_offset = (state->write_offset + bytes) % state->buffer_size;
            state->last_writable_bytes = 0u;
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x6A8C3CD5u,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::uint32_t output = ctx.gpr[5];
            const std::uint32_t samples_addr = ctx.gpr[6];
            const std::uint32_t finish_addr = ctx.gpr[7];
            const std::uint32_t remain_addr = ctx.gpr[8];
            for (const std::uint32_t address : {samples_addr, finish_addr, remain_addr}) {
                if (address != 0u && !rt.memory().contains(address, 4u)) {
                    ctx.set_gpr(2, kAtracErrorBadAddress); return;
                }
            }
            const std::uint32_t max_samples = atrac_samples_per_frame(*state);
            const std::size_t max_bytes =
                static_cast<std::size_t>(max_samples) * kAtracOutputChannels * 2u;
            if (output != 0u && !rt.memory().contains(output, max_bytes)) {
                ctx.set_gpr(2, kAtracErrorBadAddress); return;
            }
            if (state->source_path.empty()) {
                state->internal_error = kAtracErrorUnknownFormat;
                ctx.set_gpr(2, kAtracErrorApiFail); return;
            }
            auto restart_for_loop = [&]() -> bool {
                if (state->loop_num == 0) return false;
                // A stream with no loop region must not be restarted. The line
                // below fell back to sample 0 when loop_start was negative, so a
                // clip that simply ended -- a radio news bulletin -- was played
                // again from the top instead of finishing and handing the
                // station back to the music. loop_num survives in a reused
                // context, so the bulletin inherited the music's loop.
                if (state->header.loop_start < 0) {
                    if (std::getenv("PSPRECOMP_ATRAC_DIAG") != nullptr)
                        std::cerr << "[atrac] fim de stream sem regiao de loop: "
                                  << state->source_path.filename().string()
                                  << " (loop_num=" << state->loop_num
                                  << " ignorado)\n";
                    return false;
                }
                if (state->loop_num > 0) --state->loop_num;
                state->sample_position = state->header.loop_start >= 0 ?
                    static_cast<std::uint32_t>(state->header.loop_start) : 0u;
                close_atrac_decoder(*state);
                return open_atrac_decoder(*state);
            };
            if (state->sample_position >= state->header.total_samples && !restart_for_loop()) {
                if (samples_addr != 0u) rt.memory().store32(samples_addr, 0u);
                if (finish_addr != 0u) rt.memory().store32(finish_addr, 1u);
                if (remain_addr != 0u) rt.memory().store32(remain_addr, 0u);
                set_success(ctx);
                return;
            }
            const std::uint32_t requested_samples = static_cast<std::uint32_t>(std::min<std::uint64_t>(
                max_samples, state->header.total_samples - state->sample_position));
            // Reused across calls. This was a fresh std::vector every
            // sceAtracDecodeData -- allocate, zero a few kilobytes, decode into
            // it, free -- on the hottest audio import there is, and the radio
            // runs two of these at once whenever a news bulletin plays over the
            // music. The buffer only ever grows, and only one guest audio thread
            // reaches this import at a time.
            static std::vector<std::uint8_t> pcm;
            const std::size_t pcm_bytes =
                static_cast<std::size_t>(requested_samples) * kAtracOutputChannels * 2u;
            if (pcm.size() < pcm_bytes) pcm.resize(pcm_bytes);
            const std::span<std::uint8_t> pcm_span(pcm.data(), pcm_bytes);
            static const bool audio_summary_enabled = [] {
                const char *text = std::getenv("PSPRECOMP_AUDIO_SUMMARY");
                return text != nullptr && *text != '\0' && std::strcmp(text, "0") != 0;
            }();
            const auto decode_started = audio_summary_enabled
                ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
            std::size_t got = read_atrac_pcm(*state, pcm_span);
            if (got == 0u && restart_for_loop()) {
                got = read_atrac_pcm(*state, pcm_span);
            }
            if (audio_summary_enabled) {
                static std::uint64_t decode_calls = 0u;
                static std::uint64_t decode_total_ns = 0u;
                static std::uint64_t decode_max_ns = 0u;
                const std::uint64_t elapsed_ns = static_cast<std::uint64_t>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::steady_clock::now() - decode_started).count());
                ++decode_calls;
                decode_total_ns += elapsed_ns;
                decode_max_ns = std::max(decode_max_ns, elapsed_ns);
                if ((decode_calls & 255u) == 0u) {
                    std::cerr << "[atrac-summary] calls=" << decode_calls
                              << " avg_us=" << decode_total_ns / decode_calls / 1000u
                              << " max_us=" << decode_max_ns / 1000u << "\n";
                }
            }
            const std::size_t bytes_per_sample = static_cast<std::size_t>(kAtracOutputChannels) * 2u;
            const std::uint32_t samples = static_cast<std::uint32_t>(got / bytes_per_sample);
            got = static_cast<std::size_t>(samples) * bytes_per_sample;
            if (output != 0u && got != 0u) rt.memory().copy_in(output, std::span<const std::uint8_t>(pcm.data(), got));
            state->sample_position += samples;
            if (state->buffered_encoded_bytes >= state->header.block_align)
                state->buffered_encoded_bytes -= state->header.block_align;
            else
                state->buffered_encoded_bytes = 0u;
            const bool finished = samples == 0u ||
                (state->sample_position >= state->header.total_samples && state->loop_num == 0);
            const std::uint32_t remaining_frames = state->header.block_align == 0u ? 0u :
                state->buffered_encoded_bytes / state->header.block_align;
            if (samples_addr != 0u) rt.memory().store32(samples_addr, samples);
            if (finish_addr != 0u) rt.memory().store32(finish_addr, finished ? 1u : 0u);
            if (remain_addr != 0u) rt.memory().store32(remain_addr, remaining_frames);
            if (std::getenv("PSPRECOMP_ATRAC_DIAG") != nullptr) {
                std::cerr << "[atrac] decode id=" << ctx.gpr[4] << " samples=" << samples
                          << " stream_channels=" << state->header.channels
                          << " stream_rate=" << state->header.sample_rate
                          << " position=" << state->sample_position << " finish=" << finished
                          << " buffered_frames=" << remaining_frames << "\n";
            }
            // Decoding above is synchronous host work.  Delaying the guest
            // audio thread by another 2300 us double-counted that work and,
            // once the city became busy, made it miss its 512-frame feeding
            // cadence.  sceAudioOutput2OutputBlocking already provides the
            // hardware pacing at the end of the pipeline; ATRAC decode itself
            // must return as soon as its PCM is ready.
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x9AE849A7u,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
            const std::uint32_t remaining = state->next_file_offset >= state->header.file_size ? 0xFFFFFFFFu :
                state->buffered_encoded_bytes / state->header.block_align;
            rt.memory().store32(ctx.gpr[5], remaining);
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0xA554A158u,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
            rt.memory().store32(ctx.gpr[5], atrac_bitrate_kbps(*state));
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0xA2BBA8BEu,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::array<std::pair<std::uint32_t, std::uint32_t>, 3> outputs{{
                {ctx.gpr[5], state->header.total_samples == 0u ? 0u : state->header.total_samples - 1u},
                {ctx.gpr[6], static_cast<std::uint32_t>(state->header.loop_start)},
                {ctx.gpr[7], static_cast<std::uint32_t>(state->header.loop_end)},
            }};
            for (const auto &[address, value] : outputs) {
                if (address != 0u) {
                    if (!rt.memory().contains(address, 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
                    rt.memory().store32(address, value);
                }
            }
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0xFAA4F89Bu,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (ctx.gpr[5] != 0u) {
                if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
                rt.memory().store32(ctx.gpr[5], static_cast<std::uint32_t>(state->loop_num));
            }
            if (ctx.gpr[6] != 0u) {
                if (!rt.memory().contains(ctx.gpr[6], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
                rt.memory().store32(ctx.gpr[6], state->header.loop_start >= 0 ? 1u : 0u);
            }
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x868120B5u,
        [get_atrac](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            state->loop_num = static_cast<std::int32_t>(ctx.gpr[5]);
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0xE88F759Bu,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (ctx.gpr[5] != 0u) {
                if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
                rt.memory().store32(ctx.gpr[5], state->internal_error);
            }
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x2DD3E298u,
        [get_atrac](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::uint32_t sample = ctx.gpr[5];
            const std::uint32_t info = ctx.gpr[6];
            if (!rt.memory().contains(info, 32u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
            const std::uint32_t frame = sample / atrac_samples_per_frame(*state);
            const std::uint64_t pos64 = static_cast<std::uint64_t>(state->header.data_offset) +
                static_cast<std::uint64_t>(frame) * state->header.block_align;
            const std::uint32_t file_pos = static_cast<std::uint32_t>(std::min<std::uint64_t>(pos64, state->header.file_size));
            const std::uint32_t writable = std::min(state->buffer_size, state->header.file_size - file_pos);
            rt.memory().store32(info + 0u, state->buffer_address);
            rt.memory().store32(info + 4u, writable);
            rt.memory().store32(info + 8u, std::min<std::uint32_t>(writable, state->header.block_align));
            rt.memory().store32(info + 12u, file_pos);
            rt.memory().zero(info + 16u, 16u);
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0x644E5607u,
        [get_atrac](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            const std::uint32_t sample = std::min(ctx.gpr[5], state->header.total_samples);
            const std::uint32_t bytes_first = ctx.gpr[6];
            const std::uint32_t frame = sample / atrac_samples_per_frame(*state);
            const std::uint64_t pos64 = static_cast<std::uint64_t>(state->header.data_offset) +
                static_cast<std::uint64_t>(frame) * state->header.block_align;
            state->sample_position = sample;
            state->next_file_offset = static_cast<std::uint32_t>(std::min<std::uint64_t>(pos64 + bytes_first, state->header.file_size));
            state->buffered_encoded_bytes = std::min(bytes_first, state->buffer_size);
            state->write_offset = state->buffer_size == 0u ? 0u : bytes_first % state->buffer_size;
            close_atrac_decoder(*state);
            set_success(ctx);
        });

    // Whether the stream has run out: past its last sample with no loop left to
    // take. sceAtracDecodeData restarts a looping stream when it gets there.
    const auto stream_ended = [](const AtracContextState &state) {
        return state.sample_position >= state.header.total_samples &&
            !(state.loop_num != 0 && state.header.loop_start >= 0);
    };

    runtime.register_hle("sceAtrac3plus", 0x36FAABFBu,  // sceAtracGetNextSample
        [get_atrac, stream_ended](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
            std::uint32_t samples = 0u;
            if (!stream_ended(*state)) {
                samples = state->sample_position >= state->header.total_samples
                    ? atrac_samples_per_frame(*state)
                    : static_cast<std::uint32_t>(std::min<std::uint64_t>(
                          atrac_samples_per_frame(*state),
                          state->header.total_samples - state->sample_position));
            }
            rt.memory().store32(ctx.gpr[5], samples);
            set_success(ctx);
        });

    runtime.register_hle("sceAtrac3plus", 0xE23E3A35u,  // sceAtracGetNextDecodePosition
        [get_atrac, stream_ended](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            constexpr std::uint32_t kAtracErrorAllDataDecoded = 0x80630024u;
            auto *state = get_atrac(ctx.gpr[4]);
            if (!state) { ctx.set_gpr(2, kAtracErrorBadId); return; }
            if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, kAtracErrorBadAddress); return; }
            if (stream_ended(*state)) { ctx.set_gpr(2, kAtracErrorAllDataDecoded); return; }
            const std::uint64_t position = state->sample_position >= state->header.total_samples
                ? static_cast<std::uint64_t>(state->header.loop_start) : state->sample_position;
            rt.memory().store32(ctx.gpr[5], static_cast<std::uint32_t>(position));
            set_success(ctx);
        });
}

} // namespace psprecomp::hle
