#include "vcs_profile.hpp"
#include "vcs_native_fast_paths.hpp"
#include "audio_output.hpp"
#include "display_window.hpp"
#include "vcs_camera_input.hpp"
#include "vcs_vehicle_input.hpp"
#include "vcs_media_decoder.hpp"
#include "vcs_config.hpp"
#include "framebuffer_capture.hpp"
#include "ge_renderer.hpp"
#include "ge_gpu_backend.hpp"
#include "vcs_project2dfx.hpp"

#include "psprecomp/common.hpp"
#include "psprecomp/hle/atrac.hpp"
#include "psprecomp/hle/audio.hpp"
#include "psprecomp/hle/ctrl.hpp"
#include "psprecomp/hle/display.hpp"
#include "psprecomp/hle/ge.hpp"
#include "psprecomp/hle/io.hpp"
#include "psprecomp/hle/kernel.hpp"
#include "psprecomp/hle/system.hpp"
#include "psprecomp/hle/utility.hpp"
#include "psprecomp/deflate.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <ios>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <deque>
#include <atomic>
#include <sstream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace psprecomp {
using RuntimePostImportHook = void (*)(Runtime &, AllegrexContext &);
void set_runtime_post_import_hook(RuntimePostImportHook hook) noexcept;
}

namespace vcs {
using namespace psprecomp::hle;
namespace {

struct DeflateFastPending {
    std::uint32_t return_pc{};
    std::uint64_t remaining_work{};
    std::uint64_t call{};
};
std::unordered_map<std::int32_t, DeflateFastPending> deflate_fast_pending;

std::uint64_t estimate_vcs_deflate_guest_work(std::uint32_t consumed, std::uint32_t produced) {
    // Measured against the translated VCS inflater with the same scheduler and
    // cross-unit chaining configuration.  Exact known streams keep deterministic
    // frame timing; the fixed-point model covers later streams conservatively.
    if (consumed == 2'267'436u && produced == 6'300'880u) return 21'431u;
    if (consumed == 38'278u && produced == 132'636u) return 330u;
    if (consumed == 2'236'400u && produced == 3'849'816u) return 19'418u;
    if (consumed == 1'741'959u && produced == 3'710'400u) return 15'635u;
    const std::uint64_t scaled = static_cast<std::uint64_t>(consumed) * 7'455u +
                                 static_cast<std::uint64_t>(produced) * 727u;
    const std::uint64_t estimated = (scaled + 500'000u) / 1'000'000u;
    return std::max<std::uint64_t>(1u, estimated > 51u ? estimated - 51u : 1u);
}

void vcs_raw_deflate_fast(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    constexpr std::uint32_t kFastEntry = 0x08B648B0u;
    const std::int32_t thread_uid = psprecomp::runtime_thread_uid();
    const auto pending = deflate_fast_pending.find(thread_uid);
    if (pending != deflate_fast_pending.end()) {
        DeflateFastPending &work = pending->second;
        if (work.remaining_work > 1u) {
            --work.remaining_work;
            ctx.pc = kFastEntry;
        } else {
            const std::uint32_t return_pc = work.return_pc;
            const std::uint64_t call = work.call;
            deflate_fast_pending.erase(pending);
            if (std::getenv("PSPRECOMP_DEFLATE_DIAG") != nullptr)
                std::cerr << "[deflate-fast-timing-complete] call=" << call
                          << " uid=" << thread_uid << " return=" << psprecomp::hex32(return_pc) << "\n";
            ctx.pc = return_pc;
        }
        return;
    }

    const std::uint32_t output = ctx.gpr[4];
    const std::uint32_t capacity = ctx.gpr[5];
    const std::uint32_t input = ctx.gpr[6];
    const std::uint32_t consumed_output = ctx.gpr[7];
    const std::uint32_t return_pc = ctx.gpr[31];
    static std::uint64_t deflate_calls = 0u;
    const std::uint64_t deflate_call = ++deflate_calls;
    if (std::getenv("PSPRECOMP_DEFLATE_DIAG") != nullptr) {
        std::cerr << "[deflate-fast-begin] call=" << deflate_call
                  << " input=" << psprecomp::hex32(input)
                  << " output=" << psprecomp::hex32(output)
                  << " capacity=" << capacity << "\n";
    }
    const psprecomp::RawDeflateResult result =
        psprecomp::inflate_raw_deflate(runtime.memory(), output, capacity, input);
    switch (result.status) {
    case psprecomp::RawDeflateStatus::Ok:
        if (consumed_output != 0u) {
            if (!runtime.memory().contains(consumed_output, 4u)) {
                ctx.set_gpr(2, 0x80000108u);
                break;
            }
            runtime.memory().store32(consumed_output, input + result.input_consumed);
        }
        ctx.set_gpr(2, result.output_size);
        break;
    case psprecomp::RawDeflateStatus::OutputOverflow:
        ctx.set_gpr(2, 0x80000104u);
        break;
    case psprecomp::RawDeflateStatus::InvalidData:
        ctx.set_gpr(2, 0x80000108u);
        break;
    }

    std::uint64_t guest_work = 1u;
    if (result.status == psprecomp::RawDeflateStatus::Ok)
        guest_work = estimate_vcs_deflate_guest_work(result.input_consumed, result.output_size);
    if (std::getenv("PSPRECOMP_DEFLATE_DIAG") != nullptr) {
        if (deflate_call <= 16u || (deflate_call % 1000u) == 0u) {
            std::cerr << "[deflate-fast] call=" << deflate_call
                      << " input=" << psprecomp::hex32(input)
                      << " output=" << psprecomp::hex32(output)
                      << " capacity=" << capacity
                      << " produced=" << result.output_size
                      << " consumed=" << result.input_consumed
                      << " status=" << static_cast<unsigned>(result.status)
                      << " guest_work=" << guest_work << "\n";
        }
    }

    // The current host invocation already accounts for one dispatch.  Returning
    // to the same fast entry for the remaining work preserves scheduler ticks,
    // thread preemption and vblank timing while avoiding the expensive Huffman
    // and byte-copy loops.  Each thread owns its own continuation state.
    if (result.status == psprecomp::RawDeflateStatus::Ok && guest_work > 1u &&
        std::getenv("PSPRECOMP_NO_FAST_DEFLATE_TIMING") == nullptr) {
        deflate_fast_pending.emplace(thread_uid, DeflateFastPending{return_pc, guest_work - 1u, deflate_call});
        ctx.pc = kFastEntry;
    } else {
        ctx.pc = return_pc;
    }
}

struct ParsedPsmfHeader {
    std::uint32_t raw_version{};
    std::uint32_t stream_offset{};
    std::uint32_t stream_size{};
    std::uint64_t first_timestamp{};
    std::uint64_t last_timestamp{};
    std::uint32_t width{};
    std::uint32_t height{};
};

struct MpegStreamState {
    std::uint32_t type{};
    std::uint32_t number{};
    bool needs_reset{true};
};

struct MpegContextState {
    std::uint32_t handle_address{};
    std::uint32_t ring_address{};
    ParsedPsmfHeader header{};
    std::unordered_map<std::uint32_t, MpegStreamState> streams;
    std::array<bool, 2> avc_es_buffers{};
    std::uint32_t video_pixel_mode{3u};
    std::uint32_t video_au_count{};
    std::uint32_t audio_au_count{};
    std::uint32_t decoded_video_frames{};
    std::uint32_t consumed_video_packets{};
    std::filesystem::path source_path;
    VideoStreamDecoder video;
    PmfAudioDecoder audio;
    std::filesystem::path audio_source;
    bool video_eof{};
    bool analyzed{};
};

std::uint32_t read_be32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return (static_cast<std::uint32_t>(bytes[offset]) << 24u) |
        (static_cast<std::uint32_t>(bytes[offset + 1u]) << 16u) |
        (static_cast<std::uint32_t>(bytes[offset + 2u]) << 8u) |
        static_cast<std::uint32_t>(bytes[offset + 3u]);
}

std::uint64_t read_psmf_timestamp(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint64_t>(bytes[offset + 5u]) |
        (static_cast<std::uint64_t>(bytes[offset + 4u]) << 8u) |
        (static_cast<std::uint64_t>(bytes[offset + 3u]) << 16u) |
        (static_cast<std::uint64_t>(bytes[offset + 2u]) << 24u) |
        (static_cast<std::uint64_t>(bytes[offset + 1u]) << 32u) |
        (static_cast<std::uint64_t>(bytes[offset]) << 36u);
}

void write_mpeg_timestamp(psprecomp::GuestMemory &memory, std::uint32_t address, std::uint64_t value) {
    // SceMpegAu stores 64-bit timestamps with the two 32-bit words reversed.
    memory.store32(address, static_cast<std::uint32_t>(value >> 32u));
    memory.store32(address + 4u, static_cast<std::uint32_t>(value));
}

bool parse_psmf_header(std::span<const std::uint8_t> bytes, ParsedPsmfHeader &header) {
    if (bytes.size() < 2048u || bytes[0] != 'P' || bytes[1] != 'S' ||
        bytes[2] != 'M' || bytes[3] != 'F') return false;
    header.raw_version = static_cast<std::uint32_t>(bytes[4]) |
        (static_cast<std::uint32_t>(bytes[5]) << 8u) |
        (static_cast<std::uint32_t>(bytes[6]) << 16u) |
        (static_cast<std::uint32_t>(bytes[7]) << 24u);
    const bool known_version = header.raw_version == 0x32313030u ||
        header.raw_version == 0x33313030u || header.raw_version == 0x34313030u ||
        header.raw_version == 0x35313030u;
    if (!known_version) return false;
    header.stream_offset = read_be32(bytes, 8u);
    header.stream_size = read_be32(bytes, 12u);
    header.first_timestamp = read_psmf_timestamp(bytes, 0x54u);
    header.last_timestamp = read_psmf_timestamp(bytes, 0x5Au);
    header.width = static_cast<std::uint32_t>(bytes[142u]) * 16u;
    header.height = static_cast<std::uint32_t>(bytes[143u]) * 16u;
    return true;
}



struct DeferredIoResume {
    // The UMD worker may switch to the request submitter from inside sceIoRead.
    // Host I/O is effectively instantaneous, so resuming at the request-pointer
    // store is still too early: VCS clears WorldStreamEventFlag immediately
    // afterwards and can erase the worker's completion bit.  Keep both the PC
    // where execution was handed off and the later atomic-commit boundary where
    // the submitter has cleared the stale event state and published bit 0x4.
    std::int32_t handoff_uid{};
    std::uint32_t handoff_pc{};
    std::uint32_t release_pc{};
    std::uint64_t observed_dispatches{};
};

std::uint32_t io_handoff_release_pc(std::uint32_t handoff_pc) {
    switch (handoff_pc) {
    case 0x08955E7Cu: return 0x08955EA4u; // single world-stream submission
    case 0x08956258u: return 0x08956280u; // batched world-stream submission
    default: return handoff_pc;
    }
}

// A very small host-backed read can finish before the allocating guest call has
// unwound to 0x08955E7C/0x08956258.  In that case the currently restored
// submitter may still be inside the UMD allocator semaphore unlock (for example
// 0x08939C4C), so deriving the release point from ctx.pc is too early.  The
// request callback identifies the two VCS world-stream submission paths.  When
// manager+0x274 does not yet point at this request, hold the worker until the
// corresponding clear-event/set-0x4 transaction has committed.
std::uint32_t uncommitted_world_stream_release_pc(psprecomp::Runtime &runtime,
                                                   std::uint32_t request) {
    constexpr std::uint32_t world_stream_manager = 0x08E91200u;
    constexpr std::uint32_t active_offset = 628u;
    if (request == 0u || !runtime.memory().contains(request, 52u) ||
        !runtime.memory().contains(world_stream_manager + active_offset, 4u))
        return 0u;
    if (runtime.memory().load32(world_stream_manager + active_offset) == request)
        return 0u; // already committed; ordinary per-read handoff is sufficient
    switch (runtime.memory().load32(request + 48u)) {
    case 0x08953990u: return 0x08955EA4u;
    case 0x089539CCu: return 0x08956280u;
    default: return 0u;
    }
}

std::unordered_map<std::int32_t, DeferredIoResume> deferred_io_resumes;

// The post-dispatch callback is needed only while a rare diagnostic/frozen-
// clock guard is active or while a deferred UMD handoff is actually armed.
// Keeping the function pointer installed permanently taxes every outer AOT
// dispatch even though deferred_io_resumes is empty for the normal case.
void vcs_post_dispatch_hook(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                            std::uint32_t dispatch_pc, std::int32_t dispatch_thread_uid);
void refresh_vcs_post_dispatch_hook();

// Stage 9 targeted event diagnostics. The general event trace is too noisy
// during a frontend run, so allow filtering by flag name and a bounded poll
// count that captures the scheduler state exactly when progress stops.




std::string shell_quote(const std::string &value) {
#if defined(_WIN32)
    std::string quoted = "\"";
    for (const char ch : value) quoted += ch == '\"' ? "\\\"" : std::string(1, ch);
    quoted += "\"";
    return quoted;
#else
    std::string quoted = "'";
    for (const char ch : value) quoted += ch == '\'' ? "'\"'\"'" : std::string(1, ch);
    quoted += "'";
    return quoted;
#endif
}

// The MPEG and ATRAC HLE paths decode through a host `ffmpeg` process.  On
// Windows `_popen` succeeds even when the executable does not exist: the shell
// starts, prints "not recognized" and exits, so every read returns EOF.  The
// guest then waits forever for a frame that can never arrive, which looks
// exactly like a hang with no diagnostic.  Probe once and fail loudly instead.
// The host ffmpeg process is gone: decoding is in-process through the vendored
// minimal FFmpeg libraries. See host/vcs_media_decoder.cpp.

// Buffers the movie decoder writes decoded pictures into.
//
// The game does not draw a movie as geometry: it hands sceMpegAvcDecode a plain
// RAM buffer and then points the display at that buffer, so a movie frame
// reaches the screen without a single GE draw. That makes these addresses the
// reliable answer to "is a movie on screen right now?" -- compared against the
// displayed framebuffer once per vblank, in the display path below.
//
// Two, in practice, alternating; the set is tiny and cleared when a movie ends.
std::unordered_set<std::uint32_t> movie_output_buffers;

// PSP RAM is visible both cached and uncached, and the display and the decoder
// do not have to agree on which mirror they name.
[[nodiscard]] std::uint32_t normalize_ram_address(std::uint32_t address) noexcept {
    return address & 0x1FFFFFFFu;
}

void close_video_decoder(MpegContextState &state) {
    state.video.close();
    movie_output_buffers.clear();
    // The soundtrack belongs to the same movie. Leaving it open meant the
    // second cutscene kept reading the first one's exhausted stream and played
    // silent.
    state.audio.close();
    state.audio_source.clear();
    state.video_eof = false;
    state.decoded_video_frames = 0u;
    state.consumed_video_packets = 0u;
}

bool open_video_decoder(MpegContextState &state) {
    if (state.video.is_open()) return true;
    if (state.source_path.empty() || state.header.width == 0u || state.header.height == 0u) return false;
    if (!state.video.open(state.source_path)) return false;
    state.video_eof = false;
    state.decoded_video_frames = 0u;
    state.consumed_video_packets = 0u;
    if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr)
        std::cerr << "[mpeg] decoder opened source=\"" << state.source_path.string() << "\"\n";
    return true;
}

bool read_video_frame(MpegContextState &state, std::span<std::uint8_t> frame) {
    if (!open_video_decoder(state)) return false;
    if (state.video.read(frame) < frame.size()) {
        state.video_eof = true;
        return false;
    }
    ++state.decoded_video_frames;
    return true;
}

std::filesystem::path identify_pmf_source(std::span<const std::uint8_t> header, const ParsedPsmfHeader &parsed) {
    const std::uint64_t expected_size = static_cast<std::uint64_t>(parsed.stream_offset) + parsed.stream_size;
    for (const auto &[key, file] : file_table.virtual_files_by_path) {
        if (file.size != expected_size) continue;
        std::string extension = file.native_path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
        if (extension != ".PMF") continue;
        std::array<std::uint8_t, 2048> candidate{};
        std::ifstream input(file.native_path, std::ios::binary);
        if (!input) continue;
        input.read(reinterpret_cast<char *>(candidate.data()), static_cast<std::streamsize>(candidate.size()));
        if (input.gcount() == static_cast<std::streamsize>(candidate.size()) &&
            std::equal(candidate.begin(), candidate.end(), header.begin())) return file.native_path;
    }
    return {};
}


std::unordered_map<std::uint32_t, MpegContextState> mpeg_contexts;
std::uint32_t next_mpeg_stream_id{1u};

constexpr std::uint32_t kGuestFrameLimiterBranch = 0x08A070C8u;
constexpr std::uint32_t kGuestFrameLimiterContinue = 0x08A070D0u;
constexpr std::int32_t kGuestFrameCounterGpOffset = -8852;

std::uint32_t configured_game_frame_rate() noexcept {
    return vcs_configuration().timing.frame_rate;
}

std::uint32_t virtual_display_refresh_hz() noexcept {
    // 30 FPS is the stock game running on the PSP's 60 Hz display. Every
    // unlocked mode renders once per virtual vblank.
    return std::max(60u, configured_game_frame_rate());
}

std::uint64_t virtual_vblank_period_us() noexcept {
    // Preserve the port's existing 59.94 Hz PSP period exactly at 30/60.
    const std::uint64_t refresh = virtual_display_refresh_hz();
    return std::max<std::uint64_t>(1u, (16683u * 60u + refresh / 2u) / refresh);
}

void unlocked_frame_limiter_patch(psprecomp::Runtime &runtime,
                                  psprecomp::AllegrexContext &ctx) {
    // CWCheat 0x202070C8 writes a NOP over the branch at guest 0x08A070C8.
    // Its delay-slot load still executes, then control falls through to D0.
    ctx.set_gpr(4, runtime.memory().load32(
        ctx.gpr[28] + static_cast<std::uint32_t>(kGuestFrameCounterGpOffset)));
    ctx.pc = kGuestFrameLimiterContinue;
}





// Stage 9 diagnostic guard.  Setting PSPRECOMP_TIME_TICK_DISPATCHES=0 is useful
// for isolated scheduler/I/O ordering tests, but it deliberately disables the
// execution-driven PSP timer.  A polling thread can then keep delayed workers
// from ever reaching their deadlines.  Detect that configuration before it
// burns hundreds of millions of dispatches while appearing to be a game hang.
std::uint64_t execution_clock_dispatch_interval{256u};
std::uint64_t frozen_clock_guard_limit{5'000'000u};
std::uint64_t frozen_clock_guard_dispatches{};
std::uint64_t frozen_clock_guard_vblank{};


std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}


void dump_ram_if_requested(const psprecomp::GuestMemory &memory) {
    struct Config {
        std::filesystem::path directory;
        std::uint64_t start{};
        std::uint64_t end{};
        std::uint64_t interval{1u};
        bool dump_vram{};
        bool enabled{};
    };
    static const Config config = [] {
        Config value{};
        const char *directory = std::getenv("PSPRECOMP_RAM_DUMP_DIR");
        if (directory == nullptr || *directory == '\0') return value;
        value.directory = directory;
        value.start = parse_environment_u64("PSPRECOMP_RAM_DUMP_START_VBLANK");
        value.end = parse_environment_u64("PSPRECOMP_RAM_DUMP_END_VBLANK", value.start);
        value.interval = std::max<std::uint64_t>(1u, parse_environment_u64("PSPRECOMP_RAM_DUMP_INTERVAL", 1u));
        value.dump_vram = parse_environment_u64("PSPRECOMP_RAM_DUMP_VRAM") != 0u;
        value.enabled = true;
        return value;
    }();
    if (!config.enabled || display_vblank_index < config.start || display_vblank_index > config.end ||
        ((display_vblank_index - config.start) % config.interval) != 0u) return;

    std::filesystem::create_directories(config.directory);
    std::ostringstream stem;
    stem << "ram_vblank_" << std::setw(6) << std::setfill('0') << display_vblank_index;
    const auto write_bytes = [&](const std::filesystem::path &path, const std::vector<std::uint8_t> &bytes) {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        if (!output) throw std::runtime_error("Unable to create RAM diagnostic dump: " + path.string());
        output.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!output) throw std::runtime_error("Unable to write RAM diagnostic dump: " + path.string());
    };
    const std::filesystem::path ram_path = config.directory / (stem.str() + ".bin");
    write_bytes(ram_path, memory.bytes());
    if (config.dump_vram)
        write_bytes(config.directory / (stem.str() + ".vram.bin"), memory.vram_bytes());
    std::cerr << "[ram-dump] vblank=" << display_vblank_index
              << " path=" << ram_path.string()
              << " bytes=" << memory.bytes().size() << "\n";
}







class O32VarArgs {
public:
    O32VarArgs(psprecomp::Runtime &runtime, const psprecomp::AllegrexContext &ctx)
        : runtime_(runtime), ctx_(ctx) {}

    std::uint32_t next_u32() {
        // The VCS PSP build's variadic call convention keeps consuming the
        // argument register window through a2, a3, t0, t1, t2 and t3 before
        // spilling additional words to the caller argument area.
        if (index_ < 6u) {
            return ctx_.gpr[6u + index_++];
        }
        // Spilled words start at sp+0, not at the sp+16 an o32 caller would
        // use. This build fills a2, a3 and t0..t3 first, so by the time it
        // reaches the stack the four-word argument save area is not what it is
        // writing into -- it simply continues at the bottom of the frame.
        //
        // Measured at the save-description call (0x08AB73E0), whose format
        // "%s\n%s %s\n%s %d, $%d.\n%.1f%% %s" is the first in the game to need
        // more than six variadic words. The caller emits exactly three stores:
        //
        //   sw v0, 0(sp)    low half of the %.1f double
        //   sw v1, 4(sp)    high half
        //   sw s7, 8(sp)    the pointer for the trailing %s
        //
        // Reading those from sp+16 gave the last %s whatever happened to be
        // above the frame, and formatting it as a string dereferenced it: that
        // is the "guest memory access outside PSP RAM at 0x00000160" that
        // killed the game the moment a save was written. Nothing else hit it
        // because no other format in the EBOOT spills.
        const std::uint32_t address = ctx_.gpr[29] + static_cast<std::uint32_t>((index_ - 6u) * 4u);
        ++index_;
        return runtime_.memory().load32(address);
    }

