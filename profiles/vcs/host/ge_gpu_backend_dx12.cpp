#include "ge_gpu_backend.hpp"
#include "ge_cloud_camera_math.hpp"
#include "propershaders/ProperShadersBridge.hpp"
#include "vcs_config.hpp"
#include "vcs_draw_distance_patch.hpp"
#include "vcs_runtime_log.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <span>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#endif

namespace vcs {

#if defined(_WIN32)
namespace {

using Microsoft::WRL::ComPtr;
constexpr std::uint32_t kReferenceWidth = 480u;
constexpr std::uint32_t kReferenceHeight = 272u;
constexpr std::size_t kGeometryUploadCapacity = 64u * 1024u * 1024u;
// V4 ExecuteIndirect arguments live in their own persistently mapped upload
// arena. Keeping them separate means a draw-heavy frame can never steal bytes
// from the established 64 MiB geometry budget. The shadow-aware command is
// 260/264 bytes, so 16 MiB covers more than 60k commands before falling back
// to the ordinary path.
constexpr std::size_t kIndirectUploadCapacity = 16u * 1024u * 1024u;
// Stage 45.2: persistent per-frame texture upload arena. The 44.7 path created,
// mapped and destroyed one committed upload resource for every decoded texture.
// Streaming bursts therefore paid kernel/D3D12 allocation overhead on the hot GE
// thread even though the bytes live only until the frame fence completes. Keep a
// mapped arena per frame slot instead; fence-slot reuse makes it safe without any
// extra synchronization. Oversized bursts retain a committed-resource fallback.
constexpr std::size_t kTextureUploadCapacity = 32u * 1024u * 1024u;
constexpr DXGI_FORMAT kColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT kDepthFormat = DXGI_FORMAT_D32_FLOAT;
constexpr DXGI_FORMAT kCloudFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
constexpr UINT kFrameCount = 2u;
constexpr UINT kSrvCapacity = 65536u;
constexpr UINT kSamplerCapacity = 128u;
constexpr UINT kFramebufferTargetCapacity = 256u;
constexpr UINT kAmdVendorId = 0x1002u;
constexpr UINT kNvidiaVendorId = 0x10DEu;
constexpr std::size_t kPacked0115GuestStride = 10u;
constexpr UINT kPacked0115NativeStride = 10u;
constexpr UINT kPacked0115AmdUmaStride = 12u;

struct Dx12Batch {
    GeGpuDrawDescriptor draw{};
    std::uint32_t first_vertex{};
    std::uint32_t vertex_count{};
    std::uint32_t first_index{};
    std::uint32_t index_count{};
    bool indexed{};
    // Dominant VCS 0x000115 stream kept in its original 10-byte PSP encoding.
    // A separate IA layout/VS decodes UV/5551/s16 directly on the GPU.
    bool packed_0115{};
    std::uint32_t logical_draw_count{1u};
    bool framebuffer_feedback{};
    std::uint32_t feedback_address{};
    // Stage 44.7: keep model-space vertices model-space. The 44.6 backend
    // multiplied every hardware-transform vertex on the CPU before upload,
    // throwing away the main reason for having a native GPU frontend.
    bool hardware_transform{};
    GeGpuHardwareTransform transform{};
};

struct Dx12TransformConstants {
    std::array<float, 4> row0{};
    std::array<float, 4> row1{};
    std::array<float, 4> row2{};
    std::array<float, 4> row3{};
    std::array<float, 4> view_z{};
    std::array<float, 4> uv{1.0f, 1.0f, 0.0f, 0.0f};
    std::array<float, 4> fog{};
    std::array<std::uint32_t, 4> control{};
    // Stage 45.5: affine per-draw vertex lighting for the packed 0x0115 path.
    // This replaces tens of thousands of CPU lighting evaluations with eight
    // root constants and one VS MAD.
    std::array<float, 4> color_mul{1.0f, 1.0f, 1.0f, 1.0f};
    std::array<float, 4> color_add{};
};
static_assert(sizeof(Dx12TransformConstants) == 40u * sizeof(std::uint32_t));

// Stage 45.4: the native DX12 upload stream now carries only attributes that
// actually vary per vertex.  Alpha/texture/fog/framebuffer state is constant
// for a PSP draw and is sent once as root constants.  The old stream was 56
// bytes/vertex; this one is 36 bytes (-35.7% CPU memcpy + upload traffic).
struct Dx12UploadVertex {
    float x{};
    float y{};
    float z{};
    float w{1.0f};
    std::uint32_t rgba{0xFFFFFFFFu};
    float u{};
    float v{};
    float fog_factor{1.0f};
    float q{1.0f};
};
static_assert(sizeof(Dx12UploadVertex) == 36u);

struct Dx12PixelConstants {
    std::uint32_t alpha_control{};
    std::uint32_t texture_control{};
    std::uint32_t texture_env{};
    std::uint32_t fog_control{};
    std::uint32_t framebuffer_format{};
};
static_assert(sizeof(Dx12PixelConstants) == 5u * sizeof(std::uint32_t));

// Tier-2 V4 / 150-FPS path. ExecuteIndirect moves the per-draw root constant
// writes and Draw* call out of the CPU command-recording loop. The private
// material-shadow matrix is included in the command as well, so enabling the
// path does not require disabling realtime shadows. Runs still preserve guest
// order; only adjacent draws with identical fixed GPU state participate.
struct Dx12IndirectDrawCommand {
    std::uint32_t transform[40]{};
    std::uint32_t pixel_and_shadow[21]{};
    D3D12_DRAW_ARGUMENTS draw{};
};
struct Dx12IndirectDrawIndexedCommand {
    std::uint32_t transform[40]{};
    std::uint32_t pixel_and_shadow[21]{};
    D3D12_DRAW_INDEXED_ARGUMENTS draw{};
};
static_assert(sizeof(Dx12IndirectDrawCommand) == 260u);
static_assert(sizeof(Dx12IndirectDrawIndexedCommand) == 264u);

struct CloudCameraCandidate {
    std::array<float, 12> view{};
    std::array<float, 16> projection{};
    // scale X/Y/Z, center X/Y/Z and offset X/Y from the GE viewport. Keeping
    // this with the exact draw camera lets private full-screen effects rebuild
    // the same effective projection/depth range used by native geometry.
    std::array<float, 8> viewport{};
    std::array<float, 3> camera_position{};
    std::uint32_t target{};
    std::uint64_t weight{};
    std::uint64_t occluding_weight{};
};

struct CloudShaderConstants {
    std::array<float, 4> ray_right_time{};
    std::array<float, 4> ray_up_seed{};
    std::array<float, 4> ray_forward_opacity{};
    std::array<float, 4> camera_settings{};
    std::array<float, 4> coverage_speed{};
    std::array<float, 4> sun_direction_day{};
    std::array<float, 4> sun_color_atmosphere{};
    std::array<float, 4> cloud_color_mist{};
    std::array<float, 4> fog_color_start{};
    std::array<float, 4> brightness_padding{};
};
static_assert(sizeof(CloudShaderConstants) == 40u * sizeof(std::uint32_t));

struct CloudTemporalConstants {
    std::array<float, 4> previous_right_history{};
    std::array<float, 4> previous_up_blend{};
    std::array<float, 4> previous_forward_spatial{};
    std::array<float, 4> texel_subpixel{};
    std::array<float, 4> control{};
};
static_assert(sizeof(CloudTemporalConstants) == 20u * sizeof(std::uint32_t));

struct Dx12FrameResources {
    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12Resource> upload_buffer;
    std::byte *mapped_upload{};
    ComPtr<ID3D12Resource> indirect_upload_buffer;
    std::byte *mapped_indirect_upload{};
    ComPtr<ID3D12Resource> texture_upload_buffer;
    std::byte *mapped_texture_upload{};
    std::size_t texture_upload_cursor{};
    UINT64 fence_value{};
    std::vector<ComPtr<ID3D12Resource>> transient_resources;
};

struct Dx12Texture {
    GeGpuDrawDescriptor descriptor{};
    ComPtr<ID3D12Resource> image;
    ComPtr<ID3D12Resource> pending_upload;
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t mip_levels{1u};
    std::uint32_t srv_index{};
    std::uint32_t sampler_index{};
    std::uint64_t checksum{};
    std::uint64_t signature_epoch{};
    std::uint64_t last_used_epoch{};
    std::vector<std::byte> rgba8;
};

struct Dx12FramebufferTarget {
    std::uint32_t address{};
    // Logical PSP coordinate/texture extent represented by this target. The
    // Stage 44.6 backend treated every target as 480x272 even though VCS uses
    // larger offscreen surfaces (the world surface is sampled back through a
    // larger texture). That cropped the offscreen image before composition and
    // appeared as a large zoom in the first physical DX12 run.
    std::uint32_t logical_width{};
    std::uint32_t logical_height{};
    // `color` is always a single-sample shader-readable image. With MSAA=1 it
    // is also the render target. With MSAA>1, `msaa_color` receives rasterized
    // samples and is resolved into `color` before feedback/presentation.
    ComPtr<ID3D12Resource> color;
    ComPtr<ID3D12Resource> msaa_color;
    ComPtr<ID3D12Resource> depth;
    ComPtr<ID3D12Resource> feedback_copy;
    std::uint32_t rtv_index{};
    std::uint32_t dsv_index{};
    std::uint32_t srv_index{};
    std::uint32_t feedback_srv_index{};
    // Optional shader-readable view of the native world depth buffer. Two
    // contiguous descriptors are reserved because the shared fullscreen root
    // signature exposes t0..t1.  Only the private realtime-shadow module uses
    // them; public builds keep this at zero and pay no descriptor/resource
    // state cost beyond the normal DSV.
    std::uint32_t depth_srv_base{};
    D3D12_RESOURCE_STATES color_state{D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE};
    D3D12_RESOURCE_STATES msaa_state{D3D12_RESOURCE_STATE_RENDER_TARGET};
    D3D12_RESOURCE_STATES depth_state{D3D12_RESOURCE_STATE_DEPTH_WRITE};
    D3D12_RESOURCE_STATES feedback_state{D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE};
    std::uint64_t last_render_epoch{};
};

struct Dx12RetiredSrv {
    std::uint32_t index{};
    UINT64 fence_value{};
};

struct CloudRenderTarget {
    ComPtr<ID3D12Resource> image;
    std::uint32_t rtv_index{};
    std::uint32_t srv_index{};
    std::uint32_t width{};
    std::uint32_t height{};
    D3D12_RESOURCE_STATES state{D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE};
};

struct ShadowMapTarget {
    ComPtr<ID3D12Resource> depth;
    std::uint32_t dsv_index{};
    std::uint32_t srv_index{};
    std::uint32_t resolution{};
    std::uint32_t caster_draws{};
    D3D12_RESOURCE_STATES state{D3D12_RESOURCE_STATE_DEPTH_WRITE};
};

struct Dx12GeState {
    GeGpuBackendReport report{};
    bool enabled{};
    std::uint32_t display_framebuffer{};
    std::uint32_t target_width{480u};
    std::uint32_t target_height{272u};
    UINT sample_count{1u};
    UINT sample_quality{};
    UINT adapter_vendor_id{};
    UINT adapter_device_id{};
    std::uint64_t adapter_dedicated_video_memory{};
    std::uint64_t adapter_shared_system_memory{};
    bool adapter_uma{};
    bool adapter_cache_coherent_uma{};
    bool amd_uma_safe_mode{};
    UINT packed_0115_gpu_stride{kPacked0115NativeStride};
    DXGI_FORMAT depth_format{kDepthFormat};
    std::uint32_t depth_bits{32u};
    std::vector<Dx12UploadVertex> vertices;
    std::vector<std::byte> packed_0115_vertices;
    std::vector<std::uint32_t> indices;
    std::vector<Dx12Batch> batches;
    std::vector<CloudCameraCandidate> cloud_cameras;
    std::vector<std::byte> frame_rgba;
    std::vector<std::byte> last_texture_rgba;

    ComPtr<IDXGIFactory6> factory;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> queue;
    std::array<Dx12FrameResources, kFrameCount> frames;
    UINT frame_cursor{};
    ComPtr<ID3D12GraphicsCommandList> list;
    ComPtr<ID3D12CommandAllocator> texture_allocator;
    ComPtr<ID3D12GraphicsCommandList> texture_list;
    ComPtr<ID3D12DescriptorHeap> rtv_heap;
    ComPtr<ID3D12DescriptorHeap> dsv_heap;
    ComPtr<ID3D12DescriptorHeap> srv_heap;
    ComPtr<ID3D12DescriptorHeap> sampler_heap;
    UINT rtv_stride{};
    UINT dsv_stride{};
    UINT srv_stride{};
    UINT sampler_stride{};
    UINT next_rtv{};
    UINT next_dsv{};
    UINT next_srv{1u};
    UINT next_sampler{1u};
    std::unordered_map<std::uint32_t, Dx12FramebufferTarget> frame_targets;
    ComPtr<ID3D12Resource> readback_buffer;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT readback_footprint{};
    UINT readback_rows{};
    UINT64 readback_row_size{};
    UINT64 readback_bytes{};
    ComPtr<ID3D12Fence> fence;
    HANDLE fence_event{};
    UINT64 next_fence{1u};
    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12RootSignature> cloud_root_signature;
    ComPtr<ID3D12RootSignature> realtime_shadow_caster_root_signature;
    ComPtr<ID3D12RootSignature> realtime_shadow_composite_root_signature;
    ComPtr<ID3D12CommandSignature> indirect_draw_signature;
    ComPtr<ID3D12CommandSignature> indirect_draw_indexed_signature;
    ComPtr<ID3DBlob> vertex_shader;
    ComPtr<ID3DBlob> packed_0115_vertex_shader;
    ComPtr<ID3DBlob> pixel_shader;
    ComPtr<ID3DBlob> building_vertex_shader;
    ComPtr<ID3DBlob> building_packed_0115_vertex_shader;
    ComPtr<ID3DBlob> building_pixel_shader;
    ComPtr<ID3DBlob> skin_vertex_shader;
    ComPtr<ID3DBlob> skin_packed_0115_vertex_shader;
    ComPtr<ID3DBlob> skin_pixel_shader;
    ComPtr<ID3DBlob> vehicle_vertex_shader;
    ComPtr<ID3DBlob> vehicle_packed_0115_vertex_shader;
    ComPtr<ID3DBlob> vehicle_pixel_shader;
    ComPtr<ID3DBlob> realtime_shadow_caster_vertex_shader;
    ComPtr<ID3DBlob> realtime_shadow_caster_packed_vertex_shader;
    ComPtr<ID3DBlob> realtime_shadow_caster_alpha_pixel_shader;
    ComPtr<ID3DBlob> realtime_shadow_pixel_shader;
    std::unordered_map<std::uint64_t, ComPtr<ID3D12PipelineState>> pipelines;
    std::unordered_map<std::uint64_t, Dx12Texture> textures;
    // Hot draw streams reuse the same texture for long runs. Avoid repeating an
    // unordered_map probe in texture_available() and again while recording the
    // same batch sequence. unordered_map node addresses survive rehash; the
    // cache is explicitly cleared before erasing any texture node.
    std::uint64_t last_texture_lookup_key{};
    Dx12Texture *last_texture_lookup{};
    std::vector<std::uint64_t> pending_texture_keys;
    std::vector<std::uint32_t> free_texture_srvs;
    std::vector<Dx12RetiredSrv> retired_texture_srvs;
    std::unordered_map<std::uint64_t, std::uint32_t> sampler_cache;
    std::unordered_set<std::uint32_t> known_frame_targets;
    std::uint32_t last_registered_framebuffer_target{0xFFFFFFFFu};
    std::uint64_t texture_cache_bytes{};
    std::uint64_t frame_epoch{1u};

    ComPtr<IDXGISwapChain3> swapchain;
    ComPtr<ID3D12DescriptorHeap> swap_rtv_heap;
    std::array<ComPtr<ID3D12Resource>, kFrameCount> backbuffers;
    UINT swap_rtv_stride{};
    std::uint32_t swap_width{};
    std::uint32_t swap_height{};
    ComPtr<ID3D12PipelineState> present_pipeline;
    ComPtr<ID3D12PipelineState> cloud_target_pipeline;
    ComPtr<ID3D12PipelineState> cloud_resolve_pipeline;
    ComPtr<ID3D12PipelineState> cloud_composite_pipeline;
    ComPtr<ID3D12PipelineState> realtime_shadow_caster_pipeline;
    ComPtr<ID3D12PipelineState> realtime_shadow_caster_packed_pipeline;
    // Alpha-tested variants, used only by draws with GE alpha test enabled.
    ComPtr<ID3D12PipelineState> realtime_shadow_caster_alpha_pipeline;
    ComPtr<ID3D12PipelineState> realtime_shadow_caster_packed_alpha_pipeline;
    ComPtr<ID3D12PipelineState> realtime_shadow_pipeline;
    ComPtr<ID3DBlob> present_vertex_shader;
    ComPtr<ID3DBlob> present_pixel_shader;
    ComPtr<ID3DBlob> cloud_target_pixel_shader;
    ComPtr<ID3DBlob> cloud_resolve_pixel_shader;
    ComPtr<ID3DBlob> cloud_composite_pixel_shader;
    std::array<CloudRenderTarget, 2> cloud_history;
    CloudRenderTarget cloud_march;
    ShadowMapTarget realtime_shadow_map;
    // Optional private effects must never decide whether the native GE backend
    // itself lives. This latch becomes true only after shadow HLSL compiled.
    bool realtime_shadows_available{};
    std::array<std::uint32_t, 2> cloud_resolve_srv_base{};
    std::uint32_t cloud_history_index{};
    std::uint32_t cloud_temporal_frame{};
    std::array<float, 3> cloud_previous_camera{};
    std::array<float, 9> cloud_previous_ray_basis{};
    bool cloud_history_valid{};
    bool direct_present_ok{};
    std::uint32_t presented_framebuffer{};
    std::uint32_t missed_display_intervals{};
    bool swapchain_tearing{};
    HWND native_window{};
    bool readback_enabled{};
    bool texture_upload_ring_enabled{true};
    std::string adapter_name;
};

Dx12GeState &state() {
    static Dx12GeState s;
    return s;
}

std::uint64_t hash_mix(std::uint64_t hash, std::uint64_t value) noexcept {
    hash ^= value + 0x9E3779B97F4A7C15ull + (hash << 6u) + (hash >> 2u);
    return hash;
}

bool explicit_host_decoded_texture(const GeGpuDrawDescriptor &draw) noexcept {
    // Savedata UI host-decoded RGBA textures (font atlas + ICON0.PNG) use
    // texture_address=0 because they do not live in PSP VRAM. DX12 also keeps
    // framebuffer target 0 alive for the display, so treating every address-0
    // texture as framebuffer feedback aliases these host textures to the last
    // gameplay frame. The savedata uploader deliberately stamps the same
    // nonzero host identity into all three host-only key/signature fields;
    // normal PSP textures never use that triple-equality contract.
    return draw.texture_enabled && draw.texture_address == 0u &&
           draw.texture_cache_key_hint != 0u &&
           draw.texture_cache_key_hint == draw.texture_image_key_hint &&
           draw.texture_cache_key_hint == draw.texture_content_signature;
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

// Forward declaration used by batch/indirect compatibility checks. The sampler
// implementation lives next to sampler creation below.
std::uint64_t sampler_key(const GeGpuDrawDescriptor &draw) noexcept;

D3D12_CPU_DESCRIPTOR_HANDLE rtv_cpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE h = s.rtv_heap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<SIZE_T>(index) * s.rtv_stride;
    return h;
}

D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE h = s.dsv_heap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<SIZE_T>(index) * s.dsv_stride;
    return h;
}

D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE h = s.srv_heap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<SIZE_T>(index) * s.srv_stride;
    return h;
}

D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_GPU_DESCRIPTOR_HANDLE h = s.srv_heap->GetGPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<UINT64>(index) * s.srv_stride;
    return h;
}

D3D12_CPU_DESCRIPTOR_HANDLE sampler_cpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE h = s.sampler_heap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<SIZE_T>(index) * s.sampler_stride;
    return h;
}

D3D12_GPU_DESCRIPTOR_HANDLE sampler_gpu(Dx12GeState &s, UINT index) noexcept {
    D3D12_GPU_DESCRIPTOR_HANDLE h = s.sampler_heap->GetGPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<UINT64>(index) * s.sampler_stride;
    return h;
}

void reap_retired_texture_srvs(Dx12GeState &s) noexcept {
    if (!s.fence || s.retired_texture_srvs.empty()) return;
    const UINT64 completed = s.fence->GetCompletedValue();
    std::size_t write = 0u;
    for (std::size_t i = 0u; i < s.retired_texture_srvs.size(); ++i) {
        const Dx12RetiredSrv retired = s.retired_texture_srvs[i];
        if (retired.fence_value == 0u || completed >= retired.fence_value) {
            s.free_texture_srvs.push_back(retired.index);
        } else {
            if (write != i) s.retired_texture_srvs[write] = retired;
            ++write;
        }
    }
    s.retired_texture_srvs.resize(write);
}

std::uint32_t allocate_texture_srv(Dx12GeState &s) noexcept {
    reap_retired_texture_srvs(s);
    if (!s.free_texture_srvs.empty()) {
        const std::uint32_t index = s.free_texture_srvs.back();
        s.free_texture_srvs.pop_back();
        ++s.report.recycled_texture_descriptor_sets;
        return index;
    }
    if (s.next_srv >= kSrvCapacity) return 0u;
    return s.next_srv++;
}

void retire_texture_srv(Dx12GeState &s, std::uint32_t index) noexcept {
    if (index == 0u) return;
    UINT64 retire_after = 0u;
    for (const Dx12FrameResources &frame : s.frames)
        retire_after = std::max(retire_after, frame.fence_value);
    if (!s.fence || retire_after == 0u || s.fence->GetCompletedValue() >= retire_after)
        s.free_texture_srvs.push_back(index);
    else
        s.retired_texture_srvs.push_back({index, retire_after});
}

D3D12_CPU_DESCRIPTOR_HANDLE swap_rtv(Dx12GeState &s, UINT index) noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE h = s.swap_rtv_heap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += static_cast<SIZE_T>(index) * s.swap_rtv_stride;
    return h;
}

std::uint64_t fnv1a64(std::span<const std::byte> bytes) noexcept {
    std::uint64_t hash = 1469598103934665603ull;
    for (const std::byte b : bytes) {
        hash ^= static_cast<std::uint8_t>(b);
        hash *= 1099511628211ull;
    }
    return hash;
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

Dx12PixelConstants make_pixel_constants(const GeGpuDrawDescriptor &draw,
                                        bool sampled_texture) noexcept {
    Dx12PixelConstants out{};
    out.alpha_control = packed_alpha_control(draw);
    out.texture_control = packed_texture_control(draw, sampled_texture);
    out.texture_env = draw.texture_env & 0x00FFFFFFu;
    out.fog_control = (draw.fog_color & 0x00FFFFFFu) |
        (static_cast<std::uint32_t>(draw.fog_enabled ? 0xFFu : 0u) << 24u);
    out.framebuffer_format = draw.framebuffer_format & 3u;
    return out;
}

Dx12UploadVertex make_upload_vertex(const GeGpuVertex &source) noexcept {
    return {source.x, source.y, source.z, source.w, source.rgba, source.u, source.v,
            source.fog_factor, source.q};
}

bool env_truthy(const char *name) noexcept {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return false;
    return std::strcmp(text, "0") != 0 &&
           std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
           std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
}

bool native_indexed_draw_enabled() noexcept {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_DX12_NATIVE_INDEXED_DRAW");
        // Default on: the production GE probe covers it on every build and the
        // launcher scripts had been enabling it by hand. =0 restores the old path.
        if (text == nullptr || *text == '\0') return true;
        return std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
    }();
    return enabled;
}

std::string hr_text(HRESULT hr, const char *where) {
    std::ostringstream out;
    out << where << " failed (HRESULT=0x" << std::hex << std::uppercase
        << static_cast<unsigned long>(hr) << ')';
    LPSTR message = nullptr;
    const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS;
    if (FormatMessageA(flags, nullptr, static_cast<DWORD>(hr),
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       reinterpret_cast<LPSTR>(&message), 0u, nullptr) != 0u && message != nullptr) {
        std::string text(message);
        LocalFree(message);
        while (!text.empty() && (text.back() == '\r' || text.back() == '\n')) text.pop_back();
        if (!text.empty()) out << ": " << text;
    }
    return out.str();
}

void transition(ID3D12GraphicsCommandList *list, ID3D12Resource *resource,
                D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) noexcept {
    if (before == after || list == nullptr || resource == nullptr) return;
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = resource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    list->ResourceBarrier(1u, &barrier);
}

void prepare_target_for_render(Dx12GeState &s, Dx12FramebufferTarget &target) noexcept {
    if (target.msaa_color) {
        transition(s.list.Get(), target.msaa_color.Get(), target.msaa_state,
                   D3D12_RESOURCE_STATE_RENDER_TARGET);
        target.msaa_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    } else {
        transition(s.list.Get(), target.color.Get(), target.color_state,
                   D3D12_RESOURCE_STATE_RENDER_TARGET);
        target.color_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
}

void resolve_target_for_sampling(Dx12GeState &s, Dx12FramebufferTarget &target,
                                 bool resume_render) noexcept {
    if (target.msaa_color) {
        transition(s.list.Get(), target.msaa_color.Get(), target.msaa_state,
                   D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        target.msaa_state = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        transition(s.list.Get(), target.color.Get(), target.color_state,
                   D3D12_RESOURCE_STATE_RESOLVE_DEST);
        target.color_state = D3D12_RESOURCE_STATE_RESOLVE_DEST;
        s.list->ResolveSubresource(target.color.Get(), 0u, target.msaa_color.Get(), 0u,
                                   kColorFormat);
        ++s.report.dx12_resolves;
        transition(s.list.Get(), target.color.Get(), target.color_state,
                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        target.color_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (resume_render) prepare_target_for_render(s, target);
    } else {
        transition(s.list.Get(), target.color.Get(), target.color_state,
                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        target.color_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
}

bool wait_for_fence(Dx12GeState &s, UINT64 value, std::string &error) noexcept {
    if (value == 0u || !s.fence || s.fence->GetCompletedValue() >= value) return true;
    HRESULT hr = s.fence->SetEventOnCompletion(value, s.fence_event);
    if (FAILED(hr)) {
        error = hr_text(hr, "ID3D12Fence::SetEventOnCompletion(GE frame)");
        return false;
    }
    const DWORD result = WaitForSingleObject(s.fence_event, 5000u);
    if (result != WAIT_OBJECT_0) {
        std::ostringstream out;
        out << "DirectX 12 frame fence wait failed/timed out (wait=" << result << ')';
        if (s.device) {
            const HRESULT removed = s.device->GetDeviceRemovedReason();
            if (FAILED(removed)) out << "; device removed reason=0x" << std::hex
                                     << static_cast<unsigned long>(removed);
        }
        error = out.str();
        return false;
    }
    return true;
}

bool wait_for_gpu(Dx12GeState &s, std::string &error) noexcept {
    if (!s.queue || !s.fence) return true;
    const UINT64 value = s.next_fence++;
    HRESULT hr = s.queue->Signal(s.fence.Get(), value);
    if (FAILED(hr)) {
        error = hr_text(hr, "ID3D12CommandQueue::Signal(GE)");
        return false;
    }
    if (s.fence->GetCompletedValue() >= value) return true;
    hr = s.fence->SetEventOnCompletion(value, s.fence_event);
    if (FAILED(hr)) {
        error = hr_text(hr, "ID3D12Fence::SetEventOnCompletion(GE)");
        return false;
    }
    const DWORD result = WaitForSingleObject(s.fence_event, 5000u);
    if (result != WAIT_OBJECT_0) {
        std::ostringstream out;
        out << "DirectX 12 GE fence wait failed/timed out (wait=" << result << ')';
        if (s.device) {
            const HRESULT removed = s.device->GetDeviceRemovedReason();
            if (FAILED(removed)) out << "; device removed reason=0x" << std::hex
                                     << static_cast<unsigned long>(removed);
        }
        error = out.str();
        return false;
    }
    return true;
}

bool select_adapter(Dx12GeState &s, std::string &error) noexcept {
    for (UINT index = 0u;; ++index) {
        ComPtr<IDXGIAdapter1> candidate;
        HRESULT hr = s.factory->EnumAdapterByGpuPreference(
            index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&candidate));
        if (hr == DXGI_ERROR_NOT_FOUND) break;
        if (FAILED(hr)) continue;
        DXGI_ADAPTER_DESC1 desc{};
        candidate->GetDesc1(&desc);
        if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0u) continue;
        if (SUCCEEDED(D3D12CreateDevice(candidate.Get(), D3D_FEATURE_LEVEL_11_0,
                                         __uuidof(ID3D12Device), nullptr))) {
            s.adapter = candidate;
            s.adapter_vendor_id = desc.VendorId;
            s.adapter_device_id = desc.DeviceId;
            s.adapter_dedicated_video_memory = static_cast<std::uint64_t>(desc.DedicatedVideoMemory);
            s.adapter_shared_system_memory = static_cast<std::uint64_t>(desc.SharedSystemMemory);
            char utf8[512]{};
            const int count = WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1,
                                                   utf8, static_cast<int>(sizeof(utf8)), nullptr, nullptr);
            s.adapter_name = count > 0 ? utf8 : "Direct3D 12 adapter";
            return true;
        }
    }
    error = "No Direct3D 12-capable hardware adapter was found for the GE renderer";
    return false;
}

void detect_adapter_architecture_and_compat(Dx12GeState &s) noexcept {
    s.adapter_uma = false;
    s.adapter_cache_coherent_uma = false;
    if (s.device) {
        D3D12_FEATURE_DATA_ARCHITECTURE1 architecture{};
        architecture.NodeIndex = 0u;
        if (SUCCEEDED(s.device->CheckFeatureSupport(
                D3D12_FEATURE_ARCHITECTURE1, &architecture, sizeof(architecture)))) {
            s.adapter_uma = architecture.UMA != FALSE;
            s.adapter_cache_coherent_uma = architecture.CacheCoherentUMA != FALSE;
        } else {
            D3D12_FEATURE_DATA_ARCHITECTURE legacy{};
            legacy.NodeIndex = 0u;
            if (SUCCEEDED(s.device->CheckFeatureSupport(
                    D3D12_FEATURE_ARCHITECTURE, &legacy, sizeof(legacy)))) {
                s.adapter_uma = legacy.UMA != FALSE;
                s.adapter_cache_coherent_uma = legacy.CacheCoherentUMA != FALSE;
            }
        }
    }

    const bool amd_uma = s.adapter_vendor_id == kAmdVendorId && s.adapter_uma;
    const bool force_fast = env_truthy("PSPRECOMP_DX12_AMD_UMA_FASTPATHS");
    s.amd_uma_safe_mode = amd_uma && !force_fast;
    s.packed_0115_gpu_stride =
        s.amd_uma_safe_mode ? kPacked0115AmdUmaStride : kPacked0115NativeStride;

    std::ostringstream line;
    line << "dx12 adapter compatibility vendor=0x" << std::hex << s.adapter_vendor_id
         << " device=0x" << s.adapter_device_id << std::dec
         << " uma=" << (s.adapter_uma ? 1 : 0)
         << " coherent_uma=" << (s.adapter_cache_coherent_uma ? 1 : 0)
         << " amd_uma_safe=" << (s.amd_uma_safe_mode ? 1 : 0)
         << " packed0115_stride=" << s.packed_0115_gpu_stride
         << " dedicated_mb=" << (s.adapter_dedicated_video_memory / (1024u * 1024u))
         << " shared_mb=" << (s.adapter_shared_system_memory / (1024u * 1024u));
    runtime_log_line(line.str());
}

DXGI_FORMAT requested_depth_format(std::uint32_t bits) noexcept {
    if (bits <= 16u) return DXGI_FORMAT_D16_UNORM;
    if (bits <= 24u) return DXGI_FORMAT_D24_UNORM_S8_UINT;
    return DXGI_FORMAT_D32_FLOAT;
}

DXGI_FORMAT typeless_depth_resource_format(DXGI_FORMAT dsv_format) noexcept {
    switch (dsv_format) {
    case DXGI_FORMAT_D16_UNORM: return DXGI_FORMAT_R16_TYPELESS;
    case DXGI_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_R24G8_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT: return DXGI_FORMAT_R32_TYPELESS;
    default: return dsv_format;
    }
}

DXGI_FORMAT depth_srv_format(DXGI_FORMAT dsv_format) noexcept {
    switch (dsv_format) {
    case DXGI_FORMAT_D16_UNORM: return DXGI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}

bool realtime_shadows_requested() noexcept {
    const auto &proper = vcs_configuration().proper_shaders;
    return proper.enabled && proper.realtime_shadows.enabled &&
           proper_shaders_private_available();
}

bool format_supports_depth(ID3D12Device *device, DXGI_FORMAT format) noexcept {
    if (device == nullptr) return false;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support{};
    support.Format = format;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support))))
        return false;
    return (support.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL) != 0u;
}

void select_depth_and_msaa(Dx12GeState &s) noexcept {
    const RenderingConfiguration &rendering = vcs_configuration().rendering;
    s.depth_bits = rendering.depth_precision;
    s.depth_format = requested_depth_format(rendering.depth_precision);
    if (!format_supports_depth(s.device.Get(), s.depth_format)) {
        s.depth_format = DXGI_FORMAT_D32_FLOAT;
        s.depth_bits = 32u;
    }

    UINT requested = static_cast<UINT>(std::clamp(rendering.msaa, 1u, 16u));
    if (requested != 1u && requested != 2u && requested != 4u &&
        requested != 8u && requested != 16u) requested = 1u;

    // Each PSP framebuffer target is represented at the selected internal
    // resolution. On UMA this makes MSAA multiply shared-memory pressure across
    // several color+depth targets at once. Vega 8/5700G drivers have been seen
    // to device-remove instead of cleanly returning OOM. Prefer a deterministic
    // 1x fallback on UMA; advanced users can opt back in for diagnostics.
    if (s.adapter_uma && requested > 1u && !env_truthy("PSPRECOMP_DX12_UMA_MSAA")) {
        runtime_log_line("dx12 msaa compatibility: UMA adapter requested " +
                         std::to_string(requested) +
                         "x; forcing 1x to avoid multisampled framebuffer memory/device-removal");
        requested = 1u;
    }
    s.sample_count = 1u;
    s.sample_quality = 0u;
    for (UINT samples = requested; samples >= 2u; samples >>= 1u) {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS color{};
        color.Format = kColorFormat;
        color.SampleCount = samples;
        color.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS depth{};
        depth.Format = s.depth_format;
        depth.SampleCount = samples;
        depth.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        const bool color_ok = SUCCEEDED(s.device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &color, sizeof(color))) &&
            color.NumQualityLevels != 0u;
        const bool depth_ok = SUCCEEDED(s.device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &depth, sizeof(depth))) &&
            depth.NumQualityLevels != 0u;
        if (color_ok && depth_ok) {
            s.sample_count = samples;
            // Quality 0 is universally valid whenever NumQualityLevels > 0 and
            // avoids vendor-specific quality-mode assumptions.
            s.sample_quality = 0u;
            break;
        }
    }
    s.report.dx12_msaa_samples = s.sample_count;
    s.report.dx12_depth_bits = s.depth_bits;
}

D3D12_COMPARISON_FUNC depth_compare(std::uint32_t function) noexcept {
    switch (function & 7u) {
    case 0u: return D3D12_COMPARISON_FUNC_NEVER;
    case 1u: return D3D12_COMPARISON_FUNC_ALWAYS;
    case 2u: return D3D12_COMPARISON_FUNC_EQUAL;
    case 3u: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case 4u: return D3D12_COMPARISON_FUNC_LESS;
    case 5u: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case 6u: return D3D12_COMPARISON_FUNC_GREATER;
    case 7u: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    }
    return D3D12_COMPARISON_FUNC_ALWAYS;
}

struct Dx12BlendPlan {
    bool enabled{};
    bool exact{true};
    bool uses_constant{};
    std::uint32_t constant_rgb{};
    D3D12_BLEND src{D3D12_BLEND_ONE};
    D3D12_BLEND dst{D3D12_BLEND_ZERO};
    D3D12_BLEND src_alpha{D3D12_BLEND_ONE};
    D3D12_BLEND dst_alpha{D3D12_BLEND_ZERO};
    D3D12_BLEND_OP op{D3D12_BLEND_OP_ADD};
    D3D12_BLEND_OP op_alpha{D3D12_BLEND_OP_ADD};
};

