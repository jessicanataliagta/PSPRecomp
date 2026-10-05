#pragma once

// Generic PSP GE (graphics engine) HLE shared by title profiles: display-list
// interpretation (jumps, calls, signals, stalls, finish), GE context save and
// restore, finish/signal guest callbacks, the optional async list worker
// (PSPRECOMP_GE_ASYNC) and the sceGe_user imports. Drawing is delegated to a
// GeRenderer; the default renderer only advances the vertex/index streams.

#include "psprecomp/allegrex_context.hpp"
#include "psprecomp/guest_memory.hpp"
#include "psprecomp/hle/kernel.hpp"
#include "psprecomp/runtime.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace psprecomp::hle {

// Matrix state is separate from the 256 GE command registers. Matrix DATA commands
// auto-increment an internal cursor, so retaining only the latest command word loses
// almost the entire matrix. Keep the expanded state here and feed it to each PRIM.
struct GeTransformState {
    std::array<float, 96> bones{};
    std::array<float, 12> world{};
    std::array<float, 12> view{};
    std::array<float, 16> projection{};
    std::array<float, 12> texture{};
    std::array<float, 8> morph_weights{};
    std::uint32_t bone_cursor{};
    std::uint32_t world_cursor{};
    std::uint32_t view_cursor{};
    std::uint32_t projection_cursor{};
    std::uint32_t texture_cursor{};
};

void reset_ge_transform_state(GeTransformState &state) noexcept;
void update_ge_transform_state(GeTransformState &state, std::uint32_t command,
                               std::uint32_t data) noexcept;

struct GeBoundingBoxResult {
    bool visible{};
    std::uint32_t next_vertex_address{};
    std::uint32_t next_index_address{};
};

struct GeRenderStats {
    std::uint64_t primitives{};
    std::uint64_t points{};
    std::uint64_t lines{};
    std::uint64_t triangles{};
    std::uint64_t rectangles{};
    std::uint64_t skinned_vertices{};
    std::uint64_t morphed_vertices{};
    std::uint64_t lit_vertices{};
    std::uint64_t generated_uv_vertices{};
    std::uint64_t culled_triangles{};
    std::uint64_t flat_shaded_primitives{};
    std::uint64_t pixels_tested{};
    std::uint64_t pixels_written{};
    std::uint64_t unsupported_primitives{};
    std::uint64_t decoded_vertices{};
    std::uint64_t nonfinite_clip_vertices{};
    std::uint64_t screen_vertices{};
    bool has_clip_bounds{};
    bool has_screen_bounds{};
    float clip_min_x{};
    float clip_min_y{};
    float clip_min_z{};
    float clip_min_w{};
    float clip_max_x{};
    float clip_max_y{};
    float clip_max_z{};
    float clip_max_w{};
    float screen_min_x{};
    float screen_min_y{};
    float screen_max_x{};
    float screen_max_y{};
    float min_abs_w{};
    float max_abs_screen_coordinate{};
    std::uint32_t next_vertex_address{};
    std::uint32_t next_index_address{};
};

void reset_ge_transform_state(GeTransformState &state) noexcept;
void update_ge_transform_state(GeTransformState &state, std::uint32_t command,
                               std::uint32_t data) noexcept;

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

inline constexpr std::uint32_t kGeListIdMagic = 0x35000000u;
inline constexpr std::uint32_t kGeCommandNop = 0x00u;
inline constexpr std::uint32_t kGeCommandVertexAddress = 0x01u;
inline constexpr std::uint32_t kGeCommandIndexAddress = 0x02u;
inline constexpr std::uint32_t kGeCommandPrimitive = 0x04u;
inline constexpr std::uint32_t kGeCommandBoundingBox = 0x07u;
inline constexpr std::uint32_t kGeCommandJump = 0x08u;
inline constexpr std::uint32_t kGeCommandBoundingBoxJump = 0x09u;
inline constexpr std::uint32_t kGeCommandCall = 0x0Au;
inline constexpr std::uint32_t kGeCommandReturn = 0x0Bu;
inline constexpr std::uint32_t kGeCommandEnd = 0x0Cu;
inline constexpr std::uint32_t kGeCommandSignal = 0x0Eu;
inline constexpr std::uint32_t kGeCommandFinish = 0x0Fu;
inline constexpr std::uint32_t kGeCommandBase = 0x10u;
inline constexpr std::uint32_t kGeCommandOffsetAddress = 0x13u;

inline constexpr std::uint32_t kGeCommandOrigin = 0x14u;