    std::uint64_t next_u64_aligned() {
        // O32 aligns 64-bit variadic values to an even word slot.  Our slot 0
        // corresponds to physical argument register a2, which is already even.
        if ((index_ & 1u) != 0u) ++index_;
        const std::uint64_t low = next_u32();
        const std::uint64_t high = next_u32();
        return low | (high << 32u);
    }

    double next_double() {
        return std::bit_cast<double>(next_u64_aligned());
    }

private:
    psprecomp::Runtime &runtime_;
    const psprecomp::AllegrexContext &ctx_;
    std::size_t index_{};
};

std::string format_integer(std::uint64_t value, bool negative, unsigned base, bool upper,
                           int width, int precision, bool left, bool zero, bool plus,
                           bool blank, bool alternate) {
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string number;
    do {
        number.push_back(digits[value % base]);
        value /= base;
    } while (value != 0u);
    std::reverse(number.begin(), number.end());
    if (precision == 0 && number == "0") number.clear();
    while (static_cast<int>(number.size()) < precision) number.insert(number.begin(), '0');

    std::string prefix;
    if (negative) prefix = "-";
    else if (plus) prefix = "+";
    else if (blank) prefix = " ";
    if (alternate && base == 16u && !number.empty() && number != "0") prefix += upper ? "0X" : "0x";
    if (alternate && base == 8u && (number.empty() || number.front() != '0')) prefix += "0";

    std::string result = prefix + number;
    if (width > static_cast<int>(result.size())) {
        const std::size_t padding = static_cast<std::size_t>(width - static_cast<int>(result.size()));
        if (left) result.append(padding, ' ');
        else if (zero && precision < 0) result = prefix + std::string(padding, '0') + number;
        else result.insert(0, padding, ' ');
    }
    return result;
}

std::uint32_t rot_mix_lookup2(std::span<const std::uint8_t> key, std::uint32_t init_value) {
    auto read_le32 = [](const std::uint8_t *p) -> std::uint32_t {
        return static_cast<std::uint32_t>(p[0]) |
               (static_cast<std::uint32_t>(p[1]) << 8u) |
               (static_cast<std::uint32_t>(p[2]) << 16u) |
               (static_cast<std::uint32_t>(p[3]) << 24u);
    };
    auto mix = [](std::uint32_t &a, std::uint32_t &b, std::uint32_t &c) {
        a -= b; a -= c; a ^= c >> 13u;
        b -= c; b -= a; b ^= a << 8u;
        c -= a; c -= b; c ^= b >> 13u;
        a -= b; a -= c; a ^= c >> 12u;
        b -= c; b -= a; b ^= a << 16u;
        c -= a; c -= b; c ^= b >> 5u;
        a -= b; a -= c; a ^= c >> 3u;
        b -= c; b -= a; b ^= a << 10u;
        c -= a; c -= b; c ^= b >> 15u;
    };

    std::uint32_t a = 0x9E3779B9u;
    std::uint32_t b = 0x9E3779B9u;
    std::uint32_t c = init_value;
    const std::uint32_t original_length = static_cast<std::uint32_t>(key.size());
    std::size_t offset = 0u;
    while (key.size() - offset >= 12u) {
        a += read_le32(key.data() + offset);
        b += read_le32(key.data() + offset + 4u);
        c += read_le32(key.data() + offset + 8u);
        mix(a, b, c);
        offset += 12u;
    }

    c += original_length;
    const std::uint8_t *tail = key.data() + offset;
    switch (key.size() - offset) {
    case 11: c += static_cast<std::uint32_t>(tail[10]) << 24u; [[fallthrough]];
    case 10: c += static_cast<std::uint32_t>(tail[9]) << 16u; [[fallthrough]];
    case 9:  c += static_cast<std::uint32_t>(tail[8]) << 8u; [[fallthrough]];
    case 8:  b += static_cast<std::uint32_t>(tail[7]) << 24u; [[fallthrough]];
    case 7:  b += static_cast<std::uint32_t>(tail[6]) << 16u; [[fallthrough]];
    case 6:  b += static_cast<std::uint32_t>(tail[5]) << 8u; [[fallthrough]];
    case 5:  b += tail[4]; [[fallthrough]];
    case 4:  a += static_cast<std::uint32_t>(tail[3]) << 24u; [[fallthrough]];
    case 3:  a += static_cast<std::uint32_t>(tail[2]) << 16u; [[fallthrough]];
    case 2:  a += static_cast<std::uint32_t>(tail[1]) << 8u; [[fallthrough]];
    case 1:  a += tail[0];
    default: break;
    }
    mix(a, b, c);
    return c;
}

void vcs_load_codec_modules(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::uint32_t table = ctx.gpr[28] + 0x0C90u;
    const std::string prefix = runtime.memory().read_c_string(0x08B88280u, 1024u);
    std::size_t loaded = 0u;
    for (std::size_t index = 0u; index < 64u; ++index) {
        const std::uint32_t entry = table + static_cast<std::uint32_t>(index * 8u);
        const std::uint32_t name_pointer = runtime.memory().load32(entry);
        if (name_pointer == 0u) break;
        const std::int32_t existing = static_cast<std::int32_t>(runtime.memory().load32(entry + 4u));
        if (existing != -1) continue;

        std::string module_name = runtime.memory().read_c_string(name_pointer, 1024u);
        if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
            std::cerr << "[hle] codec entry " << index << " name=" << module_name
                      << " existing=" << existing << "\n";
        }
        std::string guest_path = prefix + module_name;
        std::transform(guest_path.begin(), guest_path.end(), guest_path.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        const auto native = runtime.translate_path(guest_path);
        if (!std::filesystem::is_regular_file(native)) {
            runtime.stop("Required PSP module is missing: " + guest_path + " -> " + native.string());
            return;
        }
        runtime.memory().store32(entry + 4u, static_cast<std::uint32_t>(next_module_uid++));
        ++loaded;
    }
    if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
        std::cerr << "[hle] codec modules ready: " << loaded << " prefix=" << prefix << "\n";
    }
    ctx.set_gpr(2, 0u);
    ctx.pc = ctx.gpr[31];
}

void vcs_path_hash(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::string path = runtime.memory().read_c_string(ctx.gpr[4], 65536u);
    const auto bytes = std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t *>(path.data()), path.size());
    ctx.set_gpr(2, rot_mix_lookup2(bytes, 0x04C11DB7u));
    ctx.pc = ctx.gpr[31];
}

void vcs_sprintf(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::uint32_t destination = ctx.gpr[4];
    const std::string format = runtime.memory().read_c_string(ctx.gpr[5], 4096u);
    O32VarArgs args(runtime, ctx);
    std::string output;
    output.reserve(format.size() + 64u);

    for (std::size_t i = 0; i < format.size(); ++i) {
        if (format[i] != '%') {
            output.push_back(format[i]);
            continue;
        }
        if (++i >= format.size()) break;
        if (format[i] == '%') {
            output.push_back('%');
            continue;
        }

        bool left = false, plus = false, blank = false, alternate = false, zero = false;
        for (;;) {
            if (format[i] == '-') left = true;
            else if (format[i] == '+') plus = true;
            else if (format[i] == ' ') blank = true;
            else if (format[i] == '#') alternate = true;
            else if (format[i] == '0') zero = true;
            else break;
            if (++i >= format.size()) break;
        }

        int width = 0;
        if (i < format.size() && format[i] == '*') {
            width = static_cast<std::int32_t>(args.next_u32());
            if (width < 0) { left = true; width = -width; }
            ++i;
        } else {
            while (i < format.size() && format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (format[i] - '0');
                ++i;
            }
        }

        int precision = -1;
        if (i < format.size() && format[i] == '.') {
            ++i;
            precision = 0;
            if (i < format.size() && format[i] == '*') {
                precision = static_cast<std::int32_t>(args.next_u32());
                ++i;
            } else {
                while (i < format.size() && format[i] >= '0' && format[i] <= '9') {
                    precision = precision * 10 + (format[i] - '0');
                    ++i;
                }
            }
            if (precision < 0) precision = -1;
        }

        int length = 0;
        if (i < format.size() && format[i] == 'l') {
            length = 1;
            if (++i < format.size() && format[i] == 'l') { length = 2; ++i; }
        } else if (i < format.size() && format[i] == 'h') {
            length = -1;
            if (++i < format.size() && format[i] == 'h') { length = -2; ++i; }
        }
        if (i >= format.size()) break;

        const char conversion = format[i];
        if (conversion == 's') {
            const std::uint32_t pointer = args.next_u32();
            std::string value = pointer == 0u ? "(null)" : runtime.memory().read_c_string(pointer, 65536u);
            if (precision >= 0 && static_cast<int>(value.size()) > precision) value.resize(static_cast<std::size_t>(precision));
            if (width > static_cast<int>(value.size())) {
                const std::size_t padding = static_cast<std::size_t>(width - static_cast<int>(value.size()));
                if (left) value.append(padding, ' '); else value.insert(0, padding, ' ');
            }
            output += value;
        } else if (conversion == 'c') {
            std::string value(1u, static_cast<char>(args.next_u32() & 0xFFu));
            if (width > 1) {
                if (left) value.append(static_cast<std::size_t>(width - 1), ' ');
                else value.insert(0, static_cast<std::size_t>(width - 1), ' ');
            }
            output += value;
        } else if (conversion == 'd' || conversion == 'i') {
            std::int64_t signed_value = static_cast<std::int32_t>(args.next_u32());
            if (length == 2) {
                const std::uint64_t lo = static_cast<std::uint64_t>(static_cast<std::uint32_t>(signed_value));
                const std::uint64_t hi = args.next_u32();
                signed_value = static_cast<std::int64_t>((hi << 32u) | lo);
            }
            const bool negative = signed_value < 0;
            const std::uint64_t magnitude = negative ? static_cast<std::uint64_t>(-(signed_value + 1)) + 1u : static_cast<std::uint64_t>(signed_value);
            output += format_integer(magnitude, negative, 10u, false, width, precision, left, zero, plus, blank, false);
        } else if (conversion == 'u' || conversion == 'o' || conversion == 'x' || conversion == 'X' || conversion == 'p') {
            std::uint64_t value = args.next_u32();
            if (length == 2) value |= static_cast<std::uint64_t>(args.next_u32()) << 32u;
            const unsigned base = conversion == 'o' ? 8u : ((conversion == 'x' || conversion == 'X' || conversion == 'p') ? 16u : 10u);
            const bool pointer = conversion == 'p';
            output += format_integer(value, false, base, conversion == 'X', width, precision, left, zero, false, false, alternate || pointer);
        } else if (conversion == 'f' || conversion == 'F' || conversion == 'e' || conversion == 'E' ||
                   conversion == 'g' || conversion == 'G') {
            const double value = args.next_double();
            std::string specification{"%"};
            if (left) specification.push_back('-');
            if (plus) specification.push_back('+');
            if (blank) specification.push_back(' ');
            if (alternate) specification.push_back('#');
            if (zero) specification.push_back('0');
            if (width > 0) specification += std::to_string(width);
            if (precision >= 0) specification += "." + std::to_string(precision);
            specification.push_back(conversion);

            const int required = std::snprintf(nullptr, 0, specification.c_str(), value);
            if (required < 0) {
                runtime.stop("VCS sprintf floating conversion failed for " + specification);
                return;
            }
            std::vector<char> formatted(static_cast<std::size_t>(required) + 1u);
            const int written = std::snprintf(formatted.data(), formatted.size(), specification.c_str(), value);
            if (written != required) {
                runtime.stop("VCS sprintf floating conversion length mismatch for " + specification);
                return;
            }
            output.append(formatted.data(), static_cast<std::size_t>(written));
        } else if (conversion == 'n') {
            const std::uint32_t pointer = args.next_u32();
            runtime.memory().store32(pointer, static_cast<std::uint32_t>(output.size()));
        } else {
            runtime.stop(std::string("Unsupported VCS sprintf conversion %") + conversion);
            return;
        }
    }

    if (!runtime.memory().contains(destination, output.size() + 1u)) {
        runtime.stop("VCS sprintf destination outside guest memory");
        return;
    }
    std::vector<std::uint8_t> bytes(output.begin(), output.end());
    bytes.push_back(0u);
    runtime.memory().copy_in(destination, bytes);
    ctx.set_gpr(2, static_cast<std::uint32_t>(output.size()));
    ctx.pc = ctx.gpr[31];
}


bool defer_current_thread_for_io_handoff(psprecomp::Runtime &runtime,
                                          psprecomp::AllegrexContext &ctx,
                                          std::uint32_t return_value,
                                          std::uint32_t release_pc_hint = 0u) {
    const std::int32_t worker_uid = thread_table.current_uid;
    auto worker = thread_table.threads.find(worker_uid);
    if (worker == thread_table.threads.end()) {
        ctx.set_gpr(2, return_value);
        return false;
    }

    // With nobody else ready there is no submitter/worker race to break.
    if (best_ready_thread() == thread_table.continuations.end()) {
        ctx.set_gpr(2, return_value);
        return false;
    }

    psprecomp::AllegrexContext suspended = make_wait_context(ctx);
    suspended.set_gpr(2, return_value);
    worker->second.state = ThreadState::IoDeferred;
    worker->second.suspended_context = suspended;

    if (!activate_next_thread(ctx, "io-handoff")) {
        worker->second.state = ThreadState::Running;
        ctx = suspended;
        psprecomp::set_runtime_thread_identity(worker_uid, worker->second.name);
        return false;
    }

    const std::uint32_t handoff_pc = ctx.pc;
    const std::uint32_t release_pc = release_pc_hint != 0u
        ? release_pc_hint
        : io_handoff_release_pc(handoff_pc);
    deferred_io_resumes[worker_uid] =
        DeferredIoResume{thread_table.current_uid, handoff_pc, release_pc, 0u};
    refresh_vcs_post_dispatch_hook();
    if (std::getenv("PSPRECOMP_UMD_STREAM_DIAG") != nullptr ||
        std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[io-handoff] arm worker=" << worker_uid
                  << " worker_name=" << worker->second.name
                  << " worker_resume=" << psprecomp::hex32(suspended.pc)
                  << " handoff_uid=" << thread_table.current_uid
                  << " handoff_pc=" << psprecomp::hex32(handoff_pc)
                  << " release_pc=" << psprecomp::hex32(release_pc)
                  << " result=" << return_value << "\n";
    }
    return true;
}





void vcs_post_import_hook(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    if (ge_async_running()) {
        ge_async_drain_completions();
        if (!ge_async_check_fatal(runtime)) return;
    }
    (void)maybe_start_pending_guest_callback(ctx);
}

struct CollisionChainTraceFrame {
    std::uint32_t target{};
    std::uint32_t a0{};
    std::uint32_t a1{};
    std::uint32_t a2{};
    std::uint32_t a3{};
    std::uint32_t sp{};
    std::uint32_t ra{};
    bool traced{};
};
std::vector<CollisionChainTraceFrame> collision_chain_trace_stack;

struct CollisionRootProbeState {
    std::uint64_t emitted{};
};
CollisionRootProbeState collision_root_probe_state;

struct PhysicsVcallCensusState {
    std::uint64_t emitted{};
};
PhysicsVcallCensusState physics_vcall_census_state;

bool physics_vcall_census_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_PHYSICS_VCALL_CENSUS") != nullptr;
    return enabled;
}

bool physics_vcall_census_in_window() {
    if (!physics_vcall_census_enabled()) return false;
    static const std::uint64_t start =
        parse_environment_u64("PSPRECOMP_PHYSICS_VCALL_CENSUS_START_VBLANK");
    static const std::uint64_t end =
        parse_environment_u64("PSPRECOMP_PHYSICS_VCALL_CENSUS_END_VBLANK", start);
    return display_vblank_index >= start && display_vblank_index <= end;
}

bool physics_vcall_census_can_emit() {
    static const std::uint64_t limit =
        parse_environment_u64("PSPRECOMP_PHYSICS_VCALL_CENSUS_MAX", 2048u);
    return physics_vcall_census_in_window() &&
           (limit == 0u || physics_vcall_census_state.emitted < limit);
}

bool collision_root_probe_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_COLLISION_ROOT_PROBE") != nullptr;
    return enabled;
}

bool collision_root_probe_in_window() {
    if (!collision_root_probe_enabled()) return false;
    static const std::uint64_t start =
        parse_environment_u64("PSPRECOMP_COLLISION_ROOT_PROBE_START_VBLANK");
    static const std::uint64_t end =
        parse_environment_u64("PSPRECOMP_COLLISION_ROOT_PROBE_END_VBLANK", start);
    return display_vblank_index >= start && display_vblank_index <= end;
}

std::uint32_t collision_root_probe_pc_start() {
    static const std::uint32_t value = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_COLLISION_ROOT_PROBE_PC", 0x0899F9ECu));
    return value;
}

std::uint32_t collision_root_probe_pc_end() {
    static const std::uint32_t value = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_COLLISION_ROOT_PROBE_PC_END",
                              collision_root_probe_pc_start()));
    return value;
}

bool collision_root_probe_matches(std::uint32_t pc) {
    return pc >= collision_root_probe_pc_start() && pc <= collision_root_probe_pc_end();
}

bool collision_probe_a0_matches(std::uint32_t a0) {
    static const std::uint32_t wanted = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_COLLISION_PROBE_A0", 0u));
    return wanted == 0u || a0 == wanted;
}

bool collision_root_probe_can_emit() {
    static const std::uint64_t limit =
        parse_environment_u64("PSPRECOMP_COLLISION_ROOT_PROBE_MAX", 256u);
    return collision_root_probe_in_window() &&
           (limit == 0u || collision_root_probe_state.emitted < limit);
}

std::string collision_probe_object_words(psprecomp::Runtime &rt, std::uint32_t address) {
    if (!rt.memory().contains(address, 0x80u)) return "invalid";
    constexpr std::array<std::uint32_t, 16> offsets{
        0x00u, 0x04u, 0x08u, 0x0Cu,
        0x30u, 0x34u, 0x38u, 0x3Cu,
        0x48u, 0x50u, 0x54u, 0x58u,
        0x70u, 0x74u, 0x78u, 0x7Cu};
    std::ostringstream out;
    bool first = true;
    for (const std::uint32_t offset : offsets) {
        if (!first) out << ',';
        first = false;
        const std::uint32_t bits = rt.memory().load32(address + offset);
        out << std::hex << offset << ':' << psprecomp::hex32(bits);
    }
    return out.str();
}

std::string collision_probe_pointer70_words(psprecomp::Runtime &rt, std::uint32_t address) {
    if (!rt.memory().contains(address + 0x70u, 4u)) return "invalid-a0";
    const std::uint32_t pointer = rt.memory().load32(address + 0x70u);
    std::ostringstream out;
    out << "ptr=" << psprecomp::hex32(pointer) << ';';
    if (!rt.memory().contains(pointer, 0x60u)) { out << "invalid"; return out.str(); }
    constexpr std::array<std::uint32_t, 12> offsets{
        0x00u, 0x10u, 0x20u, 0x30u,
        0x40u, 0x48u, 0x4Cu, 0x50u,
        0x54u, 0x58u, 0x5Cu, 0x60u};
    bool first = true;
    for (const std::uint32_t offset : offsets) {
        if (!first) out << ',';
        first = false;
        if (!rt.memory().contains(pointer + offset, 4u)) { out << std::hex << offset << ":invalid"; continue; }
        out << std::hex << offset << ':' << psprecomp::hex32(rt.memory().load32(pointer + offset));
    }
    return out.str();
}

bool collision_chain_trace_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_COLLISION_CHAIN_TRACE") != nullptr;
    return enabled;
}

bool collision_chain_trace_in_window() {
    if (!collision_chain_trace_enabled()) return false;
    static const std::uint64_t start =
        parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_START_VBLANK");
    static const std::uint64_t end =
        parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_END_VBLANK", start);
    return display_vblank_index >= start && display_vblank_index <= end;
}

std::uint32_t collision_chain_trace_root_start() {
    static const std::uint32_t value = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_ROOT_START", 0x0899F9ECu));
    return value;
}

std::uint32_t collision_chain_trace_root_end() {
    static const std::uint32_t value = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_ROOT_END",
                              collision_chain_trace_root_start()));
    return value;
}

bool collision_point_trace_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_COLLISION_POINT_TRACE") != nullptr;
    return enabled;
}

bool dispatch_collision_diagnostics_enabled() {
    return collision_root_probe_enabled() || collision_chain_trace_enabled();
}

bool chained_call_collision_diagnostics_enabled() {
    return dispatch_collision_diagnostics_enabled() || physics_vcall_census_enabled();
}

void refresh_vcs_post_dispatch_hook() {
    const bool frozen_clock_guard_needed =
        execution_clock_dispatch_interval == 0u && frozen_clock_guard_limit != 0u;
    const bool needed = !deferred_io_resumes.empty() ||
        dispatch_collision_diagnostics_enabled() || frozen_clock_guard_needed;
    psprecomp::set_runtime_post_dispatch_hook(needed ? &vcs_post_dispatch_hook : nullptr);
}

std::string collision_trace_words(psprecomp::Runtime &rt, std::uint32_t address) {
    if (!rt.memory().contains(address, 16u)) return "invalid";
    std::ostringstream out;
    out << psprecomp::hex32(rt.memory().load32(address + 0u)) << ','
        << psprecomp::hex32(rt.memory().load32(address + 4u)) << ','
        << psprecomp::hex32(rt.memory().load32(address + 8u)) << ','
        << psprecomp::hex32(rt.memory().load32(address + 12u));
    return out.str();
}

std::string collision_trace_colpoint(psprecomp::Runtime &rt, std::uint32_t address) {
    if (!rt.memory().contains(address, 32u)) return "invalid";
    const auto as_float = [&](std::uint32_t offset) {
        return std::bit_cast<float>(rt.memory().load32(address + offset));
    };
    std::ostringstream out;
    out << "point=(" << as_float(0u) << ',' << as_float(4u) << ',' << as_float(8u) << ')'
        << " depth=" << as_float(12u)
        << " normal=(" << as_float(16u) << ',' << as_float(20u) << ',' << as_float(24u) << ')'
        << " surfaces=" << psprecomp::hex32(rt.memory().load32(address + 28u));
    return out.str();
}