std::size_t blend_variant(const GeGpuDrawDescriptor &draw) noexcept {
    // Retained only for the legacy per-mode counters in GeGpuBackendReport.
    // Pipeline selection itself uses dx12_blend_plan(), because VCS uses more
    // PSP GE blend states than the old five hard-coded variants.
    if (!draw.blend_enabled || draw.clear_mode) return 0u;
    const std::uint32_t eq = draw.blend_equation & 7u;
    const std::uint32_t src = draw.blend_source_factor & 0xFu;
    const std::uint32_t dst = draw.blend_dest_factor & 0xFu;
    if (eq == 0u && src == 2u && dst == 3u) return 1u; // src alpha / inv src alpha
    if (eq == 0u && src == 10u && dst == 10u) {
        const std::uint32_t fs = draw.blend_fix_source & 0x00FFFFFFu;
        const std::uint32_t fd = draw.blend_fix_dest & 0x00FFFFFFu;
        if (fs == 0x00FFFFFFu && fd == 0u) return 2u; // replace
        if (fs == 0x00FFFFFFu && fd == 0x00FFFFFFu) return 3u; // additive
        bool complements = true;
        for (std::uint32_t shift = 0u; shift < 24u; shift += 8u)
            complements &= (((fs >> shift) & 0xFFu) + ((fd >> shift) & 0xFFu)) == 0xFFu;
        if (complements) return 4u; // blend factor / inverse blend factor
    }
    if (eq == 0u && src == 2u && dst == 10u &&
        (draw.blend_fix_dest & 0x00FFFFFFu) == 0x00FFFFFFu) return 5u;
    return 0u;
}

bool fixed_rgb_is_complement(std::uint32_t left, std::uint32_t right) noexcept {
    left &= 0x00FFFFFFu;
    right &= 0x00FFFFFFu;
    for (std::uint32_t shift = 0u; shift < 24u; shift += 8u) {
        if ((((left >> shift) & 0xFFu) + ((right >> shift) & 0xFFu)) != 0xFFu)
            return false;
    }
    return true;
}

bool blend_source_factor(Dx12BlendPlan &plan, std::uint32_t factor,
                         std::uint32_t fixed_rgb) noexcept {
    switch (factor & 0xFu) {
    case 0u:
        plan.src = D3D12_BLEND_DEST_COLOR;
        plan.src_alpha = D3D12_BLEND_DEST_ALPHA;
        return true;
    case 1u:
        plan.src = D3D12_BLEND_INV_DEST_COLOR;
        plan.src_alpha = D3D12_BLEND_INV_DEST_ALPHA;
        return true;
    case 2u:
        plan.src = D3D12_BLEND_SRC_ALPHA;
        plan.src_alpha = D3D12_BLEND_SRC_ALPHA;
        return true;
    case 3u:
        plan.src = D3D12_BLEND_INV_SRC_ALPHA;
        plan.src_alpha = D3D12_BLEND_INV_SRC_ALPHA;
        return true;
    case 4u:
        plan.src = D3D12_BLEND_DEST_ALPHA;
        plan.src_alpha = D3D12_BLEND_DEST_ALPHA;
        return true;
    case 5u:
        plan.src = D3D12_BLEND_INV_DEST_ALPHA;
        plan.src_alpha = D3D12_BLEND_INV_DEST_ALPHA;
        return true;
    case 10u: {
        const std::uint32_t fixed = fixed_rgb & 0x00FFFFFFu;
        if (fixed == 0u) {
            plan.src = D3D12_BLEND_ZERO;
            plan.src_alpha = D3D12_BLEND_ZERO;
        } else if (fixed == 0x00FFFFFFu) {
            plan.src = D3D12_BLEND_ONE;
            plan.src_alpha = D3D12_BLEND_ONE;
        } else {
            plan.src = D3D12_BLEND_BLEND_FACTOR;
            // PSP FIX alpha is effectively 1.0: the GE register contains RGB.
            plan.src_alpha = D3D12_BLEND_ONE;
            plan.uses_constant = true;
            plan.constant_rgb = fixed;
        }
        return true;
    }
    default:
        // PSP factors 6..9 are 2x-alpha forms. D3D12 fixed-function blending
        // has no bit-exact equivalent, so those states take the safe fallback.
        return false;
    }
}

bool blend_dest_factor(Dx12BlendPlan &plan, std::uint32_t factor,
                       std::uint32_t fixed_rgb) noexcept {
    switch (factor & 0xFu) {
    case 0u:
        plan.dst = D3D12_BLEND_SRC_COLOR;
        plan.dst_alpha = D3D12_BLEND_SRC_ALPHA;
        return true;
    case 1u:
        plan.dst = D3D12_BLEND_INV_SRC_COLOR;
        plan.dst_alpha = D3D12_BLEND_INV_SRC_ALPHA;
        return true;
    case 2u:
        plan.dst = D3D12_BLEND_SRC_ALPHA;
        plan.dst_alpha = D3D12_BLEND_SRC_ALPHA;
        return true;
    case 3u:
        plan.dst = D3D12_BLEND_INV_SRC_ALPHA;
        plan.dst_alpha = D3D12_BLEND_INV_SRC_ALPHA;
        return true;
    case 4u:
        plan.dst = D3D12_BLEND_DEST_ALPHA;
        plan.dst_alpha = D3D12_BLEND_DEST_ALPHA;
        return true;
    case 5u:
        plan.dst = D3D12_BLEND_INV_DEST_ALPHA;
        plan.dst_alpha = D3D12_BLEND_INV_DEST_ALPHA;
        return true;
    case 10u: {
        const std::uint32_t fixed = fixed_rgb & 0x00FFFFFFu;
        if (fixed == 0u) {
            plan.dst = D3D12_BLEND_ZERO;
            plan.dst_alpha = D3D12_BLEND_ZERO;
        } else if (fixed == 0x00FFFFFFu) {
            plan.dst = D3D12_BLEND_ONE;
            plan.dst_alpha = D3D12_BLEND_ONE;
        } else {
            plan.dst = D3D12_BLEND_BLEND_FACTOR;
            plan.dst_alpha = D3D12_BLEND_ONE;
            plan.uses_constant = true;
            plan.constant_rgb = fixed;
        }
        return true;
    }
    default:
        return false;
    }
}

Dx12BlendPlan dx12_blend_plan(const GeGpuDrawDescriptor &draw) noexcept {
    Dx12BlendPlan plan{};
    if (!draw.blend_enabled || draw.clear_mode) return plan;
    plan.enabled = true;

    const std::uint32_t equation = draw.blend_equation & 7u;
    const std::uint32_t src_factor = draw.blend_source_factor & 0xFu;
    const std::uint32_t dst_factor = draw.blend_dest_factor & 0xFu;
    const std::uint32_t src_fix = draw.blend_fix_source & 0x00FFFFFFu;
    const std::uint32_t dst_fix = draw.blend_fix_dest & 0x00FFFFFFu;

    // MIN/MAX in the PSP software reference operate on the unweighted source
    // and destination colors. D3D12 MIN/MAX do exactly that with ONE/ONE.
    if (equation == 3u || equation == 4u) {
        plan.src = plan.dst = D3D12_BLEND_ONE;
        plan.src_alpha = plan.dst_alpha = D3D12_BLEND_ONE;
        plan.op = plan.op_alpha = equation == 3u ? D3D12_BLEND_OP_MIN : D3D12_BLEND_OP_MAX;
        return plan;
    }

    switch (equation) {
    case 0u: plan.op = plan.op_alpha = D3D12_BLEND_OP_ADD; break;
    case 1u: plan.op = plan.op_alpha = D3D12_BLEND_OP_SUBTRACT; break;
    case 2u: plan.op = plan.op_alpha = D3D12_BLEND_OP_REV_SUBTRACT; break;
    default:
        // ABS_DIFF and unknown equations cannot be expressed by the D3D12
        // fixed-function blend unit.
        plan.exact = false;
        break;
    }

    if (plan.exact && src_factor == 10u && dst_factor == 10u &&
        src_fix != 0u && src_fix != 0x00FFFFFFu &&
        dst_fix != 0u && dst_fix != 0x00FFFFFFu) {
        // D3D12 exposes one dynamic blend constant. Two arbitrary PSP FIX/FIX
        // colors are representable only when they are identical or complementary.
        if (src_fix == dst_fix) {
            plan.uses_constant = true;
            plan.constant_rgb = src_fix;
            plan.src = plan.dst = D3D12_BLEND_BLEND_FACTOR;
            plan.src_alpha = plan.dst_alpha = D3D12_BLEND_ONE;
        } else if (fixed_rgb_is_complement(src_fix, dst_fix)) {
            plan.uses_constant = true;
            plan.constant_rgb = src_fix;
            plan.src = D3D12_BLEND_BLEND_FACTOR;
            plan.dst = D3D12_BLEND_INV_BLEND_FACTOR;
            plan.src_alpha = D3D12_BLEND_ONE;
            plan.dst_alpha = D3D12_BLEND_ZERO;
        } else {
            plan.exact = false;
        }
    } else if (plan.exact) {
        const bool src_ok = blend_source_factor(plan, src_factor, src_fix);
        const bool src_uses_constant = plan.uses_constant;
        const std::uint32_t src_constant = plan.constant_rgb;

        // Evaluate the destination independently so an arbitrary FIX on each
        // side cannot silently overwrite the one D3D12 blend constant.
        Dx12BlendPlan dst_plan{};
        const bool dst_ok = blend_dest_factor(dst_plan, dst_factor, dst_fix);
        if (!src_ok || !dst_ok ||
            (src_uses_constant && dst_plan.uses_constant && src_constant != dst_plan.constant_rgb)) {
            plan.exact = false;
        } else {
            plan.dst = dst_plan.dst;
            plan.dst_alpha = dst_plan.dst_alpha;
            if (dst_plan.uses_constant) {
                plan.uses_constant = true;
                plan.constant_rgb = dst_plan.constant_rgb;
            }
        }
    }

    if (!plan.exact) {
        // Safety fallback. The old DX12 path returned blend variant 0 here,
        // disabling blending and turning translucent sun/lighting overlays into
        // opaque full-screen washes. Keep the draw translucent instead. Exact
        // parity for 2x-alpha/ABS_DIFF/non-complementary FIX/FIX still requires
        // a shader/RMW path, and is reported as unsupported below.
        plan.enabled = true;
        plan.uses_constant = false;
        plan.constant_rgb = 0u;
        plan.src = D3D12_BLEND_SRC_ALPHA;
        plan.dst = D3D12_BLEND_INV_SRC_ALPHA;
        plan.src_alpha = D3D12_BLEND_ONE;
        plan.dst_alpha = D3D12_BLEND_INV_SRC_ALPHA;
        plan.op = plan.op_alpha = D3D12_BLEND_OP_ADD;
    }
    return plan;
}

std::uint8_t color_write_mask(const GeGpuDrawDescriptor &draw) noexcept {
    std::uint8_t mask = 0u;
    for (std::uint32_t channel = 0u; channel < 4u; ++channel) {
        const std::uint32_t byte = (draw.color_write_mask >> (channel * 8u)) & 0xFFu;
        if (byte != 0xFFu) mask |= static_cast<std::uint8_t>(1u << channel);
    }
    return mask;
}

std::uint64_t pipeline_key(const GeGpuDrawDescriptor &draw) noexcept {
    const Dx12BlendPlan blend = dx12_blend_plan(draw);
    std::uint64_t key = static_cast<std::uint64_t>(draw.depth_test_enabled ? 1u : 0u);
    key |= static_cast<std::uint64_t>(draw.depth_write_enabled ? 1u : 0u) << 1u;
    key |= static_cast<std::uint64_t>(draw.depth_function & 7u) << 2u;
    key |= static_cast<std::uint64_t>(blend.enabled ? 1u : 0u) << 5u;
    key |= static_cast<std::uint64_t>(blend.exact ? 0u : 1u) << 6u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.src) & 0x1Fu) << 7u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.dst) & 0x1Fu) << 12u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.src_alpha) & 0x1Fu) << 17u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.dst_alpha) & 0x1Fu) << 22u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.op) & 0x7u) << 27u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint32_t>(blend.op_alpha) & 0x7u) << 30u;
    key |= static_cast<std::uint64_t>(color_write_mask(draw) & 0xFu) << 33u;
    key |= static_cast<std::uint64_t>(blend.uses_constant ? 1u : 0u) << 37u;
    key |= static_cast<std::uint64_t>(static_cast<std::uint8_t>(draw.shader_pipe) & 3u) << 38u;
    return key;
}

bool hardware_transform_equal(const GeGpuHardwareTransform &a,
                              const GeGpuHardwareTransform &b) noexcept {
    // Fields consumed by the native draw constants *or* the private shadow
    // caster participate. The trailing accounting counters deliberately do not:
    // two adjacent PRIMs can
    // have different logical batching statistics while still sharing identical
    // shader constants.  Explicit field comparison avoids relying on struct
    // padding with memcmp.
    return a.model_to_world == b.model_to_world &&
           a.model_to_clip == b.model_to_clip &&
           a.model_to_view_z == b.model_to_view_z &&
           a.viewport_scale_x == b.viewport_scale_x &&
           a.viewport_scale_y == b.viewport_scale_y &&
           a.viewport_scale_z == b.viewport_scale_z &&
           a.viewport_center_x == b.viewport_center_x &&
           a.viewport_center_y == b.viewport_center_y &&
           a.viewport_center_z == b.viewport_center_z &&
           a.viewport_offset_x == b.viewport_offset_x &&
           a.viewport_offset_y == b.viewport_offset_y &&
           a.uv_scale_u == b.uv_scale_u && a.uv_scale_v == b.uv_scale_v &&
           a.uv_offset_u == b.uv_offset_u && a.uv_offset_v == b.uv_offset_v &&
           a.fog_end == b.fog_end && a.fog_slope == b.fog_slope &&
           a.depth_clip_enabled == b.depth_clip_enabled &&
           a.cull_enabled == b.cull_enabled &&
           a.accept_counter_clockwise == b.accept_counter_clockwise &&
           a.primitive == b.primitive &&
           a.vertex_color_affine == b.vertex_color_affine &&
           a.vertex_color_mul == b.vertex_color_mul &&
           a.vertex_color_add == b.vertex_color_add;
}

bool adjacent_batch_merge_compatible(const Dx12Batch &a, const Dx12Batch &b) noexcept {
    // Framebuffer-feedback draws have an exact snapshot boundary. Indexed lists
    // are mergeable too: append_or_merge_batch() rebases the new local indices
    // onto the previous batch's base vertex before extending the draw.
    if (a.framebuffer_feedback || b.framebuffer_feedback) return false;
    if (a.indexed != b.indexed) return false;
    if (a.packed_0115 != b.packed_0115) return false;
    if (a.first_vertex + a.vertex_count != b.first_vertex) return false;
    if (a.indexed && a.first_index + a.index_count != b.first_index) return false;
    if ((a.draw.framebuffer_address & 0x001FFFF0u) !=
        (b.draw.framebuffer_address & 0x001FFFF0u)) return false;
    if (a.hardware_transform != b.hardware_transform) return false;
    // Concatenating independent triangle strips would synthesize bridge
    // triangles between PRIMs. Only triangle-list batches may merge.
    if ((a.hardware_transform && a.transform.primitive != 3u) ||
        (b.hardware_transform && b.transform.primitive != 3u)) return false;
    if (pipeline_key(a.draw) != pipeline_key(b.draw)) return false;
    if (a.draw.texture_enabled != b.draw.texture_enabled) return false;
    if (a.draw.texture_enabled &&
        (texture_key(a.draw) != texture_key(b.draw) || sampler_key(a.draw) != sampler_key(b.draw)))
        return false;
    if (a.draw.scissor_x0 != b.draw.scissor_x0 || a.draw.scissor_y0 != b.draw.scissor_y0 ||
        a.draw.scissor_x1 != b.draw.scissor_x1 || a.draw.scissor_y1 != b.draw.scissor_y1)
        return false;
    const Dx12BlendPlan blend_a = dx12_blend_plan(a.draw);
    const Dx12BlendPlan blend_b = dx12_blend_plan(b.draw);
    if (blend_a.uses_constant && blend_a.constant_rgb != blend_b.constant_rgb) return false;
    // Pixel state moved out of the vertex stream in 45.4, so merged draws must
    // share the exact root-constant state rather than merely a compatible PSO.
    const Dx12PixelConstants pa = make_pixel_constants(a.draw, a.draw.texture_enabled);
    const Dx12PixelConstants pb = make_pixel_constants(b.draw, b.draw.texture_enabled);
    if (std::memcmp(&pa, &pb, sizeof(pa)) != 0) return false;
    if (a.draw.texture_mipmap_enabled != b.draw.texture_mipmap_enabled ||
        a.draw.texture_mipmap_linear != b.draw.texture_mipmap_linear) return false;
    return !a.hardware_transform || hardware_transform_equal(a.transform, b.transform);
}

bool dx12_execute_indirect_enabled(const Dx12GeState &s) noexcept {
    // V5 carries the complete 21-DWORD root state (including material
    // shadows), but the matched NVIDIA A/B trace showed lower total FPS even
    // when GE time improved slightly: guest-side work rose enough to lose
    // about 3.7%. Keep this experimental path explicit instead of silently
    // enabling it when the executable is launched directly.
    static const int environment_override = [] {
        const char *text = std::getenv("PSPRECOMP_DX12_EXECUTE_INDIRECT");
        if (text == nullptr || *text == '\0') return -1;
        const bool enabled = std::strcmp(text, "0") != 0 &&
            std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
            std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
        return enabled ? 1 : 0;
    }();
    (void)s;
    return environment_override > 0;
}

D3D12_PRIMITIVE_TOPOLOGY batch_topology(const Dx12Batch &batch) noexcept {
    return batch.hardware_transform && batch.transform.primitive == 4u
        ? D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
        : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

std::uint64_t batch_full_pipeline_key(const Dx12Batch &batch) noexcept {
    const bool cull = batch.hardware_transform && batch.transform.cull_enabled;
    const bool accept_ccw = cull && batch.transform.accept_counter_clockwise;
    return pipeline_key(batch.draw) |
        (batch.packed_0115 ? (std::uint64_t{1} << 63u) : 0u) |
        (cull ? (std::uint64_t{1} << 62u) : 0u) |
        (accept_ccw ? (std::uint64_t{1} << 61u) : 0u);
}

bool indirect_run_compatible(const Dx12Batch &a, const Dx12Batch &b) noexcept {
    if (a.framebuffer_feedback || b.framebuffer_feedback) return false;
    if (a.draw.clear_mode || b.draw.clear_mode) return false;
    if (a.indexed != b.indexed || a.packed_0115 != b.packed_0115) return false;
    if ((a.draw.framebuffer_address & 0x001FFFF0u) !=
        (b.draw.framebuffer_address & 0x001FFFF0u)) return false;
    if (batch_topology(a) != batch_topology(b)) return false;
    if (batch_full_pipeline_key(a) != batch_full_pipeline_key(b)) return false;
    if (a.draw.texture_enabled != b.draw.texture_enabled) return false;
    if (a.draw.texture_enabled &&
        (texture_key(a.draw) != texture_key(b.draw) || sampler_key(a.draw) != sampler_key(b.draw)))
        return false;
    if (a.draw.scissor_x0 != b.draw.scissor_x0 || a.draw.scissor_y0 != b.draw.scissor_y0 ||
        a.draw.scissor_x1 != b.draw.scissor_x1 || a.draw.scissor_y1 != b.draw.scissor_y1)
        return false;
    const Dx12BlendPlan ba = dx12_blend_plan(a.draw);
    const Dx12BlendPlan bb = dx12_blend_plan(b.draw);
    if (ba.uses_constant != bb.uses_constant) return false;
    if (ba.uses_constant && ba.constant_rgb != bb.constant_rgb) return false;
    return true;
}

void account_executed_batch(Dx12GeState &s, const Dx12Batch &batch,
                            std::uint32_t srv_index, const Dx12BlendPlan &blend_plan) {
    if (batch.draw.depth_test_enabled) s.report.depth_tested_game_draw_calls += batch.logical_draw_count;
    if (batch.draw.depth_write_enabled) s.report.depth_writing_game_draw_calls += batch.logical_draw_count;
    if (batch.draw.alpha_test_enabled) s.report.alpha_tested_game_draw_calls += batch.logical_draw_count;
    switch (blend_variant(batch.draw)) {
    case 1u: s.report.standard_alpha_blended_game_draw_calls += batch.logical_draw_count; break;
    case 2u: s.report.fixed_replace_blended_game_draw_calls += batch.logical_draw_count; break;
    case 3u: s.report.additive_blended_game_draw_calls += batch.logical_draw_count; break;
    default: break;
    }
    if (batch.draw.blend_enabled && !batch.draw.clear_mode && !blend_plan.exact) {
        s.report.unsupported_blend_game_draw_calls += batch.logical_draw_count;
        static std::uint32_t diagnostic_count = 0u;
        if (std::getenv("PSPRECOMP_DX12_BLEND_DIAG") != nullptr && diagnostic_count < 32u) {
            std::ostringstream line;
            line << "DX12 unsupported PSP blend fallback #" << (diagnostic_count + 1u)
                 << ": eq=" << (batch.draw.blend_equation & 7u)
                 << " src=" << (batch.draw.blend_source_factor & 0xFu)
                 << " dst=" << (batch.draw.blend_dest_factor & 0xFu)
                 << " fixS=0x" << std::hex << (batch.draw.blend_fix_source & 0x00FFFFFFu)
                 << " fixD=0x" << (batch.draw.blend_fix_dest & 0x00FFFFFFu) << std::dec;
            const std::string message = line.str();
            std::cerr << "[blend] " << message << "\n";
            runtime_log_error("blend", message);
            ++diagnostic_count;
        }
    }
    if (batch.draw.fog_enabled) s.report.fogged_game_draw_calls += batch.logical_draw_count;
    if (srv_index != 0u) {
        const std::uint32_t submitted_vertices = batch.indexed ? batch.index_count : batch.vertex_count;
        s.report.textured_game_triangles +=
            batch.hardware_transform && batch.transform.primitive == 4u
                ? (submitted_vertices > 2u ? submitted_vertices - 2u : 0u)
                : submitted_vertices / 3u;
        s.report.textured_game_vertices += submitted_vertices;
        switch (batch.draw.texture_function & 7u) {
        case 0u: s.report.modulate_texture_game_draw_calls += batch.logical_draw_count; break;
        case 1u: s.report.decal_texture_game_draw_calls += batch.logical_draw_count; break;
        case 2u: s.report.blend_texture_game_draw_calls += batch.logical_draw_count; break;
        case 3u: s.report.replace_texture_game_draw_calls += batch.logical_draw_count; break;
        case 4u: s.report.add_texture_game_draw_calls += batch.logical_draw_count; break;
        default: ++s.report.unsupported_texture_function_game_draw_calls; break;
        }
        if (batch.draw.texture_double_color)
            s.report.double_color_texture_game_draw_calls += batch.logical_draw_count;
        if (batch.draw.texture_mipmap_enabled) {
            s.report.mipmapped_game_draw_calls += batch.logical_draw_count;
            if (batch.draw.texture_mipmap_linear)
                s.report.mip_linear_game_draw_calls += batch.logical_draw_count;
        }
    }
}

bool append_or_merge_batch(Dx12GeState &s, Dx12Batch batch) {
    static const bool merge_enabled = [] {
        const char *text = std::getenv("PSPRECOMP_DX12_BATCH_MERGE");
        return text == nullptr || (*text != '\0' && std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0);
    }();
    if (merge_enabled && !s.amd_uma_safe_mode && !s.batches.empty() &&
        adjacent_batch_merge_compatible(s.batches.back(), batch)) {
        Dx12Batch &previous = s.batches.back();
        const bool counts_fit =
            batch.vertex_count <= std::numeric_limits<std::uint32_t>::max() - previous.vertex_count &&
            (!batch.indexed || batch.index_count <=
                std::numeric_limits<std::uint32_t>::max() - previous.index_count);
        bool indices_fit = counts_fit;
        std::size_t begin = 0u, end = 0u;
        std::uint32_t base_delta = 0u;
        if (indices_fit && batch.indexed) {
            base_delta = batch.first_vertex - previous.first_vertex;
            begin = batch.first_index;
            end = begin + batch.index_count;
            indices_fit = end <= s.indices.size();
            for (std::size_t i = begin; indices_fit && i < end; ++i)
                indices_fit = s.indices[i] <= std::numeric_limits<std::uint32_t>::max() - base_delta;
        }
        if (indices_fit) {
            if (batch.indexed) {
                for (std::size_t i = begin; i < end; ++i) s.indices[i] += base_delta;
                previous.index_count += batch.index_count;
            }
            previous.vertex_count += batch.vertex_count;
            previous.logical_draw_count += batch.logical_draw_count;
            ++s.report.dx12_batch_merges;
            return true;
        }
    }
    s.batches.push_back(std::move(batch));
    ++s.report.dx12_batch_appends;
    return false;
}


void promote_vehicle_batch_groups(Dx12GeState &s) noexcept {
    const auto &proper = vcs_configuration().proper_shaders;
    if (!proper.enabled || !proper.vehicle_pipe.enabled ||
        !proper_shaders_private_available() || s.batches.empty())
        return;

    // The palette classifier gives us high-confidence anchor materials, but a
    // RenderWare vehicle is an atomic/group with wheels, glass, lights and trim
    // that often use non-paletted textures. Promote the *contiguous entity run*
    // around each anchor. Spatial/model-transform checks keep static world
    // batches from being swallowed by a nearby car.
    std::vector<std::size_t> anchors;
    anchors.reserve(32u);
    for (std::size_t i = 0u; i < s.batches.size(); ++i) {
        if (s.batches[i].draw.shader_pipe == GeShaderPipe::Vehicle)
            anchors.push_back(i);
    }
    if (anchors.empty()) return;

    const auto origin = [](const Dx12Batch &batch) {
        return std::array<float, 3>{batch.transform.model_to_world[12],
                                    batch.transform.model_to_world[13],
                                    batch.transform.model_to_world[14]};
    };
    const auto finite_origin = [](const std::array<float, 3> &v) {
        return std::isfinite(v[0]) && std::isfinite(v[1]) && std::isfinite(v[2]);
    };
    const auto distance2 = [](const std::array<float, 3> &a,
                              const std::array<float, 3> &b) {
        const float x = a[0] - b[0], y = a[1] - b[1], z = a[2] - b[2];
        return x * x + y * y + z * z;
    };
    const auto approximately_identity_origin = [](const std::array<float, 3> &v) {
        return std::fabs(v[0]) < 1.0e-4f && std::fabs(v[1]) < 1.0e-4f &&
               std::fabs(v[2]) < 1.0e-4f;
    };

    std::uint32_t promoted = 0u;
    constexpr std::size_t kMaxNeighbourDraws = 18u;
    constexpr float kMaxVehiclePartDistance2 = 14.0f * 14.0f;
    for (const std::size_t anchor_index : anchors) {
        if (anchor_index >= s.batches.size()) continue;
        const Dx12Batch &anchor = s.batches[anchor_index];
        if (!anchor.hardware_transform) continue;
        const std::uint32_t target = anchor.draw.framebuffer_address & 0x001FFFF0u;
        const auto anchor_origin = origin(anchor);
        const bool anchor_origin_valid = finite_origin(anchor_origin);
        const bool anchor_identity_origin = approximately_identity_origin(anchor_origin);

        const auto candidate_matches = [&](const Dx12Batch &candidate,
                                           std::size_t distance_in_draws) {
            if (!candidate.hardware_transform || candidate.draw.clear_mode ||
                candidate.framebuffer_feedback || candidate.draw.through)
                return false;
            if ((candidate.draw.framebuffer_address & 0x001FFFF0u) != target)
                return false;
            if (candidate.draw.shader_pipe == GeShaderPipe::Skin ||
                candidate.draw.shader_pipe == GeShaderPipe::Native)
                return false;
            if (candidate.draw.depth_test_enabled != anchor.draw.depth_test_enabled ||
                candidate.draw.depth_write_enabled != anchor.draw.depth_write_enabled)
                return false;
            const auto candidate_origin = origin(candidate);
            if (!anchor_origin_valid || !finite_origin(candidate_origin)) return false;
            const bool candidate_identity = approximately_identity_origin(candidate_origin);
            if (!anchor_identity_origin || !candidate_identity)
                return distance2(anchor_origin, candidate_origin) <= kMaxVehiclePartDistance2;
            // Some VCS entity draws arrive pretransformed in world space and
            // therefore carry identity model_to_world. In that case spatial
            // origin cannot distinguish them; keep promotion intentionally
            // narrow and contiguous around the high-confidence palette anchor.
            return distance_in_draws <= 12u;
        };

        for (int direction : {-1, 1}) {
            for (std::size_t step = 1u; step <= kMaxNeighbourDraws; ++step) {
                const std::int64_t signed_index = static_cast<std::int64_t>(anchor_index) +
                    static_cast<std::int64_t>(direction) * static_cast<std::int64_t>(step);
                if (signed_index < 0 ||
                    signed_index >= static_cast<std::int64_t>(s.batches.size()))
                    break;
                Dx12Batch &candidate = s.batches[static_cast<std::size_t>(signed_index)];
                if (!candidate_matches(candidate, step)) break;
                if (candidate.draw.shader_pipe == GeShaderPipe::Building) {
                    candidate.draw.shader_pipe = GeShaderPipe::Vehicle;
                    ++promoted;
                }
            }
        }
    }

    if (promoted != 0u && (s.frame_epoch < 8u || (s.frame_epoch % 120u) == 0u)) {
        runtime_log_line("ProperShaders vehicle entity promotion anchors=" +
                         std::to_string(anchors.size()) + " promoted_parts=" +
                         std::to_string(promoted));
    }
}

bool compile_shaders(Dx12GeState &s, std::string &error) noexcept {
    const char *shader = R"HLSL(
Texture2D<float4> SourceTexture : register(t0);
SamplerState SourceSampler : register(s0);
cbuffer DrawTransform : register(b0) {
    float4 TransformRow0;
    float4 TransformRow1;
    float4 TransformRow2;
    float4 TransformRow3;
    float4 ModelToViewZ;
    float4 UvScaleOffset;
    float4 FogParameters;
    uint4 TransformControl;
    float4 VertexColorMul;
    float4 VertexColorAdd;
};
cbuffer DrawPixelState : register(b1) {
    uint AlphaControlPacked;
    uint TextureControlPacked;
    uint TextureEnvPacked;
    uint FogControlPacked;
    uint FramebufferFormat;
    // The remaining 16 DWORDs are private-material scratch constants. Their
    // scalar/float3 packing deliberately starts at c1.y so all 21 root DWORDs
    // remain densely addressable without cbuffer alignment holes.
    float3 ProperShadowRow0XYZ;
    float ProperShadowRow0W;
    float3 ProperShadowRow1XYZ;
    float ProperShadowRow1W;
    float3 ProperShadowRow2XYZ;
    float ProperShadowRow2W;
    float3 ProperShadowRow3XYZ;
    float ProperShadowRow3W;
};
struct VSIn {
    float4 position : POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float q : TEXCOORD1;
    float fogFactor : FOG0;
};
struct VSOut {
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float q : TEXCOORD1;
    float fogFactor : FOG0;
};
VSOut VSMain(VSIn input) {
    VSOut o;
    if (TransformControl.x == 1u) {
        float4 p = input.position;
        float clipW = dot(TransformRow3, p);
        if (abs(clipW) < 1.0e-12) clipW = 1.0;
        float clipZ = dot(TransformRow2, p);
        if (TransformControl.y == 0u) clipZ = clamp(clipZ, 0.0, clipW);
        o.position = float4(dot(TransformRow0, p), dot(TransformRow1, p), clipZ, clipW);
        o.uv = input.uv * UvScaleOffset.xy + UvScaleOffset.zw;
        float viewZ = dot(ModelToViewZ, p);
        o.fogFactor = saturate((viewZ + FogParameters.x) * FogParameters.y);
    } else if (TransformControl.x == 2u) {
        float clipW = input.position.w;
        if (abs(clipW) < 1.0e-12) clipW = 1.0;
        o.position = float4(
            (input.position.x * UvScaleOffset.x - 1.0) * clipW,
            (1.0 - input.position.y * UvScaleOffset.y) * clipW,
            saturate(input.position.z * UvScaleOffset.z) * clipW,
            clipW);
        o.uv = input.uv;
        o.fogFactor = input.fogFactor;
    } else {
        o.position = input.position;
        o.uv = input.uv;
        o.fogFactor = input.fogFactor;
    }
    o.color = input.color;
    if (TransformControl.z != 0u) {
        float4 lit = saturate(o.color * VertexColorMul + VertexColorAdd);
        o.color = floor(lit * 255.0) * (1.0 / 255.0);
    }
    o.q = input.q;
    return o;
}

// Stage 45.4: direct GPU decode for VCS's dominant 10-byte PSP world vertex
// (vtype 0x000115). The CPU only snapshots the original bytes; conversion to
// float UV / RGBA8 / normalized XYZ is performed by the vertex shader.
struct VSInPacked0115 {
    uint2 uv8 : TEXCOORD2;
    uint color5551 : COLOR1;
    int2 positionXY : POSITION1;
    int positionZ : POSITION2;
};
float Expand5ToFloat(uint value) {
    value &= 31u;
    uint expanded = (value << 3u) | (value >> 2u);
    return float(expanded) * (1.0 / 255.0);
}
VSOut VSMainPacked0115(VSInPacked0115 input) {
    VSOut o;
    float3 model = float3(float2(input.positionXY), float(input.positionZ)) * (1.0 / 32768.0);
    float4 p = float4(model, 1.0);
    float clipW = dot(TransformRow3, p);
    if (abs(clipW) < 1.0e-12) clipW = 1.0;
    float clipZ = dot(TransformRow2, p);
    if (TransformControl.y == 0u) clipZ = clamp(clipZ, 0.0, clipW);
    o.position = float4(dot(TransformRow0, p), dot(TransformRow1, p), clipZ, clipW);
    float2 rawUv = float2(input.uv8) * (1.0 / 128.0);
    o.uv = rawUv * UvScaleOffset.xy + UvScaleOffset.zw;
    float viewZ = dot(ModelToViewZ, p);
    o.fogFactor = saturate((viewZ + FogParameters.x) * FogParameters.y);
    uint packed = input.color5551;
    o.color = float4(
        Expand5ToFloat(packed),
        Expand5ToFloat(packed >> 5u),
        Expand5ToFloat(packed >> 10u),
        (packed & 0x8000u) != 0u ? 1.0 : 0.0);
    if (TransformControl.z != 0u) {
        // Match the CPU path's per-channel RGBA8 clamp/truncation boundary.
        float4 lit = saturate(o.color * VertexColorMul + VertexColorAdd);
        o.color = floor(lit * 255.0) * (1.0 / 255.0);
    }
    o.q = 1.0;
    return o;
}

bool AlphaPass(uint fn, uint lhs, uint rhs) {
    switch (fn & 7u) {
        case 0u: return false;
        case 1u: return true;
        case 2u: return lhs == rhs;
        case 3u: return lhs != rhs;
        case 4u: return lhs < rhs;
        case 5u: return lhs <= rhs;
        case 6u: return lhs > rhs;
        case 7u: return lhs >= rhs;
    }
    return true;
}
float4 ApplyTextureFunction(float4 vertex, float4 textureValue, uint4 control, uint4 envBytes) {
    uint fn = control.x & 7u;
    bool useAlpha = control.y != 0u;
    bool doubleColor = control.z != 0u;
    float4 outColor = vertex;
    float3 env = float3(envBytes.xyz) / 255.0;
    if (fn == 0u) { // MODULATE
        outColor.rgb = vertex.rgb * textureValue.rgb;
        outColor.a = useAlpha ? vertex.a * textureValue.a : vertex.a;
    } else if (fn == 1u) { // DECAL
        float a = useAlpha ? textureValue.a : 1.0;
        outColor.rgb = lerp(vertex.rgb, textureValue.rgb, a);
        outColor.a = vertex.a;
    } else if (fn == 2u) { // BLEND
        outColor.rgb = lerp(vertex.rgb, env, textureValue.rgb);
        outColor.a = useAlpha ? vertex.a * textureValue.a : vertex.a;
    } else if (fn == 3u) { // REPLACE
        outColor = textureValue;
        if (!useAlpha) outColor.a = vertex.a;
    } else if (fn == 4u) { // ADD
        outColor.rgb = saturate(vertex.rgb + textureValue.rgb);
        outColor.a = useAlpha ? vertex.a * textureValue.a : vertex.a;
    }
    if (doubleColor) outColor.rgb = saturate(outColor.rgb * 2.0);
    return outColor;
}
float Quantize(float value, float levels) {
    return floor(saturate(value) * levels + 0.5) / levels;
}
float4 QuantizeFramebuffer(float4 color, uint format) {
    color = saturate(color);
    if ((format & 3u) == 0u) { // PSP GU_PSM_5650
        color.r = Quantize(color.r, 31.0);
        color.g = Quantize(color.g, 63.0);
        color.b = Quantize(color.b, 31.0);
        color.a = 1.0;
    } else if ((format & 3u) == 1u) { // GU_PSM_5551
        color.rgb = float3(Quantize(color.r,31.0), Quantize(color.g,31.0), Quantize(color.b,31.0));
        color.a = color.a >= 0.5 ? 1.0 : 0.0;
    } else if ((format & 3u) == 2u) { // GU_PSM_4444
        color = float4(Quantize(color.r,15.0), Quantize(color.g,15.0),
                       Quantize(color.b,15.0), Quantize(color.a,15.0));
    }
    return color;
}
float4 PSMain(VSOut input) : SV_TARGET {
    const uint4 textureControl = uint4(TextureControlPacked & 0xFFu,
        (TextureControlPacked >> 8u) & 0xFFu, (TextureControlPacked >> 16u) & 0xFFu,
        (TextureControlPacked >> 24u) & 0xFFu);
    const uint4 textureEnv = uint4(TextureEnvPacked & 0xFFu,
        (TextureEnvPacked >> 8u) & 0xFFu, (TextureEnvPacked >> 16u) & 0xFFu, 0u);
    const uint4 fogControl = uint4(FogControlPacked & 0xFFu,
        (FogControlPacked >> 8u) & 0xFFu, (FogControlPacked >> 16u) & 0xFFu,
        (FogControlPacked >> 24u) & 0xFFu);
    const uint4 alphaControl = uint4(AlphaControlPacked & 0xFFu,
        (AlphaControlPacked >> 8u) & 0xFFu, (AlphaControlPacked >> 16u) & 0xFFu,
        (AlphaControlPacked >> 24u) & 0xFFu);
    float4 color = saturate(input.color);
    if (textureControl.w != 0u) {
        float q = abs(input.q) < 1.0e-20 ? 1.0 : input.q;
        float2 uv = input.uv / q;
        float4 texel = SourceTexture.Sample(SourceSampler, uv);
        color = ApplyTextureFunction(color, texel, textureControl, textureEnv);
    }
    if (fogControl.w != 0u) {
        float3 fog = float3(fogControl.xyz) / 255.0;
        color.rgb = lerp(fog, color.rgb, saturate(input.fogFactor));
    }
    if (alphaControl.x != 0u) {
        uint a = (uint)floor(saturate(color.a) * 255.0 + 0.5);
        uint mask = alphaControl.w;
        if (!AlphaPass(alphaControl.y, a & mask, alphaControl.z & mask)) discard;
    }
    return QuantizeFramebuffer(color, FramebufferFormat);
}
)HLSL";
    UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    ComPtr<ID3DBlob> errors;
    HRESULT hr = D3DCompile(shader, std::strlen(shader), "VCSNativeDX12GE", nullptr, nullptr,
                            "VSMain", "vs_5_1", flags, 0u, &s.vertex_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE VS)");
        return false;
    }
    errors.Reset();
    hr = D3DCompile(shader, std::strlen(shader), "VCSNativeDX12GE", nullptr, nullptr,
                    "VSMainPacked0115", "vs_5_1", flags, 0u,
                    &s.packed_0115_vertex_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE packed 0115 VS)");
        return false;
    }
    errors.Reset();
    hr = D3DCompile(shader, std::strlen(shader), "VCSNativeDX12GE", nullptr, nullptr,
                    "PSMain", "ps_5_1", flags, 0u, &s.pixel_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE PS)");
        return false;
    }

    if (proper_shaders_private_available() && vcs_configuration().proper_shaders.enabled) {
        const auto compile_pipe = [&](GeShaderPipe pipe,
                                      bool material_shadows,
                                      ComPtr<ID3DBlob> &vs_blob,
                                      ComPtr<ID3DBlob> &packed_vs_blob,
                                      ComPtr<ID3DBlob> &ps_blob,
                                      const char *name) -> bool {
            const std::string source = proper_shaders_make_pipe_hlsl(pipe, shader, material_shadows);
            // Private material variants are optional. Do not turn conservative FXC
            // warnings into a hard pipe shutdown; the native GE shader keeps the
            // stricter warning policy above.
            constexpr UINT pipe_flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
            errors.Reset();
            HRESULT pipe_hr = D3DCompile(source.data(), source.size(), name, nullptr, nullptr,
                                         "VSMain", "vs_5_1", pipe_flags, 0u, &vs_blob, &errors);
            if (FAILED(pipe_hr)) {
                error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                               : hr_text(pipe_hr, name);
                return false;
            }
            errors.Reset();
            pipe_hr = D3DCompile(source.data(), source.size(), name, nullptr, nullptr,
                                 "VSMainPacked0115", "vs_5_1", pipe_flags, 0u,
                                 &packed_vs_blob, &errors);
            if (FAILED(pipe_hr)) {
                error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                               : hr_text(pipe_hr, name);
                return false;
            }
            errors.Reset();
            pipe_hr = D3DCompile(source.data(), source.size(), name, nullptr, nullptr,
                                 "PSMain", "ps_5_1", pipe_flags, 0u, &ps_blob, &errors);
            if (FAILED(pipe_hr)) {
                error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                               : hr_text(pipe_hr, name);
                return false;
            }
            return true;
        };
        const auto compile_optional_pipe = [&](GeShaderPipe pipe,
                                               ComPtr<ID3DBlob> &vs_blob,
                                               ComPtr<ID3DBlob> &packed_vs_blob,
                                               ComPtr<ID3DBlob> &ps_blob,
                                               const char *name) {
            std::string shadow_error;
            if (compile_pipe(pipe, true, vs_blob, packed_vs_blob, ps_blob, name)) {
                runtime_log_line(std::string("ProperShaders pipe compiled: ") + name +
                                 " shadow_receiver=1");
                return;
            }
            shadow_error = error;
            error.clear();
            vs_blob.Reset(); packed_vs_blob.Reset(); ps_blob.Reset();

            // A shadow receiver must never be allowed to kill a working material
            // pipe. Retry the same Building/Skin/Vehicle debug/custom shader with
            // the shadow extension completely absent. This reproduces the last
            // known-good pipe architecture if the receiver integration regresses.
            if (compile_pipe(pipe, false, vs_blob, packed_vs_blob, ps_blob, name)) {
                runtime_log_error(std::string("ProperShaders shadow receiver disabled; pipe recovered: ") + name,
                                  shadow_error);
                runtime_log_line(std::string("ProperShaders pipe compiled fallback: ") + name +
                                 " shadow_receiver=0");
                return;
            }
            const std::string fallback_error = error;
            error.clear();
            vs_blob.Reset(); packed_vs_blob.Reset(); ps_blob.Reset();
            runtime_log_error(std::string("optional ProperShaders pipe disabled after shadow-free retry: ") + name,
                              fallback_error);
        };
        compile_optional_pipe(GeShaderPipe::Building,
                              s.building_vertex_shader, s.building_packed_0115_vertex_shader,
                              s.building_pixel_shader, "VCSBuildingPipe");
        compile_optional_pipe(GeShaderPipe::Skin,
                              s.skin_vertex_shader, s.skin_packed_0115_vertex_shader,
                              s.skin_pixel_shader, "VCSSkinPipe");
        compile_optional_pipe(GeShaderPipe::Vehicle,
                              s.vehicle_vertex_shader, s.vehicle_packed_0115_vertex_shader,
                              s.vehicle_pixel_shader, "VCSVehiclePipe");
    }

    const char *present = proper_shaders_volumetric_clouds_hlsl();
    if (present == nullptr) {
        // Public build: keep the native fullscreen copy shader self-contained.
        present = R"HLSL(
Texture2D<float4> CloudTexture0 : register(t0);
SamplerState CloudSampler : register(s0);
struct PresentVertexOutput { float4 position : SV_POSITION; float2 uv : TEXCOORD0; };
PresentVertexOutput PresentVS(uint id : SV_VertexID) { PresentVertexOutput o; if(id==0){o.position=float4(-1,-1,0,1);o.uv=float2(0,1);} else if(id==1){o.position=float4(-1,3,0,1);o.uv=float2(0,-1);} else{o.position=float4(3,-1,0,1);o.uv=float2(2,1);} return o; }
float4 PresentPS(PresentVertexOutput i):SV_TARGET { return CloudTexture0.SampleLevel(CloudSampler,i.uv,0.0); }
float4 CloudMarchPS(PresentVertexOutput i):SV_TARGET { return float4(0,0,0,1); }
float4 CloudTemporalResolvePS(PresentVertexOutput i):SV_TARGET { return float4(0,0,0,1); }
float4 CloudCompositePS(PresentVertexOutput i):SV_TARGET { return float4(0,0,0,0); }
)HLSL";
    }
    errors.Reset();
    hr = D3DCompile(present, std::strlen(present), "VCSNativeDX12GEPresent", nullptr, nullptr,
                    "PresentVS", "vs_5_1", flags, 0u, &s.present_vertex_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE Present VS)");
        return false;
    }
    errors.Reset();
    hr = D3DCompile(present, std::strlen(present), "VCSNativeDX12GEPresent", nullptr, nullptr,
                    "PresentPS", "ps_5_1", flags, 0u, &s.present_pixel_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE Present PS)");
        return false;
    }
    errors.Reset();
    // FXC emits X4000 false positives after aggressively inlining the original
    // CloudWorks nested march/atmosphere functions (their out parameters are
    // explicitly initialized). Keep warnings-as-errors for the host shaders,
    // but compile this faithful large port with the same optimisation and let
    // genuine HLSL errors remain fatal.
    const UINT cloud_flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
    hr = D3DCompile(present, std::strlen(present), "VCSNativeDX12GECloudMarch",
                    nullptr, nullptr, "CloudMarchPS", "ps_5_1", cloud_flags, 0u,
                    &s.cloud_target_pixel_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                     errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE cloud target PS)");
        return false;
    }
    errors.Reset();
    hr = D3DCompile(present, std::strlen(present), "VCSNativeDX12GECloudTemporalResolve",
                    nullptr, nullptr, "CloudTemporalResolvePS", "ps_5_1", cloud_flags, 0u,
                    &s.cloud_resolve_pixel_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                     errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE cloud temporal resolve PS)");
        return false;
    }
    errors.Reset();
    hr = D3DCompile(present, std::strlen(present), "VCSNativeDX12GECloudComposite",
                    nullptr, nullptr, "CloudCompositePS", "ps_5_1", flags, 0u,
                    &s.cloud_composite_pixel_shader, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                     errors->GetBufferSize())
                       : hr_text(hr, "D3DCompile(DX12 GE cloud composite PS)");
        return false;
    }
    s.realtime_shadows_available = false;
    if (realtime_shadows_requested()) {
        const char *shadow = proper_shaders_realtime_shadows_hlsl();
        if (shadow != nullptr) {
            // This is an optional effect. FXC warnings in the private shader
            // must not demote the entire native GE to the software backend.
            const UINT shadow_flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
            std::string shadow_error;
            bool shadow_ok = true;
            errors.Reset();
            hr = D3DCompile(shadow, std::strlen(shadow), "VCSNativeDX12GEShadowCaster",
                            nullptr, nullptr, "ShadowCasterVS", "vs_5_1", shadow_flags, 0u,
                            &s.realtime_shadow_caster_vertex_shader, &errors);
            if (FAILED(hr)) {
                shadow_error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                                     errors->GetBufferSize())
                                      : hr_text(hr, "D3DCompile(DX12 GE shadow caster VS)");
                shadow_ok = false;
            }
            if (shadow_ok) {
                errors.Reset();
                hr = D3DCompile(shadow, std::strlen(shadow), "VCSNativeDX12GEShadowCasterPacked",
                                nullptr, nullptr, "ShadowCasterPackedVS", "vs_5_1", shadow_flags, 0u,
                                &s.realtime_shadow_caster_packed_vertex_shader, &errors);
                if (FAILED(hr)) {
                    shadow_error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                                         errors->GetBufferSize())
                                          : hr_text(hr, "D3DCompile(DX12 GE packed shadow caster VS)");
                    shadow_ok = false;
                }
            }
            if (shadow_ok) {
                errors.Reset();
                hr = D3DCompile(shadow, std::strlen(shadow), "VCSNativeDX12GEShadowCasterAlpha",
                                nullptr, nullptr, "ShadowCasterAlphaPS", "ps_5_1", shadow_flags, 0u,
                                &s.realtime_shadow_caster_alpha_pixel_shader, &errors);
                if (FAILED(hr)) {
                    shadow_error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                                        errors->GetBufferSize())
                                          : hr_text(hr, "D3DCompile(DX12 GE alpha-tested shadow caster PS)");
                    shadow_ok = false;
                }
            }
            // The old fullscreen ShadowPS/contact-composite path is deliberately
            // not compiled. Realtime shadows now consist of a depth-only caster
            // pass plus PCF sampling inside the actual material shaders.
            s.realtime_shadow_pixel_shader.Reset();
            if (shadow_ok) {
                s.realtime_shadows_available = true;
                runtime_log_line("ProperShaders realtime shadow HLSL compiled");
            } else {
                s.realtime_shadow_caster_vertex_shader.Reset();
                s.realtime_shadow_caster_packed_vertex_shader.Reset();
                s.realtime_shadow_caster_alpha_pixel_shader.Reset();
                s.realtime_shadow_pixel_shader.Reset();
                runtime_log_error("optional ProperShaders realtime shadows disabled", shadow_error);
            }
        }
    }
    return true;
}

