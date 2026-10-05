// Vulkan GE backend. Same contract as the VCS DX12 backend
// (profiles/vcs/host/ge_gpu_backend_dx12.cpp), which this follows closely:
//
//  * The GE renderer reports every draw (record_draw), uploads decoded
//    textures, and hands over either screen-space triangles or model-space
//    vertices with a per-draw transform (hardware transform).
//  * Draws accumulate into batches. finish_frame() replays them in order, each
//    into the render target for its PSP framebuffer address. Targets persist
//    across frames like EDRAM; a draw that samples a target's address reads
//    the GPU image (render to texture), snapshotting it when the draw renders
//    into that same target.
//  * Depth is the PSP's 16-bit value / 65535 with the guest's compare
//    functions unchanged. Like EDRAM, colour and depth persist until the game
//    clears them (initially black and 0); unlike the DX12 backend nothing is
//    cleared per frame, which would cut a frame whose draws span two vblanks.
//  * The displayed target is copied to a host buffer per frame slot; the
//    frontend presents it (latest_frame). There is no swapchain here, so the
//    same path works windowed, headless and for captures.
//
// Requires Vulkan 1.3 (dynamic rendering, synchronization2): the Steam Deck's
// RADV and current desktop drivers all provide it.

#include "psprecomp/hle/ge_gpu_vulkan.hpp"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

namespace psprecomp::hle {
namespace {

#include "ge_gpu_vulkan_shaders.inc"

constexpr std::uint32_t kReferenceWidth = 480u;
constexpr std::uint32_t kReferenceHeight = 272u;
constexpr std::uint32_t kFrameCount = 2u;
constexpr std::size_t kGeometryCapacity = 32u * 1024u * 1024u;
constexpr std::size_t kTextureStagingCapacity = 32u * 1024u * 1024u;
constexpr VkFormat kColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
// The PSP depth buffer is 16-bit; quantizing the same way keeps EQUAL/LEQUAL
// multi-pass geometry stable when its passes come from different transform
// paths (CPU screen space vs. hardware transform).
constexpr VkFormat kDepthFormat = VK_FORMAT_D16_UNORM;

struct UploadVertex {
    float x{}, y{}, z{}, w{1.0f};
    std::uint32_t rgba{0xFFFFFFFFu};
    float u{}, v{};
    float fog_factor{1.0f};
    float q{1.0f};
};
static_assert(sizeof(UploadVertex) == 36u);

// Mirrors the push-constant block in src/hle/shaders/ge_common.glsl.
struct PushConstants {
    std::array<float, 4> row0{}, row1{}, row2{}, row3{};
    std::array<float, 4> view_z{};
    std::array<float, 4> uv{1.0f, 1.0f, 0.0f, 0.0f};
    std::array<float, 4> fog{};
    std::array<std::uint32_t, 4> control{};
    std::array<float, 4> color_mul{1.0f, 1.0f, 1.0f, 1.0f};
    std::array<float, 4> color_add{};
    std::array<std::uint32_t, 4> pixel{};
    std::array<std::uint32_t, 4> pixel2{};
    // Applied to normalized texture coordinates before sampling: a render
    // target holds only its logical extent, while the guest addresses it as a
    // texture of its own (power-of-two) size.
    std::array<float, 4> sample{1.0f, 1.0f, 0.0f, 0.0f};
};
static_assert(sizeof(PushConstants) == 208u);

struct Batch {
    GeGpuDrawDescriptor draw{};
    std::uint32_t first_vertex{};
    std::uint32_t vertex_count{};
    std::uint32_t first_index{};
    std::uint32_t index_count{};
    bool indexed{};
    bool framebuffer_feedback{};
    std::uint32_t feedback_address{};
    bool hardware_transform{};
    GeGpuHardwareTransform transform{};
    std::uint32_t draw_index{};  // 1-based PRIM index within the frame (diagnostics)
};

struct Buffer {
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    std::byte *mapped{};
    VkDeviceSize size{};
    bool coherent{true};
};

struct Image {
    VkImage image{};
    VkDeviceMemory memory{};
    VkImageView view{};
    VkImageLayout layout{VK_IMAGE_LAYOUT_UNDEFINED};
};

struct Texture {
    GeGpuDrawDescriptor descriptor{};
    Image image;
    std::uint32_t width{}, height{}, mip_levels{1u};
    std::uint64_t checksum{};
    std::uint64_t signature_epoch{};
    std::uint64_t last_used_epoch{};
    std::vector<std::byte> pending_rgba;  // uploaded at the next finish_frame
    std::size_t bytes{};
};

struct Target {
    std::uint32_t address{};
    std::uint32_t logical_width{};
    std::uint32_t logical_height{};
    Image color;
    Image depth;
    Image snapshot;  // self-feedback copy, created on demand
    std::uint64_t last_render_epoch{};
};

struct FrameSlot {
    VkCommandBuffer commands{};
    VkFence fence{};
    Buffer geometry;
    Buffer staging;
    Buffer readback;
    std::uint64_t serial{};
    bool readback_valid{};
    std::uint64_t readback_vblank{};
};

struct VulkanState {
    GeGpuBackendReport report{};
    bool enabled{};
    std::uint32_t scale{2u};
    std::uint32_t target_width{kReferenceWidth * 2u};
    std::uint32_t target_height{kReferenceHeight * 2u};
    std::uint32_t display_framebuffer{};
    std::string device_name;

    VkInstance instance{};
    VkPhysicalDevice physical{};
    VkDevice device{};
    VkQueue queue{};
    std::uint32_t queue_family{};
    VkPhysicalDeviceMemoryProperties memory_properties{};
    VkCommandPool command_pool{};
    VkDescriptorSetLayout set_layout{};
    VkPipelineLayout pipeline_layout{};
    VkShaderModule vertex_shader{};
    VkShaderModule fragment_shader{};
    std::vector<VkDescriptorPool> descriptor_pools;
    std::unordered_map<std::uint64_t, VkPipeline> pipelines;
    std::unordered_map<std::uint64_t, VkSampler> samplers;
    // (image view, sampler) -> descriptor set and the pool it came from.
    struct DescriptorEntry {
        VkDescriptorSet set{};
        VkDescriptorPool pool{};
    };
    std::unordered_map<std::uint64_t, DescriptorEntry> descriptor_sets;
    Image white;
    VkSampler white_sampler{};

    std::array<FrameSlot, kFrameCount> frames{};
    std::uint32_t frame_cursor{};
    std::uint64_t submitted_serial{};
    std::uint64_t completed_serial{};
    // Destroyers queued until every frame that may use the object has finished.
    std::vector<std::pair<std::uint64_t, std::function<void()>>> deferred;

    std::unordered_map<std::uint32_t, Target> targets;
    std::uint32_t last_registered_target{0xFFFFFFFFu};
    std::unordered_map<std::uint64_t, Texture> textures;
    std::vector<std::uint64_t> pending_textures;
    std::uint64_t texture_cache_bytes{};
    std::size_t texture_cache_entry_limit{2048u};
    std::uint64_t texture_cache_byte_limit{512ull * 1024ull * 1024ull};
    std::uint64_t frame_epoch{1u};

    std::vector<UploadVertex> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<Batch> batches;
    std::uint32_t frame_draws{};
    // PSPRECOMP_VK_MAX_DRAW=vblank,n: in the frame finished at that vblank,
    // replay only PRIMs up to the n-th.
    std::uint32_t max_draw{};
    std::uint64_t max_draw_vblank{};

    std::vector<std::byte> frame_rgba;
    std::uint64_t frame_vblank{};
    std::uint64_t frame_serial{};
};

VulkanState &state() {
    static VulkanState s;
    return s;
}

// ----------------------------------------------------------------------------
// Keys and PSP state mapping (shared with the DX12 backend's definitions).

std::uint64_t hash_mix(std::uint64_t hash, std::uint64_t value) noexcept {
    hash ^= value + 0x9E3779B97F4A7C15ull + (hash << 6u) + (hash >> 2u);
    return hash;
}

std::uint64_t texture_key(const GeGpuDrawDescriptor &draw) noexcept {
    if (draw.texture_cache_key_hint != 0u) return draw.texture_cache_key_hint;
    std::uint64_t key = 0xCBF29CE484222325ull;
    const std::uint32_t levels = draw.texture_level_addresses[0] != 0u && draw.texture_mipmap_enabled
        ? std::min<std::uint32_t>(8u, draw.texture_max_level + 1u) : 1u;
    key = hash_mix(key, levels);
    for (std::uint32_t level = 0u; level < levels; ++level) {
        key = hash_mix(key, draw.texture_level_addresses[level] != 0u
            ? draw.texture_level_addresses[level] : draw.texture_address);
        key = hash_mix(key, draw.texture_level_buffer_widths[level] != 0u
            ? draw.texture_level_buffer_widths[level] : draw.texture_buffer_width);
        key = hash_mix(key, draw.texture_level_widths[level] != 0u
            ? draw.texture_level_widths[level] : draw.texture_width);
        key = hash_mix(key, draw.texture_level_heights[level] != 0u
            ? draw.texture_level_heights[level] : draw.texture_height);
    }
    key = hash_mix(key, draw.texture_format);
    key = hash_mix(key, draw.clut_address);
    key = hash_mix(key, draw.clut_format);
    key = hash_mix(key, draw.clut_shift);
    key = hash_mix(key, draw.clut_mask);
    key = hash_mix(key, draw.clut_start);
    key = hash_mix(key, draw.clut_checksum);
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_swizzled));
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_min_linear));
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_mag_linear));
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_mipmap_enabled));
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_mipmap_linear));
    key = hash_mix(key, draw.texture_max_level);
    key = hash_mix(key, draw.texture_level_mode);
    key = hash_mix(key, static_cast<std::uint32_t>(draw.texture_level_offset16));
    key = hash_mix(key, draw.texture_selected_level);
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_clamp_u));
    key = hash_mix(key, static_cast<std::uint64_t>(draw.texture_clamp_v));
    return key;
}

std::uint64_t fnv1a64(std::span<const std::byte> bytes) noexcept {
    std::uint64_t hash = 1469598103934665603ull;
    for (const std::byte b : bytes) {
        hash ^= static_cast<std::uint8_t>(b);
        hash *= 1099511628211ull;
    }
    return hash;
}