void vcs_pre_chained_call_hook(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                               std::uint32_t target_pc, std::uint32_t native_depth) {
    const std::uint32_t root_start = collision_chain_trace_root_start();
    const std::uint32_t root_end = collision_chain_trace_root_end();
    const bool inherited = !collision_chain_trace_stack.empty() &&
                           collision_chain_trace_stack.back().traced;
    const std::uint32_t outer_pc = psprecomp::runtime_dispatch_pc();
    const bool root = collision_chain_trace_in_window() && collision_probe_a0_matches(ctx.gpr[4]) &&
                      ((target_pc >= root_start && target_pc <= root_end) ||
                       (outer_pc >= root_start && outer_pc <= root_end));
    const bool traced = inherited || root;
    collision_chain_trace_stack.push_back(CollisionChainTraceFrame{
        target_pc, ctx.gpr[4], ctx.gpr[5], ctx.gpr[6], ctx.gpr[7], ctx.gpr[29], ctx.gpr[31], traced});
    if (physics_vcall_census_can_emit() && native_depth == 0u) {
        static const std::uint32_t wanted_outer = static_cast<std::uint32_t>(
            parse_environment_u64("PSPRECOMP_PHYSICS_VCALL_CENSUS_OUTER_PC", 0x08898F70u));
        static const std::uint32_t wanted_ra = static_cast<std::uint32_t>(
            parse_environment_u64("PSPRECOMP_PHYSICS_VCALL_CENSUS_RA", 0x0889912Cu));
        if (outer_pc == wanted_outer && (wanted_ra == 0u || ctx.gpr[31] == wanted_ra)) {
            ++physics_vcall_census_state.emitted;
            std::cerr << "[physics-vcall] vblank=" << display_vblank_index
                      << " target=" << psprecomp::hex32(target_pc)
                      << " outer=" << psprecomp::hex32(outer_pc)
                      << " a0=" << psprecomp::hex32(ctx.gpr[4])
                      << " a1=" << psprecomp::hex32(ctx.gpr[5])
                      << " a2=" << psprecomp::hex32(ctx.gpr[6])
                      << " a3=" << psprecomp::hex32(ctx.gpr[7])
                      << " ra=" << psprecomp::hex32(ctx.gpr[31])
                      << " a0_words=" << collision_probe_object_words(rt, ctx.gpr[4])
                      << "\n";
        }
    }
    if (collision_root_probe_can_emit() && collision_root_probe_matches(target_pc) && collision_probe_a0_matches(ctx.gpr[4])) {
        ++collision_root_probe_state.emitted;
        std::cerr << "[collision-root-enter] vblank=" << display_vblank_index
                  << " native_depth=" << native_depth
                  << " target=" << psprecomp::hex32(target_pc)
                  << " outer=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                  << " a0=" << psprecomp::hex32(ctx.gpr[4])
                  << " a1=" << psprecomp::hex32(ctx.gpr[5])
                  << " a2=" << psprecomp::hex32(ctx.gpr[6])
                  << " a3=" << psprecomp::hex32(ctx.gpr[7])
                  << " sp=" << psprecomp::hex32(ctx.gpr[29])
                  << " ra=" << psprecomp::hex32(ctx.gpr[31])
                  << " f12=" << ctx.fpr[12]
                  << " f13=" << ctx.fpr[13]
                  << " f14=" << ctx.fpr[14]
                  << " a0_words=" << collision_probe_object_words(rt, ctx.gpr[4])
                  << " a0_ptr70=" << collision_probe_pointer70_words(rt, ctx.gpr[4])
                  << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
                  << "\n";
    }
    if (!traced) return;
    std::cerr << "[collision-chain-enter] vblank=" << display_vblank_index
              << " level=" << (collision_chain_trace_stack.size() - 1u)
              << " native_depth=" << native_depth
              << " target=" << psprecomp::hex32(target_pc)
              << " outer=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
              << " a0=" << psprecomp::hex32(ctx.gpr[4])
              << " a1=" << psprecomp::hex32(ctx.gpr[5])
              << " a2=" << psprecomp::hex32(ctx.gpr[6])
              << " a3=" << psprecomp::hex32(ctx.gpr[7])
              << " sp=" << psprecomp::hex32(ctx.gpr[29])
              << " ra=" << psprecomp::hex32(ctx.gpr[31])
              << " f12=" << ctx.fpr[12]
              << " f13=" << ctx.fpr[13]
              << " f14=" << ctx.fpr[14]
              << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
              << "\n";
}

void vcs_pre_dispatch_hook(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                           std::uint32_t dispatch_pc, std::int32_t dispatch_thread_uid) {
    if (collision_root_probe_can_emit() && collision_root_probe_matches(dispatch_pc) && collision_probe_a0_matches(ctx.gpr[4])) {
        ++collision_root_probe_state.emitted;
        std::cerr << "[collision-root-outer-enter] vblank=" << display_vblank_index
                  << " uid=" << dispatch_thread_uid
                  << " pc=" << psprecomp::hex32(dispatch_pc)
                  << " a0=" << psprecomp::hex32(ctx.gpr[4])
                  << " a1=" << psprecomp::hex32(ctx.gpr[5])
                  << " a2=" << psprecomp::hex32(ctx.gpr[6])
                  << " a3=" << psprecomp::hex32(ctx.gpr[7])
                  << " sp=" << psprecomp::hex32(ctx.gpr[29])
                  << " ra=" << psprecomp::hex32(ctx.gpr[31])
                  << " f12=" << ctx.fpr[12]
                  << " f13=" << ctx.fpr[13]
                  << " f14=" << ctx.fpr[14]
                  << " a0_words=" << collision_probe_object_words(rt, ctx.gpr[4])
                  << " a0_ptr70=" << collision_probe_pointer70_words(rt, ctx.gpr[4])
                  << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
                  << "\n";
    }
    if (!collision_chain_trace_in_window()) return;
    const std::uint32_t root_start = collision_chain_trace_root_start();
    const std::uint32_t root_end = collision_chain_trace_root_end();
    if (dispatch_pc < root_start || dispatch_pc > root_end) return;
    std::cerr << "[collision-outer-enter] vblank=" << display_vblank_index
              << " uid=" << dispatch_thread_uid
              << " pc=" << psprecomp::hex32(dispatch_pc)
              << " a0=" << psprecomp::hex32(ctx.gpr[4])
              << " a1=" << psprecomp::hex32(ctx.gpr[5])
              << " a2=" << psprecomp::hex32(ctx.gpr[6])
              << " a3=" << psprecomp::hex32(ctx.gpr[7])
              << " sp=" << psprecomp::hex32(ctx.gpr[29])
              << " ra=" << psprecomp::hex32(ctx.gpr[31])
              << " f12=" << ctx.fpr[12]
              << " f13=" << ctx.fpr[13]
              << " f14=" << ctx.fpr[14]
              << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
              << "\n";
}

void vcs_post_chained_call_hook(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                                std::uint32_t target_pc, std::uint32_t native_depth) {
    if (collision_chain_trace_stack.empty()) return;
    const CollisionChainTraceFrame frame = collision_chain_trace_stack.back();
    collision_chain_trace_stack.pop_back();
    if (collision_root_probe_in_window() && collision_root_probe_matches(frame.target) && collision_probe_a0_matches(frame.a0)) {
        std::cerr << "[collision-root-exit] vblank=" << display_vblank_index
                  << " native_depth=" << native_depth
                  << " target=" << psprecomp::hex32(frame.target)
                  << " next=" << psprecomp::hex32(ctx.pc)
                  << " v0=" << psprecomp::hex32(ctx.gpr[2])
                  << " v1=" << psprecomp::hex32(ctx.gpr[3])
                  << " f0=" << ctx.fpr[0]
                  << " saved_a0=" << psprecomp::hex32(frame.a0)
                  << " a0_words=" << collision_probe_object_words(rt, frame.a0)
                  << " a0_ptr70=" << collision_probe_pointer70_words(rt, frame.a0)
                  << " saved_a1=" << psprecomp::hex32(frame.a1)
                  << " a1_words=" << collision_trace_words(rt, frame.a1)
                  << "\n";
    }
    if (frame.traced && frame.target == 0x0893084Cu &&
        collision_point_trace_enabled()) {
        std::cerr << "[collision-point] vblank=" << display_vblank_index
                  << " entity=" << psprecomp::hex32(frame.a0)
                  << " other=" << psprecomp::hex32(frame.a1)
                  << " count=" << ctx.gpr[2]
                  << " col0=" << collision_trace_colpoint(rt, frame.a2)
                  << "\n";
    }
    if (!frame.traced) return;
    std::cerr << "[collision-chain-exit] vblank=" << display_vblank_index
              << " level=" << collision_chain_trace_stack.size()
              << " native_depth=" << native_depth
              << " target=" << psprecomp::hex32(target_pc)
              << " next=" << psprecomp::hex32(ctx.pc)
              << " v0=" << psprecomp::hex32(ctx.gpr[2])
              << " v1=" << psprecomp::hex32(ctx.gpr[3])
              << " f0=" << ctx.fpr[0]
              << " saved_a1=" << psprecomp::hex32(frame.a1)
              << " a1_words=" << collision_trace_words(rt, frame.a1)
              << "\n";
}

void vcs_post_dispatch_hook(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                            std::uint32_t dispatch_pc, std::int32_t dispatch_thread_uid) {

    if (collision_root_probe_in_window() && collision_root_probe_matches(dispatch_pc)) {
        std::cerr << "[collision-root-outer-exit] vblank=" << display_vblank_index
                  << " uid=" << dispatch_thread_uid
                  << " pc=" << psprecomp::hex32(dispatch_pc)
                  << " next=" << psprecomp::hex32(ctx.pc)
                  << " v0=" << psprecomp::hex32(ctx.gpr[2])
                  << " v1=" << psprecomp::hex32(ctx.gpr[3])
                  << " f0=" << ctx.fpr[0]
                  << " a0=" << psprecomp::hex32(ctx.gpr[4])
                  << " a0_words=" << collision_probe_object_words(rt, ctx.gpr[4])
                  << " a0_ptr70=" << collision_probe_pointer70_words(rt, ctx.gpr[4])
                  << " a1=" << psprecomp::hex32(ctx.gpr[5])
                  << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
                  << "\n";
    }
    if (collision_chain_trace_in_window()) {
        const std::uint32_t root_start = static_cast<std::uint32_t>(
            parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_ROOT_START", 0x0899F9ECu));
        const std::uint32_t root_end = static_cast<std::uint32_t>(
            parse_environment_u64("PSPRECOMP_COLLISION_CHAIN_TRACE_ROOT_END", root_start));
        if (dispatch_pc >= root_start && dispatch_pc <= root_end) {
            std::cerr << "[collision-outer-exit] vblank=" << display_vblank_index
                      << " uid=" << dispatch_thread_uid
                      << " pc=" << psprecomp::hex32(dispatch_pc)
                      << " next=" << psprecomp::hex32(ctx.pc)
                      << " v0=" << psprecomp::hex32(ctx.gpr[2])
                      << " v1=" << psprecomp::hex32(ctx.gpr[3])
                      << " f0=" << ctx.fpr[0]
                      << " a1=" << psprecomp::hex32(ctx.gpr[5])
                      << " a1_words=" << collision_trace_words(rt, ctx.gpr[5])
                      << "\n";
        }
    }
    if (execution_clock_dispatch_interval == 0u && frozen_clock_guard_limit != 0u) {
        const bool has_delayed_thread = std::any_of(
            thread_table.threads.begin(), thread_table.threads.end(), [](const auto &item) {
                return item.second.state == ThreadState::Delayed;
            });
        if (display_vblank_index != frozen_clock_guard_vblank || !has_delayed_thread) {
            frozen_clock_guard_vblank = display_vblank_index;
            frozen_clock_guard_dispatches = 0u;
        } else if (++frozen_clock_guard_dispatches >= frozen_clock_guard_limit) {
            std::cerr << "[frozen-clock-guard] vblank=" << display_vblank_index
                      << " dispatches=" << frozen_clock_guard_dispatches
                      << " pc=" << psprecomp::hex32(dispatch_pc)
                      << " current_uid=" << thread_table.current_uid
                      << " delayed=";
            bool first = true;
            std::size_t reported = 0u;
            for (const auto &[uid, thread] : thread_table.threads) {
                if (thread.state != ThreadState::Delayed) continue;
                if (!first) std::cerr << ',';
                std::cerr << uid << ':' << thread.name << "@" << thread.delay_until_us;
                first = false;
                if (++reported == 8u) break;
            }
            std::cerr << " virtual_time_us=" << virtual_time_us << "\n";
            rt.stop(
                "Execution-driven PSP clock is disabled while delayed threads are pending. "
                "Remove PSPRECOMP_TIME_TICK_DISPATCHES=0 (default is 256), or set "
                "PSPRECOMP_FROZEN_CLOCK_GUARD_DISPATCHES=0 only for an isolated ordering test.");
            return;
        }
    }

    if (deferred_io_resumes.empty()) return;

    std::vector<std::int32_t> completed;
    completed.reserve(deferred_io_resumes.size());
    for (auto &[worker_uid, barrier] : deferred_io_resumes) {
        ++barrier.observed_dispatches;
        if (dispatch_thread_uid == barrier.handoff_uid && dispatch_pc == barrier.release_pc)
            completed.push_back(worker_uid);
    }
    if (completed.empty()) return;

    for (const std::int32_t worker_uid : completed) {
        const auto barrier = deferred_io_resumes.find(worker_uid);
        const auto worker = thread_table.threads.find(worker_uid);
        if (barrier == deferred_io_resumes.end()) continue;
        if (worker != thread_table.threads.end() && worker->second.state == ThreadState::IoDeferred) {
            if (std::getenv("PSPRECOMP_UMD_STREAM_DIAG") != nullptr ||
                std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
                std::cerr << "[io-handoff] release worker=" << worker_uid
                          << " handoff_uid=" << barrier->second.handoff_uid
                          << " handoff_pc=" << psprecomp::hex32(barrier->second.handoff_pc)
                          << " release_pc=" << psprecomp::hex32(barrier->second.release_pc)
                          << " observed=" << barrier->second.observed_dispatches
                          << " resume=" << psprecomp::hex32(worker->second.suspended_context.pc)
                          << "\n";
            }
            if (worker->second.externally_suspended) {
                worker->second.state = ThreadState::Ready;
            } else {
                enqueue_continuation(worker_uid, worker->second.suspended_context);
            }
        }
        deferred_io_resumes.erase(barrier);
    }
    refresh_vcs_post_dispatch_hook();

    // The completed read worker normally has higher PSP priority than the
    // submitter.  The return dispatch is now finished, so this boundary is the
    // first safe point at which the real kernel could schedule it again.
    const auto current = thread_table.threads.find(thread_table.current_uid);
    const auto best = best_ready_thread();
    if (current == thread_table.threads.end() || current->second.state != ThreadState::Running ||
        best == thread_table.continuations.end() ||
        thread_priority(best->uid) >= thread_priority(thread_table.current_uid)) return;

    enqueue_continuation(thread_table.current_uid, ctx);
    (void)activate_next_thread(ctx, "io-handoff-complete");
}


// Wall-clock split between translated guest execution and software
// rasterization.  Reported per vblank under PSPRECOMP_FRAME_TIME_DIAG so the
// frame budget can be attributed instead of guessed at.
struct FrameTimeStats {
    std::chrono::steady_clock::duration ge_time{};
    // Frame assembly, swapchain blit, present and any fence wait they imply.
    // It used to be folded into cpu_us, where it was indistinguishable from
    // recompiled MIPS execution -- the two are attacked in completely different
    // ways, so the split has to be visible.
    std::chrono::steady_clock::duration present_time{};
    std::chrono::steady_clock::time_point last_vblank{};
    std::uint64_t ge_calls{};
    std::uint64_t last_guest_time{};
    bool started{};
};
FrameTimeStats frame_time_stats;

// Presentation census, reported at shutdown.  See the call site for why.
std::uint64_t swapchain_presents{};
std::uint64_t software_presents{};
std::uint64_t software_presents_after_gpu{};
bool gpu_has_presented{};

// GE command words interpreted since the last vblank report.  list_us without
// it cannot say whether display-list execution is slow per command or simply
// has an enormous number of them, and those have opposite fixes.

struct RealtimeSpeedSample {
    double host_us_per_vblank{};
    double guest_us_per_vblank{};
    double simulated_vblank_hz{};
    double emulation_speed_percent{};
};

RealtimeSpeedSample calculate_realtime_speed_sample(std::uint64_t host_us,
                                                     std::uint64_t guest_us,
                                                     std::uint64_t vblanks) {
    if (host_us == 0u || vblanks == 0u) return {};
    const double host = static_cast<double>(host_us);
    const double guest = static_cast<double>(guest_us);
    const double frames = static_cast<double>(vblanks);
    return RealtimeSpeedSample{
        host / frames,
        guest / frames,
        frames * 1'000'000.0 / host,
        guest * 100.0 / host,
    };
}

struct RealtimeSpeedStats {
    std::chrono::steady_clock::time_point host_start{};
    std::uint64_t guest_start{};
    std::uint64_t vblank_start{};
    bool started{};
};
RealtimeSpeedStats realtime_speed_stats;

bool realtime_speed_diag_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_REALTIME_SPEED_DIAG") != nullptr;
    return enabled;
}

std::uint64_t realtime_speed_diag_interval() {
    static const std::uint64_t interval = std::max<std::uint64_t>(1u,
        parse_environment_u64("PSPRECOMP_REALTIME_SPEED_INTERVAL", 120u));
    return interval;
}

std::uint64_t gpu_dump_vblank() noexcept {
    static const std::uint64_t value = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_DUMP_VBLANK");
        if (text == nullptr || *text == '\0') return std::uint64_t{0};
        char *end = nullptr;
        const unsigned long long parsed = std::strtoull(text, &end, 10);
        return end != text && *end == '\0' ? static_cast<std::uint64_t>(parsed) : 0u;
    }();
    return value;
}

bool gpu_color_preview_enabled() noexcept {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_COLOR_PREVIEW");
        return text != nullptr && *text != '\0' && std::strcmp(text, "0") != 0;
    }();
    return enabled;
}

void dump_gpu_internal_frame_if_requested(std::uint64_t vblank) {
    static bool dumped = false;
    const std::uint64_t requested = gpu_dump_vblank();
    if (dumped || requested == 0u || vblank < requested) return;
    const GeGpuBackendReport report = ge_gpu_backend_report();
    if (report.game_frame_vblank == 0u || report.offscreen_width == 0u ||
        report.offscreen_height == 0u || report.game_frame_readback_bytes == 0u) return;
    std::vector<std::byte> rgba(report.game_frame_readback_bytes);
    if (!ge_gpu_backend_copy_game_frame_rgba(rgba)) return;
    std::filesystem::path output_path;
    if (const char *path = std::getenv("PSPRECOMP_GE_GPU_DUMP_PATH");
        path != nullptr && *path != '\0') {
        output_path = path;
    } else {
        std::ostringstream name;
        name << "VCSNative_internal_" << report.offscreen_width << 'x'
             << report.offscreen_height << "_vblank_" << std::setw(6)
             << std::setfill('0') << report.game_frame_vblank << ".ppm";
        output_path = name.str();
    }
    std::ofstream output(output_path, std::ios::binary | std::ios::trunc);
    if (!output) return;
    output << "P6\n" << report.offscreen_width << ' ' << report.offscreen_height << "\n255\n";
    for (std::size_t offset = 0u; offset + 3u < rgba.size(); offset += 4u) {
        const char rgb[3]{
            static_cast<char>(rgba[offset + 0u]),
            static_cast<char>(rgba[offset + 1u]),
            static_cast<char>(rgba[offset + 2u]),
        };
        output.write(rgb, sizeof(rgb));
    }
    if (output.good()) {
        dumped = true;
        std::cerr << "[gpu-internal-frame] vblank=" << report.game_frame_vblank
                  << " resolution=" << report.offscreen_width << 'x' << report.offscreen_height
                  << " changed_pixels=" << report.game_frame_changed_pixels
                  << " checksum=" << report.game_frame_checksum
                  << " depth_attachment=" << report.depth_attachment_active
                  << " depth_variants=" << report.depth_pipeline_variants_created
                  << " depth_tested_draws=" << report.depth_tested_game_draw_calls
                  << " depth_writing_draws=" << report.depth_writing_game_draw_calls
                  << " alpha_shader=" << report.alpha_test_shader_active
                  << " alpha_tested_draws=" << report.alpha_tested_game_draw_calls
                  << " blend_active=" << report.standard_alpha_blend_pipeline_active
                  << " observed_blends_active=" << report.observed_blend_modes_pipeline_active
                  << " blend_variants=" << report.blend_pipeline_variants_created
                  << " standard_blended_draws=" << report.standard_alpha_blended_game_draw_calls
                  << " fixed_replace_blended_draws=" << report.fixed_replace_blended_game_draw_calls
                  << " additive_blended_draws=" << report.additive_blended_game_draw_calls
                  << " unsupported_blend_draws=" << report.unsupported_blend_game_draw_calls
                  << " texture_function_active=" << report.observed_texture_function_shader_active
                  << " complete_texture_functions=" << report.complete_texture_function_shader_active
                  << " modulate_texture_draws=" << report.modulate_texture_game_draw_calls
                  << " decal_texture_draws=" << report.decal_texture_game_draw_calls
                  << " blend_texture_draws=" << report.blend_texture_game_draw_calls
                  << " replace_texture_draws=" << report.replace_texture_game_draw_calls
                  << " add_texture_draws=" << report.add_texture_game_draw_calls
                  << " double_color_texture_draws=" << report.double_color_texture_game_draw_calls
                  << " unsupported_texture_function_draws=" << report.unsupported_texture_function_game_draw_calls
                  << " color_mask_active=" << report.color_write_mask_pipeline_active
                  << " color_mask_variants=" << report.color_mask_pipeline_variants_created
                  << " masked_color_draws=" << report.masked_color_game_draw_calls
                  << " unsupported_partial_color_masks=" << report.unsupported_partial_color_mask_game_draw_calls
                  << " base_texture_formats=" << report.base_texture_formats_active
                  << " decoded_direct16=" << report.decoded_direct16_textures
                  << " decoded_direct32=" << report.decoded_direct32_textures
                  << " decoded_indexed16=" << report.decoded_indexed16_textures
                  << " decoded_indexed32=" << report.decoded_indexed32_textures
                  << " compressed_texture_formats=" << report.compressed_texture_formats_active
                  << " decoded_dxt1=" << report.decoded_dxt1_textures
                  << " decoded_dxt3=" << report.decoded_dxt3_textures
                  << " decoded_dxt5=" << report.decoded_dxt5_textures
                  << " mipmap_state=" << report.mipmap_state_active
                  << " mipmapped_draws=" << report.mipmapped_game_draw_calls
                  << " mip_linear_draws=" << report.mip_linear_game_draw_calls
                  << " fixed_lod_draws=" << report.fixed_lod_game_draw_calls
                  << " selected_nonzero_mip_draws=" << report.selected_nonzero_mip_game_draw_calls
                  << " fog_shader=" << report.fog_shader_active
                  << " fogged_draws=" << report.fogged_game_draw_calls
                  << " path=" << output_path.string() << "\n";
    }
}