bool create_root_signature(Dx12GeState &s, std::string &error) noexcept {
    D3D12_DESCRIPTOR_RANGE srv_range{};
    srv_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srv_range.NumDescriptors = 1u;
    srv_range.BaseShaderRegister = 0u;
    srv_range.RegisterSpace = 0u;
    srv_range.OffsetInDescriptorsFromTableStart = 0u;
    D3D12_DESCRIPTOR_RANGE sampler_range{};
    sampler_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    sampler_range.NumDescriptors = 1u;
    sampler_range.BaseShaderRegister = 0u;
    sampler_range.RegisterSpace = 0u;
    sampler_range.OffsetInDescriptorsFromTableStart = 0u;
    D3D12_DESCRIPTOR_RANGE shadow_srv_range{};
    shadow_srv_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    shadow_srv_range.NumDescriptors = 1u;
    shadow_srv_range.BaseShaderRegister = 1u;
    shadow_srv_range.RegisterSpace = 0u;
    shadow_srv_range.OffsetInDescriptorsFromTableStart = 0u;
    std::array<D3D12_ROOT_PARAMETER, 5> parameters{};
    parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[0].DescriptorTable.NumDescriptorRanges = 1u;
    parameters[0].DescriptorTable.pDescriptorRanges = &srv_range;
    parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[1].DescriptorTable.NumDescriptorRanges = 1u;
    parameters[1].DescriptorTable.pDescriptorRanges = &sampler_range;
    parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    // 40 DWORD root constants = four final clip rows + view-Z + UV + fog +
    // control + affine vertex-colour lighting. Still below the D3D12 root
    // signature 64-DWORD budget together with the descriptor tables/pixel state.
    parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[2].Constants.ShaderRegister = 0u;
    parameters[2].Constants.RegisterSpace = 0u;
    parameters[2].Constants.Num32BitValues = 40u;
    parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[3].Constants.ShaderRegister = 1u;
    parameters[3].Constants.RegisterSpace = 0u;
    // 5 draw-state DWORDs plus 16 private material DWORDs (used by the
    // model->light matrix when realtime shadows are enabled). Visibility ALL
    // lets the pipe VS consume those rows while the stock PS still sees the
    // original first five values.
    parameters[3].Constants.Num32BitValues = 21u;
    parameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    // t1 is a dedicated material-shadow SRV. A descriptor table costs one root
    // DWORD, bringing this root signature to exactly the D3D12 64-DWORD limit.
    parameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[4].DescriptorTable.NumDescriptorRanges = 1u;
    parameters[4].DescriptorTable.pDescriptorRanges = &shadow_srv_range;
    parameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(parameters.size());
    desc.pParameters = parameters.data();
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ComPtr<ID3DBlob> blob, errors;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                              &blob, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()), errors->GetBufferSize())
                       : hr_text(hr, "D3D12SerializeRootSignature(DX12 GE)");
        return false;
    }
    hr = s.device->CreateRootSignature(0u, blob->GetBufferPointer(), blob->GetBufferSize(),
                                       IID_PPV_ARGS(&s.root_signature));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateRootSignature(DX12 GE)");
        return false;
    }
    return true;
}

bool create_indirect_signatures(Dx12GeState &s, std::string &error) noexcept {
    std::array<D3D12_INDIRECT_ARGUMENT_DESC, 3> args{};
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
    args[0].Constant.RootParameterIndex = 2u;
    args[0].Constant.DestOffsetIn32BitValues = 0u;
    args[0].Constant.Num32BitValuesToSet = 40u;
    args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
    args[1].Constant.RootParameterIndex = 3u;
    args[1].Constant.DestOffsetIn32BitValues = 0u;
    // Five pixel-state DWORDs followed by the sixteen per-draw material
    // shadow-matrix DWORDs.  The direct path writes the same 21-value root
    // parameter in two calls; ExecuteIndirect must carry the complete state.
    args[1].Constant.Num32BitValuesToSet = 21u;

    D3D12_COMMAND_SIGNATURE_DESC desc{};
    desc.NumArgumentDescs = static_cast<UINT>(args.size());
    desc.pArgumentDescs = args.data();

    args[2].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
    desc.ByteStride = sizeof(Dx12IndirectDrawCommand);
    HRESULT hr = s.device->CreateCommandSignature(&desc, s.root_signature.Get(),
                                                  IID_PPV_ARGS(&s.indirect_draw_signature));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateCommandSignature(DX12 GE draw)");
        return false;
    }

    args[2].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
    desc.ByteStride = sizeof(Dx12IndirectDrawIndexedCommand);
    hr = s.device->CreateCommandSignature(&desc, s.root_signature.Get(),
                                          IID_PPV_ARGS(&s.indirect_draw_indexed_signature));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateCommandSignature(DX12 GE draw indexed)");
        s.indirect_draw_signature.Reset();
        return false;
    }
    return true;
}

bool create_targets(Dx12GeState &s, std::string &error) noexcept {
    D3D12_DESCRIPTOR_HEAP_DESC rtv_desc{};
    rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_desc.NumDescriptors = kFramebufferTargetCapacity;
    HRESULT hr = s.device->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&s.rtv_heap));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDescriptorHeap(DX12 GE RTV)"); return false; }
    s.rtv_stride = s.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC dsv_desc{};
    dsv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_desc.NumDescriptors = kFramebufferTargetCapacity;
    hr = s.device->CreateDescriptorHeap(&dsv_desc, IID_PPV_ARGS(&s.dsv_heap));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDescriptorHeap(DX12 GE DSV)"); return false; }
    s.dsv_stride = s.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_DESCRIPTOR_HEAP_DESC srv_desc{};
    srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_desc.NumDescriptors = kSrvCapacity;
    srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = s.device->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&s.srv_heap));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDescriptorHeap(DX12 GE SRV)"); return false; }
    s.srv_stride = s.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC sampler_desc{};
    sampler_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    sampler_desc.NumDescriptors = kSamplerCapacity;
    sampler_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = s.device->CreateDescriptorHeap(&sampler_desc, IID_PPV_ARGS(&s.sampler_heap));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDescriptorHeap(DX12 GE sampler)"); return false; }
    s.sampler_stride = s.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    // Descriptor zero is a valid black/null texture. Persistent framebuffer
    // targets and decoded PSP textures share the rest of the SRV heap.
    D3D12_SHADER_RESOURCE_VIEW_DESC null_srv{};
    null_srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    null_srv.Format = kColorFormat;
    null_srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    null_srv.Texture2D.MipLevels = 1u;
    s.device->CreateShaderResourceView(nullptr, &null_srv, srv_cpu(s, 0u));

    D3D12_SAMPLER_DESC default_sampler{};
    default_sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    default_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    default_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    default_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    default_sampler.MinLOD = 0.0f;
    default_sampler.MaxLOD = D3D12_FLOAT32_MAX;
    default_sampler.MaxAnisotropy = 1u;
    default_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    s.device->CreateSampler(&default_sampler, sampler_cpu(s, 0u));

    if (s.realtime_shadows_available) {
        if (s.next_dsv >= kFramebufferTargetCapacity || s.next_srv >= kSrvCapacity) {
            runtime_log_error("optional ProperShaders shadow map disabled",
                              "DX12 GE descriptor capacity exhausted");
            s.realtime_shadows_available = false;
        } else {
            const auto &shadow_config = vcs_configuration().proper_shaders.realtime_shadows;
            ShadowMapTarget shadow{};
            shadow.resolution = std::clamp<std::uint32_t>(shadow_config.map_resolution, 512u, 8192u);
            shadow.dsv_index = s.next_dsv++;
            shadow.srv_index = s.next_srv++;

            D3D12_RESOURCE_DESC shadow_desc{};
            shadow_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            shadow_desc.Width = shadow.resolution;
            shadow_desc.Height = shadow.resolution;
            shadow_desc.DepthOrArraySize = 1u;
            shadow_desc.MipLevels = 1u;
            shadow_desc.Format = DXGI_FORMAT_R32_TYPELESS;
            shadow_desc.SampleDesc.Count = 1u;
            shadow_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            shadow_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            D3D12_HEAP_PROPERTIES heap{};
            heap.Type = D3D12_HEAP_TYPE_DEFAULT;
            D3D12_CLEAR_VALUE clear{};
            clear.Format = DXGI_FORMAT_D32_FLOAT;
            clear.DepthStencil.Depth = 1.0f;
            clear.DepthStencil.Stencil = 0u;
            hr = s.device->CreateCommittedResource(
                &heap, D3D12_HEAP_FLAG_NONE, &shadow_desc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear, IID_PPV_ARGS(&shadow.depth));
            if (FAILED(hr)) {
                runtime_log_error("optional ProperShaders shadow map disabled",
                                  hr_text(hr, "CreateCommittedResource(DX12 GE realtime shadow map)"));
                s.realtime_shadows_available = false;
            } else {
                D3D12_DEPTH_STENCIL_VIEW_DESC shadow_dsv{};
                shadow_dsv.Format = DXGI_FORMAT_D32_FLOAT;
                shadow_dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                s.device->CreateDepthStencilView(shadow.depth.Get(), &shadow_dsv,
                                                 dsv_cpu(s, shadow.dsv_index));
                D3D12_SHADER_RESOURCE_VIEW_DESC shadow_srv{};
                // Map the single R32 depth component to RGB as well. Material
                // shaders still read it as Texture2D<float>, while DebugMode=1
                // can present the exact raw shadow map as grayscale without a
                // second debug-only pixel shader.
                shadow_srv.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
                    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
                    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
                    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
                    D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1);
                shadow_srv.Format = DXGI_FORMAT_R32_FLOAT;
                shadow_srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                shadow_srv.Texture2D.MipLevels = 1u;
                s.device->CreateShaderResourceView(shadow.depth.Get(), &shadow_srv,
                                                   srv_cpu(s, shadow.srv_index));
                s.realtime_shadow_map = std::move(shadow);
                runtime_log_line("ProperShaders directional shadow map created " +
                                 std::to_string(s.realtime_shadow_map.resolution) + "x" +
                                 std::to_string(s.realtime_shadow_map.resolution));
            }
        }
    }

    // Readback exists only for the validation probe / explicit diagnostics.
    // Normal gameplay goes render-target -> SRV -> swapchain entirely on GPU.
    if (s.readback_enabled) {
        D3D12_RESOURCE_DESC color{};
        color.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        color.Width = s.target_width;
        color.Height = s.target_height;
        color.DepthOrArraySize = 1u;
        color.MipLevels = 1u;
        color.Format = kColorFormat;
        color.SampleDesc.Count = 1u;
        color.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        color.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        s.device->GetCopyableFootprints(&color, 0u, 1u, 0u, &s.readback_footprint,
                                         &s.readback_rows, &s.readback_row_size, &s.readback_bytes);
        D3D12_RESOURCE_DESC readback{};
        readback.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        readback.Width = std::max<UINT64>(s.readback_bytes, 256u);
        readback.Height = 1u;
        readback.DepthOrArraySize = 1u;
        readback.MipLevels = 1u;
        readback.Format = DXGI_FORMAT_UNKNOWN;
        readback.SampleDesc.Count = 1u;
        readback.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        D3D12_HEAP_PROPERTIES readback_heap{};
        readback_heap.Type = D3D12_HEAP_TYPE_READBACK;
        hr = s.device->CreateCommittedResource(&readback_heap, D3D12_HEAP_FLAG_NONE, &readback,
                                                D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                IID_PPV_ARGS(&s.readback_buffer));
        if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 GE readback)"); return false; }
        s.frame_rgba.resize(static_cast<std::size_t>(s.target_width) * s.target_height * 4u);
    } else {
        s.frame_rgba.clear();
        s.readback_bytes = 0u;
    }

    if (proper_shaders_private_available() &&
        vcs_configuration().proper_shaders.enabled &&
        vcs_configuration().proper_shaders.volumetric_clouds.enabled) {
        const std::uint32_t divisor = std::clamp<std::uint32_t>(
            vcs_configuration().proper_shaders.volumetric_clouds.downscale_div, 1u, 8u);
        std::uint32_t cloud_width = std::max(2u, (s.target_width + divisor - 1u) / divisor);
        std::uint32_t cloud_height = std::max(2u, (s.target_height + divisor - 1u) / divisor);
        cloud_width = (cloud_width + 1u) & ~1u;
        cloud_height = (cloud_height + 1u) & ~1u;
        if (s.next_rtv + 3u > kFramebufferTargetCapacity ||
            s.next_srv + 7u > kSrvCapacity) {
            error = "DX12 GE descriptor capacity exhausted by CloudWorks temporal targets";
            return false;
        }
        D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv.Format = kCloudFormat;
        srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv.Texture2D.MipLevels = 1u;
        const auto create_cloud_target = [&](CloudRenderTarget &target,
                                             std::uint32_t width,
                                             std::uint32_t height,
                                             const char *name) -> bool {
            target.width = width;
            target.height = height;
            target.rtv_index = s.next_rtv++;
            target.srv_index = s.next_srv++;
            D3D12_RESOURCE_DESC image{};
            image.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            image.Width = width;
            image.Height = height;
            image.DepthOrArraySize = 1u;
            image.MipLevels = 1u;
            image.Format = kCloudFormat;
            image.SampleDesc.Count = 1u;
            image.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            image.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            D3D12_HEAP_PROPERTIES heap{};
            heap.Type = D3D12_HEAP_TYPE_DEFAULT;
            D3D12_CLEAR_VALUE clear{};
            clear.Format = kCloudFormat;
            // Cloud buffers store premultiplied light + remaining transmittance.
            clear.Color[3] = 1.0f;
            const HRESULT create_hr = s.device->CreateCommittedResource(
                &heap, D3D12_HEAP_FLAG_NONE, &image,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear,
                IID_PPV_ARGS(&target.image));
            if (FAILED(create_hr)) {
                error = hr_text(create_hr, name);
                return false;
            }
            s.device->CreateRenderTargetView(target.image.Get(), nullptr,
                                             rtv_cpu(s, target.rtv_index));
            s.device->CreateShaderResourceView(target.image.Get(), &srv,
                                               srv_cpu(s, target.srv_index));
            return true;
        };
        if (!create_cloud_target(s.cloud_history[0], cloud_width, cloud_height,
                                 "Create CloudWorks history 0") ||
            !create_cloud_target(s.cloud_history[1], cloud_width, cloud_height,
                                 "Create CloudWorks history 1") ||
            !create_cloud_target(s.cloud_march, cloud_width / 2u, cloud_height / 2u,
                                 "Create CloudWorks sparse march"))
            return false;
        // Resolve needs t0=march and t1=the selected previous history in one
        // contiguous descriptor table. Duplicate the SRVs for each ping-pong side.
        for (std::uint32_t index = 0u; index < 2u; ++index) {
            s.cloud_resolve_srv_base[index] = s.next_srv;
            s.device->CreateShaderResourceView(s.cloud_march.image.Get(), &srv,
                                               srv_cpu(s, s.next_srv++));
            s.device->CreateShaderResourceView(s.cloud_history[index].image.Get(), &srv,
                                               srv_cpu(s, s.next_srv++));
        }
        runtime_log_line("CloudWorks temporal buffers history=" +
                         std::to_string(cloud_width) + "x" + std::to_string(cloud_height) +
                         " march=" + std::to_string(cloud_width / 2u) + "x" +
                         std::to_string(cloud_height / 2u) + " (DownscaleDiv=" +
                         std::to_string(divisor) + ")");
    }
    return true;
}

Dx12FramebufferTarget *find_framebuffer_target(Dx12GeState &s, std::uint32_t address) noexcept {
    const auto found = s.frame_targets.find(address & 0x001FFFF0u);
    return found == s.frame_targets.end() ? nullptr : &found->second;
}

const Dx12FramebufferTarget *find_framebuffer_target(const Dx12GeState &s,
                                                      std::uint32_t address) noexcept {
    const auto found = s.frame_targets.find(address & 0x001FFFF0u);
    return found == s.frame_targets.end() ? nullptr : &found->second;
}

void note_framebuffer_logical_extent(Dx12GeState &s, std::uint32_t address,
                                     std::uint32_t width, std::uint32_t height) noexcept {
    address &= 0x001FFFF0u;
    Dx12FramebufferTarget *target = find_framebuffer_target(s, address);
    if (target == nullptr) return;
    if (address == s.display_framebuffer) {
        target->logical_width = kReferenceWidth;
        target->logical_height = kReferenceHeight;
        return;
    }
    if (width != 0u) target->logical_width = std::max(target->logical_width, width);
    if (height != 0u) target->logical_height = std::max(target->logical_height, height);
}

bool ensure_framebuffer_target(Dx12GeState &s, std::uint32_t address,
                               std::string &error) noexcept {
    address &= 0x001FFFF0u;
    if (auto *existing = find_framebuffer_target(s, address)) {
        ++s.report.dx12_framebuffer_target_hits;
        return existing->color != nullptr;
    }
    const bool shadow_depth_srv = s.realtime_shadows_available;
    const std::uint32_t required_srvs = shadow_depth_srv ? 3u : 1u;
    if (!s.device || !s.rtv_heap || !s.dsv_heap || !s.srv_heap ||
        s.next_rtv >= kFramebufferTargetCapacity || s.next_dsv >= kFramebufferTargetCapacity ||
        s.next_srv > kSrvCapacity - required_srvs) {
        error = "DX12 framebuffer target/descriptor capacity exhausted";
        return false;
    }

    Dx12FramebufferTarget target{};
    target.address = address;
    target.rtv_index = s.next_rtv++;
    target.dsv_index = s.next_dsv++;
    target.srv_index = s.next_srv++;
    if (shadow_depth_srv) {
        target.depth_srv_base = s.next_srv;
        s.next_srv += 2u;
    }

    D3D12_HEAP_PROPERTIES default_heap{};
    default_heap.Type = D3D12_HEAP_TYPE_DEFAULT;

    // The shader-readable color image is always single-sampled so texture
    // feedback and swapchain presentation never need Texture2DMS shaders.
    D3D12_RESOURCE_DESC color{};
    color.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    color.Width = s.target_width;
    color.Height = s.target_height;
    color.DepthOrArraySize = 1u;
    color.MipLevels = 1u;
    color.Format = kColorFormat;
    color.SampleDesc.Count = 1u;
    color.SampleDesc.Quality = 0u;
    color.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    color.Flags = s.sample_count == 1u ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
                                       : D3D12_RESOURCE_FLAG_NONE;
    D3D12_CLEAR_VALUE color_clear{};
    color_clear.Format = kColorFormat;
    color_clear.Color[3] = 1.0f;
    HRESULT hr = s.device->CreateCommittedResource(
        &default_heap, D3D12_HEAP_FLAG_NONE, &color,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        s.sample_count == 1u ? &color_clear : nullptr,
        IID_PPV_ARGS(&target.color));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 framebuffer color)"); return false; }

    ID3D12Resource *rtv_resource = target.color.Get();
    if (s.sample_count > 1u) {
        D3D12_RESOURCE_DESC msaa = color;
        msaa.SampleDesc.Count = s.sample_count;
        msaa.SampleDesc.Quality = s.sample_quality;
        msaa.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        hr = s.device->CreateCommittedResource(&default_heap, D3D12_HEAP_FLAG_NONE, &msaa,
                                                D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                &color_clear, IID_PPV_ARGS(&target.msaa_color));
        if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 framebuffer MSAA color)"); return false; }
        target.msaa_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
        rtv_resource = target.msaa_color.Get();
    }
    s.device->CreateRenderTargetView(rtv_resource, nullptr, rtv_cpu(s, target.rtv_index));

    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Format = kColorFormat;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = 1u;
    s.device->CreateShaderResourceView(target.color.Get(), &srv, srv_cpu(s, target.srv_index));

    D3D12_RESOURCE_DESC depth{};
    depth.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depth.Width = s.target_width;
    depth.Height = s.target_height;
    depth.DepthOrArraySize = 1u;
    depth.MipLevels = 1u;
    depth.Format = shadow_depth_srv ? typeless_depth_resource_format(s.depth_format)
                                    : s.depth_format;
    depth.SampleDesc.Count = s.sample_count;
    depth.SampleDesc.Quality = s.sample_quality;
    depth.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE depth_clear{};
    depth_clear.Format = s.depth_format;
    depth_clear.DepthStencil.Depth = 0.0f;
    depth_clear.DepthStencil.Stencil = 0u;
    hr = s.device->CreateCommittedResource(&default_heap, D3D12_HEAP_FLAG_NONE, &depth,
                                            D3D12_RESOURCE_STATE_DEPTH_WRITE, &depth_clear,
                                            IID_PPV_ARGS(&target.depth));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 framebuffer depth)"); return false; }
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv{};
    dsv.Format = s.depth_format;
    dsv.ViewDimension = s.sample_count > 1u ? D3D12_DSV_DIMENSION_TEXTURE2DMS
                                            : D3D12_DSV_DIMENSION_TEXTURE2D;
    s.device->CreateDepthStencilView(target.depth.Get(), &dsv, dsv_cpu(s, target.dsv_index));
    if (shadow_depth_srv) {
        D3D12_SHADER_RESOURCE_VIEW_DESC depth_srv{};
        depth_srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        depth_srv.Format = depth_srv_format(s.depth_format);
        depth_srv.ViewDimension = s.sample_count > 1u
            ? D3D12_SRV_DIMENSION_TEXTURE2DMS
            : D3D12_SRV_DIMENSION_TEXTURE2D;
        if (s.sample_count == 1u) depth_srv.Texture2D.MipLevels = 1u;
        s.device->CreateShaderResourceView(target.depth.Get(), &depth_srv,
                                           srv_cpu(s, target.depth_srv_base));
        // t1 is the private stable directional shadow map. Copy its descriptor
        // beside scene depth so one descriptor table binds both resources.
        if (s.realtime_shadow_map.depth) {
            s.device->CopyDescriptorsSimple(
                1u, srv_cpu(s, target.depth_srv_base + 1u),
                srv_cpu(s, s.realtime_shadow_map.srv_index),
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
    }

    target.color_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    s.frame_targets.emplace(address, std::move(target));
    s.known_frame_targets.insert(address);
    ++s.report.dx12_framebuffer_target_creates;
    s.report.framebuffer_targets_observed = s.known_frame_targets.size();
    s.report.dx12_native_framebuffer_targets = s.frame_targets.size();
    s.report.dx12_srv_high_water = std::max<std::uint64_t>(s.report.dx12_srv_high_water, s.next_srv);
    {
        std::ostringstream log;
        const auto created = s.frame_targets.find(address);
        log << "dx12 framebuffer target created address=0x" << std::hex << address
            << std::dec << " size=" << s.target_width << 'x' << s.target_height
            << " msaa=" << s.sample_count << " depth=" << s.depth_bits
            << " srv=" << (created != s.frame_targets.end() ? created->second.srv_index : 0u)
            << " depth_srv=" << (created != s.frame_targets.end()
                ? created->second.depth_srv_base : 0u);
        runtime_log_line(log.str());
    }
    return true;
}

bool ensure_feedback_copy(Dx12GeState &s, Dx12FramebufferTarget &target,
                          std::string &error) noexcept {
    if (target.feedback_copy) return true;
    if (s.next_srv >= kSrvCapacity) {
        error = "DX12 feedback SRV descriptor capacity exhausted";
        return false;
    }
    D3D12_HEAP_PROPERTIES default_heap{};
    default_heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC color{};
    color.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    color.Width = s.target_width;
    color.Height = s.target_height;
    color.DepthOrArraySize = 1u;
    color.MipLevels = 1u;
    color.Format = kColorFormat;
    color.SampleDesc.Count = 1u;
    color.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    color.Flags = D3D12_RESOURCE_FLAG_NONE;
    HRESULT hr = s.device->CreateCommittedResource(&default_heap, D3D12_HEAP_FLAG_NONE, &color,
                                                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                                    nullptr, IID_PPV_ARGS(&target.feedback_copy));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 feedback snapshot)"); return false; }
    target.feedback_srv_index = s.next_srv++;
    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Format = kColorFormat;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = 1u;
    s.device->CreateShaderResourceView(target.feedback_copy.Get(), &srv,
                                       srv_cpu(s, target.feedback_srv_index));
    target.feedback_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    return true;
}

