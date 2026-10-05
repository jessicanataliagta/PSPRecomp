#include "psprecomp/hle/ge.hpp"

#include "psprecomp/common.hpp"
#include "psprecomp/hle/display.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp
// and ge_renderer.cpp); drawing is reached through g_ge_renderer.

namespace psprecomp::hle {
namespace {

std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}

bool frame_time_diag_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_FRAME_TIME_DIAG") != nullptr;
    return enabled;
}

bool ge_phase_diag_line_enabled() {
    static const bool enabled = std::getenv("PSPRECOMP_GE_PHASE_DIAG") != nullptr;
    return enabled;
}

float decode_float24(std::uint32_t data) noexcept {
    return std::bit_cast<float>((data & 0x00FFFFFFu) << 8u);
}

// --- Null renderer -----------------------------------------------------------
// Vertex stride per the GE vertex-type register (cmd 0x12), using the same
// component sizes and alignment as the PSP vertex decoder.
std::uint32_t ge_align_up(std::uint32_t value, std::uint32_t alignment) noexcept {
    return alignment <= 1u ? value : (value + alignment - 1u) & ~(alignment - 1u);
}

std::uint32_t ge_vertex_stride(std::uint32_t type) noexcept {
    static constexpr std::array<std::uint32_t, 4> tc_size{0u, 2u, 4u, 8u};
    static constexpr std::array<std::uint32_t, 4> tc_align{1u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 8> color_size{0u, 0u, 0u, 0u, 2u, 2u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 8> color_align{1u, 1u, 1u, 1u, 2u, 2u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> component_size{0u, 3u, 6u, 12u};
    static constexpr std::array<std::uint32_t, 4> component_align{1u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> weight_size{0u, 1u, 2u, 4u};
    const std::uint32_t tc = type & 3u, color = (type >> 2u) & 7u, normal = (type >> 5u) & 3u;
    const std::uint32_t position = (type >> 7u) & 3u, weight = (type >> 9u) & 3u;
    const std::uint32_t weight_count = ((type >> 14u) & 7u) + 1u, morph_count = ((type >> 18u) & 7u) + 1u;
    std::uint32_t offset = 0u;
    if (weight != 0u) offset = ge_align_up(offset, component_align[weight]) + weight_size[weight] * weight_count;
    offset = ge_align_up(offset, tc_align[tc]) + tc_size[tc];
    offset = ge_align_up(offset, color_align[color]) + color_size[color];
    offset = ge_align_up(offset, component_align[normal]) + component_size[normal];
    offset = ge_align_up(offset, component_align[position]) + component_size[position];
    const std::uint32_t alignment = std::max({tc_align[tc], color_align[color], component_align[normal],
                                              component_align[position], component_align[weight]});
    return ge_align_up(offset, alignment) * morph_count;
}

void advance_ge_streams(const std::array<std::uint32_t, 256> &commands, std::uint32_t count,
                        std::uint32_t vertex_address, std::uint32_t index_address,
                        std::uint32_t &next_vertex, std::uint32_t &next_index) noexcept {
    static constexpr std::array<std::uint32_t, 4> index_size{0u, 1u, 2u, 4u};
    const std::uint32_t type = commands[0x12u] & 0x00FFFFFFu;
    const std::uint32_t isize = index_size[(type >> 11u) & 3u];
    next_vertex = vertex_address;
    next_index = index_address;
    if (isize == 0u) next_vertex = vertex_address + count * ge_vertex_stride(type);
    else next_index = index_address + count * isize;
}

bool null_render_primitive(GuestMemory &, const std::array<std::uint32_t, 256> &commands,
                           const GeTransformState &, std::uint32_t vertex_address,
                           std::uint32_t index_address, std::uint32_t primitive_data,
                           GeRenderStats &stats, std::string &, std::uint32_t, std::uint64_t,
                           std::uint64_t, std::uint64_t, bool) {
    advance_ge_streams(commands, primitive_data & 0xFFFFu, vertex_address, index_address,
                       stats.next_vertex_address, stats.next_index_address);
    return true;
}

bool null_test_bounding_box(const GuestMemory &, const std::array<std::uint32_t, 256> &commands,
                            const GeTransformState &, std::uint32_t vertex_address,
                            std::uint32_t index_address, std::uint32_t count,
                            GeBoundingBoxResult &result, std::string &) {
    result.visible = true;
    advance_ge_streams(commands, count, vertex_address, index_address,
                       result.next_vertex_address, result.next_index_address);
    return true;
}

} // namespace

GeRenderer null_ge_renderer() { return GeRenderer{&null_render_primitive, &null_test_bounding_box}; }

GeRenderer g_ge_renderer = null_ge_renderer();
GeHooks g_ge_hooks{};

void reset_ge_transform_state(GeTransformState &state) noexcept {
    state = GeTransformState{};
    for (std::size_t bone = 0; bone < 8u; ++bone) {
        const std::size_t base = bone * 12u;
        state.bones[base + 0u] = 1.0f;
        state.bones[base + 4u] = 1.0f;
        state.bones[base + 8u] = 1.0f;
    }
    state.world[0] = state.world[4] = state.world[8] = 1.0f;
    state.view[0] = state.view[4] = state.view[8] = 1.0f;
    state.texture[0] = state.texture[4] = state.texture[8] = 1.0f;
    state.projection[0] = state.projection[5] = state.projection[10] = state.projection[15] = 1.0f;
    state.morph_weights[0] = 1.0f;
}

void update_ge_transform_state(GeTransformState &state, std::uint32_t command,
                               std::uint32_t data) noexcept {
    switch (command) {
    case 0x2Au:
        state.bone_cursor = data & 0x7Fu;
        break;
    case 0x2Bu: {
        // The cursor is seven bits wide, but only 0..95 are backed by matrix
        // storage. Reserved values 96..127 discard writes instead of wrapping
        // through modulo 96 and corrupting the first bones.
        const std::uint32_t index = state.bone_cursor & 0x7Fu;
        if (index < state.bones.size()) state.bones[index] = decode_float24(data);
        state.bone_cursor = (index + 1u) & 0x7Fu;
        break;
    }
    case 0x2Cu: case 0x2Du: case 0x2Eu: case 0x2Fu:
    case 0x30u: case 0x31u: case 0x32u: case 0x33u:
        state.morph_weights[command - 0x2Cu] = decode_float24(data);
        break;
    case 0x3Au:
        state.world_cursor = data & 0xFu;
        break;
    case 0x3Bu: {
        const std::uint32_t index = state.world_cursor & 0xFu;
        if (index < state.world.size()) state.world[index] = decode_float24(data);
        state.world_cursor = (index + 1u) & 0xFu;
        break;
    }
    case 0x3Cu:
        state.view_cursor = data & 0xFu;
        break;
    case 0x3Du: {
        const std::uint32_t index = state.view_cursor & 0xFu;
        if (index < state.view.size()) state.view[index] = decode_float24(data);
        state.view_cursor = (index + 1u) & 0xFu;
        break;
    }
    case 0x3Eu:
        state.projection_cursor = data & 0xFu;
        break;
    case 0x3Fu: {
        const std::uint32_t index = state.projection_cursor & 0xFu;
        state.projection[index] = decode_float24(data);
        state.projection_cursor = (index + 1u) & 0xFu;
        break;
    }
    case 0x40u:
        state.texture_cursor = data & 0xFu;
        break;
    case 0x41u: {
        const std::uint32_t index = state.texture_cursor & 0xFu;
        if (index < state.texture.size()) state.texture[index] = decode_float24(data);
        state.texture_cursor = (index + 1u) & 0xFu;
        break;
    }
    default:
        break;
    }
}

std::uint32_t ge_edram_translation{};
GeCallbackTable ge_callback_table{};

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

// Stage 45.7: the PSP GE is an independent processor.  Previous stages executed
// the complete display list inside sceGeListEnQueue(), serializing translated
// Allegrex work with vertex decode / draw preparation / DX12 accumulation.
// The async worker below turns enqueue into a producer operation and consumes
// lists on one dedicated host thread.  The worker remains strictly ordered (one
// GE command stream at a time), while the guest CPU can continue until an
// explicit GE sync or display-vblank visibility boundary requires completion.
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

std::uint64_t ge_commands_this_vblank{};

bool execute_ge_list(psprecomp::Runtime &runtime, GeListRecord &list,
                     std::vector<GuestCallbackInvocation> &callbacks,
                     const std::atomic<std::uint32_t> *async_stall) {
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
            if (g_ge_hooks.account_ge_time != nullptr)
                g_ge_hooks.account_ge_time(std::chrono::steady_clock::now() - entry);
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
            if (!g_ge_renderer.render_primitive(runtime.memory(), ge_state.commands, ge_state.transform,
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
            // PSPRECOMP_GE_DUMP_AT=vblank,n: after the n-th PRIM (1-based) of that
            // vblank, save the framebuffer it drew into (32-bit targets only).
            // Pairs with PSPRECOMP_VK_MAX_DRAW to bisect GPU/software differences.
            static const std::pair<std::uint64_t, std::uint64_t> dump_at = [] {
                std::pair<std::uint64_t, std::uint64_t> value{0u, 0u};
                if (const char *text = std::getenv("PSPRECOMP_GE_DUMP_AT"))
                    (void)std::sscanf(text, "%llu,%llu", reinterpret_cast<unsigned long long *>(&value.first),
                                      reinterpret_cast<unsigned long long *>(&value.second));
                return value;
            }();
            if (dump_at.first != 0u) {
                static std::uint64_t counted_vblank = 0u;
                static std::uint64_t prim_index = 0u;
                if (counted_vblank != display_vblank_index) { counted_vblank = display_vblank_index; prim_index = 0u; }
                ++prim_index;
                if (display_vblank_index == dump_at.first && prim_index == dump_at.second &&
                    (ge_state.commands[0xD2u] & 3u) == 3u) {
                    const std::uint32_t fb = 0x04000000u | (ge_state.commands[0x9Cu] & 0x001FFFF0u);
                    const std::uint32_t stride = ge_state.commands[0x9Du] & 0x7FCu;
                    std::ofstream out("captures/ge_dump_at.ppm", std::ios::binary);
                    out << "P6\n480 272\n255\n";
                    for (std::uint32_t y = 0; y < 272u; ++y)
                        for (std::uint32_t x = 0; x < 480u; ++x) {
                            const std::uint32_t pixel = runtime.memory().load32(fb + (y * stride + x) * 4u);
                            const char rgb[3] = {static_cast<char>(pixel), static_cast<char>(pixel >> 8u),
                                                 static_cast<char>(pixel >> 16u)};
                            out.write(rgb, 3);
                        }
                    std::cerr << "[ge-dump-at] fb=" << psprecomp::hex32(fb) << " prim " << prim_index << "\n";
                }
            }
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
                              << " texaddr=" << psprecomp::hex32((ge_state.commands[0xA0u] & 0x00FFFFFFu) |
                                                                 ((ge_state.commands[0xA8u] & 0x000F0000u) << 8u))
                              << " fb=" << psprecomp::hex32(ge_state.commands[0x9Cu] & 0x00FFFFFFu)
                              << " fbfmt=" << (ge_state.commands[0xD2u] & 3u)
                              << " alphatest=" << (ge_state.commands[0x22u] & 1u) << ':'
                              << psprecomp::hex32(ge_state.commands[0xDBu] & 0x00FFFFFFu)
                              << " stencil=" << (ge_state.commands[0x24u] & 1u) << ':'
                              << psprecomp::hex32(ge_state.commands[0xDCu] & 0x00FFFFFFu) << ':'
                              << psprecomp::hex32(ge_state.commands[0xDDu] & 0x00FFFFFFu)
                              << " blend=" << (ge_state.commands[0x21u] & 1u) << ':'
                              << psprecomp::hex32(ge_state.commands[0xDFu] & 0x00FFFFFFu)
                              << " mask=" << psprecomp::hex32(ge_state.commands[0xE8u] & 0x00FFFFFFu) << ','
                              << psprecomp::hex32(ge_state.commands[0xE9u] & 0x00FFFFFFu)
                              << " uvgen=" << psprecomp::hex32(ge_state.commands[0xC0u] & 0x00FFFFFFu)
                              << " light=" << (ge_state.commands[0x17u] & 1u)
                              << " uvscale=" << psprecomp::hex32(ge_state.commands[0x48u] & 0x00FFFFFFu) << ','
                              << psprecomp::hex32(ge_state.commands[0x49u] & 0x00FFFFFFu)
                              << " texsize=" << psprecomp::hex32(ge_state.commands[0xB8u] & 0x00FFFFFFu)
                              << " texfilter=" << psprecomp::hex32(ge_state.commands[0xC6u] & 0x00FFFFFFu)
                              << " texmode=" << psprecomp::hex32(ge_state.commands[0xC2u] & 0x00FFFFFFu)
                              << " zfunc=" << (ge_state.commands[0xDEu] & 7u)
                              << " zmask=" << (ge_state.commands[0xE7u] & 1u)
                              << " vpz=" << psprecomp::hex32(ge_state.commands[0x44u] & 0x00FFFFFFu) << ','
                              << psprecomp::hex32(ge_state.commands[0x47u] & 0x00FFFFFFu)
                              << " zrange=" << (ge_state.commands[0xD6u] & 0xFFFFu) << ','
                              << (ge_state.commands[0xD7u] & 0xFFFFu)
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
            if (!g_ge_renderer.test_bounding_box(runtime.memory(), ge_state.commands, ge_state.transform,
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


void reset_ge(const GeRenderer &renderer, const GeHooks &hooks) {
    g_ge_renderer = renderer.render_primitive != nullptr && renderer.test_bounding_box != nullptr
        ? renderer : null_ge_renderer();
    g_ge_hooks = hooks;
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
}

void install_ge_hle(psprecomp::Runtime &runtime) {
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
}

} // namespace psprecomp::hle