void report_realtime_speed_if_requested() {
    if (!realtime_speed_diag_enabled()) return;
    const auto now = std::chrono::steady_clock::now();
    if (!realtime_speed_stats.started) {
        realtime_speed_stats.host_start = now;
        realtime_speed_stats.guest_start = virtual_time_us;
        realtime_speed_stats.vblank_start = display_vblank_index;
        realtime_speed_stats.started = true;
        return;
    }

    const std::uint64_t vblanks = display_vblank_index - realtime_speed_stats.vblank_start;
    if (vblanks < realtime_speed_diag_interval()) return;
    const std::uint64_t host_us = static_cast<std::uint64_t>(
        std::max<std::int64_t>(1, std::chrono::duration_cast<std::chrono::microseconds>(
            now - realtime_speed_stats.host_start).count()));
    const std::uint64_t guest_us = virtual_time_us - realtime_speed_stats.guest_start;
    const RealtimeSpeedSample sample = calculate_realtime_speed_sample(host_us, guest_us, vblanks);

    const char *diagnosis = "normal";
    // A correct PSP clock should average close to one 59.94 Hz period per
    // logical vblank. If that is correct but wall-clock throughput is lower,
    // the observed slow motion is performance-bound rather than a doubled tick.
    if (sample.guest_us_per_vblank < 15'000.0 || sample.guest_us_per_vblank > 18'500.0) {
        diagnosis = "guest-clock-mismatch";
    } else if (sample.emulation_speed_percent < 90.0) {
        diagnosis = "host-cannot-keep-up-slow-motion";
    } else if (sample.emulation_speed_percent > 110.0) {
        diagnosis = "running-faster-than-realtime";
    }

    // Composed and written once: see write_diag_line.  The local stream also
    // keeps std::fixed/setprecision off std::cerr itself, which used to leak
    // into every later diagnostic on the stream.
    std::ostringstream speed_line;
    speed_line << std::fixed << std::setprecision(3)
               << "[realtime-speed] vblank=" << display_vblank_index
               << " window_vblanks=" << vblanks
               << " host_us=" << host_us
               << " guest_us=" << guest_us
               << " host_us_per_vblank=" << sample.host_us_per_vblank
               << " guest_us_per_vblank=" << sample.guest_us_per_vblank
               << " simulated_vblank_hz=" << sample.simulated_vblank_hz
               << " emulation_speed_percent=" << sample.emulation_speed_percent
               << " diagnosis=" << diagnosis << "\n";
    const std::string speed_text = speed_line.str();
    std::cerr.write(speed_text.data(), static_cast<std::streamsize>(speed_text.size()));

    realtime_speed_stats.host_start = now;
    realtime_speed_stats.guest_start = virtual_time_us;
    realtime_speed_stats.vblank_start = display_vblank_index;
}

bool frame_time_diag_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_FRAME_TIME_DIAG") != nullptr;
    return enabled;
}

bool ge_phase_diag_line_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_GE_PHASE_DIAG") != nullptr;
    return enabled;
}

bool gpu_timing_diag_line_enabled() {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GPU_TIMING_DIAG");
        return text != nullptr && *text != '\0' && std::strcmp(text, "0") != 0;
    }();
    return enabled;
}

struct GpuTimingCensus {
    GeGpuBackendReport previous{};
    bool started{};
};
GpuTimingCensus gpu_timing_census;

// std::cerr is unit-buffered: every operator<< flushes, which on Windows is a
// separate WriteFile on the redirected handle.  A per-vblank diagnostic line is
// a dozen of those, and measured against each other two runs showed one such
// line costing over a millisecond per vblank -- the profiler was reporting a
// frame budget the game does not actually have, and the stutter it produced was
// visible while playing.  Compose the line first, emit it with one write.
void write_diag_line(const std::ostringstream &line) {
    const std::string text = line.str();
    std::cerr.write(text.data(), static_cast<std::streamsize>(text.size()));
}

// Paces the vblank loop against the guest clock.
//
// There was no frame limiter anywhere in the host: the loop ran vblanks as fast
// as the machine allowed. A light scene therefore played at 200-400% speed --
// the game "starting accelerated" -- while a heavy one fell to 46%, and the two
// together read as wildly inconsistent speed rather than as a slow section.
// VCS derives its logic from the vblank clock (see the previous handoff's §7),
// so pacing vblanks is what makes wall-clock speed match the guest's own idea
// of time.
//
// Falling behind is not repaid: catching up by running the next vblanks early
// would turn a slow section into a fast-forward. The anchor is reset instead,
// so a slow stretch is simply slow and normal speed resumes after it.
//
// PSPRECOMP_FRAME_LIMIT=0 disables it, which is what performance measurement
// needs -- with the limiter on, frame_us just reads back the target period.
void limit_frame_rate() {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_FRAME_LIMIT");
        return text == nullptr || (*text != '\0' && std::strcmp(text, "0") != 0);
    }();
    if (!enabled) return;

    static bool anchored = false;
    static std::chrono::steady_clock::time_point wall_anchor{};
    static std::uint64_t guest_anchor = 0u;
    if (!anchored) {
        anchored = true;
        wall_anchor = std::chrono::steady_clock::now();
        guest_anchor = virtual_time_us;
        return;
    }

    const auto target = wall_anchor + std::chrono::microseconds(virtual_time_us - guest_anchor);
    const auto now = std::chrono::steady_clock::now();
    if (now >= target) {
        // The selected rate is a ceiling, not a promise that this renderer can
        // finish inside the budget. When it misses, advance guest time by the
        // wall-clock deficit so the game runs at NORMAL SPEED at whatever FPS
        // the host sustains.
        //
        // This used to happen only above 60 Hz. Below it, the code either let
        // the debt accumulate or re-anchored and forgave it -- and both leave
        // the guest clock permanently behind the wall clock, which is slow
        // motion by definition. That is what "the counter says 24 fps but it
        // feels much slower" was, and the audio rides the same clock, so the
        // radio and everything else dragged with it.
        //
        // The correction is capped per vblank. Uncapped at 240 Hz a 30 ms frame
        // moved the guest clock seven periods at once, and the audio mixer
        // sealed and queued in bursts until the device ring sat permanently
        // full: 24 of 24 blocks, 0.68 s of latency and 3387 timeline resyncs.
        // Capping keeps the clock honest without the leap.
        const auto behind = std::chrono::duration_cast<std::chrono::microseconds>(
            now - target).count();
        if (behind > 0) {
            const std::uint64_t cap = virtual_vblank_period_us() * 4u;
            virtual_time_us += std::min(static_cast<std::uint64_t>(behind), cap);
        }
        wall_anchor = now;
        guest_anchor = virtual_time_us;
        return;
    }
    // Sleep the bulk, spin the tail: a plain sleep_until overshoots by up to a
    // scheduler tick, which at 60 Hz is most of a frame.
    constexpr auto spin_margin = std::chrono::microseconds(1500);
    if (target - now > spin_margin) std::this_thread::sleep_until(target - spin_margin);
    while (std::chrono::steady_clock::now() < target) std::this_thread::yield();
}





// sceIoRead world-stream integration: the request pointer and release hint are
// sampled before the read lands, then used for the deferred worker handoff.
std::uint64_t vcs_before_umd_read(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
    std::uint32_t stream_request = 0u;
    std::uint32_t release_pc_hint = 0u;
    if (thread_table.current_uid == 5 &&
        rt.memory().contains(ctx.gpr[22] + 6916u, 4u)) {
        stream_request = rt.memory().load32(ctx.gpr[22] + 6916u);
        release_pc_hint = uncommitted_world_stream_release_pc(rt, stream_request);
    }
    return (static_cast<std::uint64_t>(stream_request) << 32u) | release_pc_hint;
}

void vcs_after_umd_read(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx,
                        std::uint32_t size, std::size_t read, std::uint64_t token) {
    const auto stream_request = static_cast<std::uint32_t>(token >> 32u);
    const auto release_pc_hint = static_cast<std::uint32_t>(token);
    static const bool umd_stream_diag = std::getenv("PSPRECOMP_UMD_STREAM_DIAG") != nullptr;
    if (umd_stream_diag &&
        thread_table.current_uid == 5 &&
        rt.memory().contains(ctx.gpr[22] + 6916u, 4u)) {
        static std::uint64_t stream_read_count = 0u;
        ++stream_read_count;
        const std::uint32_t request = stream_request;
        if (request != 0u && rt.memory().contains(request, 52u)) {
            const std::uint32_t remaining = rt.memory().load32(request + 24u);
            const std::uint32_t progressed = rt.memory().load32(request + 28u);
            const std::uint32_t callback = rt.memory().load32(request + 48u);
            if (stream_read_count <= 128u || remaining <= read || callback != 0u ||
                stream_read_count % 4096u == 0u) {
                std::cerr << "[umdstream] read#" << stream_read_count
                          << " req=" << psprecomp::hex32(request)
                          << " source=" << psprecomp::hex32(rt.memory().load32(request + 16u))
                          << " offset=" << rt.memory().load32(request + 20u)
                          << " remaining=" << remaining
                          << " progressed=" << progressed
                          << " callback=" << psprecomp::hex32(callback)
                          << " asked=" << size << " got=" << read
                          << " return_pc=" << psprecomp::hex32(ctx.gpr[31])
                          << " release_hint=" << psprecomp::hex32(release_pc_hint) << "\n";
            }
        }
    }
    // A host-backed UMD read may finish in the same native timeslice in
    // which a higher-priority worker was woken.  Physical PSP I/O could not:
    // the request submitter had time to return and store its request pointer.
    // Defer the worker until that exact translated return dispatch completes.
    // This is independent of the optional execution-driven virtual clock, so
    // PSPRECOMP_TIME_TICK_DISPATCHES=0 cannot strand the worker forever.
    if (read != 0u) {
        (void)defer_current_thread_for_io_handoff(
            rt, ctx, static_cast<std::uint32_t>(read), release_pc_hint);
    } else {
        ctx.set_gpr(2, 0u);
    }
}

void vcs_thread_removed(std::int32_t uid) {
    deferred_io_resumes.erase(uid);
    refresh_vcs_post_dispatch_hook();
}

void vcs_dump_event_stall_extra() {
    for (const auto &[worker_uid, barrier] : deferred_io_resumes) {
        std::cerr << "[event-stall-io] worker=" << worker_uid
                  << " handoff_uid=" << barrier.handoff_uid
                  << " handoff_pc=" << psprecomp::hex32(barrier.handoff_pc)
                  << " release_pc=" << psprecomp::hex32(barrier.release_pc)
                  << " observed=" << barrier.observed_dispatches << "\n";
    }
}

bool continue_mpeg_ringbuffer_callback(psprecomp::Runtime &runtime,
                                        psprecomp::AllegrexContext &ctx,
                                        AsyncReturnFrame &frame) {
    if (!runtime.memory().contains(frame.ring_address, 48u)) {
        runtime.stop("MPEG ringbuffer callback returned to an invalid ringbuffer");
        return false;
    }
    const auto callback_result = static_cast<std::int32_t>(ctx.gpr[2]);
    const std::int32_t packets = static_cast<std::int32_t>(runtime.memory().load32(frame.ring_address));
    std::int32_t write_position = static_cast<std::int32_t>(runtime.memory().load32(frame.ring_address + 8u));
    std::int32_t packets_available = static_cast<std::int32_t>(runtime.memory().load32(frame.ring_address + 12u));
    if (packets <= 0) {
        runtime.stop("MPEG ringbuffer callback returned to a ring with no packets");
        return false;
    }

    if (callback_result > 0) {
        const std::int32_t accepted = std::min({callback_result, frame.requested_this_round,
                                                std::max(0, packets - packets_available)});
        frame.total_packets += accepted;
        write_position += accepted;
        packets_available += accepted;
        runtime.memory().store32(frame.ring_address + 4u,
                                 runtime.memory().load32(frame.ring_address + 4u) + static_cast<std::uint32_t>(accepted));
        runtime.memory().store32(frame.ring_address + 8u, static_cast<std::uint32_t>(write_position));
        runtime.memory().store32(frame.ring_address + 12u, static_cast<std::uint32_t>(packets_available));
    }

    if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr) {
        std::cerr << "[mpeg] ring callback returned=" << callback_result
                  << " total=" << frame.total_packets
                  << " remaining=" << frame.remaining_packets
                  << " write_pos=" << write_position
                  << " used=" << packets_available << "\n";
    }

    if (callback_result > 0 && frame.remaining_packets > 0 && packets_available < packets) {
        const std::int32_t write_offset = write_position % packets;
        const std::int32_t desired = std::min({frame.remaining_packets, packets - write_offset,
                                               packets - packets_available});
        if (desired > 0) {
            frame.remaining_packets -= desired;
            frame.requested_this_round = desired;
            const std::uint32_t data = runtime.memory().load32(frame.ring_address + 20u);
            const std::uint32_t callback = runtime.memory().load32(frame.ring_address + 24u);
            const std::uint32_t argument = runtime.memory().load32(frame.ring_address + 28u);
            ctx = frame.resume;
            ctx.set_gpr(4, data + static_cast<std::uint32_t>(write_offset) * 2048u);
            ctx.set_gpr(5, static_cast<std::uint32_t>(desired));
            ctx.set_gpr(6, argument);
            ctx.set_gpr(31, 0x00000004u);
            ctx.pc = callback;
            return true;
        }
    }

    ctx = frame.resume;
    if (callback_result < 0 && frame.total_packets == 0)
        ctx.set_gpr(2, static_cast<std::uint32_t>(callback_result));
    else
        ctx.set_gpr(2, static_cast<std::uint32_t>(frame.total_packets));
    return false;
}

}

void vcs_account_ge_time(std::chrono::steady_clock::duration elapsed) {
    frame_time_stats.ge_time += elapsed;
    ++frame_time_stats.ge_calls;
}

std::uint32_t vcs_host_buttons() { return display_window_buttons(); }

void vcs_host_analog(std::uint8_t &x, std::uint8_t &y) {
    const HostInputState host = display_window_input();
    // The camera axes leave here rather than through sceCtrl: the PSP pad has
    // no field for them. The guest reads them through the hook in
    // generated_unit_0098.cpp.
    vcs_camera_set_axes(host.camera_x, host.camera_y);
    // Same reason: throttle and brake reach the guest through the vehicle's own
    // accessors, not through the pad's Cross and Square, so that W and S can
    // drive without also sprinting and jumping on foot.
    vcs_set_host_drive_inputs(host.accelerate, host.brake);
    x = host.analog_x;
    y = host.analog_y;
}

// Saves live beside the executable, not inside the (possibly read-only, shared)
// extracted game data; tests run without a configuration and fall back.
std::filesystem::path vcs_savedata_root() {
    const VcsConfiguration &config = vcs_configuration();
    if (config.initialized && !config.executable_directory.empty())
        return config.executable_directory / "SAVEDATA";
    return {};
}

bool vcs_before_vblank_wait(psprecomp::Runtime &rt, psprecomp::AllegrexContext &) {
    // The display consumes the completed GE frame.  This is a real PSP
    // visibility boundary: allow guest/GE overlap during the frame, then
    // wait only here before framebuffer presentation and vblank callbacks.
    return ge_async_wait_idle(rt);
}

// Per-vblank host work: audio clock, frame diagnostics, capture, presentation
// and frame pacing. Runs after display_vblank_index advanced.
bool vcs_on_vblank(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
    vcs::audio_output_advance(virtual_time_us);
    report_realtime_speed_if_requested();
    if (frame_time_diag_enabled()) {
        const auto now = std::chrono::steady_clock::now();
        if (frame_time_stats.started) {
            const auto frame = now - frame_time_stats.last_vblank;
            const auto frame_us = std::chrono::duration_cast<std::chrono::microseconds>(frame).count();
            const auto ge_us = std::chrono::duration_cast<std::chrono::microseconds>(
                frame_time_stats.ge_time).count();
            // guest_us is the delta the game itself observes.  If it holds a
            // steady ~16683 the guest believes it is running at 60 Hz no
            // matter how slow the host wall clock is, and any physics step
            // derived from it is unaffected by emulator speed.
            const auto present_us = std::chrono::duration_cast<std::chrono::microseconds>(
                frame_time_stats.present_time).count();
            const auto io_us = std::chrono::duration_cast<std::chrono::microseconds>(
                io_host_time_this_vblank).count();
            const auto ge_async_wait_us = ge_async_running()
                ? static_cast<std::int64_t>(ge_async.last_wait_ns.load(std::memory_order_acquire) / 1000u)
                : 0;
            // In async mode ge_us is worker CPU time that overlaps Allegrex
            // execution, so subtracting it from wall time would under-report
            // guest work.  Only the actual GE visibility wait is serialized.
            const auto accounted_non_guest = ge_async_running()
                ? present_us + io_us + ge_async_wait_us
                : ge_us + present_us + io_us;
            std::ostringstream frame_line;
            frame_line << "[frame-time] vblank=" << display_vblank_index
                       << " frame_us=" << frame_us
                       << " ge_us=" << ge_us
                       << " ge_async_wait_us=" << ge_async_wait_us
                       << " present_us=" << present_us
                       << " io_us=" << io_us
                       << " cpu_us=" << (ge_async_running()
                            ? (frame_us > present_us + io_us ? frame_us - present_us - io_us : 0)
                            : (frame_us > ge_us ? frame_us - ge_us : 0))
                       << " guest_cpu_us="
                       << (frame_us > accounted_non_guest ? frame_us - accounted_non_guest : 0)
                       << " guest_us=" << (virtual_time_us - frame_time_stats.last_guest_time)
                       << " ge_calls=" << frame_time_stats.ge_calls
                       << " fps=" << (frame_us > 0 ? 1000000 / frame_us : 0) << "\n";
            write_diag_line(frame_line);
            // Splits ge_us into the per-fragment pixel loop and everything
            // else, which is per-triangle geometry.  Says directly which of
            // the two a heavy frame is actually spent on.
            if (ge_phase_diag_line_enabled()) {
                const vcs::GePhaseTotals phases = vcs::ge_phase_totals();
                const std::int64_t pixel_us =
                    static_cast<std::int64_t>(phases.pixel_loop_ns / 1000u);
                const auto us = [](std::uint64_t ns) {
                    return static_cast<std::int64_t>(ns / 1000u);
                };
                // geometry_us stays as it was (ge_us minus the pixel loop) so
                // older logs remain comparable; the named sub-phases below
                // account for it and their sum plus pixel_us should be close
                // to ge_us, the remainder being GE list interpretation.
                const std::int64_t geometry_us = ge_us > pixel_us ? ge_us - pixel_us : 0;
                const std::int64_t accounted = pixel_us + us(phases.draw_setup_ns) +
                    us(phases.texture_upload_ns) + us(phases.vertex_decode_ns) +
                    us(phases.gpu_stage_ns) + us(phases.triangle_prep_ns) +
                    us(phases.gpu_accumulate_ns);
                std::ostringstream phase_line;
                phase_line << "[ge-phase] vblank=" << display_vblank_index
                           << " ge_us=" << ge_us
                           << " pixel_us=" << pixel_us
                           << " geometry_us=" << geometry_us
                           << " triangles=" << phases.triangles
                           << " draws=" << phases.primitives
                           << " verts=" << phases.vertices
                           << " setup_us=" << us(phases.draw_setup_ns)
                           << " texupload_us=" << us(phases.texture_upload_ns)
                           << " vdecode_us=" << us(phases.vertex_decode_ns)
                           << " stage_us=" << us(phases.gpu_stage_ns)
                           << " triprep_us=" << us(phases.triangle_prep_ns)
                           << " accum_us=" << us(phases.gpu_accumulate_ns)
                           << " list_us=" << (ge_us > accounted ? ge_us - accounted : 0)
                           << " ge_commands=" << ge_commands_this_vblank
                           << "\n";
                write_diag_line(phase_line);
                ge_commands_this_vblank = 0u;
                vcs::reset_ge_phase_totals();
            }
            if (gpu_timing_diag_line_enabled()) {
                const GeGpuBackendReport current = ge_gpu_backend_report();
                if (gpu_timing_census.started) {
                    const GeGpuBackendReport &previous = gpu_timing_census.previous;
                    const auto delta = [](std::uint64_t now_value, std::uint64_t old_value) {
                        return now_value >= old_value ? now_value - old_value : 0u;
                    };
                    const auto ns_to_us = [](std::uint64_t ns) { return ns / 1000u; };
                    std::ostringstream gpu_line;
                    gpu_line << "[gpu-time] vblank=" << display_vblank_index
                             << " finish_calls=" << delta(current.perf_finish_frame_calls, previous.perf_finish_frame_calls)
                             << " finish_us=" << ns_to_us(delta(current.perf_finish_frame_ns, previous.perf_finish_frame_ns))
                             << " fence_calls=" << delta(current.perf_wait_for_frame_calls, previous.perf_wait_for_frame_calls)
                             << " fence_us=" << ns_to_us(delta(current.perf_wait_for_frame_ns, previous.perf_wait_for_frame_ns))
                             << " flush_wait_calls=" << delta(current.perf_upload_flush_wait_calls, previous.perf_upload_flush_wait_calls)
                             << " flush_wait_us=" << ns_to_us(delta(current.perf_upload_flush_wait_ns, previous.perf_upload_flush_wait_ns))
                             << " acquire_calls=" << delta(current.perf_acquire_calls, previous.perf_acquire_calls)
                             << " acquire_us=" << ns_to_us(delta(current.perf_acquire_ns, previous.perf_acquire_ns))
                             << " submit_calls=" << delta(current.perf_queue_submit_calls, previous.perf_queue_submit_calls)
                             << " submit_us=" << ns_to_us(delta(current.perf_queue_submit_ns, previous.perf_queue_submit_ns))
                             << " present_calls=" << delta(current.perf_queue_present_calls, previous.perf_queue_present_calls)
                             << " queue_present_us=" << ns_to_us(delta(current.perf_queue_present_ns, previous.perf_queue_present_ns))
                             << " tex_requests=" << delta(current.texture_decode_requests, previous.texture_decode_requests)
                             << " tex_hits=" << delta(current.texture_cache_hits, previous.texture_cache_hits)
                             << " tex_uploads=" << delta(current.decoded_texture_uploads, previous.decoded_texture_uploads)
                             << " tex_evictions=" << delta(current.evicted_textures, previous.evicted_textures)
                             << " transfer_submits=" << delta(current.transfer_submissions, previous.transfer_submissions)
                             << " game_draws=" << delta(current.game_draw_calls, previous.game_draw_calls)
                             << " game_tris=" << delta(current.game_triangles, previous.game_triangles)
                             << " swapchain=" << current.swapchain_active
                             << " direct_present=" << current.gpu_frame_presented_to_window
                             << "\n";
                    write_diag_line(gpu_line);
                }
                gpu_timing_census.previous = current;
                gpu_timing_census.started = true;
            }
        }
        frame_time_stats.started = true;
        frame_time_stats.last_vblank = now;
        frame_time_stats.last_guest_time = virtual_time_us;
        frame_time_stats.ge_time = std::chrono::steady_clock::duration{};
        frame_time_stats.present_time = std::chrono::steady_clock::duration{};
        io_host_time_this_vblank = std::chrono::steady_clock::duration{};
        frame_time_stats.ge_calls = 0u;
    }
    const FramebufferDescription displayed{
        display_state.frame_buffer,
        display_state.width,
        display_state.height,
        display_state.buffer_width,
        display_state.pixel_format,
    };
    capture_frame_if_requested(rt.memory(), displayed);
    dump_ram_if_requested(rt.memory());
    ge_gpu_backend_set_display_framebuffer(display_state.frame_buffer);
    project2dfx_render_frame(
        rt.memory(), ctx.gpr[28], display_vblank_index, display_state.frame_buffer);
    // A movie frame is a finished 480x272 picture with no more image at the
    // sides, so widening it can only stretch it. Present it black-barred at
    // its own shape instead; gameplay keeps the widescreen treatment.
    display_window_set_aspect_lock(
        !movie_output_buffers.empty() &&
        movie_output_buffers.count(normalize_ram_address(display_state.frame_buffer)) != 0u);
    const auto present_entry = frame_time_diag_enabled()
        ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
    const bool gpu_frame_ready = ge_gpu_backend_finish_color_frame(display_vblank_index);
    // VCS only fills the displayed framebuffer on every other vblank, so the
    // GPU path produces a frame at half the vblank rate. Presenting the
    // software framebuffer in between alternated two differently scaled
    // images at 60 Hz, which reads as flicker. Hold the last GPU frame
    // instead, and only hand the window back to software when the GPU has
    // been silent for several vblanks (menus, videos, backend shutdown).
    static std::uint64_t vblanks_since_gpu_frame = 0u;
    static bool holding_gpu_frame = false;
    if (gpu_frame_ready) {
        holding_gpu_frame = true;
        vblanks_since_gpu_frame = 0u;
    } else if (holding_gpu_frame && ++vblanks_since_gpu_frame > 4u) {
        holding_gpu_frame = false;
    }
    bool presented_gpu_frame = false;
    if (ge_gpu_backend_presents_directly()) {
        // The backend blitted straight into the swapchain. Dropped vblanks
        // simply leave the previous image on screen, which is the hold
        // behaviour for free, and mixing a GDI blit into the same window
        // would fight the presentation engine.
        presented_gpu_frame = true;
    } else if ((gpu_frame_ready || holding_gpu_frame) &&
               (ge_gpu_backend_active() || gpu_color_preview_enabled())) {
        const GeGpuBackendReport gpu = ge_gpu_backend_report();
        const std::span<const std::byte> rgba = ge_gpu_backend_game_frame_rgba();
        if (!rgba.empty()) {
            display_window_present_rgba(rgba, gpu.offscreen_width, gpu.offscreen_height);
            ge_gpu_backend_mark_window_presented();
            presented_gpu_frame = true;
        }
    }
    if (!presented_gpu_frame) {
        holding_gpu_frame = false;
        // The window is showing the guest framebuffer that the software GE
        // filled.  Counting these tells whether
        // PSPRECOMP_GE_GPU_SKIP_DISPLAYED_RASTER could ever put a stale
        // surface on screen: the flag only skips while the swapchain is
        // presenting, so what matters is how often the run leaves that state
        // after having entered it.
        ++software_presents;
        if (gpu_has_presented) ++software_presents_after_gpu;
        display_window_present(rt.memory(), displayed);
    } else if (ge_gpu_backend_presents_directly()) {
        gpu_has_presented = true;
        ++swapchain_presents;
    }
    if (gpu_frame_ready) dump_gpu_internal_frame_if_requested(display_vblank_index);
    if (frame_time_diag_enabled())
        frame_time_stats.present_time += std::chrono::steady_clock::now() - present_entry;
    limit_frame_rate();
    if (display_window_close_requested()) {
        ctx.set_gpr(2, 0u);
        rt.stop("Display window closed by the user");
        return false;
    }
    return true;
}

