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
#include "vcs_runtime_log.hpp"

#include "psprecomp/common.hpp"
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
struct DirectoryHandle {
    std::vector<std::filesystem::directory_entry> entries;
    std::size_t index{};
};

struct VirtualDiscFile {
    std::filesystem::path native_path;
    std::uint32_t start_sector{};
    std::uint64_t size{};
};

struct VirtualDiscHandle {
    std::uint64_t base_offset{};
    std::uint64_t length{};
    std::uint64_t position{};
};

// Host file kept open across UMD sector reads. VCS streams assets in many
// consecutive sceIoRead calls; reopening the same Windows file for every chunk
// serialized CreateFile/open + metadata work onto the guest CPU thread.
struct VirtualDiscStream {
    std::ifstream input;
    std::uint64_t position{};
    bool position_valid{};
};

struct FileTable {
    std::int32_t next_fd{3};
    std::uint32_t next_virtual_sector{0x00010000u};
    std::unordered_map<std::int32_t, std::fstream> files;
    std::unordered_set<std::int32_t> synthetic_empty_files;
    std::unordered_map<std::int32_t, DirectoryHandle> directories;
    std::unordered_map<std::int32_t, VirtualDiscHandle> virtual_disc_handles;
    std::unordered_map<std::string, VirtualDiscFile> virtual_files_by_path;
    std::map<std::uint32_t, std::string> virtual_path_by_sector;
    std::unordered_map<std::string, VirtualDiscStream> virtual_disc_streams;
};

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

struct AudioChannelState {
    bool reserved{};
    std::uint32_t sample_count{};
    std::uint32_t format{};
    std::uint32_t left_volume{};
    std::uint32_t right_volume{};
    std::uint64_t busy_until_us{};
    // sceAudioSRCChReserve picks these per channel. They used to be discarded,
    // so a 22050 Hz talk-radio stream was played as if it were 44100 and came
    // out at double speed.
    std::uint32_t frequency{44100u};
    std::uint32_t channel_count{2u};

    // Hardware pacing anchor.  The DAC consumes queued buffers back to back, so
    // the start time of buffer N is the end time of buffer N-1 -- never "now
    // plus one buffer".  Deriving it from an accumulated frame count keeps the
    // channel exactly on the 44100 Hz grid instead of charging the guest's own
    // decode time to the audio timeline.
    bool queue_active{};
    std::uint64_t queue_anchor_us{};
    std::uint64_t queued_frames{};
};

enum class ThreadState {
    Created,
    Ready,
    Running,
    Sleeping,
    Delayed,
    IoDeferred,
    Completed,
};

struct ThreadRecord {
    std::string name;
    std::uint32_t entry{};
    std::uint32_t priority{};
    std::uint32_t stack_size{};
    std::uint32_t attributes{};
    std::uint32_t stack_top{};
    std::uint32_t stack_bottom{};
    std::uint32_t kernel_context{};
    ThreadState state{ThreadState::Created};
    std::uint32_t exit_status{};
    bool externally_suspended{};
    psprecomp::AllegrexContext suspended_context{};
    std::uint32_t wakeup_count{};
    std::uint64_t delay_until_us{};
    std::uint64_t delay_sequence{};
};

struct ThreadContinuation {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::uint64_t ready_sequence{};
};

struct FreeThreadStack {
    std::uint32_t bottom{};
    std::uint32_t top{};
};

struct PartitionBlock {
    std::string name;
    std::uint32_t address{};
    std::uint32_t size{};
};

struct PartitionTable {
    std::int32_t next_uid{0x100};
    std::uint32_t next_address{};
    std::unordered_map<std::int32_t, PartitionBlock> blocks;
};

struct CallbackRecord {
    std::string name;
    std::uint32_t function{};
    std::uint32_t common{};
    std::int32_t owner_uid{};
    std::uint32_t notify_count{};
    std::uint32_t notify_argument{};
};

struct CallbackTable {
    std::int32_t next_uid{0x200};
    std::unordered_map<std::int32_t, CallbackRecord> callbacks;
};

struct SemaphoreWaiter {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::int32_t requested{};
};

struct SemaphoreRecord {
    std::string name;
    std::int32_t count{};
    std::int32_t maximum{};
    std::vector<SemaphoreWaiter> waiters;
};

struct SemaphoreTable {
    std::int32_t next_uid{0x300};
    std::unordered_map<std::int32_t, SemaphoreRecord> semaphores;
};

struct EventFlagWaiter {
    std::int32_t uid{};
    psprecomp::AllegrexContext context{};
    std::uint32_t requested{};
    std::uint32_t mode{};
    std::uint32_t output_address{};
};

struct EventFlagRecord {
    std::string name;
    std::uint32_t attributes{};
    std::uint32_t initial_pattern{};
    std::uint32_t current_pattern{};
    std::vector<EventFlagWaiter> waiters;
};

struct EventFlagTable {
    std::int32_t next_uid{0x600};
    std::unordered_map<std::int32_t, EventFlagRecord> flags;
};

struct FixedPoolRecord {
    std::string name;
    std::uint32_t address{};
    std::uint32_t block_size{};
    std::uint32_t block_count{};
    std::vector<bool> allocated;
};

struct FixedPoolTable {
    std::int32_t next_uid{0x500};
    std::unordered_map<std::int32_t, FixedPoolRecord> pools;
};

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
std::uint64_t event_diag_poll_count{};
std::uint64_t event_diag_stop_polls{};
bool event_diag_stall_reported{};

struct ThreadTable {
    std::int32_t next_uid{1};
    std::int32_t current_uid{0};
    // PSP user RAM ends at 0x0A000000.  User thread stacks are allocated
    // downward from the real partition top with 256-byte granularity.
    std::uint32_t next_stack_top{0x0A000000u};
    std::uint64_t next_ready_sequence{1u};
    std::uint64_t next_delay_sequence{1u};
    std::unordered_map<std::int32_t, ThreadRecord> threads;
    std::vector<ThreadContinuation> continuations;
    std::unordered_map<std::int32_t, std::vector<ThreadContinuation>> thread_end_waiters;
    std::vector<FreeThreadStack> free_stacks;
};

FileTable file_table;

std::string normalized_native_path(const std::filesystem::path &path) {
    std::error_code error;
    auto normalized = std::filesystem::weakly_canonical(path, error);
    if (error) normalized = std::filesystem::absolute(path, error);
    if (error) normalized = path.lexically_normal();
    return normalized.generic_string();
}

const VirtualDiscFile *register_virtual_disc_file(const std::filesystem::path &path) {
    std::error_code error;
    if (!std::filesystem::is_regular_file(path, error) || error) return nullptr;

    const std::string key = normalized_native_path(path);
    if (const auto found = file_table.virtual_files_by_path.find(key);
        found != file_table.virtual_files_by_path.end()) {
        return &found->second;
    }

    const std::uint64_t size = std::filesystem::file_size(path, error);
    if (error) return nullptr;
    const std::uint64_t sector_count = std::max<std::uint64_t>(1u, (size + 2047u) / 2048u);
    if (sector_count > 0xFFFFFFFFull ||
        static_cast<std::uint64_t>(file_table.next_virtual_sector) + sector_count > 0x100000000ull) {
        return nullptr;
    }

    VirtualDiscFile item{};
    item.native_path = path;
    item.start_sector = file_table.next_virtual_sector;
    item.size = size;
    file_table.next_virtual_sector += static_cast<std::uint32_t>(sector_count);
    const auto [inserted, ok] = file_table.virtual_files_by_path.emplace(key, std::move(item));
    if (!ok) return &inserted->second;
    file_table.virtual_path_by_sector.emplace(inserted->second.start_sector, key);
    return &inserted->second;
}

const VirtualDiscFile *find_virtual_disc_file(std::uint32_t start_sector, std::uint64_t requested_size) {
    const auto sector = file_table.virtual_path_by_sector.find(start_sector);
    if (sector == file_table.virtual_path_by_sector.end()) return nullptr;
    const auto file = file_table.virtual_files_by_path.find(sector->second);
    if (file == file_table.virtual_files_by_path.end()) return nullptr;
    if (requested_size != 0u && requested_size > file->second.size) return nullptr;
    return &file->second;
}

// Virtual-disc read accounting.
//
// A sector range that no registered file covers is silently zero-filled below.
// The guest cannot tell that apart from real data, so missing world geometry or
// missing collision models look like renderer or physics bugs instead of an
// incomplete sector map.  Count both paths so the difference is measurable.
struct DiscReadStats {
    std::uint64_t bytes_from_files{};
    std::uint64_t bytes_zero_filled{};
    std::uint64_t zero_fill_events{};
    std::uint64_t short_reads{};
    std::uint64_t open_failures{};
    std::uint64_t reported_events{};
};
DiscReadStats disc_read_stats;
// Host time physically spent inside sceIoRead. Only accumulated while the
// frame-time diagnostic is enabled, so the production fast path pays no clock
// query cost. This makes cold-storage stalls visible separately from guest AOT.
std::chrono::steady_clock::duration io_host_time_this_vblank{};

bool disc_read_diag_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_DISC_READ_DIAG") != nullptr;
    return enabled;
}

std::size_t read_virtual_disc(VirtualDiscHandle &handle, std::span<std::uint8_t> output) {
    if (handle.position >= handle.length || output.empty()) return 0u;
    const std::uint64_t available = handle.length - handle.position;
    const std::size_t requested = static_cast<std::size_t>(
        std::min<std::uint64_t>(available, output.size()));
    std::fill(output.begin(), output.begin() + requested, 0u);

    std::size_t written = 0u;
    while (written < requested) {
        const std::uint64_t absolute = handle.base_offset + handle.position + written;
        const std::uint64_t sector64 = absolute / 2048u;
        if (sector64 > 0xFFFFFFFFull) break;
        const auto next = file_table.virtual_path_by_sector.upper_bound(static_cast<std::uint32_t>(sector64));

        const VirtualDiscFile *file = nullptr;
        const std::string *file_key = nullptr;
        if (next != file_table.virtual_path_by_sector.begin()) {
            const auto previous = std::prev(next);
            const auto found = file_table.virtual_files_by_path.find(previous->second);
            if (found != file_table.virtual_files_by_path.end()) {
                const std::uint64_t file_start = static_cast<std::uint64_t>(found->second.start_sector) * 2048u;
                if (absolute >= file_start && absolute < file_start + found->second.size) {
                    file = &found->second;
                    file_key = &previous->second;
                }
            }
        }

        if (file != nullptr && file_key != nullptr) {
            const std::uint64_t file_start = static_cast<std::uint64_t>(file->start_sector) * 2048u;
            const std::uint64_t file_offset = absolute - file_start;
            const std::size_t chunk = static_cast<std::size_t>(std::min<std::uint64_t>(
                requested - written, file->size - file_offset));

            // Reuse one host handle per registered disc file. Most world-stream
            // requests are sequential, so retain the native stream position too
            // and skip seekg() when the next chunk starts where the previous one
            // ended. The OS page cache can now do useful read-ahead instead of
            // seeing a new open/close lifetime for every PSP read.
            VirtualDiscStream &cached = file_table.virtual_disc_streams[*file_key];
            if (!cached.input.is_open()) {
                cached.input.open(file->native_path, std::ios::binary);
                cached.position = 0u;
                cached.position_valid = cached.input.good();
            }
            if (!cached.input) {
                ++disc_read_stats.open_failures;
                cached.position_valid = false;
                if (disc_read_diag_enabled()) {
                    std::cerr << "[disc-read] open failed path=\"" << file->native_path.string()
                              << "\"\n";
                }
                break;
            }
            if (!cached.position_valid || cached.position != file_offset) {
                cached.input.clear();
                cached.input.seekg(static_cast<std::streamoff>(file_offset), std::ios::beg);
                if (!cached.input) {
                    ++disc_read_stats.short_reads;
                    cached.position_valid = false;
                    break;
                }
                cached.position = file_offset;
                cached.position_valid = true;
            }
            cached.input.read(reinterpret_cast<char *>(output.data() + written),
                              static_cast<std::streamsize>(chunk));
            const auto actual = static_cast<std::size_t>(cached.input.gcount());
            cached.position += actual;
            cached.position_valid = true;
            written += actual;
            disc_read_stats.bytes_from_files += actual;
            if (actual != chunk) {
                ++disc_read_stats.short_reads;
                // EOF/fail flags are expected after a short read; clear them so
                // a later explicit seek can recover this persistent handle.
                cached.input.clear();
                if (disc_read_diag_enabled()) {
                    std::cerr << "[disc-read] short read path=\"" << file->native_path.string()
                              << "\" wanted=" << chunk << " got=" << actual
                              << " file_offset=" << file_offset << "\n";
                }
                break;
            }
            continue;
        }

        std::uint64_t zero_end = handle.base_offset + handle.length;
        if (next != file_table.virtual_path_by_sector.end())
            zero_end = std::min(zero_end, static_cast<std::uint64_t>(next->first) * 2048u);
        if (zero_end <= absolute) zero_end = absolute + 1u;
        const std::size_t filled = static_cast<std::size_t>(std::min<std::uint64_t>(
            requested - written, zero_end - absolute));
        written += filled;
        disc_read_stats.bytes_zero_filled += filled;
        ++disc_read_stats.zero_fill_events;
        if (disc_read_diag_enabled() && disc_read_stats.reported_events < 40u) {
            ++disc_read_stats.reported_events;
            std::cerr << "[disc-read] zero-filled bytes=" << filled
                      << " absolute=" << absolute
                      << " sector=" << (absolute / 2048u)
                      << " handle_base=" << handle.base_offset
                      << " handle_pos=" << handle.position << "\n";
        }
    }
    handle.position += written;
    return written;
}

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