VkCompareOp compare_op(std::uint32_t function) noexcept {
    switch (function & 7u) {
    case 0u: return VK_COMPARE_OP_NEVER;
    case 1u: return VK_COMPARE_OP_ALWAYS;
    case 2u: return VK_COMPARE_OP_EQUAL;
    case 3u: return VK_COMPARE_OP_NOT_EQUAL;
    case 4u: return VK_COMPARE_OP_LESS;
    case 5u: return VK_COMPARE_OP_LESS_OR_EQUAL;
    case 6u: return VK_COMPARE_OP_GREATER;
    default: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    }
}

// PSP blend state as a Vulkan pipeline expresses it. The GE has separate
// fixed source and destination colours (factor 10) and Vulkan one blend
// constant, so a fixed source colour is multiplied into the fragment colour by
// the shader (factor ONE) and the blend constant serves the destination; that
// is exact for every equation. Fixed colours have alpha 255 (factor 1 on
// alpha), as in the software rasterizer. The doubled alpha factors (6..9) are
// approximated by their plain counterparts, and equation 5 (absolute
// difference) by ADD.
struct BlendMapping {
    bool enable{};
    VkBlendOp op{VK_BLEND_OP_ADD};
    VkBlendFactor src{VK_BLEND_FACTOR_ONE};
    VkBlendFactor dst{VK_BLEND_FACTOR_ZERO};
    std::uint32_t constant{};       // destination fixed colour, 0xBBGGRR
    std::uint32_t source_scale{};   // fixed source colour applied in the shader
    bool scale_source{};
};

BlendMapping blend_mapping(const GeGpuDrawDescriptor &draw) noexcept {
    BlendMapping m{};
    if (!draw.blend_enabled || draw.clear_mode) return m;
    m.enable = true;
    const auto factor = [&](std::uint32_t value, bool source) {
        switch (value & 0xFu) {
        case 0u: return source ? VK_BLEND_FACTOR_DST_COLOR : VK_BLEND_FACTOR_SRC_COLOR;
        case 1u: return source ? VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR : VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case 2u: case 6u: return VK_BLEND_FACTOR_SRC_ALPHA;
        case 3u: case 7u: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case 4u: case 8u: return VK_BLEND_FACTOR_DST_ALPHA;
        case 5u: case 9u: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case 10u: {
            const std::uint32_t fixed = (source ? draw.blend_fix_source : draw.blend_fix_dest) & 0x00FFFFFFu;
            if (fixed == 0x00FFFFFFu) return VK_BLEND_FACTOR_ONE;
            if (fixed == 0u) return VK_BLEND_FACTOR_ZERO;
            if (source) {
                m.scale_source = true;
                m.source_scale = fixed;
                return VK_BLEND_FACTOR_ONE;
            }
            m.constant = fixed;
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        }
        default: return VK_BLEND_FACTOR_ONE;
        }
    };
    m.src = factor(draw.blend_source_factor, true);
    m.dst = factor(draw.blend_dest_factor, false);
    switch (draw.blend_equation & 7u) {
    case 1u: m.op = VK_BLEND_OP_SUBTRACT; break;
    case 2u: m.op = VK_BLEND_OP_REVERSE_SUBTRACT; break;
    case 3u: m.op = VK_BLEND_OP_MIN; break;
    case 4u: m.op = VK_BLEND_OP_MAX; break;
    default: m.op = VK_BLEND_OP_ADD; break;
    }
    // MIN/MAX ignore the factors in both APIs.
    if (m.op == VK_BLEND_OP_MIN || m.op == VK_BLEND_OP_MAX) m.scale_source = false;
    return m;
}

// PSP write-mask bytes: 0xFF masks the whole channel. Partial masks keep the
// channel writable, as in the DX12 backend.
VkColorComponentFlags color_write_mask(const GeGpuDrawDescriptor &draw) noexcept {
    VkColorComponentFlags mask = 0u;
    for (std::uint32_t channel = 0u; channel < 4u; ++channel) {
        if (((draw.color_write_mask >> (channel * 8u)) & 0xFFu) != 0xFFu)
            mask |= static_cast<VkColorComponentFlags>(1u << channel);
    }
    return mask;
}

std::uint64_t pipeline_key(const GeGpuDrawDescriptor &draw, bool strip, bool cull, bool accept_ccw) noexcept {
    const BlendMapping blend = blend_mapping(draw);
    std::uint64_t key = draw.depth_test_enabled ? 1u : 0u;
    key |= static_cast<std::uint64_t>(draw.depth_write_enabled ? 1u : 0u) << 1u;
    key |= static_cast<std::uint64_t>(draw.depth_function & 7u) << 2u;
    key |= static_cast<std::uint64_t>(blend.enable ? 1u : 0u) << 5u;
    key |= static_cast<std::uint64_t>(blend.op) << 6u;
    key |= static_cast<std::uint64_t>(blend.src) << 12u;
    key |= static_cast<std::uint64_t>(blend.dst) << 18u;
    key |= static_cast<std::uint64_t>(color_write_mask(draw)) << 24u;
    key |= static_cast<std::uint64_t>(strip ? 1u : 0u) << 28u;
    key |= static_cast<std::uint64_t>(cull ? 1u : 0u) << 29u;
    key |= static_cast<std::uint64_t>(accept_ccw ? 1u : 0u) << 30u;
    return key;
}

std::uint32_t packed_texture_control(const GeGpuDrawDescriptor &draw, bool enabled) noexcept {
    return (draw.texture_function & 0xFFu) |
           (static_cast<std::uint32_t>(draw.texture_use_alpha ? 1u : 0u) << 8u) |
           (static_cast<std::uint32_t>(draw.texture_double_color ? 1u : 0u) << 16u) |
           (static_cast<std::uint32_t>(enabled ? 1u : 0u) << 24u);
}

std::uint32_t packed_alpha_control(const GeGpuDrawDescriptor &draw) noexcept {
    return static_cast<std::uint32_t>(draw.alpha_test_enabled ? 1u : 0u) |
           ((draw.alpha_function & 7u) << 8u) |
           ((draw.alpha_reference & 0xFFu) << 16u) |
           ((draw.alpha_mask & 0xFFu) << 24u);
}

PushConstants make_push_constants(const Batch &batch, std::uint32_t logical_width,
                                  std::uint32_t logical_height, bool sampled) noexcept {
    PushConstants c{};
    logical_width = std::max<std::uint32_t>(1u, logical_width);
    logical_height = std::max<std::uint32_t>(1u, logical_height);
    const GeGpuDrawDescriptor &draw = batch.draw;
    c.pixel = {packed_alpha_control(draw), packed_texture_control(draw, sampled),
               draw.texture_env & 0x00FFFFFFu,
               (draw.fog_color & 0x00FFFFFFu) | (static_cast<std::uint32_t>(draw.fog_enabled ? 0xFFu : 0u) << 24u)};
    const BlendMapping blend = blend_mapping(draw);
    c.pixel2 = {draw.framebuffer_format & 3u, blend.source_scale, blend.scale_source ? 1u : 0u, 0u};
    if (!batch.hardware_transform) {
        c.uv = {2.0f / static_cast<float>(logical_width), 2.0f / static_cast<float>(logical_height),
                1.0f / 65535.0f, 0.0f};
        c.control = {2u, 0u, 0u, 0u};
        return c;
    }
    const GeGpuHardwareTransform &hw = batch.transform;
    const auto row = [&](std::size_t r) {
        return std::array<float, 4>{hw.model_to_clip[r], hw.model_to_clip[4u + r],
                                    hw.model_to_clip[8u + r], hw.model_to_clip[12u + r]};
    };
    const auto add_scaled = [](const std::array<float, 4> &a, float sa, const std::array<float, 4> &b, float sb) {
        return std::array<float, 4>{a[0] * sa + b[0] * sb, a[1] * sa + b[1] * sb,
                                    a[2] * sa + b[2] * sb, a[3] * sa + b[3] * sb};
    };
    const float x_a = hw.viewport_scale_x * (2.0f / static_cast<float>(logical_width));
    const float x_b = (hw.viewport_center_x - hw.viewport_offset_x) * (2.0f / static_cast<float>(logical_width)) - 1.0f;
    const float y_a = hw.viewport_scale_y * (2.0f / static_cast<float>(logical_height));
    const float y_b = (hw.viewport_center_y - hw.viewport_offset_y) * (2.0f / static_cast<float>(logical_height)) - 1.0f;
    constexpr float inv_depth = 1.0f / 65535.0f;
    c.row0 = add_scaled(row(0u), x_a, row(3u), x_b);
    c.row1 = add_scaled(row(1u), y_a, row(3u), y_b);  // Vulkan clip space is +Y down, like the PSP screen
    c.row2 = add_scaled(row(2u), hw.viewport_scale_z * inv_depth, row(3u), hw.viewport_center_z * inv_depth);
    c.row3 = row(3u);
    c.view_z = hw.model_to_view_z;
    c.uv = {hw.uv_scale_u, hw.uv_scale_v, hw.uv_offset_u, hw.uv_offset_v};
    c.fog = {hw.fog_end, hw.fog_slope, 0.0f, 0.0f};
    c.control = {1u, hw.depth_clip_enabled ? 1u : 0u, hw.vertex_color_affine ? 1u : 0u, 0u};
    c.color_mul = hw.vertex_color_mul;
    c.color_add = hw.vertex_color_add;
    return c;
}

bool merge_compatible(const Batch &a, const Batch &b) noexcept {
    if (a.framebuffer_feedback || b.framebuffer_feedback || a.indexed || b.indexed) return false;
    if (a.first_vertex + a.vertex_count != b.first_vertex) return false;
    if ((a.draw.framebuffer_address & 0x001FFFF0u) != (b.draw.framebuffer_address & 0x001FFFF0u)) return false;
    if (a.hardware_transform || b.hardware_transform) return false;
    if (pipeline_key(a.draw, false, false, false) != pipeline_key(b.draw, false, false, false)) return false;
    if (a.draw.texture_enabled != b.draw.texture_enabled) return false;
    if (a.draw.texture_enabled && texture_key(a.draw) != texture_key(b.draw)) return false;
    if (a.draw.scissor_x0 != b.draw.scissor_x0 || a.draw.scissor_y0 != b.draw.scissor_y0 ||
        a.draw.scissor_x1 != b.draw.scissor_x1 || a.draw.scissor_y1 != b.draw.scissor_y1) return false;
    const BlendMapping blend_a = blend_mapping(a.draw);
    const BlendMapping blend_b = blend_mapping(b.draw);
    if (blend_a.constant != blend_b.constant || blend_a.source_scale != blend_b.source_scale ||
        blend_a.scale_source != blend_b.scale_source) return false;
    return packed_alpha_control(a.draw) == packed_alpha_control(b.draw) &&
           packed_texture_control(a.draw, true) == packed_texture_control(b.draw, true) &&
           (a.draw.texture_env & 0x00FFFFFFu) == (b.draw.texture_env & 0x00FFFFFFu) &&
           a.draw.fog_enabled == b.draw.fog_enabled &&
           (a.draw.fog_color & 0x00FFFFFFu) == (b.draw.fog_color & 0x00FFFFFFu) &&
           (a.draw.framebuffer_format & 3u) == (b.draw.framebuffer_format & 3u);
}

