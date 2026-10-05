#include "psprecomp/hle/io.hpp"

#include "psprecomp/common.hpp"
#include "psprecomp/hle/kernel.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <system_error>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp);
// title-specific behaviour is reached through g_io_hooks.

namespace psprecomp::hle {
namespace {

bool frame_time_diag_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_FRAME_TIME_DIAG") != nullptr;
    return enabled;
}

} // namespace

IoHooks g_io_hooks{};

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

std::int32_t next_module_uid{0x400};
std::unordered_map<std::int32_t, bool> loaded_modules;
std::uint32_t memory_stick_fat_state{1u};

std::unordered_map<std::int32_t, std::int64_t> async_io_results;

void reset_io(const IoHooks &hooks) {
    g_io_hooks = hooks;
    file_table = FileTable{};
    async_io_results.clear();
    loaded_modules.clear();
    next_module_uid = 0x400;
    memory_stick_fat_state = 1u;
}

namespace {

constexpr std::uint32_t kIoErrorBadFile = 0x80010009u;
constexpr std::uint32_t kIoErrorNoAsync = 0x80020329u;  // no completed async operation on this fd

// Runs a synchronous IoFileMgrForUser import on the same arguments and returns its $v0.
std::uint32_t run_sync_io(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx, std::uint32_t nid) {
    rt.invoke_import("IoFileMgrForUser", nid, ctx);
    return ctx.gpr[2];
}

std::int64_t signed_io_result(std::uint32_t value) {
    return static_cast<std::int64_t>(static_cast<std::int32_t>(value));
}

// sceIoPollAsync / sceIoWaitAsync: report and consume a completed result.
void finish_async_io(psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
    const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
    const std::uint32_t result_address = ctx.gpr[5];
    const auto found = async_io_results.find(fd);
    if (found == async_io_results.end()) {
        ctx.set_gpr(2, kIoErrorNoAsync);
        return;
    }
    if (result_address != 0u && rt.memory().contains(result_address, 8u)) {
        const auto value = static_cast<std::uint64_t>(found->second);
        rt.memory().store32(result_address, static_cast<std::uint32_t>(value));
        rt.memory().store32(result_address + 4u, static_cast<std::uint32_t>(value >> 32u));
    }
    async_io_results.erase(found);
    set_success(ctx);
}

} // namespace

void install_io_hle(psprecomp::Runtime &runtime) {
    // --- Async I/O (not used by VCS): each operation completes immediately. ---
    runtime.register_hle("IoFileMgrForUser", 0x89AA9906u,  // sceIoOpenAsync(path, flags, mode)
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t fd = run_sync_io(rt, ctx, 0x109F50BCu);
            if (static_cast<std::int32_t>(fd) >= 0)
                async_io_results[static_cast<std::int32_t>(fd)] = static_cast<std::int32_t>(fd);
            ctx.set_gpr(2, fd);
        });
    runtime.register_hle("IoFileMgrForUser", 0xFF5940B6u,  // sceIoCloseAsync(fd)
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            async_io_results[fd] = signed_io_result(run_sync_io(rt, ctx, 0x810C4BC3u));
            set_success(ctx);
        });
    runtime.register_hle("IoFileMgrForUser", 0xA0B5A7C2u,  // sceIoReadAsync(fd, buffer, size)
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            async_io_results[fd] = signed_io_result(run_sync_io(rt, ctx, 0x6A638D83u));
            set_success(ctx);
        });
    runtime.register_hle("IoFileMgrForUser", 0x3251EA56u, &finish_async_io);  // sceIoPollAsync
    runtime.register_hle("IoFileMgrForUser", 0xE23EEC33u, &finish_async_io);  // sceIoWaitAsync
    runtime.register_hle("IoFileMgrForUser", 0x35DBD746u, &finish_async_io);  // sceIoWaitAsyncCB

    // sceIoWrite(fd, data, size): host files opened for writing; fd 1/2 go to the console.
    runtime.register_hle("IoFileMgrForUser", 0x42EC03ACu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const auto fd = static_cast<std::int32_t>(ctx.gpr[4]);
            const std::uint32_t source = ctx.gpr[5];
            const std::uint32_t size = ctx.gpr[6];
            const std::uint8_t *data = rt.memory().raw_pointer(source, size);
            if (data == nullptr && size != 0u) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            if (fd == 1 || fd == 2) {
                (fd == 1 ? std::cout : std::cerr).write(reinterpret_cast<const char *>(data), size);
                ctx.set_gpr(2, size);
                return;
            }
            const auto file = file_table.files.find(fd);
            if (file == file_table.files.end()) {
                ctx.set_gpr(2, file_table.synthetic_empty_files.contains(fd) ? size : kIoErrorBadFile);
                return;
            }
            file->second.write(reinterpret_cast<const char *>(data), static_cast<std::streamsize>(size));
            ctx.set_gpr(2, file->second ? size : kIoErrorBadFile);
        });
    // sceIoRename(old, new) within the translated game root.
    runtime.register_hle("IoFileMgrForUser", 0x779103A0u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            std::error_code error;
            std::filesystem::rename(rt.translate_path(rt.memory().read_c_string(ctx.gpr[4], 256u)),
                                    rt.translate_path(rt.memory().read_c_string(ctx.gpr[5], 256u)), error);
            ctx.set_gpr(2, error ? 0x80010002u : 0u);
        });
    // sceKernelGetModuleIdByAddress: the only module whose code the guest can
    // name is the main executable, which has a fixed uid below the loaded range.
    runtime.register_hle("ModuleMgrForUser", 0xD8B73127u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0x3FFu); });
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
            // Register / unregister a memory-stick insert-eject callback. The
            // stick is always inserted, so the kernel reports that at once.
            if (notify_device_callbacks && (command == 0x02415821u || command == 0x02015804u) &&
                (device == "fatms0:" || device == "ms0:" || device == "mscmhc0:")) {
                if (input == 0u || input_length < 4u || !rt.memory().contains(input, 4u)) {
                    ctx.set_gpr(2, 0x80010016u);
                    return;
                }
                const auto callback = static_cast<std::int32_t>(rt.memory().load32(input));
                ctx.set_gpr(2, notify_callback(callback, 1u) ? 0u : 0x80010016u);
                return;
            }
            if (notify_device_callbacks && (command == 0x02415822u || command == 0x02015805u) &&
                (device == "fatms0:" || device == "ms0:" || device == "mscmhc0:")) {
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
                const std::uint64_t read_token = g_io_hooks.before_umd_read != nullptr
                    ? g_io_hooks.before_umd_read(rt, ctx) : 0u;
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
                if (io_diag)
                    std::cerr << "[io] sceIoRead virtual fd=" << fd << " size=" << size << " -> " << read << "\n";
                if (g_io_hooks.after_umd_read != nullptr) {
                    g_io_hooks.after_umd_read(rt, ctx, size, read, read_token);
                } else {
                    ctx.set_gpr(2, static_cast<std::uint32_t>(read));
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

} // namespace psprecomp::hle