void install_profile(psprecomp::Runtime &runtime, std::uint32_t user_arena_start) {
    install_native_fast_paths(runtime);
    if (configured_game_frame_rate() > 30u) {
        runtime.register_function(kGuestFrameLimiterBranch,
                                  &unlocked_frame_limiter_patch,
                                  "vcs_unlocked_frame_limiter");
    }
    std::cerr << "[frame-rate] target=" << configured_game_frame_rate()
              << " virtual_display=" << virtual_display_refresh_hz() << " Hz\n";
    psprecomp::hle::reset_io(psprecomp::hle::IoHooks{&vcs_before_umd_read, &vcs_after_umd_read});
    for (auto &[address, state] : mpeg_contexts) close_video_decoder(state);
    mpeg_contexts.clear();
    psprecomp::hle::reset_atrac();
    next_mpeg_stream_id = 1u;
    vcs::audio_output_shutdown();
    psprecomp::hle::reset_audio(psprecomp::hle::AudioHooks{
        &vcs::audio_output_enabled, &vcs::audio_output_submit, &vcs::audio_output_reset_channel});
    deferred_io_resumes.clear();
    psprecomp::hle::reset_kernel(runtime, user_arena_start, psprecomp::hle::KernelHooks{
        &vcs_thread_removed, &continue_mpeg_ringbuffer_callback, &vcs_dump_event_stall_extra});
    install_ge_renderer_hooks();
    psprecomp::hle::reset_ge(psprecomp::hle::GeRenderer{&render_ge_primitive, &test_ge_bounding_box},
                             psprecomp::hle::GeHooks{&vcs_account_ge_time});
    psprecomp::hle::reset_display(psprecomp::hle::DisplayHooks{
        &virtual_display_refresh_hz, &vcs_before_vblank_wait, &vcs_on_vblank});
    frame_time_stats = FrameTimeStats{};
    gpu_timing_census = GpuTimingCensus{};
    realtime_speed_stats = RealtimeSpeedStats{};
    frozen_clock_guard_dispatches = 0u;
    frozen_clock_guard_vblank = 0u;
    psprecomp::hle::reset_ctrl(psprecomp::hle::CtrlHooks{&vcs_host_buttons, &vcs_host_analog});
    psprecomp::hle::reset_utility(psprecomp::hle::UtilityHooks{&vcs_savedata_root});
    deflate_fast_pending.clear();
    collision_chain_trace_stack.clear();
    psprecomp::set_runtime_post_import_hook(&vcs_post_import_hook);
    // Pre-dispatch and chained-call hooks are diagnostic-only. Do not put them
    // on the gameplay hot path unless their trace was explicitly requested.
    // The post-dispatch hook stays installed because it also performs
    // deferred-I/O handoff and the frozen-clock safety guard.
    psprecomp::set_runtime_pre_dispatch_hook(
        dispatch_collision_diagnostics_enabled() ? &vcs_pre_dispatch_hook : nullptr);
    refresh_vcs_post_dispatch_hook();
    const bool chained_diagnostics = chained_call_collision_diagnostics_enabled();
    psprecomp::set_runtime_pre_chained_call_hook(
        chained_diagnostics ? &vcs_pre_chained_call_hook : nullptr);
    psprecomp::set_runtime_post_chained_call_hook(
        chained_diagnostics ? &vcs_post_chained_call_hook : nullptr);
    reset_frame_capture();
    runtime.register_function(0x08B562D8u, &vcs_sprintf, "vcs_sprintf");
    runtime.register_function(0x088B4FA8u, &vcs_path_hash, "vcs_path_hash");
    runtime.register_function(0x08B1B36Cu, &vcs_load_codec_modules, "vcs_load_codec_modules");
    if (std::getenv("PSPRECOMP_NO_FAST_DEFLATE") == nullptr)
        runtime.register_function(0x08B648B0u, &vcs_raw_deflate_fast, "vcs_raw_deflate_fast");
    psprecomp::hle::install_kernel_hle(runtime);
    psprecomp::hle::install_ctrl_hle(runtime);


    psprecomp::hle::install_ge_hle(runtime);

    psprecomp::hle::install_display_hle(runtime);

    psprecomp::hle::install_utility_hle(runtime);



    psprecomp::hle::install_audio_hle(runtime);

    psprecomp::hle::install_atrac_hle(runtime);



    psprecomp::hle::install_system_hle(runtime);
    runtime.register_hle("sceMpeg", 0x682A619Bu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceMpeg", 0x874624D6u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceMpeg", 0xD7A29F46u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto packets = static_cast<std::int32_t>(ctx.gpr[4]);
            if (packets < 0) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(packets) * (2048u + 104u));
        });

    runtime.register_hle("sceMpeg", 0xC132E22Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // VCS ships with the 1.05+ MPEG module ABI.
            ctx.set_gpr(2, 0x00010000u);
        });

    runtime.register_hle("sceMpeg", 0x37295ED8u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t ring = ctx.gpr[4];
            const auto packets = static_cast<std::int32_t>(ctx.gpr[5]);
            const std::uint32_t data = ctx.gpr[6];
            const std::uint32_t size = ctx.gpr[7];
            // PSP user ABI continues arguments through t0-t3 before the stack.
            const std::uint32_t callback = ctx.gpr[8];
            const std::uint32_t callback_arg = ctx.gpr[9];
            if (packets < 0 || !rt.memory().contains(ring, 48u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint64_t required = static_cast<std::uint64_t>(packets) * (2048u + 104u);
            if (required > size || !rt.memory().contains(data, static_cast<std::size_t>(packets) * 2048u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            rt.memory().zero(ring, 48u);
            rt.memory().store32(ring + 0u, static_cast<std::uint32_t>(packets));
            rt.memory().store32(ring + 4u, 0u);      // packetsRead
            rt.memory().store32(ring + 8u, 0u);      // packetsWritePos
            rt.memory().store32(ring + 12u, 0u);     // packetsAvail
            rt.memory().store32(ring + 16u, 2048u);  // packetSize
            rt.memory().store32(ring + 20u, data);
            rt.memory().store32(ring + 24u, callback);
            rt.memory().store32(ring + 28u, callback_arg);
            rt.memory().store32(ring + 32u, data + static_cast<std::uint32_t>(packets) * 2048u);
            rt.memory().store32(ring + 36u, 0u);     // semaID/padding
            rt.memory().store32(ring + 40u, 0u);     // mpeg pointer, set by Create
            rt.memory().store32(ring + 44u, ctx.gpr[28]);
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xD8C5F121u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t mpeg_out = ctx.gpr[4];
            const std::uint32_t data = ctx.gpr[5];
            const std::uint32_t size = ctx.gpr[6];
            const std::uint32_t ring = ctx.gpr[7];
            const std::uint32_t frame_width = ctx.gpr[8];
            const std::uint32_t mode = ctx.gpr[9];
            const std::uint32_t ddr_top = ctx.gpr[10];
            (void)mode;
            (void)ddr_top;
            if (size < 0x10000u || !rt.memory().contains(mpeg_out, 4u) ||
                !rt.memory().contains(data, size) || !rt.memory().contains(ring, 48u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            const std::uint32_t handle = data + 0x30u;
            if (!rt.memory().contains(handle, 24u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            rt.memory().store32(mpeg_out, handle);
            const std::array<std::uint8_t, 8> magic{'L','I','B','M','P','E','G',0};
            const std::array<std::uint8_t, 4> version{'0','0','1',0};
            rt.memory().copy_in(handle, magic);
            rt.memory().copy_in(handle + 8u, version);
            rt.memory().store32(handle + 12u, 0xFFFFFFFFu);
            rt.memory().store32(handle + 16u, ring);
            rt.memory().store32(handle + 20u, rt.memory().load32(ring + 32u));
            rt.memory().store32(ring + 40u, mpeg_out);
            MpegContextState state{};
            state.handle_address = handle;
            state.ring_address = ring;
            state.video_pixel_mode = 3u;
            mpeg_contexts[mpeg_out] = std::move(state);
            (void)frame_width;
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x21FF80E4u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t mpeg = ctx.gpr[4];
            const std::uint32_t buffer = ctx.gpr[5];
            const std::uint32_t output = ctx.gpr[6];
            const auto state = mpeg_contexts.find(mpeg);
            if (state == mpeg_contexts.end() || !rt.memory().contains(buffer, 2048u) ||
                !rt.memory().contains(output, 4u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            std::array<std::uint8_t, 2048> bytes{};
            rt.memory().copy_out(buffer, bytes);
            ParsedPsmfHeader header{};
            if (!parse_psmf_header(bytes, header)) {
                rt.memory().store32(output, 0u);
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            if (header.stream_offset == 0u || (header.stream_offset & 2047u) != 0u) {
                rt.memory().store32(output, 0u);
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            const auto source = identify_pmf_source(bytes, header);
            if (state->second.source_path != source) {
                close_video_decoder(state->second);
                state->second.source_path = source;
                // A new PSMF is a new timestamp domain even when the game
                // reuses the same SceMpeg work area and ringbuffer.  AU
                // counters are per stream, not lifetime totals.
                state->second.video_au_count = 0u;
                state->second.audio_au_count = 0u;
                for (auto &[id, stream] : state->second.streams) {
                    (void)id;
                    stream.needs_reset = true;
                }
            }
            state->second.header = header;
            state->second.analyzed = true;
            rt.memory().store32(output, header.stream_offset);
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr) {
                std::cerr << "[mpeg] PSMF version=" << std::string(bytes.begin() + 4, bytes.begin() + 8)
                          << " offset=" << header.stream_offset << " size=" << header.stream_size
                          << " dimensions=" << header.width << "x" << header.height
                          << " first_pts=" << header.first_timestamp << " last_pts=" << header.last_timestamp
                          << " source=\"" << state->second.source_path.string() << "\"\n";
            }
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x611E9E11u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t buffer = ctx.gpr[4];
            const std::uint32_t output = ctx.gpr[5];
            if (!rt.memory().contains(buffer, 2048u) || !rt.memory().contains(output, 4u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            std::array<std::uint8_t, 2048> bytes{};
            rt.memory().copy_out(buffer, bytes);
            ParsedPsmfHeader header{};
            if (!parse_psmf_header(bytes, header) || (header.stream_offset & 2047u) != 0u) {
                rt.memory().store32(output, 0u);
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            rt.memory().store32(output, header.stream_size);
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x42560F23u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            if (state == mpeg_contexts.end()) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            const std::uint32_t stream_id = next_mpeg_stream_id++;
            state->second.streams.emplace(stream_id, MpegStreamState{ctx.gpr[5], ctx.gpr[6], true});
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr)
                std::cerr << "[mpeg] register stream id=" << stream_id << " type=" << ctx.gpr[5]
                          << " number=" << ctx.gpr[6] << "\n";
            ctx.set_gpr(2, stream_id);
        });

    runtime.register_hle("sceMpeg", 0x591A4AA2u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            if (state == mpeg_contexts.end() || state->second.streams.erase(ctx.gpr[5]) != 1u) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x707B7629u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            if (state == mpeg_contexts.end()) { ctx.set_gpr(2, 0x806101FEu); return; }
            state->second.analyzed = false;
            state->second.video_au_count = 0u;
            state->second.audio_au_count = 0u;
            close_video_decoder(state->second);
            for (auto &[id, stream] : state->second.streams) stream.needs_reset = true;
            const std::uint32_t ring = state->second.ring_address;
            if (ring != 0u && rt.memory().contains(ring, 48u)) {
                rt.memory().store32(ring + 4u, 0u);
                rt.memory().store32(ring + 8u, 0u);
                rt.memory().store32(ring + 12u, 0u);
            }
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xA780CF7Eu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            if (state == mpeg_contexts.end()) { ctx.set_gpr(2, 0x806101FEu); return; }
            for (std::size_t index = 0; index < state->second.avc_es_buffers.size(); ++index) {
                if (!state->second.avc_es_buffers[index]) {
                    state->second.avc_es_buffers[index] = true;
                    ctx.set_gpr(2, static_cast<std::uint32_t>(index + 1u));
                    return;
                }
            }
            ctx.set_gpr(2, 0u);
        });

    runtime.register_hle("sceMpeg", 0xCEB870B1u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t buffer = ctx.gpr[5];
            if (state == mpeg_contexts.end() || buffer == 0u || buffer > 2u ||
                !state->second.avc_es_buffers[buffer - 1u]) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            state->second.avc_es_buffers[buffer - 1u] = false;
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x167AFD9Eu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t buffer = ctx.gpr[5];
            const std::uint32_t au = ctx.gpr[6];
            if (state == mpeg_contexts.end() || !rt.memory().contains(au, 24u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            const bool avc = buffer >= 1u && buffer <= 2u && state->second.avc_es_buffers[buffer - 1u];
            rt.memory().zero(au, 24u);
            if (!avc) {
                rt.memory().store32(au + 8u, 0xFFFFFFFFu);
                rt.memory().store32(au + 12u, 0xFFFFFFFFu);
            }
            rt.memory().store32(au + 20u, avc ? 2048u : 2112u);
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xF8DCB679u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (mpeg_contexts.find(ctx.gpr[4]) == mpeg_contexts.end() ||
                !rt.memory().contains(ctx.gpr[5], 4u) || !rt.memory().contains(ctx.gpr[6], 4u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            rt.memory().store32(ctx.gpr[5], 2112u);
            rt.memory().store32(ctx.gpr[6], 8192u);
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x800C44DFu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t au = ctx.gpr[5];
            const std::uint32_t output = ctx.gpr[6];
            if (state == mpeg_contexts.end() || !rt.memory().contains(au, 24u) ||
                !rt.memory().contains(output, 8192u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            // The movie's own soundtrack. It lives in private_stream_1 packets
            // that no generic demuxer surfaces, so PmfAudioDecoder walks the
            // container itself; see vcs_media_decoder.cpp. Silence remains the
            // fallback, because a mute intro beats a stalled one.
            MpegContextState &mpeg = state->second;
            // Reopen when the movie changes, not merely when nothing is open: a
            // context is reused across cutscenes, and an exhausted stream from
            // the previous one still reports itself as open.
            if (!mpeg.source_path.empty() &&
                (!mpeg.audio.is_open() || mpeg.audio_source != mpeg.source_path)) {
                mpeg.audio_source = mpeg.source_path;
                (void)mpeg.audio.open(mpeg.source_path);
            }
            std::array<std::uint8_t, 8192u> pcm{};
            const std::size_t decoded = mpeg.audio.is_open()
                ? mpeg.audio.read(pcm) : 0u;
            if (decoded != 0u) {
                rt.memory().copy_in(output, std::span<const std::uint8_t>(pcm.data(), pcm.size()));
            } else {
                rt.memory().zero(output, 8192u);
            }
            const std::uint64_t pts = state->second.header.first_timestamp +
                static_cast<std::uint64_t>(state->second.audio_au_count) * 4180u;
            write_mpeg_timestamp(rt.memory(), au, pts);
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr && state->second.audio_au_count <= 3u)
                std::cerr << "[mpeg] ATRAC decode bytes=" << decoded
                          << " pts=" << pts
                          << " output=" << psprecomp::hex32(output) << "\n";
            (void)delay_current_thread(rt, ctx, 3000u, 0u);
        });

    runtime.register_hle("sceMpeg", 0x0E3C2E9Du,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t au = ctx.gpr[5];
            std::uint32_t frame_width = ctx.gpr[6];
            const std::uint32_t buffer_pointer = ctx.gpr[7];
            const std::uint32_t status_pointer = ctx.gpr[8];
            if (state == mpeg_contexts.end() || !state->second.analyzed ||
                !rt.memory().contains(au, 24u) || !rt.memory().contains(buffer_pointer, 4u) ||
                !rt.memory().contains(status_pointer, 4u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            if (frame_width == 0u) frame_width = state->second.header.width;
            if (frame_width < state->second.header.width) { ctx.set_gpr(2, 0x806201FEu); return; }
            const std::uint32_t destination = rt.memory().load32(buffer_pointer);
            const std::size_t frame_bytes = static_cast<std::size_t>(state->second.header.width) *
                state->second.header.height * 4u;
            const std::size_t destination_bytes = static_cast<std::size_t>(frame_width) *
                state->second.header.height * 4u;
            if (destination == 0u || !rt.memory().contains(destination, destination_bytes)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            std::vector<std::uint8_t> frame(frame_bytes);
            if (!read_video_frame(state->second, frame)) {
                rt.memory().store32(status_pointer, 0u);
                ctx.set_gpr(2, 0x80628002u);
                return;
            }
            movie_output_buffers.insert(normalize_ram_address(destination));
            const std::size_t source_stride = static_cast<std::size_t>(state->second.header.width) * 4u;
            const std::size_t destination_stride = static_cast<std::size_t>(frame_width) * 4u;
            for (std::uint32_t y = 0u; y < state->second.header.height; ++y) {
                rt.memory().copy_in(destination + static_cast<std::uint32_t>(y * destination_stride),
                    std::span<const std::uint8_t>(frame.data() + y * source_stride, source_stride));
            }
            rt.memory().store32(status_pointer, 1u);

            const std::uint32_t total_frames = std::max<std::uint32_t>(1u, static_cast<std::uint32_t>(
                (state->second.header.last_timestamp - state->second.header.first_timestamp) / 3003u));
            const std::uint32_t total_packets = (state->second.header.stream_size + 2047u) / 2048u;
            const std::uint32_t target_consumed = static_cast<std::uint32_t>(std::min<std::uint64_t>(
                total_packets, static_cast<std::uint64_t>(state->second.decoded_video_frames) * total_packets / total_frames));
            const std::uint32_t consume = target_consumed - state->second.consumed_video_packets;
            state->second.consumed_video_packets = target_consumed;
            const std::uint32_t ring = state->second.ring_address;
            if (consume != 0u && rt.memory().contains(ring, 48u)) {
                const std::uint32_t used = rt.memory().load32(ring + 12u);
                rt.memory().store32(ring + 12u, used > consume ? used - consume : 0u);
            }
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr &&
                (state->second.decoded_video_frames <= 3u || state->second.decoded_video_frames % 30u == 0u)) {
                std::cerr << "[mpeg] decoded frame=" << state->second.decoded_video_frames
                          << " destination=" << psprecomp::hex32(destination)
                          << " stride=" << frame_width << " consume=" << consume << "\n";
            }
            const std::uint32_t delay = state->second.decoded_video_frames <= 1u ? 3600u : 5400u;
            (void)delay_current_thread(rt, ctx, delay, 0u);
        });

    runtime.register_hle("sceMpeg", 0x740FCCD1u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            // sceMpegAvcDecodeStop(mpeg, frameWidth, bufferAddr, statusAddr).
            // Our sequential decoder does not retain a delayed final frame, so
            // the correct drain result is a zero status without modifying the
            // caller's framebuffer pointer.
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t buffer_pointer = ctx.gpr[6];
            const std::uint32_t status_pointer = ctx.gpr[7];
            if (state == mpeg_contexts.end()) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            if (!rt.memory().contains(buffer_pointer, 4u) ||
                !rt.memory().contains(status_pointer, 4u)) {
                ctx.set_gpr(2, 0x80610103u);
                return;
            }
            rt.memory().store32(status_pointer, 0u);
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr)
                std::cerr << "[mpeg] AVC decode stop: no pending frame\n";
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xE1CE83A7u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t au = ctx.gpr[6];
            const std::uint32_t attributes = ctx.gpr[7];
            if (state == mpeg_contexts.end()) { ctx.set_gpr(2, 0x806101FEu); return; }
            const auto stream = state->second.streams.find(ctx.gpr[5]);
            if (stream == state->second.streams.end() ||
                (stream->second.type != 1u && stream->second.type != 15u) ||
                !rt.memory().contains(au, 24u)) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            const std::uint32_t ring = state->second.ring_address;
            if (!rt.memory().contains(ring, 48u) || rt.memory().load32(ring + 12u) == 0u) {
                write_mpeg_timestamp(rt.memory(), au, 0u);
                write_mpeg_timestamp(rt.memory(), au + 8u, 0u);
                ctx.set_gpr(2, 0x80618001u);
                return;
            }
            const std::uint64_t pts = state->second.header.first_timestamp +
                static_cast<std::uint64_t>(state->second.audio_au_count) * 4180u;
            write_mpeg_timestamp(rt.memory(), au, pts);
            write_mpeg_timestamp(rt.memory(), au + 8u, pts);
            rt.memory().store32(au + 16u, stream->second.number);
            rt.memory().store32(au + 20u, 2112u);
            if (attributes != 0u && rt.memory().contains(attributes, 4u)) rt.memory().store32(attributes, 0u);
            stream->second.needs_reset = false;
            ++state->second.audio_au_count;
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr)
                std::cerr << "[mpeg] ATRAC AU stream=" << ctx.gpr[5] << " pts=" << pts
                          << " used_packets=" << rt.memory().load32(ring + 12u) << "\n";
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xFE246728u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto state = mpeg_contexts.find(ctx.gpr[4]);
            const std::uint32_t au = ctx.gpr[6];
            const std::uint32_t attributes = ctx.gpr[7];
            if (state == mpeg_contexts.end()) { ctx.set_gpr(2, 0x806101FEu); return; }
            const auto stream = state->second.streams.find(ctx.gpr[5]);
            if (stream == state->second.streams.end() ||
                stream->second.type != 0u || !rt.memory().contains(au, 24u)) {
                ctx.set_gpr(2, 0x806101FEu);
                return;
            }
            const std::uint32_t ring = state->second.ring_address;
            if (!rt.memory().contains(ring, 48u) || rt.memory().load32(ring + 12u) == 0u) {
                write_mpeg_timestamp(rt.memory(), au, 0u);
                write_mpeg_timestamp(rt.memory(), au + 8u, 0u);
                ctx.set_gpr(2, 0x80618001u);
                return;
            }
            const std::uint64_t pts = state->second.header.first_timestamp +
                static_cast<std::uint64_t>(state->second.video_au_count) * 3003u;
            const std::uint64_t dts = pts >= 3003u ? pts - 3003u : 0u;
            write_mpeg_timestamp(rt.memory(), au, pts);
            write_mpeg_timestamp(rt.memory(), au + 8u, dts);
            rt.memory().store32(au + 16u, stream->second.number);
            rt.memory().store32(au + 20u, 2048u);
            if (attributes != 0u && rt.memory().contains(attributes, 4u)) rt.memory().store32(attributes, 1u);
            stream->second.needs_reset = false;
            ++state->second.video_au_count;
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr)
                std::cerr << "[mpeg] AVC AU stream=" << ctx.gpr[5] << " pts=" << pts << " dts=" << dts
                          << " used_packets=" << rt.memory().load32(ring + 12u) << "\n";
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0xB240A59Eu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t ring = ctx.gpr[4];
            std::int32_t requested = static_cast<std::int32_t>(ctx.gpr[5]);
            const std::int32_t caller_available = static_cast<std::int32_t>(ctx.gpr[6]);
            if (!rt.memory().contains(ring, 48u)) { ctx.set_gpr(2, 0x80610103u); return; }
            const std::int32_t packets = static_cast<std::int32_t>(rt.memory().load32(ring));
            const std::int32_t used = static_cast<std::int32_t>(rt.memory().load32(ring + 12u));
            const std::int32_t write_position = static_cast<std::int32_t>(rt.memory().load32(ring + 8u));
            const std::uint32_t data = rt.memory().load32(ring + 20u);
            const std::uint32_t callback = rt.memory().load32(ring + 24u);
            const std::uint32_t callback_argument = rt.memory().load32(ring + 28u);
            if (packets <= 0 || callback == 0u) { ctx.set_gpr(2, 0x806101FEu); return; }
            requested = std::min({requested, caller_available, std::max(0, packets - used)});
            if (requested <= 0) { set_success(ctx); return; }
            const std::int32_t write_offset = write_position % packets;
            const std::int32_t desired = std::min(requested, packets - write_offset);

            psprecomp::AllegrexContext resume = ctx;
            resume.pc = ctx.gpr[31];
            resume.set_gpr(2, 0u);
            auto &frames = async_return_frames[thread_table.current_uid];
            if (!frames.empty()) {
                rt.stop("Nested MPEG ringbuffer callback on one PSP thread");
                return;
            }
            frames.push_back(AsyncReturnFrame{AsyncReturnKind::MpegRingbuffer, resume, ring,
                                               requested - desired, desired, 0});
            ctx.set_gpr(4, data + static_cast<std::uint32_t>(write_offset) * 2048u);
            ctx.set_gpr(5, static_cast<std::uint32_t>(desired));
            ctx.set_gpr(6, callback_argument);
            ctx.set_gpr(31, 0x00000004u);
            ctx.pc = callback;
            if (std::getenv("PSPRECOMP_MPEG_DIAG") != nullptr) {
                std::cerr << "[mpeg] ring put ring=" << psprecomp::hex32(ring)
                          << " callback=" << psprecomp::hex32(callback)
                          << " data=" << psprecomp::hex32(ctx.gpr[4])
                          << " desired=" << desired << " remaining=" << requested - desired << "\n";
            }
        });

    runtime.register_hle("sceMpeg", 0xB5F6DC87u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t ring = ctx.gpr[4];
            if (!rt.memory().contains(ring, 48u)) { ctx.set_gpr(2, 0x800200D3u); return; }
            const std::int32_t packets = static_cast<std::int32_t>(rt.memory().load32(ring));
            const std::int32_t used = static_cast<std::int32_t>(rt.memory().load32(ring + 12u));
            ctx.set_gpr(2, static_cast<std::uint32_t>(std::max(0, packets - used)));
        });

    runtime.register_hle("sceMpeg", 0x606A4649u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            // sceMpegDelete receives the address of the SceMpeg handle.  The
            // firmware tears down decoder-side state while leaving ownership
            // of the caller-provided work buffer with the game.
            const std::uint32_t mpeg_out = ctx.gpr[4];
            if (mpeg_out == 0u || !rt.memory().contains(mpeg_out, 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint32_t handle = rt.memory().load32(mpeg_out);
            if (handle != 0u && rt.memory().contains(handle, 24u)) {
                const std::uint32_t ring = rt.memory().load32(handle + 16u);
                if (ring != 0u && rt.memory().contains(ring, 48u)) {
                    rt.memory().store32(ring + 40u, 0u);
                }
            }
            if (auto state = mpeg_contexts.find(mpeg_out); state != mpeg_contexts.end())
                close_video_decoder(state->second);
            mpeg_contexts.erase(mpeg_out);
            set_success(ctx);
        });

    runtime.register_hle("sceMpeg", 0x13407F13u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t ring = ctx.gpr[4];
            if (ring != 0u && rt.memory().contains(ring, 48u)) {
                rt.memory().store32(ring + 12u, 0u);
                rt.memory().store32(ring + 40u, 0u);
            }
            set_success(ctx);
        });

    psprecomp::hle::install_io_hle(runtime);
}


namespace {


} // namespace

void report_disc_read_stats() {
    const std::uint64_t total =
        disc_read_stats.bytes_from_files + disc_read_stats.bytes_zero_filled;
    if (total == 0u) return;
    std::cerr << "[disc-read-summary] from_files=" << disc_read_stats.bytes_from_files
              << " zero_filled=" << disc_read_stats.bytes_zero_filled
              << " zero_fill_events=" << disc_read_stats.zero_fill_events
              << " short_reads=" << disc_read_stats.short_reads
              << " open_failures=" << disc_read_stats.open_failures
              << " zero_percent="
              << (disc_read_stats.bytes_zero_filled * 100.0 / static_cast<double>(total)) << "\n";
}

void report_present_stats() {
    // Runtime::run() has returned but the Runtime object is still alive here.
    // Join the Stage 45.7 GE consumer now so no global worker can retain a
    // dangling Runtime pointer during process/static destruction.
    const bool async_was_running = ge_async_running();
    std::uint64_t async_submitted = 0u;
    std::uint64_t async_completed = 0u;
    std::uint64_t async_wait_calls = 0u;
    std::uint64_t async_wait_us = 0u;
    if (async_was_running) {
        {
            std::lock_guard lock(ge_async.mutex);
            async_submitted = ge_async.submitted;
            async_completed = ge_async.completed;
            async_wait_calls = ge_async.wait_calls;
            async_wait_us = static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::microseconds>(ge_async.wait_time).count());
        }
        ge_async_stop_worker();
    }
    const GeGpuBackendReport gpu = ge_gpu_backend_report();
    std::cerr << "[present-census] swapchain=" << swapchain_presents
              << " software=" << software_presents
              << " software_after_gpu=" << software_presents_after_gpu
              << " display_fb_sampled_draws=" << gpu.display_framebuffer_sampled_draws << "\n";
    if (async_was_running) {
        std::cerr << "[ge-async-summary] submitted=" << async_submitted
                  << " completed=" << async_completed
                  << " wait_calls=" << async_wait_calls
                  << " wait_us=" << async_wait_us << "\n";
    }
}

void install_starvation_preemption() {
    const std::uint64_t interval = parse_environment_u64("PSPRECOMP_TIME_TICK_DISPATCHES", 256u);
    execution_clock_dispatch_interval = interval;
    frozen_clock_guard_limit = parse_environment_u64(
        "PSPRECOMP_FROZEN_CLOCK_GUARD_DISPATCHES", 5'000'000u);
    frozen_clock_guard_dispatches = 0u;
    frozen_clock_guard_vblank = display_vblank_index;
    psprecomp::hle::install_execution_clock(interval);
    std::cerr << "[scheduler-clock] dispatch_interval=" << interval
              << " tick_us=" << (interval == 0u ? 0u : psprecomp::hle::execution_clock_tick_microseconds())
              << " frozen_guard=" << (interval == 0u ? frozen_clock_guard_limit : 0u)
              << "\n";
    if (interval == 0u) {
        std::cerr << "[scheduler-clock] warning: execution-driven PSP time is disabled; "
                     "use this only for isolated ordering diagnostics, not a full frontend/world run.\n";
    }
}

void install_display_heartbeat() {
    if (!display_window_enabled()) return;
    psprecomp::set_runtime_heartbeat_hook(
        [](std::uint64_t dispatch, std::uint32_t pc) {
            std::ostringstream status;
            status << "vblank " << display_vblank_index << " | dispatch "
                   << (dispatch / 1000000u) << "M | pc " << psprecomp::hex32(pc);
            display_window_set_status(status.str().c_str());
        },
        4'000'000u);
}

bool run_profile_self_tests(std::string &error) {
    const auto require = [](bool condition, const char *message) {
        if (!condition) throw std::runtime_error(message);
    };
    const auto reset = [] {
        thread_table = ThreadTable{};
        pending_guest_callbacks.clear();
        async_return_frames.clear();
        virtual_time_us = 0u;
        psprecomp::set_runtime_thread_identity(-1, "none");
    };

    try {
        {
            const RealtimeSpeedSample realtime =
                calculate_realtime_speed_sample(1'000'000u, 1'000'000u, 60u);
            require(std::abs(realtime.emulation_speed_percent - 100.0) < 0.001,
                    "real-time speed diagnostic misreported a 1:1 clock");
            require(std::abs(realtime.guest_us_per_vblank - 16'666.6666667) < 0.01,
                    "real-time speed diagnostic miscomputed guest time per vblank");
            const RealtimeSpeedSample half_speed =
                calculate_realtime_speed_sample(2'000'000u, 1'000'000u, 60u);
            require(std::abs(half_speed.emulation_speed_percent - 50.0) < 0.001,
                    "real-time speed diagnostic did not detect half-speed execution");
            require(std::abs(half_speed.host_us_per_vblank - 33'333.3333333) < 0.01,
                    "real-time speed diagnostic miscomputed host frame time");
        }
        require(estimate_vcs_deflate_guest_work(2'267'436u, 6'300'880u) == 21'431u,
                "VCS deflate timing calibration for the large bootstrap stream changed");
        require(estimate_vcs_deflate_guest_work(38'278u, 132'636u) == 330u,
                "VCS deflate timing calibration for the small stream changed");
        require(estimate_vcs_deflate_guest_work(1u, 1u) >= 1u,
                "VCS deflate timing estimator returned zero work");

        // Stage 45.7 asynchronous GE smoke test.  Run this branch when the test
        // process opts into PSPRECOMP_GE_ASYNC=1: enqueue must return before the
        // worker consumes the list, and the explicit wait must observe FINISH.
        if (ge_async_enabled()) {
            ge_async_stop_worker();
            ge_list_table = GeListTable{};
            ge_callback_table = GeCallbackTable{};
            ge_state = GeState{};
            reset_ge_transform_state(ge_state.transform);
            {
                std::lock_guard lock(ge_async.mutex);
                ge_async.stop_requested = false;
                ge_async.fatal.store(false, std::memory_order_release);
                ge_async.fatal_reason.clear();
            }
            psprecomp::Runtime ge_runtime;
            constexpr std::uint32_t list_pc = 0x08810000u;
            ge_runtime.memory().store32(list_pc + 0u, (kGeCommandFinish << 24u) | 0x1234u);
            ge_runtime.memory().store32(list_pc + 4u, kGeCommandEnd << 24u);
            psprecomp::AllegrexContext ge_ctx{};
            ge_ctx.gpr[4] = list_pc;
            ge_ctx.gpr[5] = 0u;
            ge_ctx.gpr[6] = 0xFFFFFFFFu;
            ge_ctx.gpr[7] = 0u;
            ge_ctx.gpr[31] = 0x08820000u;
            enqueue_ge_display_list(ge_runtime, ge_ctx, false);
            const std::uint32_t id = ge_ctx.gpr[2];
            require(id != 0u && (id & 0xFF000000u) == (kGeListIdMagic & 0xFF000000u),
                    "async GE enqueue did not return a list id");
            require(ge_async_wait_idle(ge_runtime), "async GE worker reported a fatal error");
            {
                std::lock_guard lock(ge_async.mutex);
                const auto found = ge_list_table.lists.find(id);
                require(found != ge_list_table.lists.end() &&
                            found->second.state == GeListState::Completed &&
                            found->second.callback_token == 0x1234u,
                        "async GE worker did not complete FINISH/END in order");
            }
            ge_async_stop_worker();
            ge_list_table = GeListTable{};
        }
        {
            std::array<std::uint8_t, 2048> psmf{};
            psmf[0] = 'P'; psmf[1] = 'S'; psmf[2] = 'M'; psmf[3] = 'F';
            psmf[4] = '0'; psmf[5] = '0'; psmf[6] = '1'; psmf[7] = '4';
            psmf[8] = 0; psmf[9] = 0; psmf[10] = 8; psmf[11] = 0;
            psmf[12] = 0; psmf[13] = 8; psmf[14] = 40; psmf[15] = 0;
            psmf[142] = 30; psmf[143] = 17;
            ParsedPsmfHeader parsed{};
            require(parse_psmf_header(psmf, parsed), "valid PSMF header was rejected");
            require(parsed.stream_offset == 2048u && parsed.stream_size == 534528u,
                    "PSMF big-endian stream fields were decoded incorrectly");
            require(parsed.width == 480u && parsed.height == 272u,
                    "PSMF dimensions were decoded incorrectly");
        }

        // A blocking audio submission must be scheduled where the hardware
        // would really start playing it: back to back with the previous buffer,
        // regardless of how much guest time the caller burned decoding.  The
        // old "now plus one buffer" pacing let sceAtracDecodeData's 2300 us push
        // the stream ~5% ahead of the mix, which the host sink then papered over
        // with a timeline resync -- an audible click -- on every submission.
        {
            const std::uint64_t previous_time = virtual_time_us;
            AudioChannelState channel{};
            channel.reserved = true;
            channel.sample_count = 2048u;

            const auto elapsed_us = [](std::uint64_t frames) {
                return (frames * 1'000'000ull) / 44'100ull;
            };
            virtual_time_us = 1'000'000u;
            const std::uint64_t first = audio_queue_buffer(channel, channel.sample_count);
            require(first == 1'000'000u, "the first audio buffer must start immediately");

            // The queue was empty, so the first submission did not block: the
            // guest spends 2300 us decoding and submits again well before the
            // buffer it just queued has finished playing.
            virtual_time_us = first + 2'300u;
            const std::uint64_t second = audio_queue_buffer(channel, channel.sample_count);
            require(second == first + elapsed_us(2048u),
                    "audio buffers were not scheduled contiguously");

            // From here the guest is paced by the hardware: it wakes when the
            // previous buffer drains, decodes, and submits again.
            virtual_time_us = second + 2'300u;
            const std::uint64_t third = audio_queue_buffer(channel, channel.sample_count);
            require(third == first + elapsed_us(4096u),
                    "guest decode time leaked into the audio timeline");
            require(channel.queued_frames == 6144u,
                    "the audio queue lost track of the submitted frame count");

            // A gap larger than the queue really is a drained channel and has to
            // re-anchor rather than schedule into the past.
            virtual_time_us = channel.busy_until_us + 5'000'000u;
            require(audio_queue_buffer(channel, channel.sample_count) == virtual_time_us,
                    "a drained audio channel did not re-anchor to the current time");
            require(channel.queued_frames == 2048u,
                    "re-anchoring an audio channel did not restart its frame count");

            virtual_time_us = previous_time;
        }

        // A voice configured through __sceSasSetADSR alone -- rates only, no
        // call to __sceSasSetADSRmode -- must still retire when the game keys
        // it off.  VCS does exactly this for the vehicle engine, and the old
        // all-zero mode defaults made "release" walk the envelope upward, so
        // the engine kept sounding under the pause menu.
        {
            SasVoiceState voice{};
            require(voice.adsr_modes[0] == 0, "default attack curve must rise");
            require(voice.adsr_modes[1] == 1 && voice.adsr_modes[2] == 1 &&
                    voice.adsr_modes[3] == 1,
                    "default decay/sustain/release curves must fall");

            voice.type = SasVoiceType::Vag;
            voice.adsr_configured = true;
            voice.playing = true;
            voice.on = false;
            voice.envelope_height = kSasEnvelopeMaximum;
            voice.envelope_phase = SasEnvelopePhase::Release;
            voice.adsr_rates[3] = 0x10000000;  // the rate VCS actually sets

            std::uint32_t steps = 0u;
            while (voice.playing && steps < 64u) {
                sas_step_envelope(voice);
                ++steps;
            }
            require(!voice.playing, "a keyed-off voice never released its envelope");
            require(voice.envelope_height == 0u,
                    "a released voice was retired with a non-zero envelope");
        }

        // A GE context supplied to sceGeListEnQueue is a real serialized PSP
        // context, not merely a command-memory snapshot. It must include matrix
        // DATA words and the global renderer state must be restored after END.
        {
            const GeState previous_ge_state = ge_state;
            psprecomp::Runtime ge_runtime;
            constexpr std::uint32_t context_address = 0x08830000u;
            ge_state = GeState{};
            ++ge_draw_state_revision;
            ++ge_lighting_state_revision;
            reset_ge_transform_state(ge_state.transform);
            ge_state.commands[0x42u] = ge_float24_command(0x42u, 240.0f);
            ge_state.commands[0x43u] = ge_float24_command(0x43u, 136.0f);
            ge_state.offset_address = 0x00123000u;
            ge_state.vertex_address = 0x08901000u;
            ge_state.index_address = 0x08902000u;
            ge_state.bounding_box_result = true;
            ge_state.transform.world[9] = 123.5f;
            ge_state.transform.view[10] = -7.25f;
            ge_state.transform.projection[15] = 0.5f;
            ge_state.transform.bones[95] = 3.75f;
            ge_state.transform.bone_cursor = 101u;
            ge_state.transform.world_cursor = 14u;

            GeListRecord record{};
            record.context_address = context_address;
            save_ge_list_context(ge_runtime, record);
            require(record.has_saved_context, "GE context was not captured for a list");
            require(ge_runtime.memory().load32(context_address + 5u * 4u) == 0x08901000u &&
                        ge_runtime.memory().load32(context_address + 6u * 4u) == 0x08902000u &&
                        ge_runtime.memory().load32(context_address + 7u * 4u) == 0x00123000u,
                    "GE context header did not serialize stream addresses");

            bool found_world_translation = false;
            for (std::uint32_t word = 17u; word + 1u < 512u; ++word) {
                const std::uint32_t value = ge_runtime.memory().load32(context_address + word * 4u);
                const std::uint32_t next = ge_runtime.memory().load32(context_address + (word + 1u) * 4u);
                if ((value >> 24u) == 0x3Au && (next >> 24u) == 0x3Bu &&
                    (next & 0x00FFFFFFu) == (ge_float24_command(0x3Bu, ge_state.transform.world[0]) & 0x00FFFFFFu)) {
                    for (std::uint32_t i = 1u; i < 12u; ++i) {
                        const std::uint32_t data = ge_runtime.memory().load32(context_address + (word + 1u + i) * 4u);
                        if (i == 9u && (data & 0x00FFFFFFu) ==
                                (ge_float24_command(0x3Bu, 123.5f) & 0x00FFFFFFu))
                            found_world_translation = true;
                    }
                    break;
                }
            }
            require(found_world_translation, "GE context omitted expanded world matrix DATA commands");

            ge_state = GeState{};
            ++ge_draw_state_revision;
            ++ge_lighting_state_revision;
            reset_ge_transform_state(ge_state.transform);
            restore_ge_list_context(record);
            require(ge_state.commands[0x42u] == ge_float24_command(0x42u, 240.0f) &&
                        ge_state.offset_address == 0x00123000u &&
                        ge_state.vertex_address == 0x08901000u && ge_state.index_address == 0x08902000u &&
                        ge_state.bounding_box_result && ge_state.transform.world[9] == 123.5f &&
                        ge_state.transform.view[10] == -7.25f && ge_state.transform.projection[15] == 0.5f &&
                        ge_state.transform.bones[95] == 3.75f && ge_state.transform.bone_cursor == 101u &&
                        ge_state.transform.world_cursor == 14u,
                    "GE list completion did not restore the complete saved context");
            ge_state = previous_ge_state;
        }

        // Multiple delayed threads can expire on one virtual-time jump. Their
        // order must be independent of unordered_map bucket layout.
        reset();
        for (const auto [uid, sequence, pc] : std::array<std::tuple<std::int32_t, std::uint64_t, std::uint32_t>, 3>{
                 std::tuple{30, 3u, 0x3000u}, std::tuple{10, 1u, 0x1000u}, std::tuple{20, 2u, 0x2000u}}) {
            ThreadRecord record{};
            record.name = "delay-" + std::to_string(uid);
            record.priority = 32u;
            record.state = ThreadState::Delayed;
            record.delay_until_us = 100u;
            record.delay_sequence = sequence;
            record.suspended_context.pc = pc;
            thread_table.threads.emplace(uid, std::move(record));
        }
        virtual_time_us = 100u;
        promote_expired_delays();
        require(thread_table.continuations.size() == 3u, "expired delays were not promoted");
        require(thread_table.continuations[0].uid == 10 && thread_table.continuations[1].uid == 20 &&
                    thread_table.continuations[2].uid == 30,
                "expired delay order is not deterministic FIFO");

        // Higher PSP priority wins; equal priorities retain ready-queue FIFO.
        reset();
        for (const auto [uid, priority] : std::array<std::pair<std::int32_t, std::uint32_t>, 3>{
                 std::pair{1, 40u}, std::pair{2, 20u}, std::pair{3, 20u}}) {
            ThreadRecord record{};
            record.name = "ready-" + std::to_string(uid);
            record.priority = priority;
            record.state = ThreadState::Ready;
            thread_table.threads.emplace(uid, std::move(record));
            psprecomp::AllegrexContext context{};
            context.pc = 0x8000u + static_cast<std::uint32_t>(uid) * 4u;
            enqueue_continuation(uid, context);
        }
        psprecomp::AllegrexContext selected{};
        require(activate_next_thread(selected, "self-test"), "ready queue did not select a thread");
        require(thread_table.current_uid == 2, "higher-priority ready thread was not selected");
        require(activate_next_thread(selected, "self-test"), "ready queue lost equal-priority peer");
        require(thread_table.current_uid == 3, "equal-priority FIFO order was not preserved");

        // Waking a higher-priority thread is an immediate kernel scheduling
        // point. The caller must remain ready with its post-HLE return state.
        reset();
        ThreadRecord low{};
        low.name = "low";
        low.priority = 40u;
        low.state = ThreadState::Running;
        thread_table.threads.emplace(1, std::move(low));
        ThreadRecord high{};
        high.name = "high";
        high.priority = 16u;
        high.state = ThreadState::Sleeping;
        high.suspended_context.pc = 0x08809000u;
        thread_table.threads.emplace(2, std::move(high));
        thread_table.current_uid = 1;
        psprecomp::AllegrexContext wake_context{};
        wake_context.pc = 0x08B70000u;
        wake_context.set_gpr(2, 0u);
        wake_context.set_gpr(31, 0x08808000u);
        require(wake_thread(2) == 0u, "higher-priority sleeping thread did not wake");
        require(preempt_if_higher_priority(wake_context, "self-test-wakeup"),
                "higher-priority wakeup did not preempt the caller");
        require(thread_table.current_uid == 2 && wake_context.pc == 0x08809000u,
                "woken higher-priority thread did not receive the CPU");
        const auto saved_caller = std::find_if(
            thread_table.continuations.begin(), thread_table.continuations.end(),
            [](const ThreadContinuation &item) { return item.uid == 1; });
        require(saved_caller != thread_table.continuations.end() && saved_caller->context.pc == 0x08808000u &&
                    saved_caller->context.gpr[2] == 0u,
                "preempted caller did not preserve its post-HLE return context");

        // Host-backed UMD reads complete much faster than a physical PSP drive.
        // The worker must remain blocked until the request submitter completes
        // its translated return dispatch and stores the active request pointer.
        // This barrier must work even when execution-driven virtual time is off.
        {
            psprecomp::Runtime io_runtime;
            install_profile(io_runtime, 0x08E00000u);
            thread_table = ThreadTable{};
            deferred_io_resumes.clear();
            pending_guest_callbacks.clear();
            async_return_frames.clear();
            virtual_time_us = 0u;

            constexpr std::int32_t submitter_uid = 3;
            constexpr std::int32_t worker_uid = 5;
            constexpr std::int32_t fd = 42;
            constexpr std::uint32_t destination = 0x08824000u;
            constexpr std::uint32_t byte_count = 16u;
            constexpr std::uint32_t submitter_pc = 0x08955E7Cu;
            constexpr std::uint32_t submission_commit_pc = 0x08955EA4u;
            constexpr std::uint32_t submitter_next_pc = 0x08955E90u;
            constexpr std::uint32_t worker_return_pc = 0x08826000u;
            constexpr std::uint32_t active_slot = 0x08827000u;
            constexpr std::uint32_t event_pattern_slot = 0x08827004u;
            constexpr std::uint32_t request_pointer = 0x08828000u;

            ThreadRecord submitter{};
            submitter.name = "threadmain";
            submitter.priority = 56u;
            submitter.state = ThreadState::Ready;
            thread_table.threads.emplace(submitter_uid, std::move(submitter));
            psprecomp::AllegrexContext submitter_context{};
            submitter_context.pc = submitter_pc;
            enqueue_continuation(submitter_uid, submitter_context);

            ThreadRecord worker{};
            worker.name = "UmdStreamThread";
            worker.priority = 32u;
            worker.state = ThreadState::Running;
            thread_table.threads.emplace(worker_uid, std::move(worker));
            thread_table.current_uid = worker_uid;
            psprecomp::set_runtime_thread_identity(worker_uid, "UmdStreamThread");

            // A virtual-disc gap is defined as zero-filled readable media, so
            // this exercises the exact HLE path without a temporary host file.
            file_table.virtual_disc_handles.emplace(fd, VirtualDiscHandle{0u, byte_count, 0u});
            for (std::uint32_t offset = 0u; offset < byte_count; offset += 4u)
                io_runtime.memory().store32(destination + offset, 0xA5A5A5A5u);
            io_runtime.memory().store32(active_slot, 0u);
            io_runtime.memory().store32(event_pattern_slot, 0x1u);

            psprecomp::AllegrexContext read_context{};
            read_context.set_gpr(4u, static_cast<std::uint32_t>(fd));
            read_context.set_gpr(5u, destination);
            read_context.set_gpr(6u, byte_count);
            read_context.set_gpr(31u, worker_return_pc);
            io_runtime.invoke_import("IoFileMgrForUser", 0x6A638D83u, read_context);

            require(!io_runtime.stopped(), "virtual UMD read stopped the runtime");
            require(thread_table.current_uid == submitter_uid && read_context.pc == submitter_pc,
                    "virtual UMD read did not hand execution back to the request submitter");
            const auto deferred_worker = thread_table.threads.find(worker_uid);
            require(deferred_worker != thread_table.threads.end() &&
                        deferred_worker->second.state == ThreadState::IoDeferred &&
                        deferred_worker->second.suspended_context.pc == worker_return_pc &&
                        deferred_worker->second.suspended_context.gpr[2] == byte_count,
                    "virtual UMD read did not preserve the worker return state behind the dispatch barrier");
            require(deferred_io_resumes.contains(worker_uid) &&
                        deferred_io_resumes.at(worker_uid).handoff_uid == submitter_uid &&
                        deferred_io_resumes.at(worker_uid).handoff_pc == submitter_pc &&
                        deferred_io_resumes.at(worker_uid).release_pc == submission_commit_pc,
                    "virtual UMD read armed the barrier for the wrong atomic submission boundary");
            require(io_handoff_release_pc(0x08956258u) == 0x08956280u,
                    "batched world-stream submission did not map to its atomic commit boundary");
            require(io_handoff_release_pc(0x08801234u) == 0x08801234u,
                    "non-VCS I/O handoff unexpectedly changed its release PC");
            for (std::uint32_t offset = 0u; offset < byte_count; offset += 4u)
                require(io_runtime.memory().load32(destination + offset) == 0u,
                        "virtual UMD gap read did not copy deterministic zero data");

            // Returning from the read import and unrelated dispatches must not
            // release the worker before the active request store executes.
            vcs_post_dispatch_hook(io_runtime, read_context, 0x08B70000u, worker_uid);
            vcs_post_dispatch_hook(io_runtime, read_context, submitter_pc + 4u, submitter_uid);
            require(thread_table.current_uid == submitter_uid &&
                        thread_table.threads.at(worker_uid).state == ThreadState::IoDeferred &&
                        io_runtime.memory().load32(active_slot) == 0u,
                    "UMD worker escaped its barrier before the submitter return dispatch");

            // Model the translated 0x08955E7C unit.  Storing manager+0x274 is
            // not yet a safe release point: VCS still clears WorldStreamEventFlag
            // and would erase a completion bit published by the fast host worker.
            io_runtime.memory().store32(active_slot, request_pointer);
            read_context.pc = submitter_next_pc;
            vcs_post_dispatch_hook(io_runtime, read_context, submitter_pc, submitter_uid);
            require(deferred_io_resumes.contains(worker_uid) &&
                        thread_table.current_uid == submitter_uid &&
                        thread_table.threads.at(worker_uid).state == ThreadState::IoDeferred,
                    "UMD worker resumed before the world-stream event reset/arm sequence");

            // Model 0x08955E8C..0x08955EA4: clear stale completion state and
            // publish submission bit 0x4.  Only after the translated 0x08955EA4
            // dispatch has completed may the higher-priority worker run.
            io_runtime.memory().store32(event_pattern_slot, 0u);
            io_runtime.memory().store32(event_pattern_slot,
                                        io_runtime.memory().load32(event_pattern_slot) | 0x4u);
            vcs_post_dispatch_hook(io_runtime, read_context, submission_commit_pc, submitter_uid);
            require(virtual_time_us == 0u,
                    "UMD dispatch barrier unexpectedly depended on virtual-time advancement");
            require(!deferred_io_resumes.contains(worker_uid),
                    "completed UMD atomic-submission barrier was not removed");
            require(thread_table.current_uid == worker_uid && read_context.pc == worker_return_pc &&
                        read_context.gpr[2] == byte_count,
                    "UMD worker did not resume with its preserved read result after submission commit");
            require(io_runtime.memory().load32(active_slot) == request_pointer,
                    "UMD worker resumed before the submitter's active request store was visible");
            require(io_runtime.memory().load32(event_pattern_slot) == 0x4u,
                    "UMD worker resumed before WorldStreamEventFlag submission bit was published");
            io_runtime.memory().store32(event_pattern_slot,
                                        io_runtime.memory().load32(event_pattern_slot) | 0x1u);
            require(io_runtime.memory().load32(event_pattern_slot) == 0x5u,
                    "worker completion bit was lost after the atomic submission boundary");
            file_table = FileTable{};
        }

        // A tiny first read can complete before the world-stream allocator has
        // unwound to the request-pointer store.  Reproduce the 272-byte Stage 9
        // race where threadmain is still inside the allocator semaphore unlock
        // at 0x08939C4C.  The request callback, rather than that intermediate PC,
        // must hold the worker through the batched submission commit.
        {
            psprecomp::Runtime io_runtime;
            install_profile(io_runtime, 0x08E00000u);
            thread_table = ThreadTable{};
            deferred_io_resumes.clear();
            pending_guest_callbacks.clear();
            async_return_frames.clear();
            virtual_time_us = 0u;

            constexpr std::int32_t submitter_uid = 3;
            constexpr std::int32_t worker_uid = 5;
            constexpr std::int32_t fd = 43;
            constexpr std::uint32_t destination = 0x0882A000u;
            constexpr std::uint32_t byte_count = 272u;
            constexpr std::uint32_t allocator_unlock_pc = 0x08939C4Cu;
            constexpr std::uint32_t batch_commit_pc = 0x08956280u;
            constexpr std::uint32_t worker_return_pc = 0x0893A48Cu;
            constexpr std::uint32_t umd_manager = 0x08E8F000u;
            constexpr std::uint32_t request_pointer = 0x08E90C68u;
            constexpr std::uint32_t world_stream_manager = 0x08E91200u;
            constexpr std::uint32_t active_slot = world_stream_manager + 628u;
            constexpr std::uint32_t event_pattern_slot = 0x0882B000u;

            ThreadRecord submitter{};
            submitter.name = "threadmain";
            submitter.priority = 56u;
            submitter.state = ThreadState::Ready;
            thread_table.threads.emplace(submitter_uid, std::move(submitter));
            psprecomp::AllegrexContext submitter_context{};
            submitter_context.pc = allocator_unlock_pc;
            enqueue_continuation(submitter_uid, submitter_context);

            ThreadRecord worker{};
            worker.name = "UmdStreamThread";
            worker.priority = 32u;
            worker.state = ThreadState::Running;
            thread_table.threads.emplace(worker_uid, std::move(worker));
            thread_table.current_uid = worker_uid;
            psprecomp::set_runtime_thread_identity(worker_uid, "UmdStreamThread");

            file_table.virtual_disc_handles.emplace(fd, VirtualDiscHandle{0u, byte_count, 0u});
            io_runtime.memory().store32(umd_manager + 6916u, request_pointer);
            io_runtime.memory().store32(request_pointer + 16u, 0x08E8F708u);
            io_runtime.memory().store32(request_pointer + 20u, 997376u);
            io_runtime.memory().store32(request_pointer + 24u, byte_count);
            io_runtime.memory().store32(request_pointer + 28u, 0u);
            io_runtime.memory().store32(request_pointer + 48u, 0x089539CCu);
            io_runtime.memory().store32(active_slot, 0u);
            io_runtime.memory().store32(event_pattern_slot, 0x1u);

            require(uncommitted_world_stream_release_pc(io_runtime, request_pointer) == batch_commit_pc,
                    "tiny batched request did not select the final world-stream commit");
            io_runtime.memory().store32(request_pointer + 48u, 0x08953990u);
            require(uncommitted_world_stream_release_pc(io_runtime, request_pointer) == 0x08955EA4u,
                    "tiny single request did not select the final world-stream commit");
            io_runtime.memory().store32(request_pointer + 48u, 0x089539CCu);
            io_runtime.memory().store32(active_slot, request_pointer);
            require(uncommitted_world_stream_release_pc(io_runtime, request_pointer) == 0u,
                    "already-published world-stream request was treated as uncommitted");
            io_runtime.memory().store32(active_slot, 0u);

            psprecomp::AllegrexContext read_context{};
            read_context.set_gpr(4u, static_cast<std::uint32_t>(fd));
            read_context.set_gpr(5u, destination);
            read_context.set_gpr(6u, byte_count);
            read_context.set_gpr(22u, umd_manager);
            read_context.set_gpr(31u, worker_return_pc);
            io_runtime.invoke_import("IoFileMgrForUser", 0x6A638D83u, read_context);

            require(!io_runtime.stopped(), "tiny virtual UMD read stopped the runtime");
            require(thread_table.current_uid == submitter_uid && read_context.pc == allocator_unlock_pc,
                    "tiny UMD read did not restore the allocator-side submitter");
            require(deferred_io_resumes.contains(worker_uid) &&
                        deferred_io_resumes.at(worker_uid).handoff_pc == allocator_unlock_pc &&
                        deferred_io_resumes.at(worker_uid).release_pc == batch_commit_pc,
                    "tiny UMD read used the allocator unlock as its release boundary");

            // Neither the allocator unlock nor the eventual request-pointer store
            // is sufficient; the completion bit would still be erased by the
            // following event clear.
            vcs_post_dispatch_hook(io_runtime, read_context, allocator_unlock_pc, submitter_uid);
            io_runtime.memory().store32(active_slot, request_pointer);
            vcs_post_dispatch_hook(io_runtime, read_context, 0x08956258u, submitter_uid);
            require(deferred_io_resumes.contains(worker_uid) &&
                        thread_table.threads.at(worker_uid).state == ThreadState::IoDeferred,
                    "tiny UMD worker resumed before the batched event transaction committed");

            io_runtime.memory().store32(event_pattern_slot, 0u);
            io_runtime.memory().store32(event_pattern_slot, 0x4u);
            vcs_post_dispatch_hook(io_runtime, read_context, batch_commit_pc, submitter_uid);
            require(!deferred_io_resumes.contains(worker_uid) &&
                        thread_table.current_uid == worker_uid &&
                        read_context.pc == worker_return_pc &&
                        read_context.gpr[2] == byte_count,
                    "tiny UMD worker did not resume at the final batched commit");
            io_runtime.memory().store32(event_pattern_slot,
                                        io_runtime.memory().load32(event_pattern_slot) | 0x1u);
            require(io_runtime.memory().load32(event_pattern_slot) == 0x5u,
                    "tiny request completion bit was lost after the final commit");
            file_table = FileTable{};
        }

        // A callback chain must restore the complete original Allegrex state
        // before every callback and after the final callback.
        reset();
        ThreadRecord callback_thread{};
        callback_thread.name = "callback-test";
        callback_thread.priority = 32u;
        callback_thread.state = ThreadState::Running;
        thread_table.threads.emplace(7, std::move(callback_thread));
        thread_table.current_uid = 7;

        psprecomp::AllegrexContext original{};
        for (std::uint32_t i = 1u; i < original.gpr.size(); ++i) original.gpr[i] = 0x10000000u + i;
        original.hi = 0xA1A2A3A4u;
        original.lo = 0xB1B2B3B4u;
        original.pc = 0x08801234u;
        for (std::size_t i = 0u; i < original.fpr.size(); ++i) original.fpr[i] = static_cast<float>(i) + 0.25f;
        original.fcr31 = 0x01020304u;
        for (std::size_t i = 0u; i < original.vfpu.size(); ++i) original.vfpu[i] = static_cast<float>(i) - 3.5f;
        for (std::size_t i = 0u; i < original.vfpu_ctrl.size(); ++i)
            original.vfpu_ctrl[i] = 0x20000000u + static_cast<std::uint32_t>(i);

        pending_guest_callbacks[7] = {
            GuestCallbackInvocation{0x08810000u, 1u, 2u, 3u},
            GuestCallbackInvocation{0x08820000u, 4u, 5u, 6u},
        };
        psprecomp::AllegrexContext callback = original;
        require(maybe_start_pending_guest_callback(callback), "first callback did not start");
        require(callback.pc == 0x08810000u && callback.gpr[4] == 1u && callback.gpr[5] == 2u && callback.gpr[6] == 3u,
                "first callback arguments are incorrect");
        callback.gpr.fill(0xDEADBEEFu);
        callback.hi = callback.lo = 0xDEADBEEFu;
        callback.pc = 4u;
        callback.fpr.fill(-99.0f);
        callback.fcr31 = 0xFFFFFFFFu;
        callback.vfpu.fill(-88.0f);
        callback.vfpu_ctrl.fill(0xFFFFFFFFu);

        psprecomp::Runtime runtime;
        psp_interrupt_return(runtime, callback);
        require(callback.pc == 0x08820000u && callback.gpr[4] == 4u && callback.gpr[5] == 5u && callback.gpr[6] == 6u,
                "second callback did not start from the restored frame");
        require(callback.gpr[16] == original.gpr[16] && callback.gpr[29] == original.gpr[29] &&
                    callback.hi == original.hi && callback.lo == original.lo && callback.fpr[7] == original.fpr[7] &&
                    callback.fcr31 == original.fcr31 && callback.vfpu[60] == original.vfpu[60] &&
                    callback.vfpu_ctrl[9] == original.vfpu_ctrl[9],
                "callback leaked guest CPU/FPU/VFPU state into the next callback");

        callback.gpr.fill(0xCAFEBABEu);
        callback.fpr.fill(-77.0f);
        callback.vfpu.fill(-66.0f);
        callback.vfpu_ctrl.fill(0xEEEEEEEEu);
        callback.pc = 4u;
        psp_interrupt_return(runtime, callback);
        require(callback.gpr == original.gpr && callback.hi == original.hi && callback.lo == original.lo &&
                    callback.pc == original.pc && callback.fpr == original.fpr && callback.fcr31 == original.fcr31 &&
                    callback.vfpu == original.vfpu && callback.vfpu_ctrl == original.vfpu_ctrl,
                "final callback return did not restore the complete guest context");
        require(async_return_frames.empty() && pending_guest_callbacks.empty(),
                "callback bookkeeping remained after the callback chain ended");

        // VCS variadic ABI consumes a2/a3 and t0-t3 before the caller stack.
        {
            psprecomp::Runtime sprintf_runtime;
            psprecomp::AllegrexContext sprintf_context{};
            constexpr std::uint32_t destination = 0x08810000u;
            constexpr std::uint32_t format_address = 0x08810100u;
            constexpr std::uint32_t strings_address = 0x08810200u;
            constexpr std::uint32_t stack_address = 0x08811000u;
            const std::string format = "%s%s%s%s%s%s%s";
            std::vector<std::uint8_t> format_bytes(format.begin(), format.end());
            format_bytes.push_back(0u);
            sprintf_runtime.memory().copy_in(format_address, format_bytes);
            for (std::uint32_t i = 0u; i < 7u; ++i) {
                const std::array<std::uint8_t, 2> text{static_cast<std::uint8_t>('A' + i), 0u};
                sprintf_runtime.memory().copy_in(strings_address + i * 4u, text);
            }
            sprintf_context.set_gpr(4u, destination);
            sprintf_context.set_gpr(5u, format_address);
            for (std::uint32_t i = 0u; i < 6u; ++i)
                sprintf_context.set_gpr(6u + i, strings_address + i * 4u);
            sprintf_context.set_gpr(29u, stack_address);
            sprintf_context.set_gpr(31u, 0x08812000u);
            // Spilled words sit at sp+0. These two self-tests used to place
            // them at sp+16, which is where an ordinary o32 caller would, but
            // that was written to match the reader rather than the game: the
            // save-description call stores its spilled words at 0(sp), 4(sp)
            // and 8(sp). See O32VarArgs::next_u32().
            sprintf_runtime.memory().store32(stack_address, strings_address + 24u);
            vcs_sprintf(sprintf_runtime, sprintf_context);
            require(!sprintf_runtime.stopped(), "VCS sprintf variadic ABI self-test stopped runtime");
            require(sprintf_runtime.memory().read_c_string(destination, 32u) == "ABCDEFG",
                    "VCS sprintf did not consume a2/a3/t0-t3 before stack arguments");
            require(sprintf_context.gpr[2] == 7u && sprintf_context.pc == 0x08812000u,
                    "VCS sprintf return state is incorrect");
        }

        {
            psprecomp::Runtime sprintf_runtime;
            psprecomp::AllegrexContext sprintf_context{};
            constexpr std::uint32_t destination = 0x08812000u;
            constexpr std::uint32_t format_address = 0x08812100u;
            constexpr std::uint32_t first_string = 0x08812200u;
            constexpr std::uint32_t last_string = 0x08812210u;
            constexpr std::uint32_t stack_address = 0x08813000u;
            const std::string format = "%s %.2f %.1e %.3g %s";
            std::vector<std::uint8_t> format_bytes(format.begin(), format.end());
            format_bytes.push_back(0u);
            sprintf_runtime.memory().copy_in(format_address, format_bytes);
            const std::array<std::uint8_t, 2> first_text{'X', 0u};
            const std::array<std::uint8_t, 2> last_text{'Y', 0u};
            sprintf_runtime.memory().copy_in(first_string, first_text);
            sprintf_runtime.memory().copy_in(last_string, last_text);

            const auto set_pair = [&](std::uint32_t low_reg, double value) {
                const std::uint64_t bits = std::bit_cast<std::uint64_t>(value);
                sprintf_context.set_gpr(low_reg, static_cast<std::uint32_t>(bits));
                sprintf_context.set_gpr(low_reg + 1u, static_cast<std::uint32_t>(bits >> 32u));
            };
            sprintf_context.set_gpr(4u, destination);
            sprintf_context.set_gpr(5u, format_address);
            sprintf_context.set_gpr(6u, first_string);
            sprintf_context.set_gpr(7u, 0xDEADBEEFu); // skipped for 64-bit alignment
            set_pair(8u, 1.25);
            set_pair(10u, 2.5);
            sprintf_context.set_gpr(29u, stack_address);
            sprintf_context.set_gpr(31u, 0x08814000u);
            const std::uint64_t third_bits = std::bit_cast<std::uint64_t>(3.75);
            sprintf_runtime.memory().store32(stack_address, static_cast<std::uint32_t>(third_bits));
            sprintf_runtime.memory().store32(stack_address + 4u, static_cast<std::uint32_t>(third_bits >> 32u));
            sprintf_runtime.memory().store32(stack_address + 8u, last_string);

            vcs_sprintf(sprintf_runtime, sprintf_context);
            require(!sprintf_runtime.stopped(), "VCS sprintf floating ABI self-test stopped runtime");
            require(sprintf_runtime.memory().read_c_string(destination, 128u) == "X 1.25 2.5e+00 3.75 Y",
                    "VCS sprintf floating conversions or 64-bit alignment are incorrect");
        }

        {
            psprecomp::Runtime wlan_runtime;
            install_profile(wlan_runtime, 0x08E8AC00u);
            psprecomp::AllegrexContext wlan_context{};
            wlan_context.set_gpr(2u, 0xFFFFFFFFu);
            wlan_runtime.invoke_import("sceWlanDrv", 0xD7763699u, wlan_context);
            require(!wlan_runtime.stopped(), "sceWlanGetSwitchState is not registered");
            require(wlan_context.gpr[2] == 0u,
                    "offline native profile did not report the WLAN switch as off");

            psprecomp::AllegrexContext profiler_context{};
            profiler_context.set_gpr(2u, 0xFFFFFFFFu);
            wlan_runtime.invoke_import("ThreadManForUser", 0x64D4540Eu, profiler_context);
            require(!wlan_runtime.stopped() && profiler_context.gpr[2] == 0u,
                    "sceKernelReferThreadProfiler did not return a null profiler block");
            profiler_context.set_gpr(2u, 0xFFFFFFFFu);
            wlan_runtime.invoke_import("ThreadManForUser", 0x8218B4DDu, profiler_context);
            require(!wlan_runtime.stopped() && profiler_context.gpr[2] == 0u,
                    "sceKernelReferGlobalProfiler did not return a null profiler block");

            // Standard streamed RIFF/ATRAC3+ initialization and metadata flow.
            constexpr std::uint32_t atrac_buffer = 0x08818000u;
            constexpr std::uint32_t atrac_outputs = 0x08819000u;
            std::vector<std::uint8_t> atrac_header(0x100u, 0u);
            const auto put16 = [&](std::size_t offset, std::uint16_t value) {
                atrac_header[offset] = static_cast<std::uint8_t>(value);
                atrac_header[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
            };
            const auto put32 = [&](std::size_t offset, std::uint32_t value) {
                for (std::size_t i = 0u; i < 4u; ++i)
                    atrac_header[offset + i] = static_cast<std::uint8_t>(value >> (i * 8u));
            };
            std::memcpy(atrac_header.data() + 0u, "RIFF", 4u);
            put32(4u, 0x1000u - 8u);
            std::memcpy(atrac_header.data() + 8u, "WAVE", 4u);
            std::memcpy(atrac_header.data() + 12u, "fmt ", 4u);
            put32(16u, 0x34u);
            put16(20u, 0xFFFEu); put16(22u, 2u); put32(24u, 44100u);
            put32(28u, 12058u); put16(32u, 560u); put16(34u, 0u);
            std::memcpy(atrac_header.data() + 72u, "fact", 4u);
            put32(76u, 8u); put32(80u, 4096u); put32(84u, 0x800u);
            std::memcpy(atrac_header.data() + 88u, "data", 4u);
            put32(92u, 0x1000u - 96u);
            wlan_runtime.memory().copy_in(atrac_buffer, atrac_header);

            psprecomp::AllegrexContext atrac_context{};
            atrac_context.set_gpr(4u, atrac_buffer);
            atrac_context.set_gpr(5u, 0x100u);
            atrac_context.set_gpr(6u, 0x400u);
            wlan_runtime.invoke_import("sceAtrac3plus", 0x0FAE370Eu, atrac_context);
            require(!wlan_runtime.stopped() && atrac_context.gpr[2] == 0u,
                    "sceAtracSetHalfwayBufferAndGetID rejected a valid ATRAC3+ RIFF header");

            atrac_context = {};
            atrac_context.set_gpr(4u, 0u); atrac_context.set_gpr(5u, atrac_outputs);
            wlan_runtime.invoke_import("sceAtrac3plus", 0xA554A158u, atrac_context);
            require(atrac_context.gpr[2] == 0u && wlan_runtime.memory().load32(atrac_outputs) == 96u,
                    "sceAtracGetBitrate did not derive the ATRAC3+ bitrate from block alignment");

            atrac_context = {};
            atrac_context.set_gpr(4u, 0u);
            atrac_context.set_gpr(5u, atrac_outputs + 4u);
            atrac_context.set_gpr(6u, atrac_outputs + 8u);
            atrac_context.set_gpr(7u, atrac_outputs + 12u);
            wlan_runtime.invoke_import("sceAtrac3plus", 0x5D268707u, atrac_context);
            require(atrac_context.gpr[2] == 0u &&
                    wlan_runtime.memory().load32(atrac_outputs + 4u) == atrac_buffer + 0x100u &&
                    wlan_runtime.memory().load32(atrac_outputs + 8u) == 0x300u &&
                    wlan_runtime.memory().load32(atrac_outputs + 12u) == 0x100u,
                    "sceAtracGetStreamDataInfo returned an incorrect ring-buffer window");

            atrac_context = {};
            atrac_context.set_gpr(4u, 0u); atrac_context.set_gpr(5u, 560u);
            wlan_runtime.invoke_import("sceAtrac3plus", 0x7DB31251u, atrac_context);
            require(atrac_context.gpr[2] == 0u, "sceAtracAddStreamData rejected its advertised write size");
            atrac_context = {};
            atrac_context.set_gpr(4u, 0u);
            wlan_runtime.invoke_import("sceAtrac3plus", 0x61EB33F5u, atrac_context);
            require(atrac_context.gpr[2] == 0u, "sceAtracReleaseAtracID failed for a valid context");

            constexpr std::uint32_t sas_core = 0x08820000u;
            constexpr std::uint32_t sas_data = 0x08821000u;
            constexpr std::uint32_t sas_loop_data = 0x08821100u;
            constexpr std::uint32_t sas_output = 0x08822000u;

            // Two deliberately non-zero PSX-ADPCM blocks.  Filter 0/shift 0
            // makes nibble 1 decode to +4096 and nibble 2 to +8192, giving the
            // mixer test a deterministic audible signal instead of validating
            // the old all-zero bring-up stub.
            std::array<std::uint8_t, 32> finite_vag{};
            finite_vag[0] = 0x00u; finite_vag[1] = 0x00u;
            std::fill(finite_vag.begin() + 2, finite_vag.begin() + 16, 0x11u);
            finite_vag[16] = 0x00u; finite_vag[17] = 0x07u;
            std::fill(finite_vag.begin() + 18, finite_vag.end(), 0x22u);
            wlan_runtime.memory().copy_in(sas_data, finite_vag);

            // Loop-start / loop-end markers exercise the PSP SAS loop semantics.
            std::array<std::uint8_t, 32> loop_vag{};
            loop_vag[0] = 0x00u; loop_vag[1] = 0x06u;
            std::fill(loop_vag.begin() + 2, loop_vag.begin() + 16, 0x11u);
            loop_vag[16] = 0x00u; loop_vag[17] = 0x03u;
            std::fill(loop_vag.begin() + 18, loop_vag.end(), 0x22u);
            wlan_runtime.memory().copy_in(sas_loop_data, loop_vag);

            psprecomp::AllegrexContext sas_context{};
            sas_context.set_gpr(4u, sas_core);
            sas_context.set_gpr(5u, 0x100u);
            sas_context.set_gpr(6u, 32u);
            sas_context.set_gpr(7u, 0u);
            sas_context.set_gpr(8u, 44100u);
            wlan_runtime.invoke_import("sceSasCore", 0x42778A9Fu, sas_context);
            require(!wlan_runtime.stopped() && sas_context.gpr[2] == 0u,
                    "__sceSasInit rejected a valid mixer configuration");

            const auto configure_voice = [&](std::uint32_t address, std::uint32_t loop) {
                psprecomp::AllegrexContext c{};
                c.set_gpr(4u, sas_core); c.set_gpr(5u, 0u);
                c.set_gpr(6u, address); c.set_gpr(7u, 0x20u); c.set_gpr(8u, loop);
                wlan_runtime.invoke_import("sceSasCore", 0x99944089u, c);
                require(c.gpr[2] == 0u, "__sceSasSetVoice rejected valid VAG metadata");
                c = {};
                c.set_gpr(4u, sas_core); c.set_gpr(5u, 0u);
                c.set_gpr(6u, 0x1000u); c.set_gpr(7u, 0x1000u);
                c.set_gpr(8u, 0u); c.set_gpr(9u, 0u);
                wlan_runtime.invoke_import("sceSasCore", 0x440CA7D8u, c);
                require(c.gpr[2] == 0u, "__sceSasSetVolume rejected unity dry volume");
            };
            const auto key_on_voice0 = [&] {
                psprecomp::AllegrexContext c{};
                c.set_gpr(4u, sas_core); c.set_gpr(5u, 0u);
                wlan_runtime.invoke_import("sceSasCore", 0x76F01ACAu, c);
                require(c.gpr[2] == 0u, "__sceSasSetKeyOn failed");
            };
            const auto output_has_nonzero_pcm = [&] {
                for (std::uint32_t frame = 0u; frame < 0x100u; ++frame) {
                    const auto l = static_cast<std::int16_t>(wlan_runtime.memory().load16(sas_output + frame * 4u));
                    const auto r = static_cast<std::int16_t>(wlan_runtime.memory().load16(sas_output + frame * 4u + 2u));
                    if (l != 0 || r != 0) return true;
                }
                return false;
            };

            configure_voice(sas_data, 0u);
            key_on_voice0();
            sas_context = {};
            sas_context.set_gpr(4u, sas_core);
            wlan_runtime.invoke_import("sceSasCore", 0x68A46B95u, sas_context);
            require((sas_context.gpr[2] & 1u) == 0u,
                    "active SAS voice was reported ended before mixing");

            wlan_runtime.memory().zero(sas_output, 0x400u);
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            wlan_runtime.invoke_import("sceSasCore", 0xA3589D81u, sas_context);
            require(sas_context.gpr[2] == 0u && output_has_nonzero_pcm(),
                    "__sceSasCore failed to render non-zero VAG PCM");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core);
            wlan_runtime.invoke_import("sceSasCore", 0x68A46B95u, sas_context);
            require((sas_context.gpr[2] & 1u) != 0u,
                    "finite non-looping SAS voice did not reach its end flag");

            // Re-triggering the exact same voice must rewind the ADPCM decoder.
            // This catches the old bug where a reused gunshot/footstep resumed at
            // EOF and therefore vanished after its first play.
            key_on_voice0();
            wlan_runtime.memory().zero(sas_output, 0x400u);
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            wlan_runtime.invoke_import("sceSasCore", 0xA3589D81u, sas_context);
            require(sas_context.gpr[2] == 0u && output_has_nonzero_pcm(),
                    "SAS KeyOn did not rewind/replay a reused VAG voice");

            // CoreWithMix must scale the caller's existing PCM and then add SAS
            // voices.  VCS uses this path for real effects; preserving the input
            // unchanged (the old stub) made those voices completely inaudible.
            configure_voice(sas_loop_data, 1u);
            key_on_voice0();
            for (std::uint32_t frame = 0u; frame < 0x100u; ++frame) {
                wlan_runtime.memory().store16(sas_output + frame * 4u, static_cast<std::uint16_t>(1000));
                wlan_runtime.memory().store16(sas_output + frame * 4u + 2u,
                                               static_cast<std::uint16_t>(static_cast<std::int16_t>(-1000)));
            }
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            sas_context.set_gpr(6u, 0x800u); sas_context.set_gpr(7u, 0x800u);
            wlan_runtime.invoke_import("sceSasCore", 0x50A14DFCu, sas_context);
            const auto mixed_l = static_cast<std::int16_t>(wlan_runtime.memory().load16(sas_output));
            const auto mixed_r = static_cast<std::int16_t>(wlan_runtime.memory().load16(sas_output + 2u));
            require(sas_context.gpr[2] == 0u && mixed_l > 500 && mixed_r > -500,
                    "__sceSasCoreWithMix did not scale input and add the SAS voice");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core);
            wlan_runtime.invoke_import("sceSasCore", 0x68A46B95u, sas_context);
            require((sas_context.gpr[2] & 1u) == 0u,
                    "loop-marker SAS voice ended instead of returning to its loop start");

            // Noise voices used to be accepted by the HLE but never rendered.
            // Use voice 1 so the looped VAG above also verifies multi-voice sum.
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 1u); sas_context.set_gpr(6u, 63u);
            wlan_runtime.invoke_import("sceSasCore", 0xB7660A23u, sas_context);
            require(sas_context.gpr[2] == 0u, "__sceSasSetNoise rejected a valid frequency");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 1u);
            sas_context.set_gpr(6u, 0x1000u); sas_context.set_gpr(7u, 0x1000u);
            sas_context.set_gpr(8u, 0u); sas_context.set_gpr(9u, 0u);
            wlan_runtime.invoke_import("sceSasCore", 0x440CA7D8u, sas_context);
            require(sas_context.gpr[2] == 0u, "noise voice volume setup failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 1u);
            wlan_runtime.invoke_import("sceSasCore", 0x76F01ACAu, sas_context);
            require(sas_context.gpr[2] == 0u, "noise voice KeyOn failed");
            wlan_runtime.memory().zero(sas_output, 0x400u);
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            wlan_runtime.invoke_import("sceSasCore", 0xA3589D81u, sas_context);
            require(sas_context.gpr[2] == 0u && output_has_nonzero_pcm(),
                    "SAS noise voice was configured but rendered silence");

            // Effect-only routing used to be dropped because effectLeft/effectRight
            // were stored but never mixed.  Reinitialize the core, send voice 0
            // only to the wet bus, and require audible output with dry disabled.
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0x100u);
            sas_context.set_gpr(6u, 32u); sas_context.set_gpr(7u, 0u);
            sas_context.set_gpr(8u, 44100u);
            wlan_runtime.invoke_import("sceSasCore", 0x42778A9Fu, sas_context);
            require(sas_context.gpr[2] == 0u, "SAS re-init before wet-bus test failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0u);
            sas_context.set_gpr(6u, sas_data); sas_context.set_gpr(7u, 0x20u);
            sas_context.set_gpr(8u, 0u);
            wlan_runtime.invoke_import("sceSasCore", 0x99944089u, sas_context);
            require(sas_context.gpr[2] == 0u, "wet-bus VAG setup failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0u);
            sas_context.set_gpr(6u, 0u); sas_context.set_gpr(7u, 0u);
            sas_context.set_gpr(8u, 0x1000u); sas_context.set_gpr(9u, 0x1000u);
            wlan_runtime.invoke_import("sceSasCore", 0x440CA7D8u, sas_context);
            require(sas_context.gpr[2] == 0u, "wet-bus volume setup failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0x1000u);
            sas_context.set_gpr(6u, 0x1000u);
            wlan_runtime.invoke_import("sceSasCore", 0xD5A229C9u, sas_context);
            require(sas_context.gpr[2] == 0u, "wet-bus global volume setup failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0u);
            sas_context.set_gpr(6u, 1u);
            wlan_runtime.invoke_import("sceSasCore", 0xF983B186u, sas_context);
            require(sas_context.gpr[2] == 0u, "wet-only RevVON setup failed");
            key_on_voice0();
            wlan_runtime.memory().zero(sas_output, 0x400u);
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            wlan_runtime.invoke_import("sceSasCore", 0xA3589D81u, sas_context);
            require(sas_context.gpr[2] == 0u && output_has_nonzero_pcm(),
                    "effect-only SAS voice disappeared from the wet bus");

            // Raw SAS output is four signed-16 planes (dry L/R, send L/R), not
            // the mono buffer used by the old HLE.  Validate both the larger
            // layout and the effect-send planes.
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0x100u);
            sas_context.set_gpr(6u, 32u); sas_context.set_gpr(7u, 1u);
            sas_context.set_gpr(8u, 44100u);
            wlan_runtime.invoke_import("sceSasCore", 0x42778A9Fu, sas_context);
            require(sas_context.gpr[2] == 0u, "SAS raw-mode init failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0u);
            sas_context.set_gpr(6u, sas_data); sas_context.set_gpr(7u, 0x20u);
            sas_context.set_gpr(8u, 0u);
            wlan_runtime.invoke_import("sceSasCore", 0x99944089u, sas_context);
            require(sas_context.gpr[2] == 0u, "raw-mode VAG setup failed");
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, 0u);
            sas_context.set_gpr(6u, 0x1000u); sas_context.set_gpr(7u, 0x800u);
            sas_context.set_gpr(8u, 0x600u); sas_context.set_gpr(9u, 0x400u);
            wlan_runtime.invoke_import("sceSasCore", 0x440CA7D8u, sas_context);
            require(sas_context.gpr[2] == 0u, "raw-mode volume setup failed");
            key_on_voice0();
            wlan_runtime.memory().zero(sas_output, 0x800u);
            sas_context = {};
            sas_context.set_gpr(4u, sas_core); sas_context.set_gpr(5u, sas_output);
            wlan_runtime.invoke_import("sceSasCore", 0xA3589D81u, sas_context);
            bool raw_dry_nonzero = false;
            bool raw_send_nonzero = false;
            for (std::uint32_t frame = 0u; frame < 0x100u; ++frame) {
                raw_dry_nonzero |= static_cast<std::int16_t>(
                    wlan_runtime.memory().load16(sas_output + frame * 2u)) != 0;
                raw_send_nonzero |= static_cast<std::int16_t>(
                    wlan_runtime.memory().load16(sas_output + 0x400u + frame * 2u)) != 0;
            }
            require(sas_context.gpr[2] == 0u && raw_dry_nonzero && raw_send_nonzero,
                    "SAS raw-mode did not expose dry/effect planes");

        }

        reset();
        error.clear();
        return true;
    } catch (const std::exception &exception) {
        error = exception.what();
        return false;
    }
}

} // namespace vcs
