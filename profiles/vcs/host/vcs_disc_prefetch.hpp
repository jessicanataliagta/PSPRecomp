#pragma once

// [Disc] Prefetch: warm the OS page cache with the game's data files.
//
// World streaming reads PSP_GAME/USRDIR synchronously inside the guest's
// frame. When those bytes are not cached yet, every request pays the storage
// path -- measured at ~40 ms per streaming read on a zstd-compressed btrfs
// /home, 10 of 17 >40 ms frames in a ten-minute drive. With the same files
// already cached the >40 ms frames dropped from 17 to 5 and the I/O spikes
// from 10 to 2.
//
// So read every file once, largest (the streaming archives) first, on a
// detached low-priority thread. The data is discarded: only the OS keeps it,
// so the process footprint does not grow, and the guest's own reads still go
// through the normal path. Nothing waits on this thread.

#include "vcs_runtime_log.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace vcs {

inline void start_disc_prefetch(const std::filesystem::path &game_root) {
    std::filesystem::path data = game_root / "PSP_GAME" / "USRDIR";
    runtime_log_line("disc prefetch started root=" + data.string());
    std::thread([data = std::move(data)] {
#ifdef _WIN32
        // Background mode lowers both CPU and I/O priority, so the guest's own
        // synchronous reads are served first.
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
#endif
        const auto started = std::chrono::steady_clock::now();
        std::vector<std::pair<std::uintmax_t, std::filesystem::path>> files;
        std::error_code error;
        for (std::filesystem::recursive_directory_iterator it(data, error), end;
             !error && it != end; it.increment(error)) {
            std::error_code entry_error;
            if (!it->is_regular_file(entry_error) || entry_error) continue;
            const std::uintmax_t size = it->file_size(entry_error);
            if (!entry_error && size != 0u) files.emplace_back(size, it->path());
        }
        std::sort(files.begin(), files.end(),
                  [](const auto &a, const auto &b) { return a.first > b.first; });

        std::vector<char> buffer(1u << 20u);
        std::uintmax_t total = 0u;
        for (const auto &[size, path] : files) {
            std::ifstream input(path, std::ios::binary);
            while (input.read(buffer.data(), static_cast<std::streamsize>(buffer.size())) ||
                   input.gcount() > 0)
                total += static_cast<std::uintmax_t>(input.gcount());
        }
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - started).count();
        std::ostringstream line;
        line << "disc prefetch done files=" << files.size() << " MiB=" << (total >> 20u)
             << " ms=" << elapsed;
        runtime_log_line(line.str());
    }).detach();
}

} // namespace vcs