ComPtr<ID3D12PipelineState> create_pipeline(Dx12GeState &s,
                                            const GeGpuDrawDescriptor &draw,
                                            bool packed_0115, bool cull_enabled,
                                            bool accept_counter_clockwise,
                                            std::string &error) noexcept {
    static const D3D12_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, x)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"COLOR", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, rgba)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, u)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 1u, DXGI_FORMAT_R32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, q)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"FOG", 0u, DXGI_FORMAT_R32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, fog_factor)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
    };
    static const D3D12_INPUT_ELEMENT_DESC packed_layout[] = {
        {"TEXCOORD", 2u, DXGI_FORMAT_R8G8_UINT, 0u, 0u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"COLOR", 1u, DXGI_FORMAT_R16_UINT, 0u, 2u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"POSITION", 1u, DXGI_FORMAT_R16G16_SINT, 0u, 4u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"POSITION", 2u, DXGI_FORMAT_R16_SINT, 0u, 8u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
    };
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = s.root_signature.Get();
    ID3DBlob *vs = packed_0115 ? s.packed_0115_vertex_shader.Get() : s.vertex_shader.Get();
    ID3DBlob *ps = s.pixel_shader.Get();
    if (proper_shaders_private_available()) {
        if (draw.shader_pipe == GeShaderPipe::Building && s.building_pixel_shader) {
            vs = packed_0115 ? s.building_packed_0115_vertex_shader.Get()
                             : s.building_vertex_shader.Get();
            ps = s.building_pixel_shader.Get();
        } else if (draw.shader_pipe == GeShaderPipe::Skin && s.skin_pixel_shader) {
            vs = packed_0115 ? s.skin_packed_0115_vertex_shader.Get()
                             : s.skin_vertex_shader.Get();
            ps = s.skin_pixel_shader.Get();
        } else if (draw.shader_pipe == GeShaderPipe::Vehicle && s.vehicle_pixel_shader) {
            vs = packed_0115 ? s.vehicle_packed_0115_vertex_shader.Get()
                             : s.vehicle_vertex_shader.Get();
            ps = s.vehicle_pixel_shader.Get();
        }
    }
    pso.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
    pso.PS = {ps->GetBufferPointer(), ps->GetBufferSize()};
    pso.InputLayout = packed_0115
        ? D3D12_INPUT_LAYOUT_DESC{packed_layout, static_cast<UINT>(std::size(packed_layout))}
        : D3D12_INPUT_LAYOUT_DESC{layout, static_cast<UINT>(std::size(layout))};
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = cull_enabled ? D3D12_CULL_MODE_BACK : D3D12_CULL_MODE_NONE;
    // The transform's Y row is negated to convert the PSP framebuffer origin to
    // D3D's (see the row1 construction in the constant setup).  Mirroring an axis
    // reverses the screen-space winding of every triangle, so the guest's
    // front-face bit from GE command 0x9B must be inverted here.  The software
    // rasterizer classifies winding on the CPU, before that matrix, which is why
    // only the hardware path was affected: it culled the visible faces and kept
    // the hidden ones, dropping parts of models.
    pso.RasterizerState.FrontCounterClockwise = accept_counter_clockwise ? FALSE : TRUE;
    pso.RasterizerState.DepthClipEnable = TRUE;
    pso.BlendState.AlphaToCoverageEnable = FALSE;
    pso.BlendState.IndependentBlendEnable = FALSE;
    D3D12_RENDER_TARGET_BLEND_DESC blend{};
    blend.RenderTargetWriteMask = color_write_mask(draw);
    const Dx12BlendPlan blend_plan = dx12_blend_plan(draw);
    blend.BlendEnable = blend_plan.enabled ? TRUE : FALSE;
    blend.SrcBlend = blend_plan.src;
    blend.DestBlend = blend_plan.dst;
    blend.BlendOp = blend_plan.op;
    blend.SrcBlendAlpha = blend_plan.src_alpha;
    blend.DestBlendAlpha = blend_plan.dst_alpha;
    blend.BlendOpAlpha = blend_plan.op_alpha;
    pso.BlendState.RenderTarget[0] = blend;
    pso.DepthStencilState.DepthEnable = draw.depth_test_enabled ? TRUE : FALSE;
    pso.DepthStencilState.DepthWriteMask = draw.depth_write_enabled
        ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    pso.DepthStencilState.DepthFunc = draw.depth_test_enabled
        ? depth_compare(draw.depth_function) : D3D12_COMPARISON_FUNC_ALWAYS;
    pso.DepthStencilState.StencilEnable = FALSE;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1u;
    pso.RTVFormats[0] = kColorFormat;
    pso.DSVFormat = s.depth_format;
    pso.SampleDesc.Count = s.sample_count;
    pso.SampleDesc.Quality = s.sample_quality;
    ComPtr<ID3D12PipelineState> pipeline;
    const HRESULT hr = s.device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateGraphicsPipelineState(DX12 GE)");
        return {};
    }
    return pipeline;
}

ID3D12PipelineState *pipeline_for(Dx12GeState &s, const GeGpuDrawDescriptor &draw,
                                  bool packed_0115, bool cull_enabled,
                                  bool accept_counter_clockwise, std::string &error) noexcept {
    const std::uint64_t key = pipeline_key(draw) |
        (packed_0115 ? (std::uint64_t{1} << 63u) : 0u) |
        (cull_enabled ? (std::uint64_t{1} << 62u) : 0u) |
        (accept_counter_clockwise ? (std::uint64_t{1} << 61u) : 0u);
    const auto found = s.pipelines.find(key);
    if (found != s.pipelines.end()) return found->second.Get();
    ComPtr<ID3D12PipelineState> pipeline = create_pipeline(
        s, draw, packed_0115, cull_enabled, accept_counter_clockwise, error);
    if (!pipeline) return nullptr;
    ID3D12PipelineState *raw = pipeline.Get();
    s.pipelines.emplace(key, std::move(pipeline));
    s.report.unique_pipeline_keys = s.pipelines.size();
    s.report.graphics_pipeline_created = true;
    return raw;
}

D3D12_FILTER texture_filter(const GeGpuDrawDescriptor &draw) noexcept {
    const std::uint32_t af = std::clamp(vcs_configuration().rendering.anisotropic_filtering, 1u, 16u);
    if (af > 1u && draw.texture_mipmap_enabled) return D3D12_FILTER_ANISOTROPIC;
    const bool min_linear = draw.texture_min_linear;
    const bool mag_linear = draw.texture_mag_linear;
    const bool mip_linear = draw.texture_mipmap_enabled && draw.texture_mipmap_linear;
    if (!mip_linear) {
        if (min_linear && mag_linear) return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        if (min_linear) return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        if (mag_linear) return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
    if (min_linear && mag_linear) return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    if (min_linear) return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    if (mag_linear) return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
}

std::uint64_t sampler_key(const GeGpuDrawDescriptor &draw) noexcept {
    std::uint64_t key = static_cast<std::uint64_t>(texture_filter(draw));
    key = hash_mix(key, draw.texture_clamp_u ? 1u : 0u);
    key = hash_mix(key, draw.texture_clamp_v ? 1u : 0u);
    key = hash_mix(key, draw.texture_level_mode);
    key = hash_mix(key, static_cast<std::uint32_t>(draw.texture_level_offset16));
    key = hash_mix(key, draw.texture_selected_level);
    key = hash_mix(key, draw.texture_max_level);
    return key;
}

std::uint32_t ensure_sampler(Dx12GeState &s, const GeGpuDrawDescriptor &draw) noexcept {
    const std::uint64_t key = sampler_key(draw);
    if (const auto found = s.sampler_cache.find(key); found != s.sampler_cache.end())
        return found->second;
    if (s.next_sampler >= kSamplerCapacity) return 0u;
    const std::uint32_t index = s.next_sampler++;
    D3D12_SAMPLER_DESC sampler{};
    sampler.Filter = texture_filter(draw);
    sampler.AddressU = draw.texture_clamp_u ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = draw.texture_clamp_v ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.MipLODBias = static_cast<float>(draw.texture_level_offset16) / 16.0f;
    sampler.MaxAnisotropy = std::clamp(vcs_configuration().rendering.anisotropic_filtering, 1u, 16u);
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = static_cast<float>(std::max<std::uint32_t>(1u, draw.texture_max_level + 1u));
    if (draw.texture_level_mode == 1u) {
        const float level = static_cast<float>(draw.texture_selected_level);
        sampler.MinLOD = level;
        sampler.MaxLOD = level;
    }
    s.device->CreateSampler(&sampler, sampler_cpu(s, index));
    s.sampler_cache.emplace(key, index);
    s.report.texture_samplers_created = s.sampler_cache.size() + 1u;
    return index;
}

bool create_present_pipeline(Dx12GeState &s, std::string &error) noexcept {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = s.root_signature.Get();
    pso.VS = {s.present_vertex_shader->GetBufferPointer(), s.present_vertex_shader->GetBufferSize()};
    pso.PS = {s.present_pixel_shader->GetBufferPointer(), s.present_pixel_shader->GetBufferSize()};
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.RasterizerState.DepthClipEnable = TRUE;
    pso.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.DepthStencilState.StencilEnable = FALSE;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1u;
    pso.RTVFormats[0] = kColorFormat;
    pso.SampleDesc.Count = 1u;
    const HRESULT hr = s.device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&s.present_pipeline));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateGraphicsPipelineState(DX12 GE present)");
        return false;
    }
    return true;
}

void release_swapchain_buffers(Dx12GeState &s) noexcept {
    for (auto &buffer : s.backbuffers) buffer.Reset();
}

bool create_swapchain_buffers(Dx12GeState &s, std::string &error) noexcept {
    for (UINT i = 0u; i < kFrameCount; ++i) {
        HRESULT hr = s.swapchain->GetBuffer(i, IID_PPV_ARGS(&s.backbuffers[i]));
        if (FAILED(hr)) { error = hr_text(hr, "IDXGISwapChain::GetBuffer(DX12 GE)"); return false; }
        s.device->CreateRenderTargetView(s.backbuffers[i].Get(), nullptr, swap_rtv(s, i));
    }
    return true;
}

bool ensure_swapchain(Dx12GeState &s, std::string &error) noexcept {
    if (s.native_window == nullptr) {
        error = "DX12 GE direct present has no active display window";
        return false;
    }
    RECT client{};
    if (!GetClientRect(s.native_window, &client)) {
        error = "GetClientRect failed for DX12 GE direct present";
        return false;
    }
    const std::uint32_t surface_width = static_cast<std::uint32_t>(std::max<LONG>(1, client.right - client.left));
    const std::uint32_t surface_height = static_cast<std::uint32_t>(std::max<LONG>(1, client.bottom - client.top));
    if (s.swapchain && s.swap_width == surface_width && s.swap_height == surface_height)
        return true;
    if (s.swapchain) {
        if (!wait_for_gpu(s, error)) return false;
        release_swapchain_buffers(s);
        const UINT resize_flags = s.swapchain_tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
        const HRESULT hr = s.swapchain->ResizeBuffers(kFrameCount, surface_width, surface_height,
                                                       kColorFormat, resize_flags);
        if (FAILED(hr)) { error = hr_text(hr, "IDXGISwapChain::ResizeBuffers(DX12 GE)"); return false; }
        s.swap_width = surface_width;
        s.swap_height = surface_height;
        return create_swapchain_buffers(s, error);
    }
    D3D12_DESCRIPTOR_HEAP_DESC heap{};
    heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heap.NumDescriptors = kFrameCount;
    HRESULT hr = s.device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(&s.swap_rtv_heap));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDescriptorHeap(DX12 GE swap RTV)"); return false; }
    s.swap_rtv_stride = s.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = surface_width;
    desc.Height = surface_height;
    desc.Format = kColorFormat;
    desc.SampleDesc.Count = 1u;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = kFrameCount;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    ComPtr<IDXGISwapChain1> swap1;
    hr = s.factory->CreateSwapChainForHwnd(s.queue.Get(), s.native_window,
                                            &desc, nullptr, nullptr, &swap1);
    s.swapchain_tearing = SUCCEEDED(hr);
    if (FAILED(hr)) {
        // A driver may reject tearing on a particular setup; retry without it.
        desc.Flags = 0u;
        hr = s.factory->CreateSwapChainForHwnd(s.queue.Get(), s.native_window,
                                                &desc, nullptr, nullptr, &swap1);
        s.swapchain_tearing = false;
    }
    if (FAILED(hr)) { error = hr_text(hr, "CreateSwapChainForHwnd(DX12 GE)"); return false; }
    (void)s.factory->MakeWindowAssociation(s.native_window, DXGI_MWA_NO_ALT_ENTER);
    hr = swap1.As(&s.swapchain);
    if (FAILED(hr)) { error = hr_text(hr, "Query IDXGISwapChain3(DX12 GE)"); return false; }
    s.swap_width = surface_width;
    s.swap_height = surface_height;
    if (!create_swapchain_buffers(s, error)) return false;
    s.report.swapchain_active = true;
    runtime_log_line("dx12 ge direct swapchain created " + std::to_string(surface_width) + "x" +
                     std::to_string(surface_height));
    return true;
}

std::uint32_t present_sampler(Dx12GeState &s) noexcept {
    GeGpuDrawDescriptor draw{};
    const bool linear = vcs_configuration().display.upscale_filter == DisplayUpscaleFilter::Bilinear;
    draw.texture_min_linear = linear;
    draw.texture_mag_linear = linear;
    draw.texture_clamp_u = true;
    draw.texture_clamp_v = true;
    return ensure_sampler(s, draw);
}

std::uint32_t cloud_linear_sampler(Dx12GeState &s) noexcept {
    GeGpuDrawDescriptor draw{};
    draw.texture_min_linear = true;
    draw.texture_mag_linear = true;
    draw.texture_clamp_u = true;
    draw.texture_clamp_v = true;
    return ensure_sampler(s, draw);
}

std::uint32_t shadow_point_sampler(Dx12GeState &s) noexcept {
    GeGpuDrawDescriptor draw{};
    draw.texture_min_linear = false;
    draw.texture_mag_linear = false;
    draw.texture_clamp_u = true;
    draw.texture_clamp_v = true;
    return ensure_sampler(s, draw);
}

bool create_cloud_root_signature(Dx12GeState &s, std::string &error) noexcept {
    D3D12_DESCRIPTOR_RANGE srv_range{};
    srv_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srv_range.NumDescriptors = 2u;
    srv_range.BaseShaderRegister = 0u;
    D3D12_DESCRIPTOR_RANGE sampler_range{};
    sampler_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    sampler_range.NumDescriptors = 1u;
    sampler_range.BaseShaderRegister = 0u;
    std::array<D3D12_ROOT_PARAMETER, 4> parameters{};
    parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[0].Constants.ShaderRegister = 0u;
    parameters[0].Constants.Num32BitValues = 40u;
    parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[1].DescriptorTable.NumDescriptorRanges = 1u;
    parameters[1].DescriptorTable.pDescriptorRanges = &srv_range;
    parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[2].DescriptorTable.NumDescriptorRanges = 1u;
    parameters[2].DescriptorTable.pDescriptorRanges = &sampler_range;
    parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[3].Constants.ShaderRegister = 2u;
    parameters[3].Constants.Num32BitValues = 20u;
    parameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(parameters.size());
    desc.pParameters = parameters.data();
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ComPtr<ID3DBlob> blob, errors;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                              &blob, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                     errors->GetBufferSize())
                       : hr_text(hr, "D3D12SerializeRootSignature(DX12 GE clouds)");
        return false;
    }
    hr = s.device->CreateRootSignature(0u, blob->GetBufferPointer(), blob->GetBufferSize(),
                                       IID_PPV_ARGS(&s.cloud_root_signature));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateRootSignature(DX12 GE clouds)");
        return false;
    }
    return true;
}

bool create_realtime_shadow_root_signatures(Dx12GeState &s, std::string &error) noexcept {
    if (!s.realtime_shadows_available) return true;

    // Caster root: per-draw light MVP plus GE uv scale/offset (20 floats) for
    // the VS, and the alpha-test state / cutout texture for the optional PS.
    // Parameter 0 keeps its index so the depth-only path is unchanged.
    D3D12_DESCRIPTOR_RANGE caster_srv_range{};
    caster_srv_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    caster_srv_range.NumDescriptors = 1u;
    caster_srv_range.BaseShaderRegister = 0u;
    caster_srv_range.RegisterSpace = 0u;
    caster_srv_range.OffsetInDescriptorsFromTableStart = 0u;
    D3D12_DESCRIPTOR_RANGE caster_sampler_range{};
    caster_sampler_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    caster_sampler_range.NumDescriptors = 1u;
    caster_sampler_range.BaseShaderRegister = 0u;
    caster_sampler_range.RegisterSpace = 0u;
    caster_sampler_range.OffsetInDescriptorsFromTableStart = 0u;
    std::array<D3D12_ROOT_PARAMETER, 4> caster_parameters{};
    caster_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    caster_parameters[0].Constants.ShaderRegister = 1u;
    caster_parameters[0].Constants.Num32BitValues = 20u;
    caster_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    caster_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    caster_parameters[1].Constants.ShaderRegister = 0u;
    caster_parameters[1].Constants.Num32BitValues = 1u;
    caster_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    caster_parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    caster_parameters[2].DescriptorTable.NumDescriptorRanges = 1u;
    caster_parameters[2].DescriptorTable.pDescriptorRanges = &caster_srv_range;
    caster_parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    caster_parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    caster_parameters[3].DescriptorTable.NumDescriptorRanges = 1u;
    caster_parameters[3].DescriptorTable.pDescriptorRanges = &caster_sampler_range;
    caster_parameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC caster_desc{};
    caster_desc.NumParameters = static_cast<UINT>(caster_parameters.size());
    caster_desc.pParameters = caster_parameters.data();
    caster_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    // DENY_PIXEL_SHADER_ROOT_ACCESS used to live here, from when this pass was
    // strictly depth-only. It makes every PSO carrying the alpha-test caster PS
    // fail with E_INVALIDARG, which is what silently disabled cutout shadows.
    ComPtr<ID3DBlob> blob, errors;
    HRESULT hr = D3D12SerializeRootSignature(&caster_desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                              &blob, &errors);
    if (FAILED(hr)) {
        error = errors ? std::string(static_cast<const char *>(errors->GetBufferPointer()),
                                     errors->GetBufferSize())
                       : hr_text(hr, "D3D12SerializeRootSignature(shadow caster)");
        return false;
    }
    hr = s.device->CreateRootSignature(0u, blob->GetBufferPointer(), blob->GetBufferSize(),
                                       IID_PPV_ARGS(&s.realtime_shadow_caster_root_signature));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateRootSignature(shadow caster)");
        return false;
    }

    // No fullscreen/composite root signature is created. The completed
    // directional depth map is bound as t1 on the normal GE material root.
    s.realtime_shadow_composite_root_signature.Reset();
    return true;
}

bool invert_cloud_matrix(const std::array<float, 16> &matrix,
                         std::array<double, 16> &inverse) noexcept {
    // Gauss-Jordan in double precision. GE projection matrices are small, but
    // the far plane can still make a float-only inverse needlessly fragile.
    double rows[4][8]{};
    double scale = 0.0;
    for (std::size_t row = 0u; row < 4u; ++row) {
        for (std::size_t column = 0u; column < 4u; ++column) {
            const double value = matrix[column * 4u + row];
            if (!std::isfinite(value)) return false;
            rows[row][column] = value;
            scale = std::max(scale, std::abs(value));
        }
        rows[row][4u + row] = 1.0;
    }
    if (!(scale > 0.0)) return false;
    const double epsilon = scale * 1.0e-12;
    for (std::size_t column = 0u; column < 4u; ++column) {
        std::size_t pivot = column;
        for (std::size_t row = column + 1u; row < 4u; ++row) {
            if (std::abs(rows[row][column]) > std::abs(rows[pivot][column]))
                pivot = row;
        }
        if (std::abs(rows[pivot][column]) <= epsilon) return false;
        if (pivot != column) {
            for (std::size_t entry = 0u; entry < 8u; ++entry)
                std::swap(rows[pivot][entry], rows[column][entry]);
        }
        const double divisor = rows[column][column];
        for (double &entry : rows[column]) entry /= divisor;
        for (std::size_t row = 0u; row < 4u; ++row) {
            if (row == column) continue;
            const double factor = rows[row][column];
            for (std::size_t entry = 0u; entry < 8u; ++entry)
                rows[row][entry] -= factor * rows[column][entry];
        }
    }
    std::array<double, 16> result{};
    for (std::size_t row = 0u; row < 4u; ++row) {
        for (std::size_t column = 0u; column < 4u; ++column) {
            const double value = rows[row][4u + column];
            if (!std::isfinite(value)) return false;
            result[column * 4u + row] = value;
        }
    }
    inverse = result;
    return true;
}

std::array<float, 16> shadow_affine_4x3_to_mat4(
    const std::array<float, 12> &m) noexcept {
    return {m[0], m[1], m[2], 0.0f,
            m[3], m[4], m[5], 0.0f,
            m[6], m[7], m[8], 0.0f,
            m[9], m[10], m[11], 1.0f};
}

std::array<float, 16> shadow_multiply_mat4(const std::array<float, 16> &a,
                                           const std::array<float, 16> &b) noexcept {
    std::array<float, 16> result{};
    for (std::size_t column = 0u; column < 4u; ++column) {
        for (std::size_t row = 0u; row < 4u; ++row) {
            float sum = 0.0f;
            for (std::size_t k = 0u; k < 4u; ++k)
                sum += a[k * 4u + row] * b[column * 4u + k];
            result[column * 4u + row] = sum;
        }
    }
    return result;
}

void shadow_matrix_rows(const std::array<float, 16> &matrix,
                        float *destination) noexcept {
    for (std::size_t row = 0u; row < 4u; ++row) {
        for (std::size_t column = 0u; column < 4u; ++column)
            destination[row * 4u + column] = matrix[column * 4u + row];
    }
}

bool shadow_effective_camera_vp(const CloudCameraCandidate &camera,
                                const Dx12FramebufferTarget &target,
                                std::array<float, 16> &view_projection,
                                std::array<float, 16> &inverse_view_projection) noexcept {
    const float logical_width = static_cast<float>(
        std::max<std::uint32_t>(1u, target.logical_width));
    const float logical_height = static_cast<float>(
        std::max<std::uint32_t>(1u, target.logical_height));
    const float x_a = camera.viewport[0] * (2.0f / logical_width);
    const float y_a = camera.viewport[1] * (2.0f / logical_height);
    const float z_a = camera.viewport[2] * (1.0f / 65535.0f);
    const float x_b = (camera.viewport[3] - camera.viewport[6]) *
                          (2.0f / logical_width) - 1.0f;
    const float y_b = (camera.viewport[4] - camera.viewport[7]) *
                          (2.0f / logical_height) - 1.0f;
    const float z_b = camera.viewport[5] * (1.0f / 65535.0f);
    if (!std::isfinite(x_a) || !std::isfinite(y_a) || !std::isfinite(z_a) ||
        !std::isfinite(x_b) || !std::isfinite(y_b) || !std::isfinite(z_b) ||
        std::abs(x_a) < 1.0e-7f || std::abs(y_a) < 1.0e-7f ||
        std::abs(z_a) < 1.0e-9f)
        return false;

    // Match make_transform_constants(): fold GE viewport X/Y and the PSP
    // 0..65535 depth range into the guest projection before multiplying view.
    std::array<float, 16> effective_projection{};
    for (std::size_t column = 0u; column < 4u; ++column) {
        const std::size_t base = column * 4u;
        const float w = camera.projection[base + 3u];
        effective_projection[base + 0u] =
            x_a * camera.projection[base + 0u] + x_b * w;
        effective_projection[base + 1u] =
            -y_a * camera.projection[base + 1u] - y_b * w;
        effective_projection[base + 2u] =
            z_a * camera.projection[base + 2u] + z_b * w;
        effective_projection[base + 3u] = w;
    }
    view_projection = shadow_multiply_mat4(
        effective_projection, shadow_affine_4x3_to_mat4(camera.view));
    std::array<double, 16> inverse{};
    if (!invert_cloud_matrix(view_projection, inverse)) return false;
    for (std::size_t i = 0u; i < inverse.size(); ++i) {
        if (!std::isfinite(inverse[i]) ||
            std::abs(inverse[i]) > static_cast<double>(std::numeric_limits<float>::max()))
            return false;
        inverse_view_projection[i] = static_cast<float>(inverse[i]);
    }
    return true;
}

// World-space eye position, taken from the GE view matrix this frame's draws
// actually used. No guest address is involved.
//
// The two sources tried before this both put the box in empty space. VCS has no
// documented memory map, and CloudCameraCandidate::camera_position comes from
// the hardcoded kVcsCameraPosition in ge_renderer.cpp: measured against the
// caster geometry it was ~1490 world units off, so the 2*WorldRadius box landed
// where nothing is drawn (DebugMode=1 white, DebugMode=3 red). Unprojecting the
// effective view-projection was no better, because the GE folds the PSP 0..65535
// depth range into the matrix and NDC z=0 is not the near plane.
//
// camera.view is the affine 4x3 GE view in the layout affine_4x3_to_mat4()
// expects: m[0..2], m[3..5], m[6..8] are the rotation basis columns and
// m[9..11] the translation. world->view is v = R*p + t, so the eye is -R^T * t
// whenever R is orthonormal. Reflection and camera-relative passes do not
// satisfy that, which is why orthonormality is verified instead of assumed.
bool shadow_view_eye_position(const CloudCameraCandidate &camera,
                              std::array<float, 3> &eye) noexcept {
    const std::array<float, 12> &m = camera.view;
    if (!std::all_of(m.begin(), m.end(),
                     [](float value) { return std::isfinite(value); }))
        return false;
    for (std::size_t column = 0u; column < 3u; ++column) {
        const double length2 =
            static_cast<double>(m[column * 3u + 0u]) * m[column * 3u + 0u] +
            static_cast<double>(m[column * 3u + 1u]) * m[column * 3u + 1u] +
            static_cast<double>(m[column * 3u + 2u]) * m[column * 3u + 2u];
        if (!(length2 > 0.99 && length2 < 1.01)) return false;
    }
    const std::array<float, 3> translation{m[9], m[10], m[11]};
    std::array<float, 3> candidate{};
    for (std::size_t axis = 0u; axis < 3u; ++axis) {
        candidate[axis] = -(m[axis * 3u + 0u] * translation[0] +
                            m[axis * 3u + 1u] * translation[1] +
                            m[axis * 3u + 2u] * translation[2]);
    }
    if (!std::all_of(candidate.begin(), candidate.end(),
                     [](float value) { return std::isfinite(value); }))
        return false;
    eye = candidate;
    return true;
}

bool shadow_directional_light_vp(const CloudCameraCandidate &camera,
                                 const Dx12FramebufferTarget &target,
                                 std::array<float, 16> &light_view_projection) noexcept {
    const auto &proper = vcs_configuration().proper_shaders;
    const auto &shadow = proper.realtime_shadows;
    const auto &cloud = proper.volumetric_clouds;
    std::array<float, 3> sun = shadow.use_cloud_sun_direction
        ? std::array<float, 3>{cloud.sun_direction_x, cloud.sun_direction_y,
                               cloud.sun_direction_z}
        : std::array<float, 3>{shadow.light_direction_x, shadow.light_direction_y,
                               shadow.light_direction_z};
    const auto normalize = [](std::array<float, 3> v,
                              const std::array<float, 3> &fallback) {
        const double length2 = static_cast<double>(v[0]) * v[0] +
                               static_cast<double>(v[1]) * v[1] +
                               static_cast<double>(v[2]) * v[2];
        if (!std::isfinite(length2) || length2 < 1.0e-12) return fallback;
        const float inv_length = static_cast<float>(1.0 / std::sqrt(length2));
        for (float &value : v) value *= inv_length;
        return v;
    };
    const auto cross = [](const std::array<float, 3> &a,
                          const std::array<float, 3> &b) {
        return std::array<float, 3>{a[1] * b[2] - a[2] * b[1],
                                    a[2] * b[0] - a[0] * b[2],
                                    a[0] * b[1] - a[1] * b[0]};
    };
    const auto dot = [](const std::array<float, 3> &a,
                        const std::array<float, 3> &b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    };

    sun = normalize(sun, {0.38f, -0.28f, 0.88f});
    // lightZ follows the rays from the sun toward the scene. With conventional
    // LESS depth, casters nearer the sun then have smaller depth than receivers.
    const std::array<float, 3> light_z{-sun[0], -sun[1], -sun[2]};
    const std::array<float, 3> up_seed = std::abs(light_z[2]) < 0.92f
        ? std::array<float, 3>{0.0f, 0.0f, 1.0f}
        : std::array<float, 3>{0.0f, 1.0f, 0.0f};
    const std::array<float, 3> light_x =
        normalize(cross(up_seed, light_z), {1.0f, 0.0f, 0.0f});
    const std::array<float, 3> light_y =
        normalize(cross(light_z, light_x), {0.0f, 1.0f, 0.0f});

    const float radius = std::max(20.0f, shadow.world_radius);
    const float depth_range = std::max(40.0f, shadow.depth_range);
    const float resolution = static_cast<float>(std::clamp<std::uint32_t>(
        shadow.map_resolution, 512u, 8192u));
    const float texel_world = (radius * 2.0f) / resolution;
    std::array<float, 3> center{};
    const bool center_from_view = shadow_view_eye_position(camera, center);
    if (!center_from_view) center = camera.camera_position;
    static bool center_logged = false;
    if (!center_logged) {
        center_logged = true;
        std::ostringstream line;
        line << std::fixed << std::setprecision(3)
             << "ProperShaders shadow box centre source="
             << (center_from_view ? "ge-view-matrix" : "guest-camera-position")
             << " centre=(" << center[0] << ',' << center[1] << ',' << center[2] << ')'
             << " guest_camera=(" << camera.camera_position[0] << ','
             << camera.camera_position[1] << ',' << camera.camera_position[2] << ')'
             << " radius=" << radius << " depth_range=" << depth_range
             << " sun=(" << sun[0] << ',' << sun[1] << ',' << sun[2] << ')';
        runtime_log_line(line.str());
    }
    const float center_x = std::round(dot(light_x, center) / texel_world) * texel_world;
    const float center_y = std::round(dot(light_y, center) / texel_world) * texel_world;
    const float center_z = dot(light_z, center);

    // Column-major matrix, D3D x/y in [-1,1] and depth in [0,1]. Snapping the
    // light-space X/Y origin to one shadow texel prevents camera shimmer.
    light_view_projection = {
        light_x[0] / radius, light_y[0] / radius, light_z[0] / depth_range, 0.0f,
        light_x[1] / radius, light_y[1] / radius, light_z[1] / depth_range, 0.0f,
        light_x[2] / radius, light_y[2] / radius, light_z[2] / depth_range, 0.0f,
        -center_x / radius, -center_y / radius,
        0.5f - center_z / depth_range, 1.0f};
    return std::all_of(light_view_projection.begin(), light_view_projection.end(),
                       [](float value) { return std::isfinite(value); });
}

bool create_cloud_target_pipeline(Dx12GeState &s, std::string &error) noexcept {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = s.cloud_root_signature.Get();
    pso.VS = {s.present_vertex_shader->GetBufferPointer(),
              s.present_vertex_shader->GetBufferSize()};
    pso.PS = {s.cloud_target_pixel_shader->GetBufferPointer(),
              s.cloud_target_pixel_shader->GetBufferSize()};
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.RasterizerState.DepthClipEnable = TRUE;
    auto &blend = pso.BlendState.RenderTarget[0];
    blend.BlendEnable = FALSE;
    blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.DepthStencilState.StencilEnable = FALSE;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1u;
    pso.RTVFormats[0] = kCloudFormat;
    pso.SampleDesc.Count = 1u;
    const HRESULT hr = s.device->CreateGraphicsPipelineState(
        &pso, IID_PPV_ARGS(&s.cloud_target_pipeline));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateGraphicsPipelineState(DX12 GE cloud target)");
        return false;
    }
    return true;
}

bool create_cloud_resolve_pipeline(Dx12GeState &s, std::string &error) noexcept {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = s.cloud_root_signature.Get();
    pso.VS = {s.present_vertex_shader->GetBufferPointer(),
              s.present_vertex_shader->GetBufferSize()};
    pso.PS = {s.cloud_resolve_pixel_shader->GetBufferPointer(),
              s.cloud_resolve_pixel_shader->GetBufferSize()};
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.RasterizerState.DepthClipEnable = TRUE;
    pso.BlendState.RenderTarget[0].BlendEnable = FALSE;
    pso.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1u;
    pso.RTVFormats[0] = kCloudFormat;
    pso.SampleDesc.Count = 1u;
    const HRESULT hr = s.device->CreateGraphicsPipelineState(
        &pso, IID_PPV_ARGS(&s.cloud_resolve_pipeline));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateGraphicsPipelineState(DX12 GE cloud temporal resolve)");
        return false;
    }
    return true;
}

bool create_cloud_composite_pipeline(Dx12GeState &s, std::string &error) noexcept {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = s.cloud_root_signature.Get();
    pso.VS = {s.present_vertex_shader->GetBufferPointer(),
              s.present_vertex_shader->GetBufferSize()};
    pso.PS = {s.cloud_composite_pixel_shader->GetBufferPointer(),
              s.cloud_composite_pixel_shader->GetBufferSize()};
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.RasterizerState.DepthClipEnable = TRUE;
    auto &blend = pso.BlendState.RenderTarget[0];
    blend.BlendEnable = TRUE;
    blend.SrcBlend = D3D12_BLEND_ONE;
    blend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blend.BlendOp = D3D12_BLEND_OP_ADD;
    // Alpha in the PSP world framebuffer is composition metadata, not spare
    // storage for cloud opacity. Preserve it exactly; changing it makes the
    // later framebuffer/HUD composite treat clouds as foreground content.
    blend.SrcBlendAlpha = D3D12_BLEND_ZERO;
    blend.DestBlendAlpha = D3D12_BLEND_ONE;
    blend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED |
                                  D3D12_COLOR_WRITE_ENABLE_GREEN |
                                  D3D12_COLOR_WRITE_ENABLE_BLUE;
    pso.DepthStencilState.DepthEnable = TRUE;
    pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1u;
    pso.RTVFormats[0] = kColorFormat;
    pso.DSVFormat = s.depth_format;
    pso.SampleDesc.Count = s.sample_count;
    pso.SampleDesc.Quality = s.sample_quality;
    const HRESULT hr = s.device->CreateGraphicsPipelineState(
        &pso, IID_PPV_ARGS(&s.cloud_composite_pipeline));
    if (FAILED(hr)) {
        error = hr_text(hr, "CreateGraphicsPipelineState(DX12 GE cloud composite)");
        return false;
    }
    return true;
}