std::filesystem::path identify_atrac_source(std::span<const std::uint8_t> header, const ParsedAtracHeader &parsed) {
    const std::size_t compare_size = std::min<std::size_t>(header.size(), 256u);
    for (const auto &[key, file] : file_table.virtual_files_by_path) {
        if (file.size != parsed.file_size) continue;
        std::string extension = file.native_path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
        if (extension != ".AT3" && extension != ".AA3" && extension != ".OMA") continue;
        std::vector<std::uint8_t> candidate(compare_size);
        std::ifstream input(file.native_path, std::ios::binary);
        if (!input) continue;
        input.read(reinterpret_cast<char *>(candidate.data()), static_cast<std::streamsize>(candidate.size()));
        if (input.gcount() == static_cast<std::streamsize>(candidate.size()) &&
            std::equal(candidate.begin(), candidate.end(), header.begin())) return file.native_path;
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

ThreadTable thread_table;
PartitionTable partition_table;
CallbackTable callback_table;
SemaphoreTable semaphore_table;
EventFlagTable event_flag_table;
FixedPoolTable fixed_pool_table;
std::uint32_t compiled_sdk_version{};
std::uint32_t compiler_version{};
std::int32_t next_module_uid{0x400};
std::unordered_map<std::int32_t, bool> loaded_modules;
std::unordered_map<std::uint32_t, MpegContextState> mpeg_contexts;
std::array<AtracContextState, 6> atrac_contexts{};
std::uint32_t next_mpeg_stream_id{1u};
std::array<AudioChannelState, 9> audio_channels{};
std::uint64_t virtual_time_us{};

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
bool volatile_memory_locked{};
std::uint32_t general_purpose_io{};
std::uint32_t ge_edram_translation{};
struct GeCallbackRecord {
    std::uint32_t signal_function{};
    std::uint32_t signal_argument{};
    std::uint32_t finish_function{};
    std::uint32_t finish_argument{};
};
struct GeCallbackTable {
    std::int32_t next_uid{0};
    std::unordered_map<std::int32_t, GeCallbackRecord> callbacks;
};
GeCallbackTable ge_callback_table{};

enum class GeListState : std::uint32_t {
    None = 0u,
    Queued = 1u,
    Running = 2u,
    Completed = 3u,
    Paused = 4u,
    Stalled = 5u,
    Error = 6u,
};

struct GeStackEntry {
    std::uint32_t pc{};
    std::uint32_t offset_address{};
    std::uint32_t base_command{};
};

struct GeListRecord {
    std::uint32_t guest_id{};
    std::uint32_t start_pc{};
    std::uint32_t pc{};
    std::uint32_t stall{};
    std::int32_t callback_id{-1};
    std::uint32_t context_address{};
    std::uint32_t stack_address{};
    std::uint32_t stack_capacity{32u};
    GeListState state{GeListState::None};
    std::uint8_t signal_behavior{};
    std::uint16_t callback_token{};
    std::vector<GeStackEntry> stack;
    std::array<std::uint64_t, 256> histogram{};
    std::uint64_t executed_commands{};
    std::uint64_t primitive_commands{};

    // sceGeListEnQueue may supply a context buffer. The GE saves the current
    // global state before the list and restores it when the list completes.
    bool has_saved_context{};
    std::array<std::uint32_t, 256> saved_commands{};
    GeTransformState saved_transform{};
    std::uint32_t saved_offset_address{};
    std::uint32_t saved_vertex_address{};
    std::uint32_t saved_index_address{};
    bool saved_bounding_box_result{};
};

struct GeState {
    std::array<std::uint32_t, 256> commands{};
    GeTransformState transform{};
    std::uint32_t offset_address{};
    std::uint32_t vertex_address{};
    std::uint32_t index_address{};
    bool bounding_box_result{};
};

struct GeListTable {
    std::uint32_t next_raw_id{};
    std::unordered_map<std::uint32_t, GeListRecord> lists;
    std::vector<std::uint32_t> queue;
};

struct GuestCallbackInvocation {
    std::uint32_t function{};
    std::uint32_t a0{};
    std::uint32_t a1{};
    std::uint32_t a2{};
};

GeState ge_state{};
// Monotonic generation for GE draw-state commands. Vertex/index pointers and
// PRIM counts change almost every draw but do not alter Vulkan pipeline/texture
// state, so they are excluded below. The renderer uses this to reuse a decoded
// GeGpuDrawDescriptor across consecutive draws with identical state.
std::uint64_t ge_draw_state_revision = 1u;
// Lighting/material state is substantially more expensive to decode than the
// small draw descriptor, but is also much more stable across city geometry.
// Track it independently so the renderer can cache PreparedLighting without
// invalidating it for texture/blend/scissor or per-object matrix changes.
std::uint64_t ge_lighting_state_revision = 1u;
// Camera-only generation for Project2DFX. Unlike the generic draw revision,
// world/model matrix changes must NOT invalidate this: VCS updates those per
// object while the view/projection camera stays identical for hundreds of draws.
std::uint64_t ge_camera_state_revision = 1u;
GeListTable ge_list_table{};
std::unordered_map<std::int32_t, std::vector<GuestCallbackInvocation>> pending_guest_callbacks;

// Stage 45.7: the PSP GE is an independent processor.  Previous stages executed
// the complete display list inside sceGeListEnQueue(), serializing translated
// Allegrex work with vertex decode / draw preparation / DX12 accumulation.
// The async worker below turns enqueue into a producer operation and consumes
// lists on one dedicated host thread.  The worker remains strictly ordered (one
// GE command stream at a time), while the guest CPU can continue until an
// explicit GE sync or display-vblank visibility boundary requires completion.
struct GeAsyncTask {
    std::uint32_t id{};
    std::int32_t submitter_uid{};
    std::shared_ptr<std::atomic<std::uint32_t>> stall;
};
struct GeAsyncCompletion {
    std::int32_t submitter_uid{};
    std::vector<GuestCallbackInvocation> callbacks;
};
struct GeAsyncWorkerState {
    std::mutex mutex;
    std::condition_variable cv;
    std::thread thread;
    psprecomp::Runtime *runtime{};
    bool stop_requested{};
    std::atomic<bool> started{false};
    std::deque<GeAsyncTask> pending;
    std::unordered_map<std::uint32_t, std::shared_ptr<std::atomic<std::uint32_t>>> live_stalls;
    std::deque<GeAsyncCompletion> completions;
    std::atomic<std::uint32_t> outstanding{0u};
    std::atomic<std::uint32_t> completion_count{0u};
    std::atomic<std::uint64_t> last_wait_ns{0u};
    std::atomic<bool> fatal{false};
    std::string fatal_reason;
    std::uint64_t submitted{};
    std::uint64_t completed{};
    std::uint64_t wait_calls{};
    std::chrono::steady_clock::duration wait_time{};
};
GeAsyncWorkerState ge_async{};
thread_local bool ge_async_worker_thread = false;

bool ge_async_enabled() noexcept {
    static const bool enabled = [] {
        const char *value = std::getenv("PSPRECOMP_GE_ASYNC");
        return value != nullptr && *value != '\0' && std::strcmp(value, "0") != 0;
    }();
    return enabled;
}

bool ge_async_running() noexcept {
    return ge_async.started.load(std::memory_order_acquire);
}

void ge_async_worker_main();
void ge_async_drain_completions();
bool ge_async_wait_idle(psprecomp::Runtime &runtime);
bool ge_async_wait_list(psprecomp::Runtime &runtime, std::uint32_t id);
bool ge_async_check_fatal(psprecomp::Runtime &runtime);
void ge_async_stop_worker();

void ge_async_record_fatal(std::string reason) {
    {
        std::lock_guard lock(ge_async.mutex);
        if (ge_async.fatal_reason.empty()) ge_async.fatal_reason = std::move(reason);
    }
    ge_async.fatal.store(true, std::memory_order_release);
    ge_async.cv.notify_all();
}

void ge_execution_stop(psprecomp::Runtime &runtime, std::string reason) {
    if (ge_async_worker_thread) {
        ge_async_record_fatal(std::move(reason));
        return;
    }
    runtime.stop(std::move(reason));
}

void ge_async_start_worker(psprecomp::Runtime &runtime) {
    if (!ge_async_enabled()) return;
    std::lock_guard lock(ge_async.mutex);
    if (ge_async.started.load(std::memory_order_acquire)) return;
    ge_async.runtime = &runtime;
    ge_async.stop_requested = false;
    ge_async.started.store(true, std::memory_order_release);
    ge_async.thread = std::thread(&ge_async_worker_main);
}

struct GeAsyncLifetimeGuard {
    ~GeAsyncLifetimeGuard() { ge_async_stop_worker(); }
};
GeAsyncLifetimeGuard ge_async_lifetime_guard{};

constexpr std::array<std::pair<std::uint8_t, std::uint8_t>, 18> kGeContextCommandRanges{{
    {0x00u, 0x02u}, {0x10u, 0x10u}, {0x12u, 0x28u}, {0x2Cu, 0x33u},
    {0x36u, 0x38u}, {0x42u, 0x4Du}, {0x50u, 0x51u}, {0x53u, 0x58u},
    {0x5Bu, 0xB5u}, {0xB8u, 0xC3u}, {0xC5u, 0xD0u}, {0xD2u, 0xE9u},
    {0xEBu, 0xECu}, {0xEEu, 0xEEu}, {0xF0u, 0xF6u}, {0xF8u, 0xF9u},
    // Empty sentinels keep the table fixed-size and are skipped below.
    {0xFFu, 0x00u}, {0xFFu, 0x00u},
}};

std::uint32_t ge_float24_command(std::uint32_t command, float value) {
    return (command << 24u) | ((std::bit_cast<std::uint32_t>(value) >> 8u) & 0x00FFFFFFu);
}

bool ge_command_affects_lighting(std::uint32_t command) noexcept {
    return command == 0x17u ||
        (command >= 0x18u && command <= 0x1Bu) ||
        command == 0x53u ||
        (command >= 0x54u && command <= 0x58u) ||
        (command >= 0x5Bu && command <= 0x5Du) ||
        (command >= 0x5Fu && command <= 0x9Au);
}

void write_ge_context_buffer(psprecomp::Runtime &runtime, std::uint32_t address,
                             const GeState &state) {
    runtime.memory().zero(address, 512u * 4u);
    runtime.memory().store32(address + 5u * 4u, state.vertex_address);
    runtime.memory().store32(address + 6u * 4u, state.index_address);
    runtime.memory().store32(address + 7u * 4u, state.offset_address);

    std::uint32_t word = 17u;
    for (const auto [first, last] : kGeContextCommandRanges) {
        if (first > last) continue;
        for (std::uint32_t command = first; command <= last; ++command)
            runtime.memory().store32(address + word++ * 4u, state.commands[command]);
    }
    const auto save_matrix = [&](std::uint32_t number_command, std::uint32_t data_command,
                                 const auto &matrix) {
        runtime.memory().store32(address + word++ * 4u, number_command << 24u);
        for (float value : matrix)
            runtime.memory().store32(address + word++ * 4u, ge_float24_command(data_command, value));
    };
    save_matrix(0x2Au, 0x2Bu, state.transform.bones);
    save_matrix(0x3Au, 0x3Bu, state.transform.world);
    save_matrix(0x3Cu, 0x3Du, state.transform.view);
    save_matrix(0x3Eu, 0x3Fu, state.transform.projection);
    save_matrix(0x40u, 0x41u, state.transform.texture);
    runtime.memory().store32(address + word++ * 4u, (0x2Au << 24u) | (state.transform.bone_cursor & 0x7Fu));
    runtime.memory().store32(address + word++ * 4u, (0x3Au << 24u) | (state.transform.world_cursor & 0xFu));
    runtime.memory().store32(address + word++ * 4u, (0x3Cu << 24u) | (state.transform.view_cursor & 0xFu));
    runtime.memory().store32(address + word++ * 4u, (0x3Eu << 24u) | (state.transform.projection_cursor & 0xFu));
    runtime.memory().store32(address + word++ * 4u, (0x40u << 24u) | (state.transform.texture_cursor & 0xFu));
    runtime.memory().store32(address + word++ * 4u, 0x0C000000u);
}

void save_ge_list_context(psprecomp::Runtime &runtime, GeListRecord &record) {
    if (record.context_address == 0u) return;
    record.has_saved_context = true;
    record.saved_commands = ge_state.commands;
    record.saved_transform = ge_state.transform;
    record.saved_offset_address = ge_state.offset_address;
    record.saved_vertex_address = ge_state.vertex_address;
    record.saved_index_address = ge_state.index_address;
    record.saved_bounding_box_result = ge_state.bounding_box_result;
    write_ge_context_buffer(runtime, record.context_address, ge_state);
}

void restore_ge_list_context(const GeListRecord &record) {
    if (!record.has_saved_context) return;
    ge_state.commands = record.saved_commands;
    ++ge_draw_state_revision;
    ++ge_lighting_state_revision;
    ++ge_camera_state_revision;
    ge_state.transform = record.saved_transform;
    ge_state.offset_address = record.saved_offset_address;
    ge_state.vertex_address = record.saved_vertex_address;
    ge_state.index_address = record.saved_index_address;
    ge_state.bounding_box_result = record.saved_bounding_box_result;
}

enum class AsyncReturnKind : std::uint8_t {
    GeCallbackChain,
    SubInterrupt,
    MpegRingbuffer,
    UserCallback,
};

struct AsyncReturnFrame {
    AsyncReturnKind kind{AsyncReturnKind::GeCallbackChain};
    psprecomp::AllegrexContext resume{};
    std::uint32_t ring_address{};
    std::int32_t remaining_packets{};
    std::int32_t requested_this_round{};
    std::int32_t total_packets{};
    std::int32_t callback_uid{};
};

std::unordered_map<std::int32_t, std::vector<AsyncReturnFrame>> async_return_frames;

struct DisplayState {
    std::uint32_t mode{};
    std::uint32_t width{480u};
    std::uint32_t height{272u};
    std::uint32_t frame_buffer{};
    std::uint32_t buffer_width{512u};
    std::uint32_t pixel_format{3u};
    std::uint32_t sync_mode{};
};
DisplayState display_state{};
struct SubInterruptRecord {
    std::uint32_t handler{};
    std::uint32_t argument{};
    bool enabled{};
    bool occurred{};
};
std::unordered_map<std::uint64_t, SubInterruptRecord> sub_interrupts;
std::uint64_t sub_interrupt_key(std::uint32_t interrupt_number, std::uint32_t sub_number) {
    return (static_cast<std::uint64_t>(interrupt_number) << 32u) | sub_number;
}
std::uint32_t memory_stick_fat_state{1u};
struct ControllerState {
    std::uint32_t sampling_cycle{};
    std::uint32_t sampling_mode{};
    std::uint32_t buttons{};
    std::uint8_t lx{128u};
    std::uint8_t ly{128u};
    std::uint8_t rx{128u};
    std::uint8_t ry{128u};
};
ControllerState controller_state{};
std::uint64_t display_vblank_index{};

// Stage 9 diagnostic guard.  Setting PSPRECOMP_TIME_TICK_DISPATCHES=0 is useful
// for isolated scheduler/I/O ordering tests, but it deliberately disables the
// execution-driven PSP timer.  A polling thread can then keep delayed workers
// from ever reaching their deadlines.  Detect that configuration before it
// burns hundreds of millions of dispatches while appearing to be a game hang.
std::uint64_t execution_clock_dispatch_interval{256u};
std::uint64_t frozen_clock_guard_limit{5'000'000u};
std::uint64_t frozen_clock_guard_dispatches{};
std::uint64_t frozen_clock_guard_vblank{};

struct ControllerPulseConfig {
    std::uint32_t buttons{};
    std::uint64_t start_vblank{};
    std::uint64_t end_vblank{};
    std::uint8_t lx{128u};
    std::uint8_t ly{128u};
    bool has_lx{};
    bool has_ly{};
};

std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}

const std::array<ControllerPulseConfig, 8> &controller_pulse_configs() {
    static const std::array<ControllerPulseConfig, 8> configs = [] {
        std::array<ControllerPulseConfig, 8> values{};
        constexpr std::array<const char *, 8> suffixes{"", "2", "3", "4", "5", "6", "7", "8"};
        for (std::size_t index = 0; index < values.size(); ++index) {
            const std::string suffix = suffixes[index];
            const std::string prefix = "PSPRECOMP_CTRL_PULSE" + suffix;
            const std::string buttons_name = prefix + "_BUTTONS";
            const std::string start_name = prefix + "_START_VBLANK";
            const std::string end_name = prefix + "_END_VBLANK";
            const std::string lx_name = prefix + "_LX";
            const std::string ly_name = prefix + "_LY";
            ControllerPulseConfig &value = values[index];
            value.buttons = static_cast<std::uint32_t>(parse_environment_u64(buttons_name.c_str()));
            value.start_vblank = parse_environment_u64(start_name.c_str());
            value.end_vblank = parse_environment_u64(end_name.c_str(), value.start_vblank);
            if (value.end_vblank < value.start_vblank) value.end_vblank = value.start_vblank;
            if (std::getenv(lx_name.c_str()) != nullptr) {
                value.lx = static_cast<std::uint8_t>(std::min<std::uint64_t>(255u, parse_environment_u64(lx_name.c_str(), 128u)));
                value.has_lx = true;
            }
            if (std::getenv(ly_name.c_str()) != nullptr) {
                value.ly = static_cast<std::uint8_t>(std::min<std::uint64_t>(255u, parse_environment_u64(ly_name.c_str(), 128u)));
                value.has_ly = true;
            }
        }
        return values;
    }();
    return configs;
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

bool controller_pulse_active(const ControllerPulseConfig &pulse) {
    return display_vblank_index >= pulse.start_vblank && display_vblank_index <= pulse.end_vblank &&
           (pulse.buttons != 0u || pulse.has_lx || pulse.has_ly);
}

std::uint32_t effective_controller_buttons() {
    // Live keyboard input is ORed in; deterministic vblank pulses keep scripted
    // validation runs reproducible whether or not a window is open.
    std::uint32_t buttons = controller_state.buttons | display_window_buttons();
    for (const ControllerPulseConfig &pulse : controller_pulse_configs()) {
        if (controller_pulse_active(pulse)) buttons |= pulse.buttons;
    }
    return buttons;
}

std::pair<std::uint8_t, std::uint8_t> effective_controller_analog() {
    std::uint8_t analog_x = controller_state.lx;
    std::uint8_t analog_y = controller_state.ly;
    const HostInputState host = display_window_input();
    // The camera axes leave here rather than through sceCtrl: the PSP pad has
    // no field for them. The guest reads them through the hook in
    // generated_unit_0098.cpp.
    vcs_camera_set_axes(host.camera_x, host.camera_y);
    // Same reason: throttle and brake reach the guest through the vehicle's own
    // accessors, not through the pad's Cross and Square, so that W and S can
    // drive without also sprinting and jumping on foot.
    vcs_set_host_drive_inputs(host.accelerate, host.brake);
    const std::uint8_t window_x = host.analog_x;
    const std::uint8_t window_y = host.analog_y;
    if (window_x != 128u || window_y != 128u) {
        analog_x = window_x;
        analog_y = window_y;
    }
    // Later pulses intentionally win, allowing a scripted route to replace one
    // steering segment with the next while buttons remain independently ORed.
    for (const ControllerPulseConfig &pulse : controller_pulse_configs()) {
        if (!controller_pulse_active(pulse)) continue;
        if (pulse.has_lx) analog_x = pulse.lx;
        if (pulse.has_ly) analog_y = pulse.ly;
    }
    return {analog_x, analog_y};
}

enum class SasVoiceType : std::uint8_t {
    Off,
    Vag,
    Noise,
};

enum class SasEnvelopePhase : std::uint8_t {
    Attack,
    Decay,
    Sustain,
    Release,
    Off,
};

struct SasVoiceState {
    SasVoiceType type{SasVoiceType::Off};
    std::uint32_t data_address{};
    std::int32_t data_size{};
    bool loop{};
    std::int32_t noise_frequency{};
    std::int32_t pitch{0x1000};
    std::int32_t left_volume{};
    std::int32_t right_volume{};
    std::int32_t effect_left_volume{};
    std::int32_t effect_right_volume{};
    std::array<std::int32_t, 4> adsr_rates{};
    // Attack rises, everything else falls -- the same parity rule that
    // __sceSasSetADSRmode enforces on the game (even for attack, odd for decay
    // and release).  Defaulting all four to zero broke that rule: zero is
    // "linear increase", so a voice configured through __sceSasSetADSR alone,
    // which sets rates and never touches the modes, walked its release phase
    // *upward*.  The envelope pinned at maximum, `height <= 0` never happened,
    // and a keyed-off looping voice -- the vehicle engine -- kept sounding at
    // full volume under the pause menu.
    std::array<std::int32_t, 4> adsr_modes{0, 1, 1, 1};
    std::int32_t sustain_level{};
    std::uint32_t simple_adsr1{};
    std::uint32_t simple_adsr2{};
    bool adsr_configured{};
    SasEnvelopePhase envelope_phase{SasEnvelopePhase::Off};
    std::uint32_t key_on_delay_samples{};
    bool on{};
    bool playing{};
    bool paused{};
    std::uint32_t envelope_height{};
    std::uint64_t total_samples{};
    std::uint64_t remaining_samples{};

    // Stateful VAG decoder.  A voice is frequently re-used by the game, so a
    // KeyOn must rewind all of these fields rather than resuming at the end of
    // the previous sound.
    std::uint32_t decode_offset{};
    std::int32_t history1{};
    std::int32_t history2{};
    std::array<std::int16_t, 28> block_samples{};
    std::uint32_t block_position{28u};
    std::uint32_t loop_start_offset{};
    std::int32_t loop_start_history1{};
    std::int32_t loop_start_history2{};
    bool loop_start_valid{};
    bool finished{};

    // Pitch interpolation keeps a source sample pair alive across grain
    // boundaries.  The previous nearest-neighbour stepping clicked badly on
    // pitched engine/weapon/ambient effects.
    std::int16_t current_sample{};
    std::int16_t next_sample{};
    bool current_sample_valid{};
    bool next_sample_valid{};
    std::uint32_t pitch_accumulator{}; // 12-bit fraction, 0x1000 == one sample

    // Deterministic noise generator for the SAS noise-voice path.
    std::uint32_t noise_lfsr{0x13579BDFu};
    std::uint32_t noise_phase{};
    std::int16_t noise_sample{};
};
struct SasReverbState {
    std::int32_t type{-1};
    std::int32_t delay{};
    std::int32_t feedback{};
    std::uint32_t left_volume{};
    std::uint32_t right_volume{};
    // PSP SAS starts with the dry bus enabled.  Wet processing is opt-in.
    bool dry{true};
    bool wet{};
    // Persistent effect history.  This is intentionally owned by the SAS core
    // rather than rebuilt per grain so effect-only voices do not disappear at
    // grain boundaries and delay tails remain continuous.
    std::vector<std::int32_t> history_left;
    std::vector<std::int32_t> history_right;
    std::size_t history_cursor{};
};

struct SasState {
    bool initialized{};
    std::uint32_t core_address{};
    std::uint32_t grain_size{};
    std::uint32_t max_voices{32u};
    std::uint32_t output_mode{};
    std::uint32_t sample_rate{44100u};
    std::array<SasVoiceState, 32> voices{};
    SasReverbState reverb{};
};

SasState sas_state{};
std::uint64_t sas_core_mix_calls{};
std::uint64_t sas_core_with_mix_calls{};

bool sas_audio_diagnostics_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr ||
        std::getenv("PSPRECOMP_SAS_DIAG") != nullptr;
    return enabled;
}

std::size_t sas_playing_voice_count() {
    return static_cast<std::size_t>(std::count_if(
        sas_state.voices.begin(), sas_state.voices.end(),
        [](const SasVoiceState &voice) { return voice.playing && !voice.paused; }));
}

void sas_log_mix_checkpoint(const char *kind, std::uint64_t count) {
    if (!sas_audio_diagnostics_enabled()) return;
    if (count <= 8u || (count % 256u) == 0u) {
        std::cerr << "[sas] " << kind << " call=" << count
                  << " voices=" << sas_playing_voice_count()
                  << " dry=" << sas_state.reverb.dry
                  << " wet=" << sas_state.reverb.wet
                  << " effect_type=" << sas_state.reverb.type
                  << " grain=" << sas_state.grain_size << "\n";
    }
}

constexpr std::uint32_t kSasErrorInvalidGrain = 0x80420001u;
constexpr std::uint32_t kSasErrorInvalidMaxVoices = 0x80420002u;
constexpr std::uint32_t kSasErrorInvalidOutputMode = 0x80420003u;
constexpr std::uint32_t kSasErrorInvalidSampleRate = 0x80420004u;
constexpr std::uint32_t kSasErrorBadAddress = 0x80420005u;
constexpr std::uint32_t kSasErrorInvalidVoice = 0x80420010u;
constexpr std::uint32_t kSasErrorInvalidNoiseFrequency = 0x80420011u;
constexpr std::uint32_t kSasErrorInvalidPitch = 0x80420012u;
constexpr std::uint32_t kSasErrorInvalidAdsrMode = 0x80420013u;
constexpr std::uint32_t kSasErrorInvalidParameter = 0x80420014u;
constexpr std::uint32_t kSasErrorInvalidLoop = 0x80420015u;
constexpr std::uint32_t kSasErrorVoicePaused = 0x80420016u;
constexpr std::uint32_t kSasErrorInvalidVolume = 0x80420018u;
constexpr std::uint32_t kSasErrorInvalidAdsrRate = 0x80420019u;
constexpr std::uint32_t kSasErrorReverbType = 0x80420020u;
constexpr std::uint32_t kSasErrorReverbFeedback = 0x80420021u;
constexpr std::uint32_t kSasErrorReverbDelay = 0x80420022u;
constexpr std::uint32_t kSasErrorReverbVolume = 0x80420023u;
constexpr std::uint32_t kSasErrorNotInitialized = 0x80420100u;
constexpr std::uint32_t kSasEnvelopeMaximum = 0x40000000u;

bool sas_valid_core(std::uint32_t core) noexcept {
    return sas_state.initialized && core == sas_state.core_address;
}

SasVoiceState *sas_voice(std::uint32_t core, std::int32_t voice, psprecomp::AllegrexContext &ctx) {
    if (!sas_valid_core(core)) {
        ctx.set_gpr(2, kSasErrorNotInitialized);
        return nullptr;
    }
    if (voice < 0 || voice >= 32) {
        ctx.set_gpr(2, kSasErrorInvalidVoice);
        return nullptr;
    }
    return &sas_state.voices[static_cast<std::size_t>(voice)];
}

void sas_reset_voice_duration(SasVoiceState &voice) noexcept {
    if (voice.type == SasVoiceType::Vag && voice.data_size > 0) {
        // PSP VAG/ADPCM uses 16-byte blocks producing 28 PCM samples.  This is
        // an upper bound for streams with an earlier end marker, but it remains
        // useful for diagnostics and never drives end-of-voice by itself.
        voice.total_samples = static_cast<std::uint64_t>(voice.data_size / 16) * 28u;
        voice.remaining_samples = voice.total_samples;
    } else {
        voice.total_samples = 0u;
        voice.remaining_samples = 0u;
    }
}

void sas_reset_decoder(SasVoiceState &voice) noexcept {
    voice.decode_offset = 0u;
    voice.history1 = 0;
    voice.history2 = 0;
    voice.block_position = 28u;
    voice.loop_start_offset = 0u;
    voice.loop_start_history1 = 0;
    voice.loop_start_history2 = 0;
    voice.loop_start_valid = false;
    voice.finished = false;
    voice.current_sample = 0;
    voice.next_sample = 0;
    voice.current_sample_valid = false;
    voice.next_sample_valid = false;
    voice.pitch_accumulator = 0u;
    voice.noise_lfsr = 0x13579BDFu;
    voice.noise_phase = 0u;
    voice.noise_sample = 0;
    sas_reset_voice_duration(voice);
}

// When a title never configures ADSR, use a short de-click ramp rather than
// leaving the envelope at zero forever.  Once VCS supplies real SAS envelope
// parameters we follow those curves instead.
constexpr std::uint32_t kSasFallbackAttackSamples = 8u;
constexpr std::uint32_t kSasFallbackReleaseSamples = 32u;
constexpr std::uint32_t kSasFallbackAttackStep =
    kSasEnvelopeMaximum / kSasFallbackAttackSamples;
constexpr std::uint32_t kSasFallbackReleaseStep =
    kSasEnvelopeMaximum / kSasFallbackReleaseSamples;

std::int64_t sas_walk_envelope_curve(std::int64_t height, std::int32_t mode,
                                     std::int32_t rate) noexcept {
    const std::int64_t r = std::max<std::int64_t>(0, rate);
    switch (mode) {
    case 0: // linear increase
        return height + r;
    case 1: // linear decrease
        return height - r;
    case 2: // linear bent increase
        return height + (height <= static_cast<std::int64_t>(kSasEnvelopeMaximum) * 3 / 4
            ? r : r / 4);
    case 3: { // exponential decrease
        std::int64_t delta = height - static_cast<std::int64_t>(kSasEnvelopeMaximum);
        delta += ((-delta) * r) >> 32;
        return delta + kSasEnvelopeMaximum - (r + 3) / 4;
    }
    case 4: { // exponential increase
        std::int64_t delta = height - static_cast<std::int64_t>(kSasEnvelopeMaximum);
        delta += ((-delta) * r) >> 32;
        return delta + kSasEnvelopeMaximum + 0x4000;
    }
    case 5: // direct
        return r;
    default:
        return height;
    }
}

std::uint32_t sas_step_envelope(SasVoiceState &voice) noexcept {
    if (!voice.playing) return 0u;

    if (!voice.adsr_configured) {
        if (voice.on) {
            if (voice.envelope_height < kSasEnvelopeMaximum) {
                voice.envelope_height = std::min<std::uint32_t>(
                    kSasEnvelopeMaximum, voice.envelope_height + kSasFallbackAttackStep);
            }
        } else if (voice.envelope_height <= kSasFallbackReleaseStep) {
            voice.envelope_height = 0u;
            voice.envelope_phase = SasEnvelopePhase::Off;
            voice.playing = false;
        } else {
            voice.envelope_height -= kSasFallbackReleaseStep;
        }
        return voice.envelope_height;
    }

    // Real PSP SAS holds the envelope at zero briefly after KeyOn.  Keeping the
    // delay in samples (rather than grains) avoids a sharp transient and makes
    // the same voice deterministic at every grain size.
    if (voice.key_on_delay_samples != 0u) {
        --voice.key_on_delay_samples;
        voice.envelope_height = 0u;
        return 0u;
    }

    if (!voice.on && voice.envelope_phase != SasEnvelopePhase::Off)
        voice.envelope_phase = SasEnvelopePhase::Release;

    std::int64_t height = voice.envelope_height;
    switch (voice.envelope_phase) {
    case SasEnvelopePhase::Attack:
        height = sas_walk_envelope_curve(height, voice.adsr_modes[0], voice.adsr_rates[0]);
        if (height >= static_cast<std::int64_t>(kSasEnvelopeMaximum) || height < 0) {
            height = kSasEnvelopeMaximum;
            voice.envelope_phase = SasEnvelopePhase::Decay;
        }
        break;
    case SasEnvelopePhase::Decay:
        height = sas_walk_envelope_curve(height, voice.adsr_modes[1], voice.adsr_rates[1]);
        if (height <= voice.sustain_level) {
            height = std::max<std::int64_t>(0, voice.sustain_level);
            voice.envelope_phase = SasEnvelopePhase::Sustain;
        }
        break;
    case SasEnvelopePhase::Sustain:
        height = sas_walk_envelope_curve(height, voice.adsr_modes[2], voice.adsr_rates[2]);
        if (height <= 0) {
            height = 0;
            voice.envelope_phase = SasEnvelopePhase::Release;
        } else if (height > static_cast<std::int64_t>(kSasEnvelopeMaximum)) {
            height = kSasEnvelopeMaximum;
        }
        break;
    case SasEnvelopePhase::Release:
        height = sas_walk_envelope_curve(height, voice.adsr_modes[3], voice.adsr_rates[3]);
        if (height <= 0) {
            height = 0;
            voice.envelope_phase = SasEnvelopePhase::Off;
            voice.playing = false;
        }
        break;
    case SasEnvelopePhase::Off:
        height = 0;
        voice.playing = false;
        break;
    }

    height = std::clamp<std::int64_t>(height, 0, kSasEnvelopeMaximum);
    voice.envelope_height = static_cast<std::uint32_t>(height);
    return voice.envelope_height;
}

std::int32_t sas_simple_rate(std::uint32_t value) noexcept {
    value &= 0x7Fu;
    if (value == 0x7Fu) return 0;
    const std::uint64_t base = static_cast<std::uint64_t>(7u - (value & 3u)) << 26u;
    const std::uint64_t rate = base >> (value >> 2u);
    return static_cast<std::int32_t>(std::max<std::uint64_t>(1u, rate));
}

std::int32_t sas_exponent_rate(std::uint32_t value) noexcept {
    value &= 0x7Fu;
    if (value == 0x7Fu) return 0;
    const std::uint64_t base = static_cast<std::uint64_t>(7u - (value & 3u)) << 24u;
    const std::uint64_t rate = base >> (value >> 2u);
    return static_cast<std::int32_t>(std::max<std::uint64_t>(1u, rate));
}

void sas_decode_simple_adsr(SasVoiceState &voice) noexcept {
    const std::uint32_t a1 = voice.simple_adsr1;
    const std::uint32_t a2 = voice.simple_adsr2;
    voice.adsr_rates[0] = sas_simple_rate(a1 >> 8u);
    voice.adsr_modes[0] = (a1 & 0x8000u) == 0u ? 0 : 2;

    const std::uint32_t decay = (a1 >> 4u) & 0x0Fu;
    voice.adsr_rates[1] = decay == 0u ? 0x7FFFFFFF :
        static_cast<std::int32_t>(0x80000000u >> decay);
    voice.adsr_modes[1] = 3;

    voice.adsr_modes[2] = static_cast<std::int32_t>((a2 >> 14u) & 3u);
    voice.adsr_rates[2] = voice.adsr_modes[2] == 3
        ? sas_exponent_rate(a2 >> 6u) : sas_simple_rate(a2 >> 6u);

    const std::uint32_t release = a2 & 0x1Fu;
    voice.adsr_modes[3] = (a2 & 0x20u) == 0u ? 1 : 3;
    if (release == 31u) {
        voice.adsr_rates[3] = 0;
    } else if (voice.adsr_modes[3] == 1) {
        if (release == 30u) voice.adsr_rates[3] = 0x40000000;
        else if (release == 29u) voice.adsr_rates[3] = 1;
        else voice.adsr_rates[3] = static_cast<std::int32_t>(0x10000000u >> release);
    } else {
        voice.adsr_rates[3] = release == 0u ? 0x7FFFFFFF :
            static_cast<std::int32_t>(0x80000000u >> release);
    }

    voice.sustain_level = static_cast<std::int32_t>(((a1 & 0x0Fu) + 1u) << 26u);
    voice.adsr_configured = true;
}

// PSP VAG is the PS1 ADPCM format: 16-byte blocks holding 28 four-bit samples
// plus a shift/filter byte and a flags byte. The predictor coefficients are the
// standard table, scaled by 64.
constexpr std::int32_t kVagFilter0[16] = {
    0, 60, 115, 98, 122, 0, 0, 52, 55, 60, 0, 0, 0, 2, 125, 0
};
constexpr std::int32_t kVagFilter1[16] = {
    0, 0, -52, -55, -60, 0, 0, 0, -2, -125, 0, -91, 0, -216, -6, -151
};

// Decodes the next 16-byte block into voice.block_samples.  Loop markers follow
// the PSP SAS convention used by VCS: 6 marks the loop start, 3 the loop end,
// and 7 the terminal block.  Marker 1 is also accepted as a terminal block for
// ordinary PSX-style VAG assets.
bool sas_decode_next_block(const psprecomp::GuestMemory &memory, SasVoiceState &voice) {
    if (voice.data_address == 0u || voice.data_size <= 0 || voice.finished) return false;

    const auto rewind_loop = [&]() {
        // PSP SAS keeps the ADPCM predictor history across a loop jump.
        // Resetting it at the marker makes otherwise seamless ambient loops
        // click every time they wrap.
        voice.decode_offset = voice.loop_start_valid ? voice.loop_start_offset : 0u;
        voice.remaining_samples = voice.total_samples;
    };

    if (voice.decode_offset + 16u > static_cast<std::uint32_t>(voice.data_size)) {
        if (!voice.loop) return false;
        rewind_loop();
    }
    const std::uint32_t relative_offset = voice.decode_offset;
    const std::uint32_t base = voice.data_address + relative_offset;
    if (!memory.contains(base, 16u)) return false;

    const std::uint32_t header = memory.aot_load8(base);
    const std::uint32_t flags = memory.aot_load8(base + 1u);
    const std::int32_t history_before_1 = voice.history1;
    const std::int32_t history_before_2 = voice.history2;
    std::int32_t shift = static_cast<std::int32_t>(header & 0x0Fu);
    std::int32_t filter = static_cast<std::int32_t>((header >> 4u) & 0x0Fu);
    // All four predictor bits and all four shift bits are meaningful on the
    // PSP SAS decoder.
    filter &= 0x0F;
    shift &= 0x0F;

    // Flag 7 is a terminal marker block; it does not contribute 28 samples.
    if (flags == 7u) {
        voice.finished = true;
        return false;
    }

    for (std::uint32_t index = 0u; index < 28u; ++index) {
        const std::uint32_t byte = memory.aot_load8(base + 2u + index / 2u);
        const std::uint32_t nibble = (index & 1u) != 0u ? (byte >> 4u) : (byte & 0x0Fu);
        std::int32_t sample = static_cast<std::int32_t>(nibble << 12u);
        if (sample & 0x8000) sample = static_cast<std::int32_t>(sample | 0xFFFF0000u);
        sample >>= shift;
        sample += (voice.history1 * kVagFilter0[filter] +
                   voice.history2 * kVagFilter1[filter] + 32) / 64;
        sample = std::clamp(sample, -32768, 32767);
        voice.block_samples[index] = static_cast<std::int16_t>(sample);
        voice.history2 = voice.history1;
        voice.history1 = sample;
    }

    if (flags == 6u) {
        // Re-entering the loop must restore the predictor state from immediately
        // before the loop-start block, otherwise each pass drifts/clicks.
        voice.loop_start_offset = relative_offset;
        voice.loop_start_history1 = history_before_1;
        voice.loop_start_history2 = history_before_2;
        voice.loop_start_valid = true;
    }

    voice.decode_offset += 16u;
    voice.block_position = 0u;

    if (flags == 3u) {
        if (voice.loop) rewind_loop();
        else voice.finished = true;
    } else if (flags == 1u) {
        voice.finished = true;
    }
    return true;
}

bool sas_fetch_vag_sample(const psprecomp::GuestMemory &memory, SasVoiceState &voice,
                          std::int16_t &sample) {
    if (voice.block_position >= 28u) {
        if (voice.finished || !sas_decode_next_block(memory, voice)) return false;
    }
    sample = voice.block_samples[voice.block_position++];
    if (voice.remaining_samples != 0u) --voice.remaining_samples;
    return true;
}

bool sas_prepare_sample_pair(const psprecomp::GuestMemory &memory, SasVoiceState &voice) {
    if (!voice.current_sample_valid) {
        if (!sas_fetch_vag_sample(memory, voice, voice.current_sample)) return false;
        voice.current_sample_valid = true;
    }
    if (!voice.next_sample_valid) {
        std::int16_t next{};
        if (sas_fetch_vag_sample(memory, voice, next)) {
            voice.next_sample = next;
            voice.next_sample_valid = true;
        }
    }
    return true;
}

std::int32_t sas_render_vag_sample(const psprecomp::GuestMemory &memory, SasVoiceState &voice) {
    if (!sas_prepare_sample_pair(memory, voice)) {
        voice.playing = false;
        voice.on = false;
        voice.envelope_height = 0u;
        return 0;
    }

    const std::int32_t current = voice.current_sample;
    const std::int32_t next = voice.next_sample_valid ? voice.next_sample : current;
    const std::int32_t sample = current +
        ((next - current) * static_cast<std::int32_t>(voice.pitch_accumulator)) / 0x1000;

    const std::uint32_t pitch = voice.pitch < 0 ? 0u : static_cast<std::uint32_t>(voice.pitch);
    voice.pitch_accumulator += pitch;
    while (voice.pitch_accumulator >= 0x1000u && voice.playing) {
        voice.pitch_accumulator -= 0x1000u;
        if (!voice.next_sample_valid) {
            // The current sample was the last one.  It has just been rendered;
            // retire the voice without creating a discontinuous extra zero.
            voice.playing = false;
            voice.on = false;
            voice.envelope_height = 0u;
            break;
        }
        voice.current_sample = voice.next_sample;
        voice.current_sample_valid = true;
        std::int16_t following{};
        if (sas_fetch_vag_sample(memory, voice, following)) {
            voice.next_sample = following;
            voice.next_sample_valid = true;
        } else {
            voice.next_sample_valid = false;
        }
    }
    return sample;
}

std::int32_t sas_render_noise_sample(SasVoiceState &voice) noexcept {
    // Map 0..63 to progressively faster LFSR updates.  Exact spectral shaping is
    // hardware-specific, but implementing the path is crucial: VCS uses SAS
    // noise voices for effects that were previously completely silent.
    voice.noise_phase += static_cast<std::uint32_t>(voice.noise_frequency + 1);
    while (voice.noise_phase >= 64u) {
        voice.noise_phase -= 64u;
        const std::uint32_t feedback =
            ((voice.noise_lfsr >> 0u) ^ (voice.noise_lfsr >> 1u) ^
             (voice.noise_lfsr >> 21u) ^ (voice.noise_lfsr >> 31u)) & 1u;
        voice.noise_lfsr = (voice.noise_lfsr >> 1u) | (feedback << 31u);
        voice.noise_sample = (voice.noise_lfsr & 1u) != 0u ? 12288 : -12288;
    }
    return voice.noise_sample;
}

// Renders one voice into separate dry and effect-send accumulators.  PSP SAS
// has two independent volume pairs per voice.  The old HLE threw the effect
// pair away entirely, so any sound routed only to the wet bus was silent.
void sas_render_voice(const psprecomp::GuestMemory &memory, SasVoiceState &voice,
                      std::vector<std::int32_t> &dry_mix,
                      std::vector<std::int32_t> &effect_send,
                      std::uint32_t frames) {
    if (!voice.playing || voice.paused || voice.type == SasVoiceType::Off) return;

    for (std::uint32_t frame = 0u; frame < frames && voice.playing; ++frame) {
        const std::uint32_t envelope = sas_step_envelope(voice);
        if (!voice.playing || envelope == 0u) continue;

        std::int32_t sample = 0;
        if (voice.type == SasVoiceType::Vag)
            sample = sas_render_vag_sample(memory, voice);
        else if (voice.type == SasVoiceType::Noise)
            sample = sas_render_noise_sample(voice);

        const auto accumulate = [&](std::vector<std::int32_t> &target,
                                    std::int32_t left_volume,
                                    std::int32_t right_volume) {
            const std::int64_t left_gain =
                (static_cast<std::int64_t>(left_volume) * envelope) >> 30;
            const std::int64_t right_gain =
                (static_cast<std::int64_t>(right_volume) * envelope) >> 30;
            target[frame * 2u] += static_cast<std::int32_t>((sample * left_gain) >> 12);
            target[frame * 2u + 1u] += static_cast<std::int32_t>((sample * right_gain) >> 12);
        };
        accumulate(dry_mix, voice.left_volume, voice.right_volume);
        accumulate(effect_send, voice.effect_left_volume, voice.effect_right_volume);
    }
}

void sas_render_buses(const psprecomp::GuestMemory &memory, std::uint32_t frames,
                      std::vector<std::int32_t> &dry_mix,
                      std::vector<std::int32_t> &effect_send) {
    dry_mix.assign(static_cast<std::size_t>(frames) * 2u, 0);
    effect_send.assign(static_cast<std::size_t>(frames) * 2u, 0);
    for (auto &voice : sas_state.voices)
        sas_render_voice(memory, voice, dry_mix, effect_send, frames);
}

// A conservative host-side recreation of the SAS effect-send path.  The exact
// PSP presets are hardware-specific, but the important observable semantics are
// preserved here: effectLeft/effectRight feed a persistent wet bus, RevVON can
// select dry/wet independently, RevEVOL scales wet output, and delay/feedback
// persist between grains.  Type OFF is a transparent wet send rather than a
// destructive silent sink; this avoids dropping effect-routed SFX while still
// keeping the default (dry on, wet off) bit-for-bit simple.
void sas_process_effect_send(const std::vector<std::int32_t> &effect_send,
                             std::vector<std::int32_t> &wet_mix,
                             std::uint32_t frames) {
    wet_mix.assign(static_cast<std::size_t>(frames) * 2u, 0);
    if (!sas_state.reverb.wet) return;

    const std::int64_t global_left = sas_state.reverb.left_volume;
    const std::int64_t global_right = sas_state.reverb.right_volume;
    if (sas_state.reverb.type < 0) {
        for (std::uint32_t frame = 0u; frame < frames; ++frame) {
            wet_mix[frame * 2u] = static_cast<std::int32_t>(
                (static_cast<std::int64_t>(effect_send[frame * 2u]) * global_left) >> 12);
            wet_mix[frame * 2u + 1u] = static_cast<std::int32_t>(
                (static_cast<std::int64_t>(effect_send[frame * 2u + 1u]) * global_right) >> 12);
        }
        return;
    }

    // Long enough for every legal delay parameter while keeping the history
    // tiny compared with the rest of the guest runtime.  Preset type changes
    // the base spacing; delay and feedback remain the caller-controlled knobs.
    constexpr std::size_t kEffectHistoryFrames = 16384u;
    auto &reverb = sas_state.reverb;
    if (reverb.history_left.size() != kEffectHistoryFrames) {
        reverb.history_left.assign(kEffectHistoryFrames, 0);
        reverb.history_right.assign(kEffectHistoryFrames, 0);
        reverb.history_cursor = 0u;
    }
    const std::size_t type_offset = static_cast<std::size_t>(std::clamp(reverb.type, 0, 8)) * 73u;
    const std::size_t delay_frames = std::clamp<std::size_t>(
        64u + type_offset + static_cast<std::size_t>(reverb.delay) * 24u,
        1u, kEffectHistoryFrames - 1u);
    const std::int64_t feedback = std::clamp<std::int32_t>(reverb.feedback, 0, 127);

    for (std::uint32_t frame = 0u; frame < frames; ++frame) {
        const std::size_t read_index =
            (reverb.history_cursor + kEffectHistoryFrames - delay_frames) % kEffectHistoryFrames;
        const std::int64_t delayed_left = reverb.history_left[read_index];
        const std::int64_t delayed_right = reverb.history_right[read_index];
        const std::int64_t input_left = effect_send[frame * 2u];
        const std::int64_t input_right = effect_send[frame * 2u + 1u];

        // Wet output includes the current send plus one delayed component.  The
        // feedback line itself is bounded to signed 24-bit-ish headroom so a
        // pathological guest setting cannot accumulate indefinitely.
        const std::int64_t effect_left = input_left + delayed_left;
        const std::int64_t effect_right = input_right + delayed_right;
        const std::int64_t next_left = input_left + (delayed_left * feedback) / 128;
        const std::int64_t next_right = input_right + (delayed_right * feedback) / 128;
        reverb.history_left[reverb.history_cursor] = static_cast<std::int32_t>(
            std::clamp<std::int64_t>(next_left, -0x7FFFFF, 0x7FFFFF));
        reverb.history_right[reverb.history_cursor] = static_cast<std::int32_t>(
            std::clamp<std::int64_t>(next_right, -0x7FFFFF, 0x7FFFFF));
        reverb.history_cursor = (reverb.history_cursor + 1u) % kEffectHistoryFrames;

        wet_mix[frame * 2u] = static_cast<std::int32_t>((effect_left * global_left) >> 12);
        wet_mix[frame * 2u + 1u] = static_cast<std::int32_t>((effect_right * global_right) >> 12);
    }
}

// Mixed output mode: CoreWithMix starts from caller PCM, then the selected SAS
// dry/wet buses are added.  Plain Core uses the same path without caller input.
void sas_mix_into(psprecomp::Runtime &rt, std::uint32_t output, std::uint32_t frames,
                  bool include_input = false,
                  std::uint32_t input_left = 0x1000u,
                  std::uint32_t input_right = 0x1000u) {
    static thread_local std::vector<std::int32_t> dry_mix;
    static thread_local std::vector<std::int32_t> effect_send;
    static thread_local std::vector<std::int32_t> wet_mix;
    sas_render_buses(rt.memory(), frames, dry_mix, effect_send);
    sas_process_effect_send(effect_send, wet_mix, frames);

    for (std::uint32_t frame = 0u; frame < frames; ++frame) {
        std::int64_t l = 0;
        std::int64_t r = 0;
        if (include_input) {
            const auto input_l = static_cast<std::int16_t>(
                rt.memory().aot_load16(output + frame * 4u));
            const auto input_r = static_cast<std::int16_t>(
                rt.memory().aot_load16(output + frame * 4u + 2u));
            l += (static_cast<std::int64_t>(input_l) * input_left) >> 12;
            r += (static_cast<std::int64_t>(input_r) * input_right) >> 12;
        }
        if (sas_state.reverb.dry) {
            l += dry_mix[frame * 2u];
            r += dry_mix[frame * 2u + 1u];
        }
        if (sas_state.reverb.wet) {
            l += wet_mix[frame * 2u];
            r += wet_mix[frame * 2u + 1u];
        }
        rt.memory().store16(output + frame * 4u, static_cast<std::uint16_t>(
            static_cast<std::int16_t>(std::clamp<std::int64_t>(l, -32768, 32767))));
        rt.memory().store16(output + frame * 4u + 2u, static_cast<std::uint16_t>(
            static_cast<std::int16_t>(std::clamp<std::int64_t>(r, -32768, 32767))));
    }
}

// Raw output mode exposes four non-interleaved planes: dry L, dry R, effect L,
// effect R.  The previous HLE incorrectly treated mode 1 as mono and allocated
// only two bytes per frame, which could both lose send-routed sounds and write
// the wrong guest buffer layout.
void sas_mix_raw(psprecomp::Runtime &rt, std::uint32_t output, std::uint32_t frames) {
    static thread_local std::vector<std::int32_t> dry_mix;
    static thread_local std::vector<std::int32_t> effect_send;
    sas_render_buses(rt.memory(), frames, dry_mix, effect_send);
    const std::uint32_t left_base = output;
    const std::uint32_t right_base = output + frames * 2u;
    const std::uint32_t send_left_base = output + frames * 4u;
    const std::uint32_t send_right_base = output + frames * 6u;
    for (std::uint32_t frame = 0u; frame < frames; ++frame) {
        const auto store = [&](std::uint32_t base, std::int32_t value) {
            rt.memory().store16(base + frame * 2u, static_cast<std::uint16_t>(
                static_cast<std::int16_t>(std::clamp(value, -32768, 32767))));
        };
        store(left_base, dry_mix[frame * 2u]);
        store(right_base, dry_mix[frame * 2u + 1u]);
        store(send_left_base, effect_send[frame * 2u]);
        store(send_right_base, effect_send[frame * 2u + 1u]);
    }
}

enum class UtilityStatus : std::uint32_t {
    None = 0u,
    Init = 1u,
    Visible = 2u,
    Quit = 3u,
    Finished = 4u,
};

struct SavedataUtilityState {
    UtilityStatus status{UtilityStatus::None};
    std::uint32_t parameter_address{};
    bool operation_complete{};
};

SavedataUtilityState savedata_utility{};

constexpr std::uint32_t kUtilityCommonResultOffset = 0x1Cu;
constexpr std::uint32_t kSavedataModeOffset = 0x30u;
constexpr std::uint32_t kSavedataGameNameOffset = 0x3Cu;
constexpr std::uint32_t kSavedataSaveNameOffset = 0x4Cu;
constexpr std::uint32_t kSavedataFileNameOffset = 0x64u;
constexpr std::uint32_t kSavedataDataBufferOffset = 0x74u;
constexpr std::uint32_t kSavedataDataBufferSizeOffset = 0x78u;
constexpr std::uint32_t kSavedataDataSizeOffset = 0x7Cu;
constexpr std::uint32_t kSavedataIcon0Offset = 0x584u;
constexpr std::uint32_t kSavedataIcon1Offset = 0x594u;
constexpr std::uint32_t kSavedataPic1Offset = 0x5A4u;
constexpr std::uint32_t kSavedataSnd0Offset = 0x5B4u;
constexpr std::uint32_t kSavedataIdListOffset = 0x5F4u;
constexpr std::uint32_t kSavedataFileListOffset = 0x5F8u;
constexpr std::uint32_t kSavedataSizeInfoOffset = 0x5FCu;
constexpr std::uint32_t kSavedataParameterMinimumSize = 0x600u;

std::string read_fixed_string(const psprecomp::GuestMemory &memory, std::uint32_t address, std::size_t size) {
    std::string result;
    result.reserve(size);
    for (std::size_t index = 0; index < size; ++index) {
        const char value = static_cast<char>(memory.load8(address + static_cast<std::uint32_t>(index)));
        if (value == '\0') break;
        result.push_back(value);
    }
    return result;
}

std::string safe_savedata_component(std::string value) {
    value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char c) {
        return c == '/' || c == '\\' || c == ':' || c < 0x20u;
    }), value.end());
    return value;
}

std::filesystem::path savedata_root(const psprecomp::Runtime &runtime) {
    // Beside the executable, not inside the game data. A player pointing the
    // runtime at an extracted disc should not have to dig through it to find
    // their progress, and that copy may well be read-only or shared between
    // builds. Falls back to the game root when the configuration has not been
    // initialized, which is only the case in tests.
    const ::vcs::VcsConfiguration &config = ::vcs::vcs_configuration();
    if (config.initialized && !config.executable_directory.empty())
        return config.executable_directory / "SAVEDATA";
    return runtime.game_root() / "PSP" / "SAVEDATA";
}

std::filesystem::path savedata_directory(const psprecomp::Runtime &runtime, std::uint32_t parameter_address) {
    const std::string game = safe_savedata_component(read_fixed_string(
        runtime.memory(), parameter_address + kSavedataGameNameOffset, 13u));
    const std::string save = safe_savedata_component(read_fixed_string(
        runtime.memory(), parameter_address + kSavedataSaveNameOffset, 20u));
    return savedata_root(runtime) / (game + save);
}

