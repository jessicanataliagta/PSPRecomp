#pragma once

// Generic PSP file-system HLE shared by title profiles: host-backed ms0:/host0:
// files and directories, a virtual UMD (disc0:/umd0: sector access mapped onto
// the extracted PSP_GAME tree), and the IoFileMgrForUser / ModuleMgrForUser /
// sceUmdUser imports. Paths resolve through Runtime::translate_path().

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace psprecomp::hle {

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

struct DiscReadStats {
    std::uint64_t bytes_from_files{};
    std::uint64_t bytes_zero_filled{};
    std::uint64_t zero_fill_events{};
    std::uint64_t short_reads{};
    std::uint64_t open_failures{};
    std::uint64_t reported_events{};
};

struct IoHooks {
    // Runs before a virtual-disc sceIoRead copies data; the returned token is
    // passed to after_umd_read. Guest memory still holds pre-read contents.
    std::uint64_t (*before_umd_read)(Runtime &runtime, AllegrexContext &ctx) = nullptr;
    // Completes a virtual-disc sceIoRead (sets $v0 and may switch threads).
    // Without a hook the read returns the byte count synchronously.
    void (*after_umd_read)(Runtime &runtime, AllegrexContext &ctx, std::uint32_t requested,
                           std::size_t read, std::uint64_t token) = nullptr;
};

extern IoHooks g_io_hooks;
extern FileTable file_table;
extern DiscReadStats disc_read_stats;
// Host time spent inside sceIoRead, accumulated only with PSPRECOMP_FRAME_TIME_DIAG.
extern std::chrono::steady_clock::duration io_host_time_this_vblank;
extern std::int32_t next_module_uid;
extern std::unordered_map<std::int32_t, bool> loaded_modules;
extern std::uint32_t memory_stick_fat_state;
// Results of completed *Async operations, per fd, until sceIoPollAsync /
// sceIoWaitAsync consumes them. Async I/O is performed synchronously.
extern std::unordered_map<std::int32_t, std::int64_t> async_io_results;

std::string normalized_native_path(const std::filesystem::path &path);
const VirtualDiscFile *register_virtual_disc_file(const std::filesystem::path &path);
const VirtualDiscFile *find_virtual_disc_file(std::uint32_t start_sector, std::uint64_t requested_size);
bool disc_read_diag_enabled();
std::size_t read_virtual_disc(VirtualDiscHandle &handle, std::span<std::uint8_t> output);

void reset_io(const IoHooks &hooks = {});
void install_io_hle(Runtime &runtime);

} // namespace psprecomp::hle