bool create_realtime_shadow_pipeline(Dx12GeState &s, std::string &error) noexcept {
    if (!s.realtime_shadows_available) return true;
    if (!s.realtime_shadow_caster_root_signature ||
        !s.realtime_shadow_caster_vertex_shader ||
        !s.realtime_shadow_caster_packed_vertex_shader) {
        error = "ProperShaders directional shadow caster shaders/root signature are incomplete";
        return false;
    }

    static const D3D12_INPUT_ELEMENT_DESC caster_layout[] = {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, x)),
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u,
         static_cast<UINT>(offsetof(Dx12UploadVertex, u)),
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
    };
    static const D3D12_INPUT_ELEMENT_DESC packed_caster_layout[] = {
        {"TEXCOORD", 2u, DXGI_FORMAT_R8G8_UINT, 0u, 0u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"POSITION", 1u, DXGI_FORMAT_R16G16_SINT, 0u, 4u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
        {"POSITION", 2u, DXGI_FORMAT_R16_SINT, 0u, 8u,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
    };
    const auto &shadow_cfg = vcs_configuration().proper_shaders.realtime_shadows;
    const auto create_caster = [&](bool packed, bool alpha_tested,
                                   ID3D12PipelineState **output) -> bool {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
        pso.pRootSignature = s.realtime_shadow_caster_root_signature.Get();
        ID3DBlob *vs = packed ? s.realtime_shadow_caster_packed_vertex_shader.Get()
                              : s.realtime_shadow_caster_vertex_shader.Get();
        pso.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
        if (alpha_tested && s.realtime_shadow_caster_alpha_pixel_shader) {
            pso.PS = {s.realtime_shadow_caster_alpha_pixel_shader->GetBufferPointer(),
                      s.realtime_shadow_caster_alpha_pixel_shader->GetBufferSize()};
        }
        pso.InputLayout = packed
            ? D3D12_INPUT_LAYOUT_DESC{packed_caster_layout,
                                      static_cast<UINT>(std::size(packed_caster_layout))}
            : D3D12_INPUT_LAYOUT_DESC{caster_layout,
                                      static_cast<UINT>(std::size(caster_layout))};
        pso.SampleMask = UINT_MAX;
        pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        // Private casters deliberately render two-sided. It is more robust for
        // GTA-era thin geometry and avoids inheriting PSP winding differences.
        pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        pso.RasterizerState.DepthClipEnable = TRUE;
        // Slope-scaled bias stands in for the normal-offset bias the desktop
        // ProperShaders applies: this vertex stream has no normals, and a
        // constant bias alone cannot cover a wall lit at a grazing angle, which
        // is where the diagonal acne banding came from.
        pso.RasterizerState.DepthBias = static_cast<INT>(
            std::min<std::uint32_t>(shadow_cfg.depth_bias_constant, 100000u));
        pso.RasterizerState.SlopeScaledDepthBias = shadow_cfg.depth_bias_slope;
        pso.RasterizerState.DepthBiasClamp = 0.0f;
        pso.DepthStencilState.DepthEnable = TRUE;
        pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        pso.DepthStencilState.StencilEnable = FALSE;
        pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso.NumRenderTargets = 0u;
        pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        pso.SampleDesc.Count = 1u;
        const HRESULT hr = s.device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(output));
        if (FAILED(hr)) {
            error = hr_text(hr, packed
                ? "CreateGraphicsPipelineState(DX12 GE packed shadow caster)"
                : "CreateGraphicsPipelineState(DX12 GE shadow caster)");
            return false;
        }
        return true;
    };
    if (!create_caster(false, false, s.realtime_shadow_caster_pipeline.ReleaseAndGetAddressOf()) ||
        !create_caster(true, false, s.realtime_shadow_caster_packed_pipeline.ReleaseAndGetAddressOf()))
        return false;
    // A failed alpha variant must not take the whole shadow system down: the
    // depth-only pipelines above are already valid and render correct shadows
    // for everything except cutout materials.
    if (s.realtime_shadow_caster_alpha_pixel_shader) {
        std::string alpha_error;
        const auto create_alpha = [&](bool packed, ID3D12PipelineState **output) {
            std::string &target_error = error;
            const bool ok = create_caster(packed, true, output);
            if (!ok) { alpha_error = target_error; target_error.clear(); }
            return ok;
        };
        if (!create_alpha(false, s.realtime_shadow_caster_alpha_pipeline.ReleaseAndGetAddressOf()) ||
            !create_alpha(true, s.realtime_shadow_caster_packed_alpha_pipeline.ReleaseAndGetAddressOf())) {
            s.realtime_shadow_caster_alpha_pipeline.Reset();
            s.realtime_shadow_caster_packed_alpha_pipeline.Reset();
            runtime_log_error("optional ProperShaders alpha-tested shadow casters disabled",
                              alpha_error);
        }
    }

    s.realtime_shadow_pipeline.Reset();
    runtime_log_line("ProperShaders realtime shadows active (directional shadow map + per-material PCF receivers; fullscreen contact pass removed)");
    return true;
}

const CloudCameraCandidate *select_cloud_camera(const Dx12GeState &s) noexcept {
    if (s.display_framebuffer == 0u) return nullptr;
    std::vector<std::uint32_t> ancestors;
    ancestors.reserve(std::min<std::size_t>(s.frame_targets.size() + 1u,
                                             kFramebufferTargetCapacity));
    ancestors.push_back(s.display_framebuffer & 0x001FFFF0u);
    const auto contains = [&](std::uint32_t address) {
        address &= 0x001FFFF0u;
        return std::find(ancestors.begin(), ancestors.end(), address) != ancestors.end();
    };
    bool changed = true;
    while (changed && ancestors.size() < kFramebufferTargetCapacity) {
        changed = false;
        for (std::size_t batch_cursor = 0u; batch_cursor < s.batches.size(); ++batch_cursor) {
        const Dx12Batch &batch = s.batches[batch_cursor];
            if (!batch.framebuffer_feedback) continue;
            const std::uint32_t source = batch.feedback_address & 0x001FFFF0u;
            const std::uint32_t destination = batch.draw.framebuffer_address & 0x001FFFF0u;
            if (source == destination || !contains(destination) || contains(source)) continue;
            ancestors.push_back(source);
            changed = true;
            if (ancestors.size() == kFramebufferTargetCapacity) break;
        }
    }

    // VCS' main 3D world surface is 0x00088000. Prefer it strictly over the
    // later display/HUD target; choosing the latter makes clouds cover the HUD
    // and every transparent entity regardless of the insertion boundary.
    constexpr std::uint32_t kVcsWorldFramebuffer = 0x00088000u;
    const CloudCameraCandidate *best_world = nullptr;
    for (const CloudCameraCandidate &candidate : s.cloud_cameras) {
        if ((candidate.target & 0x001FFFF0u) != kVcsWorldFramebuffer ||
            candidate.occluding_weight == 0u)
            continue;
        if (best_world == nullptr ||
            candidate.occluding_weight > best_world->occluding_weight ||
            (candidate.occluding_weight == best_world->occluding_weight &&
             candidate.weight > best_world->weight))
            best_world = &candidate;
    }
    // Never fall back to a later framebuffer. Missing clouds are safer and
    // correctable; clouds composited over HUD/transparency are categorically
    // the wrong render phase.
    return best_world;
}

CloudShaderConstants cloud_present_constants(const Dx12GeState &s) noexcept {
    CloudShaderConstants out{};
    const auto &config = vcs_configuration().proper_shaders.volumetric_clouds;
    if (!proper_shaders_private_available() ||
        !vcs_configuration().proper_shaders.enabled ||
        !config.enabled || s.cloud_cameras.empty()) return out;
    const CloudCameraCandidate *camera = select_cloud_camera(s);
    if (camera == nullptr) return out;

    GeCloudCameraFrame frame{};
    if (!ge_cloud_camera_frame_from_view(camera->view, frame)) return out;
    const Dx12FramebufferTarget *target = find_framebuffer_target(s, camera->target);
    const float logical_width = static_cast<float>(std::max<std::uint32_t>(
        1u, target != nullptr ? target->logical_width : kReferenceWidth));
    const float logical_height = static_cast<float>(std::max<std::uint32_t>(
        1u, target != nullptr ? target->logical_height : kReferenceHeight));
    const float x_a = camera->viewport[0] * (2.0f / logical_width);
    const float y_a = camera->viewport[1] * (2.0f / logical_height);
    const float x_b = (camera->viewport[3] - camera->viewport[6]) *
                          (2.0f / logical_width) - 1.0f;
    const float y_b = (camera->viewport[4] - camera->viewport[7]) *
                          (2.0f / logical_height) - 1.0f;
    if (!std::isfinite(x_a) || !std::isfinite(y_a) ||
        std::abs(x_a) < 1.0e-6f || std::abs(y_a) < 1.0e-6f)
        return out;

    // Fold the GE viewport into projection exactly as make_transform_constants
    // does for native geometry. This eliminates the camera-relative drift that
    // came from treating raw P00/P11 as if every pass occupied the full target.
    std::array<float, 16> effective_projection{};
    for (std::size_t column = 0u; column < 4u; ++column) {
        const std::size_t base = column * 4u;
        effective_projection[base + 0u] =
            x_a * camera->projection[base + 0u] + x_b * camera->projection[base + 3u];
        effective_projection[base + 1u] =
            -y_a * camera->projection[base + 1u] - y_b * camera->projection[base + 3u];
        effective_projection[base + 2u] = camera->projection[base + 2u];
        effective_projection[base + 3u] = camera->projection[base + 3u];
    }
    std::array<double, 16> inverse_projection{};
    if (!invert_cloud_matrix(effective_projection, inverse_projection)) return out;

    const auto view_direction = [&](double ndc_x, double ndc_y,
                                    std::array<double, 3> &direction) {
        constexpr double clip_z = 0.5;
        const std::array<double, 4> clip{ndc_x, ndc_y, clip_z, 1.0};
        std::array<double, 4> point{};
        for (std::size_t row = 0u; row < 4u; ++row) {
            for (std::size_t column = 0u; column < 4u; ++column)
                point[row] += inverse_projection[column * 4u + row] * clip[column];
        }
        if (!std::isfinite(point[3]) || std::abs(point[3]) < 1.0e-12) return false;
        for (std::size_t axis = 0u; axis < 3u; ++axis) {
            direction[axis] = point[axis] / point[3];
            if (!std::isfinite(direction[axis])) return false;
        }
        return true;
    };
    std::array<double, 3> center_view{}, right_view{}, up_view{};
    if (!view_direction(0.0, 0.0, center_view) ||
        !view_direction(1.0, 0.0, right_view) ||
        !view_direction(0.0, 1.0, up_view)) return out;
    const auto to_world = [&](const std::array<double, 3> &value) {
        return std::array<double, 3>{
            frame.view_to_world[0] * value[0] + frame.view_to_world[1] * value[1] +
                frame.view_to_world[2] * value[2],
            frame.view_to_world[3] * value[0] + frame.view_to_world[4] * value[1] +
                frame.view_to_world[5] * value[2],
            frame.view_to_world[6] * value[0] + frame.view_to_world[7] * value[1] +
                frame.view_to_world[8] * value[2]};
    };
    const std::array<double, 3> center_world = to_world(center_view);
    const std::array<double, 3> right_world = to_world(right_view);
    const std::array<double, 3> up_world = to_world(up_view);
    for (std::size_t axis = 0u; axis < 3u; ++axis) {
        const double ray_right = right_world[axis] - center_world[axis];
        const double ray_up = up_world[axis] - center_world[axis];
        if (!std::isfinite(ray_right) || !std::isfinite(ray_up) ||
            !std::isfinite(center_world[axis])) return {};
        out.ray_right_time[axis] = static_cast<float>(ray_right);
        out.ray_up_seed[axis] = static_cast<float>(ray_up);
        out.ray_forward_opacity[axis] = static_cast<float>(center_world[axis]);
        out.camera_settings[axis] = camera->camera_position[axis];
    }
    // ProperShaders converts CTimer's milliseconds to seconds before setting
    // g_Time. Keep the same unit: both the stochastic march and the animated
    // density field use this value directly.
    out.ray_right_time[3] = static_cast<float>(s.frame_epoch) * (1.0f / 60.0f);
    out.ray_up_seed[3] = config.random_seed;
    out.ray_forward_opacity[3] = config.opacity;
    const std::uint32_t settings = std::clamp<std::uint32_t>(config.layers, 1u, 3u) |
        (std::clamp<std::uint32_t>(config.shadow_steps, 2u, 8u) << 8u) | 0x10000u;
    out.camera_settings[3] = std::bit_cast<float>(settings);
    out.coverage_speed = {config.coverage_low, config.coverage_mid,
                          config.coverage_high, config.speed};
    out.sun_direction_day = {config.sun_direction_x, config.sun_direction_y,
                             config.sun_direction_z, config.day_progression};
    out.sun_color_atmosphere = {config.sun_color_r, config.sun_color_g,
                                config.sun_color_b, config.atmosphere_density};
    out.cloud_color_mist = {config.cloud_base_color_r, config.cloud_base_color_g,
                            config.cloud_base_color_b, config.mist};
    out.fog_color_start = {config.fog_color_r, config.fog_color_g,
                           config.fog_color_b, config.fog_start};
    out.brightness_padding[0] = config.brightness;
    return out;
}