bool write_guest_file(psprecomp::Runtime &runtime, const std::filesystem::path &path,
                      std::uint32_t buffer, std::uint32_t size) {
    if (size == 0u) return true;
    if (buffer == 0u || !runtime.memory().contains(buffer, size)) return false;
    std::filesystem::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) return false;
    std::vector<std::uint8_t> data(size);
    for (std::uint32_t index = 0u; index < size; ++index)
        data[index] = runtime.memory().load8(buffer + index);
    output.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));
    return output.good();
}

bool write_savedata_auxiliary(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                              std::uint32_t descriptor_offset, const char *filename) {
    const std::uint32_t descriptor = parameter_address + descriptor_offset;
    const std::uint32_t buffer = runtime.memory().load32(descriptor);
    const std::uint32_t buffer_size = runtime.memory().load32(descriptor + 4u);
    const std::uint32_t actual_size = runtime.memory().load32(descriptor + 8u);
    if (buffer == 0u || actual_size == 0u) return true;
    if (actual_size > buffer_size) return false;
    return write_guest_file(runtime, savedata_directory(runtime, parameter_address) / filename, buffer, actual_size);
}

std::uint32_t load_savedata_file(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                                 const std::filesystem::path &path, bool raw_mode) {
    if (!std::filesystem::is_regular_file(path)) {
        return raw_mode ? 0x80110329u : 0x80110307u;
    }
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) return raw_mode ? 0x80110329u : 0x80110305u;
    const auto end = input.tellg();
    if (end < 0) return 0x80110305u;
    const auto file_size = static_cast<std::uint64_t>(end);
    const std::uint32_t destination = runtime.memory().load32(parameter_address + kSavedataDataBufferOffset);
    const std::uint32_t capacity = runtime.memory().load32(parameter_address + kSavedataDataBufferSizeOffset);
    if (file_size > capacity || file_size > 0xFFFFFFFFull ||
        (file_size != 0u && (destination == 0u || !runtime.memory().contains(destination, static_cast<std::size_t>(file_size))))) {
        return raw_mode ? 0x80110328u : 0x80110308u;
    }
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(file_size));
    input.seekg(0, std::ios::beg);
    if (!bytes.empty()) input.read(reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!input && !bytes.empty()) return 0x80110305u;
    if (!bytes.empty()) runtime.memory().copy_in(destination, bytes);
    runtime.memory().store32(parameter_address + kSavedataDataSizeOffset, static_cast<std::uint32_t>(bytes.size()));
    return 0u;
}

std::uint32_t save_savedata_file(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                                 const std::filesystem::path &path, bool raw_mode) {
    const std::uint32_t source = runtime.memory().load32(parameter_address + kSavedataDataBufferOffset);
    const std::uint32_t capacity = runtime.memory().load32(parameter_address + kSavedataDataBufferSizeOffset);
    const std::uint32_t size = runtime.memory().load32(parameter_address + kSavedataDataSizeOffset);
    if (size > capacity || (size != 0u && (source == 0u || !runtime.memory().contains(source, size)))) {
        return raw_mode ? 0x80110328u : 0x80110388u;
    }
    if (!write_guest_file(runtime, path, source, size)) return raw_mode ? 0x80110329u : 0x80110385u;
    if (!raw_mode) {
        if (!write_savedata_auxiliary(runtime, parameter_address, kSavedataIcon0Offset, "ICON0.PNG") ||
            !write_savedata_auxiliary(runtime, parameter_address, kSavedataIcon1Offset, "ICON1.PMF") ||
            !write_savedata_auxiliary(runtime, parameter_address, kSavedataPic1Offset, "PIC1.PNG") ||
            !write_savedata_auxiliary(runtime, parameter_address, kSavedataSnd0Offset, "SND0.AT3")) {
            return 0x80110385u;
        }
    }
    return 0u;
}

std::uint32_t list_savedata_directories(psprecomp::Runtime &runtime, std::uint32_t parameter_address) {
    const std::uint32_t info = runtime.memory().load32(parameter_address + kSavedataIdListOffset);
    if (info == 0u || !runtime.memory().contains(info, 12u)) return 0x80110328u;
    const std::int32_t max_count = static_cast<std::int32_t>(runtime.memory().load32(info));
    const std::uint32_t entries = runtime.memory().load32(info + 8u);
    if (max_count < 0 || (max_count > 0 && (entries == 0u || !runtime.memory().contains(entries, static_cast<std::size_t>(max_count) * 72u)))) {
        return 0x80110328u;
    }
    const std::string game = safe_savedata_component(read_fixed_string(
        runtime.memory(), parameter_address + kSavedataGameNameOffset, 13u));
    std::vector<std::string> names;
    const auto root = savedata_root(runtime);
    if (std::filesystem::is_directory(root)) {
        for (const auto &entry : std::filesystem::directory_iterator(root)) {
            if (!entry.is_directory()) continue;
            const std::string directory_name = entry.path().filename().string();
            if (!directory_name.starts_with(game)) continue;
            names.push_back(directory_name.substr(game.size()));
        }
    }
    std::sort(names.begin(), names.end());
    if (names.size() > static_cast<std::size_t>(max_count)) names.resize(static_cast<std::size_t>(max_count));
    for (std::size_t index = 0; index < names.size(); ++index) {
        const std::uint32_t entry = entries + static_cast<std::uint32_t>(index * 72u);
        runtime.memory().zero(entry, 72u);
        runtime.memory().store32(entry, 0x11FFu);
        std::vector<std::uint8_t> bytes(names[index].begin(), names[index].end());
        if (bytes.size() > 19u) bytes.resize(19u);
        bytes.push_back(0u);
        runtime.memory().copy_in(entry + 52u, bytes);
    }
    runtime.memory().store32(info + 4u, static_cast<std::uint32_t>(names.size()));
    return 0u;
}

std::uint64_t directory_size_bytes(const std::filesystem::path &directory) {
    std::uint64_t total = 0u;
    if (!std::filesystem::is_directory(directory)) return total;
    std::error_code error;
    for (std::filesystem::recursive_directory_iterator it(directory, error), end; it != end && !error; it.increment(error)) {
        if (it->is_regular_file(error)) total += it->file_size(error);
    }
    return total;
}

void write_small_size_string(psprecomp::GuestMemory &memory, std::uint32_t address, std::uint64_t kilobytes) {
    const std::string text = kilobytes > 99999u ? "99999KB" : std::to_string(kilobytes) + "KB";
    memory.zero(address, 8u);
    std::vector<std::uint8_t> bytes(text.begin(), text.end());
    if (bytes.size() > 7u) bytes.resize(7u);
    bytes.push_back(0u);
    memory.copy_in(address, bytes);
}

void write_used_data_info(psprecomp::GuestMemory &memory, std::uint32_t address,
                          std::uint64_t used_bytes, std::uint32_t cluster_size) {
    const std::uint64_t clusters = (used_bytes + cluster_size - 1u) / cluster_size;
    const std::uint64_t used_kb = (used_bytes + 1023u) / 1024u;
    const std::uint64_t used_32kb = clusters * (cluster_size / 1024u);
    memory.store32(address + 0u, static_cast<std::uint32_t>(std::min<std::uint64_t>(clusters, 0xFFFFFFFFull)));
    memory.store32(address + 4u, static_cast<std::uint32_t>(std::min<std::uint64_t>(used_kb, 0xFFFFFFFFull)));
    write_small_size_string(memory, address + 8u, used_kb);
    memory.store32(address + 16u, static_cast<std::uint32_t>(std::min<std::uint64_t>(used_32kb, 0xFFFFFFFFull)));
    write_small_size_string(memory, address + 20u, used_32kb);
}

std::uint32_t query_savedata_sizes(psprecomp::Runtime &runtime, std::uint32_t parameter_address) {
    constexpr std::uint32_t cluster_size = 32u * 1024u;
    const auto root = savedata_root(runtime);
    std::error_code error;
    std::filesystem::create_directories(root, error);
    const auto space = std::filesystem::space(root, error);
    const std::uint64_t available = error ? 512ull * 1024ull * 1024ull : space.available;
    const std::uint64_t free_clusters = available / cluster_size;
    const std::uint64_t free_kb = available / 1024u;
    const std::uint64_t used = directory_size_bytes(savedata_directory(runtime, parameter_address));

    const std::uint32_t ms_free = runtime.memory().load32(parameter_address + 0x5D0u);
    if (ms_free != 0u) {
        if (!runtime.memory().contains(ms_free, 20u)) return 0x801103C8u;
        runtime.memory().store32(ms_free + 0u, cluster_size);
        runtime.memory().store32(ms_free + 4u, static_cast<std::uint32_t>(std::min<std::uint64_t>(free_clusters, 0xFFFFFFFFull)));
        runtime.memory().store32(ms_free + 8u, static_cast<std::uint32_t>(std::min<std::uint64_t>(free_kb, 0xFFFFFFFFull)));
        write_small_size_string(runtime.memory(), ms_free + 12u, free_kb);
    }

    const std::uint32_t ms_data = runtime.memory().load32(parameter_address + 0x5D4u);
    if (ms_data != 0u) {
        if (!runtime.memory().contains(ms_data, 64u)) return 0x801103C8u;
        runtime.memory().zero(ms_data, 64u);
        for (std::uint32_t index = 0u; index < 13u; ++index)
            runtime.memory().store8(ms_data + index, runtime.memory().load8(parameter_address + kSavedataGameNameOffset + index));
        for (std::uint32_t index = 0u; index < 20u; ++index)
            runtime.memory().store8(ms_data + 16u + index, runtime.memory().load8(parameter_address + kSavedataSaveNameOffset + index));
        write_used_data_info(runtime.memory(), ms_data + 36u, used, cluster_size);
    }

    const std::uint32_t utility_data = runtime.memory().load32(parameter_address + 0x5D8u);
    if (utility_data != 0u) {
        if (!runtime.memory().contains(utility_data, 28u)) return 0x801103C8u;
        write_used_data_info(runtime.memory(), utility_data, used, cluster_size);
    }
    return 0u;
}

std::uint32_t execute_savedata_operation(psprecomp::Runtime &runtime, std::uint32_t parameter_address) {
    const std::uint32_t mode = runtime.memory().load32(parameter_address + kSavedataModeOffset);
    const std::string file_name_value = safe_savedata_component(read_fixed_string(
        runtime.memory(), parameter_address + kSavedataFileNameOffset, 13u));
    const std::string file_name = file_name_value.empty() ? "DATA.BIN" : file_name_value;
    const auto directory = savedata_directory(runtime, parameter_address);
    const auto data_path = directory / file_name;
    switch (mode) {
    case 0u: // AUTOLOAD
    case 2u: // LOAD
    case 4u: // LISTLOAD (selected saveName is already supplied by the game)
        return load_savedata_file(runtime, parameter_address, data_path, false);
    case 1u: // AUTOSAVE
    case 3u: // SAVE
    case 5u: // LISTSAVE
        return save_savedata_file(runtime, parameter_address, data_path, false);
    case 9u: // AUTODELETE
    case 10u: // DELETE
        if (!std::filesystem::exists(directory)) return 0x80110347u;
        return std::filesystem::remove_all(directory) != 0u ? 0u : 0x80110345u;
    case 11u: // LIST
        return list_savedata_directories(runtime, parameter_address);
    case 13u: // MAKEDATASECURE
    case 14u: // MAKEDATA
    case 17u: // WRITEDATASECURE
    case 18u: // WRITEDATA
        return save_savedata_file(runtime, parameter_address, data_path, true);
    case 15u: // READDATASECURE
    case 16u: // READDATA
        return load_savedata_file(runtime, parameter_address, data_path, true);
    case 19u: // ERASESECURE
    case 20u: // ERASE
    case 21u: // DELETEDATA
        if (!std::filesystem::is_regular_file(data_path)) return 0x80110329u;
        return std::filesystem::remove(data_path) ? 0u : 0x80110329u;
    case 8u:  // SIZES
        return query_savedata_sizes(runtime, parameter_address);
    case 12u: // FILES
    case 22u: // GETSIZE
        return 0u;
    default:
        return 0x80110300u;
    }
}

std::uint64_t system_time_microseconds() {
    return virtual_time_us;
}

std::uint32_t audio_remaining_samples(const AudioChannelState &channel) {
    if (!channel.reserved || channel.busy_until_us <= virtual_time_us) return 0u;
    const std::uint64_t remaining_us = channel.busy_until_us - virtual_time_us;
    const std::uint64_t samples = (remaining_us * 44100u + 999999u) / 1000000u;
    return static_cast<std::uint32_t>(std::min<std::uint64_t>(samples, channel.sample_count));
}

std::uint32_t audio_buffer_duration_us(std::uint32_t samples) {
    return static_cast<std::uint32_t>((static_cast<std::uint64_t>(samples) * 1000000u + 44099u) / 44100u);
}

// Queues one buffer on a channel and returns the virtual time at which it
// starts playing.
//
// This models what the PSP audio hardware actually does.  The previous code
// slept the guest for a whole buffer duration counted from the moment of the
// call, so the period between two submissions was buffer_duration *plus* the
// guest's own work in between -- the 2300 us charged by sceAtracDecodeData
// alone put the stream ~5% ahead of the mix, which is the constant "timeline
// resync" the host sink was reporting on every single submission.  On hardware
// the blocking call returns when the *previous* buffer has drained, so guest
// CPU time is absorbed and consecutive buffers are exactly contiguous.
//
// The start time is rebuilt from an accumulated frame count each call, so the
// per-buffer integer rounding cannot pile up into audible drift.
std::uint64_t audio_queue_buffer(AudioChannelState &channel, std::uint32_t frames) {
    const std::uint64_t rate = channel.frequency == 0u ? 44100u : channel.frequency;
    const auto elapsed_us = [&](std::uint64_t sample_frames) {
        return (sample_frames * 1000000ull) / rate;
    };
    std::uint64_t start = channel.queue_anchor_us + elapsed_us(channel.queued_frames);
    if (!channel.queue_active || start < virtual_time_us) {
        // Either the first buffer of a stream, or the guest fell far enough
        // behind that the queue really did drain.  Both are genuine
        // discontinuities: restart the anchor here.
        channel.queue_active = true;
        channel.queue_anchor_us = virtual_time_us;
        channel.queued_frames = 0u;
        start = virtual_time_us;
    }
    channel.queued_frames += frames;
    channel.busy_until_us = channel.queue_anchor_us + elapsed_us(channel.queued_frames);
    return start;
}

void set_success(psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); }

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

bool maybe_start_pending_guest_callback(psprecomp::AllegrexContext &ctx);

void enqueue_continuation(std::int32_t uid, const psprecomp::AllegrexContext &context) {
    auto thread = thread_table.threads.find(uid);
    if (thread != thread_table.threads.end()) {
        thread->second.state = ThreadState::Ready;
        thread->second.suspended_context = context;
        if (thread->second.externally_suspended) {
            thread_table.continuations.erase(
                std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                               [uid](const ThreadContinuation &item) { return item.uid == uid; }),
                thread_table.continuations.end());
            return;
        }
    }
    const auto existing = std::find_if(
        thread_table.continuations.begin(), thread_table.continuations.end(),
        [uid](const ThreadContinuation &item) { return item.uid == uid; });
    if (existing != thread_table.continuations.end()) {
        existing->context = context;
    } else {
        thread_table.continuations.push_back(
            ThreadContinuation{uid, context, thread_table.next_ready_sequence++});
    }
}

bool activate_next_thread(psprecomp::AllegrexContext &ctx, const char *reason);

std::uint32_t thread_priority(std::int32_t uid) {
    const auto found = thread_table.threads.find(uid);
    return found != thread_table.threads.end() ? found->second.priority : 0xFFFFFFFFu;
}

auto best_ready_thread() {
    return std::min_element(
        thread_table.continuations.begin(), thread_table.continuations.end(),
        [](const ThreadContinuation &left, const ThreadContinuation &right) {
            const std::uint32_t left_priority = thread_priority(left.uid);
            const std::uint32_t right_priority = thread_priority(right.uid);
            if (left_priority != right_priority) return left_priority < right_priority;
            return left.ready_sequence < right.ready_sequence;
        });
}

bool preempt_if_higher_priority(psprecomp::AllegrexContext &ctx, const char *reason) {
    const auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end() || current->second.state != ThreadState::Running) return false;
    const auto best = best_ready_thread();
    if (best == thread_table.continuations.end() ||
        thread_priority(best->uid) >= thread_priority(thread_table.current_uid)) {
        return false;
    }

    const std::int32_t caller_uid = thread_table.current_uid;
    const std::int32_t target_uid = best->uid;
    const std::uint32_t target_priority = thread_priority(target_uid);
    psprecomp::AllegrexContext caller = ctx;
    caller.pc = ctx.gpr[31];
    enqueue_continuation(caller_uid, caller);
    if (std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr || std::getenv("PSPRECOMP_TRACE") != nullptr) {
        std::cerr << "[sched] preempt reason=" << reason
                  << " caller=" << caller_uid
                  << " caller_priority=" << thread_priority(caller_uid)
                  << " target=" << target_uid
                  << " target_priority=" << target_priority << "\n";
    }
    return activate_next_thread(ctx, reason);
}

void promote_expired_delays() {
    struct ExpiredDelay {
        std::int32_t uid{};
        std::uint64_t deadline{};
        std::uint64_t sequence{};
    };
    std::vector<ExpiredDelay> expired;
    expired.reserve(thread_table.threads.size());
    for (const auto &[uid, thread] : thread_table.threads) {
        if (thread.state == ThreadState::Delayed && thread.delay_until_us <= virtual_time_us)
            expired.push_back(ExpiredDelay{uid, thread.delay_until_us, thread.delay_sequence});
    }
    // unordered_map iteration must never decide PSP scheduling order.  Kernel
    // wakeups are replayed by deadline and by the order in which the waits were
    // armed, with UID only as a final total-order guard.
    std::sort(expired.begin(), expired.end(), [](const ExpiredDelay &left, const ExpiredDelay &right) {
        if (left.deadline != right.deadline) return left.deadline < right.deadline;
        if (left.sequence != right.sequence) return left.sequence < right.sequence;
        return left.uid < right.uid;
    });
    for (const ExpiredDelay &item : expired) {
        const auto thread = thread_table.threads.find(item.uid);
        if (thread != thread_table.threads.end()) enqueue_continuation(item.uid, thread->second.suspended_context);
    }
}

bool activate_next_thread(psprecomp::AllegrexContext &ctx, const char *reason) {
    const std::int32_t previous_uid = thread_table.current_uid;
    promote_expired_delays();
    thread_table.continuations.erase(
        std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                       [](const ThreadContinuation &item) {
                           const auto thread = thread_table.threads.find(item.uid);
                           return thread == thread_table.threads.end() || thread->second.externally_suspended;
                       }),
        thread_table.continuations.end());
    if (thread_table.continuations.empty()) {
        std::uint64_t earliest = UINT64_MAX;
        for (const auto &[uid, thread] : thread_table.threads) {
            (void)uid;
            if (thread.state == ThreadState::Delayed)
                earliest = std::min(earliest, thread.delay_until_us);
        }
        if (earliest != UINT64_MAX) {
            // The recomp runtime uses deterministic virtual PSP time.  When no
            // thread is runnable, advance directly to the next kernel wakeup.
            virtual_time_us = std::max(virtual_time_us, earliest);
            promote_expired_delays();
        }
    }
    if (thread_table.continuations.empty()) return false;

    // PSP priorities are inverted: a smaller numeric value means a higher
    // scheduling priority. Equal-priority threads keep explicit FIFO order.
    const auto selected = best_ready_thread();
    ThreadContinuation continuation = *selected;
    thread_table.continuations.erase(selected);
    thread_table.current_uid = continuation.uid;
    std::string thread_name = "unknown";
    if (auto thread = thread_table.threads.find(continuation.uid); thread != thread_table.threads.end()) {
        thread->second.state = ThreadState::Running;
        thread_name = thread->second.name;
    }
    ctx = continuation.context;
    psprecomp::set_runtime_thread_identity(continuation.uid, thread_name);

    if (std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr || std::getenv("PSPRECOMP_TRACE") != nullptr) {
        const auto pending = pending_guest_callbacks.find(continuation.uid);
        const auto frames = async_return_frames.find(continuation.uid);
        std::cerr << "[sched] reason=" << reason
                  << " from_uid=" << previous_uid
                  << " to_uid=" << continuation.uid
                  << " name=" << thread_name
                  << " priority=" << thread_priority(continuation.uid)
                  << " pc=" << psprecomp::hex32(ctx.pc)
                  << " sp=" << psprecomp::hex32(ctx.gpr[29])
                  << " ra=" << psprecomp::hex32(ctx.gpr[31])
                  << " gp=" << psprecomp::hex32(ctx.gpr[28])
                  << " a0=" << psprecomp::hex32(ctx.gpr[4])
                  << " a1=" << psprecomp::hex32(ctx.gpr[5])
                  << " a2=" << psprecomp::hex32(ctx.gpr[6])
                  << " a3=" << psprecomp::hex32(ctx.gpr[7])
                  << " t0=" << psprecomp::hex32(ctx.gpr[8])
                  << " t1=" << psprecomp::hex32(ctx.gpr[9])
                  << " t2=" << psprecomp::hex32(ctx.gpr[10])
                  << " t3=" << psprecomp::hex32(ctx.gpr[11])
                  << " s0=" << psprecomp::hex32(ctx.gpr[16])
                  << " s1=" << psprecomp::hex32(ctx.gpr[17])
                  << " s2=" << psprecomp::hex32(ctx.gpr[18])
                  << " s3=" << psprecomp::hex32(ctx.gpr[19])
                  << " s4=" << psprecomp::hex32(ctx.gpr[20])
                  << " s5=" << psprecomp::hex32(ctx.gpr[21])
                  << " s6=" << psprecomp::hex32(ctx.gpr[22])
                  << " s7=" << psprecomp::hex32(ctx.gpr[23])
                  << " ready=" << thread_table.continuations.size()
                  << " pending_callbacks=" << (pending == pending_guest_callbacks.end() ? 0u : pending->second.size())
                  << " async_frames=" << (frames == async_return_frames.end() ? 0u : frames->second.size())
                  << "\n";
    }

    (void)maybe_start_pending_guest_callback(ctx);
    return true;
}

bool yield_current_thread(psprecomp::AllegrexContext &ctx) {
    psprecomp::AllegrexContext suspended = ctx;
    suspended.set_gpr(2, 0u);
    suspended.pc = ctx.gpr[31];
    enqueue_continuation(thread_table.current_uid, suspended);
    return activate_next_thread(ctx, "yield");
}

psprecomp::AllegrexContext make_wait_context(const psprecomp::AllegrexContext &ctx) {
    psprecomp::AllegrexContext suspended = ctx;
    suspended.set_gpr(2, 0u);
    suspended.pc = ctx.gpr[31];
    return suspended;
}

bool delay_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx,
                          std::uint32_t delay_microseconds, std::uint32_t return_value = 0u) {
    auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end()) {
        ctx.set_gpr(2, 0x80020198u);
        return false;
    }
    psprecomp::AllegrexContext suspended = make_wait_context(ctx);
    suspended.set_gpr(2, return_value);
    current->second.state = ThreadState::Delayed;
    current->second.suspended_context = suspended;
    current->second.delay_until_us = virtual_time_us + delay_microseconds;
    current->second.delay_sequence = thread_table.next_delay_sequence++;
    if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
        std::cerr << "[sched] delay uid=" << thread_table.current_uid
                  << " usec=" << delay_microseconds
                  << " resume=" << psprecomp::hex32(suspended.pc) << "\n";
    }
    if (!activate_next_thread(ctx, "delay")) {
        runtime.stop("PSP scheduler deadlock while delaying thread");
        return false;
    }
    return true;
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

bool suspend_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx,
                            const psprecomp::AllegrexContext &suspended,
                            const std::string &reason) {
    if (auto current = thread_table.threads.find(thread_table.current_uid);
        current != thread_table.threads.end()) {
        current->second.state = ThreadState::Sleeping;
        current->second.suspended_context = suspended;
    }
    if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
        const auto found = thread_table.threads.find(thread_table.current_uid);
        std::cerr << "[sched] block uid=" << thread_table.current_uid
                  << " name=" << (found != thread_table.threads.end() ? found->second.name : "unknown")
                  << " reason=" << reason << " resume=" << psprecomp::hex32(suspended.pc) << "\n";
    }
    if (!activate_next_thread(ctx, reason.c_str())) {
        runtime.stop("PSP scheduler deadlock while waiting for " + reason);
        return false;
    }
    return true;
}

bool sleep_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end()) {
        ctx.set_gpr(2, 0x80020198u);
        return false;
    }
    if (current->second.wakeup_count != 0u) {
        --current->second.wakeup_count;
        set_success(ctx);
        return true;
    }
    const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
    current->second.state = ThreadState::Sleeping;
    current->second.suspended_context = suspended;
    if (!activate_next_thread(ctx, "sleep")) {
        runtime.stop("PSP scheduler deadlock: every thread is sleeping");
        return false;
    }
    return true;
}

std::uint32_t wake_thread(std::int32_t uid) {
    const auto found = thread_table.threads.find(uid);
    if (found == thread_table.threads.end()) return 0x80020198u;
    ThreadRecord &thread = found->second;
    if (thread.state == ThreadState::Completed || thread.state == ThreadState::Created)
        return 0x800201A2u;
    if (thread.state == ThreadState::Sleeping) {
        enqueue_continuation(uid, thread.suspended_context);
    } else {
        ++thread.wakeup_count;
    }
    return 0u;
}

void release_thread_stack(const ThreadRecord &thread) {
    if (thread.stack_bottom == 0u || thread.stack_top <= thread.stack_bottom) return;
    thread_table.free_stacks.push_back({thread.stack_bottom, thread.stack_top});
    std::sort(thread_table.free_stacks.begin(), thread_table.free_stacks.end(),
              [](const FreeThreadStack &left, const FreeThreadStack &right) {
                  return left.bottom < right.bottom;
              });
    std::vector<FreeThreadStack> merged;
    for (const FreeThreadStack block : thread_table.free_stacks) {
        if (!merged.empty() && block.bottom <= merged.back().top) {
            merged.back().top = std::max(merged.back().top, block.top);
        } else {
            merged.push_back(block);
        }
    }
    thread_table.free_stacks = std::move(merged);

    // Collapse any free block adjacent to the downward allocation frontier.
    for (;;) {
        const auto adjacent = std::find_if(thread_table.free_stacks.begin(), thread_table.free_stacks.end(),
            [](const FreeThreadStack &block) { return block.bottom == thread_table.next_stack_top; });
        if (adjacent == thread_table.free_stacks.end()) break;
        thread_table.next_stack_top = adjacent->top;
        thread_table.free_stacks.erase(adjacent);
    }
}

bool allocate_thread_stack(std::uint32_t stack_size, std::uint32_t &bottom, std::uint32_t &top) {
    // Reuse a deleted thread stack first. Allocate from the high end to retain
    // the PSP's top-down stack layout and leave any remainder reusable.
    auto best = thread_table.free_stacks.end();
    for (auto it = thread_table.free_stacks.begin(); it != thread_table.free_stacks.end(); ++it) {
        const std::uint32_t size = it->top - it->bottom;
        if (size < stack_size) continue;
        if (best == thread_table.free_stacks.end() || size < best->top - best->bottom) best = it;
    }
    if (best != thread_table.free_stacks.end()) {
        top = best->top;
        bottom = top - stack_size;
        if (bottom == best->bottom)
            thread_table.free_stacks.erase(best);
        else
            best->top = bottom;
        return true;
    }

    top = thread_table.next_stack_top & ~0xFFu;
    if (top < stack_size) return false;
    bottom = top - stack_size;
    if (bottom < partition_table.next_address) return false;
    thread_table.next_stack_top = bottom;
    return true;
}

void remove_thread_from_wait_queues(std::int32_t uid) {
    for (auto &[semaphore_uid, semaphore] : semaphore_table.semaphores) {
        (void)semaphore_uid;
        semaphore.waiters.erase(std::remove_if(semaphore.waiters.begin(), semaphore.waiters.end(),
            [uid](const SemaphoreWaiter &waiter) { return waiter.uid == uid; }), semaphore.waiters.end());
    }
    for (auto &[flag_uid, flag] : event_flag_table.flags) {
        (void)flag_uid;
        flag.waiters.erase(std::remove_if(flag.waiters.begin(), flag.waiters.end(),
            [uid](const EventFlagWaiter &waiter) { return waiter.uid == uid; }), flag.waiters.end());
    }
    for (auto &[target_uid, waiters] : thread_table.thread_end_waiters) {
        (void)target_uid;
        waiters.erase(std::remove_if(waiters.begin(), waiters.end(),
            [uid](const ThreadContinuation &waiter) { return waiter.uid == uid; }), waiters.end());
    }
    std::erase_if(thread_table.thread_end_waiters,
                  [](const auto &entry) { return entry.second.empty(); });
    std::erase_if(callback_table.callbacks,
                  [uid](const auto &entry) { return entry.second.owner_uid == uid; });
}

void wake_thread_end_waiters(std::int32_t completed_uid, std::uint32_t result = 0u) {
    const auto found = thread_table.thread_end_waiters.find(completed_uid);
    if (found == thread_table.thread_end_waiters.end()) return;
    for (auto &waiter : found->second) {
        waiter.context.set_gpr(2, result);
        enqueue_continuation(waiter.uid, waiter.context);
    }
    thread_table.thread_end_waiters.erase(found);
}


constexpr std::uint32_t kGeListIdMagic = 0x35000000u;
constexpr std::uint32_t kGeCommandNop = 0x00u;
constexpr std::uint32_t kGeCommandVertexAddress = 0x01u;
constexpr std::uint32_t kGeCommandIndexAddress = 0x02u;
constexpr std::uint32_t kGeCommandPrimitive = 0x04u;
constexpr std::uint32_t kGeCommandBoundingBox = 0x07u;
constexpr std::uint32_t kGeCommandJump = 0x08u;
constexpr std::uint32_t kGeCommandBoundingBoxJump = 0x09u;
constexpr std::uint32_t kGeCommandCall = 0x0Au;
constexpr std::uint32_t kGeCommandReturn = 0x0Bu;
constexpr std::uint32_t kGeCommandEnd = 0x0Cu;
constexpr std::uint32_t kGeCommandSignal = 0x0Eu;
constexpr std::uint32_t kGeCommandFinish = 0x0Fu;
constexpr std::uint32_t kGeCommandBase = 0x10u;
constexpr std::uint32_t kGeCommandOffsetAddress = 0x13u;

// Stage 45.1 safe frontend optimization: GeGpuDrawDescriptor only depends on
// the registers below. Matrix/light/control-flow writes are intentionally not
// part of this revision. This keeps the proven Stage 44.7 renderer semantics
// while avoiding repeated ~60-register descriptor rebuilds in dense lists.
constexpr bool ge_command_affects_gpu_draw_descriptor(std::uint32_t command) noexcept {
    if (command >= 0xA0u && command <= 0xAFu) return true; // texture addresses/strides
    if (command >= 0xB8u && command <= 0xBFu) return true; // texture sizes
    switch (command) {
    case 0x12u: // vertex type / through mode
    case 0x1Eu: case 0x1Fu: // texture/fog enable
    case 0x21u: case 0x22u: case 0x23u: // blend/alpha/depth enable
    case 0x9Cu: case 0x9Du: // framebuffer address/stride
    case 0xB0u: case 0xB1u: // CLUT address
    case 0xC2u: case 0xC3u: case 0xC5u: case 0xC6u: case 0xC7u:
    case 0xC8u: case 0xC9u: case 0xCAu:
    case 0xCDu: case 0xCEu: case 0xCFu: case 0xD0u:
    case 0xD2u: case 0xD3u: case 0xD4u: case 0xD5u:
    case 0xDBu: case 0xDEu: case 0xDFu:
    case 0xE0u: case 0xE1u: case 0xE7u: case 0xE8u: case 0xE9u:
        return true;
    default:
        return false;
    }
}
constexpr std::uint32_t kGeCommandOrigin = 0x14u;

constexpr std::uint8_t kGeSignalNone = 0x00u;
constexpr std::uint8_t kGeSignalHandlerSuspend = 0x01u;
constexpr std::uint8_t kGeSignalHandlerContinue = 0x02u;
constexpr std::uint8_t kGeSignalHandlerPause = 0x03u;
constexpr std::uint8_t kGeSignalSync = 0x08u;
constexpr std::uint8_t kGeSignalJump = 0x10u;
constexpr std::uint8_t kGeSignalCall = 0x11u;
constexpr std::uint8_t kGeSignalReturn = 0x12u;
constexpr std::uint8_t kGeSignalRelativeJump = 0x13u;
constexpr std::uint8_t kGeSignalRelativeCall = 0x14u;
constexpr std::uint8_t kGeSignalOriginJump = 0x15u;
constexpr std::uint8_t kGeSignalOriginCall = 0x16u;

std::uint32_t ge_relative_address(std::uint32_t data) {
    const std::uint32_t base_extended = ((ge_state.commands[kGeCommandBase] & 0x000F0000u) << 8u) |
                                        (data & 0x00FFFFFFu);
    return (ge_state.offset_address + base_extended) & 0x0FFFFFFFu;
}

std::uint32_t ge_list_status(const GeListRecord &list) {
    switch (list.state) {
    case GeListState::Completed:
    case GeListState::None:
        return 0u;
    case GeListState::Queued:
        return 1u;
    case GeListState::Running:
        return 2u;
    case GeListState::Stalled:
        return 3u;
    case GeListState::Paused:
        return 4u;
    case GeListState::Error:
        return 0x80000100u;
    }
    return 0x80000100u;
}