void append_batch(VulkanState &s, Batch batch) {
    batch.draw_index = s.frame_draws;
    if (s.max_draw == 0u && !s.batches.empty() && merge_compatible(s.batches.back(), batch)) {
        s.batches.back().vertex_count += batch.vertex_count;
        return;
    }
    s.batches.push_back(batch);
}

// ----------------------------------------------------------------------------
// Vulkan plumbing.

void log_error(const std::string &where, const std::string &message) {
    std::cerr << "[vulkan] " << where << ": " << message << "\n";
}

bool check(VkResult result, const char *what, std::string &error) {
    if (result == VK_SUCCESS) return true;
    error = std::string(what) + " failed (VkResult " + std::to_string(static_cast<int>(result)) + ")";
    return false;
}

std::uint32_t find_memory_type(const VulkanState &s, std::uint32_t bits, VkMemoryPropertyFlags required,
                               VkMemoryPropertyFlags preferred = 0u) {
    for (const VkMemoryPropertyFlags wanted : {required | preferred, required}) {
        for (std::uint32_t i = 0u; i < s.memory_properties.memoryTypeCount; ++i) {
            if ((bits & (1u << i)) != 0u &&
                (s.memory_properties.memoryTypes[i].propertyFlags & wanted) == wanted)
                return i;
        }
    }
    return std::numeric_limits<std::uint32_t>::max();
}