void record_clouds_into_world_target(Dx12GeState &s, Dx12FramebufferTarget &target,
                                     const CloudShaderConstants &clouds) noexcept {
    if (!s.cloud_history[0].image || !s.cloud_history[1].image ||
        !s.cloud_march.image || !s.cloud_target_pipeline ||
        !s.cloud_resolve_pipeline || !s.cloud_composite_pipeline)
        return;
    CloudShaderConstants draw_clouds = clouds;
    const std::uint32_t settings = std::bit_cast<std::uint32_t>(draw_clouds.camera_settings[3]);
    if ((settings & 0x10000u) == 0u) return;

    const std::uint32_t previous_index = s.cloud_history_index & 1u;
    const std::uint32_t current_index = 1u - previous_index;
    CloudRenderTarget &previous = s.cloud_history[previous_index];
    CloudRenderTarget &current = s.cloud_history[current_index];
    const auto &config = vcs_configuration().proper_shaders.volumetric_clouds;

    float camera_delta_squared = 0.0f;
    if (s.cloud_history_valid) {
        for (std::size_t axis = 0u; axis < 3u; ++axis) {
            const float delta = draw_clouds.camera_settings[axis] - s.cloud_previous_camera[axis];
            camera_delta_squared += delta * delta;
        }
    }
    // VCS' third-person camera translates while orbiting the player. Reproject
    // ordinary translation against the physical cloud slabs in the shader and
    // reset only for an actual cut/teleport (50 world units in one frame).
    const bool camera_cut = s.cloud_history_valid && camera_delta_squared > 2500.0f;
    const bool full_current_frame = !s.cloud_history_valid || camera_cut;
    for (std::size_t axis = 0u; axis < 3u; ++axis)
        draw_clouds.brightness_padding[1u + axis] = s.cloud_history_valid
            ? s.cloud_previous_camera[axis] : draw_clouds.camera_settings[axis];
    constexpr std::array<std::array<float, 2>, 4> kBayerSlots{{
        {{0.0f, 0.0f}}, {{1.0f, 1.0f}}, {{1.0f, 0.0f}}, {{0.0f, 1.0f}}}};
    const auto &subpixel = kBayerSlots[s.cloud_temporal_frame & 3u];

    CloudTemporalConstants temporal{};
    const auto copy_previous_basis = [&](std::array<float, 4> &destination,
                                         std::size_t offset) {
        for (std::size_t axis = 0u; axis < 3u; ++axis)
            destination[axis] = s.cloud_history_valid
                ? s.cloud_previous_ray_basis[offset + axis]
                : (offset == 0u ? draw_clouds.ray_right_time[axis]
                   : offset == 3u ? draw_clouds.ray_up_seed[axis]
                                  : draw_clouds.ray_forward_opacity[axis]);
    };
    copy_previous_basis(temporal.previous_right_history, 0u);
    copy_previous_basis(temporal.previous_up_blend, 3u);
    copy_previous_basis(temporal.previous_forward_spatial, 6u);
    temporal.previous_right_history[3] =
        s.cloud_history_valid && !camera_cut ? 1.0f : 0.0f;
    temporal.previous_up_blend[3] = std::clamp(config.temporal_blend, 0.0f, 0.95f);
    temporal.previous_forward_spatial[3] =
        std::clamp(config.temporal_denoise * 0.012f, 0.0f, 0.25f);
    temporal.texel_subpixel = {
        1.0f / static_cast<float>(current.width),
        1.0f / static_cast<float>(current.height), subpixel[0], subpixel[1]};
    temporal.control = {
        full_current_frame ? 1.0f : 0.0f,
        config.temporal_clamp <= 0.0f ? 1000.0f
                                      : std::clamp(config.temporal_clamp * 2.0f,
                                                   0.5f, 16.0f),
        0.0f, 0.0f};

    ID3D12DescriptorHeap *heaps[]{s.srv_heap.Get(), s.sampler_heap.Get()};
    s.list->SetDescriptorHeaps(2u, heaps);
    s.list->SetGraphicsRootSignature(s.cloud_root_signature.Get());
    s.list->SetGraphicsRoot32BitConstants(0u, 40u, &draw_clouds, 0u);
    s.list->SetGraphicsRoot32BitConstants(3u, 20u, &temporal, 0u);
    s.list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr float clear_cloud[4]{0.0f, 0.0f, 0.0f, 1.0f};
    const auto bind_cloud_target = [&](CloudRenderTarget &destination,
                                       bool clear) {
        transition(s.list.Get(), destination.image.Get(), destination.state,
                   D3D12_RESOURCE_STATE_RENDER_TARGET);
        destination.state = D3D12_RESOURCE_STATE_RENDER_TARGET;
        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtv_cpu(s, destination.rtv_index);
        s.list->OMSetRenderTargets(1u, &rtv, FALSE, nullptr);
        if (clear) s.list->ClearRenderTargetView(rtv, clear_cloud, 0u, nullptr);
        const D3D12_VIEWPORT viewport{
            0.0f, 0.0f, static_cast<float>(destination.width),
            static_cast<float>(destination.height), 0.0f, 1.0f};
        const D3D12_RECT scissor{0, 0, static_cast<LONG>(destination.width),
                                 static_cast<LONG>(destination.height)};
        s.list->RSSetViewports(1u, &viewport);
        s.list->RSSetScissorRects(1u, &scissor);
    };
    const auto make_shader_readable = [&](CloudRenderTarget &source) {
        transition(s.list.Get(), source.image.Get(), source.state,
                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        source.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    };

    if (full_current_frame) {
        bind_cloud_target(current, true);
        s.list->SetPipelineState(s.cloud_target_pipeline.Get());
        s.list->DrawInstanced(3u, 1u, 0u, 0u);
        make_shader_readable(current);
    } else {
        temporal.control[0] = 0.0f;
        s.list->SetGraphicsRoot32BitConstants(3u, 20u, &temporal, 0u);
        bind_cloud_target(s.cloud_march, true);
        s.list->SetPipelineState(s.cloud_target_pipeline.Get());
        s.list->DrawInstanced(3u, 1u, 0u, 0u);
        make_shader_readable(s.cloud_march);

        bind_cloud_target(current, true);
        s.list->SetPipelineState(s.cloud_resolve_pipeline.Get());
        s.list->SetGraphicsRootDescriptorTable(
            1u, srv_gpu(s, s.cloud_resolve_srv_base[previous_index]));
        s.list->SetGraphicsRootDescriptorTable(
            2u, sampler_gpu(s, cloud_linear_sampler(s)));
        s.list->DrawInstanced(3u, 1u, 0u, 0u);
        make_shader_readable(current);
    }

    static std::uint32_t temporal_trace_count = 0u;
    if (temporal_trace_count < 8u) {
        runtime_log_line("CLOUD_TEMPORAL frame=" + std::to_string(s.frame_epoch) +
                         " mode=" + (full_current_frame ? std::string("full")
                                                        : std::string("sparse")) +
                         " delta2=" + std::to_string(camera_delta_squared) +
                         " history=" + std::to_string(previous_index) + "->" +
                         std::to_string(current_index) + " bayer=" +
                         std::to_string(static_cast<unsigned>(subpixel[0])) + "," +
                         std::to_string(static_cast<unsigned>(subpixel[1])));
        ++temporal_trace_count;
    }

    s.cloud_history_index = current_index;
    ++s.cloud_temporal_frame;
    for (std::size_t axis = 0u; axis < 3u; ++axis) {
        s.cloud_previous_camera[axis] = draw_clouds.camera_settings[axis];
        s.cloud_previous_ray_basis[axis] = draw_clouds.ray_right_time[axis];
        s.cloud_previous_ray_basis[3u + axis] = draw_clouds.ray_up_seed[axis];
        s.cloud_previous_ray_basis[6u + axis] = draw_clouds.ray_forward_opacity[axis];
    }
    s.cloud_history_valid = true;

    prepare_target_for_render(s, target);
    const D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtv_cpu(s, target.rtv_index);
    const D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsv_cpu(s, target.dsv_index);
    s.list->OMSetRenderTargets(1u, &rtv, FALSE, &dsv);
    const D3D12_VIEWPORT viewport{
        0.0f, 0.0f, static_cast<float>(s.target_width),
        static_cast<float>(s.target_height), 0.0f, 1.0f};
    const D3D12_RECT scissor{0, 0, static_cast<LONG>(s.target_width),
                             static_cast<LONG>(s.target_height)};
    s.list->RSSetViewports(1u, &viewport);
    s.list->RSSetScissorRects(1u, &scissor);
    s.list->SetPipelineState(s.cloud_composite_pipeline.Get());
    s.list->SetGraphicsRootSignature(s.cloud_root_signature.Get());
    s.list->SetGraphicsRoot32BitConstants(0u, 40u, &draw_clouds, 0u);
    temporal.control[0] = 0.0f;
    s.list->SetGraphicsRoot32BitConstants(3u, 20u, &temporal, 0u);
    s.list->SetGraphicsRootDescriptorTable(1u, srv_gpu(s, current.srv_index));
    s.list->SetGraphicsRootDescriptorTable(
        2u, sampler_gpu(s, cloud_linear_sampler(s)));
    s.list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    s.list->DrawInstanced(3u, 1u, 0u, 0u);
}

// Defined below, next to the rest of the texture cache. The caster pass needs
// it for alpha-tested cutouts.
Dx12Texture *find_cached_texture(Dx12GeState &s, std::uint64_t key) noexcept;

bool record_directional_shadow_map(
    Dx12GeState &s, Dx12FramebufferTarget &target,
    const CloudCameraCandidate &camera,
    const D3D12_VERTEX_BUFFER_VIEW &regular_vb,
    const D3D12_VERTEX_BUFFER_VIEW &packed_vb,
    const D3D12_INDEX_BUFFER_VIEW *index_buffer,
    std::array<float, 16> &light_vp_out) noexcept {
    const auto &proper = vcs_configuration().proper_shaders;
    const auto &shadow = proper.realtime_shadows;
    if (!proper.enabled || !shadow.enabled || !proper_shaders_private_available() ||
        !s.realtime_shadows_available || !s.realtime_shadow_map.depth ||
        !s.realtime_shadow_caster_pipeline ||
        !s.realtime_shadow_caster_packed_pipeline ||
        !s.realtime_shadow_caster_root_signature)
        return false;

    s.realtime_shadow_map.caster_draws = 0u;
    if (!shadow_directional_light_vp(camera, target, light_vp_out)) return false;

    // Real shadow-map prepass. This happens before the normal world replay, so
    // Building/Skin/Vehicle material shaders can sample the completed map while
    // drawing their own surfaces. No fullscreen/contact-shadow composite is
    // involved: receivers are shaded in their actual material PS.
    transition(s.list.Get(), s.realtime_shadow_map.depth.Get(),
               s.realtime_shadow_map.state, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    s.realtime_shadow_map.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    const D3D12_CPU_DESCRIPTOR_HANDLE shadow_dsv =
        dsv_cpu(s, s.realtime_shadow_map.dsv_index);
    s.list->OMSetRenderTargets(0u, nullptr, FALSE, &shadow_dsv);
    s.list->ClearDepthStencilView(shadow_dsv, D3D12_CLEAR_FLAG_DEPTH,
                                  1.0f, 0u, 0u, nullptr);
    const float map_size = static_cast<float>(s.realtime_shadow_map.resolution);
    const D3D12_VIEWPORT shadow_viewport{0.0f, 0.0f, map_size, map_size, 0.0f, 1.0f};
    const D3D12_RECT shadow_scissor{0, 0,
        static_cast<LONG>(s.realtime_shadow_map.resolution),
        static_cast<LONG>(s.realtime_shadow_map.resolution)};
    s.list->RSSetViewports(1u, &shadow_viewport);
    s.list->RSSetScissorRects(1u, &shadow_scissor);
    s.list->SetGraphicsRootSignature(s.realtime_shadow_caster_root_signature.Get());
    if (index_buffer != nullptr) s.list->IASetIndexBuffer(index_buffer);

    // One-shot coverage probe. caster_draws counts draw *calls*, which is why
    // the pass could report 101 casters while the depth map stayed at its 1.0
    // clear: the calls were issued with a light matrix that put every triangle
    // outside the box, and DepthClipEnable threw them away. This samples the
    // same vertices the IA sees and reports how many actually land in the map.
    static bool coverage_logged = false;
    const bool probe_coverage = !coverage_logged;
    std::array<double, 3> ndc_min{std::numeric_limits<double>::max(),
                                  std::numeric_limits<double>::max(),
                                  std::numeric_limits<double>::max()};
    std::array<double, 3> ndc_max{std::numeric_limits<double>::lowest(),
                                  std::numeric_limits<double>::lowest(),
                                  std::numeric_limits<double>::lowest()};
    std::uint32_t probed_vertices = 0u;
    std::uint32_t probed_inside = 0u;
    // World-space bounds of the same samples. This is the ground truth the box
    // has to enclose, and it depends on no camera source at all.
    std::array<double, 3> world_min{std::numeric_limits<double>::max(),
                                    std::numeric_limits<double>::max(),
                                    std::numeric_limits<double>::max()};
    std::array<double, 3> world_max{std::numeric_limits<double>::lowest(),
                                    std::numeric_limits<double>::lowest(),
                                    std::numeric_limits<double>::lowest()};
    std::array<double, 3> world_sum{};

    bool bound_packed = false;
    bool vertex_view_valid = false;
    ID3D12PipelineState *caster_pipeline = nullptr;
    D3D12_PRIMITIVE_TOPOLOGY caster_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    const std::uint32_t world_target = target.address & 0x001FFFF0u;
    std::uint32_t caster_draws = 0u;
    for (const Dx12Batch &batch : s.batches) {
        if ((batch.draw.framebuffer_address & 0x001FFFF0u) != world_target ||
            batch.draw.clear_mode || !batch.hardware_transform)
            continue;
        const std::uint32_t primitive = batch.transform.primitive;
        if (primitive < 3u || primitive > 5u) continue;
        const bool semantic_caster = batch.draw.shader_pipe == GeShaderPipe::Building ||
                                     batch.draw.shader_pipe == GeShaderPipe::Skin ||
                                     batch.draw.shader_pipe == GeShaderPipe::Vehicle;
        if (!semantic_caster) continue;
        // Opaque/depth-writing world is always useful. Dynamic skin/vehicle
        // draws are allowed even when their normal pass disables depth writes.
        if (!batch.draw.depth_write_enabled &&
            batch.draw.shader_pipe == GeShaderPipe::Building)
            continue;

        // Cutout casters need the texture the material pass uses. Framebuffer
        // feedback sources are deliberately not resolved here: they are never
        // cutout materials and doing so would drag the whole self-snapshot
        // machinery into the depth pass.
        std::uint32_t caster_srv = 0u;
        std::uint32_t caster_sampler = 0u;
        if (shadow.alpha_test_casters && s.realtime_shadow_caster_alpha_pipeline &&
            batch.draw.texture_enabled && batch.draw.alpha_test_enabled &&
            !batch.framebuffer_feedback) {
            if (Dx12Texture *texture = find_cached_texture(s, texture_key(batch.draw));
                texture != nullptr && texture->image) {
                caster_srv = texture->srv_index;
                caster_sampler = texture->sampler_index;
            }
        }
        const bool caster_alpha = caster_srv != 0u;
        ID3D12PipelineState *wanted_pipeline = caster_alpha
            ? (batch.packed_0115 ? s.realtime_shadow_caster_packed_alpha_pipeline.Get()
                                 : s.realtime_shadow_caster_alpha_pipeline.Get())
            : (batch.packed_0115 ? s.realtime_shadow_caster_packed_pipeline.Get()
                                 : s.realtime_shadow_caster_pipeline.Get());
        if (caster_alpha) {
            s.list->SetGraphicsRootDescriptorTable(2u, srv_gpu(s, caster_srv));
            s.list->SetGraphicsRootDescriptorTable(3u, sampler_gpu(s, caster_sampler));
            const std::uint32_t alpha_control = packed_alpha_control(batch.draw);
            s.list->SetGraphicsRoot32BitConstants(1u, 1u, &alpha_control, 0u);
        }
        if (wanted_pipeline != caster_pipeline) {
            s.list->SetPipelineState(wanted_pipeline);
            caster_pipeline = wanted_pipeline;
        }
        if (!vertex_view_valid || bound_packed != batch.packed_0115) {
            const D3D12_VERTEX_BUFFER_VIEW &active_vb =
                batch.packed_0115 ? packed_vb : regular_vb;
            s.list->IASetVertexBuffers(0u, 1u, &active_vb);
            bound_packed = batch.packed_0115;
            vertex_view_valid = true;
        }
        const D3D12_PRIMITIVE_TOPOLOGY topology = batch_topology(batch);
        if (topology != caster_topology) {
            s.list->IASetPrimitiveTopology(topology);
            caster_topology = topology;
        }

        const std::array<float, 16> caster_mvp = shadow_multiply_mat4(
            light_vp_out, batch.transform.model_to_world);
        std::array<float, 20> caster_rows{};
        shadow_matrix_rows(caster_mvp, caster_rows.data());
        caster_rows[16] = batch.transform.uv_scale_u;
        caster_rows[17] = batch.transform.uv_scale_v;
        caster_rows[18] = batch.transform.uv_offset_u;
        caster_rows[19] = batch.transform.uv_offset_v;
        s.list->SetGraphicsRoot32BitConstants(0u, 20u, caster_rows.data(), 0u);
        if (probe_coverage && batch.vertex_count != 0u && probed_vertices < 4096u) {
            const std::uint32_t step = std::max<std::uint32_t>(1u, batch.vertex_count / 32u);
            for (std::uint32_t v = 0u; v < batch.vertex_count; v += step) {
                std::array<float, 4> model{};
                if (batch.packed_0115) {
                    const std::size_t stride = s.packed_0115_gpu_stride;
                    const std::size_t base =
                        (static_cast<std::size_t>(batch.first_vertex) + v) * stride;
                    if (base + kPacked0115GuestStride > s.packed_0115_vertices.size()) break;
                    const std::byte *source = s.packed_0115_vertices.data() + base;
                    std::array<std::int16_t, 3> raw{};
                    std::memcpy(raw.data(), source + 4u, 6u);
                    model = {static_cast<float>(raw[0]) * (1.0f / 32768.0f),
                             static_cast<float>(raw[1]) * (1.0f / 32768.0f),
                             static_cast<float>(raw[2]) * (1.0f / 32768.0f), 1.0f};
                } else {
                    const std::size_t index =
                        static_cast<std::size_t>(batch.first_vertex) + v;
                    if (index >= s.vertices.size()) break;
                    const Dx12UploadVertex &source = s.vertices[index];
                    model = {source.x, source.y, source.z, source.w};
                }
                std::array<double, 4> clip{};
                for (std::size_t row = 0u; row < 4u; ++row) {
                    clip[row] = static_cast<double>(caster_mvp[row]) * model[0] +
                                static_cast<double>(caster_mvp[4u + row]) * model[1] +
                                static_cast<double>(caster_mvp[8u + row]) * model[2] +
                                static_cast<double>(caster_mvp[12u + row]) * model[3];
                }
                if (!std::isfinite(clip[3]) || std::abs(clip[3]) < 1.0e-9) continue;
                const std::array<double, 3> ndc{clip[0] / clip[3], clip[1] / clip[3],
                                                clip[2] / clip[3]};
                if (!std::all_of(ndc.begin(), ndc.end(),
                                 [](double value) { return std::isfinite(value); }))
                    continue;
                for (std::size_t axis = 0u; axis < 3u; ++axis) {
                    ndc_min[axis] = std::min(ndc_min[axis], ndc[axis]);
                    ndc_max[axis] = std::max(ndc_max[axis], ndc[axis]);
                }
                ++probed_vertices;
                if (ndc[0] >= -1.0 && ndc[0] <= 1.0 && ndc[1] >= -1.0 && ndc[1] <= 1.0 &&
                    ndc[2] >= 0.0 && ndc[2] <= 1.0)
                    ++probed_inside;
                const std::array<float, 16> &w2 = batch.transform.model_to_world;
                for (std::size_t axis = 0u; axis < 3u; ++axis) {
                    const double world = static_cast<double>(w2[axis]) * model[0] +
                                         static_cast<double>(w2[4u + axis]) * model[1] +
                                         static_cast<double>(w2[8u + axis]) * model[2] +
                                         static_cast<double>(w2[12u + axis]) * model[3];
                    if (!std::isfinite(world)) continue;
                    world_min[axis] = std::min(world_min[axis], world);
                    world_max[axis] = std::max(world_max[axis], world);
                    world_sum[axis] += world;
                }
            }
        }
        if (batch.indexed && index_buffer != nullptr) {
            s.list->DrawIndexedInstanced(batch.index_count, 1u, batch.first_index,
                                         static_cast<INT>(batch.first_vertex), 0u);
        } else {
            s.list->DrawInstanced(batch.vertex_count, 1u, batch.first_vertex, 0u);
        }
        ++caster_draws;
    }
    transition(s.list.Get(), s.realtime_shadow_map.depth.Get(),
               s.realtime_shadow_map.state, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    s.realtime_shadow_map.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    s.realtime_shadow_map.caster_draws = caster_draws;
    if (probe_coverage && probed_vertices != 0u) {
        coverage_logged = true;
        std::ostringstream line;
        line << std::fixed << std::setprecision(3)
             << "ProperShaders shadow caster coverage draws=" << caster_draws
             << " probed_vertices=" << probed_vertices
             << " inside_box=" << probed_inside
             << " ndc_x=[" << ndc_min[0] << ',' << ndc_max[0] << ']'
             << " ndc_y=[" << ndc_min[1] << ',' << ndc_max[1] << ']'
             << " ndc_z=[" << ndc_min[2] << ',' << ndc_max[2] << ']';
        const double inv = 1.0 / static_cast<double>(probed_vertices);
        line << " world_min=(" << world_min[0] << ',' << world_min[1] << ','
             << world_min[2] << ')'
             << " world_max=(" << world_max[0] << ',' << world_max[1] << ','
             << world_max[2] << ')'
             << " world_centroid=(" << world_sum[0] * inv << ','
             << world_sum[1] * inv << ',' << world_sum[2] * inv << ')';
        runtime_log_line(line.str());
    }
    static bool logged = false;
    if (!logged) {
        logged = true;
        runtime_log_line("ProperShaders material shadow map populated casters=" +
                         std::to_string(caster_draws) +
                         " receivers=Building/Skin/Vehicle (no fullscreen contact composite)");
    }
    return caster_draws != 0u;
}

bool record_direct_present(Dx12GeState &s, Dx12FramebufferTarget &source,
                           std::string &error) noexcept {
    if (!ensure_swapchain(s, error)) return false;
    const UINT index = s.swapchain->GetCurrentBackBufferIndex();
    ID3D12Resource *backbuffer = s.backbuffers[index].Get();
    resolve_target_for_sampling(s, source, false);
    transition(s.list.Get(), backbuffer, D3D12_RESOURCE_STATE_PRESENT,
               D3D12_RESOURCE_STATE_RENDER_TARGET);
    const D3D12_CPU_DESCRIPTOR_HANDLE rtv = swap_rtv(s, index);
    s.list->OMSetRenderTargets(1u, &rtv, FALSE, nullptr);
    constexpr float black[4]{0.0f, 0.0f, 0.0f, 1.0f};
    s.list->ClearRenderTargetView(rtv, black, 0u, nullptr);
    const PresentationRectangle rect = calculate_presentation_rectangle(
        s.swap_width, s.swap_height, s.target_width, s.target_height,
        vcs_configuration().display.aspect_mode, vcs_configuration().display.integer_scale);
    D3D12_VIEWPORT viewport{static_cast<float>(rect.x), static_cast<float>(rect.y),
                            static_cast<float>(std::max(1, rect.width)),
                            static_cast<float>(std::max(1, rect.height)), 0.0f, 1.0f};
    D3D12_RECT scissor{rect.x, rect.y, rect.x + std::max(1, rect.width),
                       rect.y + std::max(1, rect.height)};
    s.list->RSSetViewports(1u, &viewport);
    s.list->RSSetScissorRects(1u, &scissor);
    s.list->SetPipelineState(s.present_pipeline.Get());
    s.list->SetGraphicsRootSignature(s.root_signature.Get());
    ID3D12DescriptorHeap *heaps[]{s.srv_heap.Get(), s.sampler_heap.Get()};
    s.list->SetDescriptorHeaps(2u, heaps);
    const auto &shadow_debug = vcs_configuration().proper_shaders.realtime_shadows;
    const bool raw_shadow_debug = vcs_configuration().proper_shaders.enabled &&
        shadow_debug.enabled && shadow_debug.debug_mode == 1u &&
        proper_shaders_private_available() && s.realtime_shadows_available &&
        s.realtime_shadow_map.depth &&
        s.realtime_shadow_map.state == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    const std::uint32_t present_srv = raw_shadow_debug
        ? s.realtime_shadow_map.srv_index : source.srv_index;
    s.list->SetGraphicsRootDescriptorTable(0u, srv_gpu(s, present_srv));
    s.list->SetGraphicsRootDescriptorTable(1u, sampler_gpu(s, present_sampler(s)));
    if (raw_shadow_debug) {
        static bool shadow_debug_logged = false;
        if (!shadow_debug_logged) {
            shadow_debug_logged = true;
            runtime_log_line("ProperShaders shadow DebugMode=1 presenting raw map " +
                             std::to_string(s.realtime_shadow_map.resolution) + "x" +
                             std::to_string(s.realtime_shadow_map.resolution) +
                             " caster_draws=" +
                             std::to_string(s.realtime_shadow_map.caster_draws));
        }
    }
    // Clouds are rendered into the selected 3D world target before VCS samples
    // it for composition. Applying them here would mix world-camera rays with
    // final-display pixels and make the layer follow the screen.
    struct LegacyCloudPresentConstants { std::array<std::uint32_t, 16> zero{}; } clouds{};
    std::array<std::uint32_t, 21> present_constants{};
    std::memcpy(present_constants.data() + 5u, &clouds, sizeof(clouds));
    s.list->SetGraphicsRoot32BitConstants(
        3u, static_cast<UINT>(present_constants.size()), present_constants.data(), 0u);
    s.list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    s.list->DrawInstanced(3u, 1u, 0u, 0u);
    transition(s.list.Get(), backbuffer, D3D12_RESOURCE_STATE_RENDER_TARGET,
               D3D12_RESOURCE_STATE_PRESENT);
    // Keep persistent framebuffer targets shader-readable between display
    // intervals. The next render pass transitions only the target it writes.
    return true;
}

Dx12Texture *find_cached_texture(Dx12GeState &s, std::uint64_t key) noexcept {
    if (s.last_texture_lookup != nullptr && s.last_texture_lookup_key == key)
        return s.last_texture_lookup;
    const auto found = s.textures.find(key);
    if (found == s.textures.end()) {
        s.last_texture_lookup = nullptr;
        s.last_texture_lookup_key = key;
        return nullptr;
    }
    s.last_texture_lookup_key = key;
    s.last_texture_lookup = &found->second;
    return s.last_texture_lookup;
}

void clear_texture_lookup_cache(Dx12GeState &s) noexcept {
    s.last_texture_lookup = nullptr;
    s.last_texture_lookup_key = 0u;
}

bool prepare_texture_upload(Dx12GeState &s, const GeGpuDrawDescriptor &draw,
                            std::uint32_t base_width, std::uint32_t base_height,
                            std::uint32_t mip_levels, std::vector<std::byte> packed) noexcept {
    if (!s.enabled || !draw.texture_enabled || base_width == 0u || base_height == 0u ||
        mip_levels == 0u || mip_levels > 8u) return false;
    std::size_t expected = 0u;
    std::uint32_t w = base_width, h = base_height;
    for (std::uint32_t level = 0u; level < mip_levels; ++level) {
        const std::uint64_t bytes = static_cast<std::uint64_t>(w) * h * 4ull;
        if (bytes > std::numeric_limits<std::size_t>::max() - expected) return false;
        expected += static_cast<std::size_t>(bytes);
        w = std::max(1u, w >> 1u);
        h = std::max(1u, h >> 1u);
    }
    if (packed.size() != expected) return false;
    const std::uint64_t key = texture_key(draw);
    const std::uint64_t checksum = fnv1a64(packed);
    if (auto found = s.textures.find(key); found != s.textures.end()) {
        found->second.signature_epoch = s.frame_epoch;
        if (found->second.checksum == checksum) {
            found->second.descriptor = draw;
            ++s.report.texture_cache_hits;
            return true;
        }
        // Do not overwrite a descriptor/resource that an in-flight frame can still reference.
        // Keep one ComPtr in *every* frame slot. Each slot drops its reference only
        // after its own fence completes, so replacing a palette/streamed texture can
        // never free the old D3D12 resource while either of the two submitted frames
        // is still sampling it.
        for (Dx12FrameResources &retire : s.frames)
            retire.transient_resources.push_back(found->second.image);
        retire_texture_srv(s, found->second.srv_index);
        s.texture_cache_bytes -= std::min<std::uint64_t>(s.texture_cache_bytes, found->second.rgba8.size());
        clear_texture_lookup_cache(s);
        s.textures.erase(found);
    }
    const std::uint32_t entry_limit = vcs_configuration().rendering.texture_cache_entries;
    const std::uint64_t byte_limit = static_cast<std::uint64_t>(vcs_configuration().rendering.texture_cache_mb) * 1024ull * 1024ull;
    // Eviction used to rescan the whole cache for a single victim, so freeing k
    // textures walked k*n unordered_map nodes. Once the cache is full -- which a
    // streaming city reaches and then stays at -- every upload paid a scan of up
    // to TextureCacheEntries nodes, with the pointer-chasing locality that implies.
    //
    // One pass now collects a batch of the coldest entries, and the loop spends
    // that batch before scanning again. Same LRU victims, amortized over many
    // evictions instead of repeated per eviction.
    constexpr std::size_t kVictimBatch = 64u;
    std::vector<std::pair<std::uint64_t, std::uint64_t>> victims; // epoch, key
    while (s.textures.size() >= entry_limit || s.texture_cache_bytes + packed.size() > byte_limit) {
        if (victims.empty()) {
            for (const auto &[key, texture] : s.textures) {
                if (texture.last_used_epoch == s.frame_epoch) continue;
                victims.emplace_back(texture.last_used_epoch, key);
            }
            if (victims.empty()) {
                ++s.report.rejected_texture_decodes;
                return false;
            }
            // Coldest first, and only the batch actually needed is ordered.
            const std::size_t keep = std::min(kVictimBatch, victims.size());
            std::partial_sort(victims.begin(), victims.begin() + keep, victims.end());
            victims.resize(keep);
            std::reverse(victims.begin(), victims.end()); // pop_back takes the coldest
        }
        const std::uint64_t key = victims.back().second;
        victims.pop_back();
        const auto victim = s.textures.find(key);
        // A candidate can be touched or replaced between passes, so re-check
        // rather than trusting the snapshot.
        if (victim == s.textures.end() || victim->second.last_used_epoch == s.frame_epoch)
            continue;
        for (Dx12FrameResources &retire : s.frames)
            retire.transient_resources.push_back(victim->second.image);
        retire_texture_srv(s, victim->second.srv_index);
        s.texture_cache_bytes -= std::min<std::uint64_t>(
            s.texture_cache_bytes, victim->second.rgba8.size());
        clear_texture_lookup_cache(s);
        s.textures.erase(victim);
        ++s.report.evicted_textures;
    }

    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = base_width;
    desc.Height = base_height;
    desc.DepthOrArraySize = 1u;
    desc.MipLevels = static_cast<UINT16>(mip_levels);
    desc.Format = kColorFormat;
    desc.SampleDesc.Count = 1u;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    Dx12Texture texture{};
    texture.descriptor = draw;
    texture.width = base_width;
    texture.height = base_height;
    texture.mip_levels = mip_levels;
    texture.checksum = checksum;
    texture.signature_epoch = s.frame_epoch;
    texture.last_used_epoch = s.frame_epoch;
    texture.rgba8 = std::move(packed);
    HRESULT hr = s.device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
                                                    D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                    IID_PPV_ARGS(&texture.image));
    if (FAILED(hr)) { runtime_log_error("dx12 texture create", hr_text(hr, "CreateCommittedResource(texture)")); return false; }

    // Stage 45.2 defers the CPU copy until the frame slot is fence-safe.
    // `rgba8` already owns the decoded mip chain, so allocating/mapping a D3D12
    // upload resource here is pure churn. record_pending_texture_uploads() copies
    // it into the persistent mapped arena of the frame being submitted.
    texture.srv_index = allocate_texture_srv(s);
    if (texture.srv_index == 0u) {
        ++s.report.rejected_texture_decodes;
        runtime_log_error("dx12 texture", "SRV descriptor heap exhausted");
        return false;
    }
    texture.sampler_index = ensure_sampler(s, draw);
    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Format = kColorFormat;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = mip_levels;
    s.device->CreateShaderResourceView(texture.image.Get(), &srv, srv_cpu(s, texture.srv_index));
    s.last_texture_rgba.assign(texture.rgba8.begin(), texture.rgba8.begin() + static_cast<std::size_t>(base_width) * base_height * 4u);
    const std::uint32_t srv_index = texture.srv_index;
    s.texture_cache_bytes += texture.rgba8.size();
    s.pending_texture_keys.push_back(key);
    s.textures.emplace(key, std::move(texture));
    ++s.report.decoded_texture_uploads;
    s.report.decoded_texture_bytes += expected;
    s.report.texture_images_created = s.textures.size();
    s.report.texture_image_uploads = s.report.decoded_texture_uploads;
    s.report.texture_image_upload_bytes += expected;
    s.report.last_texture_key = key;
    s.report.last_texture_checksum = checksum;
    s.report.last_texture_width = base_width;
    s.report.last_texture_height = base_height;
    s.report.last_texture_format = draw.texture_format;
    if (draw.texture_format == 4u) ++s.report.decoded_t4_textures;
    if (draw.texture_format == 5u) ++s.report.decoded_t8_textures;
    if (draw.texture_format <= 2u) ++s.report.decoded_direct16_textures;
    if (draw.texture_format == 3u) ++s.report.decoded_direct32_textures;
    if (draw.texture_format == 6u) ++s.report.decoded_indexed16_textures;
    if (draw.texture_format == 7u) ++s.report.decoded_indexed32_textures;
    if (draw.texture_format == 8u) ++s.report.decoded_dxt1_textures;
    if (draw.texture_format == 9u) ++s.report.decoded_dxt3_textures;
    if (draw.texture_format == 10u) ++s.report.decoded_dxt5_textures;
    if (draw.texture_format >= 8u && draw.texture_format <= 10u)
        s.report.compressed_texture_formats_active = true;
    s.report.uploaded_mip_levels += mip_levels;
    s.report.texture_descriptor_layout_created = true;
    s.report.texture_descriptor_pool_created = true;
    s.report.texture_descriptor_sets_allocated = s.textures.size();
    (void)srv_index;
    return true;
}

void record_pending_texture_uploads(Dx12GeState &s, Dx12FrameResources &frame) noexcept {
    constexpr UINT64 kPlacementAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
    const auto align_up = [](UINT64 value, UINT64 alignment) noexcept {
        return (value + alignment - 1u) & ~(alignment - 1u);
    };

    for (const std::uint64_t key : s.pending_texture_keys) {
        auto found = s.textures.find(key);
        if (found == s.textures.end()) continue;
        Dx12Texture &texture = found->second;
        if (!texture.image || texture.rgba8.empty()) continue;

        const D3D12_RESOURCE_DESC desc = texture.image->GetDesc();
        std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 8> footprints{};
        std::array<UINT, 8> rows{};
        std::array<UINT64, 8> row_sizes{};
        UINT64 upload_bytes = 0u;
        s.device->GetCopyableFootprints(&desc, 0u, texture.mip_levels, 0u,
                                         footprints.data(), rows.data(), row_sizes.data(),
                                         &upload_bytes);

        const UINT64 arena_offset = align_up(
            static_cast<UINT64>(frame.texture_upload_cursor), kPlacementAlignment);
        const bool use_arena = s.texture_upload_ring_enabled &&
            frame.texture_upload_buffer && frame.mapped_texture_upload &&
            arena_offset <= kTextureUploadCapacity &&
            upload_bytes <= kTextureUploadCapacity - arena_offset;

        ID3D12Resource *upload_resource = nullptr;
        std::byte *mapped_base = nullptr;
        UINT64 footprint_base = 0u;
        ComPtr<ID3D12Resource> fallback_upload;

        if (use_arena) {
            footprint_base = arena_offset;
            s.device->GetCopyableFootprints(&desc, 0u, texture.mip_levels, footprint_base,
                                             footprints.data(), rows.data(), row_sizes.data(),
                                             nullptr);
            upload_resource = frame.texture_upload_buffer.Get();
            mapped_base = frame.mapped_texture_upload;
            frame.texture_upload_cursor = static_cast<std::size_t>(footprint_base + upload_bytes);
        } else {
            // Extremely large streaming bursts keep the old safe behavior rather
            // than dropping a texture. This path should be rare; the normal path
            // performs zero upload-resource allocations per texture.
            D3D12_RESOURCE_DESC upload{};
            upload.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            upload.Width = std::max<UINT64>(upload_bytes, 256u);
            upload.Height = 1u;
            upload.DepthOrArraySize = 1u;
            upload.MipLevels = 1u;
            upload.Format = DXGI_FORMAT_UNKNOWN;
            upload.SampleDesc.Count = 1u;
            upload.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            D3D12_HEAP_PROPERTIES up_heap{};
            up_heap.Type = D3D12_HEAP_TYPE_UPLOAD;
            const HRESULT create_hr = s.device->CreateCommittedResource(
                &up_heap, D3D12_HEAP_FLAG_NONE, &upload,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&fallback_upload));
            if (FAILED(create_hr)) {
                runtime_log_error("dx12 texture upload",
                    hr_text(create_hr, "CreateCommittedResource(texture upload fallback)"));
                continue;
            }
            void *mapped = nullptr;
            const D3D12_RANGE no_read{0u, 0u};
            const HRESULT map_hr = fallback_upload->Map(0u, &no_read, &mapped);
            if (FAILED(map_hr) || mapped == nullptr) {
                runtime_log_error("dx12 texture upload",
                    hr_text(map_hr, "Map(texture upload fallback)"));
                continue;
            }
            upload_resource = fallback_upload.Get();
            mapped_base = static_cast<std::byte *>(mapped);
        }

        std::size_t source_offset = 0u;
        std::uint32_t w = texture.width;
        std::uint32_t h = texture.height;
        for (std::uint32_t level = 0u; level < texture.mip_levels; ++level) {
            const std::size_t row_bytes = static_cast<std::size_t>(w) * 4u;
            for (std::uint32_t y = 0u; y < h; ++y) {
                std::memcpy(mapped_base + footprints[level].Offset +
                                static_cast<std::size_t>(y) * footprints[level].Footprint.RowPitch,
                            texture.rgba8.data() + source_offset +
                                static_cast<std::size_t>(y) * row_bytes,
                            row_bytes);
            }
            source_offset += row_bytes * h;
            w = std::max(1u, w >> 1u);
            h = std::max(1u, h >> 1u);
        }

        if (fallback_upload) {
            fallback_upload->Unmap(0u, nullptr);
            frame.transient_resources.push_back(fallback_upload);
        }

        for (std::uint32_t level = 0u; level < texture.mip_levels; ++level) {
            D3D12_TEXTURE_COPY_LOCATION dst{};
            dst.pResource = texture.image.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = level;
            D3D12_TEXTURE_COPY_LOCATION src{};
            src.pResource = upload_resource;
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = footprints[level];
            s.list->CopyTextureRegion(&dst, 0u, 0u, 0u, &src, nullptr);
        }
        transition(s.list.Get(), texture.image.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        texture.pending_upload.Reset();
    }
    s.pending_texture_keys.clear();
}

GeGpuVertex screen_to_d3d(GeGpuVertex source) noexcept {
    float w = source.w;
    if (!std::isfinite(w) || std::fabs(w) < 1.0e-12f) w = 1.0f;
    source.x = (source.x * (2.0f / static_cast<float>(kReferenceWidth)) - 1.0f) * w;
    // D3D's viewport has +Y upward in NDC while the PSP/Vulkan path used -1 at
    // the top edge. Negate the Vulkan-space Y mapping here.
    source.y = (1.0f - source.y * (2.0f / static_cast<float>(kReferenceHeight))) * w;
    source.z = std::clamp(source.z / 65535.0f, 0.0f, 1.0f) * w;
    source.w = w;
    source.transform_control = 0u;
    return source;
}

float dot4(const std::array<float, 16> &m, std::size_t row,
           float x, float y, float z, float w) noexcept {
    return m[row] * x + m[4u + row] * y + m[8u + row] * z + m[12u + row] * w;
}

Dx12TransformConstants make_transform_constants(const Dx12Batch &batch,
                                                 std::uint32_t logical_width,
                                                 std::uint32_t logical_height) noexcept {
    Dx12TransformConstants constants{};
    logical_width = std::max<std::uint32_t>(1u, logical_width);
    logical_height = std::max<std::uint32_t>(1u, logical_height);
    if (!batch.hardware_transform) {
        // Mode 2 converts already viewport-transformed PSP screen coordinates
        // into D3D clip space using this framebuffer's *logical* extent.
        constants.uv = {2.0f / static_cast<float>(logical_width),
                        2.0f / static_cast<float>(logical_height),
                        1.0f / 65535.0f, 0.0f};
        constants.control = {2u, 0u, 0u, 0u};
        return constants;
    }
    const GeGpuHardwareTransform &hw = batch.transform;
    const auto row = [&](std::size_t r) {
        return std::array<float, 4>{
            hw.model_to_clip[r], hw.model_to_clip[4u + r],
            hw.model_to_clip[8u + r], hw.model_to_clip[12u + r]};
    };
    const auto add_scaled = [](const std::array<float, 4> &a, float sa,
                               const std::array<float, 4> &b, float sb) {
        return std::array<float, 4>{a[0] * sa + b[0] * sb,
                                    a[1] * sa + b[1] * sb,
                                    a[2] * sa + b[2] * sb,
                                    a[3] * sa + b[3] * sb};
    };
    const auto clip_x = row(0u);
    const auto clip_y = row(1u);
    const auto clip_z = row(2u);
    const auto clip_w = row(3u);
    const float x_a = hw.viewport_scale_x * (2.0f / static_cast<float>(logical_width));
    const float x_b = (hw.viewport_center_x - hw.viewport_offset_x) *
                      (2.0f / static_cast<float>(logical_width)) - 1.0f;
    const float y_a = hw.viewport_scale_y * (2.0f / static_cast<float>(logical_height));
    const float y_b = (hw.viewport_center_y - hw.viewport_offset_y) *
                      (2.0f / static_cast<float>(logical_height)) - 1.0f;
    constexpr float inv_depth = 1.0f / 65535.0f;
    const float z_a = hw.viewport_scale_z * inv_depth;
    const float z_b = hw.viewport_center_z * inv_depth;
    constants.row0 = add_scaled(clip_x, x_a, clip_w, x_b);
    constants.row1 = add_scaled(clip_y, -y_a, clip_w, -y_b);
    constants.row2 = add_scaled(clip_z, z_a, clip_w, z_b);
    constants.row3 = clip_w;
    constants.view_z = hw.model_to_view_z;
    constants.uv = {hw.uv_scale_u, hw.uv_scale_v, hw.uv_offset_u, hw.uv_offset_v};
    // Extending world LOD/far clip without stretching the PSP fog range can make
    // the extra submitted geometry mathematically present but visually identical:
    // it is already fully blended into the fog colour at the stock distance. Scale
    // the hardware fog curve in the same distance domain as DrawDistance.World.
    // For factor = (viewZ + end) * slope, end*=m and slope/=m moves both fog
    // start and end outward by m while preserving the original curve shape.
    float fog_distance_scale = g_draw_distance_runtime_scales.world;
    if (!std::isfinite(fog_distance_scale) || fog_distance_scale < 1.0f)
        fog_distance_scale = 1.0f;
    constants.fog = {hw.fog_end * fog_distance_scale,
                     hw.fog_slope / fog_distance_scale, 0.0f, 0.0f};
    constants.control = {1u, hw.depth_clip_enabled ? 1u : 0u,
                         hw.vertex_color_affine ? 1u : 0u, 0u};
    constants.color_mul = hw.vertex_color_mul;
    constants.color_add = hw.vertex_color_add;
    return constants;
}

GeGpuVertex model_to_d3d(GeGpuVertex source, const GeGpuHardwareTransform &t) noexcept {
    const float x = source.x, y = source.y, z = source.z, w = source.w;
    const float cx = dot4(t.model_to_clip, 0u, x, y, z, w);
    const float cy = dot4(t.model_to_clip, 1u, x, y, z, w);
    float cz = dot4(t.model_to_clip, 2u, x, y, z, w);
    float cw = dot4(t.model_to_clip, 3u, x, y, z, w);
    if (!std::isfinite(cw) || std::fabs(cw) < 1.0e-12f) cw = 1.0f;
    const float x_a = t.viewport_scale_x * (2.0f / static_cast<float>(kReferenceWidth));
    const float x_b = (t.viewport_center_x - t.viewport_offset_x) *
                      (2.0f / static_cast<float>(kReferenceWidth)) - 1.0f;
    const float y_a = t.viewport_scale_y * (2.0f / static_cast<float>(kReferenceHeight));
    const float y_b = (t.viewport_center_y - t.viewport_offset_y) *
                      (2.0f / static_cast<float>(kReferenceHeight)) - 1.0f;
    constexpr float inv_depth = 1.0f / 65535.0f;
    const float z_a = t.viewport_scale_z * inv_depth;
    const float z_b = t.viewport_center_z * inv_depth;
    source.x = cx * x_a + cw * x_b;
    source.y = -(cy * y_a + cw * y_b);
    source.z = cz * z_a + cw * z_b;
    source.w = cw;
    if (!t.depth_clip_enabled)
        source.z = std::clamp(source.z, 0.0f, cw);
    const float view_z = t.model_to_view_z[0] * x + t.model_to_view_z[1] * y +
                         t.model_to_view_z[2] * z + t.model_to_view_z[3] * w;
    source.fog_factor = std::clamp((view_z + t.fog_end) * t.fog_slope, 0.0f, 1.0f);
    source.u = source.u * t.uv_scale_u + t.uv_offset_u;
    source.v = source.v * t.uv_scale_v + t.uv_offset_v;
    source.transform_control = 0u;
    return source;
}

void clear_accumulation(Dx12GeState &s) noexcept {
    s.vertices.clear();
    s.packed_0115_vertices.clear();
    s.indices.clear();
    s.batches.clear();
    s.cloud_cameras.clear();
}

bool create_backend(Dx12GeState &s, std::string &error) noexcept {
    const InternalResolutionDimensions dims = resolve_internal_resolution(vcs_configuration().rendering);
    s.target_width = std::max<std::uint32_t>(1u, dims.width);
    s.target_height = std::max<std::uint32_t>(1u, dims.height);
    const char *readback = std::getenv("PSPRECOMP_DX12_GE_READBACK");
    s.readback_enabled = readback != nullptr && *readback != '\0' && *readback != '0';
    if (const char *ring = std::getenv("PSPRECOMP_DX12_TEXTURE_UPLOAD_RING");
        ring != nullptr && *ring != '\0') {
        s.texture_upload_ring_enabled = *ring != '0' &&
            std::strcmp(ring, "false") != 0 && std::strcmp(ring, "FALSE") != 0 &&
            std::strcmp(ring, "off") != 0 && std::strcmp(ring, "OFF") != 0;
    } else {
        s.texture_upload_ring_enabled = true;
    }
    HRESULT hr = CreateDXGIFactory2(0u, IID_PPV_ARGS(&s.factory));
    if (FAILED(hr)) { error = hr_text(hr, "CreateDXGIFactory2(DX12 GE)"); return false; }
    if (!select_adapter(s, error)) return false;
    hr = D3D12CreateDevice(s.adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s.device));
    if (FAILED(hr)) { error = hr_text(hr, "D3D12CreateDevice(DX12 GE)"); return false; }
    detect_adapter_architecture_and_compat(s);
    select_depth_and_msaa(s);
    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    hr = s.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&s.queue));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommandQueue(DX12 GE)"); return false; }

    D3D12_HEAP_PROPERTIES upload_heap{};
    upload_heap.Type = D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC upload{};
    upload.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    upload.Width = kGeometryUploadCapacity;
    upload.Height = 1u;
    upload.DepthOrArraySize = 1u;
    upload.MipLevels = 1u;
    upload.Format = DXGI_FORMAT_UNKNOWN;
    upload.SampleDesc.Count = 1u;
    upload.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    for (Dx12FrameResources &frame : s.frames) {
        hr = s.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.allocator));
        if (FAILED(hr)) { error = hr_text(hr, "CreateCommandAllocator(DX12 GE frame)"); return false; }
        hr = s.device->CreateCommittedResource(&upload_heap, D3D12_HEAP_FLAG_NONE, &upload,
                                                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                IID_PPV_ARGS(&frame.upload_buffer));
        if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 GE geometry upload)"); return false; }
        void *mapped = nullptr;
        const D3D12_RANGE no_read{0u, 0u};
        hr = frame.upload_buffer->Map(0u, &no_read, &mapped);
        if (FAILED(hr) || mapped == nullptr) { error = hr_text(hr, "Map(DX12 GE geometry upload)"); return false; }
        frame.mapped_upload = static_cast<std::byte *>(mapped);

        // Optional V4 arena. Failure here must never make the renderer fail to
        // boot on an older/low-memory driver; that frame simply uses scalar
        // Draw*/root-constant recording.
        D3D12_RESOURCE_DESC indirect_upload = upload;
        indirect_upload.Width = kIndirectUploadCapacity;
        hr = s.device->CreateCommittedResource(&upload_heap, D3D12_HEAP_FLAG_NONE, &indirect_upload,
                                                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                IID_PPV_ARGS(&frame.indirect_upload_buffer));
        if (SUCCEEDED(hr) && frame.indirect_upload_buffer) {
            mapped = nullptr;
            hr = frame.indirect_upload_buffer->Map(0u, &no_read, &mapped);
            if (SUCCEEDED(hr) && mapped != nullptr) {
                frame.mapped_indirect_upload = static_cast<std::byte *>(mapped);
            } else {
                frame.indirect_upload_buffer.Reset();
                frame.mapped_indirect_upload = nullptr;
            }
        }

        if (s.texture_upload_ring_enabled) {
            D3D12_RESOURCE_DESC texture_upload = upload;
            texture_upload.Width = kTextureUploadCapacity;
            hr = s.device->CreateCommittedResource(&upload_heap, D3D12_HEAP_FLAG_NONE, &texture_upload,
                                                    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                    IID_PPV_ARGS(&frame.texture_upload_buffer));
            if (FAILED(hr)) { error = hr_text(hr, "CreateCommittedResource(DX12 texture upload arena)"); return false; }
            mapped = nullptr;
            hr = frame.texture_upload_buffer->Map(0u, &no_read, &mapped);
            if (FAILED(hr) || mapped == nullptr) { error = hr_text(hr, "Map(DX12 texture upload arena)"); return false; }
            frame.mapped_texture_upload = static_cast<std::byte *>(mapped);
        }
        frame.texture_upload_cursor = 0u;
        frame.transient_resources.reserve(128u);
    }
    hr = s.device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, s.frames[0].allocator.Get(), nullptr,
                                     IID_PPV_ARGS(&s.list));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommandList(DX12 GE)"); return false; }
    s.list->Close();

    hr = s.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&s.texture_allocator));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommandAllocator(DX12 GE texture)"); return false; }
    hr = s.device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, s.texture_allocator.Get(), nullptr,
                                     IID_PPV_ARGS(&s.texture_list));
    if (FAILED(hr)) { error = hr_text(hr, "CreateCommandList(DX12 GE texture)"); return false; }
    s.texture_list->Close();

    hr = s.device->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&s.fence));
    if (FAILED(hr)) { error = hr_text(hr, "CreateFence(DX12 GE)"); return false; }
    s.fence_event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (s.fence_event == nullptr) { error = "CreateEventW failed for DX12 GE fence"; return false; }
    if (!compile_shaders(s, error)) return false;
    if (!create_root_signature(s, error)) return false;
    {
        std::string indirect_error;
        if (!create_indirect_signatures(s, indirect_error)) {
            // Scalar recording is the fully supported fallback. Command
            // signatures are an optimization, never a backend requirement.
            s.indirect_draw_signature.Reset();
            s.indirect_draw_indexed_signature.Reset();
            if (!indirect_error.empty()) runtime_log_error("dx12 execute indirect disabled", indirect_error);
        }
    }
    if (!create_cloud_root_signature(s, error)) return false;
    if (s.realtime_shadows_available) {
        std::string shadow_error;
        if (!create_realtime_shadow_root_signatures(s, shadow_error)) {
            runtime_log_error("optional ProperShaders shadow root signature disabled", shadow_error);
            s.realtime_shadows_available = false;
            s.realtime_shadow_caster_root_signature.Reset();
            s.realtime_shadow_composite_root_signature.Reset();
        }
    }
    if (!create_targets(s, error)) return false;
    if (!create_present_pipeline(s, error)) return false;
    if (!create_cloud_target_pipeline(s, error)) return false;
    if (!create_cloud_resolve_pipeline(s, error)) return false;
    if (!create_cloud_composite_pipeline(s, error)) return false;
    if (s.realtime_shadows_available) {
        std::string shadow_error;
        if (!create_realtime_shadow_pipeline(s, shadow_error)) {
            runtime_log_error("optional ProperShaders shadow PSO disabled", shadow_error);
            s.realtime_shadows_available = false;
            s.realtime_shadow_caster_pipeline.Reset();
            s.realtime_shadow_caster_packed_pipeline.Reset();
            s.realtime_shadow_caster_alpha_pipeline.Reset();
            s.realtime_shadow_caster_packed_alpha_pipeline.Reset();
            s.realtime_shadow_pipeline.Reset();
            s.realtime_shadow_map = {};
        }
    }
    s.vertices.reserve(262144u);
    s.packed_0115_vertices.reserve(2621440u);
    s.indices.reserve(524288u);
    s.batches.reserve(4096u);
    // Stage 45.2: reserve hot node maps up front. City streaming otherwise can
    // grow buckets while the GE thread is preparing a frame. References to
    // unordered_map nodes remain valid across rehash, but avoiding rehash also
    // reduces allocator/driver-thread noise.
    s.textures.reserve(std::max<std::uint32_t>(256u,
        vcs_configuration().rendering.texture_cache_entries));
    s.frame_targets.reserve(64u);
    s.pipelines.reserve(512u);
    s.sampler_cache.reserve(64u);
    s.pending_texture_keys.reserve(256u);
    s.free_texture_srvs.reserve(1024u);
    s.retired_texture_srvs.reserve(256u);
    return true;
}

void destroy_backend(Dx12GeState &s) noexcept {
    std::string ignored;
    if (s.queue && s.fence) (void)wait_for_gpu(s, ignored);
    for (Dx12FrameResources &frame : s.frames) {
        if (frame.upload_buffer && frame.mapped_upload != nullptr)
            frame.upload_buffer->Unmap(0u, nullptr);
        if (frame.indirect_upload_buffer && frame.mapped_indirect_upload != nullptr)
            frame.indirect_upload_buffer->Unmap(0u, nullptr);
        if (frame.texture_upload_buffer && frame.mapped_texture_upload != nullptr)
            frame.texture_upload_buffer->Unmap(0u, nullptr);
        frame.mapped_upload = nullptr;
        frame.mapped_indirect_upload = nullptr;
        frame.mapped_texture_upload = nullptr;
        frame.transient_resources.clear();
        frame.indirect_upload_buffer.Reset();
        frame.texture_upload_buffer.Reset();
        frame.upload_buffer.Reset();
        frame.texture_upload_cursor = 0u;
        frame.allocator.Reset();
        frame.fence_value = 0u;
    }
    release_swapchain_buffers(s);
    s.swapchain.Reset();
    s.swap_rtv_heap.Reset();
    s.present_pipeline.Reset();
    s.cloud_target_pipeline.Reset();
    s.cloud_resolve_pipeline.Reset();
    s.cloud_composite_pipeline.Reset();
    s.realtime_shadow_caster_pipeline.Reset();
    s.realtime_shadow_caster_packed_pipeline.Reset();
    s.realtime_shadow_caster_alpha_pipeline.Reset();
    s.realtime_shadow_caster_packed_alpha_pipeline.Reset();
    s.realtime_shadow_pipeline.Reset();
    s.realtime_shadow_map = {};
    s.cloud_target_pixel_shader.Reset();
    s.cloud_resolve_pixel_shader.Reset();
    s.cloud_composite_pixel_shader.Reset();
    s.cloud_history = {};
    s.cloud_march = {};
    s.cloud_resolve_srv_base = {};
    s.cloud_history_index = 0u;
    s.cloud_temporal_frame = 0u;
    s.cloud_previous_camera = {};
    s.cloud_previous_ray_basis = {};
    s.cloud_history_valid = false;
    s.present_pixel_shader.Reset();
    s.present_vertex_shader.Reset();
    s.pipelines.clear();
    s.textures.clear();
    s.pending_texture_keys.clear();
    s.free_texture_srvs.clear();
    s.retired_texture_srvs.clear();
    s.sampler_cache.clear();
    s.known_frame_targets.clear();
    s.last_registered_framebuffer_target = 0xFFFFFFFFu;
    s.pixel_shader.Reset();
    s.building_vertex_shader.Reset();
    s.building_packed_0115_vertex_shader.Reset();
    s.building_pixel_shader.Reset();
    s.skin_vertex_shader.Reset();
    s.skin_packed_0115_vertex_shader.Reset();
    s.skin_pixel_shader.Reset();
    s.vehicle_vertex_shader.Reset();
    s.vehicle_packed_0115_vertex_shader.Reset();
    s.vehicle_pixel_shader.Reset();
    s.realtime_shadow_caster_vertex_shader.Reset();
    s.realtime_shadow_caster_packed_vertex_shader.Reset();
    s.realtime_shadow_caster_alpha_pixel_shader.Reset();
    s.realtime_shadow_pixel_shader.Reset();
    s.packed_0115_vertex_shader.Reset();
    s.vertex_shader.Reset();
    s.indirect_draw_signature.Reset();
    s.indirect_draw_indexed_signature.Reset();
    s.root_signature.Reset();
    s.cloud_root_signature.Reset();
    s.realtime_shadow_caster_root_signature.Reset();
    s.realtime_shadow_composite_root_signature.Reset();
    s.readback_buffer.Reset();
    s.frame_targets.clear();
    s.sampler_heap.Reset();
    s.srv_heap.Reset();
    s.dsv_heap.Reset();
    s.rtv_heap.Reset();
    s.texture_list.Reset();
    s.texture_allocator.Reset();
    s.list.Reset();
    if (s.fence_event != nullptr) CloseHandle(s.fence_event);
    s.fence_event = nullptr;
    s.fence.Reset();
    s.queue.Reset();
    s.device.Reset();
    s.adapter.Reset();
    s.factory.Reset();
    s.frame_rgba.clear();
    s.last_texture_rgba.clear();
    s.texture_cache_bytes = 0u;
    s.next_rtv = 0u;
    s.next_dsv = 0u;
    s.next_srv = 1u;
    s.next_sampler = 1u;
    s.frame_cursor = 0u;
    s.frame_epoch = 1u;
    s.sample_count = 1u;
    s.sample_quality = 0u;
    s.adapter_vendor_id = 0u;
    s.adapter_device_id = 0u;
    s.adapter_dedicated_video_memory = 0u;
    s.adapter_shared_system_memory = 0u;
    s.adapter_uma = false;
    s.adapter_cache_coherent_uma = false;
    s.amd_uma_safe_mode = false;
    s.packed_0115_gpu_stride = kPacked0115NativeStride;
    s.depth_format = kDepthFormat;
    s.depth_bits = 32u;
    s.swap_width = s.swap_height = 0u;
    s.native_window = nullptr;
    s.direct_present_ok = false;
    s.presented_framebuffer = 0u;
    s.missed_display_intervals = 0u;
    s.swapchain_tearing = false;
    clear_accumulation(s);
    s.enabled = false;
}

} // namespace

