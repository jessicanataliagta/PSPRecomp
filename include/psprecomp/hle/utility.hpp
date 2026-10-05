#pragma once

// Generic sceUtility save-data dialog HLE shared by title profiles: init /
// update / status / shutdown of the savedata utility, with load, save, list
// and size queries performed directly on host files under the savedata root.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/runtime.hpp"

#include <cstdint>
#include <filesystem>
#include <string>

namespace psprecomp::hle {

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


inline constexpr std::uint32_t kUtilityCommonResultOffset = 0x1Cu;
inline constexpr std::uint32_t kSavedataModeOffset = 0x30u;
inline constexpr std::uint32_t kSavedataGameNameOffset = 0x3Cu;
inline constexpr std::uint32_t kSavedataSaveNameOffset = 0x4Cu;
inline constexpr std::uint32_t kSavedataFileNameOffset = 0x64u;
inline constexpr std::uint32_t kSavedataDataBufferOffset = 0x74u;
inline constexpr std::uint32_t kSavedataDataBufferSizeOffset = 0x78u;
inline constexpr std::uint32_t kSavedataDataSizeOffset = 0x7Cu;
inline constexpr std::uint32_t kSavedataIcon0Offset = 0x584u;
inline constexpr std::uint32_t kSavedataIcon1Offset = 0x594u;
inline constexpr std::uint32_t kSavedataPic1Offset = 0x5A4u;
inline constexpr std::uint32_t kSavedataSnd0Offset = 0x5B4u;
inline constexpr std::uint32_t kSavedataIdListOffset = 0x5F4u;
inline constexpr std::uint32_t kSavedataFileListOffset = 0x5F8u;
inline constexpr std::uint32_t kSavedataSizeInfoOffset = 0x5FCu;
inline constexpr std::uint32_t kSavedataParameterMinimumSize = 0x600u;

struct UtilityHooks {
    // Host directory holding PSP/SAVEDATA-style save folders; when unset or
    // empty, <game root>/PSP/SAVEDATA is used.
    std::filesystem::path (*savedata_root)() = nullptr;
};

extern UtilityHooks g_utility_hooks;
extern SavedataUtilityState savedata_utility;

std::string read_fixed_string(const psprecomp::GuestMemory &memory, std::uint32_t address, std::size_t size);
std::string safe_savedata_component(std::string value);
std::filesystem::path savedata_root(const psprecomp::Runtime &runtime);
std::filesystem::path savedata_directory(const psprecomp::Runtime &runtime, std::uint32_t parameter_address);
bool write_guest_file(psprecomp::Runtime &runtime, const std::filesystem::path &path,
                      std::uint32_t buffer, std::uint32_t size);
bool write_savedata_auxiliary(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                              std::uint32_t descriptor_offset, const char *filename);
std::uint32_t load_savedata_file(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                                 const std::filesystem::path &path, bool raw_mode);
std::uint32_t save_savedata_file(psprecomp::Runtime &runtime, std::uint32_t parameter_address,
                                 const std::filesystem::path &path, bool raw_mode);
std::uint32_t list_savedata_directories(psprecomp::Runtime &runtime, std::uint32_t parameter_address);
std::uint64_t directory_size_bytes(const std::filesystem::path &directory);
void write_small_size_string(psprecomp::GuestMemory &memory, std::uint32_t address, std::uint64_t kilobytes);
void write_used_data_info(psprecomp::GuestMemory &memory, std::uint32_t address,
                          std::uint64_t used_bytes, std::uint32_t cluster_size);
std::uint32_t query_savedata_sizes(psprecomp::Runtime &runtime, std::uint32_t parameter_address);
std::uint32_t execute_savedata_operation(psprecomp::Runtime &runtime, std::uint32_t parameter_address);

void reset_utility(const UtilityHooks &hooks = {});
void install_utility_hle(Runtime &runtime);

} // namespace psprecomp::hle