inline constexpr std::uint8_t kGeSignalNone = 0x00u;
inline constexpr std::uint8_t kGeSignalHandlerSuspend = 0x01u;
inline constexpr std::uint8_t kGeSignalHandlerContinue = 0x02u;
inline constexpr std::uint8_t kGeSignalHandlerPause = 0x03u;
inline constexpr std::uint8_t kGeSignalSync = 0x08u;
inline constexpr std::uint8_t kGeSignalJump = 0x10u;
inline constexpr std::uint8_t kGeSignalCall = 0x11u;
inline constexpr std::uint8_t kGeSignalReturn = 0x12u;
inline constexpr std::uint8_t kGeSignalRelativeJump = 0x13u;
inline constexpr std::uint8_t kGeSignalRelativeCall = 0x14u;
inline constexpr std::uint8_t kGeSignalOriginJump = 0x15u;
inline constexpr std::uint8_t kGeSignalOriginCall = 0x16u;

struct GeRenderer {
    // Executes one PRIM: draw and report the advanced streams in stats.next_*.
    bool (*render_primitive)(GuestMemory &memory, const std::array<std::uint32_t, 256> &commands,
                             const GeTransformState &transform, std::uint32_t vertex_address,
                             std::uint32_t index_address, std::uint32_t primitive_data,
                             GeRenderStats &stats, std::string &error,
                             std::uint32_t logical_primitive_count, std::uint64_t draw_state_revision,
                             std::uint64_t camera_state_revision, std::uint64_t lighting_state_revision,
                             bool collect_diagnostic_stats) = nullptr;
    // Executes one BBOX visibility test and reports the advanced streams.
    bool (*test_bounding_box)(const GuestMemory &memory, const std::array<std::uint32_t, 256> &commands,
                              const GeTransformState &transform, std::uint32_t vertex_address,
                              std::uint32_t index_address, std::uint32_t count,
                              GeBoundingBoxResult &result, std::string &error) = nullptr;
};

struct GeHooks {
    // Host time spent interpreting one list run (PSPRECOMP_FRAME_TIME_DIAG only).
    void (*account_ge_time)(std::chrono::steady_clock::duration elapsed) = nullptr;
};

// Renderer that draws nothing: PRIM and BBOX only advance the vertex/index
// streams (BBOX reports visible), so lists, signals and callbacks still run.
GeRenderer null_ge_renderer();

extern GeRenderer g_ge_renderer;
extern GeHooks g_ge_hooks;
extern std::uint32_t ge_edram_translation;
extern GeCallbackTable ge_callback_table;
extern GeState ge_state;
extern std::uint64_t ge_draw_state_revision;
extern std::uint64_t ge_lighting_state_revision;
extern std::uint64_t ge_camera_state_revision;
extern GeListTable ge_list_table;
extern GeAsyncWorkerState ge_async;
extern thread_local bool ge_async_worker_thread;
// GE command words interpreted since the last vblank report.
extern std::uint64_t ge_commands_this_vblank;

bool ge_async_enabled() noexcept;
bool ge_async_running() noexcept;
void ge_async_worker_main();
void ge_async_drain_completions();
bool ge_async_wait_idle(Runtime &runtime);
bool ge_async_wait_list(Runtime &runtime, std::uint32_t id);
bool ge_async_check_fatal(Runtime &runtime);
void ge_async_stop_worker();
void ge_async_record_fatal(std::string reason);
void ge_execution_stop(Runtime &runtime, std::string reason);
void ge_async_start_worker(Runtime &runtime);
std::uint32_t ge_float24_command(std::uint32_t command, float value);
bool ge_command_affects_lighting(std::uint32_t command) noexcept;
void write_ge_context_buffer(Runtime &runtime, std::uint32_t address, const GeState &state);
void save_ge_list_context(Runtime &runtime, GeListRecord &record);
void restore_ge_list_context(const GeListRecord &record);
std::uint32_t ge_relative_address(std::uint32_t data);
std::uint32_t ge_list_status(const GeListRecord &list);
const char *ge_command_name(std::uint32_t command);
bool ge_histogram_diag_enabled() noexcept;
void log_ge_histogram(const GeListRecord &list);
bool append_ge_callback(const GeListRecord &list, bool signal, std::uint16_t token,
                        std::uint32_t next_pc, std::vector<GuestCallbackInvocation> &callbacks);
bool execute_ge_list(Runtime &runtime, GeListRecord &list, std::vector<GuestCallbackInvocation> &callbacks,
                     const std::atomic<std::uint32_t> *async_stall = nullptr);
std::uint32_t allocate_ge_list_id();
void enqueue_ge_display_list(Runtime &runtime, AllegrexContext &ctx, bool head);

// Stops the async worker and resets all GE state. Without a renderer the null
// renderer is used.
void reset_ge(const GeRenderer &renderer = {}, const GeHooks &hooks = {});
void install_ge_hle(Runtime &runtime);

} // namespace psprecomp::hle