bool initialize_ge_gpu_backend(std::string &error) {
    Dx12GeState &s = state();
    destroy_backend(s);
    s.report = {};
    s.display_framebuffer = 0u;
    const RenderingConfiguration &rendering = vcs_configuration().rendering;
    s.report.requested = rendering.backend == RenderingBackend::DirectX12
        ? GeGpuBackendKind::DirectX12 : GeGpuBackendKind::Software;
    s.report.active = GeGpuBackendKind::Software;
    s.report.frames_in_flight_capacity = kFrameCount;
    if (rendering.backend != RenderingBackend::DirectX12) {
        s.report.message = "Software GE backend active";
        error.clear();
        return true;
    }
    if (!rendering.dx12_ge_color) {
        s.report.message = "DirectX 12 presentation active; native DX12 GE path is available but DX12GEColor=false";
        error.clear();
        return true;
    }
    if (!create_backend(s, error)) {
        const std::string native_error = error;
        runtime_log_error("dx12 ge initialize", native_error);
        destroy_backend(s);
        const char *strict = std::getenv("PSPRECOMP_DX12_GE_STRICT");
        const bool strict_mode = strict != nullptr && *strict != '\0' && *strict != '0';
        s.report.requested = GeGpuBackendKind::DirectX12;
        s.report.active = GeGpuBackendKind::Software;
        s.report.frames_in_flight_capacity = kFrameCount;
        s.report.message = "Native DirectX 12 GE failed; using stable software GE with DX12 presentation: " + native_error;
        error = native_error;
        return !strict_mode;
    }
    s.enabled = true;
    s.report.active = GeGpuBackendKind::DirectX12;
    s.report.loader_opened = true;
    s.report.instance_created = true;
    s.report.device_created = true;
    s.report.transfer_buffer_created = true;
    s.report.transfer_memory_mapped = true;
    s.report.command_pool_created = true;
    s.report.transfer_self_test_passed = true;
    s.report.offscreen_image_created = true;
    s.report.offscreen_image_memory_bound = true;
    s.report.offscreen_image_view_created = true;
    s.report.render_pass_created = true;
    s.report.framebuffer_created = true;
    s.report.shader_modules_created = true;
    s.report.graphics_pipeline_created = false;
    s.report.offscreen_self_test_passed = true;
    s.report.physical_device_count = 1u;
    s.report.graphics_queue_family = 0u;
    s.report.memory_type_index = 0u;
    s.report.upload_capacity_bytes = kGeometryUploadCapacity;
    s.report.offscreen_width = s.target_width;
    s.report.offscreen_height = s.target_height;
    s.report.game_frame_readback_bytes = s.readback_enabled ? s.frame_rgba.size() : 0u;
    s.report.frames_in_flight_capacity = kFrameCount;
    s.report.texture_descriptor_layout_created = true;
    s.report.texture_descriptor_pool_created = true;
    s.report.textured_shader_modules_created = true;
    s.report.textured_pipeline_created = true;
    s.report.full_mip_chain_active = true;
    s.report.mipmap_state_active = true;
    s.report.base_texture_formats_active = true;
    s.report.depth_image_created = true;
    s.report.depth_image_memory_bound = true;
    s.report.depth_image_view_created = true;
    s.report.depth_attachment_active = true;
    s.report.alpha_test_shader_active = true;
    s.report.standard_alpha_blend_pipeline_active = true;
    s.report.observed_blend_modes_pipeline_active = true;
    s.report.color_write_mask_pipeline_active = true;
    s.report.fog_shader_active = true;
    s.report.message = s.amd_uma_safe_mode
        ? "DirectX 12 native GE path: AMD UMA compatibility (12-byte packed 0x0115 + scalar/nonindexed submission) + hardware transform + PSP textures + direct swapchain"
        : "DirectX 12 native GE path: packed/lit 0x0115 GPU decode + native strips/indexing + hardware culling + batch merge + PSP textures + widescreen HUD + direct swapchain";
    {
        std::ostringstream init;
        init << "dx12 ge initialized adapter=" << s.adapter_name
             << " target=" << s.target_width << 'x' << s.target_height
             << " msaa=" << s.sample_count
             << " depth=" << s.depth_bits
             << " vendor=0x" << std::hex << s.adapter_vendor_id << std::dec
             << " uma=" << (s.adapter_uma ? 1 : 0)
             << " amd_uma_safe=" << (s.amd_uma_safe_mode ? 1 : 0)
             << " packed0115_stride=" << s.packed_0115_gpu_stride;
        runtime_log_line(init.str());
    }
    error.clear();
    return true;
}

void shutdown_ge_gpu_backend() noexcept {
    Dx12GeState &s = state();
    if (s.enabled) runtime_log_line("dx12 ge shutdown");
    destroy_backend(s);
    s.report = {};
    s.display_framebuffer = 0u;
}

bool ge_gpu_backend_active() noexcept { return state().enabled; }
bool ge_gpu_backend_transfer_ready() noexcept { return state().enabled; }
bool ge_gpu_backend_graphics_ready() noexcept { return state().enabled; }

void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &draw) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled) return;
    ++s.report.draw_calls;
    s.report.vertices += draw.vertex_count;
    if (draw.texture_enabled) ++s.report.textured_draw_calls;
    const std::uint32_t target = draw.framebuffer_address & 0x001FFFF0u;
    // Register/create render targets as soon as the GE identifies them. That
    // makes a target produced by an earlier PRIM immediately available as a
    // GPU SRV to a later framebuffer-feedback PRIM in the same display interval.
    if (draw.framebuffer_stride != 0u || target == s.display_framebuffer) {
        // VCS emits long runs into the same framebuffer. Stage 44.6 performed
        // an unordered_set insert + unordered_map lookup + resource ensure for
        // every PRIM. Cache the hot target and touch the containers only when
        // the target actually changes (or has not been created yet).
        if (target != s.last_registered_framebuffer_target ||
            find_framebuffer_target(s, target) == nullptr) {
            s.last_registered_framebuffer_target = target;
            s.known_frame_targets.insert(target);
            s.report.framebuffer_targets_observed = s.known_frame_targets.size();
            std::string error;
            if (!ensure_framebuffer_target(s, target, error) && !error.empty())
                runtime_log_error("dx12 framebuffer target", error);
        }
        const std::uint32_t logical_width = target == s.display_framebuffer
            ? kReferenceWidth
            : std::max<std::uint32_t>(1u, draw.framebuffer_stride != 0u
                  ? draw.framebuffer_stride
                  : static_cast<std::uint32_t>(std::max(1, draw.scissor_x1 + 1)));
        const std::uint32_t logical_height = target == s.display_framebuffer
            ? kReferenceHeight
            : static_cast<std::uint32_t>(std::max(1, draw.scissor_y1 + 1));
        note_framebuffer_logical_extent(s, target, logical_width, logical_height);
    }
    // When a later composition pass samples a render target, the texture state
    // provides the authoritative logical texture extent. This is available
    // before command recording, so all earlier accumulated vertices can still
    // be mapped into the correct coordinate space at finish_color_frame().
    if (draw.texture_enabled) {
        const std::uint32_t feedback = draw.texture_address & 0x001FFFF0u;
        if (Dx12FramebufferTarget *feedback_target = find_framebuffer_target(s, feedback)) {
            if (feedback == s.display_framebuffer) {
                feedback_target->logical_width = kReferenceWidth;
                feedback_target->logical_height = kReferenceHeight;
            } else {
                // Texture state is authoritative once the render target is used
                // as a texture. Framebuffer stride is allocation pitch, not
                // necessarily visible width (the display itself is 480 wide
                // with a 512-pixel stride).
                const std::uint32_t texture_width = draw.texture_width != 0u
                    ? draw.texture_width : draw.texture_buffer_width;
                if (texture_width != 0u) feedback_target->logical_width = texture_width;
                if (draw.texture_height != 0u) feedback_target->logical_height = draw.texture_height;
            }
        }
    }
}

void ge_gpu_backend_observe_camera(const std::array<float, 12> &view,
                                   const std::array<float, 16> &projection,
                                   const std::array<float, 8> &viewport,
                                   const std::array<float, 3> &camera_position,
                                   const GeGpuDrawDescriptor &draw,
                                   std::uint32_t vertex_weight) noexcept {
    Dx12GeState &s = state();
    const auto &proper = vcs_configuration().proper_shaders;
    const bool observer_needed = proper_shaders_private_available() && proper.enabled &&
        (proper.volumetric_clouds.enabled || proper.realtime_shadows.enabled);
    if (!s.enabled || !observer_needed ||
        vertex_weight == 0u || !draw.depth_test_enabled) return;
    if (!std::all_of(view.begin(), view.end(), [](float value) { return std::isfinite(value); }) ||
        !std::all_of(projection.begin(), projection.end(), [](float value) { return std::isfinite(value); }) ||
        !std::all_of(viewport.begin(), viewport.end(), [](float value) { return std::isfinite(value); }) ||
        !std::all_of(camera_position.begin(), camera_position.end(),
                     [](float value) { return std::isfinite(value); }))
        return;
    const std::uint32_t target = draw.framebuffer_address & 0x001FFFF0u;
    const auto found = std::find_if(
        s.cloud_cameras.begin(), s.cloud_cameras.end(),
        [&](const CloudCameraCandidate &candidate) {
            return candidate.target == target && candidate.view == view &&
                   candidate.projection == projection && candidate.viewport == viewport;
        });
    if (found != s.cloud_cameras.end()) {
        found->weight += vertex_weight;
        found->camera_position = camera_position;
        if ((draw.depth_function & 7u) >= 2u) found->occluding_weight += vertex_weight;
        return;
    }
    // Normal gameplay has only a handful of camera variants per frame. A hard
    // cap prevents malformed guest state from growing this host-only observer.
    if (s.cloud_cameras.size() >= 16u) return;
    const std::uint64_t occluding_weight = (draw.depth_function & 7u) >= 2u
        ? vertex_weight : 0u;
    s.cloud_cameras.push_back({view, projection, viewport, camera_position,
                               target, vertex_weight, occluding_weight});
}

bool ge_gpu_backend_stage_vertices(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex> vertices) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled) return false;
    ++s.report.staged_draw_calls;
    s.report.staged_vertices += vertices.size();
    s.report.staged_bytes += vertices.size_bytes();
    return true;
}

bool ge_gpu_backend_texture_needed(const GeGpuDrawDescriptor &draw) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || !draw.texture_enabled || draw.texture_format > 10u ||
        draw.texture_width == 0u || draw.texture_height == 0u) return false;
    const std::uint32_t feedback_address = draw.texture_address & 0x001FFFF0u;
    if (!explicit_host_decoded_texture(draw) &&
        find_framebuffer_target(s, feedback_address) != nullptr) {
        ++s.report.texture_cache_hits;
        return false; // native GPU->GPU framebuffer feedback: no CPU decode
    }
    ++s.report.texture_decode_requests;
    const std::uint64_t key = texture_key(draw);
    Dx12Texture *found = find_cached_texture(s, key);
    if (found == nullptr) return true;
    found->signature_epoch = s.frame_epoch;
    found->last_used_epoch = s.frame_epoch;
    if (draw.texture_content_signature != 0u &&
        found->descriptor.texture_content_signature != draw.texture_content_signature)
        return true;
    ++s.report.texture_cache_hits;
    return false;
}

void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &draw) noexcept {
    if (!draw.texture_enabled) {
        draw.texture_cache_key_hint = 0u;
        draw.texture_image_key_hint = 0u;
        return;
    }
    draw.texture_cache_key_hint = 0u;
    draw.texture_image_key_hint = 0u;
    const std::uint64_t key = texture_key(draw);
    draw.texture_cache_key_hint = key;
    draw.texture_image_key_hint = key;
}

bool ge_gpu_backend_texture_signature_needed(const GeGpuDrawDescriptor &draw) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || !draw.texture_enabled || draw.texture_width == 0u || draw.texture_height == 0u)
        return false;
    if (!explicit_host_decoded_texture(draw) &&
        find_framebuffer_target(s, draw.texture_address) != nullptr)
        return false; // framebuffer target already has authoritative content on the GPU
    Dx12Texture *found = find_cached_texture(s, texture_key(draw));
    return found == nullptr || found->signature_epoch != s.frame_epoch;
}

bool ge_gpu_backend_is_framebuffer_feedback_texture(const GeGpuDrawDescriptor &draw) noexcept {
    const Dx12GeState &s = state();
    return s.enabled && draw.texture_enabled &&
           !explicit_host_decoded_texture(draw) &&
           find_framebuffer_target(s, draw.texture_address) != nullptr;
}
GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(
    const GeGpuDrawDescriptor &draw) noexcept {
    GeGpuWidescreenHud hud{};
    Dx12GeState &s = state();
    if (!s.enabled) return hud;

    const VcsConfiguration &config = vcs_configuration();
    if (!config.initialized || !config.widescreen.enabled) return hud;

    // Widescreen is a property of the output surface, not of the internal
    // resolution. Keeping these independent allows a 960x544 performance
    // target to still produce a correct 21:9/32:9 camera and HUD.
    const DisplaySurfaceDimensions output =
        resolve_display_surface_dimensions(config.display);
    const float shrink = widescreen_stretch_factor(config, output.width, output.height);
    if (!std::isfinite(shrink) || shrink <= 0.0f ||
        std::abs(shrink - 1.0f) < 1.0e-5f)
        return hud;

    // Through-mode HUD vertices use the logical coordinate space of the PSP
    // target. The display target is 480 px wide; oversized offscreen targets
    // scale the same span, so preserve their center while shrinking UI X.
    std::uint32_t logical_width = kReferenceWidth;
    if (const Dx12FramebufferTarget *target =
            find_framebuffer_target(s, draw.framebuffer_address)) {
        logical_width = std::max<std::uint32_t>(1u, target->logical_width);
    }

    hud.shrink = shrink;
    hud.display_scale_x = static_cast<float>(kReferenceWidth) /
                          static_cast<float>(logical_width);
    hud.source_center = static_cast<float>(logical_width) * 0.5f;
    return hud;
}
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &, float, float) noexcept {}
bool ge_gpu_backend_adopt_shared_texture(const GeGpuDrawDescriptor &) noexcept { return false; }

bool ge_gpu_backend_texture_available(const GeGpuDrawDescriptor &draw) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || !draw.texture_enabled) return false;
    if (!explicit_host_decoded_texture(draw)) {
        if (const auto *target = find_framebuffer_target(s, draw.texture_address))
            return target->color != nullptr;
    }
    Dx12Texture *found = find_cached_texture(s, texture_key(draw));
    if (found == nullptr || !found->image) return false;
    found->last_used_epoch = s.frame_epoch;
    return true;
}

bool ge_gpu_backend_upload_decoded_texture(const GeGpuDrawDescriptor &draw, std::uint32_t width,
                                           std::uint32_t height,
                                           std::span<const std::byte> rgba) noexcept {
    if (rgba.empty()) return false;
    std::vector<std::byte> packed(rgba.begin(), rgba.end());
    return prepare_texture_upload(state(), draw, width, height, 1u, std::move(packed));
}

bool ge_gpu_backend_upload_decoded_texture_chain(const GeGpuDrawDescriptor &draw,
                                                 std::span<const GeGpuDecodedMipLevel> levels) noexcept {
    if (levels.empty() || levels.size() > 8u) return false;
    std::size_t total = 0u;
    std::uint32_t w = levels.front().width;
    std::uint32_t h = levels.front().height;
    if (w == 0u || h == 0u) return false;
    for (const auto &level : levels) {
        const std::size_t bytes = static_cast<std::size_t>(level.width) * level.height * 4u;
        if (level.width != w || level.height != h || level.rgba8.size() != bytes) return false;
        total += bytes;
        w = std::max(1u, w >> 1u);
        h = std::max(1u, h >> 1u);
    }
    std::vector<std::byte> packed;
    try {
        packed.reserve(total);
        for (const auto &level : levels) packed.insert(packed.end(), level.rgba8.begin(), level.rgba8.end());
    } catch (...) { return false; }
    return prepare_texture_upload(state(), draw, levels.front().width, levels.front().height,
                                  static_cast<std::uint32_t>(levels.size()), std::move(packed));
}

bool ge_gpu_backend_upload_decoded_texture_chain_packed(const GeGpuDrawDescriptor &draw,
                                                        std::uint32_t width, std::uint32_t height,
                                                        std::uint32_t mip_levels,
                                                        std::vector<std::byte> packed) noexcept {
    return prepare_texture_upload(state(), draw, width, height, mip_levels, std::move(packed));
}
bool ge_gpu_backend_copy_last_texture_rgba(std::span<std::byte> destination) noexcept {
    const Dx12GeState &s = state();
    if (s.last_texture_rgba.empty() || destination.size() < s.last_texture_rgba.size()) return false;
    std::memcpy(destination.data(), s.last_texture_rgba.data(), s.last_texture_rgba.size());
    return true;
}

void ge_gpu_backend_accumulate_color_triangles(
    const GeGpuDrawDescriptor &draw,
    std::span<const GeGpuVertex> triangle_vertices) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || triangle_vertices.empty() || triangle_vertices.size() % 3u != 0u) return;
    const std::size_t required =
        (s.vertices.size() + triangle_vertices.size()) * sizeof(Dx12UploadVertex) +
        s.packed_0115_vertices.size() +
        s.indices.size() * sizeof(std::uint32_t) + 16u;
    if (required > kGeometryUploadCapacity ||
        s.vertices.size() > std::numeric_limits<std::uint32_t>::max()) {
        ++s.report.game_vertex_overflows;
        return;
    }
    try {
        const bool sampled_texture = draw.texture_enabled && ge_gpu_backend_texture_available(draw);
        const std::uint32_t first = static_cast<std::uint32_t>(s.vertices.size());
        for (const GeGpuVertex &source : triangle_vertices) {
            Dx12UploadVertex vertex = make_upload_vertex(source);
            if (sampled_texture && draw.texture_width != 0u && draw.texture_height != 0u) {
                vertex.u /= static_cast<float>(draw.texture_width);
                vertex.v /= static_cast<float>(draw.texture_height);
            }
            s.vertices.push_back(vertex);
        }
        const std::uint32_t feedback_address = draw.texture_address & 0x001FFFF0u;
        const bool framebuffer_feedback = draw.texture_enabled &&
            !explicit_host_decoded_texture(draw) &&
            find_framebuffer_target(s, feedback_address) != nullptr;
        Dx12Batch batch{};
        batch.draw = draw;
        batch.first_vertex = first;
        batch.vertex_count = static_cast<std::uint32_t>(triangle_vertices.size());
        batch.logical_draw_count = 1u;
        batch.framebuffer_feedback = framebuffer_feedback;
        batch.feedback_address = feedback_address;
        (void)append_or_merge_batch(s, std::move(batch));
        ++s.report.game_draw_calls;
        s.report.game_triangles += triangle_vertices.size() / 3u;
        s.report.game_vertices += triangle_vertices.size();
        if (draw.texture_enabled && sampled_texture) ++s.report.textured_game_draw_calls;
        else if (draw.texture_enabled) ++s.report.game_textured_draws_without_texture;
    } catch (...) {
        ++s.report.game_vertex_overflows;
    }
}

void ge_gpu_backend_accumulate_hardware_triangles(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const GeGpuVertex> vertices,
    std::span<const std::uint32_t> triangle_indices) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || vertices.empty()) return;

    // Native indices are deliberately isolated behind one DX12-only switch.
    // Stage 44.8 changed indexed submission *and* auto-enabled parallel decode
    // in the same regression window, so the original crash never proved the
    // index buffer itself guilty. 45.4 reintroduces only this one optimization;
    // the compatibility launcher can turn it off without rebuilding.
    const bool indexed = native_indexed_draw_enabled() && !s.amd_uma_safe_mode && !triangle_indices.empty();
    const std::size_t emitted_count = triangle_indices.empty() ? vertices.size()
                                                               : triangle_indices.size();
    if (emitted_count == 0u ||
        (transform.primitive == 4u ? emitted_count < 3u : (emitted_count % 3u) != 0u)) return;
    if (s.vertices.size() > std::numeric_limits<std::uint32_t>::max() ||
        s.indices.size() > std::numeric_limits<std::uint32_t>::max()) {
        ++s.report.game_vertex_overflows;
        return;
    }

    const std::size_t vertices_to_append = indexed ? vertices.size() : emitted_count;
    const std::size_t indices_to_append = indexed ? triangle_indices.size() : 0u;
    const std::size_t required =
        (s.vertices.size() + vertices_to_append) * sizeof(Dx12UploadVertex) +
        s.packed_0115_vertices.size() +
        (s.indices.size() + indices_to_append) * sizeof(std::uint32_t) + 16u;
    if (required > kGeometryUploadCapacity) {
        ++s.report.game_vertex_overflows;
        return;
    }

    try {
        const bool sampled_texture = draw.texture_enabled && ge_gpu_backend_texture_available(draw);
        const std::uint32_t first_vertex = static_cast<std::uint32_t>(s.vertices.size());
        const std::uint32_t first_index = static_cast<std::uint32_t>(s.indices.size());

        if (indexed || triangle_indices.empty()) {
            for (const GeGpuVertex &source : vertices)
                s.vertices.push_back(make_upload_vertex(source));
        } else {
            // Exact Stage 44.7 fallback: expand the index stream on the CPU.
            for (std::uint32_t index : triangle_indices) {
                if (static_cast<std::size_t>(index) >= vertices.size()) {
                    ++s.report.game_vertex_overflows;
                    s.vertices.resize(first_vertex);
                    return;
                }
                s.vertices.push_back(make_upload_vertex(vertices[index]));
            }
        }

        if (indexed) {
            for (std::uint32_t index : triangle_indices) {
                if (static_cast<std::size_t>(index) >= vertices.size()) {
                    ++s.report.game_vertex_overflows;
                    s.vertices.resize(first_vertex);
                    s.indices.resize(first_index);
                    return;
                }
                s.indices.push_back(index);
            }
        }

        const std::uint32_t logical = std::max<std::uint32_t>(1u, transform.logical_prim_batches);
        const std::uint32_t feedback_address = draw.texture_address & 0x001FFFF0u;
        const bool framebuffer_feedback = draw.texture_enabled &&
            !explicit_host_decoded_texture(draw) &&
            find_framebuffer_target(s, feedback_address) != nullptr;
        Dx12Batch batch{};
        batch.draw = draw;
        batch.first_vertex = first_vertex;
        batch.vertex_count = static_cast<std::uint32_t>(vertices_to_append);
        batch.first_index = first_index;
        batch.index_count = indexed ? static_cast<std::uint32_t>(triangle_indices.size()) : 0u;
        batch.indexed = indexed;
        batch.logical_draw_count = logical;
        batch.framebuffer_feedback = framebuffer_feedback;
        batch.feedback_address = feedback_address;
        batch.hardware_transform = true;
        batch.transform = transform;
        (void)append_or_merge_batch(s, std::move(batch));

        s.report.game_draw_calls += logical;
        s.report.game_triangles += transform.primitive == 4u
            ? (emitted_count > 2u ? emitted_count - 2u : 0u) : emitted_count / 3u;
        s.report.game_vertices += emitted_count;
        s.report.hw_transform_draw_calls += logical;
        s.report.hw_transform_vertices += vertices.size();
        s.report.hw_transform_prim_batches += logical;
        s.report.hw_transform_unique_vertices_decoded += transform.unique_vertices_decoded;
        s.report.hw_transform_index_reuses += transform.index_reuses;
        if (draw.texture_enabled && sampled_texture) s.report.textured_game_draw_calls += logical;
        else if (draw.texture_enabled) s.report.game_textured_draws_without_texture += logical;
    } catch (...) {
        ++s.report.game_vertex_overflows;
    }
}

bool ge_gpu_backend_accumulate_hardware_packed_0115(
    const GeGpuDrawDescriptor &draw,
    const GeGpuHardwareTransform &transform,
    std::span<const std::byte> packed_vertices,
    std::uint32_t vertex_count,
    std::span<const std::uint32_t> triangle_indices) noexcept {
    Dx12GeState &s = state();
    const std::size_t storage_stride = static_cast<std::size_t>(s.packed_0115_gpu_stride);
    if (!s.enabled || vertex_count == 0u ||
        packed_vertices.size() != static_cast<std::size_t>(vertex_count) * kPacked0115GuestStride ||
        storage_stride < kPacked0115GuestStride)
        return false;

    // Vega 8/AMD UMA compatibility: keep the direct GPU unpack shader, but
    // avoid the 10-byte IA stride and native indexed path. 12-byte records keep
    // every vertex start 4-byte aligned while preserving the guest fields at
    // offsets 0/2/4/8. This avoids vendor-sensitive packed fetch corruption
    // without falling all the way back to the 36-byte CPU-decoded stream.
    const bool indexed = native_indexed_draw_enabled() &&
        !s.amd_uma_safe_mode && !triangle_indices.empty();
    const std::size_t emitted_count = triangle_indices.empty()
        ? static_cast<std::size_t>(vertex_count) : triangle_indices.size();
    if (emitted_count == 0u ||
        (transform.primitive == 4u ? emitted_count < 3u : (emitted_count % 3u) != 0u)) return false;

    const std::size_t first_packed_byte = s.packed_0115_vertices.size();
    if ((first_packed_byte % storage_stride) != 0u) return false;
    const std::size_t first_vertex64 = first_packed_byte / storage_stride;
    if (first_vertex64 > std::numeric_limits<std::uint32_t>::max() ||
        s.indices.size() > std::numeric_limits<std::uint32_t>::max())
        return false;

    const std::size_t stored_vertex_count =
        indexed || triangle_indices.empty() ? static_cast<std::size_t>(vertex_count) : emitted_count;
    const std::size_t packed_append_bytes = stored_vertex_count * storage_stride;
    const std::size_t index_append_count = indexed ? triangle_indices.size() : 0u;
    const std::size_t required = s.vertices.size() * sizeof(Dx12UploadVertex) +
        s.packed_0115_vertices.size() + packed_append_bytes +
        (s.indices.size() + index_append_count) * sizeof(std::uint32_t) + 16u;
    if (required > kGeometryUploadCapacity) {
        ++s.report.game_vertex_overflows;
        return false;
    }

    const std::uint32_t first_vertex = static_cast<std::uint32_t>(first_vertex64);
    const std::uint32_t first_index = static_cast<std::uint32_t>(s.indices.size());

    const auto append_record = [&](std::uint32_t index) {
        const std::byte *source = packed_vertices.data() +
            static_cast<std::size_t>(index) * kPacked0115GuestStride;
        s.packed_0115_vertices.insert(
            s.packed_0115_vertices.end(), source, source + kPacked0115GuestStride);
        for (std::size_t padding = kPacked0115GuestStride; padding < storage_stride; ++padding)
            s.packed_0115_vertices.push_back(std::byte{0});
    };

    try {
        if (storage_stride == kPacked0115GuestStride &&
            (indexed || triangle_indices.empty())) {
            s.packed_0115_vertices.insert(
                s.packed_0115_vertices.end(), packed_vertices.begin(), packed_vertices.end());
        } else if (indexed || triangle_indices.empty()) {
            for (std::uint32_t index = 0u; index < vertex_count; ++index)
                append_record(index);
        } else {
            // Stable compatibility path: duplicate local records according to
            // the submitted strip/list instead of exposing native indexing.
            for (std::uint32_t index : triangle_indices) {
                if (index >= vertex_count) {
                    ++s.report.game_vertex_overflows;
                    s.packed_0115_vertices.resize(first_packed_byte);
                    return false;
                }
                append_record(index);
            }
        }
        if (indexed) {
            for (std::uint32_t index : triangle_indices) {
                if (index >= vertex_count) {
                    ++s.report.game_vertex_overflows;
                    s.packed_0115_vertices.resize(first_packed_byte);
                    s.indices.resize(first_index);
                    return false;
                }
                s.indices.push_back(index);
            }
        }

        const std::uint32_t logical = std::max<std::uint32_t>(1u, transform.logical_prim_batches);
        const std::uint32_t feedback_address = draw.texture_address & 0x001FFFF0u;
        const bool framebuffer_feedback = draw.texture_enabled &&
            !explicit_host_decoded_texture(draw) &&
            find_framebuffer_target(s, feedback_address) != nullptr;
        Dx12Batch batch{};
        batch.draw = draw;
        batch.first_vertex = first_vertex;
        batch.vertex_count = static_cast<std::uint32_t>(stored_vertex_count);
        batch.first_index = first_index;
        batch.index_count = indexed ? static_cast<std::uint32_t>(triangle_indices.size()) : 0u;
        batch.indexed = indexed;
        batch.packed_0115 = true;
        batch.logical_draw_count = logical;
        batch.framebuffer_feedback = framebuffer_feedback;
        batch.feedback_address = feedback_address;
        batch.hardware_transform = true;
        batch.transform = transform;
        (void)append_or_merge_batch(s, std::move(batch));

        s.report.game_draw_calls += logical;
        s.report.game_triangles += transform.primitive == 4u
            ? (emitted_count > 2u ? emitted_count - 2u : 0u) : emitted_count / 3u;
        s.report.game_vertices += emitted_count;
        s.report.hw_transform_draw_calls += logical;
        s.report.hw_transform_vertices += vertex_count;
        s.report.hw_transform_prim_batches += logical;
        s.report.hw_transform_unique_vertices_decoded += transform.unique_vertices_decoded;
        s.report.hw_transform_index_reuses += transform.index_reuses;
        if (draw.texture_enabled && ge_gpu_backend_texture_available(draw))
            s.report.textured_game_draw_calls += logical;
        else if (draw.texture_enabled)
            ++s.report.game_textured_draws_without_texture;
        return true;
    } catch (...) {
        s.packed_0115_vertices.resize(first_packed_byte);
        s.indices.resize(first_index);
        ++s.report.game_vertex_overflows;
        return false;
    }
}

void ge_gpu_backend_set_native_window(void *native_window) noexcept {
    Dx12GeState &s = state();
    const HWND window = static_cast<HWND>(native_window);
    if (s.native_window == window) return;
    if (s.swapchain) {
        std::string ignored;
        (void)wait_for_gpu(s, ignored);
        release_swapchain_buffers(s);
        s.swapchain.Reset();
        s.swap_rtv_heap.Reset();
        s.swap_width = s.swap_height = 0u;
        s.direct_present_ok = false;
        s.presented_framebuffer = 0u;
        s.missed_display_intervals = 0u;
        s.report.swapchain_active = false;
    }
    s.native_window = window;
}

void ge_gpu_backend_set_display_framebuffer(std::uint32_t address) noexcept {
    Dx12GeState &s = state();
    s.display_framebuffer = address & 0x001FFFF0u;
    if (!s.enabled || !s.device) return;
    std::string error;
    if (!ensure_framebuffer_target(s, s.display_framebuffer, error) && !error.empty())
        runtime_log_error("dx12 display framebuffer", error);
    note_framebuffer_logical_extent(s, s.display_framebuffer,
                                    kReferenceWidth, kReferenceHeight);
}