const char *ge_command_name(std::uint32_t command) {
    switch (command) {
    case 0x00: return "NOP";
    case 0x01: return "VADDR";
    case 0x02: return "IADDR";
    case 0x04: return "PRIM";
    case 0x05: return "BEZIER";
    case 0x06: return "SPLINE";
    case 0x07: return "BBOX";
    case 0x08: return "JUMP";
    case 0x09: return "BJUMP";
    case 0x0A: return "CALL";
    case 0x0B: return "RET";
    case 0x0C: return "END";
    case 0x0E: return "SIGNAL";
    case 0x0F: return "FINISH";
    case 0x10: return "BASE";
    case 0x12: return "VTYPE";
    case 0x13: return "OFFSET";
    case 0x14: return "ORIGIN";
    case 0x9C: return "FBPTR";
    case 0x9D: return "FBWIDTH";
    case 0x9E: return "ZBPTR";
    case 0x9F: return "ZBWIDTH";
    case 0xD2: return "FBFORMAT";
    case 0xD3: return "CLEARMODE";
    case 0xEA: return "TRANSFERSTART";
    default: return nullptr;
    }
}

bool ge_histogram_diag_enabled() noexcept {
    static const bool enabled = std::getenv("PSPRECOMP_GE_DIAG") != nullptr;
    return enabled;
}

void log_ge_histogram(const GeListRecord &list) {
    if (!ge_histogram_diag_enabled()) return;
    std::vector<std::pair<std::uint32_t, std::uint64_t>> used;
    for (std::uint32_t command = 0; command < list.histogram.size(); ++command) {
        if (list.histogram[command] != 0u)
            used.emplace_back(command, list.histogram[command]);
    }
    std::sort(used.begin(), used.end(), [](const auto &left, const auto &right) {
        if (left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    std::cerr << "[ge] list=" << psprecomp::hex32(list.guest_id)
              << " start=" << psprecomp::hex32(list.start_pc)
              << " endpc=" << psprecomp::hex32(list.pc)
              << " commands=" << list.executed_commands
              << " prim=" << list.primitive_commands
              << " state=" << static_cast<std::uint32_t>(list.state) << "\n";
    for (const auto &[command, count] : used) {
        std::cerr << "[ge]   cmd=0x" << std::hex << std::setw(2) << std::setfill('0') << command
                  << std::dec << " count=" << count;
        if (const char *name = ge_command_name(command)) std::cerr << " name=" << name;
        std::cerr << " last=" << psprecomp::hex32(ge_state.commands[command]) << "\n";
    }
}

bool start_next_guest_callback(psprecomp::AllegrexContext &ctx, bool begin_chain) {
    const std::int32_t uid = thread_table.current_uid;
    const auto found = pending_guest_callbacks.find(uid);
    if (found == pending_guest_callbacks.end() || found->second.empty()) return false;

    auto &frames = async_return_frames[uid];
    if (begin_chain) {
        // A GE callback is interrupt-like, but must never re-enter another guest
        // callback or a sub-interrupt already running on this thread.
        if (!frames.empty()) return false;
        frames.push_back(AsyncReturnFrame{AsyncReturnKind::GeCallbackChain, ctx});
    } else if (frames.empty() || frames.back().kind != AsyncReturnKind::GeCallbackChain) {
        return false;
    }

    const GuestCallbackInvocation invocation = found->second.front();
    found->second.erase(found->second.begin());
    if (found->second.empty()) pending_guest_callbacks.erase(found);
    ctx.set_gpr(4, invocation.a0);
    ctx.set_gpr(5, invocation.a1);
    ctx.set_gpr(6, invocation.a2);
    ctx.set_gpr(31, 0x00000004u);
    ctx.pc = invocation.function;
    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[callback] start uid=" << uid
                  << " function=" << psprecomp::hex32(invocation.function)
                  << " a0=" << psprecomp::hex32(invocation.a0)
                  << " a1=" << psprecomp::hex32(invocation.a1)
                  << " a2=" << psprecomp::hex32(invocation.a2)
                  << " remaining=" << (pending_guest_callbacks.contains(uid) ? pending_guest_callbacks[uid].size() : 0u)
                  << "\n";
    }
    return true;
}

bool maybe_start_pending_guest_callback(psprecomp::AllegrexContext &ctx) {
    const auto thread = thread_table.threads.find(thread_table.current_uid);
    if (thread == thread_table.threads.end() || thread->second.state != ThreadState::Running) return false;
    const auto frames = async_return_frames.find(thread_table.current_uid);
    if (frames != async_return_frames.end() && !frames->second.empty()) return false;
    return start_next_guest_callback(ctx, true);
}

void queue_guest_callback_chain(psprecomp::AllegrexContext &ctx,
                                const psprecomp::AllegrexContext &resume,
                                std::vector<GuestCallbackInvocation> callbacks) {
    callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [](const GuestCallbackInvocation &item) {
        return item.function == 0u;
    }), callbacks.end());
    ctx = resume;
    if (callbacks.empty()) return;
    auto &pending = pending_guest_callbacks[thread_table.current_uid];
    pending.insert(pending.end(), callbacks.begin(), callbacks.end());
    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[callback] queued uid=" << thread_table.current_uid
                  << " count=" << callbacks.size()
                  << " total=" << pending.size()
                  << " resume=" << psprecomp::hex32(resume.pc) << "\n";
    }
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

bool append_ge_callback(const GeListRecord &list, bool signal, std::uint16_t token,
                        std::uint32_t next_pc, std::vector<GuestCallbackInvocation> &callbacks) {
    const auto found = ge_callback_table.callbacks.find(list.callback_id);
    if (found == ge_callback_table.callbacks.end()) return false;
    const GeCallbackRecord &registered = found->second;
    const std::uint32_t function = signal ? registered.signal_function : registered.finish_function;
    const std::uint32_t argument = signal ? registered.signal_argument : registered.finish_argument;
    if (function == 0u) return false;
    callbacks.push_back(GuestCallbackInvocation{
        function,
        token,
        argument,
        compiled_sdk_version <= 0x02000010u ? 0u : next_pc,
    });
    return true;
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
std::uint64_t ge_commands_this_vblank{};

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
    runtime_log_line(std::string_view(speed_text).substr(0u, speed_text.size() - 1u));

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
    // A GUI-subsystem build has no console (and Proton drops its stderr), so
    // mirror the diagnostic into the runtime log as well.
    std::string_view logged(text);
    while (!logged.empty() && (logged.back() == '\n' || logged.back() == '\r'))
        logged.remove_suffix(1u);
    if (!logged.empty()) runtime_log_line(logged);
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

bool execute_ge_list(psprecomp::Runtime &runtime, GeListRecord &list,
                     std::vector<GuestCallbackInvocation> &callbacks,
                     const std::atomic<std::uint32_t> *async_stall = nullptr) {
    constexpr std::uint64_t kMaximumCommandsPerRun = 4'000'000u;
    const bool time_ge = frame_time_diag_enabled();
    const bool ge_histogram = ge_histogram_diag_enabled();
    const bool count_ge_commands = ge_phase_diag_line_enabled();
    const auto ge_entry_time = time_ge
        ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
    struct GeTimeGuard {
        bool active;
        std::chrono::steady_clock::time_point entry;
        ~GeTimeGuard() {
            if (!active) return;
            frame_time_stats.ge_time += std::chrono::steady_clock::now() - entry;
            ++frame_time_stats.ge_calls;
        }
    } ge_time_guard{time_ge, ge_entry_time};

    // Stage 41: GE lists are overwhelmingly sequential command streams.  The
    // old loop called contains() and then aot_load32() for every 32-bit command,
    // resolving/canonicalizing the same guest range twice.  Cache a direct RAM/
    // VRAM window for up to 64 KiB and only re-resolve when control flow leaves
    // it.  GuestMemory storage is fixed after construction, so the pointer stays
    // valid for the duration of this synchronous GE run.
    const std::uint8_t *ge_fetch_pointer = nullptr;
    std::uint32_t ge_fetch_guest_base = 0u;
    std::size_t ge_fetch_bytes = 0u;
    const auto read_le32_unaligned = [](const std::uint8_t *source) noexcept {
        std::uint32_t value{};
        std::memcpy(&value, source, sizeof(value));
        if constexpr (std::endian::native == std::endian::big) {
            value = ((value & 0x000000FFu) << 24u) |
                    ((value & 0x0000FF00u) << 8u) |
                    ((value & 0x00FF0000u) >> 8u) |
                    ((value & 0xFF000000u) >> 24u);
        }
        return value;
    };
    const auto fetch_ge_command = [&](std::uint32_t pc, std::uint32_t &value) {
        if (ge_fetch_pointer != nullptr && pc >= ge_fetch_guest_base) {
            const std::size_t offset = static_cast<std::size_t>(pc - ge_fetch_guest_base);
            if (offset <= ge_fetch_bytes && ge_fetch_bytes - offset >= 4u) {
                value = read_le32_unaligned(ge_fetch_pointer + offset);
                return true;
            }
        }

        ge_fetch_pointer = nullptr;
        ge_fetch_bytes = 0u;
        ge_fetch_guest_base = pc;
        constexpr std::array<std::size_t, 7> kFetchSizes{
            65536u, 16384u, 4096u, 1024u, 256u, 64u, 4u};
        for (const std::size_t bytes : kFetchSizes) {
            if (const std::uint8_t *pointer = runtime.memory().raw_pointer(pc, bytes)) {
                ge_fetch_pointer = pointer;
                ge_fetch_bytes = bytes;
                value = read_le32_unaligned(pointer);
                return true;
            }
        }
        return false;
    };

    list.state = GeListState::Running;
    while (list.executed_commands < kMaximumCommandsPerRun) {
        const std::uint32_t effective_stall = async_stall != nullptr
            ? async_stall->load(std::memory_order_acquire) : list.stall;
        if (effective_stall != 0u && list.pc == effective_stall) {
            list.stall = effective_stall;
            list.state = GeListState::Stalled;
            log_ge_histogram(list);
            return true;
        }
        std::uint32_t op{};
        if ((list.pc & 3u) != 0u || !fetch_ge_command(list.pc, op)) {
            list.state = GeListState::Error;
            ge_execution_stop(runtime, "GE display list PC is invalid: " + psprecomp::hex32(list.pc));
            return false;
        }

        const std::uint32_t op_pc = list.pc;
        const std::uint32_t command = op >> 24u;
        const std::uint32_t data = op & 0x00FFFFFFu;
        const std::uint32_t previous_command = ge_state.commands[command];
        ge_state.commands[command] = op;
        if (op != previous_command && ge_command_affects_gpu_draw_descriptor(command))
            ++ge_draw_state_revision;
        if (op != previous_command && ge_command_affects_lighting(command))
            ++ge_lighting_state_revision;
        // Camera matrices are streaming DATA registers: even an identical 24-bit
        // payload advances the cursor and can update a different matrix element.
        // View/projection cursor/data therefore always advance the camera
        // generation; viewport/offset registers do so only when their value changes.
        if (command >= 0x3Cu && command <= 0x3Fu)
            ++ge_camera_state_revision;
        else if (((command >= 0x42u && command <= 0x47u) ||
                  command == 0x4Cu || command == 0x4Du) && op != previous_command)
            ++ge_camera_state_revision;
        // Only matrix/morph commands 0x2A..0x3F are consumed here. Avoid a
        // function call + switch for the much larger population of unrelated
        // GE state commands on every display-list word.
        if (command >= 0x2Au && command <= 0x3Fu)
            update_ge_transform_state(ge_state.transform, command, data);
        if (ge_histogram) ++list.histogram[command];
        ++list.executed_commands;
        if (count_ge_commands) ++ge_commands_this_vblank;
        std::uint32_t next_pc = (op_pc + 4u) & 0x0FFFFFFFu;

        switch (command) {
        case kGeCommandNop:
            break;
        case kGeCommandVertexAddress:
            ge_state.vertex_address = ge_relative_address(data);
            break;
        case kGeCommandIndexAddress:
            ge_state.index_address = ge_relative_address(data);
            break;
        case kGeCommandPrimitive: {
            if (ge_histogram) ++list.primitive_commands;
            const std::uint32_t draw_vertex_address = ge_state.vertex_address;
            const std::uint32_t draw_index_address = ge_state.index_address;

            // reference-style deferred PRIM extension: consecutive triangle lists
            // with no state command between them are one logical
            // vertex stream. Folding them here removes repeated renderer setup,
            // texture/state lookup and decode dispatch while preserving PSP
            // command accounting. This is valid for contiguous indexed and
            // non-indexed triangle lists; other primitive families keep the
            // exact old path.
            std::uint32_t render_data = data;
            std::uint32_t logical_primitive_count = 1u;
            const std::uint32_t prim = (data >> 16u) & 7u;
            std::uint32_t total_count = data & 0xFFFFu;
            if (prim == 3u && total_count != 0u && (total_count % 3u) == 0u) {
                std::uint32_t cursor = next_pc;
                while (total_count < 0xFFFFu) {
                    const std::uint32_t merge_stall = async_stall != nullptr
                        ? async_stall->load(std::memory_order_relaxed) : list.stall;
                    if (merge_stall != 0u && cursor == merge_stall) break;
                    std::uint32_t next_op{};
                    if (!fetch_ge_command(cursor, next_op)) break;
                    if ((next_op >> 24u) != kGeCommandPrimitive) break;
                    const std::uint32_t next_data = next_op & 0x00FFFFFFu;
                    const std::uint32_t next_prim = (next_data >> 16u) & 7u;
                    const std::uint32_t next_count = next_data & 0xFFFFu;
                    if (next_prim != 3u || next_count == 0u ||
                        (next_count % 3u) != 0u || next_count > 0xFFFFu - total_count)
                        break;
                    total_count += next_count;
                    ++logical_primitive_count;
                    if (ge_histogram) ++list.histogram[kGeCommandPrimitive];
                    ++list.executed_commands;
                    if (count_ge_commands) ++ge_commands_this_vblank;
                    if (ge_histogram) ++list.primitive_commands;
                    ge_state.commands[kGeCommandPrimitive] = next_op;
                    cursor = (cursor + 4u) & 0x0FFFFFFFu;
                }
                if (logical_primitive_count > 1u) {
                    render_data = (3u << 16u) | total_count;
                    next_pc = cursor;
                }
            }

            // GeRenderStats contains expensive per-vertex clip/screen bounds that are
            // useful only for explicit render diagnostics.  The production DX12 path
            // needs only next_vertex/next_index, so leave the diagnostic bookkeeping
            // cold instead of doing min/max/isfinite work on every city vertex.
            static const bool collect_ge_render_stats =
                std::getenv("PSPRECOMP_GE_RENDER_TRACE") != nullptr ||
                std::getenv("PSPRECOMP_GE_RENDER_DIAG") != nullptr;
            GeRenderStats render_stats{};
            std::string render_error;
            if (!render_ge_primitive(runtime.memory(), ge_state.commands, ge_state.transform,
                                     draw_vertex_address, draw_index_address,
                                     render_data, render_stats, render_error,
                                     logical_primitive_count, ge_draw_state_revision,
                                     ge_camera_state_revision, ge_lighting_state_revision,
                                     collect_ge_render_stats)) {
                list.state = GeListState::Error;
                ge_execution_stop(runtime, "GE rasterizer failed at " + psprecomp::hex32(op_pc) + ": " + render_error);
                return false;
            }
            ge_state.vertex_address = render_stats.next_vertex_address;
            ge_state.index_address = render_stats.next_index_address;
            // Read once: this sits on the per-draw-call path, and getenv walks
            // the whole environment block on every call.
            static const bool render_trace_enabled =
                std::getenv("PSPRECOMP_GE_RENDER_TRACE") != nullptr;
            if (render_trace_enabled) {
                const std::uint64_t trace_start = parse_environment_u64(
                    "PSPRECOMP_GE_RENDER_TRACE_START_VBLANK");
                const std::uint64_t trace_end = parse_environment_u64(
                    "PSPRECOMP_GE_RENDER_TRACE_END_VBLANK", trace_start);
                const std::uint64_t min_pixels = parse_environment_u64(
                    "PSPRECOMP_GE_RENDER_TRACE_MIN_PIXELS", 0u);
                const std::uint64_t max_lines = parse_environment_u64(
                    "PSPRECOMP_GE_RENDER_TRACE_MAX_LINES", 200000u);
                static std::uint64_t trace_lines{};
                const bool in_window = display_vblank_index >= trace_start &&
                    display_vblank_index <= trace_end;
                const float screen_span_x = render_stats.has_screen_bounds
                    ? render_stats.screen_max_x - render_stats.screen_min_x : 0.0f;
                const float screen_span_y = render_stats.has_screen_bounds
                    ? render_stats.screen_max_y - render_stats.screen_min_y : 0.0f;
                const bool suspicious = render_stats.nonfinite_clip_vertices != 0u ||
                    render_stats.min_abs_w < 1.0e-5f ||
                    render_stats.max_abs_screen_coordinate > 4096.0f ||
                    screen_span_x > 2048.0f || screen_span_y > 2048.0f;
                const bool suspicious_only =
                    std::getenv("PSPRECOMP_GE_RENDER_TRACE_SUSPICIOUS_ONLY") != nullptr;
                if (in_window && trace_lines < max_lines &&
                    render_stats.pixels_tested >= min_pixels &&
                    (!suspicious_only || suspicious)) {
                    ++trace_lines;
                    const std::uint32_t vtype = ge_state.commands[0x12u] & 0x00FFFFFFu;
                    const std::uint32_t primitive = (render_data >> 16u) & 7u;
                    const std::uint32_t count = render_data & 0xFFFFu;
                    std::cerr << std::fixed << std::setprecision(5)
                              << "[ge-render-trace] vblank=" << display_vblank_index
                              << " line=" << trace_lines
                              << " pc=" << psprecomp::hex32(op_pc)
                              << " prim=" << primitive
                              << " count=" << count
                              << " vtype=" << psprecomp::hex32(vtype)
                              << " vaddr=" << psprecomp::hex32(draw_vertex_address)
                              << " iaddr=" << psprecomp::hex32(draw_index_address)
                              << " texfmt=" << (ge_state.commands[0xC3u] & 0xFu)
                              << " texen=" << (ge_state.commands[0x1Eu] & 1u)
                              << " clear=" << (ge_state.commands[0xD3u] & 0x701u)
                              << " depthclip=" << (ge_state.commands[0x1Cu] & 1u)
                              << " ztest=" << (ge_state.commands[0x23u] & 1u)
                              << " cull=" << (ge_state.commands[0x1Du] & 1u)
                              << " tested=" << render_stats.pixels_tested
                              << " written=" << render_stats.pixels_written
                              << " tris=" << render_stats.triangles
                              << " culled=" << render_stats.culled_triangles
                              << " skin=" << render_stats.skinned_vertices
                              << " morph=" << render_stats.morphed_vertices
                              << " nonfinite=" << render_stats.nonfinite_clip_vertices
                              << " minabsw=" << render_stats.min_abs_w
                              << " clip=[" << render_stats.clip_min_x << ','
                              << render_stats.clip_min_y << ',' << render_stats.clip_min_z << ','
                              << render_stats.clip_min_w << ":" << render_stats.clip_max_x << ','
                              << render_stats.clip_max_y << ',' << render_stats.clip_max_z << ','
                              << render_stats.clip_max_w << "]"
                              << " screen=[" << render_stats.screen_min_x << ','
                              << render_stats.screen_min_y << ':' << render_stats.screen_max_x << ','
                              << render_stats.screen_max_y << "]"
                              << " maxscreen=" << render_stats.max_abs_screen_coordinate
                              << " cursors=" << ge_state.transform.bone_cursor << ','
                              << ge_state.transform.world_cursor << ','
                              << ge_state.transform.view_cursor << ','
                              << ge_state.transform.projection_cursor << ','
                              << ge_state.transform.texture_cursor
                              << " worldT=" << ge_state.transform.world[9] << ','
                              << ge_state.transform.world[10] << ',' << ge_state.transform.world[11]
                              << " viewT=" << ge_state.transform.view[9] << ','
                              << ge_state.transform.view[10] << ',' << ge_state.transform.view[11]
                              << " suspicious=" << suspicious
                              << "\n";
                    std::cerr.unsetf(std::ios::floatfield);
                }
            }
            static const bool render_diag_enabled =
                std::getenv("PSPRECOMP_GE_RENDER_DIAG") != nullptr;
            if (render_diag_enabled) {
                static std::unordered_set<std::uint64_t> reported_signatures;
                const std::uint64_t signature =
                    (static_cast<std::uint64_t>((data >> 16u) & 7u) << 56u) |
                    (static_cast<std::uint64_t>(ge_state.commands[0x12u] & 0x00FFFFFFu) << 24u) |
                    static_cast<std::uint64_t>(ge_state.commands[0xC3u] & 0xFu);
                if (reported_signatures.insert(signature).second) {
                    std::cerr << "[ge-render] first pc=" << psprecomp::hex32(op_pc)
                              << " prim=" << ((data >> 16u) & 7u)
                              << " count=" << (data & 0xFFFFu)
                              << " vtype=" << psprecomp::hex32(ge_state.commands[0x12u] & 0x00FFFFFFu)
                              << " texfmt=" << (ge_state.commands[0xC3u] & 0xFu)
                              << " points=" << render_stats.points
                              << " lines=" << render_stats.lines
                              << " triangles=" << render_stats.triangles
                              << " rectangles=" << render_stats.rectangles
                              << " uvgen=" << render_stats.generated_uv_vertices
                              << " culled=" << render_stats.culled_triangles
                              << " flat=" << render_stats.flat_shaded_primitives
                              << " tested=" << render_stats.pixels_tested
                              << " written=" << render_stats.pixels_written
                              << " unsupported=" << render_stats.unsupported_primitives
                              << "\n";
                }
            }
            break;
        }
        case kGeCommandBoundingBox: {
            GeBoundingBoxResult result{};
            std::string bbox_error;
            if (!test_ge_bounding_box(runtime.memory(), ge_state.commands, ge_state.transform,
                                      ge_state.vertex_address, ge_state.index_address,
                                      data & 0xFFFFu, result, bbox_error)) {
                list.state = GeListState::Error;
                ge_execution_stop(runtime, "GE BBOX failed at " + psprecomp::hex32(op_pc) + ": " + bbox_error);
                return false;
            }
            ge_state.bounding_box_result = result.visible;
            ge_state.vertex_address = result.next_vertex_address;
            ge_state.index_address = result.next_index_address;
            break;
        }
        case kGeCommandJump:
            next_pc = ge_relative_address(data & 0x00FFFFFCu);
            break;
        case kGeCommandBoundingBoxJump:
            if (!ge_state.bounding_box_result)
                next_pc = ge_relative_address(data & 0x00FFFFFCu);
            break;
        case kGeCommandCall:
            if (list.stack.size() >= list.stack_capacity) {
                list.state = GeListState::Error;
                ge_execution_stop(runtime, "GE display-list CALL stack overflow at " + psprecomp::hex32(op_pc));
                return false;
            }
            list.stack.push_back(GeStackEntry{next_pc, ge_state.offset_address, ge_state.commands[kGeCommandBase]});
            next_pc = ge_relative_address(data & 0x00FFFFFCu);
            break;
        case kGeCommandReturn:
            if (list.stack.empty()) {
                list.state = GeListState::Error;
                ge_execution_stop(runtime, "GE display-list RET with empty stack at " + psprecomp::hex32(op_pc));
                return false;
            } else {
                const GeStackEntry entry = list.stack.back();
                list.stack.pop_back();
                ge_state.offset_address = entry.offset_address;
                next_pc = entry.pc & 0x0FFFFFFFu;
            }
            break;
        case kGeCommandOffsetAddress:
            ge_state.offset_address = op << 8u;
            break;
        case kGeCommandOrigin:
            ge_state.offset_address = op_pc;
            break;
        case kGeCommandEnd: {
            if (op_pc < 4u || !runtime.memory().contains(op_pc - 4u, 4u)) break;
            const std::uint32_t previous = runtime.memory().load32(op_pc - 4u);
            const std::uint32_t previous_command = previous >> 24u;
            if (previous_command == kGeCommandSignal) {
                const std::uint8_t behavior = static_cast<std::uint8_t>((previous >> 16u) & 0xFFu);
                const std::uint16_t token = static_cast<std::uint16_t>(previous & 0xFFFFu);
                const std::uint32_t end_data = op & 0xFFFFu;
                list.signal_behavior = behavior;
                list.callback_token = token;
                switch (behavior) {
                case kGeSignalHandlerSuspend:
                case kGeSignalHandlerContinue:
                    (void)append_ge_callback(list, true, token, next_pc, callbacks);
                    break;
                case kGeSignalHandlerPause:
                    // The callback is delivered by the next FINISH/END pair.
                    break;
                case kGeSignalSync:
                    runtime.memory().memory_barrier();
                    break;
                case kGeSignalJump:
                case kGeSignalRelativeJump:
                case kGeSignalOriginJump: {
                    const std::uint32_t combined = ((static_cast<std::uint32_t>(token) << 16u) | end_data) & 0x0FFFFFFCu;
                    if (behavior == kGeSignalRelativeJump)
                        next_pc = (combined + op_pc - 4u) & 0x0FFFFFFFu;
                    else if (behavior == kGeSignalOriginJump)
                        next_pc = ge_relative_address(combined);
                    else
                        next_pc = combined;
                    break;
                }
                case kGeSignalCall:
                case kGeSignalRelativeCall:
                case kGeSignalOriginCall: {
                    if (list.stack.size() >= list.stack_capacity) {
                        list.state = GeListState::Error;
                        ge_execution_stop(runtime, "GE SIGNAL CALL stack overflow at " + psprecomp::hex32(op_pc));
                        return false;
                    }
                    const std::uint32_t combined = ((static_cast<std::uint32_t>(token) << 16u) | end_data) & 0x0FFFFFFCu;
                    std::uint32_t target = combined;
                    if (behavior == kGeSignalRelativeCall)
                        target = (combined + op_pc - 4u) & 0x0FFFFFFFu;
                    else if (behavior == kGeSignalOriginCall)
                        target = ge_relative_address(combined);
                    list.stack.push_back(GeStackEntry{next_pc, ge_state.offset_address, ge_state.commands[kGeCommandBase]});
                    next_pc = target;
                    break;
                }
                case kGeSignalReturn:
                    if (list.stack.empty()) {
                        list.state = GeListState::Error;
                        ge_execution_stop(runtime, "GE SIGNAL RET with empty stack at " + psprecomp::hex32(op_pc));
                        return false;
                    } else {
                        const GeStackEntry entry = list.stack.back();
                        list.stack.pop_back();
                        ge_state.offset_address = entry.offset_address;
                        if (ge_state.commands[kGeCommandBase] != entry.base_command) {
                            ge_state.commands[kGeCommandBase] = entry.base_command;
                            ++ge_draw_state_revision;
                        }
                        next_pc = entry.pc & 0x0FFFFFFFu;
                    }
                    break;
                default:
                    list.state = GeListState::Error;
                    ge_execution_stop(runtime, "Unsupported GE SIGNAL behavior " + std::to_string(behavior) +
                                 " at " + psprecomp::hex32(op_pc));
                    return false;
                }
            } else if (previous_command == kGeCommandFinish) {
                const std::uint16_t token = static_cast<std::uint16_t>(previous & 0xFFFFu);
                list.callback_token = token;
                if (list.signal_behavior == kGeSignalHandlerPause) {
                    list.state = GeListState::Paused;
                    (void)append_ge_callback(list, true, token, next_pc, callbacks);
                } else {
                    list.state = GeListState::Completed;
                    restore_ge_list_context(list);
                    (void)append_ge_callback(list, false, token, next_pc, callbacks);
                }
                list.pc = next_pc;
                log_ge_histogram(list);
                return true;
            }
            break;
        }
        default:
            // State-setting commands are retained in ge_state.commands and are
            // consumed by the renderer as support is added.
            break;
        }

        list.pc = next_pc;
    }

    list.state = GeListState::Error;
    ge_execution_stop(runtime, "GE display list exceeded the command safety limit at " + psprecomp::hex32(list.pc));
    return false;
}


void ge_async_worker_main() {
    ge_async_worker_thread = true;
    for (;;) {
        GeAsyncTask task{};
        GeListRecord local{};
        psprecomp::Runtime *runtime = nullptr;
        {
            std::unique_lock lock(ge_async.mutex);
            ge_async.cv.wait(lock, [] { return ge_async.stop_requested || !ge_async.pending.empty(); });
            if (ge_async.stop_requested && ge_async.pending.empty()) break;
            task = std::move(ge_async.pending.front());
            ge_async.pending.pop_front();
            runtime = ge_async.runtime;
            const auto found = ge_list_table.lists.find(task.id);
            if (runtime == nullptr || found == ge_list_table.lists.end() ||
                found->second.state == GeListState::None ||
                found->second.state == GeListState::Completed ||
                found->second.state == GeListState::Error) {
                ge_async.live_stalls.erase(task.id);
                ge_async.outstanding.fetch_sub(1u, std::memory_order_acq_rel);
                ge_async.cv.notify_all();
                continue;
            }
            local = found->second;
            local.state = GeListState::Running;
            found->second.state = GeListState::Running;
        }

        std::vector<GuestCallbackInvocation> callbacks;
        const bool ok = execute_ge_list(*runtime, local, callbacks, task.stall.get());

        {
            std::lock_guard lock(ge_async.mutex);
            const auto found = ge_list_table.lists.find(task.id);
            if (found != ge_list_table.lists.end()) found->second = std::move(local);
            ge_async.live_stalls.erase(task.id);
            if (!callbacks.empty()) {
                ge_async.completions.push_back(GeAsyncCompletion{task.submitter_uid, std::move(callbacks)});
                ge_async.completion_count.fetch_add(1u, std::memory_order_release);
            }
            ++ge_async.completed;
            ge_async.outstanding.fetch_sub(1u, std::memory_order_acq_rel);
            if (!ok && !ge_async.fatal.load(std::memory_order_acquire)) {
                ge_async.fatal_reason = "Asynchronous GE display-list execution failed";
                ge_async.fatal.store(true, std::memory_order_release);
            }
        }
        ge_async.cv.notify_all();
    }
    ge_async_worker_thread = false;
}

void ge_async_stop_worker() {
    std::thread worker;
    {
        std::lock_guard lock(ge_async.mutex);
        if (!ge_async.started.load(std::memory_order_acquire)) return;
        ge_async.stop_requested = true;
        ge_async.cv.notify_all();
        worker = std::move(ge_async.thread);
    }
    if (worker.joinable()) worker.join();
    std::lock_guard lock(ge_async.mutex);
    ge_async.started.store(false, std::memory_order_release);
    ge_async.runtime = nullptr;
    ge_async.pending.clear();
    ge_async.live_stalls.clear();
    ge_async.completions.clear();
    ge_async.outstanding.store(0u, std::memory_order_release);
    ge_async.completion_count.store(0u, std::memory_order_release);
    ge_async.last_wait_ns.store(0u, std::memory_order_release);
}

void ge_async_drain_completions() {
    if (!ge_async_enabled() || ge_async.completion_count.load(std::memory_order_acquire) == 0u) return;
    std::deque<GeAsyncCompletion> ready;
    {
        std::lock_guard lock(ge_async.mutex);
        ready.swap(ge_async.completions);
        ge_async.completion_count.store(0u, std::memory_order_release);
    }
    while (!ready.empty()) {
        GeAsyncCompletion completion = std::move(ready.front());
        ready.pop_front();
        auto &pending = pending_guest_callbacks[completion.submitter_uid];
        pending.insert(pending.end(), completion.callbacks.begin(), completion.callbacks.end());
    }
}

bool ge_async_check_fatal(psprecomp::Runtime &runtime) {
    if (!ge_async.fatal.load(std::memory_order_acquire)) return true;
    std::string reason;
    {
        std::lock_guard lock(ge_async.mutex);
        reason = ge_async.fatal_reason.empty() ? "Asynchronous GE worker failed" : ge_async.fatal_reason;
    }
    runtime.stop(std::move(reason));
    return false;
}

bool ge_async_wait_idle(psprecomp::Runtime &runtime) {
    if (!ge_async_enabled() || !ge_async.started.load(std::memory_order_acquire)) return true;
    if (ge_async.outstanding.load(std::memory_order_acquire) == 0u) {
        ge_async.last_wait_ns.store(0u, std::memory_order_release);
        ge_async_drain_completions();
        return ge_async_check_fatal(runtime);
    }
    const auto begin = std::chrono::steady_clock::now();
    {
        std::unique_lock lock(ge_async.mutex);
        ++ge_async.wait_calls;
        ge_async.cv.wait(lock, [] {
            return ge_async.outstanding.load(std::memory_order_acquire) == 0u;
        });
        const auto elapsed = std::chrono::steady_clock::now() - begin;
        ge_async.wait_time += elapsed;
        ge_async.last_wait_ns.store(
            static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()),
            std::memory_order_release);
    }
    runtime.memory().memory_barrier();
    ge_async_drain_completions();
    return ge_async_check_fatal(runtime);
}

bool ge_async_wait_list(psprecomp::Runtime &runtime, std::uint32_t id) {
    if (!ge_async_enabled() || !ge_async.started.load(std::memory_order_acquire)) return true;
    const auto begin = std::chrono::steady_clock::now();
    {
        std::unique_lock lock(ge_async.mutex);
        ++ge_async.wait_calls;
        ge_async.cv.wait(lock, [id] {
            const auto found = ge_list_table.lists.find(id);
            if (found == ge_list_table.lists.end()) return true;
            return found->second.state != GeListState::Queued &&
                   found->second.state != GeListState::Running;
        });
        ge_async.wait_time += std::chrono::steady_clock::now() - begin;
    }
    runtime.memory().memory_barrier();
    ge_async_drain_completions();
    return ge_async_check_fatal(runtime);
}

std::uint32_t allocate_ge_list_id() {
    for (std::uint32_t attempt = 0; attempt < 64u; ++attempt) {
        const std::uint32_t raw = (ge_list_table.next_raw_id + attempt) % 64u;
        const std::uint32_t guest = kGeListIdMagic ^ raw;
        const auto found = ge_list_table.lists.find(guest);
        if (found == ge_list_table.lists.end() || found->second.state == GeListState::Completed ||
            found->second.state == GeListState::None) {
            ge_list_table.next_raw_id = (raw + 1u) % 64u;
            return guest;
        }
    }
    return 0u;
}