bool create_buffer(VulkanState &s, VkDeviceSize size, VkBufferUsageFlags usage, bool readback,
                   Buffer &out, std::string &error) {
    VkBufferCreateInfo info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!check(vkCreateBuffer(s.device, &info, nullptr, &out.buffer), "vkCreateBuffer", error)) return false;
    VkMemoryRequirements requirements{};
    vkGetBufferMemoryRequirements(s.device, out.buffer, &requirements);
    const VkMemoryPropertyFlags required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    const VkMemoryPropertyFlags preferred = readback
        ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        : VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const std::uint32_t type = find_memory_type(s, requirements.memoryTypeBits, required, preferred);
    if (type == std::numeric_limits<std::uint32_t>::max()) { error = "no host-visible memory type"; return false; }
    VkMemoryAllocateInfo allocate{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate.allocationSize = requirements.size;
    allocate.memoryTypeIndex = type;
    if (!check(vkAllocateMemory(s.device, &allocate, nullptr, &out.memory), "vkAllocateMemory(buffer)", error))
        return false;
    vkBindBufferMemory(s.device, out.buffer, out.memory, 0u);
    void *mapped = nullptr;
    if (!check(vkMapMemory(s.device, out.memory, 0u, VK_WHOLE_SIZE, 0u, &mapped), "vkMapMemory", error)) return false;
    out.mapped = static_cast<std::byte *>(mapped);
    out.size = size;
    out.coherent = (s.memory_properties.memoryTypes[type].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0u;
    return true;
}

void destroy_buffer(VulkanState &s, Buffer &buffer) {
    if (buffer.buffer != VK_NULL_HANDLE) vkDestroyBuffer(s.device, buffer.buffer, nullptr);
    if (buffer.memory != VK_NULL_HANDLE) vkFreeMemory(s.device, buffer.memory, nullptr);
    buffer = Buffer{};
}

bool create_image(VulkanState &s, std::uint32_t width, std::uint32_t height, std::uint32_t mip_levels,
                  VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, Image &out,
                  std::string &error) {
    VkImageCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = {width, height, 1u};
    info.mipLevels = mip_levels;
    info.arrayLayers = 1u;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (!check(vkCreateImage(s.device, &info, nullptr, &out.image), "vkCreateImage", error)) return false;
    VkMemoryRequirements requirements{};
    vkGetImageMemoryRequirements(s.device, out.image, &requirements);
    const std::uint32_t type = find_memory_type(s, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (type == std::numeric_limits<std::uint32_t>::max()) { error = "no device-local memory type"; return false; }
    VkMemoryAllocateInfo allocate{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate.allocationSize = requirements.size;
    allocate.memoryTypeIndex = type;
    if (!check(vkAllocateMemory(s.device, &allocate, nullptr, &out.memory), "vkAllocateMemory(image)", error))
        return false;
    vkBindImageMemory(s.device, out.image, out.memory, 0u);
    VkImageViewCreateInfo view{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view.image = out.image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = format;
    view.subresourceRange = {aspect, 0u, mip_levels, 0u, 1u};
    if (!check(vkCreateImageView(s.device, &view, nullptr, &out.view), "vkCreateImageView", error)) return false;
    out.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    return true;
}

void destroy_image_now(VulkanState &s, Image &image) {
    if (image.view != VK_NULL_HANDLE) vkDestroyImageView(s.device, image.view, nullptr);
    if (image.image != VK_NULL_HANDLE) vkDestroyImage(s.device, image.image, nullptr);
    if (image.memory != VK_NULL_HANDLE) vkFreeMemory(s.device, image.memory, nullptr);
    image = Image{};
}

void forget_descriptor_sets_for(VulkanState &s, VkImageView view);

// Destroys `image` once no submitted frame can still reference it.
void destroy_image_deferred(VulkanState &s, Image &image) {
    if (image.image == VK_NULL_HANDLE) return;
    forget_descriptor_sets_for(s, image.view);
    Image doomed = image;
    image = Image{};
    s.deferred.emplace_back(s.submitted_serial + 1u, [&s, doomed]() mutable { destroy_image_now(s, doomed); });
}

void run_deferred(VulkanState &s) {
    std::size_t write = 0u;
    for (std::size_t i = 0u; i < s.deferred.size(); ++i) {
        if (s.deferred[i].first <= s.completed_serial) {
            s.deferred[i].second();
        } else {
            if (write != i) s.deferred[write] = std::move(s.deferred[i]);
            ++write;
        }
    }
    s.deferred.resize(write);
}

void barrier(VkCommandBuffer commands, Image &image, VkImageLayout layout, VkImageAspectFlags aspect,
             std::uint32_t mip_levels = VK_REMAINING_MIP_LEVELS) {
    if (image.layout == layout) return;
    VkImageMemoryBarrier2 b{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    b.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    b.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    b.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    b.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
    b.oldLayout = image.layout;
    b.newLayout = layout;
    b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image = image.image;
    b.subresourceRange = {aspect, 0u, mip_levels, 0u, 1u};
    VkDependencyInfo dependency{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    dependency.imageMemoryBarrierCount = 1u;
    dependency.pImageMemoryBarriers = &b;
    vkCmdPipelineBarrier2(commands, &dependency);
    image.layout = layout;
}

// One-shot command buffer for initialization work.
bool run_immediate(VulkanState &s, const std::function<void(VkCommandBuffer)> &record, std::string &error) {
    VkCommandBufferAllocateInfo allocate{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocate.commandPool = s.command_pool;
    allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate.commandBufferCount = 1u;
    VkCommandBuffer commands{};
    if (!check(vkAllocateCommandBuffers(s.device, &allocate, &commands), "vkAllocateCommandBuffers", error))
        return false;
    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commands, &begin);
    record(commands);
    vkEndCommandBuffer(commands);
    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1u;
    submit.pCommandBuffers = &commands;
    const bool ok = check(vkQueueSubmit(s.queue, 1u, &submit, VK_NULL_HANDLE), "vkQueueSubmit", error) &&
                    check(vkQueueWaitIdle(s.queue), "vkQueueWaitIdle", error);
    vkFreeCommandBuffers(s.device, s.command_pool, 1u, &commands);
    return ok;
}

VkSampler sampler_for(VulkanState &s, const GeGpuDrawDescriptor &draw) {
    std::uint64_t key = hash_mix(0u, draw.texture_min_linear ? 1u : 0u);
    key = hash_mix(key, draw.texture_mag_linear ? 1u : 0u);
    key = hash_mix(key, draw.texture_mipmap_enabled && draw.texture_mipmap_linear ? 1u : 0u);
    key = hash_mix(key, draw.texture_clamp_u ? 1u : 0u);
    key = hash_mix(key, draw.texture_clamp_v ? 1u : 0u);
    key = hash_mix(key, draw.texture_level_mode);
    key = hash_mix(key, static_cast<std::uint32_t>(draw.texture_level_offset16));
    key = hash_mix(key, draw.texture_selected_level);
    key = hash_mix(key, draw.texture_max_level);
    if (const auto found = s.samplers.find(key); found != s.samplers.end()) return found->second;
    VkSamplerCreateInfo info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    info.magFilter = draw.texture_mag_linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.minFilter = draw.texture_min_linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.mipmapMode = draw.texture_mipmap_enabled && draw.texture_mipmap_linear
        ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.addressModeU = draw.texture_clamp_u ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeV = draw.texture_clamp_v ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.mipLodBias = static_cast<float>(draw.texture_level_offset16) / 16.0f;
    info.minLod = 0.0f;
    info.maxLod = draw.texture_mipmap_enabled ? static_cast<float>(draw.texture_max_level) : 0.0f;
    if (draw.texture_mipmap_enabled && draw.texture_level_mode == 1u) {
        info.minLod = info.maxLod = static_cast<float>(draw.texture_selected_level);
    }
    VkSampler sampler{};
    if (vkCreateSampler(s.device, &info, nullptr, &sampler) != VK_SUCCESS) return s.white_sampler;
    s.samplers.emplace(key, sampler);
    s.report.texture_samplers_created = s.samplers.size();
    return sampler;
}

std::uint64_t descriptor_key(VkImageView view, VkSampler sampler) noexcept {
    return hash_mix(reinterpret_cast<std::uintptr_t>(view), reinterpret_cast<std::uintptr_t>(sampler));
}

VkDescriptorSet descriptor_for(VulkanState &s, VkImageView view, VkSampler sampler) {
    const std::uint64_t key = descriptor_key(view, sampler);
    if (const auto found = s.descriptor_sets.find(key); found != s.descriptor_sets.end()) return found->second.set;
    VkDescriptorSet set{};
    for (int attempt = 0; attempt < 2 && set == VK_NULL_HANDLE; ++attempt) {
        if (attempt == 1 || s.descriptor_pools.empty()) {
            const VkDescriptorPoolSize size{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4096u};
            VkDescriptorPoolCreateInfo pool_info{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 4096u;
            pool_info.poolSizeCount = 1u;
            pool_info.pPoolSizes = &size;
            VkDescriptorPool pool{};
            if (vkCreateDescriptorPool(s.device, &pool_info, nullptr, &pool) != VK_SUCCESS) return VK_NULL_HANDLE;
            s.descriptor_pools.push_back(pool);
        }
        VkDescriptorSetAllocateInfo allocate{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocate.descriptorPool = s.descriptor_pools.back();
        allocate.descriptorSetCount = 1u;
        allocate.pSetLayouts = &s.set_layout;
        if (vkAllocateDescriptorSets(s.device, &allocate, &set) != VK_SUCCESS) set = VK_NULL_HANDLE;
    }
    if (set == VK_NULL_HANDLE) return VK_NULL_HANDLE;
    VkDescriptorImageInfo image{sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.dstSet = set;
    write.descriptorCount = 1u;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &image;
    vkUpdateDescriptorSets(s.device, 1u, &write, 0u, nullptr);
    s.descriptor_sets.emplace(key, VulkanState::DescriptorEntry{set, s.descriptor_pools.back()});
    s.report.texture_descriptor_sets_allocated = s.descriptor_sets.size();
    return set;
}

// Frees the sets referencing a view that is about to be destroyed, after the
// frames that may still bind them. Runs on eviction only.
void forget_descriptor_sets_for(VulkanState &s, VkImageView view) {
    const auto release = [&](VkSampler sampler) {
        const auto found = s.descriptor_sets.find(descriptor_key(view, sampler));
        if (found == s.descriptor_sets.end()) return;
        const VulkanState::DescriptorEntry entry = found->second;
        s.descriptor_sets.erase(found);
        s.deferred.emplace_back(s.submitted_serial + 1u, [&s, entry]() {
            vkFreeDescriptorSets(s.device, entry.pool, 1u, &entry.set);
        });
    };
    for (const auto &[sampler_key, sampler] : s.samplers) {
        (void)sampler_key;
        release(sampler);
    }
    release(s.white_sampler);
}

VkPipeline pipeline_for(VulkanState &s, const GeGpuDrawDescriptor &draw, bool strip, bool cull, bool accept_ccw) {
    const std::uint64_t key = pipeline_key(draw, strip, cull, accept_ccw);
    if (const auto found = s.pipelines.find(key); found != s.pipelines.end()) return found->second;

    const std::array<VkPipelineShaderStageCreateInfo, 2> stages{{
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0u, VK_SHADER_STAGE_VERTEX_BIT,
         s.vertex_shader, "main", nullptr},
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0u, VK_SHADER_STAGE_FRAGMENT_BIT,
         s.fragment_shader, "main", nullptr},
    }};
    const VkVertexInputBindingDescription binding{0u, sizeof(UploadVertex), VK_VERTEX_INPUT_RATE_VERTEX};
    const std::array<VkVertexInputAttributeDescription, 5> attributes{{
        {0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<std::uint32_t>(offsetof(UploadVertex, x))},
        {1u, 0u, VK_FORMAT_R8G8B8A8_UNORM, static_cast<std::uint32_t>(offsetof(UploadVertex, rgba))},
        {2u, 0u, VK_FORMAT_R32G32_SFLOAT, static_cast<std::uint32_t>(offsetof(UploadVertex, u))},
        {3u, 0u, VK_FORMAT_R32_SFLOAT, static_cast<std::uint32_t>(offsetof(UploadVertex, fog_factor))},
        {4u, 0u, VK_FORMAT_R32_SFLOAT, static_cast<std::uint32_t>(offsetof(UploadVertex, q))},
    }};
    VkPipelineVertexInputStateCreateInfo vertex_input{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input.vertexBindingDescriptionCount = 1u;
    vertex_input.pVertexBindingDescriptions = &binding;
    vertex_input.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributes.size());
    vertex_input.pVertexAttributeDescriptions = attributes.data();
    VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly.topology = strip ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineViewportStateCreateInfo viewport{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport.viewportCount = 1u;
    viewport.scissorCount = 1u;
    VkPipelineRasterizationStateCreateInfo raster{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
    // GE command 0x9B names the winding the PSP keeps as seen with +Y up;
    // Vulkan judges winding in its +Y-down framebuffer, so the sense flips
    // (verified against the software rasterizer's culling).
    raster.frontFace = accept_ccw ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster.lineWidth = 1.0f;
    VkPipelineMultisampleStateCreateInfo multisample{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineDepthStencilStateCreateInfo depth{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth.depthTestEnable = draw.depth_test_enabled || draw.depth_write_enabled ? VK_TRUE : VK_FALSE;
    depth.depthWriteEnable = draw.depth_write_enabled ? VK_TRUE : VK_FALSE;
    depth.depthCompareOp = draw.depth_test_enabled ? compare_op(draw.depth_function) : VK_COMPARE_OP_ALWAYS;
    const BlendMapping blend = blend_mapping(draw);
    VkPipelineColorBlendAttachmentState attachment{};
    attachment.blendEnable = blend.enable ? VK_TRUE : VK_FALSE;
    attachment.srcColorBlendFactor = blend.src;
    attachment.dstColorBlendFactor = blend.dst;
    attachment.colorBlendOp = blend.op;
    // The software rasterizer applies the same factors to alpha.
    const auto alpha_factor = [](VkBlendFactor factor) {
        switch (factor) {
        case VK_BLEND_FACTOR_SRC_COLOR: return VK_BLEND_FACTOR_SRC_ALPHA;
        case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case VK_BLEND_FACTOR_DST_COLOR: return VK_BLEND_FACTOR_DST_ALPHA;
        case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case VK_BLEND_FACTOR_CONSTANT_COLOR: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        default: return factor;
        }
    };
    attachment.srcAlphaBlendFactor = alpha_factor(blend.src);
    attachment.dstAlphaBlendFactor = alpha_factor(blend.dst);
    attachment.alphaBlendOp = blend.op;
    attachment.colorWriteMask = color_write_mask(draw);
    VkPipelineColorBlendStateCreateInfo color_blend{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend.attachmentCount = 1u;
    color_blend.pAttachments = &attachment;
    const std::array<VkDynamicState, 3> dynamic_states{
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_BLEND_CONSTANTS};
    VkPipelineDynamicStateCreateInfo dynamic{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
    dynamic.pDynamicStates = dynamic_states.data();
    const VkFormat color_format = kColorFormat;
    VkPipelineRenderingCreateInfo rendering{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    rendering.colorAttachmentCount = 1u;
    rendering.pColorAttachmentFormats = &color_format;
    rendering.depthAttachmentFormat = kDepthFormat;
    VkGraphicsPipelineCreateInfo info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    info.pNext = &rendering;
    info.stageCount = static_cast<std::uint32_t>(stages.size());
    info.pStages = stages.data();
    info.pVertexInputState = &vertex_input;
    info.pInputAssemblyState = &assembly;
    info.pViewportState = &viewport;
    info.pRasterizationState = &raster;
    info.pMultisampleState = &multisample;
    info.pDepthStencilState = &depth;
    info.pColorBlendState = &color_blend;
    info.pDynamicState = &dynamic;
    info.layout = s.pipeline_layout;
    VkPipeline pipeline{};
    if (vkCreateGraphicsPipelines(s.device, VK_NULL_HANDLE, 1u, &info, nullptr, &pipeline) != VK_SUCCESS) {
        log_error("pipeline", "vkCreateGraphicsPipelines failed");
        return VK_NULL_HANDLE;
    }
    s.pipelines.emplace(key, pipeline);
    s.report.unique_pipeline_keys = s.pipelines.size();
    s.report.graphics_pipeline_created = true;
    return pipeline;
}

Target *find_target(VulkanState &s, std::uint32_t address) noexcept {
    const auto found = s.targets.find(address & 0x001FFFF0u);
    return found == s.targets.end() ? nullptr : &found->second;
}

bool ensure_target(VulkanState &s, std::uint32_t address, std::string &error) {
    address &= 0x001FFFF0u;
    if (find_target(s, address) != nullptr) return true;
    if (s.targets.size() >= 256u) { error = "framebuffer target limit reached"; return false; }
    Target target{};
    target.address = address;
    target.logical_width = kReferenceWidth;
    target.logical_height = kReferenceHeight;
    if (!create_image(s, s.target_width, s.target_height, 1u, kColorFormat,
                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                      VK_IMAGE_ASPECT_COLOR_BIT, target.color, error) ||
        !create_image(s, s.target_width, s.target_height, 1u, kDepthFormat,
                      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                      VK_IMAGE_ASPECT_DEPTH_BIT, target.depth, error)) {
        destroy_image_now(s, target.color);
        destroy_image_now(s, target.depth);
        return false;
    }
    s.targets.emplace(address, std::move(target));
    s.report.framebuffer_targets_observed = s.targets.size();
    return true;
}

void note_logical_extent(VulkanState &s, std::uint32_t address, std::uint32_t width, std::uint32_t height) {
    Target *target = find_target(s, address);
    if (target == nullptr) return;
    if ((address & 0x001FFFF0u) == s.display_framebuffer) {
        target->logical_width = kReferenceWidth;
        target->logical_height = kReferenceHeight;
        return;
    }
    if (width != 0u) target->logical_width = std::max(target->logical_width, width);
    if (height != 0u) target->logical_height = std::max(target->logical_height, height);
}

Texture *find_texture(VulkanState &s, std::uint64_t key) noexcept {
    const auto found = s.textures.find(key);
    return found == s.textures.end() ? nullptr : &found->second;
}

void erase_texture(VulkanState &s, std::unordered_map<std::uint64_t, Texture>::iterator it) {
    destroy_image_deferred(s, it->second.image);
    s.texture_cache_bytes -= std::min<std::uint64_t>(s.texture_cache_bytes, it->second.bytes);
    s.textures.erase(it);
}

bool prepare_texture_upload(VulkanState &s, const GeGpuDrawDescriptor &draw, std::uint32_t width,
                            std::uint32_t height, std::uint32_t mip_levels, std::vector<std::byte> rgba) {
    if (!s.enabled || !draw.texture_enabled || width == 0u || height == 0u || mip_levels == 0u || mip_levels > 8u)
        return false;
    std::size_t expected = 0u;
    for (std::uint32_t level = 0u, w = width, h = height; level < mip_levels; ++level) {
        expected += static_cast<std::size_t>(w) * h * 4u;
        w = std::max(1u, w >> 1u);
        h = std::max(1u, h >> 1u);
    }
    if (rgba.size() != expected) return false;
    const std::uint64_t key = texture_key(draw);
    const std::uint64_t checksum = fnv1a64(rgba);
    if (auto found = s.textures.find(key); found != s.textures.end()) {
        found->second.signature_epoch = s.frame_epoch;
        found->second.last_used_epoch = s.frame_epoch;
        if (found->second.checksum == checksum && found->second.width == width &&
            found->second.height == height && found->second.mip_levels == mip_levels) {
            found->second.descriptor = draw;
            ++s.report.texture_cache_hits;
            return true;
        }
        erase_texture(s, found);
    }
    while (!s.textures.empty() && (s.textures.size() >= s.texture_cache_entry_limit ||
                                   s.texture_cache_bytes + expected > s.texture_cache_byte_limit)) {
        auto victim = s.textures.end();
        for (auto it = s.textures.begin(); it != s.textures.end(); ++it) {
            if (it->second.last_used_epoch == s.frame_epoch) continue;
            if (victim == s.textures.end() || it->second.last_used_epoch < victim->second.last_used_epoch)
                victim = it;
        }
        if (victim == s.textures.end()) {
            ++s.report.rejected_texture_decodes;
            return false;
        }
        erase_texture(s, victim);
        ++s.report.evicted_textures;
    }
    Texture texture{};
    texture.descriptor = draw;
    texture.width = width;
    texture.height = height;
    texture.mip_levels = mip_levels;
    texture.checksum = checksum;
    texture.signature_epoch = texture.last_used_epoch = s.frame_epoch;
    texture.bytes = expected;
    std::string error;
    if (!create_image(s, width, height, mip_levels, kColorFormat,
                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                      texture.image, error)) {
        log_error("texture", error);
        destroy_image_now(s, texture.image);
        return false;
    }
    texture.pending_rgba = std::move(rgba);
    s.texture_cache_bytes += expected;
    s.pending_textures.push_back(key);
    s.textures.emplace(key, std::move(texture));
    ++s.report.decoded_texture_uploads;
    s.report.decoded_texture_bytes += expected;
    s.report.texture_images_created = s.textures.size();
    s.report.uploaded_mip_levels += mip_levels;
    return true;
}

void record_texture_uploads(VulkanState &s, FrameSlot &frame) {
    std::size_t cursor = 0u;
    for (const std::uint64_t key : s.pending_textures) {
        Texture *texture = find_texture(s, key);
        if (texture == nullptr || texture->pending_rgba.empty()) continue;
        const std::size_t bytes = texture->pending_rgba.size();
        Buffer *source = &frame.staging;
        std::size_t offset = (cursor + 15u) & ~std::size_t{15u};
        Buffer oversized{};
        if (offset + bytes > frame.staging.size) {
            std::string error;
            if (!create_buffer(s, bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, false, oversized, error)) {
                log_error("texture staging", error);
                continue;
            }
            source = &oversized;
            offset = 0u;
        } else {
            cursor = offset + bytes;
        }
        std::memcpy(source->mapped + offset, texture->pending_rgba.data(), bytes);
        barrier(frame.commands, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        std::array<VkBufferImageCopy, 8> regions{};
        std::size_t level_offset = offset;
        for (std::uint32_t level = 0u, w = texture->width, h = texture->height; level < texture->mip_levels; ++level) {
            regions[level].bufferOffset = level_offset;
            regions[level].imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, level, 0u, 1u};
            regions[level].imageExtent = {w, h, 1u};
            level_offset += static_cast<std::size_t>(w) * h * 4u;
            w = std::max(1u, w >> 1u);
            h = std::max(1u, h >> 1u);
        }
        vkCmdCopyBufferToImage(frame.commands, source->buffer, texture->image.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->mip_levels, regions.data());
        barrier(frame.commands, texture->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        texture->pending_rgba.clear();
        texture->pending_rgba.shrink_to_fit();
        s.report.texture_image_upload_bytes += bytes;
        if (oversized.buffer != VK_NULL_HANDLE) {
            s.deferred.emplace_back(s.submitted_serial + 1u, [&s, oversized]() mutable { destroy_buffer(s, oversized); });
        }
    }
    s.pending_textures.clear();
}

bool wait_for_slot(VulkanState &s, FrameSlot &frame) {
    if (frame.serial == 0u) return true;
    if (vkWaitForFences(s.device, 1u, &frame.fence, VK_TRUE, 5'000'000'000ull) != VK_SUCCESS) {
        log_error("frame", "fence wait timed out");
        return false;
    }
    s.completed_serial = std::max(s.completed_serial, frame.serial);
    return true;
}

void copy_readback(VulkanState &s, FrameSlot &frame) {
    if (!frame.readback_valid) return;
    frame.readback_valid = false;
    if (frame.serial <= s.frame_serial) return;
    if (!frame.readback.coherent) {
        VkMappedMemoryRange range{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
        range.memory = frame.readback.memory;
        range.size = VK_WHOLE_SIZE;
        vkInvalidateMappedMemoryRanges(s.device, 1u, &range);
    }
    const std::size_t bytes = static_cast<std::size_t>(s.target_width) * s.target_height * 4u;
    s.frame_rgba.resize(bytes);
    std::memcpy(s.frame_rgba.data(), frame.readback.mapped, bytes);
    s.frame_vblank = frame.readback_vblank;
    s.frame_serial = frame.serial;
}

void clear_accumulation(VulkanState &s) {
    s.frame_draws = 0u;
    s.vertices.clear();
    s.indices.clear();
    s.batches.clear();
}

void destroy_all(VulkanState &s) {
    if (s.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(s.device);
        s.completed_serial = std::numeric_limits<std::uint64_t>::max();
        run_deferred(s);
        for (auto &[key, texture] : s.textures) destroy_image_now(s, texture.image);
        for (auto &[address, target] : s.targets) {
            destroy_image_now(s, target.color);
            destroy_image_now(s, target.depth);
            destroy_image_now(s, target.snapshot);
        }
        destroy_image_now(s, s.white);
        for (auto &frame : s.frames) {
            destroy_buffer(s, frame.geometry);
            destroy_buffer(s, frame.staging);
            destroy_buffer(s, frame.readback);
            if (frame.fence != VK_NULL_HANDLE) vkDestroyFence(s.device, frame.fence, nullptr);
        }
        for (auto &[key, pipeline] : s.pipelines) vkDestroyPipeline(s.device, pipeline, nullptr);
        for (auto &[key, sampler] : s.samplers) vkDestroySampler(s.device, sampler, nullptr);
        if (s.white_sampler != VK_NULL_HANDLE) vkDestroySampler(s.device, s.white_sampler, nullptr);
        for (VkDescriptorPool pool : s.descriptor_pools) vkDestroyDescriptorPool(s.device, pool, nullptr);
        if (s.vertex_shader != VK_NULL_HANDLE) vkDestroyShaderModule(s.device, s.vertex_shader, nullptr);
        if (s.fragment_shader != VK_NULL_HANDLE) vkDestroyShaderModule(s.device, s.fragment_shader, nullptr);
        if (s.pipeline_layout != VK_NULL_HANDLE) vkDestroyPipelineLayout(s.device, s.pipeline_layout, nullptr);
        if (s.set_layout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(s.device, s.set_layout, nullptr);
        if (s.command_pool != VK_NULL_HANDLE) vkDestroyCommandPool(s.device, s.command_pool, nullptr);
        vkDestroyDevice(s.device, nullptr);
    }
    if (s.instance != VK_NULL_HANDLE) vkDestroyInstance(s.instance, nullptr);
    s = VulkanState{};
}

bool create_device(VulkanState &s, const vulkan::Config &config, std::string &error) {
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.pApplicationName = "PSPRecomp";
    app.pEngineName = "PSPRecomp GE";
    app.apiVersion = VK_API_VERSION_1_3;
    std::vector<const char *> layers;
    if (config.validation) {
        std::uint32_t count = 0u;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> available(count);
        vkEnumerateInstanceLayerProperties(&count, available.data());
        for (const auto &layer : available) {
            if (std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)
                layers.push_back("VK_LAYER_KHRONOS_validation");
        }
        if (layers.empty()) std::cerr << "[vulkan] validation requested but VK_LAYER_KHRONOS_validation is missing\n";
    }
    VkInstanceCreateInfo instance_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_info.pApplicationInfo = &app;
    instance_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();
    if (!check(vkCreateInstance(&instance_info, nullptr, &s.instance), "vkCreateInstance", error)) return false;
    s.report.instance_created = true;

    std::uint32_t count = 0u;
    vkEnumeratePhysicalDevices(s.instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(s.instance, &count, devices.data());
    s.report.physical_device_count = count;
    int best_score = -1;
    const char *forced = std::getenv("PSPRECOMP_VK_DEVICE");
    for (std::uint32_t index = 0u; index < count; ++index) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(devices[index], &properties);
        if (properties.apiVersion < VK_API_VERSION_1_3) continue;
        if (properties.limits.maxPushConstantsSize < sizeof(PushConstants)) continue;
        std::uint32_t family_count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[index], &family_count, nullptr);
        std::vector<VkQueueFamilyProperties> families(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[index], &family_count, families.data());
        std::uint32_t family = std::numeric_limits<std::uint32_t>::max();
        for (std::uint32_t f = 0u; f < family_count; ++f) {
            if ((families[f].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) { family = f; break; }
        }
        if (family == std::numeric_limits<std::uint32_t>::max()) continue;
        int score = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 3
                  : properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? 2 : 1;
        if (forced != nullptr && std::atoi(forced) == static_cast<int>(index)) score = 100;
        if (score > best_score) {
            best_score = score;
            s.physical = devices[index];
            s.queue_family = family;
            s.device_name = properties.deviceName;
        }
    }
    if (s.physical == VK_NULL_HANDLE) { error = "no Vulkan 1.3 device with a graphics queue"; return false; }
    vkGetPhysicalDeviceMemoryProperties(s.physical, &s.memory_properties);

    const float priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue_info.queueFamilyIndex = s.queue_family;
    queue_info.queueCount = 1u;
    queue_info.pQueuePriorities = &priority;
    VkPhysicalDeviceVulkan13Features features13{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;
    VkDeviceCreateInfo device_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_info.pNext = &features13;
    device_info.queueCreateInfoCount = 1u;
    device_info.pQueueCreateInfos = &queue_info;
    if (!check(vkCreateDevice(s.physical, &device_info, nullptr, &s.device), "vkCreateDevice", error)) return false;
    s.report.device_created = true;
    vkGetDeviceQueue(s.device, s.queue_family, 0u, &s.queue);
    s.report.graphics_queue_family = s.queue_family;
    return true;
}

bool create_resources(VulkanState &s, std::string &error) {
    VkCommandPoolCreateInfo pool_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = s.queue_family;
    if (!check(vkCreateCommandPool(s.device, &pool_info, nullptr, &s.command_pool), "vkCreateCommandPool", error))
        return false;
    s.report.command_pool_created = true;

    const VkDescriptorSetLayoutBinding binding{0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1u,
                                               VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};
    VkDescriptorSetLayoutCreateInfo set_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    set_info.bindingCount = 1u;
    set_info.pBindings = &binding;
    if (!check(vkCreateDescriptorSetLayout(s.device, &set_info, nullptr, &s.set_layout),
               "vkCreateDescriptorSetLayout", error)) return false;
    const VkPushConstantRange push{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0u,
                                   sizeof(PushConstants)};
    VkPipelineLayoutCreateInfo layout_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    layout_info.setLayoutCount = 1u;
    layout_info.pSetLayouts = &s.set_layout;
    layout_info.pushConstantRangeCount = 1u;
    layout_info.pPushConstantRanges = &push;
    if (!check(vkCreatePipelineLayout(s.device, &layout_info, nullptr, &s.pipeline_layout),
               "vkCreatePipelineLayout", error)) return false;
    s.report.texture_descriptor_layout_created = true;

    const auto make_module = [&](const std::uint32_t *code, std::size_t bytes, VkShaderModule &module) {
        VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        info.codeSize = bytes;
        info.pCode = code;
        return check(vkCreateShaderModule(s.device, &info, nullptr, &module), "vkCreateShaderModule", error);
    };
    if (!make_module(kGeVertexSpirv, sizeof(kGeVertexSpirv), s.vertex_shader) ||
        !make_module(kGeFragmentSpirv, sizeof(kGeFragmentSpirv), s.fragment_shader)) return false;
    s.report.shader_modules_created = true;

    for (FrameSlot &frame : s.frames) {
        VkCommandBufferAllocateInfo allocate{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocate.commandPool = s.command_pool;
        allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate.commandBufferCount = 1u;
        if (!check(vkAllocateCommandBuffers(s.device, &allocate, &frame.commands), "vkAllocateCommandBuffers", error))
            return false;
        VkFenceCreateInfo fence_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (!check(vkCreateFence(s.device, &fence_info, nullptr, &frame.fence), "vkCreateFence", error)) return false;
        if (!create_buffer(s, kGeometryCapacity, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                           false, frame.geometry, error) ||
            !create_buffer(s, kTextureStagingCapacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, false, frame.staging, error) ||
            !create_buffer(s, static_cast<VkDeviceSize>(s.target_width) * s.target_height * 4u,
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT, true, frame.readback, error))
            return false;
    }
    s.report.transfer_buffer_created = true;
    s.report.transfer_memory_mapped = true;
    s.report.upload_capacity_bytes = kGeometryCapacity;
    s.report.frames_in_flight_capacity = kFrameCount;

    // 1x1 white texture bound for untextured draws, so every pipeline sees a
    // valid descriptor.
    if (!create_image(s, 1u, 1u, 1u, kColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                      VK_IMAGE_ASPECT_COLOR_BIT, s.white, error)) return false;
    VkSamplerCreateInfo sampler_info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler_info.magFilter = sampler_info.minFilter = VK_FILTER_NEAREST;
    sampler_info.addressModeU = sampler_info.addressModeV = sampler_info.addressModeW =
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    if (!check(vkCreateSampler(s.device, &sampler_info, nullptr, &s.white_sampler), "vkCreateSampler", error))
        return false;
    std::memset(s.frames[0].staging.mapped, 0xFF, 4u);
    return run_immediate(s, [&](VkCommandBuffer commands) {
        barrier(commands, s.white, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        VkBufferImageCopy region{};
        region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u};
        region.imageExtent = {1u, 1u, 1u};
        vkCmdCopyBufferToImage(commands, s.frames[0].staging.buffer, s.white.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1u, &region);
        barrier(commands, s.white, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    }, error);
}

// Brings a target that has never been rendered into a defined state (black,
// depth 0) so a draw may sample it before anything was drawn into it.
void initialize_target(VkCommandBuffer commands, Target &target) {
    if (target.color.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier(commands, target.color, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        const VkClearColorValue black{{0.0f, 0.0f, 0.0f, 1.0f}};
        const VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u};
        vkCmdClearColorImage(commands, target.color.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &black, 1u, &range);
        barrier(commands, target.color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    if (target.depth.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier(commands, target.depth, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);
        const VkClearDepthStencilValue zero{0.0f, 0u};
        const VkImageSubresourceRange range{VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u};
        vkCmdClearDepthStencilImage(commands, target.depth.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &zero, 1u, &range);
        barrier(commands, target.depth, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
}

} // namespace

// ----------------------------------------------------------------------------
// Profile-facing control API.

namespace vulkan {

bool initialize(const Config &config, std::string &error) {
    VulkanState &s = state();
    if (s.enabled) return true;
    s.scale = std::clamp<std::uint32_t>(config.scale, 1u, 8u);
    s.target_width = kReferenceWidth * s.scale;
    s.target_height = kReferenceHeight * s.scale;
    if (const char *max_draw = std::getenv("PSPRECOMP_VK_MAX_DRAW")) {
        unsigned long long frame = 0u, count = 0u;
        if (std::sscanf(max_draw, "%llu,%llu", &frame, &count) == 2) {
            s.max_draw_vblank = frame;
            s.max_draw = static_cast<std::uint32_t>(count);
        }
    }
    if (const char *mb = std::getenv("PSPRECOMP_VK_TEXTURE_CACHE_MB"))
        s.texture_cache_byte_limit = std::max<std::uint64_t>(64u, std::strtoull(mb, nullptr, 10)) * 1024ull * 1024ull;
    if (!create_device(s, config, error) || !create_resources(s, error)) {
        destroy_all(s);
        return false;
    }
    s.enabled = true;
    s.report.requested = GeGpuBackendKind::Software;
    s.report.offscreen_width = s.target_width;
    s.report.offscreen_height = s.target_height;
    return true;
}

void shutdown() { destroy_all(state()); }

bool active() { return state().enabled; }

std::string device_name() { return state().device_name; }

const GeGpuBackendReport &report() { return state().report; }

void set_display_framebuffer(std::uint32_t address) {
    VulkanState &s = state();
    s.display_framebuffer = address & 0x001FFFF0u;
    if (!s.enabled) return;
    std::string error;
    if (!ensure_target(s, s.display_framebuffer, error)) log_error("display target", error);
    note_logical_extent(s, s.display_framebuffer, kReferenceWidth, kReferenceHeight);
}

bool finish_frame(std::uint64_t vblank) {
    VulkanState &s = state();
    if (!s.enabled) return false;
    if (s.batches.empty() && s.pending_textures.empty()) {
        ++s.frame_epoch;
        return false;
    }

    FrameSlot &frame = s.frames[s.frame_cursor];
    if (!wait_for_slot(s, frame)) {
        clear_accumulation(s);
        return false;
    }
    copy_readback(s, frame);
    run_deferred(s);

    const std::size_t vertex_bytes = s.vertices.size() * sizeof(UploadVertex);
    const std::size_t index_offset = (vertex_bytes + 3u) & ~std::size_t{3u};
    const std::size_t index_bytes = s.indices.size() * sizeof(std::uint32_t);
    if (index_offset + index_bytes > frame.geometry.size) {
        ++s.report.game_vertex_overflows;
        clear_accumulation(s);
        ++s.frame_epoch;
        return false;
    }
    if (vertex_bytes != 0u) std::memcpy(frame.geometry.mapped, s.vertices.data(), vertex_bytes);
    if (index_bytes != 0u) std::memcpy(frame.geometry.mapped + index_offset, s.indices.data(), index_bytes);

    vkResetFences(s.device, 1u, &frame.fence);
    vkResetCommandBuffer(frame.commands, 0u);
    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(frame.commands, &begin);
    VkCommandBuffer cmd = frame.commands;

    record_texture_uploads(s, frame);
    for (auto &[address, target] : s.targets) initialize_target(cmd, target);

    const VkDeviceSize zero_offset = 0u;
    vkCmdBindVertexBuffers(cmd, 0u, 1u, &frame.geometry.buffer, &zero_offset);
    if (index_bytes != 0u) vkCmdBindIndexBuffer(cmd, frame.geometry.buffer, index_offset, VK_INDEX_TYPE_UINT32);
    const VkViewport viewport{0.0f, 0.0f, static_cast<float>(s.target_width), static_cast<float>(s.target_height),
                              0.0f, 1.0f};

    Target *current = nullptr;
    bool rendering = false;
    bool touched_display = false;
    VkPipeline bound_pipeline = VK_NULL_HANDLE;
    VkDescriptorSet bound_set = VK_NULL_HANDLE;
    std::uint32_t bound_constant = 0xFFFFFFFFu;

    const auto begin_rendering = [&](Target &target, bool clear_color, bool clear_depth) {
        barrier(cmd, target.color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        color.imageView = target.color.view;
        color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color.loadOp = clear_color ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        VkRenderingAttachmentInfo depth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depth.imageView = target.depth.view;
        depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth.loadOp = clear_depth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth.clearValue.depthStencil = {0.0f, 0u};
        VkRenderingInfo info{VK_STRUCTURE_TYPE_RENDERING_INFO};
        info.renderArea = {{0, 0}, {s.target_width, s.target_height}};
        info.layerCount = 1u;
        info.colorAttachmentCount = 1u;
        info.pColorAttachments = &color;
        info.pDepthAttachment = &depth;
        vkCmdBeginRendering(cmd, &info);
        vkCmdSetViewport(cmd, 0u, 1u, &viewport);
        rendering = true;
        bound_pipeline = VK_NULL_HANDLE;
        bound_set = VK_NULL_HANDLE;
        bound_constant = 0xFFFFFFFFu;
    };
    const auto end_rendering = [&]() {
        if (!rendering) return;
        vkCmdEndRendering(cmd);
        rendering = false;
        barrier(cmd, current->color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    };

    std::string error;
    static const std::uint32_t only_texture = [] {
        const char *text = std::getenv("PSPRECOMP_VK_ONLY_TEXTURE");
        return text != nullptr ? static_cast<std::uint32_t>(std::strtoul(text, nullptr, 0)) : 0u;
    }();
    for (const Batch &batch : s.batches) {
        if (only_texture != 0u && !batch.draw.clear_mode && batch.draw.texture_address != only_texture) continue;
        if (s.max_draw != 0u && vblank == s.max_draw_vblank && batch.draw_index > s.max_draw) continue;
        const std::uint32_t address = batch.draw.framebuffer_address & 0x001FFFF0u;
        Target *target = current != nullptr && current->address == address ? current : find_target(s, address);
        if (target == nullptr) continue;
        if (target != current || !rendering) {
            end_rendering();
            target->last_render_epoch = s.frame_epoch;
            current = target;
            begin_rendering(*target, false, false);
        }
        if (address == s.display_framebuffer) touched_display = true;

        // Texture source: another target's GPU image, a snapshot of this one,
        // or a decoded texture.
        VkImageView view = s.white.view;
        VkSampler sampler = s.white_sampler;
        bool sampled = false;
        std::array<float, 2> sample_scale{1.0f, 1.0f};
        if (batch.draw.texture_enabled) {
            Target *feedback = batch.framebuffer_feedback ? find_target(s, batch.feedback_address) : nullptr;
            if (feedback != nullptr) {
                sampler = sampler_for(s, batch.draw);
                if (batch.draw.texture_width != 0u && feedback->logical_width != 0u)
                    sample_scale[0] = static_cast<float>(batch.draw.texture_width) /
                                      static_cast<float>(feedback->logical_width);
                if (batch.draw.texture_height != 0u && feedback->logical_height != 0u)
                    sample_scale[1] = static_cast<float>(batch.draw.texture_height) /
                                      static_cast<float>(feedback->logical_height);
                if (feedback == current) {
                    if (feedback->snapshot.image == VK_NULL_HANDLE &&
                        !create_image(s, s.target_width, s.target_height, 1u, kColorFormat,
                                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                      VK_IMAGE_ASPECT_COLOR_BIT, feedback->snapshot, error)) {
                        log_error("feedback snapshot", error);
                    } else {
                        vkCmdEndRendering(cmd);
                        rendering = false;
                        barrier(cmd, feedback->color, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
                        barrier(cmd, feedback->snapshot, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
                        VkImageCopy copy{};
                        copy.srcSubresource = copy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u};
                        copy.extent = {s.target_width, s.target_height, 1u};
                        vkCmdCopyImage(cmd, feedback->color.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       feedback->snapshot.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1u, &copy);
                        barrier(cmd, feedback->snapshot, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VK_IMAGE_ASPECT_COLOR_BIT);
                        begin_rendering(*feedback, false, false);
                        view = feedback->snapshot.view;
                        sampled = true;
                        ++s.report.vram_feedback_refreshes;
                    }
                } else {
                    view = feedback->color.view;
                    sampled = true;
                    ++s.report.vram_feedback_refreshes;
                }
            } else if (Texture *texture = find_texture(s, texture_key(batch.draw));
                       texture != nullptr && texture->image.layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                view = texture->image.view;
                sampler = sampler_for(s, batch.draw);
                sampled = true;
            }
        }

        const bool strip = batch.hardware_transform && batch.transform.primitive == 4u;
        const bool cull = batch.hardware_transform && batch.transform.cull_enabled;
        const bool ccw = cull && batch.transform.accept_counter_clockwise;
        VkPipeline pipeline = pipeline_for(s, batch.draw, strip, cull, ccw);
        if (pipeline == VK_NULL_HANDLE) continue;
        if (pipeline != bound_pipeline) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            bound_pipeline = pipeline;
        }
        VkDescriptorSet set = descriptor_for(s, view, sampler);
        if (set == VK_NULL_HANDLE) continue;
        if (set != bound_set) {
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, s.pipeline_layout, 0u, 1u, &set, 0u, nullptr);
            bound_set = set;
        }
        const BlendMapping blend = blend_mapping(batch.draw);
        if (blend.constant != bound_constant) {
            const float constants[4]{static_cast<float>(blend.constant & 0xFFu) / 255.0f,
                                     static_cast<float>((blend.constant >> 8u) & 0xFFu) / 255.0f,
                                     static_cast<float>((blend.constant >> 16u) & 0xFFu) / 255.0f, 1.0f};
            vkCmdSetBlendConstants(cmd, constants);
            bound_constant = blend.constant;
        }
        const std::uint32_t logical_width = std::max<std::uint32_t>(1u, current->logical_width);
        const std::uint32_t logical_height = std::max<std::uint32_t>(1u, current->logical_height);
        PushConstants push = make_push_constants(batch, logical_width, logical_height, sampled);
        push.sample = {sample_scale[0], sample_scale[1], 0.0f, 0.0f};
        vkCmdPushConstants(cmd, s.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0u,
                           sizeof(push), &push);
        const auto scale = [](std::int32_t value, std::uint32_t physical, std::uint32_t logical) {
            return static_cast<std::int32_t>(std::clamp<std::int64_t>(
                static_cast<std::int64_t>(value) * physical / logical, 0, physical));
        };
        const std::int32_t x0 = scale(batch.draw.scissor_x0, s.target_width, logical_width);
        const std::int32_t y0 = scale(batch.draw.scissor_y0, s.target_height, logical_height);
        const std::int32_t x1 = scale(batch.draw.scissor_x1 + 1, s.target_width, logical_width);
        const std::int32_t y1 = scale(batch.draw.scissor_y1 + 1, s.target_height, logical_height);
        if (x1 <= x0 || y1 <= y0) continue;
        const VkRect2D scissor{{x0, y0}, {static_cast<std::uint32_t>(x1 - x0), static_cast<std::uint32_t>(y1 - y0)}};
        vkCmdSetScissor(cmd, 0u, 1u, &scissor);
        if (batch.indexed)
            vkCmdDrawIndexed(cmd, batch.index_count, 1u, batch.first_index, static_cast<std::int32_t>(batch.first_vertex), 0u);
        else
            vkCmdDraw(cmd, batch.vertex_count, 1u, batch.first_vertex, 0u);
        ++s.report.game_draw_calls;
        if (sampled) ++s.report.textured_game_draw_calls;
        if (batch.draw.depth_test_enabled) ++s.report.depth_tested_game_draw_calls;
    }
    end_rendering();

    // PSPRECOMP_VK_READBACK_TARGET=addr: present another render target instead
    // (diagnostics for render-to-texture passes).
    static const std::uint32_t readback_target = [] {
        const char *text = std::getenv("PSPRECOMP_VK_READBACK_TARGET");
        return text != nullptr ? static_cast<std::uint32_t>(std::strtoul(text, nullptr, 0)) & 0x001FFFF0u : 0u;
    }();
    Target *display = find_target(s, readback_target != 0u ? readback_target : s.display_framebuffer);
    const bool display_ready = (touched_display || readback_target != 0u) && display != nullptr;
    if (display_ready) {
        barrier(cmd, display->color, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
        VkBufferImageCopy region{};
        region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u};
        region.imageExtent = {s.target_width, s.target_height, 1u};
        vkCmdCopyImageToBuffer(cmd, display->color.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               frame.readback.buffer, 1u, &region);
        barrier(cmd, display->color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    } else {
        ++s.report.frames_without_displayed_target;
    }
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1u;
    submit.pCommandBuffers = &cmd;
    if (vkQueueSubmit(s.queue, 1u, &submit, frame.fence) != VK_SUCCESS) {
        log_error("frame", "vkQueueSubmit failed; disabling the Vulkan backend");
        s.enabled = false;
        clear_accumulation(s);
        return false;
    }
    frame.serial = ++s.submitted_serial;
    frame.readback_valid = display_ready;
    frame.readback_vblank = vblank;
    s.frame_cursor = (s.frame_cursor + 1u) % kFrameCount;
    ++s.report.game_frames;
    s.report.game_frame_vblank = vblank;
    s.report.presented_framebuffer_target = display_ready ? s.display_framebuffer : 0u;
    clear_accumulation(s);
    ++s.frame_epoch;
    return display_ready;
}

Frame latest_frame(bool wait) {
    VulkanState &s = state();
    if (!s.enabled) return {};
    // Newest submission first; with `wait` block on it, otherwise take
    // whichever slot has already finished.
    for (std::uint32_t back = 1u; back <= kFrameCount; ++back) {
        FrameSlot &frame = s.frames[(s.frame_cursor + kFrameCount - back) % kFrameCount];
        if (!frame.readback_valid) continue;
        if (wait) {
            if (!wait_for_slot(s, frame)) break;
        } else if (vkGetFenceStatus(s.device, frame.fence) != VK_SUCCESS) {
            continue;
        }
        s.completed_serial = std::max(s.completed_serial, frame.serial);
        copy_readback(s, frame);
        break;
    }
    if (s.frame_rgba.empty()) return {};
    return Frame{s.frame_rgba, s.target_width, s.target_height, s.frame_vblank};
}

} // namespace vulkan

// ----------------------------------------------------------------------------
// GE renderer entry points.

namespace gpu {

bool ge_gpu_backend_active() noexcept { return state().enabled; }
bool ge_gpu_backend_graphics_ready() noexcept { return state().enabled; }
bool ge_gpu_backend_transfer_ready() noexcept { return state().enabled; }
bool ge_gpu_backend_presents_directly() noexcept { return false; }
std::uint32_t ge_gpu_backend_display_framebuffer() noexcept { return state().display_framebuffer; }
std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept { return 0u; }
bool ge_gpu_backend_adopt_shared_texture(const GeGpuDrawDescriptor &) noexcept { return false; }
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &, float, float) noexcept {}
void ge_gpu_backend_observe_camera(const std::array<float, 12> &, const std::array<float, 16> &,
                                   const std::array<float, 6> &, const std::array<float, 3> &,
                                   const GeGpuDrawDescriptor &, std::uint32_t) noexcept {}
GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(const GeGpuDrawDescriptor &) noexcept { return {}; }

bool ge_gpu_backend_stage_vertices(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex>) noexcept {
    return state().enabled;
}

void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &draw) noexcept {
    VulkanState &s = state();
    if (!s.enabled) return;
    ++s.report.draw_calls;
    ++s.frame_draws;
    s.report.vertices += draw.vertex_count;
    const std::uint32_t target = draw.framebuffer_address & 0x001FFFF0u;
    if (draw.framebuffer_stride != 0u || target == s.display_framebuffer) {
        if (target != s.last_registered_target || find_target(s, target) == nullptr) {
            s.last_registered_target = target;
            std::string error;
            if (!ensure_target(s, target, error)) log_error("framebuffer target", error);
        }
        const bool display = target == s.display_framebuffer;
        note_logical_extent(s, target,
                            display ? kReferenceWidth : std::max<std::uint32_t>(1u, draw.framebuffer_stride),
                            display ? kReferenceHeight : static_cast<std::uint32_t>(std::max(1, draw.scissor_y1 + 1)));
    }
    // A target sampled as a texture takes its logical extent from the texture
    // state: the framebuffer stride is the allocation pitch, not the width.
    if (draw.texture_enabled) {
        const std::uint32_t feedback = draw.texture_address & 0x001FFFF0u;
        if (Target *feedback_target = find_target(s, feedback); feedback_target != nullptr) {
            if (feedback == s.display_framebuffer) {
                feedback_target->logical_width = kReferenceWidth;
                feedback_target->logical_height = kReferenceHeight;
            } else {
                const std::uint32_t width = draw.texture_width != 0u ? draw.texture_width : draw.texture_buffer_width;
                if (width != 0u) feedback_target->logical_width = width;
                if (draw.texture_height != 0u) feedback_target->logical_height = draw.texture_height;
            }
        }
    }
}

bool ge_gpu_backend_texture_needed(const GeGpuDrawDescriptor &draw) noexcept {
    VulkanState &s = state();
    if (!s.enabled || !draw.texture_enabled || draw.texture_format > 10u ||
        draw.texture_width == 0u || draw.texture_height == 0u) return false;
    if (find_target(s, draw.texture_address) != nullptr) return false;  // GPU render target: no CPU decode
    ++s.report.texture_decode_requests;
    Texture *found = find_texture(s, texture_key(draw));
    if (found == nullptr) return true;
    found->signature_epoch = s.frame_epoch;
    found->last_used_epoch = s.frame_epoch;
    if (draw.texture_content_signature != 0u &&
        found->descriptor.texture_content_signature != draw.texture_content_signature) return true;
    ++s.report.texture_cache_hits;
    return false;
}

void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &draw) noexcept {
    draw.texture_cache_key_hint = 0u;
    draw.texture_image_key_hint = 0u;
    if (!draw.texture_enabled) return;
    const std::uint64_t key = texture_key(draw);
    draw.texture_cache_key_hint = key;
    draw.texture_image_key_hint = key;
}

bool ge_gpu_backend_texture_signature_needed(const GeGpuDrawDescriptor &draw) noexcept {
    VulkanState &s = state();
    if (!s.enabled || !draw.texture_enabled || draw.texture_width == 0u || draw.texture_height == 0u) return false;
    if (find_target(s, draw.texture_address) != nullptr) return false;
    Texture *found = find_texture(s, texture_key(draw));
    return found == nullptr || found->signature_epoch != s.frame_epoch;
}

bool ge_gpu_backend_is_framebuffer_feedback_texture(const GeGpuDrawDescriptor &draw) noexcept {
    VulkanState &s = state();
    return s.enabled && draw.texture_enabled && find_target(s, draw.texture_address) != nullptr;
}

bool ge_gpu_backend_texture_available(const GeGpuDrawDescriptor &draw) noexcept {
    VulkanState &s = state();
    if (!s.enabled || !draw.texture_enabled) return false;
    if (find_target(s, draw.texture_address) != nullptr) return true;
    Texture *found = find_texture(s, texture_key(draw));
    if (found == nullptr) return false;
    found->last_used_epoch = s.frame_epoch;
    return true;
}

bool ge_gpu_backend_upload_decoded_texture(const GeGpuDrawDescriptor &draw, std::uint32_t width,
                                           std::uint32_t height, std::span<const std::byte> rgba8) noexcept {
    if (rgba8.empty()) return false;
    try {
        return prepare_texture_upload(state(), draw, width, height, 1u,
                                      std::vector<std::byte>(rgba8.begin(), rgba8.end()));
    } catch (...) {
        return false;
    }
}

bool ge_gpu_backend_upload_decoded_texture_chain_packed(const GeGpuDrawDescriptor &draw, std::uint32_t base_width,
                                                        std::uint32_t base_height, std::uint32_t mip_levels,
                                                        std::vector<std::byte> rgba8) noexcept {
    try {
        return prepare_texture_upload(state(), draw, base_width, base_height, mip_levels, std::move(rgba8));
    } catch (...) {
        return false;
    }
}

void ge_gpu_backend_accumulate_color_triangles(const GeGpuDrawDescriptor &draw,
                                               std::span<const GeGpuVertex> triangle_vertices) noexcept {
    VulkanState &s = state();
    if (!s.enabled || triangle_vertices.empty() || triangle_vertices.size() % 3u != 0u) return;
    if ((s.vertices.size() + triangle_vertices.size()) * sizeof(UploadVertex) +
            s.indices.size() * sizeof(std::uint32_t) + 16u > kGeometryCapacity) {
        ++s.report.game_vertex_overflows;
        return;
    }
    try {
        const bool feedback = draw.texture_enabled && find_target(s, draw.texture_address) != nullptr;
        const bool sampled = draw.texture_enabled && ge_gpu_backend_texture_available(draw);
        // Screen-space UVs arrive in texels; the shader samples normalized.
        const float inv_u = sampled && draw.texture_width != 0u ? 1.0f / static_cast<float>(draw.texture_width) : 1.0f;
        const float inv_v = sampled && draw.texture_height != 0u ? 1.0f / static_cast<float>(draw.texture_height) : 1.0f;
        const std::uint32_t first = static_cast<std::uint32_t>(s.vertices.size());
        for (const GeGpuVertex &source : triangle_vertices) {
            s.vertices.push_back({source.x, source.y, source.z, source.w, source.rgba, source.u * inv_u,
                                  source.v * inv_v, source.fog_factor, source.q});
        }
        Batch batch{};
        batch.draw = draw;
        batch.first_vertex = first;
        batch.vertex_count = static_cast<std::uint32_t>(triangle_vertices.size());
        batch.framebuffer_feedback = feedback;
        batch.feedback_address = draw.texture_address & 0x001FFFF0u;
        append_batch(s, batch);
        s.report.game_triangles += triangle_vertices.size() / 3u;
        s.report.game_vertices += triangle_vertices.size();
    } catch (...) {
        ++s.report.game_vertex_overflows;
    }
}

void ge_gpu_backend_accumulate_hardware_triangles(const GeGpuDrawDescriptor &draw,
                                                  const GeGpuHardwareTransform &transform,
                                                  std::span<const GeGpuVertex> vertices,
                                                  std::span<const std::uint32_t> triangle_indices) noexcept {
    VulkanState &s = state();
    if (!s.enabled || vertices.empty()) return;
    const bool indexed = !triangle_indices.empty();
    const std::size_t emitted = indexed ? triangle_indices.size() : vertices.size();
    if (emitted == 0u || (transform.primitive == 4u ? emitted < 3u : emitted % 3u != 0u)) return;
    if ((s.vertices.size() + vertices.size()) * sizeof(UploadVertex) +
            (s.indices.size() + triangle_indices.size()) * sizeof(std::uint32_t) + 16u > kGeometryCapacity) {
        ++s.report.game_vertex_overflows;
        return;
    }
    try {
        const std::uint32_t first_vertex = static_cast<std::uint32_t>(s.vertices.size());
        const std::uint32_t first_index = static_cast<std::uint32_t>(s.indices.size());
        for (const GeGpuVertex &source : vertices) {
            s.vertices.push_back({source.x, source.y, source.z, source.w, source.rgba, source.u, source.v,
                                  source.fog_factor, source.q});
        }
        for (const std::uint32_t index : triangle_indices) {
            if (index >= vertices.size()) {
                s.vertices.resize(first_vertex);
                s.indices.resize(first_index);
                ++s.report.game_vertex_overflows;
                return;
            }
            s.indices.push_back(index);
        }
        Batch batch{};
        batch.draw = draw;
        batch.first_vertex = first_vertex;
        batch.vertex_count = static_cast<std::uint32_t>(vertices.size());
        batch.first_index = first_index;
        batch.index_count = static_cast<std::uint32_t>(triangle_indices.size());
        batch.indexed = indexed;
        batch.framebuffer_feedback = draw.texture_enabled && find_target(s, draw.texture_address) != nullptr;
        batch.feedback_address = draw.texture_address & 0x001FFFF0u;
        batch.hardware_transform = true;
        batch.transform = transform;
        append_batch(s, batch);
        s.report.hw_transform_draw_calls += std::max<std::uint32_t>(1u, transform.logical_prim_batches);
        s.report.hw_transform_vertices += vertices.size();
    } catch (...) {
        ++s.report.game_vertex_overflows;
    }
}

bool ge_gpu_backend_accumulate_hardware_packed_0115(const GeGpuDrawDescriptor &, const GeGpuHardwareTransform &,
                                                    std::span<const std::byte>, std::uint32_t,
                                                    std::span<const std::uint32_t>) noexcept {
    return false;  // the renderer falls back to decoded vertices
}

} // namespace gpu
} // namespace psprecomp::hle