bool ge_gpu_backend_finish_color_frame(std::uint64_t vblank) noexcept {
    Dx12GeState &s = state();
    if (!s.enabled || !s.device) {
        clear_accumulation(s);
        ++s.frame_epoch;
        return false;
    }
    if ((s.vertices.empty() && s.packed_0115_vertices.empty()) || s.batches.empty()) {
        if (++s.missed_display_intervals > 4u) {
            s.direct_present_ok = false;
            s.presented_framebuffer = 0u;
        }
        clear_accumulation(s);
        ++s.frame_epoch;
        return false;
    }

    // Final semantic pass: once all draws for the frame are known, expand each
    // high-confidence vehicle material anchor to the complete contiguous vehicle
    // entity run (wheels/glass/trim included).
    promote_vehicle_batch_groups(s);

    const std::size_t vertex_bytes = s.vertices.size() * sizeof(Dx12UploadVertex);
    const std::size_t packed_offset = (vertex_bytes + 3u) & ~std::size_t{3u};
    const std::size_t packed_bytes = s.packed_0115_vertices.size();
    const std::size_t index_offset = (packed_offset + packed_bytes + 3u) & ~std::size_t{3u};
    const std::size_t index_bytes = s.indices.size() * sizeof(std::uint32_t);
    const std::size_t bytes = index_offset + index_bytes;
    if (bytes > kGeometryUploadCapacity) {
        ++s.report.game_vertex_overflows;
        clear_accumulation(s);
        ++s.frame_epoch;
        return false;
    }

    // Targets are registered once in ge_gpu_backend_record_draw(). Stage 44.6
    // repeated the same unordered_map/resource checks for every batch here,
    // doubling hot frontend bookkeeping before command recording even began.
    Dx12FramebufferTarget *display_target = find_framebuffer_target(s, s.display_framebuffer);

    Dx12FrameResources &frame = s.frames[s.frame_cursor];
    const bool indirect_enabled = dx12_execute_indirect_enabled(s) &&
        !s.amd_uma_safe_mode &&
        s.indirect_draw_signature && s.indirect_draw_indexed_signature &&
        frame.indirect_upload_buffer && frame.mapped_indirect_upload != nullptr;
    std::string error;
    if (!wait_for_fence(s, frame.fence_value, error)) {
        runtime_log_error("dx12 ge frame wait", error);
        clear_accumulation(s);
        return false;
    }
    frame.transient_resources.clear();
    frame.texture_upload_cursor = 0u;
    if (frame.mapped_upload == nullptr ||
        (s.texture_upload_ring_enabled && frame.mapped_texture_upload == nullptr)) {
        runtime_log_error("dx12 ge", "frame upload arena is not mapped");
        clear_accumulation(s);
        return false;
    }
    if (vertex_bytes != 0u)
        std::memcpy(frame.mapped_upload, s.vertices.data(), vertex_bytes);
    if (packed_bytes != 0u)
        std::memcpy(frame.mapped_upload + packed_offset, s.packed_0115_vertices.data(), packed_bytes);
    if (index_bytes != 0u)
        std::memcpy(frame.mapped_upload + index_offset, s.indices.data(), index_bytes);

    bool direct_possible = false;
    if (s.native_window != nullptr) {
        std::string present_error;
        direct_possible = ensure_swapchain(s, present_error);
        if (!direct_possible && !present_error.empty())
            runtime_log_error("dx12 ge swapchain", present_error);
    }

    HRESULT hr = frame.allocator->Reset();
    if (FAILED(hr)) {
        runtime_log_error("dx12 ge", hr_text(hr, "CommandAllocator::Reset"));
        clear_accumulation(s);
        return false;
    }
    hr = s.list->Reset(frame.allocator.Get(), nullptr);
    if (FAILED(hr)) {
        runtime_log_error("dx12 ge", hr_text(hr, "CommandList::Reset"));
        clear_accumulation(s);
        return false;
    }

    record_pending_texture_uploads(s, frame);

    const D3D12_VIEWPORT viewport{0.0f, 0.0f, static_cast<float>(s.target_width),
                                  static_cast<float>(s.target_height), 0.0f, 1.0f};
    s.list->RSSetViewports(1u, &viewport);
    const D3D12_VERTEX_BUFFER_VIEW vb{
        frame.upload_buffer->GetGPUVirtualAddress(), static_cast<UINT>(vertex_bytes),
        static_cast<UINT>(sizeof(Dx12UploadVertex))};
    const D3D12_VERTEX_BUFFER_VIEW packed_vb{
        frame.upload_buffer->GetGPUVirtualAddress() + packed_offset,
        static_cast<UINT>(packed_bytes), s.packed_0115_gpu_stride};
    D3D12_INDEX_BUFFER_VIEW ib{};
    if (index_bytes != 0u) {
        ib.BufferLocation = frame.upload_buffer->GetGPUVirtualAddress() + index_offset;
        ib.SizeInBytes = static_cast<UINT>(index_bytes);
        ib.Format = DXGI_FORMAT_R32_UINT;
        s.list->IASetIndexBuffer(&ib);
    }
    s.list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    s.list->SetGraphicsRootSignature(s.root_signature.Get());
    ID3D12DescriptorHeap *descriptor_heaps[]{s.srv_heap.Get(), s.sampler_heap.Get()};
    s.list->SetDescriptorHeaps(2u, descriptor_heaps);

    ID3D12PipelineState *active_pipeline = nullptr;
    std::uint64_t active_pipeline_key = std::numeric_limits<std::uint64_t>::max();
    Dx12FramebufferTarget *current_target = nullptr;
    std::uint32_t current_address = 0xFFFFFFFFu;
    std::uint32_t executed_batches = 0u;
    std::size_t indirect_cursor = 0u;
    std::uint32_t bound_srv = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t bound_sampler = std::numeric_limits<std::uint32_t>::max();
    Dx12TransformConstants active_transform{};
    bool active_transform_valid = false;
    Dx12PixelConstants active_pixel{};
    bool active_pixel_valid = false;
    std::array<float, 16> active_shadow_rows{};
    bool active_shadow_rows_valid = false;
    D3D12_RECT active_scissor{};
    bool active_scissor_valid = false;
    std::uint32_t active_blend_fix = std::numeric_limits<std::uint32_t>::max();
    bool active_packed_0115 = false;
    bool active_vertex_layout_valid = false;
    D3D12_PRIMITIVE_TOPOLOGY active_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    bool touched_display = false;
    const CloudCameraCandidate *cloud_camera = select_cloud_camera(s);
    const CloudShaderConstants clouds = cloud_present_constants(s);
    const std::uint32_t cloud_target_address = cloud_camera != nullptr
        ? cloud_camera->target : 0u;

    std::array<float, 16> material_shadow_light_vp{};
    bool material_shadow_ready = false;
    if (cloud_camera != nullptr) {
        if (Dx12FramebufferTarget *shadow_target =
                find_framebuffer_target(s, cloud_target_address);
            shadow_target != nullptr && shadow_target->color && shadow_target->depth) {
            material_shadow_ready = record_directional_shadow_map(
                s, *shadow_target, *cloud_camera, vb, packed_vb,
                index_bytes != 0u ? &ib : nullptr, material_shadow_light_vp);
        }
    }
    // Shadow-map generation uses its own depth-only root signature/viewport.
    // Restore the material GE state before replaying the actual frame. t1 is
    // always bound (null descriptor when unavailable), so every private pipe
    // has a defined resource binding.
    s.list->SetGraphicsRootSignature(s.root_signature.Get());
    s.list->SetDescriptorHeaps(2u, descriptor_heaps);
    s.list->SetGraphicsRootDescriptorTable(4u, srv_gpu(
        s, material_shadow_ready ? s.realtime_shadow_map.srv_index : 0u));
    s.list->RSSetViewports(1u, &viewport);
    if (index_bytes != 0u) s.list->IASetIndexBuffer(&ib);

    // Keep direct and indirect draws on the exact same shadow constants. The
    // indirect command carries all 21 DWORDs of root parameter 3, so it must
    // not inherit the previous batch's model-to-light matrix.
    const auto make_shadow_rows = [&](const Dx12Batch &draw_batch) {
        std::array<float, 16> rows{};
        if (material_shadow_ready && draw_batch.hardware_transform &&
            draw_batch.draw.shader_pipe != GeShaderPipe::Native) {
            const std::array<float, 16> model_to_light = shadow_multiply_mat4(
                material_shadow_light_vp, draw_batch.transform.model_to_world);
            shadow_matrix_rows(model_to_light, rows.data());
        }
        return rows;
    };

    bool clouds_injected = false;
    bool cloud_depth_writing_world_seen = false;
    static bool cloud_trace_done = false;
    const bool trace_cloud_frame = !cloud_trace_done && cloud_camera != nullptr;
    std::size_t cloud_batch_index = 0u;
    if (trace_cloud_frame) {
        std::ostringstream line;
        line << "CLOUD_TRACE build=temporal-fading-v4 frame=" << s.frame_epoch
             << " selected=0x" << std::hex << cloud_target_address
             << " display=0x" << s.display_framebuffer << std::dec
             << " batches=" << s.batches.size();
        runtime_log_line(line.str());
    }

    constexpr float black[4]{0.0f, 0.0f, 0.0f, 1.0f};
    for (std::size_t batch_cursor = 0u; batch_cursor < s.batches.size(); ++batch_cursor) {
        const Dx12Batch &batch = s.batches[batch_cursor];
        const std::uint32_t address = batch.draw.framebuffer_address & 0x001FFFF0u;
        const bool cloud_target_batch = address == cloud_target_address;
        if (trace_cloud_frame && (cloud_target_batch || address == s.display_framebuffer)) {
            std::ostringstream line;
            line << "CLOUD_BATCH i=" << cloud_batch_index << " target=0x" << std::hex
                 << address << std::dec << " clear=" << batch.draw.clear_mode
                 << " depth_test=" << batch.draw.depth_test_enabled
                 << " depth_write=" << batch.draw.depth_write_enabled
                 << " blend=" << batch.draw.blend_enabled
                 << " alpha_test=" << batch.draw.alpha_test_enabled
                 << " feedback=" << batch.framebuffer_feedback;
            runtime_log_line(line.str());
        }
        // Captured VCS frame: opaque/alpha-tested world geometry is the run
        // with depth writes (batches 2..92); FadingEntities starts at the first
        // depth-tested draw without depth writes (batch 93). Blend and alpha
        // test stay enabled throughout and cannot identify this boundary.
        if (cloud_target_batch && !batch.draw.clear_mode &&
            batch.draw.depth_test_enabled && batch.draw.depth_write_enabled)
            cloud_depth_writing_world_seen = true;
        const bool fading_entities_boundary = cloud_target_batch &&
            cloud_depth_writing_world_seen && !batch.draw.clear_mode &&
            batch.draw.depth_test_enabled && !batch.draw.depth_write_enabled;
        if (!clouds_injected && cloud_camera != nullptr &&
            fading_entities_boundary) {
            if (trace_cloud_frame) {
                runtime_log_line("CLOUD_INJECT before_batch=" +
                                 std::to_string(cloud_batch_index) +
                                 " reason=fading_entities");
            }
            if (Dx12FramebufferTarget *cloud_target =
                    find_framebuffer_target(s, cloud_target_address);
                cloud_target != nullptr && cloud_target->color && cloud_target->depth) {
                if (current_target != nullptr && current_target != cloud_target)
                    resolve_target_for_sampling(s, *current_target, false);
                record_clouds_into_world_target(s, *cloud_target, clouds);
                // Private shadow/cloud passes use root layouts different from the GE
                // material path. Restore the GE layout before FadingEntities;
                // all cached bindings below are invalidated and repopulated.
                s.list->SetGraphicsRootSignature(s.root_signature.Get());
                s.list->SetDescriptorHeaps(2u, descriptor_heaps);
                s.list->SetGraphicsRootDescriptorTable(4u, srv_gpu(
                    s, material_shadow_ready ? s.realtime_shadow_map.srv_index : 0u));
                current_target = cloud_target;
                current_address = cloud_target_address;
                clouds_injected = true;
                active_pipeline = nullptr;
                active_pipeline_key = std::numeric_limits<std::uint64_t>::max();
                bound_srv = bound_sampler = std::numeric_limits<std::uint32_t>::max();
                active_transform_valid = false;
                active_pixel_valid = false;
                active_shadow_rows_valid = false;
                active_scissor_valid = false;
                active_blend_fix = std::numeric_limits<std::uint32_t>::max();
                active_vertex_layout_valid = false;
                active_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            }
        }
        ++cloud_batch_index;
        Dx12FramebufferTarget *target = address == current_address
            ? current_target : find_framebuffer_target(s, address);
        if (target == nullptr || !target->color || !target->depth) continue;

        if (current_target != target) {
            if (current_target != nullptr)
                resolve_target_for_sampling(s, *current_target, false);
            prepare_target_for_render(s, *target);
            const D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtv_cpu(s, target->rtv_index);
            const D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsv_cpu(s, target->dsv_index);
            s.list->OMSetRenderTargets(1u, &rtv, FALSE, &dsv);

            // PSP EDRAM is persistent. Offscreen colors survive across display
            // intervals and are only initialized once; the displayed surface is
            // cleared once per interval to retain the stable Stage 44.4 behavior.
            const bool first_ever_use = target->last_render_epoch == 0u;
            const bool first_use_this_frame = target->last_render_epoch != s.frame_epoch;
            if (first_use_this_frame) {
                if (first_ever_use || address == s.display_framebuffer)
                    s.list->ClearRenderTargetView(rtv, black, 0u, nullptr);
                s.list->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0u, 0u, nullptr);
                target->last_render_epoch = s.frame_epoch;
            }
            current_target = target;
            current_address = address;
            bound_srv = std::numeric_limits<std::uint32_t>::max();
            bound_sampler = std::numeric_limits<std::uint32_t>::max();
        }

        if (address == s.display_framebuffer) touched_display = true;

        const D3D12_PRIMITIVE_TOPOLOGY topology =
            batch.hardware_transform && batch.transform.primitive == 4u
                ? D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
                : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        if (topology != active_topology) {
            s.list->IASetPrimitiveTopology(topology);
            active_topology = topology;
        }

        if (!active_vertex_layout_valid || active_packed_0115 != batch.packed_0115) {
            const D3D12_VERTEX_BUFFER_VIEW &active_vb = batch.packed_0115 ? packed_vb : vb;
            s.list->IASetVertexBuffers(0u, 1u, &active_vb);
            active_packed_0115 = batch.packed_0115;
            active_vertex_layout_valid = true;
        }

        const bool batch_cull = batch.hardware_transform && batch.transform.cull_enabled;
        const bool batch_accept_ccw = batch_cull && batch.transform.accept_counter_clockwise;
        const std::uint64_t batch_pipeline_key = pipeline_key(batch.draw) |
            (batch.packed_0115 ? (std::uint64_t{1} << 63u) : 0u) |
            (batch_cull ? (std::uint64_t{1} << 62u) : 0u) |
            (batch_accept_ccw ? (std::uint64_t{1} << 61u) : 0u);
        if (active_pipeline == nullptr || batch_pipeline_key != active_pipeline_key) {
            ID3D12PipelineState *pipeline = pipeline_for(
                s, batch.draw, batch.packed_0115, batch_cull, batch_accept_ccw, error);
            if (pipeline == nullptr) {
                runtime_log_error("dx12 ge pipeline", error);
                continue;
            }
            if (pipeline != active_pipeline)
                s.list->SetPipelineState(pipeline);
            active_pipeline = pipeline;
            active_pipeline_key = batch_pipeline_key;
        }

        std::uint32_t srv_index = 0u;
        std::uint32_t sampler_index = 0u;
        if (batch.draw.texture_enabled) {
            const std::uint32_t feedback_address = batch.feedback_address;
            Dx12FramebufferTarget *feedback = batch.framebuffer_feedback
                ? find_framebuffer_target(s, feedback_address) : nullptr;
            if (feedback != nullptr && feedback->color) {
                sampler_index = ensure_sampler(s, batch.draw);
                if (feedback == current_target) {
                    // D3D12 cannot sample the target while it is bound for
                    // writing. Resolve MSAA first (if active), snapshot the
                    // single-sample image GPU->GPU, then resume rendering.
                    std::string feedback_error;
                    if (ensure_feedback_copy(s, *feedback, feedback_error)) {
                        resolve_target_for_sampling(s, *feedback, false);
                        transition(s.list.Get(), feedback->color.Get(), feedback->color_state,
                                   D3D12_RESOURCE_STATE_COPY_SOURCE);
                        feedback->color_state = D3D12_RESOURCE_STATE_COPY_SOURCE;
                        transition(s.list.Get(), feedback->feedback_copy.Get(), feedback->feedback_state,
                                   D3D12_RESOURCE_STATE_COPY_DEST);
                        feedback->feedback_state = D3D12_RESOURCE_STATE_COPY_DEST;
                        s.list->CopyResource(feedback->feedback_copy.Get(), feedback->color.Get());
                        transition(s.list.Get(), feedback->feedback_copy.Get(), feedback->feedback_state,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                        feedback->feedback_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                        transition(s.list.Get(), feedback->color.Get(), feedback->color_state,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                        feedback->color_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                        prepare_target_for_render(s, *feedback);
                        const D3D12_CPU_DESCRIPTOR_HANDLE self_rtv = rtv_cpu(s, feedback->rtv_index);
                        const D3D12_CPU_DESCRIPTOR_HANDLE self_dsv = dsv_cpu(s, feedback->dsv_index);
                        s.list->OMSetRenderTargets(1u, &self_rtv, FALSE, &self_dsv);
                        srv_index = feedback->feedback_srv_index;
                        ++s.report.vram_feedback_refreshes;
                        ++s.report.dx12_gpu_feedback_draws;
                        ++s.report.dx12_self_feedback_snapshots;
                    } else if (!feedback_error.empty()) {
                        runtime_log_error("dx12 self-feedback", feedback_error);
                    }
                } else {
                    resolve_target_for_sampling(s, *feedback, false);
                    srv_index = feedback->srv_index;
                    ++s.report.vram_feedback_refreshes;
                    ++s.report.dx12_gpu_feedback_draws;
                }
                if (feedback_address == s.display_framebuffer)
                    ++s.report.display_framebuffer_sampled_draws;
            } else {
                if (Dx12Texture *texture = find_cached_texture(s, texture_key(batch.draw));
                    texture != nullptr && texture->image) {
                    srv_index = texture->srv_index;
                    sampler_index = texture->sampler_index;
                }
            }
        }
        if (srv_index != bound_srv) {
            s.list->SetGraphicsRootDescriptorTable(0u, srv_gpu(s, srv_index));
            bound_srv = srv_index;
        }
        if (sampler_index != bound_sampler) {
            s.list->SetGraphicsRootDescriptorTable(1u, sampler_gpu(s, sampler_index));
            bound_sampler = sampler_index;
        }

        // Discover an indirect run before writing root constants. If the run
        // fits the dedicated arena, ExecuteIndirect owns those constants too,
        // avoiding even the first scalar SetGraphicsRoot32BitConstants pair.
        std::size_t indirect_end = batch_cursor + 1u;
        if (indirect_enabled && !trace_cloud_frame && !batch.framebuffer_feedback &&
            !batch.draw.clear_mode) {
            while (indirect_end < s.batches.size() &&
                   indirect_run_compatible(batch, s.batches[indirect_end])) {
                ++indirect_end;
            }
        }
        const std::size_t indirect_count = indirect_end - batch_cursor;
        const std::size_t indirect_stride = batch.indexed
            ? sizeof(Dx12IndirectDrawIndexedCommand)
            : sizeof(Dx12IndirectDrawCommand);
        const std::size_t indirect_command_bytes = indirect_stride * indirect_count;
        const bool execute_indirect_run = indirect_count >= 3u &&
            indirect_cursor + indirect_command_bytes <= kIndirectUploadCapacity;

        const std::uint32_t logical_width = current_target != nullptr && current_target->logical_width != 0u
            ? current_target->logical_width : kReferenceWidth;
        const std::uint32_t logical_height = current_target != nullptr && current_target->logical_height != 0u
            ? current_target->logical_height : kReferenceHeight;
        if (!execute_indirect_run) {
            const Dx12TransformConstants draw_transform =
                make_transform_constants(batch, logical_width, logical_height);
            const Dx12PixelConstants pixel_state = make_pixel_constants(batch.draw, srv_index != 0u);
            const std::array<float, 16> shadow_rows = make_shadow_rows(batch);
            if (!active_transform_valid ||
                std::memcmp(&draw_transform, &active_transform, sizeof(draw_transform)) != 0) {
                s.list->SetGraphicsRoot32BitConstants(2u, 40u, &draw_transform, 0u);
                active_transform = draw_transform;
                active_transform_valid = true;
            }
            if (!active_pixel_valid ||
                std::memcmp(&pixel_state, &active_pixel, sizeof(pixel_state)) != 0) {
                s.list->SetGraphicsRoot32BitConstants(3u, 5u, &pixel_state, 0u);
                active_pixel = pixel_state;
                active_pixel_valid = true;
            }
            if (!active_shadow_rows_valid || shadow_rows != active_shadow_rows) {
                s.list->SetGraphicsRoot32BitConstants(3u, 16u, shadow_rows.data(), 5u);
                active_shadow_rows = shadow_rows;
                active_shadow_rows_valid = true;
            }
        }

        // The PSP scissor is expressed in 480x272 logical pixels and has to be
        // remapped onto a render target that is normally several times larger.
        // Both edges must round *inwards*: truncating a leading edge starts the
        // rectangle up to (scale - 1) device pixels early, which admits a sliver
        // of the neighbouring logical pixel that the guest scissored away. VCS
        // draws the radar map tiles clipped to the radar's bounding box and then
        // covers them with a circular mask, so that sliver escaped above the
        // mask as a thin horizontal line over the minimap. Trailing edges were
        // already truncated, which is the inward direction for them; only the
        // leading edges were wrong. At native 480x272 the scale is 1 and both
        // roundings are exact, which is why this only ever showed at upscaled
        // internal resolutions.
        const auto scale_leading = [](std::int32_t value, std::uint32_t target,
                                      std::uint32_t logical) {
            const std::int64_t denominator = std::max<std::uint32_t>(1u, logical);
            const std::int64_t numerator =
                static_cast<std::int64_t>(value) * target + denominator - 1;
            return static_cast<LONG>(std::clamp<std::int64_t>(
                numerator / denominator, 0, static_cast<std::int64_t>(target)));
        };
        const auto scale_trailing = [](std::int32_t value, std::uint32_t target,
                                       std::uint32_t logical) {
            return static_cast<LONG>(std::clamp<std::int64_t>(
                static_cast<std::int64_t>(value) * target /
                    std::max<std::uint32_t>(1u, logical),
                0, static_cast<std::int64_t>(target)));
        };
        D3D12_RECT scissor{
            scale_leading(batch.draw.scissor_x0, s.target_width, logical_width),
            scale_leading(batch.draw.scissor_y0, s.target_height, logical_height),
            scale_trailing(batch.draw.scissor_x1 + 1, s.target_width, logical_width),
            scale_trailing(batch.draw.scissor_y1 + 1, s.target_height, logical_height)};
        if (scissor.right <= scissor.left || scissor.bottom <= scissor.top) continue;
        if (!active_scissor_valid ||
            std::memcmp(&scissor, &active_scissor, sizeof(scissor)) != 0) {
            s.list->RSSetScissorRects(1u, &scissor);
            active_scissor = scissor;
            active_scissor_valid = true;
        }
        const Dx12BlendPlan blend_plan = dx12_blend_plan(batch.draw);
        if (blend_plan.uses_constant) {
            const std::uint32_t fix = blend_plan.constant_rgb;
            if (fix != active_blend_fix) {
                const float factors[4]{
                    static_cast<float>(fix & 0xFFu) / 255.0f,
                    static_cast<float>((fix >> 8u) & 0xFFu) / 255.0f,
                    static_cast<float>((fix >> 16u) & 0xFFu) / 255.0f,
                    1.0f};
                s.list->OMSetBlendFactor(factors);
                active_blend_fix = fix;
            }
        }
        // V4 high-margin path: collapse adjacent draws that differ only in
        // per-draw root constants into one ExecuteIndirect call. No draw is
        // reordered and framebuffer-feedback/clear boundaries never participate.
        if (execute_indirect_run) {
                const std::size_t stride = indirect_stride;
                const std::size_t command_bytes = indirect_command_bytes;
                std::byte *command_dst = frame.mapped_indirect_upload + indirect_cursor;
                for (std::size_t j = batch_cursor; j < indirect_end; ++j) {
                    const Dx12Batch &ibatch = s.batches[j];
                    const Dx12TransformConstants itransform =
                        make_transform_constants(ibatch, logical_width, logical_height);
                    const Dx12PixelConstants ipixel =
                        make_pixel_constants(ibatch.draw, srv_index != 0u);
                    if (batch.indexed) {
                        Dx12IndirectDrawIndexedCommand command{};
                        std::memcpy(command.transform, &itransform, sizeof(itransform));
                        std::memcpy(command.pixel_and_shadow, &ipixel, sizeof(ipixel));
                        const std::array<float, 16> shadow_rows = make_shadow_rows(ibatch);
                        std::memcpy(command.pixel_and_shadow + 5u, shadow_rows.data(),
                                    sizeof(shadow_rows));
                        command.draw.IndexCountPerInstance = ibatch.index_count;
                        command.draw.InstanceCount = 1u;
                        command.draw.StartIndexLocation = ibatch.first_index;
                        command.draw.BaseVertexLocation = static_cast<INT>(ibatch.first_vertex);
                        command.draw.StartInstanceLocation = 0u;
                        std::memcpy(command_dst, &command, sizeof(command));
                    } else {
                        Dx12IndirectDrawCommand command{};
                        std::memcpy(command.transform, &itransform, sizeof(itransform));
                        std::memcpy(command.pixel_and_shadow, &ipixel, sizeof(ipixel));
                        const std::array<float, 16> shadow_rows = make_shadow_rows(ibatch);
                        std::memcpy(command.pixel_and_shadow + 5u, shadow_rows.data(),
                                    sizeof(shadow_rows));
                        command.draw.VertexCountPerInstance = ibatch.vertex_count;
                        command.draw.InstanceCount = 1u;
                        command.draw.StartVertexLocation = ibatch.first_vertex;
                        command.draw.StartInstanceLocation = 0u;
                        std::memcpy(command_dst, &command, sizeof(command));
                    }
                    command_dst += stride;
                }
                ID3D12CommandSignature *signature = batch.indexed
                    ? s.indirect_draw_indexed_signature.Get()
                    : s.indirect_draw_signature.Get();
                s.list->ExecuteIndirect(signature, static_cast<UINT>(indirect_count),
                                        frame.indirect_upload_buffer.Get(), indirect_cursor,
                                        nullptr, 0u);
                indirect_cursor += command_bytes;
                executed_batches += static_cast<std::uint32_t>(indirect_count);
                ++s.report.dx12_indirect_executes;
                s.report.dx12_indirect_draws += indirect_count;
                s.report.dx12_indirect_saved_api_draws += indirect_count - 1u;
                for (std::size_t j = batch_cursor; j < indirect_end; ++j)
                    account_executed_batch(s, s.batches[j], srv_index, blend_plan);
                batch_cursor = indirect_end - 1u;
                cloud_batch_index += indirect_count - 1u;
                // ExecuteIndirect leaves all 21 root constants equal to the
                // last command; force every scalar cache to repopulate before
                // the next ordinary draw.
                active_transform_valid = false;
                active_pixel_valid = false;
                active_shadow_rows_valid = false;
                continue;
        }

        if (batch.indexed)
            s.list->DrawIndexedInstanced(batch.index_count, 1u, batch.first_index,
                                         static_cast<INT>(batch.first_vertex), 0u);
        else
            s.list->DrawInstanced(batch.vertex_count, 1u, batch.first_vertex, 0u);
        ++executed_batches;
        account_executed_batch(s, batch, srv_index, blend_plan);
    }
    if (trace_cloud_frame) {
        runtime_log_line(std::string("CLOUD_TRACE_END injected=") +
                         (clouds_injected ? "1" : "0"));
        cloud_trace_done = true;
    }

    if (current_target != nullptr)
        resolve_target_for_sampling(s, *current_target, false);

    s.report.dx12_gpu_draw_calls += executed_batches;
    if (executed_batches == 0u) {
        hr = s.list->Close();
        if (SUCCEEDED(hr)) {
            ID3D12CommandList *upload_lists[]{s.list.Get()};
            s.queue->ExecuteCommandLists(1u, upload_lists);
            frame.fence_value = s.next_fence++;
            hr = s.queue->Signal(s.fence.Get(), frame.fence_value);
            if (FAILED(hr))
                runtime_log_error("dx12 ge", hr_text(hr, "ID3D12CommandQueue::Signal(upload-only)"));
            s.frame_cursor = (s.frame_cursor + 1u) % kFrameCount;
        } else {
            runtime_log_error("dx12 ge", hr_text(hr, "CommandList::Close(upload-only)"));
        }
        clear_accumulation(s);
        ++s.frame_epoch;
        return false;
    }

    display_target = find_framebuffer_target(s, s.display_framebuffer);
    const bool display_ready = touched_display && display_target != nullptr && display_target->color;
    if (!display_ready) {
        ++s.report.frames_without_displayed_target;
        if (++s.missed_display_intervals > 4u) {
            s.direct_present_ok = false;
            s.presented_framebuffer = 0u;
        }
    }

    if (s.readback_enabled && s.readback_buffer && display_ready) {
        transition(s.list.Get(), display_target->color.Get(), display_target->color_state,
                   D3D12_RESOURCE_STATE_COPY_SOURCE);
        display_target->color_state = D3D12_RESOURCE_STATE_COPY_SOURCE;
        D3D12_TEXTURE_COPY_LOCATION src{};
        src.pResource = display_target->color.Get();
        src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION dst{};
        dst.pResource = s.readback_buffer.Get();
        dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        dst.PlacedFootprint = s.readback_footprint;
        s.list->CopyTextureRegion(&dst, 0u, 0u, 0u, &src, nullptr);
        transition(s.list.Get(), display_target->color.Get(), display_target->color_state,
                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        display_target->color_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }

    bool recorded_present = false;
    if (direct_possible && display_ready) {
        std::string present_error;
        recorded_present = record_direct_present(s, *display_target, present_error);
        if (!recorded_present && !present_error.empty())
            runtime_log_error("dx12 ge direct present", present_error);
    }

    hr = s.list->Close();
    if (FAILED(hr)) {
        runtime_log_error("dx12 ge", hr_text(hr, "CommandList::Close"));
        clear_accumulation(s);
        return false;
    }
    ID3D12CommandList *lists[]{s.list.Get()};
    s.queue->ExecuteCommandLists(1u, lists);

    bool presented = false;
    if (recorded_present && s.swapchain) {
        hr = s.swapchain->Present(0u, s.swapchain_tearing ? DXGI_PRESENT_ALLOW_TEARING : 0u);
        if (SUCCEEDED(hr)) {
            presented = true;
            s.direct_present_ok = true;
            s.presented_framebuffer = s.display_framebuffer;
            s.missed_display_intervals = 0u;
            s.report.gpu_frame_presented_to_window = true;
        } else {
            runtime_log_error("dx12 ge present", hr_text(hr, "IDXGISwapChain::Present"));
            s.direct_present_ok = false;
            s.presented_framebuffer = 0u;
        }
    }

    frame.fence_value = s.next_fence++;
    hr = s.queue->Signal(s.fence.Get(), frame.fence_value);
    if (FAILED(hr)) {
        runtime_log_error("dx12 ge", hr_text(hr, "ID3D12CommandQueue::Signal(frame)"));
        clear_accumulation(s);
        return false;
    }

    if (s.readback_enabled && s.readback_buffer && display_ready) {
        if (!wait_for_fence(s, frame.fence_value, error)) {
            runtime_log_error("dx12 ge readback wait", error);
        } else {
            void *mapped = nullptr;
            const D3D12_RANGE read_range{0u, static_cast<SIZE_T>(s.readback_bytes)};
            hr = s.readback_buffer->Map(0u, &read_range, &mapped);
            if (SUCCEEDED(hr) && mapped != nullptr) {
                const auto *source = static_cast<const std::byte *>(mapped) + s.readback_footprint.Offset;
                const std::size_t row_bytes = static_cast<std::size_t>(s.target_width) * 4u;
                for (std::uint32_t y = 0u; y < s.target_height; ++y)
                    std::memcpy(s.frame_rgba.data() + static_cast<std::size_t>(y) * row_bytes,
                                source + static_cast<std::size_t>(y) * s.readback_footprint.Footprint.RowPitch,
                                row_bytes);
                const D3D12_RANGE no_write{0u, 0u};
                s.readback_buffer->Unmap(0u, &no_write);
            }
        }
    }

    ++s.report.game_frames;
    ++s.report.transfer_submissions;
    s.report.transfer_bytes += bytes;
    s.report.game_frame_vblank = vblank;
    s.report.game_frame_readback_bytes = (s.readback_enabled && display_ready) ? s.frame_rgba.size() : 0u;
    s.report.presented_framebuffer_target = display_ready ? s.display_framebuffer : 0u;
    s.report.release_candidate_ready = s.enabled && display_ready &&
        (presented || s.readback_enabled) && s.report.transfer_self_test_passed &&
        s.report.offscreen_self_test_passed && s.report.texture_descriptor_layout_created &&
        s.report.depth_attachment_active && s.report.alpha_test_shader_active &&
        s.report.observed_blend_modes_pipeline_active && s.report.fog_shader_active;
    s.report.swapchain_active = s.swapchain != nullptr;
    clear_accumulation(s);
    s.frame_cursor = (s.frame_cursor + 1u) % kFrameCount;
    ++s.frame_epoch;
    return presented || (s.readback_enabled && display_ready);
}

bool ge_gpu_backend_copy_game_frame_rgba(std::span<std::byte> destination) noexcept {
    const Dx12GeState &s = state();
    if (s.frame_rgba.empty() || destination.size() < s.frame_rgba.size()) return false;
    std::memcpy(destination.data(), s.frame_rgba.data(), s.frame_rgba.size());
    return true;
}

bool ge_gpu_backend_presents_directly() noexcept {
    const Dx12GeState &s = state();
    return s.enabled && s.swapchain != nullptr && s.direct_present_ok;
}
std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept {
    const Dx12GeState &s = state();
    return s.enabled && s.swapchain != nullptr && s.direct_present_ok
        ? s.presented_framebuffer : 0u;
}
std::uint32_t ge_gpu_backend_display_framebuffer() noexcept { return state().display_framebuffer; }
std::span<const std::byte> ge_gpu_backend_game_frame_rgba() noexcept {
    const Dx12GeState &s = state();
    return s.frame_rgba.empty() ? std::span<const std::byte>{}
                                : std::span<const std::byte>(s.frame_rgba.data(), s.frame_rgba.size());
}
bool ge_gpu_backend_copy_offscreen_rgba(std::span<std::byte> destination) noexcept {
    return ge_gpu_backend_copy_game_frame_rgba(destination);
}
void ge_gpu_backend_mark_window_presented() noexcept { state().report.gpu_frame_presented_to_window = true; }
GeGpuBackendReport ge_gpu_backend_report() {
    auto &s = state();
    s.report.dx12_srv_high_water = std::max<std::uint64_t>(s.report.dx12_srv_high_water, s.next_srv);
    return s.report;
}

#else

namespace {
struct Dx12StubState { GeGpuBackendReport report{}; std::uint32_t display_framebuffer{}; };
Dx12StubState &state() { static Dx12StubState s; return s; }
}
bool initialize_ge_gpu_backend(std::string &error) {
    auto &s = state(); s = {};
    s.report.requested = GeGpuBackendKind::DirectX12;
    s.report.active = GeGpuBackendKind::Software;
    s.report.message = "DirectX 12 GE backend is available only on Windows";
    error.clear(); return true;
}
void shutdown_ge_gpu_backend() noexcept { state() = {}; }
bool ge_gpu_backend_active() noexcept { return false; }
bool ge_gpu_backend_transfer_ready() noexcept { return false; }
bool ge_gpu_backend_graphics_ready() noexcept { return false; }
void ge_gpu_backend_record_draw(const GeGpuDrawDescriptor &) noexcept {}
void ge_gpu_backend_observe_camera(const std::array<float, 12> &,
                                   const std::array<float, 16> &,
                                   const std::array<float, 8> &,
                                   const std::array<float, 3> &,
                                   const GeGpuDrawDescriptor &,
                                   std::uint32_t) noexcept {}
bool ge_gpu_backend_stage_vertices(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex>) noexcept { return false; }
bool ge_gpu_backend_texture_needed(const GeGpuDrawDescriptor &) noexcept { return false; }
void ge_gpu_backend_prepare_texture_keys(GeGpuDrawDescriptor &) noexcept {}
bool ge_gpu_backend_texture_signature_needed(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_is_framebuffer_feedback_texture(const GeGpuDrawDescriptor &) noexcept { return false; }
GeGpuWidescreenHud ge_gpu_backend_widescreen_hud(const GeGpuDrawDescriptor &) noexcept { return {}; }
void ge_gpu_backend_note_through_extent(const GeGpuDrawDescriptor &, float, float) noexcept {}
bool ge_gpu_backend_adopt_shared_texture(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_texture_available(const GeGpuDrawDescriptor &) noexcept { return false; }
bool ge_gpu_backend_upload_decoded_texture(const GeGpuDrawDescriptor &, std::uint32_t, std::uint32_t, std::span<const std::byte>) noexcept { return false; }
bool ge_gpu_backend_upload_decoded_texture_chain(const GeGpuDrawDescriptor &, std::span<const GeGpuDecodedMipLevel>) noexcept { return false; }
bool ge_gpu_backend_upload_decoded_texture_chain_packed(const GeGpuDrawDescriptor &, std::uint32_t, std::uint32_t, std::uint32_t, std::vector<std::byte>) noexcept { return false; }
bool ge_gpu_backend_copy_last_texture_rgba(std::span<std::byte>) noexcept { return false; }
void ge_gpu_backend_accumulate_color_triangles(const GeGpuDrawDescriptor &, std::span<const GeGpuVertex>) noexcept {}
void ge_gpu_backend_accumulate_hardware_triangles(const GeGpuDrawDescriptor &, const GeGpuHardwareTransform &, std::span<const GeGpuVertex>, std::span<const std::uint32_t>) noexcept {}
bool ge_gpu_backend_accumulate_hardware_packed_0115(const GeGpuDrawDescriptor &, const GeGpuHardwareTransform &, std::span<const std::byte>, std::uint32_t, std::span<const std::uint32_t>) noexcept { return false; }
void ge_gpu_backend_set_native_window(void *) noexcept {}
void ge_gpu_backend_set_display_framebuffer(std::uint32_t address) noexcept { state().display_framebuffer = address & 0x001FFFF0u; }
bool ge_gpu_backend_finish_color_frame(std::uint64_t) noexcept { return false; }
bool ge_gpu_backend_copy_game_frame_rgba(std::span<std::byte>) noexcept { return false; }
bool ge_gpu_backend_presents_directly() noexcept { return false; }
std::uint32_t ge_gpu_backend_owned_framebuffer() noexcept { return 0u; }
std::uint32_t ge_gpu_backend_display_framebuffer() noexcept { return state().display_framebuffer; }
std::span<const std::byte> ge_gpu_backend_game_frame_rgba() noexcept { return {}; }
bool ge_gpu_backend_copy_offscreen_rgba(std::span<std::byte>) noexcept { return false; }
void ge_gpu_backend_mark_window_presented() noexcept {}
GeGpuBackendReport ge_gpu_backend_report() { return state().report; }

#endif

const char *ge_gpu_backend_name(GeGpuBackendKind kind) noexcept {
    switch (kind) {
    case GeGpuBackendKind::Software: return "software";
    case GeGpuBackendKind::DirectX12: return "directx12";
    }
    return "unknown";
}

} // namespace vcs