void enqueue_ge_display_list(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx, bool head) {
    const std::uint32_t list_address = ctx.gpr[4] & 0x0FFFFFFFu;
    const std::uint32_t stall_address = ctx.gpr[5] & 0x0FFFFFFFu;
    const std::int32_t callback_id = static_cast<std::int32_t>(ctx.gpr[6]);
    const std::uint32_t option_address = ctx.gpr[7];

    if ((list_address & 3u) != 0u || (stall_address & 3u) != 0u ||
        !runtime.memory().contains(list_address, 4u)) {
        ctx.set_gpr(2, 0x80000103u);
        return;
    }

    GeListRecord record{};
    record.start_pc = list_address;
    record.pc = list_address;
    record.stall = stall_address;
    record.callback_id = callback_id;
    record.state = GeListState::Queued;
    record.stack_capacity = 32u;

    if (option_address != 0u) {
        if (!runtime.memory().contains(option_address, 4u)) {
            ctx.set_gpr(2, 0x800200D3u);
            return;
        }
        const std::uint32_t size = runtime.memory().load32(option_address);
        if (size >= 8u) {
            if (!runtime.memory().contains(option_address, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            record.context_address = runtime.memory().load32(option_address + 4u);
        }
        if (size >= 16u) {
            if (!runtime.memory().contains(option_address, 16u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint32_t stack_count = runtime.memory().load32(option_address + 8u);
            record.stack_address = runtime.memory().load32(option_address + 12u);
            if (stack_count >= 256u) {
                ctx.set_gpr(2, 0x80000104u);
                return;
            }
            if (stack_count != 0u) record.stack_capacity = stack_count;
        }
    }

    if (record.context_address != 0u &&
        !runtime.memory().contains(record.context_address, 512u * 4u)) {
        ctx.set_gpr(2, 0x800200D3u);
        return;
    }

    if (ge_async_enabled()) {
        // Context save/restore snapshots the global GE register file.  Lists using
        // that uncommon feature establish an explicit serialization boundary;
        // normal VCS gameplay lists stay fully asynchronous.
        if (record.context_address != 0u && !ge_async_wait_idle(runtime)) return;
        ge_async_start_worker(runtime);

        std::uint32_t guest_id = 0u;
        std::uint32_t log_stack = record.stack_capacity;
        {
            std::lock_guard lock(ge_async.mutex);
            for (const auto &[id, active] : ge_list_table.lists) {
                (void)id;
                if (active.start_pc == list_address && active.state != GeListState::Completed &&
                    active.state != GeListState::None && active.state != GeListState::Error) {
                    ctx.set_gpr(2, 0x80000021u);
                    return;
                }
            }
            guest_id = allocate_ge_list_id();
            if (guest_id == 0u) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            record.guest_id = guest_id;
            if (record.context_address != 0u) save_ge_list_context(runtime, record);

            auto [found, inserted] = ge_list_table.lists.insert_or_assign(guest_id, std::move(record));
            (void)inserted;
            if (head)
                ge_list_table.queue.insert(ge_list_table.queue.begin(), guest_id);
            else
                ge_list_table.queue.push_back(guest_id);

            auto stall = std::make_shared<std::atomic<std::uint32_t>>(found->second.stall);
            ge_async.live_stalls[guest_id] = stall;
            GeAsyncTask task{guest_id, thread_table.current_uid, stall};
            if (head)
                ge_async.pending.push_front(std::move(task));
            else
                ge_async.pending.push_back(std::move(task));
            ge_async.outstanding.fetch_add(1u, std::memory_order_release);
            ++ge_async.submitted;
        }

        if (ge_histogram_diag_enabled()) {
            std::cerr << "[ge-async] enqueue id=" << psprecomp::hex32(guest_id)
                      << " list=" << psprecomp::hex32(list_address)
                      << " stall=" << psprecomp::hex32(stall_address)
                      << " cbid=" << callback_id
                      << " option=" << psprecomp::hex32(option_address)
                      << " stack=" << log_stack << "\n";
        }
        runtime.memory().memory_barrier();
        ge_async.cv.notify_one();
        ctx.set_gpr(2, guest_id);
        return;
    }

    for (const auto &[id, active] : ge_list_table.lists) {
        (void)id;
        if (active.start_pc == list_address && active.state != GeListState::Completed &&
            active.state != GeListState::None && active.state != GeListState::Error) {
            ctx.set_gpr(2, 0x80000021u);
            return;
        }
    }

    const std::uint32_t guest_id = allocate_ge_list_id();
    if (guest_id == 0u) {
        ctx.set_gpr(2, 0x80020190u);
        return;
    }
    record.guest_id = guest_id;
    if (record.context_address != 0u) save_ge_list_context(runtime, record);

    auto [found, inserted] = ge_list_table.lists.insert_or_assign(guest_id, std::move(record));
    (void)inserted;
    if (head)
        ge_list_table.queue.insert(ge_list_table.queue.begin(), guest_id);
    else
        ge_list_table.queue.push_back(guest_id);

    GeListRecord &list = found->second;
    if (ge_histogram_diag_enabled()) {
        std::cerr << "[ge] enqueue id=" << psprecomp::hex32(guest_id)
                  << " list=" << psprecomp::hex32(list.start_pc)
                  << " stall=" << psprecomp::hex32(list.stall)
                  << " cbid=" << callback_id
                  << " option=" << psprecomp::hex32(option_address)
                  << " stack=" << list.stack_capacity << "\n";
    }

    std::vector<GuestCallbackInvocation> callbacks;
    if (!execute_ge_list(runtime, list, callbacks)) return;

    psprecomp::AllegrexContext resume = ctx;
    resume.set_gpr(2, guest_id);
    resume.pc = ctx.gpr[31];
    queue_guest_callback_chain(ctx, resume, std::move(callbacks));
}

void complete_current_thread(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::int32_t completed_uid = thread_table.current_uid;
    if (auto current = thread_table.threads.find(completed_uid); current != thread_table.threads.end())
        current->second.state = ThreadState::Completed;
    thread_table.continuations.erase(
        std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                       [completed_uid](const ThreadContinuation &item) { return item.uid == completed_uid; }),
        thread_table.continuations.end());
    pending_guest_callbacks.erase(completed_uid);
    async_return_frames.erase(completed_uid);
    deferred_io_resumes.erase(completed_uid);
    refresh_vcs_post_dispatch_hook();
    wake_thread_end_waiters(completed_uid);
    if (!activate_next_thread(ctx, "thread-complete")) {
        ctx.set_gpr(2, 0u);
        runtime.stop("All PSP threads completed");
    }
}

void vcs_module_thread_return(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    complete_current_thread(runtime, ctx);
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

void vcs_interrupt_return(psprecomp::Runtime &runtime, psprecomp::AllegrexContext &ctx) {
    const std::int32_t uid = thread_table.current_uid;
    const auto found = async_return_frames.find(uid);
    if (found == async_return_frames.end() || found->second.empty()) {
        runtime.stop("PSP interrupt/callback return without a saved thread context");
        return;
    }

    const AsyncReturnKind kind = found->second.back().kind;
    if (kind == AsyncReturnKind::MpegRingbuffer) {
        if (continue_mpeg_ringbuffer_callback(runtime, ctx, found->second.back())) return;
        found->second.pop_back();
        if (found->second.empty()) async_return_frames.erase(found);
        (void)maybe_start_pending_guest_callback(ctx);
        return;
    }
    if (kind == AsyncReturnKind::UserCallback) {
        ctx = found->second.back().resume;
        found->second.pop_back();
        if (found->second.empty()) async_return_frames.erase(found);
        (void)maybe_start_pending_guest_callback(ctx);
        return;
    }
    if (kind == AsyncReturnKind::GeCallbackChain) {
        // Restore the interrupted guest state before every callback in the
        // chain.  A callback must not leak SP, callee-saved GPRs, FPU or VFPU
        // state into the next callback merely because both were queued by one
        // display list.
        ctx = found->second.back().resume;
        if (start_next_guest_callback(ctx, false)) return;
    }

    ctx = found->second.back().resume;
    found->second.pop_back();
    if (found->second.empty()) async_return_frames.erase(found);

    if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr || std::getenv("PSPRECOMP_SCHED_DIAG") != nullptr) {
        std::cerr << "[async-return] uid=" << uid
                  << " kind=" << (kind == AsyncReturnKind::GeCallbackChain ? "ge" : "subintr")
                  << " resume=" << psprecomp::hex32(ctx.pc) << "\n";
    }

    // A pending GE callback may now run only after the interrupted frame has
    // completely unwound.  Never inject it into a nested callback/interrupt.
    (void)maybe_start_pending_guest_callback(ctx);
}
}

const char *thread_state_name(ThreadState state) {
    switch (state) {
    case ThreadState::Created: return "Created";
    case ThreadState::Ready: return "Ready";
    case ThreadState::Running: return "Running";
    case ThreadState::Sleeping: return "Sleeping";
    case ThreadState::Delayed: return "Delayed";
    case ThreadState::IoDeferred: return "IoDeferred";
    case ThreadState::Completed: return "Completed";
    }
    return "Unknown";
}

bool event_diag_matches(const EventFlagRecord &flag) {
    if (std::getenv("PSPRECOMP_EVENT_DIAG") == nullptr) return false;
    const char *filter = std::getenv("PSPRECOMP_EVENT_DIAG_FILTER");
    return filter == nullptr || *filter == '\0' || flag.name.find(filter) != std::string::npos;
}

void dump_event_stall_state(const EventFlagRecord &flag, std::int32_t flag_uid,
                            const psprecomp::AllegrexContext &ctx) {
    std::cerr << "[event-stall] uid=" << flag_uid
              << " name=\"" << flag.name << "\""
              << " pattern=" << psprecomp::hex32(flag.current_pattern)
              << " polls=" << event_diag_poll_count
              << " vblank=" << display_vblank_index
              << " virtual_time_us=" << virtual_time_us
              << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
              << " ctx_pc=" << psprecomp::hex32(ctx.pc)
              << " ra=" << psprecomp::hex32(ctx.gpr[31])
              << " current_uid=" << thread_table.current_uid
              << " ready=" << thread_table.continuations.size()
              << " deferred_io=" << deferred_io_resumes.size() << "\n";

    std::vector<std::int32_t> uids;
    uids.reserve(thread_table.threads.size());
    for (const auto &[uid, unused] : thread_table.threads) {
        (void)unused;
        uids.push_back(uid);
    }
    std::sort(uids.begin(), uids.end());
    for (const std::int32_t uid : uids) {
        const ThreadRecord &thread = thread_table.threads.at(uid);
        std::cerr << "[event-stall-thread] uid=" << uid
                  << " name=\"" << thread.name << "\""
                  << " priority=" << thread.priority
                  << " state=" << thread_state_name(thread.state)
                  << " external=" << (thread.externally_suspended ? 1 : 0)
                  << " pc=" << psprecomp::hex32(thread.suspended_context.pc)
                  << " ra=" << psprecomp::hex32(thread.suspended_context.gpr[31])
                  << " delay_until=" << thread.delay_until_us
                  << " wakeups=" << thread.wakeup_count << "\n";
    }
    for (const ThreadContinuation &ready : thread_table.continuations) {
        std::cerr << "[event-stall-ready] uid=" << ready.uid
                  << " pc=" << psprecomp::hex32(ready.context.pc)
                  << " ra=" << psprecomp::hex32(ready.context.gpr[31])
                  << " sequence=" << ready.ready_sequence << "\n";
    }
    for (const auto &[worker_uid, barrier] : deferred_io_resumes) {
        std::cerr << "[event-stall-io] worker=" << worker_uid
                  << " handoff_uid=" << barrier.handoff_uid
                  << " handoff_pc=" << psprecomp::hex32(barrier.handoff_pc)
                  << " release_pc=" << psprecomp::hex32(barrier.release_pc)
                  << " observed=" << barrier.observed_dispatches << "\n";
    }
    for (const auto &[uid, item] : event_flag_table.flags) {
        if (item.name.find("World") == std::string::npos &&
            item.name.find("Umd") == std::string::npos) continue;
        std::cerr << "[event-stall-flag] uid=" << uid
                  << " name=\"" << item.name << "\""
                  << " pattern=" << psprecomp::hex32(item.current_pattern)
                  << " waiters=" << item.waiters.size() << "\n";
        for (const EventFlagWaiter &waiter : item.waiters) {
            std::cerr << "[event-stall-waiter] flag_uid=" << uid
                      << " thread_uid=" << waiter.uid
                      << " requested=" << psprecomp::hex32(waiter.requested)
                      << " mode=" << psprecomp::hex32(waiter.mode)
                      << " pc=" << psprecomp::hex32(waiter.context.pc)
                      << " ra=" << psprecomp::hex32(waiter.context.gpr[31]) << "\n";
        }
    }
    for (const auto &[uid, semaphore] : semaphore_table.semaphores) {
        if (semaphore.name.find("Stream") == std::string::npos &&
            semaphore.name.find("stream") == std::string::npos) continue;
        std::cerr << "[event-stall-sema] uid=" << uid
                  << " name=\"" << semaphore.name << "\""
                  << " count=" << semaphore.count
                  << " maximum=" << semaphore.maximum
                  << " waiters=" << semaphore.waiters.size() << "\n";
    }
}

bool event_flag_matches(const EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode) {
    if ((mode & 1u) != 0u) return (flag.current_pattern & requested) != 0u;
    return (flag.current_pattern & requested) == requested;
}

void consume_event_flag(EventFlagRecord &flag, std::uint32_t requested, std::uint32_t mode) {
    if ((mode & 0x20u) != 0u) flag.current_pattern &= ~requested;
    if ((mode & 0x10u) != 0u) flag.current_pattern = 0u;
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
    file_table = FileTable{};
    for (auto &[address, state] : mpeg_contexts) close_video_decoder(state);
    mpeg_contexts.clear();
    for (auto &state : atrac_contexts) close_atrac_decoder(state);
    atrac_contexts = {};
    next_mpeg_stream_id = 1u;
    vcs::audio_output_shutdown();
    audio_channels = {};
    sas_state = SasState{};
    thread_table = ThreadTable{};
    deferred_io_resumes.clear();
    event_diag_poll_count = 0u;
    event_diag_stop_polls = parse_environment_u64("PSPRECOMP_EVENT_DIAG_STOP_POLLS", 0u);
    event_diag_stall_reported = false;
    partition_table = PartitionTable{};
    partition_table.next_address = (user_arena_start + 0xFFu) & ~0xFFu;
    callback_table = CallbackTable{};
    semaphore_table = SemaphoreTable{};
    event_flag_table = EventFlagTable{};
    fixed_pool_table = FixedPoolTable{};
    loaded_modules.clear();
    next_module_uid = 0x400;
    ThreadRecord module_thread{};
    module_thread.name = "module_start";
    module_thread.priority = 32u;
    module_thread.stack_size = 0x10000u;
    // Keep the loader/module stack at the top of user RAM.  The game arena grows
    // upward from the aligned end of the ELF, and subsequent thread stacks grow
    // downward below this reserved loader stack.
    module_thread.stack_top = 0x0A000000u;
    module_thread.stack_bottom = module_thread.stack_top - module_thread.stack_size;
    module_thread.kernel_context = module_thread.stack_top - 0x100u;
    thread_table.next_stack_top = module_thread.stack_bottom;
    module_thread.state = ThreadState::Running;
    if (!runtime.memory().contains(module_thread.stack_bottom, module_thread.stack_size))
        throw psprecomp::Error("VCS module_start stack falls outside PSP user RAM");
    runtime.memory().zero(module_thread.stack_bottom, module_thread.stack_size);
    runtime.memory().store32(module_thread.stack_bottom, 0u);
    runtime.memory().store32(module_thread.kernel_context + 0xC0u, 0u);
    runtime.memory().store32(module_thread.kernel_context + 0xC8u, module_thread.stack_bottom);
    runtime.memory().store32(module_thread.kernel_context + 0xF8u, 0xFFFFFFFFu);
    runtime.memory().store32(module_thread.kernel_context + 0xFCu, 0xFFFFFFFFu);
    runtime.cpu().set_gpr(26, module_thread.kernel_context);
    runtime.cpu().set_gpr(29, module_thread.kernel_context);
    thread_table.threads.emplace(0, std::move(module_thread));
    virtual_time_us = 0u;
    volatile_memory_locked = false;
    general_purpose_io = 0u;
    ge_edram_translation = 0u;
    ge_async_stop_worker();
    ge_callback_table = GeCallbackTable{};
    ge_state = GeState{};
    ++ge_draw_state_revision;
    ++ge_lighting_state_revision;
    ++ge_camera_state_revision;
    reset_ge_transform_state(ge_state.transform);
    ge_list_table = GeListTable{};
    {
        std::lock_guard lock(ge_async.mutex);
        ge_async.stop_requested = false;
        ge_async.fatal.store(false, std::memory_order_release);
        ge_async.fatal_reason.clear();
        ge_async.submitted = 0u;
        ge_async.completed = 0u;
        ge_async.wait_calls = 0u;
        ge_async.wait_time = std::chrono::steady_clock::duration{};
        ge_async.last_wait_ns.store(0u, std::memory_order_release);
    }
    pending_guest_callbacks.clear();
    async_return_frames.clear();
    display_state = DisplayState{};
    display_vblank_index = 0u;
    frame_time_stats = FrameTimeStats{};
    gpu_timing_census = GpuTimingCensus{};
    realtime_speed_stats = RealtimeSpeedStats{};
    frozen_clock_guard_dispatches = 0u;
    frozen_clock_guard_vblank = 0u;
    sub_interrupts.clear();
    memory_stick_fat_state = 1u;
    controller_state = ControllerState{};
    savedata_utility = SavedataUtilityState{};
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
    psprecomp::set_runtime_thread_identity(0, "module_start");
    runtime.register_function(0x00000000u, &vcs_module_thread_return, "psp_thread_return");
    runtime.register_function(0x00000004u, &vcs_interrupt_return, "psp_interrupt_return");
    runtime.register_function(0x08B562D8u, &vcs_sprintf, "vcs_sprintf");
    runtime.register_function(0x088B4FA8u, &vcs_path_hash, "vcs_path_hash");
    runtime.register_function(0x08B1B36Cu, &vcs_load_codec_modules, "vcs_load_codec_modules");
    if (std::getenv("PSPRECOMP_NO_FAST_DEFLATE") == nullptr)
        runtime.register_function(0x08B648B0u, &vcs_raw_deflate_fast, "vcs_raw_deflate_fast");
    runtime.register_hle("SysMemUserForUser", 0x7591C7DBu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            compiled_sdk_version = ctx.gpr[4];
            set_success(ctx);
        });
    runtime.register_hle("SysMemUserForUser", 0xF77D77CBu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            compiler_version = ctx.gpr[4];
            set_success(ctx);
        });

    runtime.register_hle("SysMemUserForUser", 0xA291F107u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // The bootstrap owns one contiguous user arena growing upward,
            // while thread stacks grow downward.  Report the actual gap.
            const std::uint32_t low = (partition_table.next_address + 0xFFu) & ~0xFFu;
            const std::uint32_t high = thread_table.next_stack_top & ~0xFFu;
            ctx.set_gpr(2, high > low ? high - low : 0u);
        });

    runtime.register_hle("SysMemUserForUser", 0x237DBD4Fu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[5] != 0u ? rt.memory().read_c_string(ctx.gpr[5], 128u) : "partition";
            const std::uint32_t size = ctx.gpr[7];
            const std::uint32_t alignment = 0x100u;
            const std::uint32_t aligned_size = (size + alignment - 1u) & ~(alignment - 1u);
            const std::uint32_t address = (partition_table.next_address + alignment - 1u) & ~(alignment - 1u);
            if (aligned_size == 0u || !rt.memory().contains(address, aligned_size)) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            rt.memory().zero(address, aligned_size);
            const std::int32_t uid = partition_table.next_uid++;
            partition_table.blocks.emplace(uid, PartitionBlock{name, address, aligned_size});
            partition_table.next_address = address + aligned_size;
            if (std::getenv("PSPRECOMP_PARTITION_DIAG") != nullptr) {
                std::cerr << "[partition] alloc uid=" << uid << " name=\"" << name
                          << "\" addr=" << psprecomp::hex32(address)
                          << " size=" << psprecomp::hex32(aligned_size)
                          << " next=" << psprecomp::hex32(partition_table.next_address)
                          << " stack_top=" << psprecomp::hex32(thread_table.next_stack_top) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("SysMemUserForUser", 0x9D9A5BA1u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = partition_table.blocks.find(uid);
            ctx.set_gpr(2, it == partition_table.blocks.end() ? 0u : it->second.address);
        });

    runtime.register_hle("SysMemUserForUser", 0xB6D61D02u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = partition_table.blocks.find(uid);
            if (std::getenv("PSPRECOMP_PARTITION_DIAG") != nullptr) {
                std::cerr << "[partition] free uid=" << uid;
                if (it != partition_table.blocks.end()) {
                    std::cerr << " name=\"" << it->second.name << "\" addr="
                              << psprecomp::hex32(it->second.address)
                              << " size=" << psprecomp::hex32(it->second.size);
                }
                std::cerr << "\n";
            }
            ctx.set_gpr(2, partition_table.blocks.erase(uid) == 1u ? 0u : 0x800200CBu);
        });

    runtime.register_hle("ThreadManForUser", 0x446D8DE6u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "unnamed";
            const std::uint32_t requested_stack = ctx.gpr[7];
            if (requested_stack < 0x200u) {
                ctx.set_gpr(2, 0x80020194u);
                return;
            }
            const std::uint32_t stack_size = (requested_stack + 0xFFu) & ~0xFFu;
            std::uint32_t stack_bottom = 0u;
            std::uint32_t stack_top = 0u;
            if (!allocate_thread_stack(stack_size, stack_bottom, stack_top) ||
                !rt.memory().contains(stack_bottom, stack_size)) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }

            ThreadRecord record{
                name,
                ctx.gpr[5],
                ctx.gpr[6],
                stack_size,
                ctx.gpr[8],
            };
            const std::int32_t uid = thread_table.next_uid++;
            record.stack_top = stack_top;
            record.stack_bottom = stack_bottom;
            record.kernel_context = stack_top - 0x100u;
            rt.memory().zero(stack_bottom, stack_size);
            rt.memory().store32(stack_bottom, static_cast<std::uint32_t>(uid));
            rt.memory().store32(record.kernel_context + 0xC0u, static_cast<std::uint32_t>(uid));
            rt.memory().store32(record.kernel_context + 0xC8u, stack_bottom);
            rt.memory().store32(record.kernel_context + 0xF8u, 0xFFFFFFFFu);
            rt.memory().store32(record.kernel_context + 0xFCu, 0xFFFFFFFFu);

            if (std::getenv("PSPRECOMP_TRACE") != nullptr || std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[sched] create uid=" << uid << " name=" << record.name
                          << " entry=" << psprecomp::hex32(record.entry)
                          << " priority=" << record.priority << " stack=" << record.stack_size
                          << " range=" << psprecomp::hex32(record.stack_bottom) << "-"
                          << psprecomp::hex32(record.stack_top) << "\n";
            }
            thread_table.threads.emplace(uid, std::move(record));
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("ThreadManForUser", 0xF475845Du,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = thread_table.threads.find(uid);
            if (it == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            ThreadRecord &thread = it->second;
            if (thread.state != ThreadState::Created) {
                ctx.set_gpr(2, 0x800201A4u);
                return;
            }

            const std::uint32_t arg_size = ctx.gpr[5];
            const std::uint32_t arg_ptr = ctx.gpr[6];
            std::uint32_t sp = thread.kernel_context;

            psprecomp::AllegrexContext next{};
            if (arg_ptr != 0u && arg_size != 0u) {
                const std::uint32_t aligned_args = (arg_size + 0xFu) & ~0xFu;
                if (sp < thread.stack_bottom + aligned_args + 64u ||
                    !rt.memory().contains(arg_ptr, arg_size)) {
                    ctx.set_gpr(2, 0x800200D3u);
                    return;
                }
                sp -= aligned_args;
                std::vector<std::uint8_t> arguments(arg_size);
                rt.memory().copy_out(arg_ptr, arguments);
                rt.memory().copy_in(sp, arguments);
                next.set_gpr(4, arg_size);
                next.set_gpr(5, sp);
            } else {
                next.set_gpr(4, 0u);
                next.set_gpr(5, 0u);
            }

            // The PSP kernel consumes another 64 bytes and places the thread
            // return trampoline at the bottom of that frame.  Address zero is
            // registered as the native thread-return target in this runtime.
            sp -= 64u;
            next.set_gpr(26, thread.kernel_context);
            next.set_gpr(28, ctx.gpr[28]);
            next.set_gpr(29, sp);
            next.set_gpr(30, sp);
            next.set_gpr(31, 0u);
            next.pc = thread.entry;
            enqueue_continuation(uid, next);

            const std::int32_t caller_uid = thread_table.current_uid;
            const std::uint32_t caller_priority = thread_priority(caller_uid);
            if (std::getenv("PSPRECOMP_TRACE") != nullptr || std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[sched] start uid=" << uid << " name=" << thread.name
                          << " entry=" << psprecomp::hex32(thread.entry)
                          << " priority=" << thread.priority
                          << " caller=" << caller_uid
                          << " caller_priority=" << caller_priority << "\n";
            }

            // Starting a thread does not automatically hand it the CPU.  It
            // only preempts when its numeric PSP priority is strictly higher.
            if (thread.priority < caller_priority) {
                psprecomp::AllegrexContext caller = ctx;
                caller.set_gpr(2, 0u);
                caller.pc = ctx.gpr[31];
                enqueue_continuation(caller_uid, caller);
                (void)activate_next_thread(ctx, "thread-control");
            } else {
                set_success(ctx);
            }
        });

    runtime.register_hle("ThreadManForUser", 0x809CE29Bu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            complete_current_thread(rt, ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x383F7BCCu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            const bool was_active = found->second.state != ThreadState::Created &&
                                    found->second.state != ThreadState::Completed;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] terminate-delete uid=" << uid
                          << " name=" << found->second.name
                          << " active=" << (was_active ? 1 : 0) << "\n";
            }
            thread_table.continuations.erase(
                std::remove_if(thread_table.continuations.begin(), thread_table.continuations.end(),
                    [uid](const ThreadContinuation &item) { return item.uid == uid; }),
                thread_table.continuations.end());
            pending_guest_callbacks.erase(uid);
            async_return_frames.erase(uid);
            deferred_io_resumes.erase(uid);
            refresh_vcs_post_dispatch_hook();
            if (was_active) wake_thread_end_waiters(uid, 0x800201ACu);
            else wake_thread_end_waiters(uid, 0u);
            remove_thread_from_wait_queues(uid);
            release_thread_stack(found->second);
            thread_table.threads.erase(found);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x9FA03CD3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x800201A4u);  // SCE_KERNEL_ERROR_NOT_DORMANT
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            if (found->second.state != ThreadState::Created &&
                found->second.state != ThreadState::Completed) {
                ctx.set_gpr(2, 0x800201A4u);
                return;
            }
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr)
                std::cerr << "[thread] delete uid=" << uid << " name=" << found->second.name << "\n";
            remove_thread_from_wait_queues(uid);
            pending_guest_callbacks.erase(uid);
            async_return_frames.erase(uid);
            deferred_io_resumes.erase(uid);
            refresh_vcs_post_dispatch_hook();
            release_thread_stack(found->second);
            thread_table.threads.erase(found);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x9944F31Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            // The PSP does not accept 0 as an alias for the current thread
            // here.  Suspending the caller (explicitly or through 0) is
            // illegal; only another live thread may be suspended.
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] suspend requested=" << uid
                          << " current=" << thread_table.current_uid
                          << " name=" << thread.name << " state=" << static_cast<int>(thread.state)
                          << " continuations=" << thread_table.continuations.size() << "\n";
            }
            if (thread.state == ThreadState::Completed || thread.state == ThreadState::Created) {
                ctx.set_gpr(2, 0x800201A2u);  // SCE_KERNEL_ERROR_DORMANT
                return;
            }
            if (thread.externally_suspended) {
                ctx.set_gpr(2, 0x800201A3u);  // SCE_KERNEL_ERROR_SUSPEND
                return;
            }

            thread.externally_suspended = true;
            const auto continuation = std::find_if(
                thread_table.continuations.begin(), thread_table.continuations.end(),
                [uid](const ThreadContinuation &item) { return item.uid == uid; });
            if (continuation != thread_table.continuations.end()) {
                thread.suspended_context = continuation->context;
                thread.state = ThreadState::Ready;
                thread_table.continuations.erase(continuation);
            }
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x75156E8Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0 || uid == thread_table.current_uid) {
                ctx.set_gpr(2, 0x80020197u);  // SCE_KERNEL_ERROR_ILLEGAL_THID
                return;
            }
            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] resume requested=" << uid
                          << " current=" << thread_table.current_uid
                          << " name=" << thread.name << " state=" << static_cast<int>(thread.state)
                          << " continuations=" << thread_table.continuations.size() << "\n";
            }
            if (!thread.externally_suspended) {
                ctx.set_gpr(2, 0x800201A5u);  // SCE_KERNEL_ERROR_NOT_SUSPEND
                return;
            }
            thread.externally_suspended = false;
            if (thread.state == ThreadState::Ready)
                enqueue_continuation(uid, thread.suspended_context);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "thread-resume");
        });

    runtime.register_hle("ThreadManForUser", 0x293B45B8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(thread_table.current_uid));
        });

    runtime.register_hle("ThreadManForUser", 0x71BC9871u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            if (uid == 0)
                uid = thread_table.current_uid;

            std::uint32_t priority = ctx.gpr[5];
            if (priority == 0u)
                priority = thread_priority(thread_table.current_uid);

            const auto found = thread_table.threads.find(uid);
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);  // SCE_KERNEL_ERROR_UNKNOWN_THID
                return;
            }
            ThreadRecord &thread = found->second;
            if (thread.state == ThreadState::Created || thread.state == ThreadState::Completed) {
                ctx.set_gpr(2, 0x800201A2u);  // SCE_KERNEL_ERROR_DORMANT
                return;
            }
            if (priority < 0x08u || priority > 0x77u) {
                ctx.set_gpr(2, 0x80020193u);  // SCE_KERNEL_ERROR_ILLEGAL_PRIORITY
                return;
            }

            thread.priority = priority;
            if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr) {
                std::cerr << "[thread] priority uid=" << uid
                          << " current=" << thread_table.current_uid
                          << " value=" << priority << "\n";
            }

            // Changing priority is a scheduling point on the PSP.  Save the
            // current HLE return state only when a strictly higher-priority
            // ready thread exists, then let the normal dispatcher select it.
            const auto best = best_ready_thread();
            const std::uint32_t current_priority = thread_priority(thread_table.current_uid);
            if (best != thread_table.continuations.end() &&
                thread_priority(best->uid) < current_priority) {
                const std::int32_t caller_uid = thread_table.current_uid;
                psprecomp::AllegrexContext caller = ctx;
                caller.set_gpr(2, 0u);
                caller.pc = ctx.gpr[31];
                enqueue_continuation(caller_uid, caller);
                (void)activate_next_thread(ctx, "thread-control");
                return;
            }
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x110DEC9Au,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t output = ctx.gpr[5];
            if (!rt.memory().contains(output, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            rt.memory().store32(output, ctx.gpr[4]);
            rt.memory().store32(output + 4u, 0u);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xC8CD158Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4]);
            ctx.set_gpr(3, 0u);
        });

    runtime.register_hle("ThreadManForUser", 0xBA6B92E2u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t clock = ctx.gpr[4];
            const std::uint32_t seconds_out = ctx.gpr[5];
            const std::uint32_t usec_out = ctx.gpr[6];
            if (!rt.memory().contains(clock, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint64_t ticks = static_cast<std::uint64_t>(rt.memory().load32(clock)) |
                (static_cast<std::uint64_t>(rt.memory().load32(clock + 4u)) << 32u);
            if (rt.memory().contains(seconds_out, 4u))
                rt.memory().store32(seconds_out, static_cast<std::uint32_t>(ticks / 1'000'000u));
            if (rt.memory().contains(usec_out, 4u))
                rt.memory().store32(usec_out, static_cast<std::uint32_t>(ticks % 1'000'000u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xE1619D7Cu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t ticks = static_cast<std::uint64_t>(ctx.gpr[4]) |
                (static_cast<std::uint64_t>(ctx.gpr[5]) << 32u);
            if (rt.memory().contains(ctx.gpr[6], 4u))
                rt.memory().store32(ctx.gpr[6], static_cast<std::uint32_t>(ticks / 1'000'000u));
            if (rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], static_cast<std::uint32_t>(ticks % 1'000'000u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xDB738F35u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t output = ctx.gpr[4];
            if (!rt.memory().contains(output, 8u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            const std::uint64_t usec = system_time_microseconds();
            rt.memory().store32(output, static_cast<std::uint32_t>(usec));
            rt.memory().store32(output + 4u, static_cast<std::uint32_t>(usec >> 32u));
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x82BC5777u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t usec = system_time_microseconds();
            ctx.set_gpr(2, static_cast<std::uint32_t>(usec));
            ctx.set_gpr(3, static_cast<std::uint32_t>(usec >> 32u));
        });

    runtime.register_hle("ThreadManForUser", 0x369ED59Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(system_time_microseconds()));
        });

    // The PSP profiler query APIs return a null profiler register block in
    // ordinary user-mode execution.  VCS probes both during startup.
    const auto refer_profiler = [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
        ctx.set_gpr(2, 0u);
    };
    runtime.register_hle("ThreadManForUser", 0x64D4540Eu, refer_profiler);
    runtime.register_hle("ThreadManForUser", 0x8218B4DDu, refer_profiler);

    runtime.register_hle("ThreadManForUser", 0xEA748E31u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // PSPSDK documents the first argument as reserved/zero.  VCS uses
            // this call to opt the current thread into VFPU context handling.
            const std::uint32_t reserved = ctx.gpr[4];
            const std::uint32_t attributes = ctx.gpr[5];
            if (reserved != 0u) {
                ctx.set_gpr(2, 0x800200D2u);
                return;
            }
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.attributes |= attributes;
            }
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] sceKernelChangeCurrentThreadAttr uid="
                          << thread_table.current_uid << " add=0x" << std::hex
                          << std::uppercase << attributes << std::dec << "\n";
            }
            set_success(ctx);
        });

    auto sleep_thread = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        (void)sleep_current_thread(rt, ctx);
    };
    runtime.register_hle("ThreadManForUser", 0x9ACE131Eu, sleep_thread);
    runtime.register_hle("ThreadManForUser", 0x82826F70u, sleep_thread);
    runtime.register_hle("ThreadManForUser", 0xD59EAD2Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t result = wake_thread(static_cast<std::int32_t>(ctx.gpr[4]));
            ctx.set_gpr(2, result);
            if (result == 0u) (void)preempt_if_higher_priority(ctx, "thread-wakeup");
        });
    runtime.register_hle("ThreadManForUser", 0xFCCFAD26u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = thread_table.threads.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (found == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            const std::uint32_t previous = found->second.wakeup_count;
            found->second.wakeup_count = 0u;
            ctx.set_gpr(2, previous);
        });

    runtime.register_hle("ThreadManForUser", 0xAA73C935u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.exit_status = ctx.gpr[4];
                if (std::getenv("PSPRECOMP_THREAD_DIAG") != nullptr)
                    std::cerr << "[thread] exit uid=" << thread_table.current_uid
                              << " name=" << current->second.name
                              << " status=" << psprecomp::hex32(ctx.gpr[4]) << "\n";
            }
            complete_current_thread(rt, ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x278C0DF5u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = thread_table.threads.find(uid);
            if (uid <= 0 || it == thread_table.threads.end()) {
                ctx.set_gpr(2, 0x80020198u);
                return;
            }
            if (it->second.state == ThreadState::Completed) {
                set_success(ctx);
                return;
            }
            psprecomp::AllegrexContext waiter = ctx;
            waiter.set_gpr(2, 0u);
            waiter.pc = ctx.gpr[31];
            thread_table.thread_end_waiters[uid].push_back({thread_table.current_uid, waiter});
            if (auto current = thread_table.threads.find(thread_table.current_uid);
                current != thread_table.threads.end()) {
                current->second.state = ThreadState::Sleeping;
                current->second.suspended_context = waiter;
            }
            if (!activate_next_thread(ctx, "kernel-wait")) {
                rt.stop("PSP thread wait deadlock on uid " + std::to_string(uid));
            }
        });

    auto delay_thread = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        (void)delay_current_thread(rt, ctx, ctx.gpr[4]);
    };
    runtime.register_hle("ThreadManForUser", 0xCEADEB47u, delay_thread);
    runtime.register_hle("ThreadManForUser", 0x68DA9E36u, delay_thread);

    runtime.register_hle("ThreadManForUser", 0xE81CAF8Fu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "callback";
            const std::int32_t uid = callback_table.next_uid++;
            callback_table.callbacks.emplace(uid, CallbackRecord{
                name, ctx.gpr[5], ctx.gpr[6], thread_table.current_uid, 0u, 0u});
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0xEDBA5844u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            ctx.set_gpr(2, callback_table.callbacks.erase(uid) == 1u ? 0u : 0x800201A1u);
        });

    runtime.register_hle("ThreadManForUser", 0x349D6D6Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // Even an empty callback checkpoint consumes CPU time on real PSP
            // hardware. Without this, a busy polling thread can freeze virtual
            // time forever and starve delayed video/audio workers.
            virtual_time_us += 25u;
            promote_expired_delays();

            auto pending = std::find_if(callback_table.callbacks.begin(), callback_table.callbacks.end(),
                [](const auto &item) {
                    return item.second.owner_uid == thread_table.current_uid &&
                        item.second.notify_count != 0u && item.second.function != 0u;
                });
            if (pending == callback_table.callbacks.end()) {
                set_success(ctx);
                (void)preempt_if_higher_priority(ctx, "check-callback");
                return;
            }

            CallbackRecord &callback = pending->second;
            const std::uint32_t count = callback.notify_count;
            const std::uint32_t argument = callback.notify_argument;
            callback.notify_count = 0u;

            psprecomp::AllegrexContext resume = ctx;
            resume.pc = ctx.gpr[31];
            resume.set_gpr(2, 1u);
            auto &frames = async_return_frames[thread_table.current_uid];
            if (!frames.empty()) {
                ctx.set_gpr(2, 0u);
                return;
            }
            frames.push_back(AsyncReturnFrame{AsyncReturnKind::UserCallback, resume, 0u, 0, 0, 0, pending->first});
            ctx.set_gpr(4, count);
            ctx.set_gpr(5, argument);
            ctx.set_gpr(6, callback.common);
            ctx.set_gpr(31, 0x00000004u);
            ctx.pc = callback.function;
        });

    runtime.register_hle("ThreadManForUser", 0xD6DA4BA1u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "semaphore";
            const auto initial = static_cast<std::int32_t>(ctx.gpr[6]);
            const auto maximum = static_cast<std::int32_t>(ctx.gpr[7]);
            if (initial < 0 || maximum <= 0 || initial > maximum) {
                ctx.set_gpr(2, 0x800201B0u);
                return;
            }
            const std::int32_t uid = semaphore_table.next_uid++;
            semaphore_table.semaphores.emplace(uid, SemaphoreRecord{name, initial, maximum, {}});
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0x28B6489Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = semaphore_table.semaphores.find(uid);
            if (found == semaphore_table.semaphores.end()) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            for (auto &waiter : found->second.waiters) {
                waiter.context.set_gpr(2, 0x800201A7u);
                enqueue_continuation(waiter.uid, waiter.context);
            }
            semaphore_table.semaphores.erase(found);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "semaphore-delete");
        });
    runtime.register_hle("ThreadManForUser", 0x3F53E640u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto it = semaphore_table.semaphores.find(uid);
            if (it == semaphore_table.semaphores.end() || amount <= 0 ||
                static_cast<std::int64_t>(it->second.count) + amount > it->second.maximum) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            SemaphoreRecord &semaphore = it->second;
            semaphore.count += amount;
            auto waiter = semaphore.waiters.begin();
            while (waiter != semaphore.waiters.end()) {
                if (semaphore.count >= waiter->requested) {
                    semaphore.count -= waiter->requested;
                    waiter->context.set_gpr(2, 0u);
                    enqueue_continuation(waiter->uid, waiter->context);
                    waiter = semaphore.waiters.erase(waiter);
                } else {
                    ++waiter;
                }
            }
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "semaphore-signal");
        });
    auto semaphore_wait = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
        const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
        const auto it = semaphore_table.semaphores.find(uid);
        if (it == semaphore_table.semaphores.end() || amount <= 0 || amount > it->second.maximum) {
            ctx.set_gpr(2, 0x80020199u);
            return;
        }
        if (it->second.count >= amount) {
            it->second.count -= amount;
            set_success(ctx);
            return;
        }
        const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
        it->second.waiters.push_back(SemaphoreWaiter{thread_table.current_uid, suspended, amount});
        (void)suspend_current_thread(rt, ctx, suspended, "semaphore " + std::to_string(uid));
    };
    runtime.register_hle("ThreadManForUser", 0x4E3A1105u, semaphore_wait);
    runtime.register_hle("ThreadManForUser", 0x6D212BACu, semaphore_wait);
    runtime.register_hle("ThreadManForUser", 0x58B1F937u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto amount = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto it = semaphore_table.semaphores.find(uid);
            if (it == semaphore_table.semaphores.end() || amount <= 0 || amount > it->second.maximum) {
                ctx.set_gpr(2, 0x80020199u);
                return;
            }
            if (it->second.count < amount) {
                ctx.set_gpr(2, 0x800201AEu);
                return;
            }
            it->second.count -= amount;
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0x55C20A00u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "event_flag";
            const std::int32_t uid = event_flag_table.next_uid++;
            event_flag_table.flags.emplace(uid, EventFlagRecord{name, ctx.gpr[5], ctx.gpr[6], ctx.gpr[6], {}});
            if (event_diag_matches(event_flag_table.flags.at(uid))) {
                std::cerr << "[event] create uid=" << uid << " name=\"" << name << "\""
                          << " attr=" << psprecomp::hex32(ctx.gpr[5])
                          << " initial=" << psprecomp::hex32(ctx.gpr[6])
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ThreadManForUser", 0xEF9E4C70u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = event_flag_table.flags.find(uid);
            if (found == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            for (auto &waiter : found->second.waiters) {
                waiter.context.set_gpr(2, 0x800201A7u);
                enqueue_continuation(waiter.uid, waiter.context);
            }
            event_flag_table.flags.erase(found);
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "event-flag-delete");
        });
    runtime.register_hle("ThreadManForUser", 0x1FB15A32u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            EventFlagRecord &flag = it->second;
            const std::uint32_t previous_pattern = flag.current_pattern;
            flag.current_pattern |= ctx.gpr[5];
            if (event_diag_matches(flag)) {
                std::cerr << "[event] set uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << flag.name << "\""
                          << " bits=" << psprecomp::hex32(ctx.gpr[5])
                          << " old=" << psprecomp::hex32(previous_pattern)
                          << " new=" << psprecomp::hex32(flag.current_pattern)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            auto waiter = flag.waiters.begin();
            while (waiter != flag.waiters.end()) {
                if (!event_flag_matches(flag, waiter->requested, waiter->mode)) {
                    ++waiter;
                    continue;
                }
                if (waiter->output_address != 0u && rt.memory().contains(waiter->output_address, 4u))
                    rt.memory().store32(waiter->output_address, flag.current_pattern);
                consume_event_flag(flag, waiter->requested, waiter->mode);
                waiter->context.set_gpr(2, 0u);
                enqueue_continuation(waiter->uid, waiter->context);
                waiter = flag.waiters.erase(waiter);
            }
            set_success(ctx);
            (void)preempt_if_higher_priority(ctx, "event-flag-set");
        });
    runtime.register_hle("ThreadManForUser", 0x812346E4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            // PSP clear semantics retain only the bits present in the mask.
            const std::uint32_t previous_pattern = it->second.current_pattern;
            it->second.current_pattern &= ctx.gpr[5];
            if (event_diag_matches(it->second)) {
                std::cerr << "[event] clear uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << it->second.name << "\""
                          << " mask=" << psprecomp::hex32(ctx.gpr[5])
                          << " old=" << psprecomp::hex32(previous_pattern)
                          << " new=" << psprecomp::hex32(it->second.current_pattern)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            set_success(ctx);
        });
    auto event_flag_wait = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
        const auto it = event_flag_table.flags.find(uid);
        if (it == event_flag_table.flags.end()) {
            ctx.set_gpr(2, 0x8002019Au);
            return;
        }
        const std::uint32_t requested = ctx.gpr[5];
        const std::uint32_t mode = ctx.gpr[6];
        if (requested == 0u || (mode & ~0x31u) != 0u) {
            ctx.set_gpr(2, 0x800201B1u);
            return;
        }
        if (event_flag_matches(it->second, requested, mode)) {
            if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
            consume_event_flag(it->second, requested, mode);
            set_success(ctx);
            return;
        }
        const psprecomp::AllegrexContext suspended = make_wait_context(ctx);
        it->second.waiters.push_back(EventFlagWaiter{
            thread_table.current_uid, suspended, requested, mode, ctx.gpr[7]});
        (void)suspend_current_thread(rt, ctx, suspended, "event flag " + std::to_string(uid));
    };
    runtime.register_hle("ThreadManForUser", 0x402FCF22u, event_flag_wait);
    runtime.register_hle("ThreadManForUser", 0x328C546Au, event_flag_wait);
    runtime.register_hle("ThreadManForUser", 0x30FD48F0u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto it = event_flag_table.flags.find(static_cast<std::int32_t>(ctx.gpr[4]));
            if (it == event_flag_table.flags.end()) {
                ctx.set_gpr(2, 0x8002019Au);
                return;
            }
            const std::uint32_t requested = ctx.gpr[5];
            const std::uint32_t mode = ctx.gpr[6];
            const bool diag = event_diag_matches(it->second);
            if (diag) ++event_diag_poll_count;
            if (diag && (event_diag_poll_count <= 32u || event_diag_poll_count % 100000u == 0u)) {
                std::cerr << "[event] poll count=" << event_diag_poll_count
                          << " uid=" << static_cast<std::int32_t>(ctx.gpr[4])
                          << " name=\"" << it->second.name << "\""
                          << " requested=" << psprecomp::hex32(requested)
                          << " mode=" << psprecomp::hex32(mode)
                          << " current=" << psprecomp::hex32(it->second.current_pattern)
                          << " match=" << (event_flag_matches(it->second, requested, mode) ? 1 : 0)
                          << " thread=" << thread_table.current_uid
                          << " dispatch_pc=" << psprecomp::hex32(psprecomp::runtime_dispatch_pc())
                          << " ctx_pc=" << psprecomp::hex32(ctx.pc)
                          << " ra=" << psprecomp::hex32(ctx.gpr[31]) << "\n";
            }
            if (diag && !event_diag_stall_reported && event_diag_stop_polls != 0u &&
                event_diag_poll_count >= event_diag_stop_polls) {
                event_diag_stall_reported = true;
                dump_event_stall_state(it->second, static_cast<std::int32_t>(ctx.gpr[4]), ctx);
                rt.stop("Target event flag exceeded PSPRECOMP_EVENT_DIAG_STOP_POLLS; scheduler state captured.");
                return;
            }
            if (!event_flag_matches(it->second, requested, mode)) {
                if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                    rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
                ctx.set_gpr(2, 0x800201AFu);
                return;
            }
            if (ctx.gpr[7] != 0u && rt.memory().contains(ctx.gpr[7], 4u))
                rt.memory().store32(ctx.gpr[7], it->second.current_pattern);
            consume_event_flag(it->second, requested, mode);
            set_success(ctx);
        });

    runtime.register_hle("ThreadManForUser", 0xC07BB470u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string name = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : "fpl";
            const std::uint32_t block_size = ctx.gpr[7];
            const std::uint32_t block_count = ctx.gpr[8];
            if (block_size == 0u || block_count == 0u ||
                block_size > 0xFFFFFFFFu / block_count) {
                ctx.set_gpr(2, 0x800201B0u);
                return;
            }
            const std::uint32_t alignment = 0x100u;
            const std::uint32_t total = block_size * block_count;
            const std::uint32_t address = (partition_table.next_address + alignment - 1u) & ~(alignment - 1u);
            const std::uint32_t reserved = (total + alignment - 1u) & ~(alignment - 1u);
            if (!rt.memory().contains(address, reserved) || address + reserved > thread_table.next_stack_top) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            rt.memory().zero(address, reserved);
            const std::int32_t uid = fixed_pool_table.next_uid++;
            fixed_pool_table.pools.emplace(uid, FixedPoolRecord{name, address, block_size, block_count,
                std::vector<bool>(block_count, false)});
            partition_table.next_address = address + reserved;
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] sceKernelCreateFpl uid=" << uid << " name=" << name
                          << " block=0x" << std::hex << std::uppercase << block_size
                          << " count=" << std::dec << block_count << " base=0x"
                          << std::hex << std::uppercase << address << std::dec << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });

    runtime.register_hle("ThreadManForUser", 0xD979E9BFu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t output = ctx.gpr[5];
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end() || !rt.memory().contains(output, 4u)) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            auto &pool = it->second;
            const auto free_it = std::find(pool.allocated.begin(), pool.allocated.end(), false);
            if (free_it == pool.allocated.end()) {
                ctx.set_gpr(2, 0x80020190u);
                return;
            }
            const std::size_t index = static_cast<std::size_t>(free_it - pool.allocated.begin());
            pool.allocated[index] = true;
            rt.memory().store32(output, pool.address + static_cast<std::uint32_t>(index) * pool.block_size);
            set_success(ctx);
        });

    // sceKernelTryAllocateFpl. The allocation above never blocks, so the
    // try-form is the same call: it either has a free block or it does not.
    runtime.register_hle("ThreadManForUser", 0x623AE665u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            rt.invoke_import("ThreadManForUser", 0xD979E9BFu, ctx);
        });

    // sceKernelFreeFpl.
    runtime.register_hle("ThreadManForUser", 0xF6414A71u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t block = ctx.gpr[5];
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end()) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            auto &pool = it->second;
            if (block < pool.address || pool.block_size == 0u) {
                ctx.set_gpr(2, 0x800201A9u);
                return;
            }
            const std::uint32_t offset = block - pool.address;
            const std::size_t index = offset / pool.block_size;
            if (offset % pool.block_size != 0u || index >= pool.allocated.size()) {
                ctx.set_gpr(2, 0x800201A9u);
                return;
            }
            pool.allocated[index] = false;
            set_success(ctx);
        });

    // sceKernelDeleteFpl. Loading a saved game from inside the game tears down
    // the previous session's pools, which is why this only ever mattered there:
    // the import was missing and the runtime stopped on a black screen.
    runtime.register_hle("ThreadManForUser", 0xED1410E0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto it = fixed_pool_table.pools.find(uid);
            if (it == fixed_pool_table.pools.end()) {
                ctx.set_gpr(2, 0x800201A8u);
                return;
            }
            // The partition allocator only ever bumps a cursor. Returning the
            // memory when this pool happens to be the most recent allocation
            // costs one comparison and is what keeps a create/delete cycle --
            // exactly what repeated in-game loads are -- from walking the
            // cursor into the thread stacks and failing the fourth or fifth
            // time. Older pools still leak their range until a proper
            // allocator exists.
            const auto &pool = it->second;
            constexpr std::uint32_t alignment = 0x100u;
            const std::uint32_t reserved =
                (pool.block_size * pool.block_count + alignment - 1u) & ~(alignment - 1u);
            if (pool.address + reserved == partition_table.next_address)
                partition_table.next_address = pool.address;
            fixed_pool_table.pools.erase(it);
            set_success(ctx);
        });

    auto volatile_mem_lock = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        constexpr std::uint32_t volatile_base = 0x08400000u;
        constexpr std::uint32_t volatile_size = 0x00400000u;
        if (ctx.gpr[4] != 0u) {
            ctx.set_gpr(2, 0x80000107u);
            return;
        }
        if (volatile_memory_locked) {
            ctx.set_gpr(2, 0x80000021u);
            return;
        }
        if (!rt.memory().contains(volatile_base, volatile_size) ||
            !rt.memory().contains(ctx.gpr[5], 4u) || !rt.memory().contains(ctx.gpr[6], 4u)) {
            ctx.set_gpr(2, 0x800200D3u);
            return;
        }
        rt.memory().store32(ctx.gpr[5], volatile_base);
        rt.memory().store32(ctx.gpr[6], volatile_size);
        rt.memory().zero(volatile_base, volatile_size);
        volatile_memory_locked = true;
        set_success(ctx);
    };
    runtime.register_hle("sceSuspendForUser", 0x3E0271D3u, volatile_mem_lock);
    runtime.register_hle("sceSuspendForUser", 0xA14F40B2u, volatile_mem_lock);
    runtime.register_hle("sceSuspendForUser", 0xA569E425u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] != 0u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            if (!volatile_memory_locked) {
                ctx.set_gpr(2, 0x800201AEu);
                return;
            }
            volatile_memory_locked = false;
            set_success(ctx);
        });
    runtime.register_hle("sceSuspendForUser", 0xEADB1BD7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4] == 0u ? 0u : 0x80000107u);
        });
    runtime.register_hle("sceSuspendForUser", 0x3AEE7261u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, ctx.gpr[4] == 0u ? 0u : 0x80000107u);
        });
    runtime.register_hle("sceSuspendForUser", 0x090CCB3Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });

    runtime.register_hle("UtilsForUser", 0x37FB5C42u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, general_purpose_io);
        });
    runtime.register_hle("UtilsForUser", 0x6AD345D7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            general_purpose_io = ctx.gpr[4];
            set_success(ctx);
        });

    // The statically recompiled CPU and host share one coherent guest-memory
    // backing store. Data-cache maintenance is therefore complete at the
    // call boundary. Instruction-cache invalidation is recorded as success;
    // dynamically loaded executable modules are handled by the PRX loader,
    // rather than by mutating the generated host code in place.
    auto cache_maintenance = [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
        set_success(ctx);
    };
    runtime.register_hle("UtilsForUser", 0xBFA98062u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x79D1C3FAu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0xB435DEC5u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x3EE30821u, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x34B9FA9Eu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x920F104Au, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0xC2DF770Eu, cache_maintenance);
    runtime.register_hle("UtilsForUser", 0x80001C4Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0x16641D70u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0x4FD31C9Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    runtime.register_hle("UtilsForUser", 0xFB05FAD0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });

    runtime.register_hle("sceCtrl", 0x6A2774F3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t previous = controller_state.sampling_cycle;
            controller_state.sampling_cycle = ctx.gpr[4];
            ctx.set_gpr(2, previous);
        });
    runtime.register_hle("sceCtrl", 0x02BAAD91u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 4u)) {
                ctx.set_gpr(2, 0x80000103u);
                return;
            }
            rt.memory().store32(ctx.gpr[4], controller_state.sampling_cycle);
            set_success(ctx);
        });
    runtime.register_hle("sceCtrl", 0x1F4011E6u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] > 1u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            const std::uint32_t previous = controller_state.sampling_mode;
            controller_state.sampling_mode = ctx.gpr[4];
            ctx.set_gpr(2, previous);
        });
    runtime.register_hle("sceCtrl", 0xDA6B76A1u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 4u)) {
                ctx.set_gpr(2, 0x80000103u);
                return;
            }
            rt.memory().store32(ctx.gpr[4], controller_state.sampling_mode);
            set_success(ctx);
        });

    auto write_controller_samples = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx, bool negative) {
        const std::uint32_t destination = ctx.gpr[4];
        const std::uint32_t count = ctx.gpr[5];
        constexpr std::uint32_t sample_size = 16u;
        if (count == 0u) {
            ctx.set_gpr(2, 0u);
            return;
        }
        if (count > 64u || !rt.memory().contains(destination, static_cast<std::size_t>(count) * sample_size)) {
            ctx.set_gpr(2, 0x80000103u);
            return;
        }
        for (std::uint32_t index = 0u; index < count; ++index) {
            const std::uint32_t sample = destination + index * sample_size;
            rt.memory().store32(sample, static_cast<std::uint32_t>(system_time_microseconds()));
            const std::uint32_t buttons = negative ? ~effective_controller_buttons() : effective_controller_buttons();
            rt.memory().store32(sample + 4u, buttons);
            const auto [analog_x, analog_y] = effective_controller_analog();
            rt.memory().store8(sample + 8u, controller_state.sampling_mode != 0u ? analog_x : 128u);
            rt.memory().store8(sample + 9u, controller_state.sampling_mode != 0u ? analog_y : 128u);
            rt.memory().store8(sample + 10u, controller_state.rx);
            rt.memory().store8(sample + 11u, controller_state.ry);
            rt.memory().zero(sample + 12u, 4u);
        }
        ctx.set_gpr(2, count);
    };
    runtime.register_hle("sceCtrl", 0x3A622550u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, false); });
    runtime.register_hle("sceCtrl", 0x1F803938u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, false); });
    runtime.register_hle("sceCtrl", 0xC152080Au,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, true); });
    runtime.register_hle("sceCtrl", 0x60B81F86u,
        [write_controller_samples](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { write_controller_samples(rt, ctx, true); });
    auto controller_latch = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 16u)) {
            ctx.set_gpr(2, 0x80000103u);
            return;
        }
        rt.memory().store32(ctx.gpr[4] + 0u, 0u);
        rt.memory().store32(ctx.gpr[4] + 4u, 0u);
        rt.memory().store32(ctx.gpr[4] + 8u, effective_controller_buttons());
        rt.memory().store32(ctx.gpr[4] + 12u, ~effective_controller_buttons());
        ctx.set_gpr(2, 0u);
    };
    runtime.register_hle("sceCtrl", 0xB1D0E5CDu, controller_latch);
    runtime.register_hle("sceCtrl", 0x0B588501u, controller_latch);

    runtime.register_hle("sceGe_user", 0xE47E40E4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, psprecomp::GuestMemory::kVramPhysicalBase);
        });
    runtime.register_hle("sceGe_user", 0x1F6752ADu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, psprecomp::GuestMemory::kVramSize);
        });
    runtime.register_hle("sceGe_user", 0xB77905EAu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t requested = ctx.gpr[4];
            const bool valid_range = requested == 0u || (requested >= 0x200u && requested <= 0x1000u);
            const bool power_of_two = requested == 0u || (requested & (requested - 1u)) == 0u;
            if (!valid_range || !power_of_two) {
                ctx.set_gpr(2, 0x800001FEu);
                return;
            }
            const std::uint32_t previous = ge_edram_translation;
            ge_edram_translation = requested;
            ctx.set_gpr(2, previous);
        });

    runtime.register_hle("sceGe_user", 0xA4FC06A4u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!ge_async_wait_idle(rt)) return;
            const std::uint32_t callback_data = ctx.gpr[4];
            if (callback_data == 0u || !rt.memory().contains(callback_data, 16u)) {
                ctx.set_gpr(2, 0x800200D3u);  // SCE_KERNEL_ERROR_ILLEGAL_ADDR
                return;
            }
            GeCallbackRecord record{
                rt.memory().load32(callback_data + 0u),
                rt.memory().load32(callback_data + 4u),
                rt.memory().load32(callback_data + 8u),
                rt.memory().load32(callback_data + 12u),
            };
            const std::int32_t uid = ge_callback_table.next_uid++;
            ge_callback_table.callbacks.emplace(uid, record);
            if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr) {
                std::cerr << "[ge] callback uid=" << uid
                          << " signal=" << psprecomp::hex32(record.signal_function)
                          << " finish=" << psprecomp::hex32(record.finish_function) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("sceGe_user", 0x05DB22CEu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!ge_async_wait_idle(rt)) return;
            const std::int32_t uid = static_cast<std::int32_t>(ctx.gpr[4]);
            ctx.set_gpr(2, ge_callback_table.callbacks.erase(uid) == 1u ? 0u : 0x80000100u);
        });

    runtime.register_hle("sceGe_user", 0xAB49E76Au,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            enqueue_ge_display_list(rt, ctx, false);
        });
    runtime.register_hle("sceGe_user", 0x1C0D95A6u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            enqueue_ge_display_list(rt, ctx, true);
        });
    runtime.register_hle("sceGe_user", 0x5FB86AB0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t id = ctx.gpr[4];
            if (ge_async_running()) {
                std::lock_guard lock(ge_async.mutex);
                const auto found = ge_list_table.lists.find(id);
                if (found == ge_list_table.lists.end()) {
                    ctx.set_gpr(2, 0x80000100u);
                    return;
                }
                if (found->second.state == GeListState::Running) {
                    ctx.set_gpr(2, 0x800201A7u);
                    return;
                }
                if (found->second.state == GeListState::Queued) {
                    const auto before = ge_async.pending.size();
                    std::erase_if(ge_async.pending, [id](const GeAsyncTask &task) { return task.id == id; });
                    if (ge_async.pending.size() != before)
                        ge_async.outstanding.fetch_sub(1u, std::memory_order_acq_rel);
                    ge_async.live_stalls.erase(id);
                }
                found->second.state = GeListState::None;
                ge_list_table.queue.erase(std::remove(ge_list_table.queue.begin(), ge_list_table.queue.end(), id),
                                          ge_list_table.queue.end());
                ge_async.cv.notify_all();
                set_success(ctx);
                return;
            }
            const auto found = ge_list_table.lists.find(id);
            if (found == ge_list_table.lists.end()) {
                ctx.set_gpr(2, 0x80000100u);
                return;
            }
            if (found->second.state == GeListState::Running) {
                ctx.set_gpr(2, 0x800201A7u);
                return;
            }
            found->second.state = GeListState::None;
            ge_list_table.queue.erase(std::remove(ge_list_table.queue.begin(), ge_list_table.queue.end(), id),
                                      ge_list_table.queue.end());
            set_success(ctx);
        });
    runtime.register_hle("sceGe_user", 0xE0D68148u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t id = ctx.gpr[4];
            if ((ctx.gpr[5] & 3u) != 0u) {
                ctx.set_gpr(2, 0x80000103u);
                return;
            }
            const std::uint32_t new_stall = ctx.gpr[5] & 0x0FFFFFFFu;
            if (ge_async_running()) {
                ge_async_start_worker(rt);
                bool resumed = false;
                {
                    std::lock_guard lock(ge_async.mutex);
                    const auto found = ge_list_table.lists.find(id);
                    if (found == ge_list_table.lists.end()) {
                        ctx.set_gpr(2, 0x80000100u);
                        return;
                    }
                    found->second.stall = new_stall;
                    if (const auto active = ge_async.live_stalls.find(id);
                        active != ge_async.live_stalls.end()) {
                        active->second->store(new_stall, std::memory_order_release);
                    } else if (found->second.state == GeListState::Stalled) {
                        auto stall = std::make_shared<std::atomic<std::uint32_t>>(new_stall);
                        ge_async.live_stalls[id] = stall;
                        found->second.state = GeListState::Queued;
                        ge_async.pending.push_back(GeAsyncTask{id, thread_table.current_uid, stall});
                        ge_async.outstanding.fetch_add(1u, std::memory_order_release);
                        ++ge_async.submitted;
                        resumed = true;
                    }
                }
                if (resumed) ge_async.cv.notify_one();
                set_success(ctx);
                return;
            }

            const auto found = ge_list_table.lists.find(id);
            if (found == ge_list_table.lists.end()) {
                ctx.set_gpr(2, 0x80000100u);
                return;
            }
            found->second.stall = new_stall;
            std::vector<GuestCallbackInvocation> callbacks;
            if (found->second.state == GeListState::Stalled && !execute_ge_list(rt, found->second, callbacks)) return;
            psprecomp::AllegrexContext resume = ctx;
            resume.set_gpr(2, 0u);
            resume.pc = ctx.gpr[31];
            queue_guest_callback_chain(ctx, resume, std::move(callbacks));
        });
    runtime.register_hle("sceGe_user", 0x03444EB4u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t id = ctx.gpr[4];
            if (ctx.gpr[5] > 1u) {
                ctx.set_gpr(2, 0x800001FEu);
                return;
            }
            if (ge_async_running()) {
                if (ctx.gpr[5] == 0u && !ge_async_wait_list(rt, id)) return;
                std::lock_guard lock(ge_async.mutex);
                const auto found = ge_list_table.lists.find(id);
                if (found == ge_list_table.lists.end()) {
                    ctx.set_gpr(2, 0x80000100u);
                    return;
                }
                ctx.set_gpr(2, ctx.gpr[5] == 1u ? ge_list_status(found->second) :
                             (found->second.state == GeListState::Completed ? 0u : ge_list_status(found->second)));
                return;
            }
            const auto found = ge_list_table.lists.find(id);
            if (found == ge_list_table.lists.end()) {
                ctx.set_gpr(2, 0x80000100u);
                return;
            }
            ctx.set_gpr(2, ctx.gpr[5] == 1u ? ge_list_status(found->second) :
                         (found->second.state == GeListState::Completed ? 0u : ge_list_status(found->second)));
        });
    runtime.register_hle("sceGe_user", 0xB287BD61u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] > 1u) {
                ctx.set_gpr(2, 0x800001FEu);
                return;
            }
            if (ge_async_running()) {
                if (ctx.gpr[4] == 0u) {
                    if (!ge_async_wait_idle(rt)) return;
                    ctx.set_gpr(2, 0u);
                    return;
                }
                std::lock_guard lock(ge_async.mutex);
                std::uint32_t state = 0u;
                for (const auto &[id, list] : ge_list_table.lists) {
                    (void)id;
                    state = std::max(state, ge_list_status(list));
                }
                ctx.set_gpr(2, state);
                return;
            }
            std::uint32_t state = 0u;
            for (const auto &[id, list] : ge_list_table.lists) {
                (void)id;
                state = std::max(state, ge_list_status(list));
            }
            ctx.set_gpr(2, ctx.gpr[4] == 0u ? 0u : state);
        });
    runtime.register_hle("sceGe_user", 0xDC93CFEFu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!ge_async_wait_idle(rt)) return;
            const std::uint32_t command = ctx.gpr[4];
            ctx.set_gpr(2, command < ge_state.commands.size() ? ge_state.commands[command] : 0x80000102u);
        });
    runtime.register_hle("sceGe_user", 0x438A385Au,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!ge_async_wait_idle(rt)) return;
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 512u * 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            for (std::uint32_t index = 0u; index < ge_state.commands.size(); ++index)
                rt.memory().store32(ctx.gpr[4] + index * 4u, ge_state.commands[index]);
            for (std::uint32_t index = static_cast<std::uint32_t>(ge_state.commands.size()); index < 512u; ++index)
                rt.memory().store32(ctx.gpr[4] + index * 4u, 0u);
            set_success(ctx);
        });
    runtime.register_hle("sceGe_user", 0x0BF608FBu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!ge_async_wait_idle(rt)) return;
            if (ctx.gpr[4] == 0u || !rt.memory().contains(ctx.gpr[4], 512u * 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            for (std::uint32_t index = 0u; index < ge_state.commands.size(); ++index)
                ge_state.commands[index] = rt.memory().load32(ctx.gpr[4] + index * 4u);
            ++ge_draw_state_revision;
            ++ge_lighting_state_revision;
            ++ge_camera_state_revision;
            ge_state.offset_address = ge_state.commands[kGeCommandOffsetAddress] << 8u;
            set_success(ctx);
        });

    runtime.register_hle("InterruptManager", 0xCA04A2B9u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t interrupt_number = ctx.gpr[4];
            const std::uint32_t sub_number = ctx.gpr[5];
            const std::uint32_t handler = ctx.gpr[6];
            const std::uint32_t argument = ctx.gpr[7];
            if (interrupt_number >= 67u || handler == 0u) {
                ctx.set_gpr(2, 0x80020064u);
                return;
            }
            const std::uint64_t key = sub_interrupt_key(interrupt_number, sub_number);
            if (sub_interrupts.contains(key)) {
                ctx.set_gpr(2, 0x80020067u);  // handler already present
                return;
            }
            sub_interrupts.emplace(key, SubInterruptRecord{handler, argument, false, false});
            if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr) {
                std::cerr << "[intr] register int=" << interrupt_number << " sub=" << sub_number
                          << " handler=" << psprecomp::hex32(handler)
                          << " arg=" << psprecomp::hex32(argument) << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0xD61E6961u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t key = sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]);
            ctx.set_gpr(2, sub_interrupts.erase(key) == 1u ? 0u : 0x80020068u);
        });
    runtime.register_hle("InterruptManager", 0xFB8E22ECu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = true;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x8A389411u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = false;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x5CB5A78Bu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            if (ctx.gpr[6] != 0u) {
                if (!rt.memory().contains(ctx.gpr[6], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[6], found->second.enabled ? 1u : 0u);
            }
            found->second.enabled = false;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0x7860E0DCu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            if (found == sub_interrupts.end()) { ctx.set_gpr(2, 0x80020068u); return; }
            found->second.enabled = ctx.gpr[6] != 0u;
            set_success(ctx);
        });
    runtime.register_hle("InterruptManager", 0xFC4374B8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto found = sub_interrupts.find(sub_interrupt_key(ctx.gpr[4], ctx.gpr[5]));
            ctx.set_gpr(2, found != sub_interrupts.end() && found->second.occurred ? 1u : 0u);
        });

    runtime.register_hle("sceDisplay", 0x0E20F177u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t mode = ctx.gpr[4];
            const std::uint32_t width = ctx.gpr[5];
            const std::uint32_t height = ctx.gpr[6];
            if (mode != 0u || width == 0u || width > 480u || height == 0u || height > 272u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            display_state.mode = mode;
            display_state.width = width;
            display_state.height = height;
            if (std::getenv("PSPRECOMP_DISPLAY_DIAG") != nullptr) {
                std::cerr << "[display] mode=" << mode << " " << width << "x" << height << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xDEA197D4u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] != 0u) {
                if (!rt.memory().contains(ctx.gpr[4], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[4], display_state.mode);
            }
            if (ctx.gpr[5] != 0u) {
                if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[5], display_state.width);
            }
            if (ctx.gpr[6] != 0u) {
                if (!rt.memory().contains(ctx.gpr[6], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[6], display_state.height);
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0x289D82FEu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t address = ctx.gpr[4];
            const std::uint32_t stride = ctx.gpr[5];
            const std::uint32_t format = ctx.gpr[6];
            const std::uint32_t sync = ctx.gpr[7];
            if (address != 0u && !rt.memory().contains(address, 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            if (stride != 0u && (stride < display_state.width || stride > 2048u)) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            if (format > 3u || sync > 1u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            display_state.frame_buffer = address;
            display_state.buffer_width = stride;
            display_state.pixel_format = format;
            display_state.sync_mode = sync;
            if (std::getenv("PSPRECOMP_DISPLAY_DIAG") != nullptr) {
                std::cerr << "[display] framebuffer=" << psprecomp::hex32(address)
                          << " stride=" << stride << " format=" << format
                          << " sync=" << sync << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xEEDA2E54u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t address_out = ctx.gpr[4];
            const std::uint32_t stride_out = ctx.gpr[5];
            const std::uint32_t format_out = ctx.gpr[6];
            const std::uint32_t sync = ctx.gpr[7];
            if (sync > 1u) { ctx.set_gpr(2, 0x80000107u); return; }
            for (const auto [ptr, value] : std::array<std::pair<std::uint32_t, std::uint32_t>, 3>{
                     std::pair{address_out, display_state.frame_buffer},
                     std::pair{stride_out, display_state.buffer_width},
                     std::pair{format_out, display_state.pixel_format}}) {
                if (ptr != 0u) {
                    if (!rt.memory().contains(ptr, 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                    rt.memory().store32(ptr, value);
                }
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xDBA6C4C4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.fpr[0] = 59.94005994f *
                (static_cast<float>(virtual_display_refresh_hz()) / 60.0f);
        });
    runtime.register_hle("sceDisplay", 0x9C6EAAD7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(
                (virtual_time_us * virtual_display_refresh_hz()) / 1000000u));
        });
    runtime.register_hle("sceDisplay", 0x4D4E10ECu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t period = virtual_vblank_period_us();
            const std::uint64_t blank = std::max<std::uint64_t>(
                1u, (731u * 60u) / virtual_display_refresh_hz());
            const std::uint64_t phase = virtual_time_us % period;
            ctx.set_gpr(2, phase < blank ? 1u : 0u);
        });
    auto wait_vblank = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        // The display consumes the completed GE frame.  This is a real PSP
        // visibility boundary: allow guest/GE overlap during the frame, then
        // wait only here before framebuffer presentation and vblank callbacks.
        if (!ge_async_wait_idle(rt)) return;
        ++display_vblank_index;
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
            return;
        }
        static const std::uint64_t stop_vblank =
            parse_environment_u64("PSPRECOMP_STOP_VBLANK");
        if (stop_vblank != 0u && display_vblank_index >= stop_vblank) {
            ctx.set_gpr(2, 0u);
            rt.stop("VBlank diagnostic stop at " + std::to_string(display_vblank_index));
            return;
        }
        const std::uint64_t period = virtual_vblank_period_us();
        const std::uint32_t delay = static_cast<std::uint32_t>(period - (virtual_time_us % period));
        const auto current = thread_table.threads.find(thread_table.current_uid);
        if (current == thread_table.threads.end()) {
            ctx.set_gpr(2, 0x80020198u);
            return;
        }

        const auto interrupt = sub_interrupts.find(sub_interrupt_key(30u, 15u));
        if (interrupt == sub_interrupts.end() || !interrupt->second.enabled || interrupt->second.handler == 0u) {
            (void)delay_current_thread(rt, ctx, delay);
            return;
        }

        const psprecomp::AllegrexContext resume = make_wait_context(ctx);
        async_return_frames[thread_table.current_uid].push_back(
            AsyncReturnFrame{AsyncReturnKind::SubInterrupt, resume});

        psprecomp::AllegrexContext handler = ctx;
        handler.set_gpr(4, 15u);
        handler.set_gpr(5, interrupt->second.argument);
        handler.set_gpr(31, 0x00000004u);
        handler.pc = interrupt->second.handler;
        current->second.state = ThreadState::Delayed;
        current->second.suspended_context = handler;
        current->second.delay_until_us = virtual_time_us + delay;
        current->second.delay_sequence = thread_table.next_delay_sequence++;
        interrupt->second.occurred = true;
        if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr) {
            std::cerr << "[intr] schedule vblank uid=" << thread_table.current_uid
                      << " handler=" << psprecomp::hex32(handler.pc)
                      << " resume=" << psprecomp::hex32(resume.pc) << "\n";
        }
        if (!activate_next_thread(ctx, "vblank-wait"))
            rt.stop("PSP scheduler deadlock while waiting for VBlank interrupt");
    };
    runtime.register_hle("sceDisplay", 0x36CDFADEu, wait_vblank);
    runtime.register_hle("sceDisplay", 0x8EB9EC49u, wait_vblank);
    runtime.register_hle("sceDisplay", 0x984C27E7u, wait_vblank);
    runtime.register_hle("sceDisplay", 0x46F186C3u, wait_vblank);
    runtime.register_hle("sceDisplay", 0xB4F378FAu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 1u); });

    runtime.register_hle("sceUtility", 0x50C4CD57u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t parameter = ctx.gpr[4];
            if (savedata_utility.status != UtilityStatus::None) {
                ctx.set_gpr(2, 0x80110001u);
                return;
            }
            if (parameter == 0u || !rt.memory().contains(parameter, 4u)) {
                ctx.set_gpr(2, 0x80110004u);
                return;
            }
            const std::uint32_t declared_size = rt.memory().load32(parameter);
            if (declared_size < 0x5C0u || !rt.memory().contains(parameter, std::min<std::uint32_t>(declared_size, kSavedataParameterMinimumSize))) {
                ctx.set_gpr(2, 0x80110004u);
                return;
            }
            savedata_utility = SavedataUtilityState{UtilityStatus::Init, parameter, false};
            rt.memory().store32(parameter + kUtilityCommonResultOffset, 0u);
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] savedata init mode=" << rt.memory().load32(parameter + kSavedataModeOffset)
                          << " game=" << read_fixed_string(rt.memory(), parameter + kSavedataGameNameOffset, 13u)
                          << " save=" << read_fixed_string(rt.memory(), parameter + kSavedataSaveNameOffset, 20u)
                          << " file=" << read_fixed_string(rt.memory(), parameter + kSavedataFileNameOffset, 13u)
                          << " size=0x" << std::hex << declared_size << std::dec << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("sceUtility", 0xD4B95FFBu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (savedata_utility.status == UtilityStatus::None || savedata_utility.status == UtilityStatus::Finished) {
                ctx.set_gpr(2, 0x80110001u);
                return;
            }
            if (savedata_utility.status == UtilityStatus::Init) {
                savedata_utility.status = UtilityStatus::Visible;
            } else if (savedata_utility.status == UtilityStatus::Visible && !savedata_utility.operation_complete) {
                const std::uint32_t result = execute_savedata_operation(rt, savedata_utility.parameter_address);
                rt.memory().store32(savedata_utility.parameter_address + kUtilityCommonResultOffset, result);
                savedata_utility.operation_complete = true;
                savedata_utility.status = UtilityStatus::Quit;
                if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                    std::cerr << "[hle] savedata operation result=0x" << std::hex << std::uppercase << result
                              << std::nouppercase << std::dec << "\n";
                }
            }
            set_success(ctx);
        });
    runtime.register_hle("sceUtility", 0x8874DBE0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const UtilityStatus reported = savedata_utility.status;
            ctx.set_gpr(2, static_cast<std::uint32_t>(reported));
            if (reported == UtilityStatus::Init) {
                // PSP utility initialization completes on its own access thread.
                // Expose INIT once, then make the dialog visible for Update().
                savedata_utility.status = UtilityStatus::Visible;
            } else if (reported == UtilityStatus::Finished) {
                savedata_utility = SavedataUtilityState{};
            }
        });
    runtime.register_hle("sceUtility", 0x9790B33Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (savedata_utility.status != UtilityStatus::Quit) {
                ctx.set_gpr(2, 0x80110001u);
                return;
            }
            savedata_utility.status = UtilityStatus::Finished;
            set_success(ctx);
        });

    auto reserve_audio_channel = [](psprecomp::AllegrexContext &ctx) {
        std::int32_t channel = static_cast<std::int32_t>(ctx.gpr[4]);
        const std::uint32_t sample_count = ctx.gpr[5];
        const std::uint32_t format = ctx.gpr[6];
        if (channel < 0) {
            channel = -1;
            for (std::int32_t candidate = 7; candidate >= 1; --candidate) {
                if (!audio_channels[static_cast<std::size_t>(candidate)].reserved) {
                    channel = candidate;
                    break;
                }
            }
            if (channel < 0) { ctx.set_gpr(2, 0x80260005u); return; }
        }
        if (channel >= 8) { ctx.set_gpr(2, 0x80260003u); return; }
        if (sample_count == 0u || (sample_count & 63u) != 0u || sample_count > 65472u) {
            ctx.set_gpr(2, 0x80260006u);
            return;
        }
        if (format != 0u && format != 0x10u) { ctx.set_gpr(2, 0x80260007u); return; }
        auto &state = audio_channels[static_cast<std::size_t>(channel)];
        if (state.reserved) { ctx.set_gpr(2, 0x80268002u); return; }
        vcs::audio_output_reset_channel(static_cast<std::uint32_t>(channel));
        state = AudioChannelState{true, sample_count, format, 0u, 0u, 0u};
        if (std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr)
            std::cerr << "[audio] reserve channel=" << channel << " samples=" << sample_count
                      << " format=" << format << "\n";
        ctx.set_gpr(2, static_cast<std::uint32_t>(channel));
    };
    runtime.register_hle("sceAudio", 0x5EC81C55u,
        [reserve_audio_channel](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { reserve_audio_channel(ctx); });

    runtime.register_hle("sceAudio", 0x6FC46853u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t channel = ctx.gpr[4];
            if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
            if (!audio_channels[channel].reserved) { ctx.set_gpr(2, 0x80260001u); return; }
            audio_channels[channel] = {};
            vcs::audio_output_reset_channel(channel);
            set_success(ctx);
        });

    runtime.register_hle("sceAudio", 0xB011922Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t channel = ctx.gpr[4];
            if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
            ctx.set_gpr(2, audio_remaining_samples(audio_channels[channel]));
        });

    runtime.register_hle("sceAudio", 0xCB2E439Eu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t channel = ctx.gpr[4];
            const std::uint32_t length = ctx.gpr[5];
            if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
            if (!audio_channels[channel].reserved) { ctx.set_gpr(2, 0x80260001u); return; }
            if (length == 0u || (length & 63u) != 0u || length > 65472u) {
                ctx.set_gpr(2, 0x80260006u); return;
            }
            audio_channels[channel].sample_count = length;
            set_success(ctx);
        });

    runtime.register_hle("sceAudio", 0x95FD0C2Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t channel = ctx.gpr[4];
            const std::uint32_t format = ctx.gpr[5];
            if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
            if (!audio_channels[channel].reserved) { ctx.set_gpr(2, 0x80260008u); return; }
            if (format != 0u && format != 0x10u) { ctx.set_gpr(2, 0x80260007u); return; }
            audio_channels[channel].format = format;
            set_success(ctx);
        });

    runtime.register_hle("sceAudio", 0xB7E1D8E7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t channel = ctx.gpr[4];
            if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
            if (!audio_channels[channel].reserved) { ctx.set_gpr(2, 0x80260008u); return; }
            if (ctx.gpr[5] > 0xFFFFu || ctx.gpr[6] > 0xFFFFu) { ctx.set_gpr(2, 0x8026000Bu); return; }
            audio_channels[channel].left_volume = ctx.gpr[5];
            audio_channels[channel].right_volume = ctx.gpr[6];
            set_success(ctx);
        });

    auto audio_output_panned = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx, bool blocking) {
        const std::uint32_t channel = ctx.gpr[4];
        const std::uint32_t left = ctx.gpr[5];
        const std::uint32_t right = ctx.gpr[6];
        const std::uint32_t samples = ctx.gpr[7];
        if (channel >= 8u) { ctx.set_gpr(2, 0x80260003u); return; }
        auto &state = audio_channels[channel];
        if (!state.reserved) { ctx.set_gpr(2, 0x80260001u); return; }
        if (left > 0xFFFFu || right > 0xFFFFu) { ctx.set_gpr(2, 0x8026000Bu); return; }
        if (!blocking && audio_remaining_samples(state) != 0u) { ctx.set_gpr(2, 0x80260002u); return; }
        const std::uint32_t channels = state.format == 0x10u ? 1u : 2u;
        const std::size_t bytes = static_cast<std::size_t>(state.sample_count) * channels * 2u;
        if (samples != 0u && !rt.memory().contains(samples, bytes)) { ctx.set_gpr(2, 0x800200D3u); return; }
        state.left_volume = left;
        state.right_volume = right;
        // The buffer used to stop here: the HLE paced the guest correctly and
        // discarded the samples, which is why everything ran at the right speed
        // in total silence. Hand them to the host device.
        //
        // The buffer is scheduled at the instant the hardware would actually
        // start playing it, not at the instant of the call, so the host mix
        // stays contiguous no matter how much guest time the caller burned
        // producing it.
        const std::uint64_t start_us = audio_queue_buffer(state, state.sample_count);
        if (samples != 0u && vcs::audio_output_enabled()) {
            std::vector<std::int16_t> pcm(bytes / sizeof(std::int16_t));
            for (std::size_t index = 0u; index < pcm.size(); ++index) {
                pcm[index] = static_cast<std::int16_t>(
                    rt.memory().aot_load16(samples +
                        static_cast<std::uint32_t>(index * sizeof(std::int16_t))));
            }
            vcs::audio_output_submit(pcm, state.sample_count, channels == 2u, left, right,
                                     state.frequency, channel, start_us);
        }
        // A blocking submission returns when the *previous* buffer finished,
        // which is exactly when this one starts.
        const std::uint64_t wait_us =
            start_us > virtual_time_us ? start_us - virtual_time_us : 0u;
        if (std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr)
            std::cerr << "[audio] output channel=" << channel << " samples=" << state.sample_count
                      << " blocking=" << blocking << " start_us=" << start_us
                      << " wait_us=" << wait_us << "\n";
        if (blocking) {
            (void)delay_current_thread(rt, ctx, static_cast<std::uint32_t>(wait_us),
                                       state.sample_count);
        } else {
            ctx.set_gpr(2, state.sample_count);
        }
    };
    runtime.register_hle("sceAudio", 0xE2D56B2Du,
        [audio_output_panned](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { audio_output_panned(rt, ctx, false); });
    runtime.register_hle("sceAudio", 0x13F592BCu,
        [audio_output_panned](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) { audio_output_panned(rt, ctx, true); });
    runtime.register_hle("sceAudio", 0x136CAF51u,
        [audio_output_panned](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t volume = ctx.gpr[5];
            const std::uint32_t buffer = ctx.gpr[6];
            ctx.set_gpr(6, volume);
            ctx.set_gpr(7, buffer);
            audio_output_panned(rt, ctx, true);
        });

    runtime.register_hle("sceAudio", 0x01562BA3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            // sceAudioOutput2Reserve takes ONE argument. Reading gpr[5]/gpr[6]
            // as a rate and a channel count picked up unrelated register
            // contents -- a diagnostic run reported freq=167495392, plainly a
            // pointer -- which drove the buffer duration to zero, stopped the
            // channel from blocking and starved the mix into constant
            // stuttering. Output2 is 44100 Hz stereo.
            const std::uint32_t samples = ctx.gpr[4] & 0x7FFFFFFFu;
            auto &state = audio_channels[8];
            if (samples < 17u || samples > 4111u) { ctx.set_gpr(2, 0x80000104u); return; }
            if (state.reserved) { ctx.set_gpr(2, 0x80268002u); return; }
            vcs::audio_output_reset_channel(8u);
            state = AudioChannelState{true, samples, 0u, 0u, 0u, 0u};
            if (std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr)
                std::cerr << "[audio] output2 reserve samples=" << samples << "\n";
            set_success(ctx);
        });
    // sceAudioSRCChReserve(sampleCount, frequency, channels) -- this is the call
    // that actually carries a rate, and it was not implemented at all. Radio
    // streams that run below 44100 were the ones playing back at chipmunk pitch.
    runtime.register_hle("sceAudio", 0x38553111u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t samples = ctx.gpr[4] & 0x7FFFFFFFu;
            const std::uint32_t frequency = ctx.gpr[5];
            const std::uint32_t channel_count = ctx.gpr[6];
            auto &state = audio_channels[8];
            if (samples < 17u || samples > 4111u) { ctx.set_gpr(2, 0x80000104u); return; }
            if (state.reserved) { ctx.set_gpr(2, 0x80268002u); return; }
            vcs::audio_output_reset_channel(8u);
            state = AudioChannelState{true, samples, 0u, 0u, 0u, 0u};
            // 0 means "keep the current rate" on hardware; 44100 is the default.
            state.frequency = frequency == 0u ? 44100u : frequency;
            state.channel_count = channel_count == 1u ? 1u : 2u;
            if (std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr)
                std::cerr << "[audio] src reserve samples=" << samples
                          << " freq=" << state.frequency
                          << " channels=" << state.channel_count << "\n";
            set_success(ctx);
        });
    runtime.register_hle("sceAudio", 0x5C37C0AEu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            audio_channels[8] = {};
            vcs::audio_output_reset_channel(8u);
            set_success(ctx);
        });

    // sceAudioOutput2OutputBlocking and sceAudioSRCOutputBlocking share this
    // body: both drain the single resampling channel, and the rate recorded at
    // reserve time is what tells them apart.
    const auto audio_src_output =
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto &state = audio_channels[8];
            const std::uint32_t volume = ctx.gpr[4];
            const std::uint32_t buffer = ctx.gpr[5];
            if (volume > 0xFFFFFu) { ctx.set_gpr(2, 0x8026000Bu); return; }
            if (!state.reserved) { ctx.set_gpr(2, 0x80260008u); return; }
            // sceAtracDecodeData always writes two interleaved channels (see the
            // note by kAtracOutputChannels), and Output2 is the port VCS feeds
            // from it. A mono source -- the radio news bulletins are mono while
            // the music is stereo -- still arrives here as stereo PCM, but
            // channel_count reports 1. Reading that buffer as mono took half the
            // bytes and advanced half a frame per sample, which plays the
            // bulletin at half rate: the dragging, slowed-down audio.
            const std::uint32_t effective_channels = 2u;
            if (state.channel_count != effective_channels &&
                std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr) {
                static bool once = false;
                if (!once) {
                    once = true;
                    std::cerr << "[audio] output2 channel_count=" << state.channel_count
                              << " tratado como " << effective_channels
                              << " (ATRAC entrega estereo intercalado)\n";
                }
            }
            const std::size_t bytes =
                static_cast<std::size_t>(state.sample_count) * effective_channels * 2u;
            if (buffer != 0u && !rt.memory().contains(buffer, bytes)) { ctx.set_gpr(2, 0x800200D3u); return; }
            state.left_volume = volume; state.right_volume = volume;
            // audio_queue_buffer already paces at the channel's own frequency,
            // so a stream that is not 44100 neither starves nor floods the mix.
            const std::uint64_t start_us = audio_queue_buffer(state, state.sample_count);
            if (buffer != 0u && vcs::audio_output_enabled()) {
                std::vector<std::int16_t> pcm(bytes / sizeof(std::int16_t));
                for (std::size_t index = 0u; index < pcm.size(); ++index) {
                    pcm[index] = static_cast<std::int16_t>(
                        rt.memory().aot_load16(buffer +
                            static_cast<std::uint32_t>(index * sizeof(std::int16_t))));
                }
                vcs::audio_output_submit(pcm, state.sample_count, true,
                                         volume, volume, state.frequency, 8u, start_us);
            }
            const std::uint64_t wait_us =
                start_us > virtual_time_us ? start_us - virtual_time_us : 0u;
            if (std::getenv("PSPRECOMP_AUDIO_DIAG") != nullptr)
                std::cerr << "[audio] output2 samples=" << state.sample_count
                          << " channels=" << state.channel_count
                          << " freq=" << state.frequency
                          << " start_us=" << start_us << " wait_us=" << wait_us << "\n";
            (void)delay_current_thread(rt, ctx, static_cast<std::uint32_t>(wait_us),
                                       state.sample_count);
        };
    runtime.register_hle("sceAudio", 0x2D53F36Eu, audio_src_output);
    runtime.register_hle("sceAudio", 0xE0727056u, audio_src_output);


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
            state.source_path = identify_atrac_source(header_bytes, parsed);
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

    // Native bring-up is intentionally offline.  Report the physical WLAN
    // switch as off rather than claiming a connected/powered radio.
    runtime.register_hle("sceWlanDrv", 0xD7763699u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });

    runtime.register_hle("sceSasCore", 0x42778A9Fu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t core = ctx.gpr[4];
            const std::uint32_t grain = ctx.gpr[5];
            const std::uint32_t max_voices = ctx.gpr[6];
            const std::uint32_t output_mode = ctx.gpr[7];
            const std::uint32_t sample_rate = ctx.gpr[8];
            if ((core & 0x3Fu) != 0u || !rt.memory().contains(core, 64u)) {
                ctx.set_gpr(2, kSasErrorBadAddress); return;
            }
            if (max_voices == 0u || max_voices > 32u) {
                ctx.set_gpr(2, kSasErrorInvalidMaxVoices); return;
            }
            if (grain < 0x40u || grain > 0x800u || (grain & 0x1Fu) != 0u) {
                ctx.set_gpr(2, kSasErrorInvalidGrain); return;
            }
            if (output_mode > 1u) {
                ctx.set_gpr(2, kSasErrorInvalidOutputMode); return;
            }
            if (sample_rate != 44100u) {
                ctx.set_gpr(2, kSasErrorInvalidSampleRate); return;
            }
            sas_state = SasState{};
            sas_core_mix_calls = 0u;
            sas_core_with_mix_calls = 0u;
            sas_state.initialized = true;
            sas_state.core_address = core;
            sas_state.grain_size = grain;
            // Hardware exposes all 32 voices even when maxVoices is smaller.
            sas_state.max_voices = 32u;
            sas_state.output_mode = output_mode;
            sas_state.sample_rate = sample_rate;
            for (auto &voice : sas_state.voices) voice.pitch = 0x1000;
            rt.memory().zero(core, 64u);
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x99944089u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::uint32_t address = ctx.gpr[6];
            std::int32_t size = static_cast<std::int32_t>(ctx.gpr[7]);
            const std::int32_t loop = static_cast<std::int32_t>(ctx.gpr[8]);
            if (size == 0 || (static_cast<std::uint32_t>(size) & 0xFu) != 0u) {
                ctx.set_gpr(2, kSasErrorInvalidParameter); return;
            }
            if (loop != 0 && loop != 1) {
                ctx.set_gpr(2, kSasErrorInvalidLoop); return;
            }
            if (size < 0) size = 0;
            if (size > 0 && !rt.memory().contains(address, static_cast<std::size_t>(size))) {
                // PSP ignores an invalid VAG pointer, leaving the voice configured.
                set_success(ctx); return;
            }
            voice->type = SasVoiceType::Vag;
            voice->data_address = address;
            voice->data_size = size;
            voice->loop = loop != 0;
            sas_reset_decoder(*voice);
            if (voice->on) voice->playing = true;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xB7660A23u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::int32_t frequency = static_cast<std::int32_t>(ctx.gpr[6]);
            if (frequency < 0 || frequency >= 64) {
                ctx.set_gpr(2, kSasErrorInvalidNoiseFrequency); return;
            }
            voice->type = SasVoiceType::Noise;
            voice->noise_frequency = frequency;
            sas_reset_decoder(*voice);
            if (voice->on) voice->playing = true;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xAD84D37Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::int32_t pitch = static_cast<std::int32_t>(ctx.gpr[6]);
            if (pitch < 0 || pitch > 0x4000) {
                ctx.set_gpr(2, kSasErrorInvalidPitch); return;
            }
            voice->pitch = pitch;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x440CA7D8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::array<std::int32_t, 4> volumes{
                static_cast<std::int32_t>(ctx.gpr[6]), static_cast<std::int32_t>(ctx.gpr[7]),
                static_cast<std::int32_t>(ctx.gpr[8]), static_cast<std::int32_t>(ctx.gpr[9])};
            for (const auto volume : volumes) {
                if (static_cast<std::int64_t>(volume) < -0x1000ll || static_cast<std::int64_t>(volume) > 0x1000ll) {
                    ctx.set_gpr(2, kSasErrorInvalidVolume); return;
                }
            }
            voice->left_volume = volumes[0]; voice->right_volume = volumes[1];
            voice->effect_left_volume = volumes[2]; voice->effect_right_volume = volumes[3];
            if (sas_audio_diagnostics_enabled() &&
                (volumes[2] != 0 || volumes[3] != 0)) {
                std::cerr << "[sas] volume voice=" << static_cast<std::int32_t>(ctx.gpr[5])
                          << " dry=" << volumes[0] << "," << volumes[1]
                          << " effect=" << volumes[2] << "," << volumes[3] << "\n";
            }
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x019B25EBu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::uint32_t flags = ctx.gpr[6] & 0xFu;
            const std::array<std::int32_t, 4> rates{
                static_cast<std::int32_t>(ctx.gpr[7]), static_cast<std::int32_t>(ctx.gpr[8]),
                static_cast<std::int32_t>(ctx.gpr[9]), static_cast<std::int32_t>(ctx.gpr[10])};
            for (std::size_t i = 0; i < rates.size(); ++i) {
                if ((flags & (1u << i)) != 0u && rates[i] < 0) {
                    ctx.set_gpr(2, kSasErrorInvalidAdsrRate); return;
                }
            }
            for (std::size_t i = 0; i < rates.size(); ++i)
                if ((flags & (1u << i)) != 0u) voice->adsr_rates[i] = rates[i];
            if (flags != 0u) voice->adsr_configured = true;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x9EC3676Au,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            const std::uint32_t flags = ctx.gpr[6] & 0xFu;
            std::array<std::int32_t, 4> modes{
                static_cast<std::int32_t>(ctx.gpr[7] & 0x7FFFFFFFu), static_cast<std::int32_t>(ctx.gpr[8] & 0x7FFFFFFFu),
                static_cast<std::int32_t>(ctx.gpr[9] & 0x7FFFFFFFu), static_cast<std::int32_t>(ctx.gpr[10] & 0x7FFFFFFFu)};
            const bool invalid_attack = modes[0] > 5 || (modes[0] & 1) != 0;
            const bool invalid_decay = modes[1] > 5 || (modes[1] & 1) != 1;
            const bool invalid_sustain = modes[2] > 5;
            const bool invalid_release = modes[3] > 5 || (modes[3] & 1) != 1;
            const std::array<bool, 4> invalid{invalid_attack, invalid_decay, invalid_sustain, invalid_release};
            for (std::size_t i = 0; i < invalid.size(); ++i) {
                if ((flags & (1u << i)) != 0u && invalid[i]) {
                    ctx.set_gpr(2, kSasErrorInvalidAdsrMode); return;
                }
            }
            for (std::size_t i = 0; i < modes.size(); ++i)
                if ((flags & (1u << i)) != 0u) voice->adsr_modes[i] = modes[i];
            if (flags != 0u) voice->adsr_configured = true;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x5F9529F6u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            voice->sustain_level = static_cast<std::int32_t>(ctx.gpr[6]);
            voice->adsr_configured = true;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xCBCD4F79u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            if (((ctx.gpr[7] >> 13u) & 1u) != 0u) {
                ctx.set_gpr(2, kSasErrorInvalidAdsrMode); return;
            }
            voice->simple_adsr1 = ctx.gpr[6] & 0xFFFFu;
            voice->simple_adsr2 = ctx.gpr[7] & 0xFFFFu;
            sas_decode_simple_adsr(*voice);
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x76F01ACAu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            if (voice->paused || voice->on) {
                ctx.set_gpr(2, kSasErrorVoicePaused); return;
            }
            sas_reset_decoder(*voice);
            voice->on = true;
            voice->playing = voice->type != SasVoiceType::Off;
            voice->envelope_height = 0u;
            voice->envelope_phase = SasEnvelopePhase::Attack;
            voice->key_on_delay_samples = voice->adsr_configured
                ? (voice->type == SasVoiceType::Vag ? 33u : 32u) : 0u;
            if (sas_audio_diagnostics_enabled())
                std::cerr << "[sas] keyon voice=" << static_cast<std::int32_t>(ctx.gpr[5])
                          << " type=" << static_cast<int>(voice->type)
                          << " pitch=" << voice->pitch
                          << " loop=" << voice->loop << "\n";
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xA0CF2FA4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            if (voice->paused || !voice->on) {
                ctx.set_gpr(2, kSasErrorVoicePaused); return;
            }
            voice->on = false;
            voice->envelope_phase = SasEnvelopePhase::Release;
            // A release rate of zero never walks the envelope down, so a looping
            // voice keyed off here would sound forever.  Log it: this is the
            // remaining suspect for the vehicle engine that keeps running under
            // the pause menu.
            if (sas_audio_diagnostics_enabled())
                std::cerr << "[sas] keyoff voice=" << static_cast<std::int32_t>(ctx.gpr[5])
                          << " loop=" << voice->loop
                          << " release_mode=" << voice->adsr_modes[3]
                          << " release_rate=" << voice->adsr_rates[3]
                          << " height=" << voice->envelope_height << "\n";
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x787D04D5u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            std::uint32_t mask = ctx.gpr[5];
            const bool pause = ctx.gpr[6] != 0u;
            for (std::size_t i = 0; i < sas_state.voices.size(); ++i)
                if ((mask & (1u << i)) != 0u) sas_state.voices[i].paused = pause;
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x2C8E6AB3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            std::uint32_t flags = 0u;
            for (std::size_t i = 0; i < sas_state.voices.size(); ++i)
                if (sas_state.voices[i].paused) flags |= 1u << i;
            ctx.set_gpr(2, flags);
        });

    runtime.register_hle("sceSasCore", 0x68A46B95u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            std::uint32_t flags = 0u;
            for (std::size_t i = 0; i < sas_state.voices.size(); ++i)
                if (!sas_state.voices[i].playing) flags |= 1u << i;
            ctx.set_gpr(2, flags);
        });

    runtime.register_hle("sceSasCore", 0x74AE582Au,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            auto *voice = sas_voice(ctx.gpr[4], static_cast<std::int32_t>(ctx.gpr[5]), ctx);
            if (!voice) return;
            ctx.set_gpr(2, voice->envelope_height);
        });

    runtime.register_hle("sceSasCore", 0x33D4AB37u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            const std::int32_t type = static_cast<std::int32_t>(ctx.gpr[5]);
            if (type < -1 || type > 8) { ctx.set_gpr(2, kSasErrorReverbType); return; }
            if (sas_state.reverb.type != type) {
                sas_state.reverb.type = type;
                sas_state.reverb.history_left.clear();
                sas_state.reverb.history_right.clear();
                sas_state.reverb.history_cursor = 0u;
            }
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x267A6DD2u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            const std::int32_t delay = static_cast<std::int32_t>(ctx.gpr[5]);
            const std::int32_t feedback = static_cast<std::int32_t>(ctx.gpr[6]);
            if (delay < 0 || delay >= 128) { ctx.set_gpr(2, kSasErrorReverbDelay); return; }
            if (feedback < 0 || feedback >= 128) { ctx.set_gpr(2, kSasErrorReverbFeedback); return; }
            sas_state.reverb.delay = delay; sas_state.reverb.feedback = feedback; set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xD5A229C9u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            if (ctx.gpr[5] > 0x1000u || ctx.gpr[6] > 0x1000u) {
                ctx.set_gpr(2, kSasErrorReverbVolume); return;
            }
            sas_state.reverb.left_volume = ctx.gpr[5]; sas_state.reverb.right_volume = ctx.gpr[6]; set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xF983B186u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            sas_state.reverb.dry = ctx.gpr[5] != 0u; sas_state.reverb.wet = ctx.gpr[6] != 0u; set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0xA3589D81u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            const std::uint32_t output = ctx.gpr[5];
            const std::size_t bytes = static_cast<std::size_t>(sas_state.grain_size) *
                (sas_state.output_mode == 0u ? 4u : 8u);
            if (!rt.memory().contains(output, bytes)) { ctx.set_gpr(2, kSasErrorInvalidParameter); return; }
            ++sas_core_mix_calls;
            sas_log_mix_checkpoint("core", sas_core_mix_calls);
            if (sas_state.output_mode == 0u)
                sas_mix_into(rt, output, sas_state.grain_size);
            else
                sas_mix_raw(rt, output, sas_state.grain_size);
            set_success(ctx);
        });

    runtime.register_hle("sceSasCore", 0x50A14DFCu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (!sas_valid_core(ctx.gpr[4])) { ctx.set_gpr(2, kSasErrorNotInitialized); return; }
            if (sas_state.output_mode == 1u) { ctx.set_gpr(2, 0x800001FFu); return; }
            const std::uint32_t inout = ctx.gpr[5];
            const std::size_t bytes = static_cast<std::size_t>(sas_state.grain_size) * 4u;
            if (!rt.memory().contains(inout, bytes)) { ctx.set_gpr(2, kSasErrorInvalidParameter); return; }
            const std::uint32_t input_left = ctx.gpr[6];
            const std::uint32_t input_right = ctx.gpr[7];
            if (input_left > 0x1000u || input_right > 0x1000u) {
                ctx.set_gpr(2, kSasErrorInvalidVolume); return;
            }
            ++sas_core_with_mix_calls;
            sas_log_mix_checkpoint("core-with-mix", sas_core_with_mix_calls);
            sas_mix_into(rt, inout, sas_state.grain_size, true, input_left, input_right);
            set_success(ctx);
        });

    runtime.register_hle("scePower", 0x04B7766Eu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("scePower", 0xDFA8BAF8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0xAEE7404Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0xBD2BDE07u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0x46EBB729u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 1u); });
    runtime.register_hle("sceUmdUser", 0x6B4A146Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0x32u); });
    runtime.register_hle("sceUmdUser", 0x8EF08FCEu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0xC6183D47u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("LoadExecForUser", 0x4AC57943u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
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


    runtime.register_hle("ModuleMgrForUser", 0xB7F46618u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            if (!file_table.files.contains(fd) && !file_table.synthetic_empty_files.contains(fd)) {
                if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr)
                    std::cerr << "[module] sceKernelLoadModuleByID rejected fd=" << fd << "\n";
                ctx.set_gpr(2, 0x80010009u);
                return;
            }
            const std::int32_t uid = next_module_uid++;
            if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr)
                std::cerr << "[module] sceKernelLoadModuleByID fd=" << fd << " -> uid=" << uid << "\n";
            loaded_modules.emplace(uid, false);
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ModuleMgrForUser", 0x50F0C1ECu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = loaded_modules.find(uid);
            if (found == loaded_modules.end()) {
                ctx.set_gpr(2, 0x8002012Eu);
                return;
            }
            // Fifth O32 argument: optional SceKernelSMOption*.  The fourth
            // argument is the module_start status output.
            const std::uint32_t status = ctx.gpr[7];
            if (status != 0u && rt.memory().contains(status, 4u)) rt.memory().store32(status, 0u);
            found->second = true;
            if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr)
                std::cerr << "[module] sceKernelStartModule uid=" << uid << " status=0\n";
            ctx.set_gpr(2, static_cast<std::uint32_t>(uid));
        });
    runtime.register_hle("ModuleMgrForUser", 0xD1FF982Au,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto found = loaded_modules.find(uid);
            if (found == loaded_modules.end()) {
                ctx.set_gpr(2, 0x8002012Eu);
                return;
            }
            const std::uint32_t status = ctx.gpr[7];
            if (status != 0u && rt.memory().contains(status, 4u)) rt.memory().store32(status, 0u);
            found->second = false;
            ctx.set_gpr(2, 0u);
        });
    runtime.register_hle("ModuleMgrForUser", 0x2E0911AAu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto uid = static_cast<std::int32_t>(ctx.gpr[4]);
            ctx.set_gpr(2, loaded_modules.erase(uid) == 1u ? 0u : 0x8002012Eu);
        });

    runtime.register_hle("IoFileMgrForUser", 0x54F5FB11u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string device = ctx.gpr[4] != 0u ? rt.memory().read_c_string(ctx.gpr[4], 128u) : std::string{};
            const std::uint32_t command = ctx.gpr[5];
            const std::uint32_t input = ctx.gpr[6];
            const std::uint32_t input_length = ctx.gpr[7];
            const std::uint32_t output = rt.memory().contains(ctx.gpr[29] + 16u, 8u)
                ? rt.memory().load32(ctx.gpr[29] + 16u) : 0u;
            const std::uint32_t output_length = rt.memory().contains(ctx.gpr[29] + 20u, 4u)
                ? rt.memory().load32(ctx.gpr[29] + 20u) : 0u;

            if (command == 0x02425823u && (device == "fatms0:" || device == "ms0:")) {
                if (output == 0u || !rt.memory().contains(output, 4u)) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                rt.memory().store32(output, memory_stick_fat_state);
                set_success(ctx);
                return;
            }
            if (command == 0x02415823u && (device == "fatms0:" || device == "ms0:")) {
                if (input == 0u || input_length < 4u || !rt.memory().contains(input, 4u)) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                memory_stick_fat_state = rt.memory().load32(input) != 0u ? 1u : 0u;
                set_success(ctx);
                return;
            }
            if (command == 0x02425824u && (device == "fatms0:" || device == "ms0:")) {
                if (output == 0u || output_length < 4u || !rt.memory().contains(output, 4u)) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                rt.memory().store32(output, 0u);
                set_success(ctx);
                return;
            }
            if (command == 0x02025806u && (device == "mscmhc0:" || device == "ms0:")) {
                if (output == 0u || output_length < 4u || !rt.memory().contains(output, 4u)) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                rt.memory().store32(output, 1u);
                set_success(ctx);
                return;
            }
            if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
                std::cerr << "[hle] unsupported sceIoDevctl device=" << device
                          << " cmd=0x" << std::hex << std::uppercase << command
                          << " in=0x" << input << "/" << std::dec << input_length
                          << " out=0x" << std::hex << output << "/" << std::dec << output_length << "\n";
            }
            ctx.set_gpr(2, 0x80010016u);
        });

    runtime.register_hle("IoFileMgrForUser", 0xB293727Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("IoFileMgrForUser", 0xB29DDF9Cu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            try {
                const auto native = rt.translate_path(rt.memory().read_c_string(ctx.gpr[4]));
                if (!std::filesystem::is_directory(native)) {
                    ctx.set_gpr(2, 0x80010002u);
                    return;
                }
                DirectoryHandle handle;
                for (const auto &entry : std::filesystem::directory_iterator(native)) handle.entries.push_back(entry);
                std::sort(handle.entries.begin(), handle.entries.end(), [](const auto &a, const auto &b) {
                    return a.path().filename().string() < b.path().filename().string();
                });
                const auto fd = file_table.next_fd++;
                file_table.directories.emplace(fd, std::move(handle));
                ctx.set_gpr(2, static_cast<std::uint32_t>(fd));
            } catch (...) {
                ctx.set_gpr(2, 0x80010002u);
            }
        });
    runtime.register_hle("IoFileMgrForUser", 0xE3EB004Cu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t dirent = ctx.gpr[5];
            const auto it = file_table.directories.find(fd);
            if (it == file_table.directories.end() || !rt.memory().contains(dirent, 0x160u)) {
                ctx.set_gpr(2, 0x80010009u);
                return;
            }
            if (it->second.index >= it->second.entries.size()) {
                ctx.set_gpr(2, 0u);
                return;
            }
            const auto &entry = it->second.entries[it->second.index++];
            rt.memory().zero(dirent, 0x160u);
            const bool is_directory = entry.is_directory();
            const std::uint32_t mode = is_directory ? 0x1000u : 0x2000u;
            rt.memory().store32(dirent, mode);
            if (!is_directory) {
                if (const auto *disc_file = register_virtual_disc_file(entry.path())) {
                    rt.memory().store32(dirent + 8u, static_cast<std::uint32_t>(disc_file->size));
                    rt.memory().store32(dirent + 12u, static_cast<std::uint32_t>(disc_file->size >> 32u));
                    rt.memory().store32(dirent + 0x40u, disc_file->start_sector);
                    if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                        std::cerr << "[io] sceIoDread file=\"" << entry.path().filename().string()
                                  << "\" sector=" << disc_file->start_sector
                                  << " size=" << disc_file->size << "\n";
                    }
                }
            }
            const std::string name = entry.path().filename().string();
            std::vector<std::uint8_t> bytes(name.begin(), name.end());
            bytes.push_back(0u);
            if (bytes.size() > 256u) bytes.resize(256u);
            rt.memory().copy_in(dirent + 0x58u, bytes);
            ctx.set_gpr(2, 1u);
        });
    runtime.register_hle("IoFileMgrForUser", 0xEB092469u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            ctx.set_gpr(2, file_table.directories.erase(fd) == 1u ? 0u : 0x80010009u);
        });

    // sceRtc. None of it existed, and the saved-game list needs it: it turns
    // each save's timestamp into a tick to sort and display it, so opening the
    // load menu stopped the runtime on a missing import.
    //
    // A PSP tick is microseconds since 0001-01-01 00:00:00, and ScePspDateTime
    // is year, month, day, hour, minute, second as 16-bit fields followed by a
    // 32-bit microsecond -- 16 bytes.
    {
        // Howard Hinnant's civil-date algorithms, which are exact over the
        // whole proleptic Gregorian range rather than only near the epoch.
        const auto days_from_civil = [](std::int64_t y, unsigned m, unsigned d) -> std::int64_t {
            y -= m <= 2;
            const std::int64_t era = (y >= 0 ? y : y - 399) / 400;
            const unsigned yoe = static_cast<unsigned>(y - era * 400);
            const unsigned doy = (153u * (m + (m > 2 ? -3 : 9)) + 2u) / 5u + d - 1u;
            const unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
            return era * 146097 + static_cast<std::int64_t>(doe) - 719468;
        };
        const auto civil_from_days = [](std::int64_t z, int &y, unsigned &m, unsigned &d) {
            z += 719468;
            const std::int64_t era = (z >= 0 ? z : z - 146096) / 146097;
            const unsigned doe = static_cast<unsigned>(z - era * 146097);
            const unsigned yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
            const std::int64_t yr = static_cast<std::int64_t>(yoe) + era * 400;
            const unsigned doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);
            const unsigned mp = (5u * doy + 2u) / 153u;
            d = doy - (153u * mp + 2u) / 5u + 1u;
            m = mp + (mp < 10u ? 3u : -9u);
            y = static_cast<int>(yr + (m <= 2u ? 1 : 0));
        };
        // Days from 0001-01-01 to 1970-01-01.
        constexpr std::int64_t kDaysToUnixEpoch = 719162;
        constexpr std::uint64_t kMicrosecondsPerDay = 86400ull * 1000000ull;

        struct RtcHelpers {
            std::function<std::uint64_t(psprecomp::Runtime &, std::uint32_t)> read_tick;
            std::function<void(psprecomp::Runtime &, std::uint32_t, std::uint64_t)> write_date;
        };
        static RtcHelpers helpers;
        helpers.read_tick = [days_from_civil](psprecomp::Runtime &rt, std::uint32_t address) -> std::uint64_t {
            const std::uint32_t year = rt.memory().load16(address + 0u);
            const std::uint32_t month = rt.memory().load16(address + 2u);
            const std::uint32_t day = rt.memory().load16(address + 4u);
            const std::uint32_t hour = rt.memory().load16(address + 6u);
            const std::uint32_t minute = rt.memory().load16(address + 8u);
            const std::uint32_t second = rt.memory().load16(address + 10u);
            const std::uint32_t microsecond = rt.memory().load32(address + 12u);
            const std::int64_t days = days_from_civil(static_cast<std::int64_t>(year),
                                                      month == 0u ? 1u : month,
                                                      day == 0u ? 1u : day) + kDaysToUnixEpoch;
            return static_cast<std::uint64_t>(days) * kMicrosecondsPerDay +
                   (hour * 3600ull + minute * 60ull + second) * 1000000ull + microsecond;
        };
        helpers.write_date = [civil_from_days](psprecomp::Runtime &rt, std::uint32_t address,
                                               std::uint64_t tick) {
            const std::uint64_t day_index = tick / kMicrosecondsPerDay;
            const std::uint64_t remainder = tick % kMicrosecondsPerDay;
            int year = 1;
            unsigned month = 1u;
            unsigned day = 1u;
            civil_from_days(static_cast<std::int64_t>(day_index) - kDaysToUnixEpoch, year, month, day);
            rt.memory().store16(address + 0u, static_cast<std::uint16_t>(year));
            rt.memory().store16(address + 2u, static_cast<std::uint16_t>(month));
            rt.memory().store16(address + 4u, static_cast<std::uint16_t>(day));
            rt.memory().store16(address + 6u, static_cast<std::uint16_t>(remainder / 3600000000ull));
            rt.memory().store16(address + 8u, static_cast<std::uint16_t>((remainder / 60000000ull) % 60ull));
            rt.memory().store16(address + 10u, static_cast<std::uint16_t>((remainder / 1000000ull) % 60ull));
            rt.memory().store32(address + 12u, static_cast<std::uint32_t>(remainder % 1000000ull));
        };
        const auto current_tick = []() -> std::uint64_t {
            // The wall clock, not the guest's virtual time: a save stamped with
            // the emulated uptime would read as the year 1 in the list.
            const auto now = std::chrono::system_clock::now().time_since_epoch();
            const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
            return static_cast<std::uint64_t>(kDaysToUnixEpoch) * kMicrosecondsPerDay +
                   static_cast<std::uint64_t>(micros);
        };
        static const auto tick_now = current_tick;

        runtime.register_hle("sceRtc", 0xC41C2853u, // sceRtcGetTickResolution
            [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
                ctx.set_gpr(2, 1000000u);
            });
        runtime.register_hle("sceRtc", 0x3F7AD767u, // sceRtcGetCurrentTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick = tick_now();
                rt.memory().store32(ctx.gpr[4], static_cast<std::uint32_t>(tick));
                rt.memory().store32(ctx.gpr[4] + 4u, static_cast<std::uint32_t>(tick >> 32u));
                set_success(ctx);
            });
        runtime.register_hle("sceRtc", 0x6FF40ACCu, // sceRtcGetTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick = helpers.read_tick(rt, ctx.gpr[4]);
                rt.memory().store32(ctx.gpr[5], static_cast<std::uint32_t>(tick));
                rt.memory().store32(ctx.gpr[5] + 4u, static_cast<std::uint32_t>(tick >> 32u));
                set_success(ctx);
            });
        runtime.register_hle("sceRtc", 0x7ED29E40u, // sceRtcSetTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick =
                    static_cast<std::uint64_t>(rt.memory().load32(ctx.gpr[5])) |
                    (static_cast<std::uint64_t>(rt.memory().load32(ctx.gpr[5] + 4u)) << 32u);
                helpers.write_date(rt, ctx.gpr[4], tick);
                set_success(ctx);
            });
        const auto current_clock = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            helpers.write_date(rt, ctx.gpr[4], tick_now());
            set_success(ctx);
        };
        runtime.register_hle("sceRtc", 0x4CFA57B0u, current_clock); // sceRtcGetCurrentClock
        runtime.register_hle("sceRtc", 0xE7C27D1Bu, current_clock); // ...LocalTime
        // No time zone is modelled: the host clock is already local, so both
        // conversions are the identity rather than a wrong offset.
        const auto copy_date = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            for (std::uint32_t offset = 0u; offset < 16u; offset += 4u)
                rt.memory().store32(ctx.gpr[4] + offset, rt.memory().load32(ctx.gpr[5] + offset));
            set_success(ctx);
        };
        runtime.register_hle("sceRtc", 0x34885E0Du, copy_date); // ConvertUtcToLocalTime
        runtime.register_hle("sceRtc", 0x779242A2u, copy_date); // ConvertLocalTimeToUTC
        runtime.register_hle("sceRtc", 0x9ED0AE87u, // sceRtcCompareTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const auto load = [&](std::uint32_t address) {
                    return static_cast<std::uint64_t>(rt.memory().load32(address)) |
                           (static_cast<std::uint64_t>(rt.memory().load32(address + 4u)) << 32u);
                };
                const std::uint64_t first = load(ctx.gpr[4]);
                const std::uint64_t second = load(ctx.gpr[5]);
                ctx.set_gpr(2, first < second ? 0xFFFFFFFFu : (first > second ? 1u : 0u));
            });
    }

    // sceIoGetstat. VCS calls it while listing saved games: the load screen
    // asks for each entry's type and size before it will show it, and with the
    // import missing the runtime stopped on a black screen the moment the load
    // menu was opened.
    //
    // SceIoStat is 0x58 bytes: mode, attr, a 64-bit size, three 16-byte
    // ScePspDateTime stamps and six private words. Only mode, attr and size
    // are read here; the timestamps are zeroed, which the dialog accepts.
    runtime.register_hle("IoFileMgrForUser", 0xACE946E8u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string path = rt.memory().read_c_string(ctx.gpr[4]);
            const std::uint32_t stat_address = ctx.gpr[5];
            if (stat_address == 0u || !rt.memory().contains(stat_address, 0x58u)) {
                ctx.set_gpr(2, 0x80010016u); // EINVAL
                return;
            }
            std::error_code error;
            const auto native = rt.translate_path(path);
            const bool directory = std::filesystem::is_directory(native, error);
            const bool regular = std::filesystem::is_regular_file(native, error);
            if (!directory && !regular) {
                ctx.set_gpr(2, 0x80010002u); // ENOENT
                return;
            }
            rt.memory().zero(stat_address, 0x58u);
            // FIO_S_IFDIR/FIO_S_IFREG with read/write/execute for all three
            // classes, which is what a memory stick reports.
            rt.memory().store32(stat_address + 0x00u, (directory ? 0x1000u : 0x2000u) | 0x01FFu);
            // FIO_SO_IFDIR/FIO_SO_IFREG.
            rt.memory().store32(stat_address + 0x04u, directory ? 0x0010u : 0x0020u);
            const std::uint64_t size = regular
                ? static_cast<std::uint64_t>(std::filesystem::file_size(native, error)) : 0u;
            rt.memory().store32(stat_address + 0x08u, static_cast<std::uint32_t>(size));
            rt.memory().store32(stat_address + 0x0Cu, static_cast<std::uint32_t>(size >> 32u));
            // The three ScePspDateTime stamps at 0x10, 0x20 and 0x30. The saved
            // game list shows the modification time, so leaving these zero put
            // every save in the year zero.
            const auto written = std::filesystem::last_write_time(native, error);
            const auto system_time = std::chrono::clock_cast<std::chrono::system_clock>(written);
            const std::time_t seconds = std::chrono::system_clock::to_time_t(system_time);
            std::tm parts{};
#if defined(_WIN32)
            localtime_s(&parts, &seconds);
#else
            localtime_r(&seconds, &parts);
#endif
            for (std::uint32_t stamp : {0x10u, 0x20u, 0x30u}) {
                const std::uint32_t base = stat_address + stamp;
                rt.memory().store16(base + 0u, static_cast<std::uint16_t>(parts.tm_year + 1900));
                rt.memory().store16(base + 2u, static_cast<std::uint16_t>(parts.tm_mon + 1));
                rt.memory().store16(base + 4u, static_cast<std::uint16_t>(parts.tm_mday));
                rt.memory().store16(base + 6u, static_cast<std::uint16_t>(parts.tm_hour));
                rt.memory().store16(base + 8u, static_cast<std::uint16_t>(parts.tm_min));
                rt.memory().store16(base + 10u, static_cast<std::uint16_t>(parts.tm_sec));
            }
            set_success(ctx);
        });
    runtime.register_hle("IoFileMgrForUser", 0x109F50BCu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::string path = rt.memory().read_c_string(ctx.gpr[4]);
            const auto native = rt.translate_path(path);
            std::ios::openmode mode = std::ios::binary;
            const std::uint32_t flags = ctx.gpr[5];
            const bool file_object_diag = std::getenv("PSPRECOMP_FILE_OBJECT_DIAG") != nullptr;

            // The PSP accepts pseudo paths such as
            // disc0:/sce_lbn0x0_size0x000 for raw UMD ranges.  VCS uses the
            // zero-length form as a capability probe before loading codec
            // modules.  It is a valid empty handle and does not require ISO
            // contents.
            if (path.rfind("disc0:/sce_lbn0x", 0u) == 0u) {
                const auto size_marker = path.find("_size0x");
                if (size_marker != std::string::npos) {
                    const std::string lbn_text = path.substr(16u, size_marker - 16u);
                    const std::string size_text = path.substr(size_marker + 7u);
                    char *lbn_end = nullptr;
                    char *size_end = nullptr;
                    const unsigned long long raw_lbn = std::strtoull(lbn_text.c_str(), &lbn_end, 16);
                    const unsigned long long raw_size = std::strtoull(size_text.c_str(), &size_end, 16);
                    const bool parsed = lbn_end != lbn_text.c_str() && *lbn_end == '\0' &&
                        size_end != size_text.c_str() && *size_end == '\0' && raw_lbn <= 0xFFFFFFFFull;
                    if (parsed && raw_size == 0u) {
                        const auto fd = file_table.next_fd++;
                        file_table.synthetic_empty_files.insert(fd);
                        ctx.set_gpr(2, static_cast<std::uint32_t>(fd));
                        return;
                    }
                    if (parsed) {
                        if (const auto *disc_file = find_virtual_disc_file(
                                static_cast<std::uint32_t>(raw_lbn), raw_size)) {
                            std::fstream stream(disc_file->native_path, std::ios::binary | std::ios::in);
                            if (stream) {
                                const auto fd = file_table.next_fd++;
                                file_table.files.emplace(fd, std::move(stream));
                                if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                                    std::cerr << "[io] raw UMD open lbn=" << raw_lbn
                                              << " size=" << raw_size
                                              << " native=\"" << disc_file->native_path.string() << "\"\n";
                                }
                                ctx.set_gpr(2, static_cast<std::uint32_t>(fd));
                                return;
                            }
                        }
                        const std::uint64_t base_offset = raw_lbn * 2048ull;
                        const std::uint64_t virtual_disc_size =
                            static_cast<std::uint64_t>(file_table.next_virtual_sector) * 2048ull;
                        if (raw_size <= virtual_disc_size && base_offset <= virtual_disc_size - raw_size) {
                            const auto fd = file_table.next_fd++;
                            file_table.virtual_disc_handles.emplace(fd, VirtualDiscHandle{base_offset, raw_size, 0u});
                            if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                                std::cerr << "[io] virtual UMD range fd=" << fd << " lbn=" << raw_lbn
                                          << " size=" << raw_size << " disc_size=" << virtual_disc_size << "\n";
                            }
                            ctx.set_gpr(2, static_cast<std::uint32_t>(fd));
                            return;
                        }
                        if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                            std::cerr << "[io] unresolved raw UMD open lbn=" << raw_lbn
                                      << " size=" << raw_size << " path=\"" << path << "\"\n";
                        }
                    }
                }
            }
            if ((flags & 0x0001u) != 0u) mode |= std::ios::in;
            if ((flags & 0x0002u) != 0u) mode |= std::ios::out;
            std::fstream stream(native, mode);
            if (!stream) {
                if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                    static std::unordered_set<std::string> reported_paths;
                    if (reported_paths.insert(path).second) {
                        std::cerr << "[io] sceIoOpen failed psp=\"" << path
                                  << "\" native=\"" << native.string()
                                  << "\" flags=" << psprecomp::hex32(flags) << "\n";
                    }
                }
                if (file_object_diag) {
                    std::cerr << "[fileobj-hle] open-fail path=\"" << path
                              << "\" native=\"" << native.string()
                              << "\" flags=" << psprecomp::hex32(flags) << "\n";
                }
                ctx.set_gpr(2, 0x80010002u);
                return;
            }
            const auto fd = file_table.next_fd++;
            file_table.files.emplace(fd, std::move(stream));
            if (file_object_diag) {
                std::cerr << "[fileobj-hle] open-ok fd=" << fd << " path=\"" << path
                          << "\" native=\"" << native.string()
                          << "\" flags=" << psprecomp::hex32(flags) << "\n";
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(fd));
        });

    runtime.register_hle("IoFileMgrForUser", 0x27EB27B8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint64_t raw_offset = static_cast<std::uint64_t>(ctx.gpr[6]) |
                (static_cast<std::uint64_t>(ctx.gpr[7]) << 32u);
            const auto offset = static_cast<std::int64_t>(raw_offset);
            const auto whence = static_cast<std::int32_t>(ctx.gpr[8]);
            if (auto virtual_handle = file_table.virtual_disc_handles.find(fd);
                virtual_handle != file_table.virtual_disc_handles.end()) {
                std::int64_t base = 0;
                if (whence == 1) base = static_cast<std::int64_t>(virtual_handle->second.position);
                else if (whence == 2) base = static_cast<std::int64_t>(virtual_handle->second.length);
                else if (whence != 0) {
                    ctx.set_gpr(2, 0x80010016u);
                    ctx.set_gpr(3, 0xFFFFFFFFu);
                    return;
                }
                const std::int64_t position = base + offset;
                if (position < 0 || static_cast<std::uint64_t>(position) > virtual_handle->second.length) {
                    ctx.set_gpr(2, 0x80010016u);
                    ctx.set_gpr(3, 0xFFFFFFFFu);
                    return;
                }
                virtual_handle->second.position = static_cast<std::uint64_t>(position);
                ctx.set_gpr(2, static_cast<std::uint32_t>(position));
                ctx.set_gpr(3, static_cast<std::uint32_t>(static_cast<std::uint64_t>(position) >> 32u));
                if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr)
                    std::cerr << "[io] sceIoLseek virtual fd=" << fd << " -> " << position << "\n";
                return;
            }
            const auto it = file_table.files.find(fd);
            if (it == file_table.files.end() || whence < 0 || whence > 2) {
                if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                    std::cerr << "[io] sceIoLseek rejected fd=" << fd << " offset=" << offset
                              << " whence=" << whence << " open=" << (it != file_table.files.end()) << "\n";
                }
                ctx.set_gpr(2, 0x80010009u);
                ctx.set_gpr(3, 0xFFFFFFFFu);
                return;
            }
            std::ios_base::seekdir direction = std::ios::beg;
            if (whence == 1) direction = std::ios::cur;
            if (whence == 2) direction = std::ios::end;
            it->second.clear();
            it->second.seekg(static_cast<std::streamoff>(offset), direction);
            if (!it->second) {
                ctx.set_gpr(2, 0x80010016u);
                ctx.set_gpr(3, 0xFFFFFFFFu);
                return;
            }
            const auto position = static_cast<std::int64_t>(it->second.tellg());
            if (position < 0) {
                ctx.set_gpr(2, 0x80010016u);
                ctx.set_gpr(3, 0xFFFFFFFFu);
                return;
            }
            const auto result = static_cast<std::uint64_t>(position);
            ctx.set_gpr(2, static_cast<std::uint32_t>(result));
            ctx.set_gpr(3, static_cast<std::uint32_t>(result >> 32u));
            if (std::getenv("PSPRECOMP_IO_DIAG") != nullptr) {
                std::cerr << "[io] sceIoLseek fd=" << fd << " offset=" << offset
                          << " whence=" << whence << " -> " << position << "\n";
            }
        });

    runtime.register_hle("IoFileMgrForUser", 0x68963324u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const auto offset = static_cast<std::int32_t>(ctx.gpr[5]);
            const auto whence = static_cast<std::int32_t>(ctx.gpr[6]);
            if (auto virtual_handle = file_table.virtual_disc_handles.find(fd);
                virtual_handle != file_table.virtual_disc_handles.end()) {
                std::int64_t base = 0;
                if (whence == 1) base = static_cast<std::int64_t>(virtual_handle->second.position);
                else if (whence == 2) base = static_cast<std::int64_t>(virtual_handle->second.length);
                else if (whence != 0) { ctx.set_gpr(2, 0x80010016u); return; }
                const std::int64_t position = base + offset;
                if (position < 0 || static_cast<std::uint64_t>(position) > virtual_handle->second.length ||
                    position > 0x7FFFFFFFll) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                virtual_handle->second.position = static_cast<std::uint64_t>(position);
                ctx.set_gpr(2, static_cast<std::uint32_t>(position));
                return;
            }
            const auto it = file_table.files.find(fd);
            if (it == file_table.files.end() || whence < 0 || whence > 2) {
                ctx.set_gpr(2, 0x80010009u);
                return;
            }
            std::ios_base::seekdir direction = std::ios::beg;
            if (whence == 1) direction = std::ios::cur;
            if (whence == 2) direction = std::ios::end;
            it->second.clear();
            it->second.seekg(static_cast<std::streamoff>(offset), direction);
            if (!it->second) {
                ctx.set_gpr(2, 0x80010016u);
                return;
            }
            const auto position = static_cast<std::int64_t>(it->second.tellg());
            if (position < 0 || position > 0x7FFFFFFFll) {
                ctx.set_gpr(2, 0x80010016u);
                return;
            }
            ctx.set_gpr(2, static_cast<std::uint32_t>(position));
        });

    runtime.register_hle("IoFileMgrForUser", 0x810C4BC3u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const bool closed = file_table.files.erase(fd) == 1u ||
                file_table.synthetic_empty_files.erase(fd) == 1u ||
                file_table.virtual_disc_handles.erase(fd) == 1u;
            if (std::getenv("PSPRECOMP_FILE_OBJECT_DIAG") != nullptr)
                std::cerr << "[fileobj-hle] close fd=" << fd << " closed=" << closed << "\n";
            ctx.set_gpr(2, closed ? 0u : 0x80010009u);
        });

    runtime.register_hle("IoFileMgrForUser", 0x6A638D83u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t dst = ctx.gpr[5];
            const std::uint32_t size = ctx.gpr[6];
            if (auto virtual_handle = file_table.virtual_disc_handles.find(fd);
                virtual_handle != file_table.virtual_disc_handles.end()) {
                if (!rt.memory().contains(dst, size)) {
                    ctx.set_gpr(2, 0x80010009u);
                    return;
                }
                std::uint32_t stream_request = 0u;
                std::uint32_t release_pc_hint = 0u;
                if (thread_table.current_uid == 5 &&
                    rt.memory().contains(ctx.gpr[22] + 6916u, 4u)) {
                    stream_request = rt.memory().load32(ctx.gpr[22] + 6916u);
                    release_pc_hint = uncommitted_world_stream_release_pc(rt, stream_request);
                }
                std::uint8_t *guest_destination = rt.memory().raw_pointer(dst, size);
                if (guest_destination == nullptr) {
                    ctx.set_gpr(2, 0x80010009u);
                    return;
                }
                const bool time_io = frame_time_diag_enabled();
                const auto io_entry = time_io ? std::chrono::steady_clock::now()
                                              : std::chrono::steady_clock::time_point{};
                const std::size_t read = read_virtual_disc(
                    virtual_handle->second,
                    std::span<std::uint8_t>(guest_destination, static_cast<std::size_t>(size)));
                if (time_io) io_host_time_this_vblank += std::chrono::steady_clock::now() - io_entry;
                static const bool io_diag = std::getenv("PSPRECOMP_IO_DIAG") != nullptr;
                static const bool umd_stream_diag = std::getenv("PSPRECOMP_UMD_STREAM_DIAG") != nullptr;
                if (io_diag)
                    std::cerr << "[io] sceIoRead virtual fd=" << fd << " size=" << size << " -> " << read << "\n";
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
                return;
            }
            if (file_table.synthetic_empty_files.contains(fd)) {
                ctx.set_gpr(2, 0u);
                return;
            }
            const auto it = file_table.files.find(fd);
            if (it == file_table.files.end() || !rt.memory().contains(dst, size)) {
                ctx.set_gpr(2, 0x80010009u);
                return;
            }
            std::uint8_t *guest_destination = rt.memory().raw_pointer(dst, size);
            if (guest_destination == nullptr) {
                ctx.set_gpr(2, 0x80010009u);
                return;
            }
            const bool time_io = frame_time_diag_enabled();
            const auto io_entry = time_io ? std::chrono::steady_clock::now()
                                          : std::chrono::steady_clock::time_point{};
            it->second.read(reinterpret_cast<char *>(guest_destination),
                            static_cast<std::streamsize>(size));
            const auto read = static_cast<std::size_t>(it->second.gcount());
            if (time_io) io_host_time_this_vblank += std::chrono::steady_clock::now() - io_entry;
            ctx.set_gpr(2, static_cast<std::uint32_t>(read));
        });
}


namespace {

// Microseconds of guest time credited per outer dispatch.  A 333 MHz Allegrex
// retires roughly a few hundred instructions in a microsecond, and one chained
// dispatch covers a comparable amount of translated work, so a quarter of a
// microsecond per dispatch is the right order of magnitude.  Only monotonicity
// and rough scale matter: every consumer compares relative deadlines.
std::uint64_t starvation_tick_microseconds = 1u;

void vcs_starvation_tick(psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
    virtual_time_us += starvation_tick_microseconds;
    promote_expired_delays();

    const auto current = thread_table.threads.find(thread_table.current_uid);
    if (current == thread_table.threads.end() || current->second.state != ThreadState::Running) return;
    const auto best = best_ready_thread();
    if (best == thread_table.continuations.end()) return;
    if (thread_priority(best->uid) >= thread_priority(thread_table.current_uid)) return;

    // Resume exactly here.  Unlike an HLE-boundary preemption the thread is not
    // inside a call, so ctx.pc -- not $ra -- is the continuation point.
    enqueue_continuation(thread_table.current_uid, ctx);
    (void)activate_next_thread(ctx, "timer-preempt");
}

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
    starvation_tick_microseconds = std::max<std::uint64_t>(1u, interval / 4u);
    psprecomp::set_runtime_starvation_hook(interval == 0u ? nullptr : &vcs_starvation_tick, interval);
    std::cerr << "[scheduler-clock] dispatch_interval=" << interval
              << " tick_us=" << (interval == 0u ? 0u : starvation_tick_microseconds)
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
        vcs_interrupt_return(runtime, callback);
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
        vcs_interrupt_return(runtime, callback);
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
