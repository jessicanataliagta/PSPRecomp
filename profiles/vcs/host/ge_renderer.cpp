#include "ge_renderer.hpp"
#include "ge_gpu_backend.hpp"
#include "propershaders/ProperShadersBridge.hpp"
#include "vcs_config.hpp"
#include "vcs_project2dfx.hpp"
#include "vcs_fps_overlay.hpp"
#include "savedata_utility_ui.hpp"
#include "vcs_texture_replacement.hpp"

#include "psprecomp/common.hpp"

#include <algorithm>
#include <atomic>
#include <bit>
#include <array>
#include <cmath>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <type_traits>
#include <limits>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#define PSPRECOMP_GE_X86_SIMD 1
#else
#define PSPRECOMP_GE_X86_SIMD 0
#endif

namespace vcs {

namespace {

// Scalar helpers that the C library would otherwise turn into calls.
//
// A bilinear-filtered fragment ran about thirty function calls: sixteen
// std::lround (which MSVC resolves to __imp_lroundf, an *indirect* call through
// the CRT import table), roughly ten std::floor, and several std::isfinite --
// each of which is a single x86-64 instruction.  Measured at ~400 cycles per
// fragment, that call traffic was the frame budget.  These replacements compute
// exactly the same values inline.

// Same predicate as std::isfinite: exponent field all ones means inf or NaN.
[[nodiscard]] inline bool finite_float(float value) noexcept {
    return (std::bit_cast<std::uint32_t>(value) & 0x7F800000u) != 0x7F800000u;
}

// Same value as static_cast<std::int32_t>(std::floor(value)), for inputs that
// fit in std::int32_t.  Anything outside that range was already undefined at
// the cast, so the guard only has to keep this function itself well defined.
// Kept branchless on purpose.  A first attempt used `? :` on the correction and
// measured *slower* than the CRT call it replaced: the comparison is
// data-dependent and mispredicts, while lroundf/floorf are branchless SSE.
[[nodiscard]] inline std::int32_t floor_to_int(float value) noexcept {
    if (!(value > -2147483000.0f && value < 2147483000.0f))
        return static_cast<std::int32_t>(std::floor(value));
    const std::int32_t truncated = static_cast<std::int32_t>(value);  // toward zero
    return truncated - static_cast<std::int32_t>(static_cast<float>(truncated) > value);
}

// Same value as std::floor, for inputs inside the 32-bit integer range.
[[nodiscard]] inline float floor_float(float value) noexcept {
    if (!(value > -2147483000.0f && value < 2147483000.0f)) return std::floor(value);
    const float truncated = static_cast<float>(static_cast<std::int32_t>(value));
    return truncated - static_cast<float>(truncated > value);
}

// Same value as static_cast<std::uint8_t>(std::clamp(std::lround(value), 0, 255)).
//
// std::lround rounds to nearest with ties away from zero.  Inside (-0.5, 255.5)
// that is computed here from the exact fractional part -- `value - trunc(value)`
// is exact for |value| < 2^24 -- and outside it the clamp alone already decides
// the answer, so the rare path defers to std::lround and stays identical
// (NaN included, since both comparisons are false for it).
[[nodiscard]] inline std::uint8_t round_clamp_to_byte(float value) noexcept {
    if (value > -0.5f && value < 255.5f) {
        const long truncated = static_cast<long>(value);
        const float fraction = value - static_cast<float>(truncated);
        const long rounded = truncated + static_cast<long>(fraction >= 0.5f) -
                                         static_cast<long>(fraction <= -0.5f);
        return static_cast<std::uint8_t>(rounded < 0L ? 0L : (rounded > 255L ? 255L : rounded));
    }
    return static_cast<std::uint8_t>(std::clamp(std::lround(value), 0L, 255L));
}

// Packed IEEE divisions preserve the result of each scalar division while
// issuing one vector divide for two, three or four independent channels.
// Numerators are still formed in the original scalar order, so framebuffer
// results remain bit-identical to the old path.
inline void divide2_same_denominator(float n0, float n1, float denominator,
                                     float &out0, float &out1) noexcept {
#if PSPRECOMP_GE_X86_SIMD
    alignas(16) float values[4]{n0, n1, 0.0f, 0.0f};
    const __m128 result = _mm_div_ps(_mm_load_ps(values), _mm_set1_ps(denominator));
    _mm_store_ps(values, result);
    out0 = values[0];
    out1 = values[1];
#else
    out0 = n0 / denominator;
    out1 = n1 / denominator;
#endif
}

inline void divide3_same_denominator(float n0, float n1, float n2, float denominator,
                                     float &out0, float &out1, float &out2) noexcept {
#if PSPRECOMP_GE_X86_SIMD
    alignas(16) float values[4]{n0, n1, n2, 0.0f};
    const __m128 result = _mm_div_ps(_mm_load_ps(values), _mm_set1_ps(denominator));
    _mm_store_ps(values, result);
    out0 = values[0];
    out1 = values[1];
    out2 = values[2];
#else
    out0 = n0 / denominator;
    out1 = n1 / denominator;
    out2 = n2 / denominator;
#endif
}

inline void divide4_same_denominator(float n0, float n1, float n2, float n3,
                                     float denominator, float (&out)[4]) noexcept {
#if PSPRECOMP_GE_X86_SIMD
    alignas(16) float values[4]{n0, n1, n2, n3};
    const __m128 result = _mm_div_ps(_mm_load_ps(values), _mm_set1_ps(denominator));
    _mm_store_ps(values, result);
    out[0] = values[0]; out[1] = values[1]; out[2] = values[2]; out[3] = values[3];
#else
    out[0] = n0 / denominator; out[1] = n1 / denominator;
    out[2] = n2 / denominator; out[3] = n3 / denominator;
#endif
}

// Phase split for the frame budget, under PSPRECOMP_GE_PHASE_DIAG.
//
// ge_us alone cannot say whether a heavy frame is spent per triangle (clipping,
// viewport transform, culling, bounding box) or per fragment (the pixel loop).
// Guessing that wrong costs a full measurement cycle, so both are timed
// directly.  The GE list runs on one thread, so plain counters suffice; the
// clock reads are only taken when the diagnostic is on.
bool ge_phase_diag_enabled() noexcept {
    static const bool enabled = std::getenv("PSPRECOMP_GE_PHASE_DIAG") != nullptr;
    return enabled;
}
std::uint64_t g_ge_pixel_ns{};
std::uint64_t g_ge_triangle_count{};
// Stage 35 sub-phase accumulators.  Written only while the diagnostic is on and
// only at draw-call granularity; see GePhaseTotals for why.
std::uint64_t g_ge_draw_setup_ns{};
std::uint64_t g_ge_texture_upload_ns{};
std::uint64_t g_ge_vertex_decode_ns{};
std::uint64_t g_ge_gpu_stage_ns{};
std::uint64_t g_ge_triangle_prep_ns{};
std::uint64_t g_ge_gpu_accumulate_ns{};
std::uint64_t g_ge_primitive_count{};
std::uint64_t g_ge_vertex_count{};

// Accumulates into `sink` only when the phase diagnostic is enabled, so the
// production path pays one predictable branch and no clock read.
struct PhaseTimer {
    std::uint64_t *sink;
    std::chrono::steady_clock::time_point entry;
    explicit PhaseTimer(std::uint64_t &target) noexcept
        : sink(ge_phase_diag_enabled() ? &target : nullptr),
          entry(sink != nullptr ? std::chrono::steady_clock::now()
                                : std::chrono::steady_clock::time_point{}) {}
    ~PhaseTimer() {
        if (sink == nullptr) return;
        *sink += static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - entry).count());
    }
    PhaseTimer(const PhaseTimer &) = delete;
    PhaseTimer &operator=(const PhaseTimer &) = delete;
};

// The Stage 22 vertex staging path copies every triangle-stream vertex into the
// mapped upload ring, but nothing ever reads those bytes back: the frame that
// Vulkan actually draws is assembled from the prepared screen triangles in
// ge_gpu_backend, and finish_color_frame overwrites the same region with them.
// It was a bring-up proof that geometry could cross the host->Vulkan boundary,
// and it stayed on the per-draw path.  Off by default now; set the variable to
// 1 to restore it for an A/B or for the transfer self-test.
bool legacy_vertex_staging_enabled() noexcept {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_STAGE_VERTICES");
        return text != nullptr && *text != '\0' && std::strcmp(text, "0") != 0;
    }();
    return enabled;
}


bool gpu_hardware_transform_enabled() noexcept {
    // Rendering.HardwareTransform decides; the environment variable still wins
    // when set, so an A/B in one binary stays possible.
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_HW_TRANSFORM");
        if (text != nullptr && *text != '\0') return std::strcmp(text, "0") != 0;
        const vcs::VcsConfiguration &config = vcs::vcs_configuration();
        return config.initialized && config.rendering.hardware_transform;
    }();
    return enabled;
}

// Off by default: the GPU face-culling path drops whole models.
//
// It shipped enabled, but the hardware frontend only covered 5% of city
// vertices, so the defect was almost invisible. Once the hybrid lit path took
// coverage to 96.6%, large pieces of scenery disappeared -- confirmed by the
// user, and confirmed fixed by setting this to 0 with everything else unchanged.
//
// The cause is NOT the world matrix handedness. That was tried and it is wrong
// reasoning: the model-space path and the hybrid path apply exactly the same
// total transform, only split differently between CPU and shader, so the
// winding the GPU sees is identical in both. The bug is in this cull path
// itself and is still unexplained.
//
// Culling on the CPU still happens for everything on the legacy path, and the
// GPU simply rasterizes the back faces it is given -- a fill-rate cost on a
// discrete GPU that measured well below the win from the hybrid transform.
//
// Now on by default: measured on real gameplay it is part of the configuration
// that runs fastest on this profile, and the launcher scripts had been setting
// it by hand ever since. PSPRECOMP_GE_GPU_HW_CULL=0 turns it back off for a
// compatibility bisect.
bool gpu_hardware_cull_enabled() noexcept {
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_HW_CULL");
        if (text == nullptr || *text == '\0') return true;
        return std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
    }();
    return enabled;
}

// Below this many covered pixels a primitive is not worth waking workers for;
// the crossover is where dispatch stops being visible next to the covered area.
std::int64_t parallel_pixel_threshold() noexcept {
    static const std::int64_t threshold = [] {
        constexpr std::int64_t default_value = 2048;
        const char *text = std::getenv("PSPRECOMP_RASTER_PARALLEL_PIXELS");
        if (text == nullptr || *text == '\0') return default_value;
        char *end = nullptr;
        const unsigned long long value = std::strtoull(text, &end, 10);
        if (end == text || *end != '\0' || value > 16'777'216ull) return default_value;
        return static_cast<std::int64_t>(value);
    }();
    return threshold;
}

// Hardware-transform vertex decoding is pure host-side work: each decoded
// vertex reads immutable guest bytes for the duration of this synchronous GE
// draw and writes one independent output slot.  Large/skinned/lit meshes are
// therefore safe to fan out across the same persistent host worker pool used
// by raster/texture work.  Keep small draws serial because waking participants
// costs more than decoding a few dozen simple vertices.
bool parallel_vertex_decode_enabled() noexcept {
    // The VCS executable installs this as an enabled-by-default performance
    // path; keeping the profile layer environment-gated preserves a clean
    // PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=0 A/B fallback and test isolation.
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_DECODE");
        return text != nullptr && *text != '\0' && std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
    }();
    return enabled;
}

bool packed_0115_gpu_decode_enabled() noexcept {
    // Stage 45.4: the dominant VCS world format can remain in its original
    // 10-byte PSP encoding all the way to the DX12 input assembler.  Unlike
    // the old parallel-decode experiment this removes CPU work instead of
    // distributing it across more host threads. Keep an A/B switch for parity.
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_DX12_PACKED_0115");
        if (text == nullptr || *text == '\0') return true;
        return std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
    }();
    return enabled;
}

bool direct_nonindexed_gpu_draw_enabled() noexcept {
    // The crash fix this was gated behind has since been validated on the
    // user's physical driver, and the production GE probe in the build script
    // exercises it on every build. Default on; =0 restores the old path.
    static const bool enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW");
        if (text == nullptr || *text == '\0') return true;
        return std::strcmp(text, "0") != 0 &&
               std::strcmp(text, "false") != 0 && std::strcmp(text, "FALSE") != 0 &&
               std::strcmp(text, "off") != 0 && std::strcmp(text, "OFF") != 0;
    }();
    return enabled;
}

std::size_t parallel_vertex_decode_threshold(bool expensive_vertex) noexcept {
    static const std::size_t simple_threshold = [] {
        constexpr std::size_t default_value = 256u;
        const char *text = std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_THRESHOLD");
        if (text == nullptr || *text == '\0') return default_value;
        char *end = nullptr;
        const unsigned long long value = std::strtoull(text, &end, 10);
        if (end == text || *end != '\0' || value > 1'048'576ull) return default_value;
        return static_cast<std::size_t>(value);
    }();
    // Skinning and CPU lighting perform substantially more math and guest
    // loads per vertex, so their worker crossover is lower.
    return expensive_vertex ? std::max<std::size_t>(64u, simple_threshold / 2u)
                            : simple_threshold;
}

unsigned parallel_vertex_decode_max_participants() noexcept {
    // The old experiment could wake every logical CPU (up to 32) for a PSP
    // draw.  That creates a synchronization storm on desktop CPUs and made the
    // experiment unnecessarily risky.  Stage 45.4 deliberately bounds the
    // default to six participants; the main thread is one of them, so at most
    // five workers are woken.  The environment variable remains available for
    // A/B tuning without rebuilding.
    static const unsigned participants = [] {
        constexpr unsigned default_value = 6u;
        const char *text = std::getenv("PSPRECOMP_GE_PARALLEL_VERTEX_MAX_WORKERS");
        if (text == nullptr || *text == '\0') return default_value;
        char *end = nullptr;
        const unsigned long value = std::strtoul(text, &end, 10);
        if (end == text || *end != '\0' || value < 1u || value > 32u)
            return default_value;
        return static_cast<unsigned>(value);
    }();
    return participants;
}

inline void raster_cpu_relax() noexcept {
#if PSPRECOMP_GE_X86_SIMD
    _mm_pause();
#else
    // A compiler barrier is enough for the short optimistic spin on platforms
    // without an explicit pause instruction. The loop eventually parks using
    // atomic::wait, so this cannot become an unbounded busy wait.
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

struct PixelLoopTimer {
    std::chrono::steady_clock::time_point entry;
    PixelLoopTimer() : entry(std::chrono::steady_clock::now()) {}
    ~PixelLoopTimer() {
        g_ge_pixel_ns += static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - entry).count());
    }
};

// Scanline-parallel rasterization.
//
// Software rasterization is roughly three quarters of a heavy frame and it ran
// on one core.  Inside a single triangle every covered pixel is written exactly
// once, so splitting its bounding box into horizontal row ranges is order-free:
// no two workers touch the same pixel, and the result is identical to the
// serial loop regardless of how the rows are distributed.  Ordering between
// primitives is untouched because each triangle is still finished before the
// next one starts.
//
// PSPRECOMP_RASTER_THREADS overrides the worker count; 1 restores the fully
// serial path.
class RowWorkerPool {
public:
    static constexpr unsigned kMaxThreads = 32u;

    static RowWorkerPool &instance() {
        static RowWorkerPool pool;
        return pool;
    }

    [[nodiscard]] unsigned worker_count() const noexcept { return worker_count_; }

    // Splits [first, last] into small contiguous row ranges and runs `body`
    // across the caller plus worker threads. The callback receives a stable
    // participant slot, so each thread can accumulate statistics without atomics.
    template <typename Body>
    void run(std::int32_t first, std::int32_t last, Body &&body,
             unsigned participant_limit = kMaxThreads) {
        const std::int32_t rows = last - first + 1;
        const unsigned participants = std::min<unsigned>(
            std::min(worker_count_, participant_limit), static_cast<unsigned>(rows));
        if (participants <= 1u || workers_.empty()) {
            body(0u, first, last);
            return;
        }

        const unsigned chunks = std::min<unsigned>(
            static_cast<unsigned>(rows), participants * kChunksPerParticipant);
        using BodyType = std::remove_reference_t<Body>;
        body_context_ = static_cast<void *>(&body);
        body_invoke_ = [](void *context, unsigned participant,
                          std::int32_t begin, std::int32_t end) {
            (*static_cast<BodyType *>(context))(participant, begin, end);
        };
        range_first_ = first;
        rows_per_chunk_ = rows / static_cast<std::int32_t>(chunks);
        remainder_ = rows % static_cast<std::int32_t>(chunks);
        chunk_count_ = chunks;
        active_participants_ = participants;
        next_chunk_.store(0u, std::memory_order_relaxed);
        pending_.store(participants - 1u, std::memory_order_relaxed);

        // Publishing a new generation releases the callback and range fields.
        // Workers acquire that generation before touching any of them.
        generation_.fetch_add(1u, std::memory_order_release);
        if (parked_.load(std::memory_order_acquire) != 0u)
            generation_.notify_all();

        run_available_chunks(0u);

        // A primitive normally completes in tens of microseconds. The caller
        // cannot make progress until every row is done, so parking it in the
        // kernel only adds wake latency. PAUSE keeps this join entirely in user
        // space without the scheduler storm caused by sched_yield.
        while (pending_.load(std::memory_order_acquire) != 0u)
            raster_cpu_relax();
        body_invoke_ = nullptr;
        body_context_ = nullptr;
    }

private:
    using BodyInvoke = void (*)(void *, unsigned, std::int32_t, std::int32_t);

    RowWorkerPool() {
        unsigned requested = std::thread::hardware_concurrency();
        if (requested == 0u) requested = 1u;
        bool explicitly_configured = false;
        if (const char *text = std::getenv("PSPRECOMP_RASTER_THREADS")) {
            char *end = nullptr;
            const unsigned long value = std::strtoul(text, &end, 10);
            if (end != text && *end == '\0' && value >= 1u && value <= kMaxThreads) {
                requested = static_cast<unsigned>(value);
                explicitly_configured = true;
            }
        }
        if (!explicitly_configured) {
            // "One worker per logical CPU" is the wrong default on a hybrid
            // desktop part. hardware_concurrency() counts E-cores and SMT
            // siblings, so on a 14600K it asks for 20 participants for work
            // that is latency-sensitive and shares cache with the serial
            // Allegrex stream -- the launcher scripts had been overriding it
            // to 4 by hand, measured faster. Four is now the default; the
            // env var still takes anything from 1 to kMaxThreads.
            constexpr unsigned kDefaultWorkers = 4u;
            requested = std::min(std::min(requested, kDefaultWorkers), kMaxThreads);
        }
        worker_count_ = std::max(1u, std::min(requested, kMaxThreads));
        if (worker_count_ <= 1u) return;
        workers_.reserve(worker_count_ - 1u);
        for (unsigned index = 1u; index < worker_count_; ++index)
            workers_.emplace_back([this, index] { worker_loop(index); });
    }

    ~RowWorkerPool() {
        stopping_.store(true, std::memory_order_release);
        generation_.fetch_add(1u, std::memory_order_release);
        generation_.notify_all();
        for (std::thread &worker : workers_)
            if (worker.joinable()) worker.join();
    }

    void run_chunk(unsigned chunk, unsigned participant) {
        const std::int32_t index = static_cast<std::int32_t>(chunk);
        const std::int32_t extra = std::min(index, remainder_);
        const std::int32_t begin = range_first_ + index * rows_per_chunk_ + extra;
        const std::int32_t count = rows_per_chunk_ + (index < remainder_ ? 1 : 0);
        if (count <= 0) return;
        body_invoke_(body_context_, participant, begin, begin + count - 1);
    }

    void run_available_chunks(unsigned participant) {
        for (;;) {
            const unsigned chunk = next_chunk_.fetch_add(1u, std::memory_order_relaxed);
            if (chunk >= chunk_count_) return;
            run_chunk(chunk, participant);
        }
    }

    void worker_loop(unsigned worker_index) {
        // Start from generation zero rather than sampling the current value: a
        // worker whose OS thread starts after the first publish must still run
        // that first batch instead of sleeping until the second one.
        std::uint64_t seen = 0u;
        for (;;) {
            unsigned spins = 0u;
            while (generation_.load(std::memory_order_acquire) == seen) {
                if (stopping_.load(std::memory_order_acquire)) return;
                if (spins++ < kWorkerSpins) {
                    raster_cpu_relax();
                    continue;
                }
                parked_.fetch_add(1u, std::memory_order_release);
                generation_.wait(seen, std::memory_order_acquire);
                parked_.fetch_sub(1u, std::memory_order_release);
                spins = 0u;
            }
            if (stopping_.load(std::memory_order_acquire)) return;
            seen = generation_.load(std::memory_order_acquire);

            // More chunks than participants balance triangular scanline work:
            // middle rows often contain far more covered pixels than edge rows.
            if (worker_index < active_participants_) {
                run_available_chunks(worker_index);
                if (pending_.fetch_sub(1u, std::memory_order_acq_rel) == 1u)
                    pending_.notify_one();
            }
        }
    }

    static constexpr unsigned kChunksPerParticipant = 4u;
    // Roughly one to a few milliseconds on modern x86. This keeps workers hot
    // across adjacent draw calls, then parks them during real frame idle time.
    static constexpr unsigned kWorkerSpins = 262144u;

    std::vector<std::thread> workers_;
    void *body_context_{};
    BodyInvoke body_invoke_{};
    std::atomic<unsigned> next_chunk_{};
    std::atomic<unsigned> pending_{};
    std::atomic<unsigned> parked_{};
    unsigned chunk_count_{};
    unsigned active_participants_{};
    std::int32_t range_first_{};
    std::int32_t rows_per_chunk_{};
    std::int32_t remainder_{};
    std::atomic<std::uint64_t> generation_{};
    std::atomic<bool> stopping_{};
    unsigned worker_count_{1u};
};

float decode_float24(std::uint32_t data) noexcept {
    return std::bit_cast<float>((data & 0x00FFFFFFu) << 8u);
}
}

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

namespace {

constexpr std::uint32_t data24(std::uint32_t command) noexcept { return command & 0x00FFFFFFu; }
constexpr std::uint32_t kVramBase = psprecomp::GuestMemory::kVramPhysicalBase;

struct Color {
    std::uint8_t r{255u};
    std::uint8_t g{255u};
    std::uint8_t b{255u};
    std::uint8_t a{255u};
};

struct Vertex {
    float u{};
    float v{};
    float q{1.0f};
    Color color{};
    float x{};
    float y{};
    float z{};
    float w{1.0f};
    float inv_w{1.0f};
    // PSP GE fog coefficient: 1 keeps the fragment color, 0 selects fog RGB.
    float fog_factor{1.0f};
};

thread_local bool g_collect_ge_render_stats = true;

struct GeRenderStatsCollectionScope {
    bool previous{};
    explicit GeRenderStatsCollectionScope(bool enabled) noexcept
        : previous(g_collect_ge_render_stats) { g_collect_ge_render_stats = enabled; }
    ~GeRenderStatsCollectionScope() { g_collect_ge_render_stats = previous; }
};

void record_clip_vertex(GeRenderStats &stats, const Vertex &vertex) noexcept {
    if (!g_collect_ge_render_stats) return;
    ++stats.decoded_vertices;
    const bool finite = std::isfinite(vertex.x) && std::isfinite(vertex.y) &&
                        std::isfinite(vertex.z) && std::isfinite(vertex.w);
    if (!finite) {
        ++stats.nonfinite_clip_vertices;
        return;
    }
    if (!stats.has_clip_bounds) {
        stats.has_clip_bounds = true;
        stats.clip_min_x = stats.clip_max_x = vertex.x;
        stats.clip_min_y = stats.clip_max_y = vertex.y;
        stats.clip_min_z = stats.clip_max_z = vertex.z;
        stats.clip_min_w = stats.clip_max_w = vertex.w;
        stats.min_abs_w = std::fabs(vertex.w);
        return;
    }
    stats.clip_min_x = std::min(stats.clip_min_x, vertex.x);
    stats.clip_min_y = std::min(stats.clip_min_y, vertex.y);
    stats.clip_min_z = std::min(stats.clip_min_z, vertex.z);
    stats.clip_min_w = std::min(stats.clip_min_w, vertex.w);
    stats.clip_max_x = std::max(stats.clip_max_x, vertex.x);
    stats.clip_max_y = std::max(stats.clip_max_y, vertex.y);
    stats.clip_max_z = std::max(stats.clip_max_z, vertex.z);
    stats.clip_max_w = std::max(stats.clip_max_w, vertex.w);
    stats.min_abs_w = std::min(stats.min_abs_w, std::fabs(vertex.w));
}

void record_screen_vertex(GeRenderStats &stats, const Vertex &vertex) noexcept {
    if (!std::isfinite(vertex.x) || !std::isfinite(vertex.y)) return;
    ++stats.screen_vertices;
    const float max_abs = std::max(std::fabs(vertex.x), std::fabs(vertex.y));
    stats.max_abs_screen_coordinate = std::max(stats.max_abs_screen_coordinate, max_abs);
    if (!stats.has_screen_bounds) {
        stats.has_screen_bounds = true;
        stats.screen_min_x = stats.screen_max_x = vertex.x;
        stats.screen_min_y = stats.screen_max_y = vertex.y;
        return;
    }
    stats.screen_min_x = std::min(stats.screen_min_x, vertex.x);
    stats.screen_min_y = std::min(stats.screen_min_y, vertex.y);
    stats.screen_max_x = std::max(stats.screen_max_x, vertex.x);
    stats.screen_max_y = std::max(stats.screen_max_y, vertex.y);
}

struct VertexLayout {
    std::uint32_t type{};
    std::uint32_t tc_type{};
    std::uint32_t color_type{};
    std::uint32_t normal_type{};
    std::uint32_t position_type{};
    std::uint32_t weight_type{};
    std::uint32_t index_type{};
    std::uint32_t weight_count{1u};
    std::uint32_t morph_count{1u};
    std::uint32_t weight_offset{};
    std::uint32_t tc_offset{};
    std::uint32_t color_offset{};
    std::uint32_t normal_offset{};
    std::uint32_t position_offset{};
    std::uint32_t one_size{};
    std::uint32_t stride{};
    bool through{};
};

std::uint32_t align_up(std::uint32_t value, std::uint32_t alignment) noexcept {
    if (alignment <= 1u) return value;
    return (value + alignment - 1u) & ~(alignment - 1u);
}

bool build_vertex_layout(std::uint32_t type, VertexLayout &layout, std::string &error) {
    static constexpr std::array<std::uint32_t, 4> tc_size{0u, 2u, 4u, 8u};
    static constexpr std::array<std::uint32_t, 4> tc_align{1u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 8> color_size{0u, 0u, 0u, 0u, 2u, 2u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 8> color_align{1u, 1u, 1u, 1u, 2u, 2u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> normal_size{0u, 3u, 6u, 12u};
    static constexpr std::array<std::uint32_t, 4> normal_align{1u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> position_size{0u, 3u, 6u, 12u};
    static constexpr std::array<std::uint32_t, 4> position_align{1u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> weight_size{0u, 1u, 2u, 4u};
    static constexpr std::array<std::uint32_t, 4> weight_align{1u, 1u, 2u, 4u};

    layout = {};
    layout.type = type;
    layout.tc_type = type & 3u;
    layout.color_type = (type >> 2u) & 7u;
    layout.normal_type = (type >> 5u) & 3u;
    layout.position_type = (type >> 7u) & 3u;
    layout.weight_type = (type >> 9u) & 3u;
    layout.index_type = (type >> 11u) & 3u;
    layout.weight_count = ((type >> 14u) & 7u) + 1u;
    layout.morph_count = ((type >> 18u) & 7u) + 1u;
    layout.through = (type & (1u << 23u)) != 0u;
    if (layout.position_type == 0u) {
        error = "GE vertex type has no valid position format";
        return false;
    }
    std::uint32_t offset = 0u;
    if (layout.weight_type != 0u) {
        offset = align_up(offset, weight_align[layout.weight_type]);
        layout.weight_offset = offset;
        offset += weight_size[layout.weight_type] * layout.weight_count;
    }
    offset = align_up(offset, tc_align[layout.tc_type]);
    layout.tc_offset = offset;
    offset += tc_size[layout.tc_type];
    offset = align_up(offset, color_align[layout.color_type]);
    layout.color_offset = offset;
    offset += color_size[layout.color_type];
    offset = align_up(offset, normal_align[layout.normal_type]);
    layout.normal_offset = offset;
    offset += normal_size[layout.normal_type];
    offset = align_up(offset, position_align[layout.position_type]);
    layout.position_offset = offset;
    offset += position_size[layout.position_type];

    const std::uint32_t final_alignment = std::max({tc_align[layout.tc_type], color_align[layout.color_type],
                                                    normal_align[layout.normal_type], position_align[layout.position_type],
                                                    weight_align[layout.weight_type]});
    layout.one_size = align_up(offset, final_alignment);
    layout.stride = layout.one_size * layout.morph_count;
    return layout.stride != 0u;
}

bool build_vertex_layout_cached(std::uint32_t type, VertexLayout &layout, std::string &error) {
    struct Cache { std::uint32_t type{0xFFFFFFFFu}; VertexLayout layout{}; bool valid{}; };
    static thread_local Cache cache;
    if (cache.valid && cache.type == type) {
        layout = cache.layout;
        return true;
    }
    VertexLayout decoded{};
    if (!build_vertex_layout(type, decoded, error)) return false;
    cache.type = type;
    cache.layout = decoded;
    cache.valid = true;
    layout = decoded;
    return true;
}

std::uint8_t expand4(std::uint32_t value) noexcept {
    value &= 0xFu;
    return static_cast<std::uint8_t>((value << 4u) | value);
}
std::uint8_t expand5(std::uint32_t value) noexcept {
    value &= 0x1Fu;
    return static_cast<std::uint8_t>((value << 3u) | (value >> 2u));
}
std::uint8_t expand6(std::uint32_t value) noexcept {
    value &= 0x3Fu;
    return static_cast<std::uint8_t>((value << 2u) | (value >> 4u));
}

Color unpack16(std::uint16_t pixel, std::uint32_t format) noexcept {
    switch (format) {
    case 0u: return {expand5(pixel), expand6(pixel >> 5u), expand5(pixel >> 11u), 255u};
    case 1u: return {expand5(pixel), expand5(pixel >> 5u), expand5(pixel >> 10u),
                     static_cast<std::uint8_t>((pixel & 0x8000u) ? 255u : 0u)};
    case 2u: return {expand4(pixel), expand4(pixel >> 4u), expand4(pixel >> 8u), expand4(pixel >> 12u)};
    default: return {};
    }
}

Color unpack32(std::uint32_t pixel) noexcept {
    return {static_cast<std::uint8_t>(pixel), static_cast<std::uint8_t>(pixel >> 8u),
            static_cast<std::uint8_t>(pixel >> 16u), static_cast<std::uint8_t>(pixel >> 24u)};
}

std::uint32_t pack32(Color color) noexcept {
    return static_cast<std::uint32_t>(color.r) |
           (static_cast<std::uint32_t>(color.g) << 8u) |
           (static_cast<std::uint32_t>(color.b) << 16u) |
           (static_cast<std::uint32_t>(color.a) << 24u);
}


std::uint16_t pack16(Color color, std::uint32_t format) noexcept {
    switch (format) {
    case 0u:
        return static_cast<std::uint16_t>((color.r >> 3u) | ((color.g >> 2u) << 5u) | ((color.b >> 3u) << 11u));
    case 1u:
        return static_cast<std::uint16_t>((color.r >> 3u) | ((color.g >> 3u) << 5u) |
                                          ((color.b >> 3u) << 10u) | ((color.a >= 128u ? 1u : 0u) << 15u));
    case 2u:
        return static_cast<std::uint16_t>((color.r >> 4u) | ((color.g >> 4u) << 4u) |
                                          ((color.b >> 4u) << 8u) | ((color.a >> 4u) << 12u));
    default:
        return 0u;
    }
}

Color read_color(const psprecomp::GuestMemory &memory, std::uint32_t address, std::uint32_t format) {
    if (format == 3u) return unpack32(memory.aot_load32(address));
    return unpack16(memory.aot_load16(address), format);
}

// Raw-pointer twins of read_color/write_color.  Byte order matches the little
// endian assembly the checked accessors perform, so results are identical.
Color read_color_raw(const std::uint8_t *pixel, std::uint32_t format) noexcept {
    if (format == 3u) {
        return unpack32(static_cast<std::uint32_t>(pixel[0]) |
                        (static_cast<std::uint32_t>(pixel[1]) << 8u) |
                        (static_cast<std::uint32_t>(pixel[2]) << 16u) |
                        (static_cast<std::uint32_t>(pixel[3]) << 24u));
    }
    return unpack16(static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(pixel[0]) |
                        (static_cast<std::uint16_t>(pixel[1]) << 8u)),
                    format);
}

void write_color_raw(std::uint8_t *pixel, std::uint32_t format, Color color,
                     std::uint32_t write_mask) noexcept {
    if (format == 3u) {
        const std::uint32_t old = static_cast<std::uint32_t>(pixel[0]) |
                                  (static_cast<std::uint32_t>(pixel[1]) << 8u) |
                                  (static_cast<std::uint32_t>(pixel[2]) << 16u) |
                                  (static_cast<std::uint32_t>(pixel[3]) << 24u);
        const std::uint32_t packed = pack32(color);
        const std::uint32_t value = (old & write_mask) | (packed & ~write_mask);
        pixel[0] = static_cast<std::uint8_t>(value);
        pixel[1] = static_cast<std::uint8_t>(value >> 8u);
        pixel[2] = static_cast<std::uint8_t>(value >> 16u);
        pixel[3] = static_cast<std::uint8_t>(value >> 24u);
        return;
    }
    const Color old = read_color_raw(pixel, format);
    if ((write_mask & 0x000000FFu) != 0u) color.r = old.r;
    if ((write_mask & 0x0000FF00u) != 0u) color.g = old.g;
    if ((write_mask & 0x00FF0000u) != 0u) color.b = old.b;
    if ((write_mask & 0xFF000000u) != 0u) color.a = old.a;
    const std::uint16_t value = pack16(color, format);
    pixel[0] = static_cast<std::uint8_t>(value);
    pixel[1] = static_cast<std::uint8_t>(value >> 8u);
}

void write_color(psprecomp::GuestMemory &memory, std::uint32_t address, std::uint32_t format, Color color,
                 std::uint32_t write_mask) {
    if (format == 3u) {
        const std::uint32_t old = memory.aot_load32(address);
        const std::uint32_t packed = pack32(color);
        memory.aot_store32(address, (old & write_mask) | (packed & ~write_mask));
    } else {
        // PSP color masks are expressed in 8888 channels. Convert through the old color so masks
        // remain channel-correct even when the target is packed 16-bit.
        Color old = read_color(memory, address, format);
        if ((write_mask & 0x000000FFu) != 0u) color.r = old.r;
        if ((write_mask & 0x0000FF00u) != 0u) color.g = old.g;
        if ((write_mask & 0x00FF0000u) != 0u) color.b = old.b;
        if ((write_mask & 0xFF000000u) != 0u) color.a = old.a;
        memory.aot_store16(address, pack16(color, format));
    }
}

float signed_normalized8(std::uint8_t value) noexcept {
    return static_cast<float>(static_cast<std::int8_t>(value)) / 128.0f;
}

float signed_normalized16(std::uint16_t value) noexcept {
    return static_cast<float>(static_cast<std::int16_t>(value)) / 32768.0f;
}

struct Vec3 { float x{}, y{}, z{}; };
struct Vec4 { float x{}, y{}, z{}, w{}; };

Vec3 operator+(Vec3 a, Vec3 b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 operator-(Vec3 a, Vec3 b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator*(Vec3 a, float scale) noexcept { return {a.x * scale, a.y * scale, a.z * scale}; }
Vec3 &operator+=(Vec3 &a, Vec3 b) noexcept { a = a + b; return a; }
float dot(Vec3 a, Vec3 b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 normalized_or_zero(Vec3 value) noexcept {
    const float length_squared = dot(value, value);
    if (!std::isfinite(length_squared) || length_squared <= 1.0e-30f) return {};
    const float inverse_length = 1.0f / std::sqrt(length_squared);
    return value * inverse_length;
}

Vec3 normalized_or_001(Vec3 value) noexcept {
    const float length_squared = dot(value, value);
    if (!std::isfinite(length_squared) || length_squared <= 1.0e-30f) return {0.0f, 0.0f, 1.0f};
    const float inverse_length = 1.0f / std::sqrt(length_squared);
    return value * inverse_length;
}

Vec3 transform_4x3(const std::array<float, 12> &matrix, Vec3 value) noexcept {
    return {
        matrix[0] * value.x + matrix[3] * value.y + matrix[6] * value.z + matrix[9],
        matrix[1] * value.x + matrix[4] * value.y + matrix[7] * value.z + matrix[10],
        matrix[2] * value.x + matrix[5] * value.y + matrix[8] * value.z + matrix[11],
    };
}

Vec3 transform_normal_4x3(const std::array<float, 12> &matrix, Vec3 value) noexcept {
    return {
        matrix[0] * value.x + matrix[3] * value.y + matrix[6] * value.z,
        matrix[1] * value.x + matrix[4] * value.y + matrix[7] * value.z,
        matrix[2] * value.x + matrix[5] * value.y + matrix[8] * value.z,
    };
}

Vec4 transform_4x4(const std::array<float, 16> &matrix, Vec3 value) noexcept {
    return {
        matrix[0] * value.x + matrix[4] * value.y + matrix[8] * value.z + matrix[12],
        matrix[1] * value.x + matrix[5] * value.y + matrix[9] * value.z + matrix[13],
        matrix[2] * value.x + matrix[6] * value.y + matrix[10] * value.z + matrix[14],
        matrix[3] * value.x + matrix[7] * value.y + matrix[11] * value.z + matrix[15],
    };
}


std::array<float, 16> affine_4x3_to_mat4(const std::array<float, 12> &m) noexcept {
    return {m[0], m[1], m[2], 0.0f,
            m[3], m[4], m[5], 0.0f,
            m[6], m[7], m[8], 0.0f,
            m[9], m[10], m[11], 1.0f};
}

std::array<float, 16> multiply_mat4(const std::array<float, 16> &a,
                                    const std::array<float, 16> &b) noexcept {
    std::array<float, 16> result{};
    for (std::size_t column = 0; column < 4u; ++column) {
        for (std::size_t row = 0; row < 4u; ++row) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < 4u; ++k)
                sum += a[k * 4u + row] * b[column * 4u + k];
            result[column * 4u + row] = sum;
        }
    }
    return result;
}

GeGpuHardwareTransform build_gpu_hardware_transform(
    const std::array<std::uint32_t, 256> &commands,
    const GeTransformState &transform,
    // Hybrid path for lit geometry: the CPU has already applied the world
    // matrix (lighting needs world-space position and normal, and porting the
    // whole PSP T&L into the vertex shader is a much larger and riskier change).
    // The vertices then arrive in world space, so the matrix the shader applies
    // must skip the world stage. Everything downstream -- view, projection,
    // viewport, clipping -- still runs on the GPU, which is where the 90.5% of
    // vertices rejected by the lighting gate were losing it.
    bool vertices_already_in_world_space = false,
    bool texture_coordinates_already_generated = false) noexcept {
    GeGpuHardwareTransform hw{};
    const auto world = vertices_already_in_world_space
        ? std::array<float, 16>{1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f}
        : affine_4x3_to_mat4(transform.world);
    hw.model_to_world = world;
    const auto view = affine_4x3_to_mat4(transform.view);
    const auto model_to_view = multiply_mat4(view, world);
    // No widescreen correction on this matrix: transform.projection was already
    // built by the guest with the configured aspect instead of 16/9, so the
    // frustum arrives widened. See the comment in decode_vertex().
    hw.model_to_clip = multiply_mat4(transform.projection, model_to_view);
    hw.model_to_view_z = {model_to_view[2], model_to_view[6],
                          model_to_view[10], model_to_view[14]};
    hw.viewport_scale_x = decode_float24(data24(commands[0x42u]));
    hw.viewport_scale_y = decode_float24(data24(commands[0x43u]));
    hw.viewport_scale_z = decode_float24(data24(commands[0x44u]));
    hw.viewport_center_x = decode_float24(data24(commands[0x45u]));
    hw.viewport_center_y = decode_float24(data24(commands[0x46u]));
    hw.viewport_center_z = decode_float24(data24(commands[0x47u]));
    hw.viewport_offset_x = static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f;
    hw.viewport_offset_y = static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f;
    // Matrix/shade UV generation is performed while decoding because it reads
    // model/world normals. Those values are already normalized texture-space
    // coordinates, so applying the ordinary GE scale/offset again would make
    // vehicle environment maps jump between two coordinate systems.
    hw.uv_scale_u = texture_coordinates_already_generated
        ? 1.0f : decode_float24(data24(commands[0x48u]));
    hw.uv_scale_v = texture_coordinates_already_generated
        ? 1.0f : decode_float24(data24(commands[0x49u]));
    hw.uv_offset_u = texture_coordinates_already_generated
        ? 0.0f : decode_float24(data24(commands[0x4Au]));
    hw.uv_offset_v = texture_coordinates_already_generated
        ? 0.0f : decode_float24(data24(commands[0x4Bu]));
    hw.fog_end = decode_float24(data24(commands[0xCDu]));
    hw.fog_slope = decode_float24(data24(commands[0xCEu]));
    hw.depth_clip_enabled = (data24(commands[0x1Cu]) & 1u) != 0u;
    hw.cull_enabled = gpu_hardware_cull_enabled() && (data24(commands[0x1Du]) & 1u) != 0u;
    hw.accept_counter_clockwise = (data24(commands[0x9Bu]) & 1u) != 0u;

    return hw;
}

Vec3 read_vector3(const psprecomp::GuestMemory &memory, std::uint32_t address,
                  std::uint32_t format) {
    switch (format) {
    case 1u:
        return {signed_normalized8(memory.aot_load8(address)),
                signed_normalized8(memory.aot_load8(address + 1u)),
                signed_normalized8(memory.aot_load8(address + 2u))};
    case 2u:
        return {signed_normalized16(memory.aot_load16(address)),
                signed_normalized16(memory.aot_load16(address + 2u)),
                signed_normalized16(memory.aot_load16(address + 4u))};
    case 3u:
        return {std::bit_cast<float>(memory.aot_load32(address)),
                std::bit_cast<float>(memory.aot_load32(address + 4u)),
                std::bit_cast<float>(memory.aot_load32(address + 8u))};
    default:
        return {};
    }
}

void read_texcoord(const psprecomp::GuestMemory &memory, std::uint32_t address,
                   std::uint32_t format, bool through, float &u, float &v) {
    switch (format) {
    case 0u: u = v = 0.0f; break;
    case 1u:
        u = static_cast<float>(memory.aot_load8(address));
        v = static_cast<float>(memory.aot_load8(address + 1u));
        if (!through) { u *= 1.0f / 128.0f; v *= 1.0f / 128.0f; }
        break;
    case 2u:
        u = static_cast<float>(memory.aot_load16(address));
        v = static_cast<float>(memory.aot_load16(address + 2u));
        if (!through) { u *= 1.0f / 32768.0f; v *= 1.0f / 32768.0f; }
        break;
    case 3u:
        u = std::bit_cast<float>(memory.aot_load32(address));
        v = std::bit_cast<float>(memory.aot_load32(address + 4u));
        break;
    }
}

Color read_vertex_color(const psprecomp::GuestMemory &memory, std::uint32_t address,
                        std::uint32_t format) {
    switch (format) {
    case 4u: return unpack16(memory.aot_load16(address), 0u);
    case 5u: return unpack16(memory.aot_load16(address), 1u);
    case 6u: return unpack16(memory.aot_load16(address), 2u);
    case 7u: return unpack32(memory.aot_load32(address));
    default: return {};
    }
}

std::uint8_t clamp_channel(float value) noexcept {
    return static_cast<std::uint8_t>(std::clamp(static_cast<long>(value), 0l, 255l));
}

// A vertex format without a colour component does not mean white. The GE
// substitutes the material ambient colour (0x55, with 0x58 for alpha), and a
// game that paints a whole surface one colour has no reason to spend four
// bytes per vertex repeating it.
//
// Defaulting to white is what silvered the sea. VCS draws water as a greyscale
// wave texture in MODULATE, tinted by the vertex colour: the tiles that carry
// per-vertex colour came out teal, and the tiles that leave it to the material
// came out texture x white -- the same texture, the same state, the same draw
// call, side by side, which is why half the water looked right.
Color material_ambient_color(const std::array<std::uint32_t, 256> &commands) noexcept {
    const std::uint32_t rgb = data24(commands[0x55u]);
    return Color{
        static_cast<std::uint8_t>(rgb & 0xFFu),
        static_cast<std::uint8_t>((rgb >> 8u) & 0xFFu),
        static_cast<std::uint8_t>((rgb >> 16u) & 0xFFu),
        static_cast<std::uint8_t>(data24(commands[0x58u]) & 0xFFu),
    };
}

Color morph_color(const psprecomp::GuestMemory &memory, std::uint32_t address,
                  const VertexLayout &layout, const GeTransformState &transform,
                  const std::array<std::uint32_t, 256> &commands) {
    if (layout.color_type < 4u) return material_ambient_color(commands);
    if (layout.morph_count == 1u)
        return read_vertex_color(memory, address + layout.color_offset, layout.color_type);

    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
    for (std::uint32_t morph = 0u; morph < layout.morph_count; ++morph) {
        const Color color = read_vertex_color(memory, address + morph * layout.one_size + layout.color_offset,
                                              layout.color_type);
        const float weight = transform.morph_weights[morph];
        r += color.r * weight; g += color.g * weight; b += color.b * weight; a += color.a * weight;
    }
    return {clamp_channel(r), clamp_channel(g), clamp_channel(b), clamp_channel(a)};
}

std::array<float, 12> compute_skin_matrix(const psprecomp::GuestMemory &memory,
                                          std::uint32_t address,
                                          const VertexLayout &layout,
                                          const GeTransformState &transform) {
    std::array<float, 12> skin{};
    if (layout.weight_type == 0u) {
        skin[0] = skin[4] = skin[8] = 1.0f;
        return skin;
    }

    for (std::uint32_t bone = 0u; bone < layout.weight_count; ++bone) {
        float weight = 0.0f;
        switch (layout.weight_type) {
        case 1u: weight = memory.aot_load8(address + layout.weight_offset + bone) * (1.0f / 128.0f); break;
        case 2u: weight = memory.aot_load16(address + layout.weight_offset + bone * 2u) * (1.0f / 32768.0f); break;
        case 3u: weight = std::bit_cast<float>(memory.aot_load32(address + layout.weight_offset + bone * 4u)); break;
        }
        if (weight == 0.0f) continue;
        const std::size_t bone_base = static_cast<std::size_t>(bone) * 12u;
        for (std::size_t element = 0u; element < skin.size(); ++element)
            skin[element] += transform.bones[bone_base + element] * weight;
    }
    return skin;
}

float psp_light_pow(float value, float exponent) noexcept {
    if (exponent <= 0.0f) return 1.0f;
    if (value > 0.0f) return std::pow(value, exponent);
    return value;
}

struct FloatColor { float r{}, g{}, b{}, a{}; };

FloatColor to_float_color(Color color) noexcept {
    return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
}

Color from_float_color(FloatColor color) noexcept {
    auto channel = [](float value) {
        return static_cast<std::uint8_t>(std::clamp(std::lround(value * 255.0f), 0l, 255l));
    };
    return {channel(color.r), channel(color.g), channel(color.b), channel(color.a)};
}

FloatColor rgb_command(std::uint32_t command, float alpha = 1.0f) noexcept {
    const Color color = unpack32(data24(command) | 0xFF000000u);
    return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, alpha};
}

FloatColor multiply(FloatColor a, FloatColor b, float scale = 1.0f) noexcept {
    return {a.r * b.r * scale, a.g * b.g * scale, a.b * b.b * scale, a.a * b.a * scale};
}

void add_rgb(FloatColor &destination, FloatColor source) noexcept {
    destination.r += source.r; destination.g += source.g; destination.b += source.b;
}

struct PreparedLight {
    bool enabled{};
    std::uint32_t computation{};
    std::uint32_t type{};
    Vec3 vector{};
    Vec3 attenuation{};
    Vec3 spot_direction{};
    float cutoff{};
    float exponent{};
    FloatColor ambient{};
    FloatColor diffuse{};
    FloatColor specular{};
};

struct PreparedLighting {
    bool enabled{};
    std::uint32_t material_update{};
    float material_alpha{};
    FloatColor material_ambient{};
    FloatColor material_diffuse{};
    FloatColor material_specular{};
    FloatColor emissive{};
    FloatColor global_ambient{};
    float specular_exponent{};
    std::array<PreparedLight, 4> lights{};
};

PreparedLighting prepare_lighting(bool has_vertex_color,
                                  const std::array<std::uint32_t, 256> &commands) {
    PreparedLighting state{};
    state.enabled = (data24(commands[0x17u]) & 1u) != 0u;
    if (!state.enabled) return state;

    state.material_update = data24(commands[0x53u]) & (has_vertex_color ? 7u : 0u);
    state.material_alpha = static_cast<float>(data24(commands[0x58u]) & 0xFFu) / 255.0f;
    state.material_ambient = rgb_command(commands[0x55u], state.material_alpha);
    state.material_diffuse = rgb_command(commands[0x56u], state.material_alpha);
    state.material_specular = rgb_command(commands[0x57u], state.material_alpha);
    state.emissive = rgb_command(commands[0x54u], 0.0f);
    const float ambient_alpha = static_cast<float>(data24(commands[0x5Du]) & 0xFFu) / 255.0f;
    state.global_ambient = rgb_command(commands[0x5Cu], ambient_alpha);
    const float exponent = decode_float24(data24(commands[0x5Bu]));
    state.specular_exponent = (!std::isfinite(exponent) || exponent < 0.0f) ? 0.0f : exponent;

    for (std::uint32_t light = 0u; light < state.lights.size(); ++light) {
        PreparedLight &prepared = state.lights[light];
        prepared.enabled = (data24(commands[0x18u + light]) & 1u) != 0u;
        if (!prepared.enabled) continue;
        const std::uint32_t type_data = data24(commands[0x5Fu + light]);
        prepared.computation = type_data & 3u;
        prepared.type = (type_data >> 8u) & 3u;
        prepared.vector = {
            decode_float24(data24(commands[0x63u + light * 3u])),
            decode_float24(data24(commands[0x64u + light * 3u])),
            decode_float24(data24(commands[0x65u + light * 3u])),
        };
        if (prepared.type == 0u) prepared.vector = normalized_or_001(prepared.vector);
        if (prepared.type != 0u) {
            prepared.attenuation = {
                decode_float24(data24(commands[0x7Bu + light * 3u])),
                decode_float24(data24(commands[0x7Cu + light * 3u])),
                decode_float24(data24(commands[0x7Du + light * 3u])),
            };
        }
        if (prepared.type >= 2u) {
            prepared.spot_direction = normalized_or_001(Vec3{
                decode_float24(data24(commands[0x6Fu + light * 3u])),
                decode_float24(data24(commands[0x70u + light * 3u])),
                decode_float24(data24(commands[0x71u + light * 3u])),
            });
            prepared.cutoff = decode_float24(data24(commands[0x8Bu + light]));
            if (!std::isfinite(prepared.cutoff)) prepared.cutoff = 0.0f;
            prepared.exponent = decode_float24(data24(commands[0x87u + light]));
            if (!std::isfinite(prepared.exponent) || prepared.exponent < 0.0f)
                prepared.exponent = 0.0f;
        }
        prepared.ambient = rgb_command(commands[0x8Fu + light * 3u]);
        prepared.diffuse = rgb_command(commands[0x90u + light * 3u]);
        prepared.specular = rgb_command(commands[0x91u + light * 3u]);
    }
    return state;
}

Color apply_prepared_lighting(Color input, Vec3 world_position, Vec3 world_normal,
                              const PreparedLighting &state,
                              bool world_normal_is_normalized = false) {
    if (!state.enabled) return input;

    const std::uint32_t material_update = state.material_update;
    const FloatColor vertex = to_float_color(input);
    const FloatColor material_ambient = (material_update & 1u) != 0u
        ? vertex : state.material_ambient;
    const FloatColor material_diffuse = (material_update & 2u) != 0u
        ? vertex : state.material_diffuse;
    const FloatColor material_specular = (material_update & 4u) != 0u
        ? vertex : state.material_specular;

    FloatColor result = state.emissive;
    const FloatColor base_ambient = multiply(material_ambient, state.global_ambient);
    add_rgb(result, base_ambient);
    result.a = base_ambient.a;

    // Most generic paths hand us an arbitrary model/world normal.  The common
    // VCS 0x0115 city format has no stored normal at all, however, so its
    // transformed default normal is identical for every vertex in a draw.
    // Stage 45.4 precomputes that normalized vector once and skips the sqrt here
    // for every vertex in the batch.
    if (!world_normal_is_normalized) world_normal = normalized_or_001(world_normal);
    for (const PreparedLight &light : state.lights) {
        if (!light.enabled) continue;
        Vec3 light_vector = light.vector;
        float attenuation_spot = 1.0f;
        if (light.type != 0u) {
            light_vector = light_vector - world_position;
            const float distance_squared = dot(light_vector, light_vector);
            const float distance = std::isfinite(distance_squared) && distance_squared > 0.0f
                ? std::sqrt(distance_squared) : 0.0f;
            light_vector = normalized_or_001(light_vector);
            const float denominator = light.attenuation.x + light.attenuation.y * distance +
                light.attenuation.z * distance * distance;
            const float value = denominator > 0.0f ? 1.0f / denominator : 0.0f;
            attenuation_spot = std::clamp(value, 0.0f, 1.0f);
        }

        if (light.type >= 2u) {
            float raw_spot = dot(light.spot_direction, light_vector);
            if (!std::isfinite(raw_spot)) raw_spot = 0.0f;
            attenuation_spot *= raw_spot >= light.cutoff
                ? std::max(0.0f, psp_light_pow(raw_spot, light.exponent)) : 0.0f;
        }

        add_rgb(result, multiply(light.ambient, material_ambient, attenuation_spot));

        float diffuse_factor = dot(light_vector, world_normal);
        if (light.computation == 2u)
            diffuse_factor = psp_light_pow(diffuse_factor, state.specular_exponent);
        if (diffuse_factor > 0.0f) {
            add_rgb(result, multiply(light.diffuse, material_diffuse,
                                     attenuation_spot * diffuse_factor));
        }

        if (light.computation == 1u && diffuse_factor >= 0.0f) {
            const Vec3 half_vector = normalized_or_001(light_vector + Vec3{0.0f, 0.0f, 1.0f});
            const float specular_factor = psp_light_pow(dot(half_vector, world_normal),
                                                        state.specular_exponent);
            if (specular_factor > 0.0f) {
                add_rgb(result, multiply(light.specular, material_specular,
                                         attenuation_spot * specular_factor));
            }
        }
    }

    // This software path currently has one interpolated color. When secondary color
    // mode is selected, folding specular into primary reproduces the final post-texture sum.
    return from_float_color(result);
}

Color apply_lighting(Color input, bool has_vertex_color, Vec3 world_position,
                     Vec3 world_normal, const std::array<std::uint32_t, 256> &commands) {
    return apply_prepared_lighting(input, world_position, world_normal,
                                   prepare_lighting(has_vertex_color, commands));
}

// Stage 45.5: collapse the complete PSP lighting equation into
// outColor = inColor * mul + add when every enabled light is directional.
// For vtype 0x0115 there is no stored normal, so the transformed default normal
// is constant for the whole draw. Directional lights have no position-based
// attenuation, making every RGB term affine in the incoming vertex colour.
// The VS quantizes back to RGBA8 before interpolation, matching the CPU path.
bool prepare_directional_lighting_affine(const PreparedLighting &state,
                                          Vec3 world_normal,
                                          std::array<float, 4> &mul,
                                          std::array<float, 4> &add) noexcept {
    mul = {0.0f, 0.0f, 0.0f, 0.0f};
    add = {state.emissive.r, state.emissive.g, state.emissive.b, 0.0f};
    if (!state.enabled) {
        mul = {1.0f, 1.0f, 1.0f, 1.0f};
        add = {};
        return true;
    }
    for (const PreparedLight &light : state.lights)
        if (light.enabled && light.type != 0u) return false;

    world_normal = normalized_or_001(world_normal);
    const auto add_term = [&](const FloatColor &material, bool from_vertex,
                              const FloatColor &light, float scale) {
        const float coeff[3]{light.r * scale, light.g * scale, light.b * scale};
        const float mat[3]{material.r, material.g, material.b};
        for (int c = 0; c < 3; ++c) {
            if (from_vertex) mul[c] += coeff[c];
            else add[c] += mat[c] * coeff[c];
        }
    };

    const bool ambient_from_vertex = (state.material_update & 1u) != 0u;
    const bool diffuse_from_vertex = (state.material_update & 2u) != 0u;
    const bool specular_from_vertex = (state.material_update & 4u) != 0u;
    add_term(state.material_ambient, ambient_from_vertex, state.global_ambient, 1.0f);
    if (ambient_from_vertex) mul[3] = state.global_ambient.a;
    else add[3] = state.material_ambient.a * state.global_ambient.a;

    for (const PreparedLight &light : state.lights) {
        if (!light.enabled) continue;
        add_term(state.material_ambient, ambient_from_vertex, light.ambient, 1.0f);
        float diffuse_factor = dot(light.vector, world_normal);
        if (light.computation == 2u)
            diffuse_factor = psp_light_pow(diffuse_factor, state.specular_exponent);
        if (diffuse_factor > 0.0f)
            add_term(state.material_diffuse, diffuse_from_vertex, light.diffuse, diffuse_factor);
        if (light.computation == 1u && diffuse_factor >= 0.0f) {
            const Vec3 half_vector = normalized_or_001(light.vector + Vec3{0.0f, 0.0f, 1.0f});
            const float specular_factor = psp_light_pow(dot(half_vector, world_normal),
                                                        state.specular_exponent);
            if (specular_factor > 0.0f)
                add_term(state.material_specular, specular_from_vertex,
                         light.specular, specular_factor);
        }
    }
    return true;
}

bool decode_vertex(const psprecomp::GuestMemory &memory, std::uint32_t address,
                   const VertexLayout &layout,
                   const std::array<std::uint32_t, 256> &commands,
                   const GeTransformState &transform,
                   Vertex &vertex, std::string &error) {
    if (!memory.contains(address, layout.stride)) {
        error = "GE vertex lies outside guest memory at " + psprecomp::hex32(address);
        return false;
    }

    if (layout.morph_count == 1u || layout.through) {
        read_texcoord(memory, address + layout.tc_offset, layout.tc_type, layout.through, vertex.u, vertex.v);
    } else {
        vertex.u = vertex.v = 0.0f;
        for (std::uint32_t morph = 0u; morph < layout.morph_count; ++morph) {
            float u = 0.0f, v = 0.0f;
            read_texcoord(memory, address + morph * layout.one_size + layout.tc_offset,
                          layout.tc_type, false, u, v);
            vertex.u += u * transform.morph_weights[morph];
            vertex.v += v * transform.morph_weights[morph];
        }
    }
    vertex.color = morph_color(memory, address, layout, transform, commands);

    if (layout.through) {
        switch (layout.position_type) {
        case 1u:
            vertex.x = vertex.y = vertex.z = 0.0f;
            break;
        case 2u:
            vertex.x = static_cast<float>(static_cast<std::int16_t>(memory.aot_load16(address + layout.position_offset)));
            vertex.y = static_cast<float>(static_cast<std::int16_t>(memory.aot_load16(address + layout.position_offset + 2u)));
            vertex.z = static_cast<float>(memory.aot_load16(address + layout.position_offset + 4u));
            break;
        case 3u:
            vertex.x = std::bit_cast<float>(memory.aot_load32(address + layout.position_offset));
            vertex.y = std::bit_cast<float>(memory.aot_load32(address + layout.position_offset + 4u));
            vertex.z = std::clamp(std::bit_cast<float>(memory.aot_load32(address + layout.position_offset + 8u)),
                                  0.0f, 65535.0f);
            break;
        default:
            error = "invalid GE position type";
            return false;
        }
        vertex.w = 1.0f;
        vertex.inv_w = 1.0f;
        vertex.fog_factor = 1.0f;
        return true;
    }

    // The vertex normal is observable only through lighting and through the two
    // UV generation modes that read it.  VCS draws its world with prelit vertex
    // colors and lighting disabled, so the normal read, its skin and world
    // matrix transforms and the square-root normalization all produced a value
    // nothing looked at -- once per vertex, about 75000 times per heavy vblank.
    const std::uint32_t uv_generation_data = data24(commands[0xC0u]);
    std::uint32_t uv_generation = uv_generation_data & 3u;
    if (uv_generation == 3u) uv_generation = 0u; // Hardware-compatible fallback for the reserved mode.
    const std::uint32_t uv_generation_source = (uv_generation_data >> 8u) & 3u;
    const bool lighting_enabled = (data24(commands[0x17u]) & 1u) != 0u;
    // Modes 2 and 3 of the matrix generator read the model normal.
    const bool model_normal_needed = lighting_enabled || uv_generation == 2u ||
        (uv_generation == 1u && uv_generation_source >= 2u);
    const bool world_normal_needed = lighting_enabled || uv_generation == 2u;

    Vec3 model_position{};
    Vec3 model_normal{0.0f, 0.0f, 1.0f};
    const bool read_normal = model_normal_needed && layout.normal_type != 0u;
    if (layout.morph_count == 1u) {
        model_position = read_vector3(memory, address + layout.position_offset, layout.position_type);
        if (read_normal)
            model_normal = read_vector3(memory, address + layout.normal_offset, layout.normal_type);
    } else {
        if (read_normal) model_normal = {};
        for (std::uint32_t morph = 0u; morph < layout.morph_count; ++morph) {
            const std::uint32_t morph_address = address + morph * layout.one_size;
            const float weight = transform.morph_weights[morph];
            model_position += read_vector3(memory, morph_address + layout.position_offset,
                                           layout.position_type) * weight;
            if (read_normal)
                model_normal += read_vector3(memory, morph_address + layout.normal_offset,
                                             layout.normal_type) * weight;
        }
    }

    if (layout.weight_type != 0u) {
        const std::array<float, 12> skin = compute_skin_matrix(memory, address, layout, transform);
        model_position = transform_4x3(skin, model_position);
        if (model_normal_needed) model_normal = transform_normal_4x3(skin, model_normal);
    }

    const Vec3 world_position = transform_4x3(transform.world, model_position);
    Vec3 world_normal{0.0f, 0.0f, 1.0f};
    if (world_normal_needed) {
        world_normal = transform_normal_4x3(transform.world, model_normal);
        if ((data24(commands[0x51u]) & 1u) != 0u) world_normal = world_normal * -1.0f;
        world_normal = normalized_or_001(world_normal);
        vertex.color = apply_lighting(vertex.color, layout.color_type >= 4u, world_position,
                                      world_normal, commands);
    }

    const Vec3 view = transform_4x3(transform.view, world_position);
    if ((data24(commands[0x1Fu]) & 1u) != 0u) {
        const float fog_end = decode_float24(data24(commands[0xCDu]));
        const float fog_slope = decode_float24(data24(commands[0xCEu]));
        const float fog = (view.z + fog_end) * fog_slope;
        vertex.fog_factor = std::isfinite(fog) ? std::clamp(fog, 0.0f, 1.0f) : 1.0f;
    } else {
        vertex.fog_factor = 1.0f;
    }
    const Vec4 clip = transform_4x4(transform.projection, view);
    // Widescreen is not applied here.  The projection this multiplies by was
    // already built with the configured aspect instead of 16/9 by the guest
    // itself (get_effective_aspect_ratio in generated/), so the frustum arrives
    // widened; correcting X a second time would double the effect.  The 2D
    // interface, which no projection touches, is handled in ge_gpu_backend.cpp.
    vertex.x = clip.x;
    vertex.y = clip.y;
    vertex.z = clip.z;
    vertex.w = clip.w;
    vertex.inv_w = 0.0f;

    const std::uint32_t texture_size = data24(commands[0xB8u]);
    const float texture_width = static_cast<float>(1u << (texture_size & 0xFu));
    const float texture_height = static_cast<float>(1u << ((texture_size >> 8u) & 0xFu));
    vertex.q = 1.0f;
    if (uv_generation == 1u) {
        Vec3 source{};
        switch (uv_generation_source) {
        case 0u: source = model_position; break;
        case 1u: source = {vertex.u, vertex.v, 0.0f}; break;
        case 2u: source = normalized_or_zero(model_normal); break;
        case 3u: source = model_normal; break;
        }
        const Vec3 stq = transform_4x3(transform.texture, source);
        vertex.u = stq.x * texture_width;
        vertex.v = stq.y * texture_height;
        vertex.q = stq.z;
    } else if (uv_generation == 2u) {
        const std::uint32_t shade = data24(commands[0xC1u]);
        const std::uint32_t light_s = shade & 3u;
        const std::uint32_t light_t = (shade >> 8u) & 3u;
        auto light_vector = [&](std::uint32_t light) {
            return normalized_or_001(Vec3{
                decode_float24(data24(commands[0x63u + light * 3u])),
                decode_float24(data24(commands[0x64u + light * 3u])),
                decode_float24(data24(commands[0x65u + light * 3u])),
            });
        };
        vertex.u = ((dot(light_vector(light_s), world_normal) + 1.0f) * 0.5f) * texture_width;
        vertex.v = ((dot(light_vector(light_t), world_normal) + 1.0f) * 0.5f) * texture_height;
    } else if (layout.tc_type != 0u) {
        const float scale_u = decode_float24(data24(commands[0x48u]));
        const float scale_v = decode_float24(data24(commands[0x49u]));
        const float offset_u = decode_float24(data24(commands[0x4Au]));
        const float offset_v = decode_float24(data24(commands[0x4Bu]));
        vertex.u = (vertex.u * scale_u + offset_u) * texture_width;
        vertex.v = (vertex.v * scale_v + offset_v) * texture_height;
    }
    return true;
}

bool decode_vertex_0115_fast(const psprecomp::GuestMemory &memory, std::uint32_t address,
                             const VertexLayout &layout,
                             const std::array<std::uint32_t, 256> &commands,
                             const GeTransformState &transform,
                             Vertex &vertex, std::string &error) {
    // VCS city geometry is overwhelmingly vtype 0x000115:
    // u8 UV, 5551 colour, no normal/weights, s16 XYZ, one morph. Avoid the
    // generic format switches and unused normal/skinning branches for this exact
    // layout while keeping the same transforms, fog and UV scale/offset.
    if (!memory.contains(address, layout.stride)) {
        error = "GE vertex lies outside guest memory at " + psprecomp::hex32(address);
        return false;
    }
    vertex.u = static_cast<float>(memory.aot_load8(address + layout.tc_offset)) * (1.0f / 128.0f);
    vertex.v = static_cast<float>(memory.aot_load8(address + layout.tc_offset + 1u)) * (1.0f / 128.0f);
    vertex.color = unpack16(memory.aot_load16(address + layout.color_offset), 1u);
    const Vec3 model_position{
        signed_normalized16(memory.aot_load16(address + layout.position_offset)),
        signed_normalized16(memory.aot_load16(address + layout.position_offset + 2u)),
        signed_normalized16(memory.aot_load16(address + layout.position_offset + 4u)),
    };
    const Vec3 world_position = transform_4x3(transform.world, model_position);
    const Vec3 view = transform_4x3(transform.view, world_position);
    if ((data24(commands[0x1Fu]) & 1u) != 0u) {
        const float fog_end = decode_float24(data24(commands[0xCDu]));
        const float fog_slope = decode_float24(data24(commands[0xCEu]));
        const float fog = (view.z + fog_end) * fog_slope;
        vertex.fog_factor = std::isfinite(fog) ? std::clamp(fog, 0.0f, 1.0f) : 1.0f;
    } else {
        vertex.fog_factor = 1.0f;
    }
    const Vec4 clip = transform_4x4(transform.projection, view);
    // See decode_vertex(): widescreen is already in transform.projection.
    vertex.x = clip.x;
    vertex.y = clip.y;
    vertex.z = clip.z;
    vertex.w = clip.w;
    vertex.inv_w = 0.0f;
    vertex.q = 1.0f;

    const std::uint32_t texture_size = data24(commands[0xB8u]);
    const float texture_width = static_cast<float>(1u << (texture_size & 0xFu));
    const float texture_height = static_cast<float>(1u << ((texture_size >> 8u) & 0xFu));
    const float scale_u = decode_float24(data24(commands[0x48u]));
    const float scale_v = decode_float24(data24(commands[0x49u]));
    const float offset_u = decode_float24(data24(commands[0x4Au]));
    const float offset_v = decode_float24(data24(commands[0x4Bu]));
    vertex.u = (vertex.u * scale_u + offset_u) * texture_width;
    vertex.v = (vertex.v * scale_v + offset_v) * texture_height;
    return true;
}

bool decode_vertex_optimized(const psprecomp::GuestMemory &memory, std::uint32_t address,
                             const VertexLayout &layout,
                             const std::array<std::uint32_t, 256> &commands,
                             const GeTransformState &transform,
                             Vertex &vertex, std::string &error) {
    const std::uint32_t uv_mode = data24(commands[0xC0u]) & 3u;
    if (layout.type == 0x000115u && !layout.through &&
        (data24(commands[0x17u]) & 1u) == 0u && (uv_mode == 0u || uv_mode == 3u))
        return decode_vertex_0115_fast(memory, address, layout, commands, transform, vertex, error);
    return decode_vertex(memory, address, layout, commands, transform, vertex, error);
}


bool decode_model_vertex_0115_for_gpu_fast(
    const psprecomp::GuestMemory &memory, std::uint32_t address,
    const VertexLayout &layout, const GeTransformState &transform,
    const std::array<std::uint32_t, 256> &commands,
    bool lighting_enabled, const PreparedLighting *prepared_lighting,
    GeGpuVertex &vertex, std::string &error,
    const std::uint8_t *prevalidated_raw = nullptr,
    const Vec3 *precomputed_world_normal = nullptr) {
    // Common VCS world layout: u8 UV + 5551 colour + s16 XYZ, no normal,
    // no weights, one morph.  The generic decoder performs several format
    // switches and six separately bounds-checked guest loads per vertex.
    // Resolve the packed vertex once and decode straight from the host pointer.
    // Contiguous GPU draws validate the entire source span once before the
    // worker loop and pass a direct pointer here.  This avoids canonicalization,
    // region selection and bounds checking once per vertex on the dominant city
    // format. Sparse callers retain the exact old checked path.
    const std::uint8_t *raw = prevalidated_raw != nullptr
        ? prevalidated_raw : memory.raw_pointer(address, layout.stride);
    if (raw == nullptr) {
        error = "GE hardware-transform vertex lies outside guest memory at " +
            psprecomp::hex32(address);
        return false;
    }
    const auto le16 = [raw](std::uint32_t offset) noexcept {
        return static_cast<std::uint16_t>(raw[offset]) |
            (static_cast<std::uint16_t>(raw[offset + 1u]) << 8u);
    };

    const float u = static_cast<float>(raw[layout.tc_offset]) * (1.0f / 128.0f);
    const float v = static_cast<float>(raw[layout.tc_offset + 1u]) * (1.0f / 128.0f);
    const Color color = unpack16(le16(layout.color_offset), 1u);
    Vec3 model_position{
        signed_normalized16(le16(layout.position_offset)),
        signed_normalized16(le16(layout.position_offset + 2u)),
        signed_normalized16(le16(layout.position_offset + 4u)),
    };

    Color final_color = color;
    if (lighting_enabled) {
        const Vec3 world_position = transform_4x3(transform.world, model_position);
        Vec3 world_normal{};
        if (precomputed_world_normal != nullptr) {
            world_normal = *precomputed_world_normal;
        } else {
            world_normal = transform_normal_4x3(
                transform.world, Vec3{0.0f, 0.0f, 1.0f});
            if ((data24(commands[0x51u]) & 1u) != 0u) world_normal = world_normal * -1.0f;
            world_normal = normalized_or_001(world_normal);
        }
        final_color = prepared_lighting
            ? apply_prepared_lighting(color, world_position, world_normal,
                                      *prepared_lighting, true)
            : apply_lighting(color, true, world_position, world_normal, commands);
        model_position = world_position;
    }

    vertex = {};
    vertex.x = model_position.x;
    vertex.y = model_position.y;
    vertex.z = model_position.z;
    vertex.w = 1.0f;
    vertex.u = u;
    vertex.v = v;
    vertex.q = 1.0f;
    vertex.fog_factor = 1.0f;
    vertex.rgba = static_cast<std::uint32_t>(final_color.r) |
        (static_cast<std::uint32_t>(final_color.g) << 8u) |
        (static_cast<std::uint32_t>(final_color.b) << 16u) |
        (static_cast<std::uint32_t>(final_color.a) << 24u);
    return true;
}

bool decode_model_vertex_for_gpu(const psprecomp::GuestMemory &memory,
                                 std::uint32_t address,
                                 const VertexLayout &layout,
                                 const GeTransformState &transform,
                                 const std::array<std::uint32_t, 256> &commands,
                                 bool lighting_enabled,
                                 const PreparedLighting *prepared_lighting,
                                 std::uint32_t uv_generation,
                                 GeGpuVertex &vertex,
                                 std::string &error) {
    // The common VCS city vertex format does not need the generic decoder at
    // all when UV generation is direct. Keep lighting support so the same
    // specialization covers lit and unlit opaque world geometry.
    if (layout.type == 0x000115u && !layout.through &&
        layout.morph_count == 1u && layout.weight_type == 0u &&
        layout.normal_type == 0u && uv_generation == 0u) {
        return decode_model_vertex_0115_for_gpu_fast(
            memory, address, layout, transform, commands, lighting_enabled,
            prepared_lighting, vertex, error);
    }

    if (!memory.contains(address, layout.stride)) {
        error = "GE hardware-transform vertex lies outside guest memory at " +
            psprecomp::hex32(address);
        return false;
    }

    float u = 0.0f, v = 0.0f;
    if (layout.morph_count == 1u) {
        read_texcoord(memory, address + layout.tc_offset, layout.tc_type, false, u, v);
    } else {
        for (std::uint32_t morph = 0u; morph < layout.morph_count; ++morph) {
            float mu = 0.0f, mv = 0.0f;
            read_texcoord(memory, address + morph * layout.one_size + layout.tc_offset,
                          layout.tc_type, false, mu, mv);
            u += mu * transform.morph_weights[morph];
            v += mv * transform.morph_weights[morph];
        }
    }

    const Color color = morph_color(memory, address, layout, transform, commands);
    Vec3 model_position{};
    if (layout.morph_count == 1u) {
        model_position = read_vector3(memory, address + layout.position_offset,
                                      layout.position_type);
    } else {
        for (std::uint32_t morph = 0u; morph < layout.morph_count; ++morph) {
            model_position += read_vector3(
                memory, address + morph * layout.one_size + layout.position_offset,
                layout.position_type) * transform.morph_weights[morph];
        }
    }
    const std::uint32_t uv_generation_source =
        (data24(commands[0xC0u]) >> 8u) & 3u;
    const bool model_normal_needed = lighting_enabled || uv_generation == 2u ||
        (uv_generation == 1u && uv_generation_source >= 2u);
    Vec3 model_normal{0.0f, 0.0f, 1.0f};
    if (model_normal_needed && layout.normal_type != 0u)
        model_normal = read_vector3(memory, address + layout.normal_offset,
                                    layout.normal_type);

    if (layout.weight_type != 0u) {
        const std::array<float, 12> skin = compute_skin_matrix(memory, address, layout, transform);
        model_position = transform_4x3(skin, model_position);
        if (model_normal_needed)
            model_normal = transform_normal_4x3(skin, model_normal);
    }

    // Generate reflection/matrix UVs on the CPU just like decode_vertex().
    // Previously these passes stayed on the legacy transform while the opaque
    // vehicle body used Vulkan, causing tiny depth disagreements and visible
    // flashing between the two layers.
    float generated_q = 1.0f;
    if (uv_generation == 1u) {
        Vec3 source{};
        switch (uv_generation_source) {
        case 0u: source = model_position; break;
        case 1u: source = {u, v, 0.0f}; break;
        case 2u: source = normalized_or_zero(model_normal); break;
        case 3u: source = model_normal; break;
        }
        const Vec3 stq = transform_4x3(transform.texture, source);
        u = stq.x;
        v = stq.y;
        generated_q = stq.z;
    } else if (uv_generation == 2u) {
        Vec3 world_normal = transform_normal_4x3(transform.world, model_normal);
        if ((data24(commands[0x51u]) & 1u) != 0u) world_normal = world_normal * -1.0f;
        world_normal = normalized_or_001(world_normal);
        const std::uint32_t shade = data24(commands[0xC1u]);
        const std::uint32_t light_s = shade & 3u;
        const std::uint32_t light_t = (shade >> 8u) & 3u;
        const auto light_vector = [&](std::uint32_t light) {
            return normalized_or_001(Vec3{
                decode_float24(data24(commands[0x63u + light * 3u])),
                decode_float24(data24(commands[0x64u + light * 3u])),
                decode_float24(data24(commands[0x65u + light * 3u])),
            });
        };
        u = (dot(light_vector(light_s), world_normal) + 1.0f) * 0.5f;
        v = (dot(light_vector(light_t), world_normal) + 1.0f) * 0.5f;
    }

    // Hybrid path for lit draws. A gate census over the capture route found
    // that 15823949 of 17481629 vertices -- 90.5% -- were refused by the
    // hardware frontend for one reason only: lighting enabled. Lighting needs
    // world-space position and normal, so the world matrix has to stay on the
    // CPU; view, projection, viewport and clipping do not, and those are what
    // triprep and two thirds of the matrix work cost.
    //
    // The vertex colour is computed by exactly the same apply_lighting() the
    // legacy path uses, so colour output is unchanged -- only where the
    // remaining matrices are applied moves.
    Color final_color = color;
    if (lighting_enabled) {
        // Same default decode_vertex() uses when the vertex format carries no
        // normal; the GE still lights those vertices.
        const Vec3 world_position = transform_4x3(transform.world, model_position);
        Vec3 world_normal = transform_normal_4x3(transform.world, model_normal);
        if ((data24(commands[0x51u]) & 1u) != 0u) world_normal = world_normal * -1.0f;
        world_normal = normalized_or_001(world_normal);
        final_color = prepared_lighting
            ? apply_prepared_lighting(color, world_position, world_normal, *prepared_lighting)
            : apply_lighting(color, layout.color_type >= 4u, world_position,
                             world_normal, commands);
        model_position = world_position;
    }

    vertex = {};
    vertex.x = model_position.x;
    vertex.y = model_position.y;
    vertex.z = model_position.z;
    vertex.w = 1.0f;
    vertex.u = u;
    vertex.v = v;
    vertex.q = generated_q;
    vertex.fog_factor = 1.0f;
    vertex.rgba = static_cast<std::uint32_t>(final_color.r) |
        (static_cast<std::uint32_t>(final_color.g) << 8u) |
        (static_cast<std::uint32_t>(final_color.b) << 16u) |
        (static_cast<std::uint32_t>(final_color.a) << 24u);
    return true;
}

std::uint32_t framebuffer_address(const std::array<std::uint32_t, 256> &commands) noexcept {
    return kVramBase | (data24(commands[0x9Cu]) & 0x001FFFF0u);
}
std::uint32_t depthbuffer_address(const std::array<std::uint32_t, 256> &commands) noexcept {
    return kVramBase | (data24(commands[0x9Eu]) & 0x001FFFF0u);
}
std::uint32_t texture_address(const std::array<std::uint32_t, 256> &commands,
                              std::uint32_t level) noexcept {
    level = std::min(level, 7u);
    return (data24(commands[0xA0u + level]) & 0x00FFFFF0u) |
           ((data24(commands[0xA8u + level]) << 8u) & 0x0F000000u);
}
std::uint32_t texture_address(const std::array<std::uint32_t, 256> &commands) noexcept {
    return texture_address(commands, 0u);
}

// Whether the CPU rasterizer can leave this surface alone.
//
// With the Vulkan backend on, every primitive is rasterized twice: here on the
// CPU into guest VRAM, and again on the GPU. The CPU copy still matters for
// surfaces the game samples back, but not for the one the swapchain is showing
// -- nothing reads those pixels. Skipping just that surface keeps VRAM correct
// everywhere else.
//
// The same applies to the surface the guest handed to sceDisplaySetFrameBuf
// while the backend presents its own image. VCS composes into it with 64
// through-mode rectangles that cost about 5.6 ms per rendered frame, and the
// composition never reaches the screen on the swapchain path.
//
// That rests on two conditions, and both are counted rather than assumed --
// report_present_stats() prints them at exit:
//
//   display_fb_sampled_draws  nothing may sample the displayed surface back as
//                             a texture, or the skipped pixels become visible
//   software_after_gpu        the run must never return to presenting that
//                             surface by GDI after the swapchain took over,
//                             which is when a stale buffer would reach the
//                             window
//
// Both were zero over a 4000-vblank scripted route and over a two-minute
// interactive session. If either ever becomes nonzero, set
// PSPRECOMP_GE_GPU_SKIP_DISPLAYED_RASTER=0 and say so in the handoff. The
// structural fix that removes the conditions entirely is the GPU-to-GPU path.
bool software_raster_skipped(const std::array<std::uint32_t, 256> &commands) noexcept {
    static const bool skip_everything = [] {
        const char *value = std::getenv("PSPRECOMP_GE_GPU_SKIP_SOFTWARE_RASTER");
        if (value != nullptr && *value != '\0') return *value != '0';
        // Native DX12 GE is authoritative. The first physical Stage 44.6 run
        // exposed that the old conservative policy still CPU-rasterized most
        // offscreen targets as well as submitting the same geometry to D3D12.
        // That double raster is unnecessary once DX12GEColor is enabled.
        const VcsConfiguration &cfg = vcs_configuration();
        return cfg.initialized && cfg.rendering.backend == RenderingBackend::DirectX12 &&
               cfg.rendering.dx12_ge_color;
    }();
    static const bool skip_owned = [] {
        const char *value = std::getenv("PSPRECOMP_GE_GPU_SKIP_OWNED_RASTER");
        return value == nullptr || (*value != '\0' && *value != '0');
    }();
    static const bool skip_displayed = [] {
        const char *value = std::getenv("PSPRECOMP_GE_GPU_SKIP_DISPLAYED_RASTER");
        return value == nullptr || (*value != '\0' && *value != '0');
    }();
    if (!ge_gpu_backend_active()) return false;
    if (skip_everything) return true;
    const std::uint32_t target = framebuffer_address(commands) & 0x001FFFF0u;
    if (skip_owned) {
        const std::uint32_t owned = ge_gpu_backend_owned_framebuffer();
        if (owned != 0u && target == owned) return true;
    }
    if (skip_displayed && ge_gpu_backend_presents_directly()) {
        const std::uint32_t displayed = ge_gpu_backend_display_framebuffer();
        if (displayed != 0u && target == displayed) return true;
    }
    return false;
}

std::int32_t signed_texture_lod_offset16(std::uint32_t texlevel) noexcept {
    const std::uint32_t raw = (texlevel >> 16u) & 0xFFu;
    return (raw & 0x80u) != 0u ? static_cast<std::int32_t>(raw) - 256
                               : static_cast<std::int32_t>(raw);
}

std::uint32_t selected_texture_level(const std::array<std::uint32_t, 256> &commands) noexcept {
    const std::uint32_t texfilter = data24(commands[0xC6u]);
    const bool mip_enabled = (texfilter & 4u) != 0u;
    const std::uint32_t max_level = (data24(commands[0xC2u]) >> 16u) & 7u;
    if (!mip_enabled || max_level == 0u) return 0u;
    const std::uint32_t texlevel = data24(commands[0xC8u]);
    const std::uint32_t mode = texlevel & 3u;
    // PSP mode 1 is a constant 4.4 fixed-point LOD. Automatic/slope modes need
    // derivatives from the final rasterizer, so they correctly stay at level 0
    // until the full image mip chain is uploaded in a later physical-GPU pass.
    if (mode != 1u) return 0u;
    const float lod = static_cast<float>(signed_texture_lod_offset16(texlevel)) / 16.0f;
    const bool mip_linear = (texfilter & 2u) != 0u;
    const int selected = mip_linear ? static_cast<int>(std::floor(lod))
                                    : static_cast<int>(std::floor(lod + 0.5f));
    return static_cast<std::uint32_t>(std::clamp(selected, 0, static_cast<int>(max_level)));
}
std::uint32_t clut_address(const std::array<std::uint32_t, 256> &commands) noexcept {
    return (data24(commands[0xB0u]) & 0x00FFFFF0u) | ((data24(commands[0xB1u]) << 8u) & 0x0F000000u);
}

std::uint32_t bytes_per_pixel(std::uint32_t format) noexcept { return format == 3u ? 4u : 2u; }

std::uint32_t swizzled_offset(std::uint32_t byte_x, std::uint32_t y, std::uint32_t row_bytes) noexcept {
    const std::uint32_t blocks_per_row = (row_bytes + 15u) / 16u;
    const std::uint32_t block_x = byte_x / 16u;
    const std::uint32_t block_y = y / 8u;
    return (block_y * blocks_per_row + block_x) * 128u + (y & 7u) * 16u + (byte_x & 15u);
}

std::uint32_t wrap_coord(std::int32_t value, std::uint32_t dimension, bool clamp) noexcept {
    if (dimension == 0u) return 0u;
    if (clamp) return static_cast<std::uint32_t>(std::clamp<std::int32_t>(value, 0, static_cast<std::int32_t>(dimension - 1u)));
    // GE texture dimensions are powers of two.  Masking gives the same wrapped
    // result for positive and negative two's-complement coordinates without an
    // integer division in every nearest sample (four times for bilinear).
    if ((dimension & (dimension - 1u)) == 0u)
        return static_cast<std::uint32_t>(value) & (dimension - 1u);
    const std::int32_t d = static_cast<std::int32_t>(dimension);
    std::int32_t result = value % d;
    if (result < 0) result += d;
    return static_cast<std::uint32_t>(result);
}

Color read_clut(const psprecomp::GuestMemory &memory, const std::array<std::uint32_t, 256> &commands,
                std::uint32_t raw_index) {
    const std::uint32_t format_data = data24(commands[0xC5u]);
    const std::uint32_t palette_format = format_data & 3u;
    const std::uint32_t shift = (format_data >> 2u) & 0x1Fu;
    const std::uint32_t mask = (format_data >> 8u) & 0xFFu;
    const std::uint32_t start = ((format_data >> 16u) & 0x1Fu) << 4u;
    const std::uint32_t wrap_mask = palette_format == 3u ? 0xFFu : 0x1FFu;
    const std::uint32_t index = (((raw_index >> shift) & mask) | (start & wrap_mask)) & wrap_mask;
    const std::uint32_t address = clut_address(commands) + index * (palette_format == 3u ? 4u : 2u);
    if (!memory.contains(address, palette_format == 3u ? 4u : 2u)) return {};
    if (palette_format == 3u) return unpack32(memory.aot_load32(address));
    return unpack16(memory.aot_load16(address), palette_format);
}

// Per-primitive texture state.
//
// A bilinear fetch calls the nearest sampler four times, and each call used to
// re-decode the texture size, wrap mode, pixel format, buffer width, swizzle
// flag and base address, then take a bounds-checked guest load -- plus the same
// again inside read_clut for palettised formats.  None of it varies within a
// primitive, so it is decoded once and the sampler becomes address arithmetic
// over a raw pointer.
struct TextureSetup {
    const std::uint8_t *pixels{};
    const std::uint8_t *clut_pixels{};
    std::uint32_t base{};
    std::uint32_t clut_base{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t buffer_width{};
    std::uint32_t format{};
    std::uint32_t clut_format{};
    std::uint32_t clut_shift{};
    std::uint32_t clut_mask{};
    std::uint32_t clut_start{};
    std::uint32_t clut_wrap_mask{};
    std::uint32_t clut_entry_bytes{};
    bool swizzled{};
    bool clamp_u{};
    bool clamp_v{};
    bool linear{};
    std::uint32_t selected_level{};
};

TextureSetup make_texture_setup_for_level(const psprecomp::GuestMemory &memory, const std::array<std::uint32_t, 256> &commands, std::uint32_t requested_level) noexcept {
    TextureSetup setup{}; setup.selected_level=std::min<std::uint32_t>(requested_level,7u);
    const std::uint32_t size = data24(commands[0xB8u + setup.selected_level]);
    setup.width = 1u << (size & 0xFu);
    setup.height = 1u << ((size >> 8u) & 0xFu);
    const std::uint32_t wrap = data24(commands[0xC7u]);
    setup.clamp_u = (wrap & 1u) != 0u;
    setup.clamp_v = (wrap & 0x100u) != 0u;
    setup.format = data24(commands[0xC3u]) & 0xFu;
    setup.buffer_width = std::max<std::uint32_t>(
        1u, data24(commands[0xA8u + setup.selected_level]) & 0x7FFu);
    setup.swizzled = (data24(commands[0xC2u]) & 1u) != 0u;
    setup.base = texture_address(commands, setup.selected_level);
    setup.linear = ((data24(commands[0xC6u]) >> 8u) & 1u) != 0u;

    const std::uint32_t clut_data = data24(commands[0xC5u]);
    setup.clut_format = clut_data & 3u;
    setup.clut_shift = (clut_data >> 2u) & 0x1Fu;
    setup.clut_mask = (clut_data >> 8u) & 0xFFu;
    setup.clut_start = ((clut_data >> 16u) & 0x1Fu) << 4u;
    setup.clut_wrap_mask = setup.clut_format == 3u ? 0xFFu : 0x1FFu;
    setup.clut_entry_bytes = setup.clut_format == 3u ? 4u : 2u;
    setup.clut_base = clut_address(commands);

    // Bound the texture by the largest offset the swizzled or linear layout can
    // produce for this size, so a single resolve covers every texel fetched.
    const std::uint32_t row_bytes = setup.buffer_width * 4u;
    const std::uint64_t span = static_cast<std::uint64_t>(setup.height + 8u) * (row_bytes + 128u);
    setup.pixels = memory.raw_pointer(setup.base, static_cast<std::size_t>(span));
    setup.clut_pixels = memory.raw_pointer(setup.clut_base,
                                           (setup.clut_wrap_mask + 1u) * setup.clut_entry_bytes);
    return setup;
}

TextureSetup make_texture_setup(const psprecomp::GuestMemory &memory, const std::array<std::uint32_t, 256> &commands) noexcept { return make_texture_setup_for_level(memory,commands,selected_texture_level(commands)); }

// Size in bytes of a texture's stored image, padding included. Shared by the
// content signature and by the DDS replacement index, which has to hash exactly
// the same span the GE samples for its match to mean anything.
std::uint64_t texture_source_byte_size(const TextureSetup &texture) noexcept {
    if (texture.base == 0u || texture.width == 0u || texture.height == 0u) return 0u;
    std::uint64_t bytes = 0u;
    switch (texture.format) {
    case 0u: case 1u: case 2u: case 6u:
        bytes = static_cast<std::uint64_t>(texture.buffer_width) * texture.height * 2u; break;
    case 3u: case 7u:
        bytes = static_cast<std::uint64_t>(texture.buffer_width) * texture.height * 4u; break;
    case 4u:
        bytes = static_cast<std::uint64_t>((texture.buffer_width + 1u) >> 1u) * texture.height; break;
    case 5u:
        bytes = static_cast<std::uint64_t>(texture.buffer_width) * texture.height; break;
    case 8u:
        bytes = static_cast<std::uint64_t>((texture.buffer_width + 3u) >> 2u) *
                ((texture.height + 3u) >> 2u) * 8u; break;
    case 9u: case 10u:
        bytes = static_cast<std::uint64_t>((texture.buffer_width + 3u) >> 2u) *
                ((texture.height + 3u) >> 2u) * 16u; break;
    default: return 0u;
    }
    if (texture.swizzled && texture.format <= 7u) {
        // PSP swizzle stores 16-byte x 8-row blocks. Include storage padding so
        // a write in a padded block still changes the signature.
        std::uint64_t row = bytes / std::max<std::uint32_t>(1u, texture.height);
        row = (row + 15u) & ~15ull;
        bytes = row * ((static_cast<std::uint64_t>(texture.height) + 7u) & ~7ull);
    }
    if (bytes > std::numeric_limits<std::size_t>::max()) return 0u;
    return bytes;
}

std::uint64_t texture_source_signature(const psprecomp::GuestMemory &memory,
                                       const TextureSetup &texture) noexcept {
    const std::uint64_t bytes = texture_source_byte_size(texture);
    if (bytes == 0u) return 0u;
    const auto size = static_cast<std::size_t>(bytes);
    const std::uint8_t *pixels = memory.raw_pointer(texture.base, size);
    if (pixels == nullptr) return 0u;

    // Full hash for small dynamic surfaces; evenly distributed cache-line
    // samples for large world textures. Stage 40 mixed one byte at a time, so a
    // 1 KiB validation sample executed ~1,000 dependent multiply operations.
    // Fold eight bytes per iteration instead. The signature is process-local
    // cache validation, not a file format, so only collision quality matters.
    std::uint64_t hash = 0x9E3779B97F4A7C15ull;
    const auto mix64 = [&hash](std::uint64_t value) noexcept {
        hash ^= value + 0x9E3779B97F4A7C15ull + (hash << 6u) + (hash >> 2u);
        hash *= 0xD6E8FEB86659FD93ull;
        hash ^= hash >> 29u;
    };
    const auto hash_range = [&](std::size_t begin, std::size_t end) noexcept {
        std::size_t i = begin;
        while (i + 8u <= end) {
            std::uint64_t word{};
            std::memcpy(&word, pixels + i, sizeof(word));
            mix64(word);
            i += 8u;
        }
        if (i < end) {
            std::uint64_t tail = 0u;
            std::memcpy(&tail, pixels + i, end - i);
            mix64(tail ^ (static_cast<std::uint64_t>(end - i) << 56u));
        }
    };
    if (size <= 4096u) {
        hash_range(0u, size);
    } else {
        constexpr std::size_t blocks = 16u;
        constexpr std::size_t block_bytes = 64u;
        for (std::size_t block = 0u; block < blocks; ++block) {
            const std::size_t center = (size - 1u) * block / (blocks - 1u);
            const std::size_t begin = center > block_bytes / 2u ? center - block_bytes / 2u : 0u;
            hash_range(begin, std::min(size, begin + block_bytes));
        }
    }
    hash ^= static_cast<std::uint64_t>(size) +
            (static_cast<std::uint64_t>(texture.width) << 32u) + texture.height;
    hash *= 1099511628211ull;
    return hash == 0u ? 1u : hash;
}

Color read_clut_fast(const psprecomp::GuestMemory &memory,
                     const TextureSetup &texture, std::uint32_t raw_index) {
    const std::uint32_t index =
        (((raw_index >> texture.clut_shift) & texture.clut_mask) |
         (texture.clut_start & texture.clut_wrap_mask)) & texture.clut_wrap_mask;
    const std::uint32_t offset = index * texture.clut_entry_bytes;
    if (texture.clut_pixels != nullptr) {
        const std::uint8_t *entry = texture.clut_pixels + offset;
        if (texture.clut_format == 3u) {
            return unpack32(static_cast<std::uint32_t>(entry[0]) |
                            (static_cast<std::uint32_t>(entry[1]) << 8u) |
                            (static_cast<std::uint32_t>(entry[2]) << 16u) |
                            (static_cast<std::uint32_t>(entry[3]) << 24u));
        }
        return unpack16(static_cast<std::uint16_t>(
                            static_cast<std::uint16_t>(entry[0]) |
                            (static_cast<std::uint16_t>(entry[1]) << 8u)),
                        texture.clut_format);
    }
    const std::uint32_t address = texture.clut_base + offset;
    if (!memory.contains(address, texture.clut_entry_bytes)) return {};
    if (texture.clut_format == 3u) return unpack32(memory.aot_load32(address));
    return unpack16(memory.aot_load16(address), texture.clut_format);
}


std::uint16_t load_le16(const std::uint8_t *p) noexcept {
    return static_cast<std::uint16_t>(static_cast<std::uint16_t>(p[0]) |
                                      (static_cast<std::uint16_t>(p[1]) << 8u));
}

std::uint32_t load_le32(const std::uint8_t *p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8u) |
           (static_cast<std::uint32_t>(p[2]) << 16u) |
           (static_cast<std::uint32_t>(p[3]) << 24u);
}

Color dxt_color(std::uint16_t c1, std::uint16_t c2, std::uint32_t index,
                std::uint8_t alpha, bool dxt1_alpha) noexcept {
    const auto endpoint = [](std::uint16_t c, std::uint8_t a) noexcept {
        // PSP S3TC endpoints use the GE's BGR565 bit ordering rather than
        // PC DXT's usual RGB presentation.
        return Color{
            static_cast<std::uint8_t>((c >> 8u) & 0xF8u),
            static_cast<std::uint8_t>((c >> 3u) & 0xFCu),
            static_cast<std::uint8_t>((c << 3u) & 0xF8u),
            a,
        };
    };
    const Color a = endpoint(c1, alpha);
    const Color b = endpoint(c2, alpha);
    if (index == 0u) return a;
    if (index == 1u) return b;
    const auto mix23 = [](std::uint8_t first, std::uint8_t second) noexcept {
        return static_cast<std::uint8_t>((static_cast<unsigned>(first) * 2u + second) / 3u);
    };
    if (c1 > c2 || !dxt1_alpha) {
        if (index == 2u) return {mix23(a.r,b.r), mix23(a.g,b.g), mix23(a.b,b.b), alpha};
        return {mix23(b.r,a.r), mix23(b.g,a.g), mix23(b.b,a.b), alpha};
    }
    if (index == 3u) return {0u,0u,0u,0u};
    return {
        static_cast<std::uint8_t>((static_cast<unsigned>(a.r) + b.r) / 2u),
        static_cast<std::uint8_t>((static_cast<unsigned>(a.g) + b.g) / 2u),
        static_cast<std::uint8_t>((static_cast<unsigned>(a.b) + b.b) / 2u),
        alpha,
    };
}

Color sample_dxt_texel(const psprecomp::GuestMemory &memory,
                       const TextureSetup &texture,
                       std::uint32_t x, std::uint32_t y) {
    const std::uint32_t block_size = texture.format == 8u ? 8u : 16u;
    const std::uint32_t blocks_per_row = std::max(1u, (texture.buffer_width + 3u) / 4u);
    const std::uint32_t block_offset = ((y / 4u) * blocks_per_row + (x / 4u)) * block_size;
    std::array<std::uint8_t,16> checked{};
    const std::uint8_t *block = nullptr;
    if (texture.pixels != nullptr) {
        block = texture.pixels + block_offset;
    } else {
        if (!memory.contains(texture.base + block_offset, block_size)) return {};
        for (std::uint32_t i=0;i<block_size;++i)
            checked[i] = memory.aot_load8(texture.base + block_offset + i);
        block = checked.data();
    }

    // PSP DXT blocks are byte-reversed relative to desktop S3TC: color row
    // selectors come first, followed by the two 565 endpoints. DXT3/DXT5 then
    // append alpha data after that color block.
    const std::uint32_t row = y & 3u;
    const std::uint32_t column = x & 3u;
    const std::uint32_t color_index = (block[row] >> (column * 2u)) & 3u;
    const std::uint16_t c1 = load_le16(block + 4u);
    const std::uint16_t c2 = load_le16(block + 6u);
    if (texture.format == 8u)
        return dxt_color(c1,c2,color_index,255u,true);
    if (texture.format == 9u) {
        const std::uint16_t alpha_line = load_le16(block + 8u + row * 2u);
        const std::uint8_t alpha = static_cast<std::uint8_t>(((alpha_line >> (column * 4u)) & 0xFu) * 17u);
        return dxt_color(c1,c2,color_index,alpha,false);
    }

    const std::uint32_t alpha_data_low = load_le32(block + 8u);
    const std::uint16_t alpha_data_high = load_le16(block + 12u);
    const std::uint64_t alpha_bits = (static_cast<std::uint64_t>(alpha_data_high) << 32u) |
                                     alpha_data_low;
    const std::uint32_t alpha_index = static_cast<std::uint32_t>(
        (alpha_bits >> (row * 12u + column * 3u)) & 7u);
    const std::uint8_t alpha1 = block[14u];
    const std::uint8_t alpha2 = block[15u];
    std::array<std::uint8_t,8> palette{alpha1,alpha2,0u,0u,0u,0u,0u,255u};
    if (alpha1 > alpha2) {
        for (std::uint32_t i=1u;i<=6u;++i) {
            const unsigned fixed = (static_cast<unsigned>(alpha1) * ((7u-i) << 8u)) / 7u +
                                   (static_cast<unsigned>(alpha2) * (i << 8u)) / 7u;
            palette[i+1u] = static_cast<std::uint8_t>((fixed + 31u) >> 8u);
        }
    } else {
        for (std::uint32_t i=1u;i<=4u;++i) {
            const unsigned fixed = (static_cast<unsigned>(alpha1) * ((5u-i) << 8u)) / 5u +
                                   (static_cast<unsigned>(alpha2) * (i << 8u)) / 5u;
            palette[i+1u] = static_cast<std::uint8_t>((fixed + 31u) >> 8u);
        }
        palette[6u]=0u; palette[7u]=255u;
    }
    return dxt_color(c1,c2,color_index,palette[alpha_index],false);
}

Color sample_texture_wrapped(const psprecomp::GuestMemory &memory,
                             const TextureSetup &texture,
                             std::uint32_t x, std::uint32_t y) {
    const std::uint32_t format = texture.format;
    const std::uint32_t buffer_width = texture.buffer_width;
    const bool swizzled = texture.swizzled;
    const std::uint32_t base = texture.base;
    const std::uint8_t *const pixels = texture.pixels;

    // Raw fetches when the whole texture resolved; otherwise the checked path.
    const auto fetch8 = [&](std::uint32_t offset, std::uint8_t &out) {
        if (pixels != nullptr) { out = pixels[offset]; return true; }
        if (!memory.contains(base + offset, 1u)) return false;
        out = memory.aot_load8(base + offset);
        return true;
    };
    const auto fetch16 = [&](std::uint32_t offset, std::uint16_t &out) {
        if (pixels != nullptr) {
            out = static_cast<std::uint16_t>(static_cast<std::uint16_t>(pixels[offset]) |
                                             (static_cast<std::uint16_t>(pixels[offset + 1u]) << 8u));
            return true;
        }
        if (!memory.contains(base + offset, 2u)) return false;
        out = memory.aot_load16(base + offset);
        return true;
    };
    const auto fetch32 = [&](std::uint32_t offset, std::uint32_t &out) {
        if (pixels != nullptr) {
            out = static_cast<std::uint32_t>(pixels[offset]) |
                  (static_cast<std::uint32_t>(pixels[offset + 1u]) << 8u) |
                  (static_cast<std::uint32_t>(pixels[offset + 2u]) << 16u) |
                  (static_cast<std::uint32_t>(pixels[offset + 3u]) << 24u);
            return true;
        }
        if (!memory.contains(base + offset, 4u)) return false;
        out = memory.aot_load32(base + offset);
        return true;
    };

    std::uint32_t offset{};
    std::uint32_t raw_index{};
    switch (format) {
    case 0u: case 1u: case 2u: {
        const std::uint32_t byte_x = x * 2u;
        offset = swizzled ? swizzled_offset(byte_x, y, buffer_width * 2u) : (y * buffer_width * 2u + byte_x);
        std::uint16_t texel{};
        if (!fetch16(offset, texel)) return {};
        return unpack16(texel, format);
    }
    case 3u: {
        const std::uint32_t byte_x = x * 4u;
        offset = swizzled ? swizzled_offset(byte_x, y, buffer_width * 4u) : (y * buffer_width * 4u + byte_x);
        std::uint32_t texel{};
        if (!fetch32(offset, texel)) return {};
        return unpack32(texel);
    }
    case 4u: {
        const std::uint32_t byte_x = x >> 1u;
        offset = swizzled ? swizzled_offset(byte_x, y, (buffer_width + 1u) / 2u) : (y * ((buffer_width + 1u) / 2u) + byte_x);
        std::uint8_t packed{};
        if (!fetch8(offset, packed)) return {};
        raw_index = (x & 1u) != 0u ? packed >> 4u : packed & 0xFu;
        return read_clut_fast(memory, texture, raw_index);
    }
    case 5u: {
        const std::uint32_t byte_x = x;
        offset = swizzled ? swizzled_offset(byte_x, y, buffer_width) : (y * buffer_width + byte_x);
        std::uint8_t packed{};
        if (!fetch8(offset, packed)) return {};
        return read_clut_fast(memory, texture, packed);
    }
    case 6u: {
        const std::uint32_t byte_x = x * 2u;
        offset = swizzled ? swizzled_offset(byte_x, y, buffer_width * 2u) : (y * buffer_width * 2u + byte_x);
        std::uint16_t packed{};
        if (!fetch16(offset, packed)) return {};
        return read_clut_fast(memory, texture, packed);
    }
    case 7u: {
        const std::uint32_t byte_x = x * 4u;
        offset = swizzled ? swizzled_offset(byte_x, y, buffer_width * 4u) : (y * buffer_width * 4u + byte_x);
        std::uint32_t packed{};
        if (!fetch32(offset, packed)) return {};
        return read_clut_fast(memory, texture, packed);
    }
    case 8u: case 9u: case 10u:
        return sample_dxt_texel(memory, texture, x, y);
    default:
        return {};
    }
}

Color sample_texture_nearest(const psprecomp::GuestMemory &memory,
                             const TextureSetup &texture, float u, float v) {
    const std::uint32_t x = wrap_coord(floor_to_int(u), texture.width, texture.clamp_u);
    const std::uint32_t y = wrap_coord(floor_to_int(v), texture.height, texture.clamp_v);
    return sample_texture_wrapped(memory, texture, x, y);
}

Color lerp_color(Color a, Color b, float t) noexcept {
#if PSPRECOMP_GE_X86_SIMD
    const __m128 av = _mm_set_ps(static_cast<float>(a.a), static_cast<float>(a.b),
                                 static_cast<float>(a.g), static_cast<float>(a.r));
    const __m128 bv = _mm_set_ps(static_cast<float>(b.a), static_cast<float>(b.b),
                                 static_cast<float>(b.g), static_cast<float>(b.r));
    const __m128 result = _mm_add_ps(av, _mm_mul_ps(_mm_sub_ps(bv, av), _mm_set1_ps(t)));
    // Every interpolated lane stays in [0, 255]. For non-negative values,
    // trunc(x + 0.5) is exactly C lroundf's halfway-away-from-zero rule. Pack
    // the four lanes directly instead of spilling and calling lroundf four
    // times (twelve calls for one bilinear texture sample).
    const __m128i rounded = _mm_cvttps_epi32(
        _mm_add_ps(result, _mm_set1_ps(0.5f)));
    const __m128i packed16 = _mm_packs_epi32(rounded, _mm_setzero_si128());
    const __m128i packed8 = _mm_packus_epi16(packed16, _mm_setzero_si128());
    const std::uint32_t packed = static_cast<std::uint32_t>(_mm_cvtsi128_si32(packed8));
    return std::bit_cast<Color>(packed);
#else
    auto channel = [t](std::uint8_t x, std::uint8_t y) {
        return round_clamp_to_byte(static_cast<float>(x) + (static_cast<float>(y) - x) * t);
    };
    return {channel(a.r, b.r), channel(a.g, b.g), channel(a.b, b.b), channel(a.a, b.a)};
#endif
}

Color sample_texture(const psprecomp::GuestMemory &memory,
                     const TextureSetup &texture, float u, float v) {
    if (!texture.linear) return sample_texture_nearest(memory, texture, u, v);
    const float shifted_u = u - 0.5f;
    const float shifted_v = v - 0.5f;
    const std::int32_t bx = floor_to_int(shifted_u);
    const std::int32_t by = floor_to_int(shifted_v);
    const float fx = shifted_u - static_cast<float>(bx);
    const float fy = shifted_v - static_cast<float>(by);
    const std::uint32_t x0 = wrap_coord(bx, texture.width, texture.clamp_u);
    const std::uint32_t x1 = wrap_coord(bx + 1, texture.width, texture.clamp_u);
    const std::uint32_t y0 = wrap_coord(by, texture.height, texture.clamp_v);
    const std::uint32_t y1 = wrap_coord(by + 1, texture.height, texture.clamp_v);

    // More than 94% of VCS textured primitives use bilinear, swizzled T4
    // textures. Decode that single dominant layout once per fragment instead
    // of dispatching the generic format/swizzle switch four times. Adjacent
    // nibbles share their packed byte when possible.
    if (texture.format == 4u && texture.swizzled && texture.pixels != nullptr &&
        texture.clut_pixels != nullptr) {
        const std::uint32_t row_bytes = (texture.buffer_width + 1u) >> 1u;
        const std::uint32_t blocks_per_row = (row_bytes + 15u) >> 4u;
        const auto packed_offset = [blocks_per_row](std::uint32_t x, std::uint32_t y) noexcept {
            const std::uint32_t byte_x = x >> 1u;
            return (((y >> 3u) * blocks_per_row + (byte_x >> 4u)) << 7u) +
                   ((y & 7u) << 4u) + (byte_x & 15u);
        };
        const auto index_from = [](std::uint8_t packed, std::uint32_t x) noexcept {
            return static_cast<std::uint32_t>((x & 1u) != 0u ? packed >> 4u : packed & 0xFu);
        };

        const std::uint32_t o00 = packed_offset(x0, y0);
        const std::uint32_t o10 = packed_offset(x1, y0);
        const std::uint32_t o01 = packed_offset(x0, y1);
        const std::uint32_t o11 = packed_offset(x1, y1);
        const std::uint8_t p00 = texture.pixels[o00];
        const std::uint8_t p10 = o10 == o00 ? p00 : texture.pixels[o10];
        const std::uint8_t p01 = texture.pixels[o01];
        const std::uint8_t p11 = o11 == o01 ? p01 : texture.pixels[o11];
        const Color c00 = read_clut_fast(memory, texture, index_from(p00, x0));
        const Color c10 = read_clut_fast(memory, texture, index_from(p10, x1));
        const Color c01 = read_clut_fast(memory, texture, index_from(p01, x0));
        const Color c11 = read_clut_fast(memory, texture, index_from(p11, x1));
        return lerp_color(lerp_color(c00, c10, fx), lerp_color(c01, c11, fx), fy);
    }

    const Color c00 = sample_texture_wrapped(memory, texture, x0, y0);
    const Color c10 = sample_texture_wrapped(memory, texture, x1, y0);
    const Color c01 = sample_texture_wrapped(memory, texture, x0, y1);
    const Color c11 = sample_texture_wrapped(memory, texture, x1, y1);
    return lerp_color(lerp_color(c00, c10, fx), lerp_color(c01, c11, fx), fy);
}


bool decode_texture_rgba_into(const psprecomp::GuestMemory &memory,
                              const TextureSetup &texture,
                              std::span<std::byte> rgba8) {
    if (texture.format > 10u || texture.width == 0u || texture.height == 0u ||
        texture.width > 2048u || texture.height > 2048u) return false;
    const std::uint64_t byte_count = static_cast<std::uint64_t>(texture.width) *
                                     texture.height * 4ull;
    if (byte_count != rgba8.size()) return false;

    // Resolve the 16-entry T4 palette once.  VCS overwhelmingly uses swizzled
    // T4 world textures; the old generic decoder performed a format switch,
    // swizzle calculation and CLUT transform for every texel.  One packed byte
    // contains two pixels, so this path also halves source loads.
    std::array<Color, 16> t4_palette{};
    std::array<std::uint32_t, 16> t4_palette_rgba{};
    const bool fast_t4 = texture.format == 4u && texture.pixels != nullptr;
    if (fast_t4) {
        for (std::uint32_t i = 0; i < t4_palette.size(); ++i) {
            t4_palette[i] = read_clut_fast(memory, texture, i);
            const Color c = t4_palette[i];
            std::uint32_t packed = static_cast<std::uint32_t>(c.r) |
                (static_cast<std::uint32_t>(c.g) << 8u) |
                (static_cast<std::uint32_t>(c.b) << 16u) |
                (static_cast<std::uint32_t>(c.a) << 24u);
            if constexpr (std::endian::native == std::endian::big)
                packed = ((packed & 0x000000FFu) << 24u) | ((packed & 0x0000FF00u) << 8u) |
                         ((packed & 0x00FF0000u) >> 8u) | ((packed & 0xFF000000u) >> 24u);
            t4_palette_rgba[i] = packed;
        }
    }

    auto decode_rows = [&](std::uint32_t first_row, std::uint32_t last_row) {
        if (fast_t4) {
            const std::uint32_t row_bytes = (texture.buffer_width + 1u) >> 1u;
            const std::uint32_t blocks_per_row = (row_bytes + 15u) >> 4u;
            for (std::uint32_t y = first_row; y <= last_row; ++y) {
                std::byte *dst = rgba8.data() +
                    static_cast<std::size_t>(y) * texture.width * 4u;
                const auto source_offset = [&](std::uint32_t byte_x) noexcept {
                    if (!texture.swizzled)
                        return static_cast<std::size_t>(y) * row_bytes + byte_x;
                    return static_cast<std::size_t>(
                        (((y >> 3u) * blocks_per_row + (byte_x >> 4u)) << 7u) +
                        ((y & 7u) << 4u) + (byte_x & 15u));
                };
                for (std::uint32_t x = 0u; x < texture.width; x += 2u) {
                    const std::uint8_t packed = texture.pixels[source_offset(x >> 1u)];
                    const std::uint32_t lo = t4_palette_rgba[packed & 0x0Fu];
                    std::memcpy(dst, &lo, sizeof(lo));
                    dst += 4;
                    if (x + 1u < texture.width) {
                        const std::uint32_t hi = t4_palette_rgba[packed >> 4u];
                        std::memcpy(dst, &hi, sizeof(hi));
                        dst += 4;
                    }
                }
            }
            return;
        }

        std::size_t offset = static_cast<std::size_t>(first_row) * texture.width * 4u;
        for (std::uint32_t y = first_row; y <= last_row; ++y) {
            for (std::uint32_t x = 0u; x < texture.width; ++x) {
                const Color color = sample_texture_wrapped(memory, texture, x, y);
                rgba8[offset + 0u] = static_cast<std::byte>(color.r);
                rgba8[offset + 1u] = static_cast<std::byte>(color.g);
                rgba8[offset + 2u] = static_cast<std::byte>(color.b);
                rgba8[offset + 3u] = static_cast<std::byte>(color.a);
                offset += 4u;
            }
        }
    };

    static const bool parallel_texture_decode = [] {
        const char *text = std::getenv("PSPRECOMP_GE_PARALLEL_TEXTURE_DECODE");
        return text == nullptr || (*text != '\0' && std::strcmp(text, "0") != 0);
    }();
    RowWorkerPool &pool = RowWorkerPool::instance();
    // T4 now does much less work per texel, so fan-out is worthwhile at a
    // slightly larger size. Other formats retain the old crossover.
    const std::uint64_t texels = static_cast<std::uint64_t>(texture.width) * texture.height;
    const std::uint64_t threshold = fast_t4 ? 32768u : 16384u;
    if (!parallel_texture_decode || pool.worker_count() <= 1u ||
        texels < threshold || texture.height < 2u) {
        decode_rows(0u, texture.height - 1u);
    } else {
        // Keep a meaningful amount of pixel work behind each wake-up. The old
        // path could wake 32 threads for a 128-row T4 texture, spending more on
        // synchronization than on the now-specialized nibble decoder.
        const std::uint64_t texels_per_participant = fast_t4 ? 8192u : 4096u;
        const unsigned useful_participants = std::max(2u, std::min<unsigned>(
            pool.worker_count(), static_cast<unsigned>((texels + texels_per_participant - 1u) /
                                                       texels_per_participant)));
        pool.run(0, static_cast<std::int32_t>(texture.height) - 1,
                 [&decode_rows](unsigned, std::int32_t begin, std::int32_t end) {
            decode_rows(static_cast<std::uint32_t>(begin),
                        static_cast<std::uint32_t>(end));
        }, useful_participants);
    }
    return true;
}

bool decode_texture_rgba(const psprecomp::GuestMemory &memory,
                         const TextureSetup &texture,
                         std::vector<std::byte> &rgba8) {
    const std::uint64_t byte_count = static_cast<std::uint64_t>(texture.width) *
                                     texture.height * 4ull;
    if (byte_count > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()))
        return false;
    try {
        rgba8.resize(static_cast<std::size_t>(byte_count));
        if (decode_texture_rgba_into(memory, texture, rgba8)) return true;
    } catch (...) {
    }
    rgba8.clear();
    return false;
}

// Per-primitive raster state.
//
// write_fragment used to re-decode every one of these GE registers for each
// pixel it touched -- framebuffer format/stride/address, both scissor corners,
// the clear flags, the colour write mask and the whole depth configuration.
// At 480x272 with heavy overdraw that decoding dominated the frame. The values
// cannot change inside a primitive, so they are decoded once here and the
// pixel loop only reads plain fields.
struct FragmentSetup {
    std::uint32_t framebuffer_format{};
    std::uint32_t framebuffer_stride{};
    std::uint32_t framebuffer_base{};
    std::uint32_t framebuffer_bpp{};
    std::int32_t scissor_x0{};
    std::int32_t scissor_y0{};
    std::int32_t scissor_x1{};
    std::int32_t scissor_y1{};
    std::uint32_t write_mask{};
    std::uint32_t depth_stride{};
    std::uint32_t depth_base{};
    std::uint32_t depth_function{};
    bool texture_enabled{};
    bool clear_mode{};
    bool clear_color{};
    bool clear_alpha{};
    bool clear_depth{};
    bool depth_test_enabled{};
    bool depth_write_enabled{};
    bool valid{};
    // Resolved once per primitive; null falls back to the checked accessors.
    std::uint8_t *color_pixels{};
    std::uint8_t *depth_pixels{};
    TextureSetup texture{};
    // Blend, alpha-test and texture-function registers, decoded once.
    bool blend_enabled{};
    std::uint32_t blend_equation{};
    std::uint32_t blend_source_factor{};
    std::uint32_t blend_dest_factor{};
    Color blend_fix_source{};
    Color blend_fix_dest{};
    bool alpha_test_enabled{};
    std::uint32_t alpha_function{};
    std::uint32_t alpha_mask{};
    std::uint32_t alpha_reference{};
    std::uint32_t texture_function{};
    Color texture_env{};
    bool texture_use_alpha{};
    bool texture_double_color{};
};


// Resolves the frame and depth buffers to raw pointers for the rows this
// primitive can touch.  Both stay null when the range is not contiguous in host
// memory, and every pixel then takes the original checked path.
void bind_fragment_buffers(FragmentSetup &setup, psprecomp::GuestMemory &memory,
                           const std::array<std::uint32_t, 256> &commands) noexcept {
    if (!setup.valid || setup.scissor_y1 < 0) return;
    if (setup.texture_enabled) setup.texture = make_texture_setup(memory, commands);
    const std::size_t rows = static_cast<std::size_t>(setup.scissor_y1) + 1u;
    const std::size_t color_bytes = rows * setup.framebuffer_stride * setup.framebuffer_bpp;
    setup.color_pixels = memory.raw_pointer(setup.framebuffer_base, color_bytes);
    if (setup.depth_stride != 0u) {
        const std::size_t depth_bytes = rows * setup.depth_stride * 2u;
        setup.depth_pixels = memory.raw_pointer(setup.depth_base, depth_bytes);
    }
}

FragmentSetup make_fragment_setup(const std::array<std::uint32_t, 256> &commands) noexcept {
    FragmentSetup setup{};
    setup.framebuffer_stride = data24(commands[0x9Du]) & 0x7FCu;
    if (setup.framebuffer_stride == 0u) return setup;
    setup.framebuffer_format = data24(commands[0xD2u]) & 3u;
    setup.framebuffer_base = framebuffer_address(commands);
    setup.framebuffer_bpp = bytes_per_pixel(setup.framebuffer_format);

    const std::uint32_t scissor1 = data24(commands[0xD4u]);
    const std::uint32_t scissor2 = data24(commands[0xD5u]);
    setup.scissor_x0 = static_cast<std::int32_t>(scissor1 & 0x3FFu);
    setup.scissor_y0 = static_cast<std::int32_t>((scissor1 >> 10u) & 0x3FFu);
    setup.scissor_x1 = scissor2 == 0u ? static_cast<std::int32_t>(setup.framebuffer_stride - 1u)
                                      : static_cast<std::int32_t>(scissor2 & 0x3FFu);
    setup.scissor_y1 = scissor2 == 0u ? 271 : static_cast<std::int32_t>((scissor2 >> 10u) & 0x3FFu);

    const std::uint32_t clear = data24(commands[0xD3u]);
    setup.clear_mode = (clear & 1u) != 0u;
    setup.clear_color = (clear & 0x100u) != 0u;
    setup.clear_alpha = (clear & 0x200u) != 0u;
    setup.clear_depth = (clear & 0x400u) != 0u;
    setup.texture_enabled = (data24(commands[0x1Eu]) & 1u) != 0u;
    setup.write_mask = data24(commands[0xE8u]) | ((data24(commands[0xE9u]) & 0xFFu) << 24u);

    setup.depth_stride = data24(commands[0x9Fu]) & 0x7FCu;
    setup.depth_base = depthbuffer_address(commands);
    setup.depth_test_enabled = (data24(commands[0x23u]) & 1u) != 0u;
    setup.depth_function = data24(commands[0xDEu]) & 7u;
    setup.depth_write_enabled = (data24(commands[0xE7u]) & 1u) == 0u;
    const std::uint32_t blend_mode = data24(commands[0xDFu]);
    setup.blend_enabled = (data24(commands[0x21u]) & 1u) != 0u;
    setup.blend_equation = (blend_mode >> 8u) & 7u;
    setup.blend_source_factor = blend_mode & 0xFu;
    setup.blend_dest_factor = (blend_mode >> 4u) & 0xFu;
    setup.blend_fix_source = unpack32(data24(commands[0xE0u]) | 0xFF000000u);
    setup.blend_fix_dest = unpack32(data24(commands[0xE1u]) | 0xFF000000u);

    const std::uint32_t alpha = data24(commands[0xDBu]);
    setup.alpha_test_enabled = (data24(commands[0x22u]) & 1u) != 0u;
    setup.alpha_function = alpha & 7u;
    setup.alpha_mask = (alpha >> 16u) & 0xFFu;
    setup.alpha_reference = (alpha >> 8u) & 0xFFu;

    const std::uint32_t texfunc = data24(commands[0xC9u]);
    setup.texture_function = texfunc & 7u;
    setup.texture_use_alpha = (texfunc & 0x100u) != 0u;
    setup.texture_double_color = (texfunc & 0x10000u) != 0u;
    setup.texture_env = unpack32(data24(commands[0xCAu]) | 0xFF000000u);

    setup.valid = true;
    return setup;
}

// Only registers consumed by make_fragment_setup participate. Most city
// draws repeat this state for long runs, so avoid decoding it again until a
// relevant register actually changes.
constexpr std::array<std::uint8_t, 23> kFragmentSetupCacheRegs{{
    0x9D,0xD2,0x9C,0xD4,0xD5,0xD3,0x1E,0xE8,0xE9,0x9F,0x9E,0x23,
    0xDE,0xE7,0xDF,0x21,0xE0,0xE1,0xDB,0x22,0xC9,0xCA,0x00}};

FragmentSetup make_fragment_setup_cached(const std::array<std::uint32_t, 256> &commands) noexcept {
    struct Cache {
        std::array<std::uint32_t, kFragmentSetupCacheRegs.size()> values{};
        FragmentSetup setup{};
        bool valid{};
    };
    static thread_local Cache cache;
    std::array<std::uint32_t, kFragmentSetupCacheRegs.size()> values{};
    for (std::size_t i = 0; i + 1 < kFragmentSetupCacheRegs.size(); ++i)
        values[i] = commands[kFragmentSetupCacheRegs[i]];
    // Framebuffer/depth base helpers also consume the high-address words.
    values.back() = commands[0x9C] ^ (commands[0x9E] * 0x9E3779B9u);
    if (cache.valid && cache.values == values) return cache.setup;
    cache.values = values;
    cache.setup = make_fragment_setup(commands);
    cache.valid = true;
    return cache.setup;
}

std::uint8_t mul8(std::uint8_t a, std::uint8_t b) noexcept {
    return static_cast<std::uint8_t>((static_cast<std::uint32_t>(a) * b + 127u) / 255u);
}

Color apply_texture_function(Color vertex, Color texture, const FragmentSetup &setup) noexcept {
    const std::uint32_t function = setup.texture_function;
    const bool use_alpha = setup.texture_use_alpha;
    const bool double_color = setup.texture_double_color;
    Color out{};
    switch (function) {
    case 0u: // MODULATE
        out = {mul8(vertex.r, texture.r), mul8(vertex.g, texture.g), mul8(vertex.b, texture.b),
               use_alpha ? mul8(vertex.a, texture.a) : vertex.a};
        break;
    case 1u: { // DECAL
        const std::uint32_t alpha = use_alpha ? texture.a : 255u;
        auto decal = [alpha](std::uint8_t vc, std::uint8_t tc) {
            return static_cast<std::uint8_t>((static_cast<std::uint32_t>(tc) * alpha +
                                              static_cast<std::uint32_t>(vc) * (255u - alpha) + 127u) / 255u);
        };
        out = {decal(vertex.r, texture.r), decal(vertex.g, texture.g), decal(vertex.b, texture.b), vertex.a};
        break;
    }
    case 2u: { // BLEND
        const Color env = setup.texture_env;
        auto blend = [](std::uint8_t vc, std::uint8_t tc, std::uint8_t ec) {
            return static_cast<std::uint8_t>((static_cast<std::uint32_t>(vc) * (255u - tc) +
                                              static_cast<std::uint32_t>(ec) * tc + 127u) / 255u);
        };
        out = {blend(vertex.r, texture.r, env.r), blend(vertex.g, texture.g, env.g),
               blend(vertex.b, texture.b, env.b), use_alpha ? mul8(vertex.a, texture.a) : vertex.a};
        break;
    }
    case 3u: // REPLACE
        out = texture;
        if (!use_alpha) out.a = vertex.a;
        break;
    case 4u: // ADD
        out = {static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(vertex.r) + texture.r)),
               static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(vertex.g) + texture.g)),
               static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(vertex.b) + texture.b)),
               use_alpha ? mul8(vertex.a, texture.a) : vertex.a};
        break;
    default:
        out = texture;
        break;
    }
    if (double_color) {
        out.r = static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(out.r) * 2u));
        out.g = static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(out.g) * 2u));
        out.b = static_cast<std::uint8_t>(std::min(255u, static_cast<unsigned>(out.b) * 2u));
    }
    return out;
}

bool compare_value(std::uint32_t function, std::uint32_t left, std::uint32_t right) noexcept {
    switch (function & 7u) {
    case 0u: return false;
    case 1u: return true;
    case 2u: return left == right;
    case 3u: return left != right;
    case 4u: return left < right;
    case 5u: return left <= right;
    case 6u: return left > right;
    case 7u: return left >= right;
    }
    return true;
}

bool alpha_test(Color source, const FragmentSetup &setup) noexcept {
    if (!setup.alpha_test_enabled) return true;
    return compare_value(setup.alpha_function, source.a & setup.alpha_mask,
                         setup.alpha_reference & setup.alpha_mask);
}

std::array<float, 4> color_float(Color c) noexcept {
    return {c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f};
}

std::array<float, 4> blend_factor(std::uint32_t factor, bool source_factor,
                                  const std::array<float, 4> &src,
                                  const std::array<float, 4> &dst,
                                  Color fixed) noexcept {
    switch (factor) {
    case 0u: return source_factor ? std::array<float,4>{dst[0], dst[1], dst[2], dst[3]}
                                  : std::array<float,4>{src[0], src[1], src[2], src[3]};
    case 1u: return source_factor ? std::array<float,4>{1-dst[0],1-dst[1],1-dst[2],1-dst[3]}
                                  : std::array<float,4>{1-src[0],1-src[1],1-src[2],1-src[3]};
    case 2u: return {src[3], src[3], src[3], src[3]};
    case 3u: return {1-src[3],1-src[3],1-src[3],1-src[3]};
    case 4u: return {dst[3],dst[3],dst[3],dst[3]};
    case 5u: return {1-dst[3],1-dst[3],1-dst[3],1-dst[3]};
    case 6u: return {std::min(1.0f, 2*src[3]),std::min(1.0f, 2*src[3]),std::min(1.0f, 2*src[3]),std::min(1.0f, 2*src[3])};
    case 7u: return {std::min(1.0f, 2*(1-src[3])),std::min(1.0f, 2*(1-src[3])),std::min(1.0f, 2*(1-src[3])),std::min(1.0f, 2*(1-src[3]))};
    case 8u: return {std::min(1.0f, 2*dst[3]),std::min(1.0f, 2*dst[3]),std::min(1.0f, 2*dst[3]),std::min(1.0f, 2*dst[3])};
    case 9u: return {std::min(1.0f, 2*(1-dst[3])),std::min(1.0f, 2*(1-dst[3])),std::min(1.0f, 2*(1-dst[3])),std::min(1.0f, 2*(1-dst[3]))};
    case 10u: return color_float(fixed);
    default: return {1,1,1,1};
    }
}

Color blend_pixel(Color source, Color destination, const FragmentSetup &setup) noexcept {
    if (!setup.blend_enabled) return source;

    // VCS overwhelmingly uses the standard source-alpha blend.  The generic
    // path below builds six float arrays and dispatches two factor switches per
    // fragment.  For equation ADD, SRC_ALPHA, ONE_MINUS_SRC_ALPHA the exact
    // positive-domain float result rounds to the same byte as this integer
    // expression; denominator 255 is odd, so an exact .5 tie cannot occur.
    if (setup.blend_equation == 0u && setup.blend_source_factor == 2u &&
        setup.blend_dest_factor == 3u) {
        const std::uint32_t alpha = source.a;
        const std::uint32_t inverse = 255u - alpha;
        const auto channel = [alpha, inverse](std::uint8_t src, std::uint8_t dst) noexcept {
            return static_cast<std::uint8_t>((static_cast<std::uint32_t>(src) * alpha +
                                              static_cast<std::uint32_t>(dst) * inverse + 127u) / 255u);
        };
        return {channel(source.r, destination.r), channel(source.g, destination.g),
                channel(source.b, destination.b), channel(source.a, destination.a)};
    }
    const std::uint32_t equation = setup.blend_equation;
    const auto src = color_float(source);
    const auto dst = color_float(destination);
    const auto fa = blend_factor(setup.blend_source_factor, true, src, dst, setup.blend_fix_source);
    const auto fb = blend_factor(setup.blend_dest_factor, false, src, dst, setup.blend_fix_dest);
    std::array<float,4> out{};
    for (std::size_t i = 0; i < 4u; ++i) {
        const float a = src[i] * fa[i];
        const float b = dst[i] * fb[i];
        switch (equation) {
        case 0u: out[i] = a + b; break;
        case 1u: out[i] = a - b; break;
        case 2u: out[i] = b - a; break;
        case 3u: out[i] = std::min(src[i], dst[i]); break;
        case 4u: out[i] = std::max(src[i], dst[i]); break;
        case 5u: out[i] = std::fabs(src[i] - dst[i]); break;
        default: out[i] = a + b; break;
        }
    }
    auto to8 = [](float v) { return round_clamp_to_byte(v * 255.0f); };
    return {to8(out[0]),to8(out[1]),to8(out[2]),to8(out[3])};
}

bool depth_test_and_write(psprecomp::GuestMemory &memory, const FragmentSetup &setup,
                          std::int32_t x, std::int32_t y, std::uint16_t z,
                          bool clear_depth) {
    if (setup.depth_stride == 0u) return true;
    const std::size_t index =
        static_cast<std::size_t>(y) * setup.depth_stride + static_cast<std::size_t>(x);
    if (setup.depth_pixels != nullptr) {
        std::uint8_t *pixel = setup.depth_pixels + index * 2u;
        if (clear_depth) {
            pixel[0] = static_cast<std::uint8_t>(z);
            pixel[1] = static_cast<std::uint8_t>(z >> 8u);
            return true;
        }
        bool passed = true;
        if (setup.depth_test_enabled) {
            const std::uint16_t stored = static_cast<std::uint16_t>(
                static_cast<std::uint16_t>(pixel[0]) |
                (static_cast<std::uint16_t>(pixel[1]) << 8u));
            passed = compare_value(setup.depth_function, z, stored);
        }
        if (passed && setup.depth_write_enabled) {
            pixel[0] = static_cast<std::uint8_t>(z);
            pixel[1] = static_cast<std::uint8_t>(z >> 8u);
        }
        return passed;
    }

    const std::uint32_t address = setup.depth_base + static_cast<std::uint32_t>(index) * 2u;
    if (!memory.contains(address, 2u)) return true;
    if (clear_depth) {
        memory.aot_store16(address, z);
        return true;
    }
    bool passed = true;
    if (setup.depth_test_enabled) {
        passed = compare_value(setup.depth_function, z, memory.aot_load16(address));
    }
    if (passed && setup.depth_write_enabled) memory.aot_store16(address, z);
    return passed;
}

void rasterize_rectangle(psprecomp::GuestMemory &memory,
                         const std::array<std::uint32_t, 256> &commands,
                         const FragmentSetup &setup,
                         const Vertex &a, const Vertex &b, GeRenderStats &stats) {
    record_screen_vertex(stats, a);
    record_screen_vertex(stats, b);
    if (!setup.valid) return;
    // The triangle path has skipped GPU-owned surfaces since Stage 35; this one
    // never did, so every sceGuClear and every full-screen blit into the surface
    // the swapchain shows was still being filled on the CPU as well.
    if (software_raster_skipped(commands)) return;
    const std::uint32_t framebuffer_format = setup.framebuffer_format;
    const std::uint32_t framebuffer_stride = setup.framebuffer_stride;
    const std::uint32_t fb = setup.framebuffer_base;
    const std::uint32_t bpp = setup.framebuffer_bpp;

    const float min_x_f = std::min(a.x, b.x);
    const float max_x_f = std::max(a.x, b.x);
    const float min_y_f = std::min(a.y, b.y);
    const float max_y_f = std::max(a.y, b.y);
    std::int32_t x0 = static_cast<std::int32_t>(std::ceil(min_x_f));
    std::int32_t x1 = static_cast<std::int32_t>(std::ceil(max_x_f)) - 1;
    std::int32_t y0 = static_cast<std::int32_t>(std::ceil(min_y_f));
    std::int32_t y1 = static_cast<std::int32_t>(std::ceil(max_y_f)) - 1;

    x0 = std::max(x0, setup.scissor_x0); y0 = std::max(y0, setup.scissor_y0);
    x1 = std::min(x1, setup.scissor_x1); y1 = std::min(y1, setup.scissor_y1);
    if (x0 > x1 || y0 > y1) return;

    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    const bool clear_mode = setup.clear_mode;
    const bool clear_color = setup.clear_color;
    const bool clear_alpha = setup.clear_alpha;
    const bool clear_depth = setup.clear_depth;
    const bool shaded_texture = setup.texture_enabled && !clear_mode;
    const std::uint32_t write_mask = setup.write_mask;
    const std::int32_t rectangle_width = x1 - x0 + 1;
    const std::int64_t covered = static_cast<std::int64_t>(rectangle_width) *
                                 static_cast<std::int64_t>(y1 - y0 + 1);
    const bool phase_diag = ge_phase_diag_enabled();
    RowWorkerPool &pool = RowWorkerPool::instance();

    // PSP games commonly clear the entire color/depth surface with one GE
    // rectangle. The generic fragment path was reading, unpacking, blending and
    // repacking every pixel even when all channels and a constant Z were being
    // overwritten. Bulk row fills preserve the exact packed result and turn a
    // 130,560-fragment clear into a handful of vectorized memory operations.
    const bool constant_depth = a.z == b.z;
    const bool full_color_clear = clear_color &&
        (clear_alpha || framebuffer_format == 0u);
    const bool no_color_clear = !clear_color && !clear_alpha;
    const bool fast_depth_clear = setup.depth_stride != 0u && clear_depth &&
        constant_depth && setup.depth_pixels != nullptr;
    const bool no_depth_effect = setup.depth_stride == 0u || fast_depth_clear ||
        (!clear_depth && !setup.depth_test_enabled && !setup.depth_write_enabled);
    const bool fast_clear = clear_mode && no_depth_effect &&
        (no_color_clear || (full_color_clear && setup.color_pixels != nullptr));
    if (fast_clear) {
        const std::uint16_t depth_value = static_cast<std::uint16_t>(
            std::clamp(static_cast<float>(a.z), 0.0f, 65535.0f));
        const std::uint32_t packed32 = pack32(b.color);
        const std::uint16_t packed16 = pack16(b.color, framebuffer_format);
        const auto clear_rows = [&](std::int32_t row_first, std::int32_t row_last,
                                    GeRenderStats &row_stats) {
            for (std::int32_t y = row_first; y <= row_last; ++y) {
                if (full_color_clear) {
                    std::uint8_t *row = setup.color_pixels +
                        (static_cast<std::size_t>(y) * framebuffer_stride +
                         static_cast<std::size_t>(x0)) * bpp;
                    if (framebuffer_format == 3u) {
                        std::fill_n(reinterpret_cast<std::uint32_t *>(row),
                                    static_cast<std::size_t>(rectangle_width), packed32);
                    } else {
                        std::fill_n(reinterpret_cast<std::uint16_t *>(row),
                                    static_cast<std::size_t>(rectangle_width), packed16);
                    }
                }
                if (fast_depth_clear) {
                    std::uint8_t *depth_row = setup.depth_pixels +
                        (static_cast<std::size_t>(y) * setup.depth_stride +
                         static_cast<std::size_t>(x0)) * 2u;
                    std::fill_n(reinterpret_cast<std::uint16_t *>(depth_row),
                                static_cast<std::size_t>(rectangle_width), depth_value);
                }
                row_stats.pixels_tested += static_cast<std::uint64_t>(rectangle_width);
                row_stats.pixels_written += static_cast<std::uint64_t>(rectangle_width);
            }
        };
        const auto invoke_clear = [&] {
            if (covered < parallel_pixel_threshold() || pool.worker_count() <= 1u) {
                clear_rows(y0, y1, stats);
                return;
            }
            const unsigned slots = pool.worker_count();
            std::array<GeRenderStats, RowWorkerPool::kMaxThreads> partial{};
            pool.run(y0, y1, [&](unsigned index, std::int32_t first, std::int32_t last) {
                clear_rows(first, last, partial[index]);
            });
            for (unsigned index = 0u; index < slots; ++index) {
                stats.pixels_tested += partial[index].pixels_tested;
                stats.pixels_written += partial[index].pixels_written;
            }
        };
        if (phase_diag) {
            PixelLoopTimer timer;
            invoke_clear();
        } else {
            invoke_clear();
        }
        return;
    }

    // Rectangles carry the full-screen work in this game: a heavy frame spends
    // about 14 ms here with no triangle drawn at all, because a 480x272 blit is
    // 130000 fragments and this loop was the one raster path still running on a
    // single thread.  Rows are independent exactly as they are for triangles --
    // every covered pixel is written once -- so splitting them is order-free.
    const auto rasterize_rect_rows = [&](std::int32_t row_first, std::int32_t row_last,
                                         GeRenderStats &row_stats) {
    for (std::int32_t y = row_first; y <= row_last; ++y) {
        const float ty = dy == 0.0f ? 0.0f : ((static_cast<float>(y) + 0.5f - a.y) / dy);
        for (std::int32_t x = x0; x <= x1; ++x) {
            ++row_stats.pixels_tested;
            const float tx = dx == 0.0f ? 0.0f : ((static_cast<float>(x) + 0.5f - a.x) / dx);
            const float interpolation = std::clamp((tx + ty) * 0.5f, 0.0f, 1.0f);
            Color source = b.color;
            if (shaded_texture) {
                const float q = a.q + (b.q - a.q) * interpolation;
                if (!finite_float(q) || std::fabs(q) < 1.0e-20f) continue;
                const float u = (a.u + (b.u - a.u) * tx) / q;
                const float v = (a.v + (b.v - a.v) * ty) / q;
                source = apply_texture_function(source, sample_texture(memory, setup.texture, u, v), setup);
            }
            if (!clear_mode && !alpha_test(source, setup)) continue;
            const float zf = static_cast<float>(a.z) + (static_cast<float>(b.z) - a.z) * ((tx + ty) * 0.5f);
            const std::uint16_t z = static_cast<std::uint16_t>(std::clamp(zf, 0.0f, 65535.0f));
            if (!depth_test_and_write(memory, setup, x, y, z, clear_mode && clear_depth)) continue;

            const std::size_t pixel_index =
                static_cast<std::size_t>(y) * framebuffer_stride + static_cast<std::size_t>(x);
            const std::uint32_t pixel_address = fb + static_cast<std::uint32_t>(pixel_index * bpp);
            std::uint8_t *pixel = setup.color_pixels != nullptr
                ? setup.color_pixels + pixel_index * bpp : nullptr;
            if (pixel == nullptr && !memory.contains(pixel_address, bpp)) continue;
            Color destination = pixel != nullptr ? read_color_raw(pixel, framebuffer_format)
                                                 : read_color(memory, pixel_address, framebuffer_format);
            if (clear_mode) {
                if (!clear_color) { source.r = destination.r; source.g = destination.g; source.b = destination.b; }
                if (!clear_alpha) source.a = destination.a;
            } else {
                source = blend_pixel(source, destination, setup);
            }
            if (pixel != nullptr) write_color_raw(pixel, framebuffer_format, source, clear_mode ? 0u : write_mask);
            else write_color(memory, pixel_address, framebuffer_format, source, clear_mode ? 0u : write_mask);
            ++row_stats.pixels_written;
        }
    }
    };

    if (covered < parallel_pixel_threshold() || pool.worker_count() <= 1u) {
        if (phase_diag) {
            PixelLoopTimer timer;
            rasterize_rect_rows(y0, y1, stats);
        } else {
            rasterize_rect_rows(y0, y1, stats);
        }
        return;
    }

    const unsigned slots = pool.worker_count();
    std::array<GeRenderStats, RowWorkerPool::kMaxThreads> partial{};
    const auto run_parallel = [&] {
        pool.run(y0, y1, [&](unsigned index, std::int32_t row_first, std::int32_t row_last) {
            rasterize_rect_rows(row_first, row_last, partial[index]);
        });
    };
    if (phase_diag) {
        PixelLoopTimer timer;
        run_parallel();
    } else {
        run_parallel();
    }
    for (unsigned index = 0u; index < slots; ++index) {
        const GeRenderStats &item = partial[index];
        stats.pixels_tested += item.pixels_tested;
        stats.pixels_written += item.pixels_written;
    }
}


float clip_distance(const Vertex &vertex, std::uint32_t plane) noexcept {
    switch (plane) {
    case 0u: return vertex.x + vertex.w;
    case 1u: return vertex.w - vertex.x;
    case 2u: return vertex.y + vertex.w;
    case 3u: return vertex.w - vertex.y;
    case 4u: return vertex.z + vertex.w;
    default: return vertex.w - vertex.z;
    }
}

Vertex interpolate_vertex(const Vertex &a, const Vertex &b, float t) noexcept {
    Vertex out{};
    auto lerp = [t](float x, float y) { return x + (y - x) * t; };
    out.u = lerp(a.u, b.u);
    out.v = lerp(a.v, b.v);
    out.q = lerp(a.q, b.q);
    out.x = lerp(a.x, b.x);
    out.y = lerp(a.y, b.y);
    out.z = lerp(a.z, b.z);
    out.w = lerp(a.w, b.w);
    out.fog_factor = lerp(a.fog_factor, b.fog_factor);
    out.color = lerp_color(a.color, b.color, t);
    return out;
}

// Sutherland-Hodgman against a plane grows a convex polygon by at most one
// vertex, so a triangle clipped against all six planes never exceeds nine.
// The polygon therefore fits in a fixed buffer.  This used to allocate one
// std::vector per clip plane, and at tens of thousands of triangles per frame
// the allocator alone was a visible share of the frame.
struct ClipPolygon {
    std::array<Vertex, 16> vertices;
    std::size_t size{};
};

bool vertex_inside_all_planes(const Vertex &vertex, std::uint32_t plane_count) noexcept {
    for (std::uint32_t plane = 0u; plane < plane_count; ++plane)
        if (clip_distance(vertex, plane) < 0.0f) return false;
    return true;
}

void clip_triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                   bool depth_clip_enabled, ClipPolygon &result) {
    const std::uint32_t plane_count = depth_clip_enabled ? 6u : 4u;

    // Trivial accept.  Nearly every triangle in a VCS frame lies completely
    // inside the frustum, and for those the Sutherland-Hodgman loop below
    // reproduces a, b, c in the original order: no edge crosses a plane, so no
    // vertex is dropped and none is interpolated.  Returning them directly is
    // therefore bit-identical, and it skips default-initializing the two
    // 16-vertex working polygons -- 1.4 KB of stores per triangle, paid roughly
    // 25000 times per frame before any clipping work happened.  A NaN
    // coordinate fails the >= 0 test and falls through to the general path,
    // exactly as it did before.
    if (vertex_inside_all_planes(a, plane_count) &&
        vertex_inside_all_planes(b, plane_count) &&
        vertex_inside_all_planes(c, plane_count)) {
        result.vertices[0] = a;
        result.vertices[1] = b;
        result.vertices[2] = c;
        result.size = 3u;
        return;
    }

    // Scratch for the clipped minority.  The GE list runs on one thread; the
    // raster worker pool never reaches this function, and thread_local keeps
    // that true by construction.
    static thread_local ClipPolygon buffers[2];
    ClipPolygon *polygon = &buffers[0];
    ClipPolygon *output = &buffers[1];
    polygon->vertices[0] = a;
    polygon->vertices[1] = b;
    polygon->vertices[2] = c;
    polygon->size = 3u;

    for (std::uint32_t plane = 0u; plane < plane_count && polygon->size != 0u; ++plane) {
        output->size = 0u;
        Vertex previous = polygon->vertices[polygon->size - 1u];
        float previous_distance = clip_distance(previous, plane);
        bool previous_inside = previous_distance >= 0.0f;
        for (std::size_t index = 0u; index < polygon->size; ++index) {
            const Vertex &current = polygon->vertices[index];
            const float current_distance = clip_distance(current, plane);
            const bool current_inside = current_distance >= 0.0f;
            if (current_inside != previous_inside) {
                const float denominator = previous_distance - current_distance;
                const float t = std::fabs(denominator) < 1.0e-20f ? 0.0f : previous_distance / denominator;
                output->vertices[output->size++] =
                    interpolate_vertex(previous, current, std::clamp(t, 0.0f, 1.0f));
            }
            if (current_inside) output->vertices[output->size++] = current;
            previous = current;
            previous_distance = current_distance;
            previous_inside = current_inside;
        }
        std::swap(polygon, output);
    }
    result.size = polygon->size;
    for (std::size_t index = 0u; index < polygon->size; ++index)
        result.vertices[index] = polygon->vertices[index];
}

bool viewport_transform(Vertex &vertex, const std::array<std::uint32_t, 256> &commands) noexcept {
    if (!std::isfinite(vertex.w) || std::fabs(vertex.w) < 1.0e-12f) return false;
    const float inv_w = 1.0f / vertex.w;
    const float scale_x = decode_float24(data24(commands[0x42u]));
    const float scale_y = decode_float24(data24(commands[0x43u]));
    const float scale_z = decode_float24(data24(commands[0x44u]));
    const float center_x = decode_float24(data24(commands[0x45u]));
    const float center_y = decode_float24(data24(commands[0x46u]));
    const float center_z = decode_float24(data24(commands[0x47u]));
    // GE raster offsets are unsigned 12.4 fixed point. Interpreting bit 15
    // as a sign bit moves legitimate PSP viewports by 4096 pixels.
    const float offset_x = static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f;
    const float offset_y = static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f;
    vertex.x = vertex.x * inv_w * scale_x + center_x - offset_x;
    vertex.y = vertex.y * inv_w * scale_y + center_y - offset_y;
    vertex.z = vertex.z * inv_w * scale_z + center_z;
    vertex.inv_w = inv_w;
    return std::isfinite(vertex.x) && std::isfinite(vertex.y) && std::isfinite(vertex.z);
}

bool point_inside_clip(const Vertex &vertex, bool depth_clip_enabled) noexcept {
    if (!std::isfinite(vertex.w) || vertex.w <= 0.0f) return false;
    const std::uint32_t plane_count = depth_clip_enabled ? 6u : 4u;
    for (std::uint32_t plane = 0u; plane < plane_count; ++plane)
        if (clip_distance(vertex, plane) < 0.0f) return false;
    return true;
}

// Whether the depth test may run before the fragment is shaded.
//
// Only the alpha test sits between the two, and it can reject a fragment solely
// on the alpha the texture produced.  When it is disabled -- or when clear mode
// bypasses it entirely -- nothing between them can discard the fragment, the
// depth test becomes the last thing that can, and running it first writes the
// depth buffer on precisely the same fragments, in the same order.
bool depth_precedes_shading(const FragmentSetup &setup) noexcept {
    return setup.clear_mode || !setup.alpha_test_enabled;
}

// The bounds/scissor rejection and the depth test of write_fragment, split out
// so the pixel loop can discard an occluded fragment before paying for the
// perspective division, the interpolated colour and the texture fetch.  This
// must stay byte-for-byte in step with the head of write_fragment below.
bool fragment_depth_prepass(psprecomp::GuestMemory &memory, const FragmentSetup &setup,
                            std::int32_t x, std::int32_t y, float zf, GeRenderStats &stats) {
    if (!setup.valid || x < 0 || y < 0 ||
        x >= static_cast<std::int32_t>(setup.framebuffer_stride)) {
        return false;
    }
    if (x < setup.scissor_x0 || x > setup.scissor_x1 ||
        y < setup.scissor_y0 || y > setup.scissor_y1) {
        return false;
    }
    ++stats.pixels_tested;
    const std::uint16_t z = static_cast<std::uint16_t>(std::clamp(zf, 0.0f, 65535.0f));
    return depth_test_and_write(memory, setup, x, y, z, setup.clear_mode && setup.clear_depth);
}

// `depth_resolved` says the caller already ran fragment_depth_prepass for this
// fragment: the bounds check, the tested counter and the depth test are then
// skipped here rather than repeated.
bool write_fragment(psprecomp::GuestMemory &memory,
                    const std::array<std::uint32_t, 256> &commands,
                    const FragmentSetup &setup,
                    std::int32_t x, std::int32_t y, float zf, float u, float v,
                    Color source, GeRenderStats &stats, bool depth_resolved = false) {
    if (!depth_resolved) {
        if (!setup.valid || x < 0 || y < 0 ||
            x >= static_cast<std::int32_t>(setup.framebuffer_stride)) {
            return false;
        }
        if (x < setup.scissor_x0 || x > setup.scissor_x1 ||
            y < setup.scissor_y0 || y > setup.scissor_y1) {
            return false;
        }
        ++stats.pixels_tested;
    }
    const std::uint16_t z = static_cast<std::uint16_t>(std::clamp(zf, 0.0f, 65535.0f));

    // Texture sampling is by far the most expensive step here -- with bilinear
    // filtering it decodes four texels per fragment -- and in a city scene most
    // fragments are occluded and end up discarded by the depth test anyway.
    const bool depth_before_shading = depth_resolved || depth_precedes_shading(setup);
    if (!depth_resolved && depth_before_shading &&
        !depth_test_and_write(memory, setup, x, y, z, setup.clear_mode && setup.clear_depth))
        return false;

    if (setup.texture_enabled && !setup.clear_mode)
        source = apply_texture_function(source, sample_texture(memory, setup.texture, u, v), setup);
    if (!setup.clear_mode && !alpha_test(source, setup)) return false;
    if (!depth_before_shading &&
        !depth_test_and_write(memory, setup, x, y, z, setup.clear_mode && setup.clear_depth))
        return false;

    const std::size_t pixel_index =
        static_cast<std::size_t>(y) * setup.framebuffer_stride + static_cast<std::size_t>(x);
    std::uint8_t *const pixel = setup.color_pixels != nullptr
        ? setup.color_pixels + pixel_index * setup.framebuffer_bpp
        : nullptr;
    const std::uint32_t pixel_address =
        setup.framebuffer_base + static_cast<std::uint32_t>(pixel_index) * setup.framebuffer_bpp;
    if (pixel == nullptr && !memory.contains(pixel_address, setup.framebuffer_bpp)) return false;

    Color destination = pixel != nullptr
        ? read_color_raw(pixel, setup.framebuffer_format)
        : read_color(memory, pixel_address, setup.framebuffer_format);
    if (setup.clear_mode) {
        if (!setup.clear_color) { source.r = destination.r; source.g = destination.g; source.b = destination.b; }
        if (!setup.clear_alpha) source.a = destination.a;
    } else {
        source = blend_pixel(source, destination, setup);
    }
    const std::uint32_t mask = setup.clear_mode ? 0u : setup.write_mask;
    if (pixel != nullptr) write_color_raw(pixel, setup.framebuffer_format, source, mask);
    else write_color(memory, pixel_address, setup.framebuffer_format, source, mask);
    ++stats.pixels_written;
    return true;
}

float edge_function(const Vertex &a, const Vertex &b, float x, float y) noexcept {
    return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}

Color perspective_color(const Vertex &a, const Vertex &b, const Vertex &c,
                        float l0, float l1, float l2, float denominator) noexcept {
    auto numerator = [&](std::uint8_t ca, std::uint8_t cb, std::uint8_t cc) {
        return l0 * static_cast<float>(ca) * a.inv_w +
               l1 * static_cast<float>(cb) * b.inv_w +
               l2 * static_cast<float>(cc) * c.inv_w;
    };
    float values[4]{};
    divide4_same_denominator(numerator(a.color.r, b.color.r, c.color.r),
                             numerator(a.color.g, b.color.g, c.color.g),
                             numerator(a.color.b, b.color.b, c.color.b),
                             numerator(a.color.a, b.color.a, c.color.a),
                             denominator, values);
    return {round_clamp_to_byte(values[0]), round_clamp_to_byte(values[1]),
            round_clamp_to_byte(values[2]), round_clamp_to_byte(values[3])};
}

struct PreparedScreenTriangle {
    Vertex a{};
    Vertex b{};
    Vertex c{};
    Color provoking_color{};
    float area{};
    std::int32_t min_x{};
    std::int32_t max_x{};
    std::int32_t min_y{};
    std::int32_t max_y{};
    bool texture_enabled{};
    bool early_depth{};
    bool flat_shading{};
#if PSPRECOMP_GE_X86_SIMD
    __m128 edge_ax{};
    __m128 edge_ay{};
    __m128 edge_dx{};
    __m128 edge_dy{};
#endif
};

std::uint32_t pack_gpu_color(Color color) noexcept {
    return static_cast<std::uint32_t>(color.r) |
           (static_cast<std::uint32_t>(color.g) << 8u) |
           (static_cast<std::uint32_t>(color.b) << 16u) |
           (static_cast<std::uint32_t>(color.a) << 24u);
}

std::uint32_t pack_gpu_alpha_control(const GeGpuDrawDescriptor &draw) noexcept {
    return static_cast<std::uint32_t>(draw.alpha_test_enabled ? 1u : 0u) |
           ((draw.alpha_function & 7u) << 8u) |
           ((draw.alpha_reference & 0xFFu) << 16u) |
           ((draw.alpha_mask & 0xFFu) << 24u);
}

std::uint32_t pack_gpu_fog_control(const GeGpuDrawDescriptor &draw) noexcept {
    return (draw.fog_color & 0x00FFFFFFu) |
           (static_cast<std::uint32_t>(draw.fog_enabled ? 0xFFu : 0u) << 24u);
}

Color gpu_draw_debug_color(const GeGpuDrawDescriptor &draw) noexcept {
    std::uint32_t hash = draw.texture_address ^ (draw.texture_address >> 11u) ^
                         (draw.texture_buffer_width * 0x9E3779B9u) ^
                         (draw.texture_format * 0x85EBCA6Bu) ^
                         (draw.primitive * 0xC2B2AE35u) ^ draw.vertex_count;
    hash ^= hash >> 16u;
    hash *= 0x7FEB352Du;
    hash ^= hash >> 15u;
    return {
        static_cast<std::uint8_t>(48u + (hash & 0xCFu)),
        static_cast<std::uint8_t>(48u + ((hash >> 8u) & 0xCFu)),
        static_cast<std::uint8_t>(48u + ((hash >> 16u) & 0xCFu)),
        255u,
    };
}

bool gpu_geometry_debug_colors_enabled() noexcept {
    static const bool enabled = [] {
        const char *value = std::getenv("PSPRECOMP_GE_GPU_GEOMETRY_DEBUG_COLORS");
        return value != nullptr && *value != '\0' && std::strcmp(value, "0") != 0;
    }();
    return enabled;
}


bool gpu_force_white_vertex_colors_enabled() noexcept {
    static const bool enabled = [] {
        const char *value = std::getenv("PSPRECOMP_GE_GPU_FORCE_WHITE_VERTEX");
        return value != nullptr && *value != '\0' && std::strcmp(value, "0") != 0;
    }();
    return enabled;
}

Color gpu_texture_debug_color(const GeGpuDrawDescriptor &draw, Color lighting) noexcept {
    // Stage 24 has not uploaded PSP textures yet.  Rendering their vertices as
    // black would hide the fact that the real game geometry is being rasterized
    // at the configured internal resolution, because many REPLACE-mode draws
    // deliberately carry black/unused vertex colors.  Until descriptor-backed
    // texture sampling lands, use a stable color derived from the actual guest
    // texture address.  MODULATE-like paths retain the vertex lighting.
    std::uint32_t hash = draw.texture_address ^ (draw.texture_address >> 11u) ^
                         (draw.texture_buffer_width * 0x9E3779B9u) ^
                         (draw.texture_format * 0x85EBCA6Bu);
    hash ^= hash >> 16u;
    hash *= 0x7FEB352Du;
    hash ^= hash >> 15u;
    const Color texture_color{
        static_cast<std::uint8_t>(72u + (hash & 0xB7u)),
        static_cast<std::uint8_t>(72u + ((hash >> 8u) & 0xB7u)),
        static_cast<std::uint8_t>(72u + ((hash >> 16u) & 0xB7u)),
        255u,
    };
    // PSP GU_TFX_REPLACE is 3.  In that mode the vertex RGB is intentionally
    // irrelevant, so return the diagnostic texture color directly.
    if (draw.texture_function == 3u ||
        (lighting.r <= 4u && lighting.g <= 4u && lighting.b <= 4u))
        return texture_color;
    auto modulate = [](std::uint8_t a, std::uint8_t b) noexcept {
        return static_cast<std::uint8_t>((static_cast<unsigned>(a) * b + 127u) / 255u);
    };
    return {modulate(texture_color.r, lighting.r),
            modulate(texture_color.g, lighting.g),
            modulate(texture_color.b, lighting.b), 255u};
}

GeGpuDrawDescriptor gpu_effective_draw_descriptor(GeGpuDrawDescriptor draw) noexcept {
    if (!draw.clear_mode) return draw;
    draw.texture_enabled = false;
    draw.blend_enabled = false;
    draw.alpha_test_enabled = false;
    draw.fog_enabled = false;
    draw.depth_test_enabled = false;
    draw.depth_write_enabled = draw.clear_depth;
    // PSP write-mask bytes use 0 for writable and 0xFF for fully masked.
    draw.color_write_mask =
        (draw.clear_color ? 0x00000000u : 0x00FFFFFFu) |
        (draw.clear_alpha ? 0x00000000u : 0xFF000000u);
    return draw;
}

void accumulate_gpu_prepared_triangles(
    const GeGpuDrawDescriptor &draw,
    const std::vector<PreparedScreenTriangle> &triangles) {
    if (!ge_gpu_backend_graphics_ready() || triangles.empty()) return;
    const GeGpuDrawDescriptor effective_draw = gpu_effective_draw_descriptor(draw);
    const bool sampled_texture_ready =
        effective_draw.texture_enabled && ge_gpu_backend_texture_available(effective_draw);
    // Reused: one allocation per draw call, about 1200 per heavy vblank, for a
    // buffer that is handed to the backend and immediately discarded.
    static thread_local std::vector<GeGpuVertex> vertices;
    try {
        vertices.clear();
        vertices.reserve(triangles.size() * 3u);
        for (const PreparedScreenTriangle &triangle : triangles) {
            Color ca = triangle.flat_shading ? triangle.provoking_color : triangle.a.color;
            Color cb = triangle.flat_shading ? triangle.provoking_color : triangle.b.color;
            Color cc = triangle.flat_shading ? triangle.provoking_color : triangle.c.color;
            if (gpu_force_white_vertex_colors_enabled()) {
                ca = cb = cc = Color{255u, 255u, 255u, 255u};
            } else if (gpu_geometry_debug_colors_enabled()) {
                ca = cb = cc = gpu_draw_debug_color(effective_draw);
            } else if (effective_draw.texture_enabled) {
                if (sampled_texture_ready) {
                    // Stage 31 implements the complete texture-function switch
                    // in the fragment shader; preserve the real interpolated
                    // vertex color for every function, including REPLACE.
                } else {
                    ca = gpu_texture_debug_color(effective_draw, ca);
                    cb = gpu_texture_debug_color(effective_draw, cb);
                    cc = gpu_texture_debug_color(effective_draw, cc);
                }
            }
            vertices.push_back({triangle.a.x, triangle.a.y, triangle.a.z, triangle.a.w,
                                pack_gpu_color(ca), triangle.a.u, triangle.a.v,
                                pack_gpu_alpha_control(effective_draw), 0u, effective_draw.texture_env,
                                triangle.a.fog_factor, pack_gpu_fog_control(effective_draw), triangle.a.q});
            vertices.push_back({triangle.b.x, triangle.b.y, triangle.b.z, triangle.b.w,
                                pack_gpu_color(cb), triangle.b.u, triangle.b.v,
                                pack_gpu_alpha_control(effective_draw), 0u, effective_draw.texture_env,
                                triangle.b.fog_factor, pack_gpu_fog_control(effective_draw), triangle.b.q});
            vertices.push_back({triangle.c.x, triangle.c.y, triangle.c.z, triangle.c.w,
                                pack_gpu_color(cc), triangle.c.u, triangle.c.v,
                                pack_gpu_alpha_control(effective_draw), 0u, effective_draw.texture_env,
                                triangle.c.fog_factor, pack_gpu_fog_control(effective_draw), triangle.c.q});
        }
        ge_gpu_backend_accumulate_color_triangles(effective_draw, vertices);
    } catch (...) {
        // The reference software path must remain authoritative even if the
        // experimental high-resolution preview runs out of host memory.
    }
}


void accumulate_gpu_rectangle(const GeGpuDrawDescriptor &draw,
                              const Vertex &a, const Vertex &b) {
    if (!ge_gpu_backend_graphics_ready()) return;
    const GeGpuDrawDescriptor effective_draw = gpu_effective_draw_descriptor(draw);
    const bool sampled_texture_ready = effective_draw.texture_enabled &&
        ge_gpu_backend_texture_available(effective_draw);

    Color color = b.color;
    if (!effective_draw.clear_mode && gpu_force_white_vertex_colors_enabled()) {
        color = Color{255u, 255u, 255u, 255u};
    } else if (!effective_draw.clear_mode && gpu_geometry_debug_colors_enabled()) {
        color = gpu_draw_debug_color(effective_draw);
    } else if (effective_draw.texture_enabled && !sampled_texture_ready) {
        color = gpu_texture_debug_color(effective_draw, color);
    }

    // PSP rectangle primitives use the second endpoint color for every fragment.
    // U varies only along X, V only along Y, while depth/Q lie on the diagonal
    // plane between the two endpoints.  Two triangles with midpoint diagonal
    // values preserve that plane and keep draw order with the 3D batches.
    const float mid_z = (a.z + b.z) * 0.5f;
    const float mid_q = (a.q + b.q) * 0.5f;
    const std::uint32_t packed = pack_gpu_color(color);
    const std::uint32_t alpha = pack_gpu_alpha_control(effective_draw);
    const std::uint32_t fog = pack_gpu_fog_control(effective_draw);
    const auto make = [&](float x, float y, float z, float u, float v,
                          float q, float fog_factor) {
        if (!std::isfinite(q) || std::fabs(q) < 1.0e-20f) q = 1.0f;
        return GeGpuVertex{x, y, z, 1.0f, packed, u, v,
                           alpha, 0u, effective_draw.texture_env, fog_factor, fog, q};
    };

    const GeGpuVertex p00 = make(a.x, a.y, a.z, a.u, a.v, a.q, a.fog_factor);
    const GeGpuVertex p10 = make(b.x, a.y, mid_z, b.u, a.v, mid_q,
                                  (a.fog_factor + b.fog_factor) * 0.5f);
    const GeGpuVertex p11 = make(b.x, b.y, b.z, b.u, b.v, b.q, b.fog_factor);
    const GeGpuVertex p01 = make(a.x, b.y, mid_z, a.u, b.v, mid_q,
                                  (a.fog_factor + b.fog_factor) * 0.5f);
    const std::array<GeGpuVertex, 6> triangles{{p00, p10, p11, p00, p11, p01}};
    ge_gpu_backend_accumulate_color_triangles(effective_draw, triangles);
}

bool prepare_screen_triangle(const std::array<std::uint32_t, 256> &commands,
                             const FragmentSetup &setup,
                             const Vertex &a, const Vertex &b, const Vertex &c,
                             Color provoking_color, GeRenderStats &stats,
                             PreparedScreenTriangle &prepared) {
    record_screen_vertex(stats, a);
    record_screen_vertex(stats, b);
    record_screen_vertex(stats, c);
    const float area = edge_function(a, b, c.x, c.y);
    if (!std::isfinite(area) || std::fabs(area) < 1.0e-8f) return false;

    const bool clear_mode = (data24(commands[0xD3u]) & 1u) != 0u;
    if (!clear_mode && (data24(commands[0x1Du]) & 1u) != 0u) {
        const bool counter_clockwise = area < 0.0f;
        const bool accept_counter_clockwise = (data24(commands[0x9Bu]) & 1u) != 0u;
        if (counter_clockwise != accept_counter_clockwise) {
            if (g_collect_ge_render_stats) ++stats.culled_triangles;
            return false;
        }
    }
    const bool flat_shading = !clear_mode && (data24(commands[0x50u]) & 1u) == 0u;
    if (flat_shading && g_collect_ge_render_stats) ++stats.flat_shaded_primitives;

    if (!setup.valid) return false;
    const std::int32_t min_x = std::max(setup.scissor_x0,
        static_cast<std::int32_t>(std::floor(std::min(a.x, std::min(b.x, c.x)))));
    const std::int32_t max_x = std::min(setup.scissor_x1,
        static_cast<std::int32_t>(std::ceil(std::max(a.x, std::max(b.x, c.x)))));
    const std::int32_t min_y = std::max(setup.scissor_y0,
        static_cast<std::int32_t>(std::floor(std::min(a.y, std::min(b.y, c.y)))));
    const std::int32_t max_y = std::min(setup.scissor_y1,
        static_cast<std::int32_t>(std::ceil(std::max(a.y, std::max(b.y, c.y)))));
    if (min_x > max_x || min_y > max_y) return false;

    prepared.a = a;
    prepared.b = b;
    prepared.c = c;
    prepared.provoking_color = provoking_color;
    prepared.area = area;
    prepared.min_x = min_x;
    prepared.max_x = max_x;
    prepared.min_y = min_y;
    prepared.max_y = max_y;
    prepared.texture_enabled = setup.texture_enabled && !clear_mode;
    prepared.early_depth = depth_precedes_shading(setup);
    prepared.flat_shading = flat_shading;
#if PSPRECOMP_GE_X86_SIMD
    prepared.edge_ax = _mm_set_ps(0.0f, a.x, c.x, b.x);
    prepared.edge_ay = _mm_set_ps(0.0f, a.y, c.y, b.y);
    prepared.edge_dx = _mm_set_ps(0.0f, b.x - a.x, a.x - c.x, c.x - b.x);
    prepared.edge_dy = _mm_set_ps(0.0f, b.y - a.y, a.y - c.y, c.y - b.y);
#endif
    return true;
}

bool prepare_gpu_only_screen_triangle(const std::array<std::uint32_t, 256> &commands,
                                      const FragmentSetup &setup,
                                      const Vertex &a, const Vertex &b, const Vertex &c,
                                      Color provoking_color, GeRenderStats &stats,
                                      PreparedScreenTriangle &prepared) {
    // Vulkan still needs viewport transform and PSP culling, but when the CPU
    // raster is skipped it does not need pixel bounds, edge SIMD coefficients,
    // early-depth metadata or any other software-fragment setup.
    record_screen_vertex(stats, a);
    record_screen_vertex(stats, b);
    record_screen_vertex(stats, c);
    const float area = edge_function(a, b, c.x, c.y);
    if (!std::isfinite(area) || std::fabs(area) < 1.0e-8f || !setup.valid) return false;
    const bool clear_mode = (data24(commands[0xD3u]) & 1u) != 0u;
    if (!clear_mode && (data24(commands[0x1Du]) & 1u) != 0u) {
        const bool counter_clockwise = area < 0.0f;
        const bool accept_counter_clockwise = (data24(commands[0x9Bu]) & 1u) != 0u;
        if (counter_clockwise != accept_counter_clockwise) {
            if (g_collect_ge_render_stats) ++stats.culled_triangles;
            return false;
        }
    }
    const bool flat_shading = !clear_mode && (data24(commands[0x50u]) & 1u) == 0u;
    if (flat_shading && g_collect_ge_render_stats) ++stats.flat_shaded_primitives;
    prepared.a = a;
    prepared.b = b;
    prepared.c = c;
    prepared.provoking_color = provoking_color;
    prepared.area = area;
    prepared.flat_shading = flat_shading;
    return true;
}

void append_prepared_triangles(const std::array<std::uint32_t, 256> &commands,
                               const FragmentSetup &setup,
                               const Vertex &a, const Vertex &b, const Vertex &c,
                               const Vertex &provoking, bool through,
                               GeRenderStats &stats,
                               std::vector<PreparedScreenTriangle> &prepared,
                               bool gpu_only = false) {
    // Built in place: a PreparedScreenTriangle is about 260 bytes, so the old
    // "value-initialize a local, then copy it into the vector" cost three full
    // passes over it per triangle kept.
    const auto emplace_prepared = [&](const Vertex &va, const Vertex &vb, const Vertex &vc) {
        prepared.emplace_back();
        const bool kept = gpu_only
            ? prepare_gpu_only_screen_triangle(commands, setup, va, vb, vc,
                                               provoking.color, stats, prepared.back())
            : prepare_screen_triangle(commands, setup, va, vb, vc, provoking.color,
                                      stats, prepared.back());
        if (!kept) prepared.pop_back();
    };

    if (through) {
        emplace_prepared(a, b, c);
        return;
    }

    const bool depth_clip_enabled = (data24(commands[0x1Cu]) & 1u) != 0u;
    // Same reason as the scratch inside clip_triangle: a plain local would
    // default-initialize 16 vertices on every triangle just to have the first
    // three overwritten.
    static thread_local ClipPolygon polygon;
    clip_triangle(a, b, c, depth_clip_enabled, polygon);
    if (polygon.size < 3u) return;
    for (std::size_t index = 0u; index < polygon.size; ++index)
        if (!viewport_transform(polygon.vertices[index], commands)) return;
    for (std::size_t index = 1u; index + 1u < polygon.size; ++index)
        emplace_prepared(polygon.vertices[0], polygon.vertices[index],
                         polygon.vertices[index + 1u]);
}

void rasterize_prepared_triangle_rows(psprecomp::GuestMemory &memory,
                                      const std::array<std::uint32_t, 256> &commands,
                                      const FragmentSetup &setup,
                                      const PreparedScreenTriangle &triangle,
                                      std::int32_t row_first, std::int32_t row_last,
                                      GeRenderStats &row_stats) {
    row_first = std::max(row_first, triangle.min_y);
    row_last = std::min(row_last, triangle.max_y);
    if (row_first > row_last) return;

    const Vertex &a = triangle.a;
    const Vertex &b = triangle.b;
    const Vertex &c = triangle.c;
#if PSPRECOMP_GE_X86_SIMD
    const __m128 area_vector = _mm_set1_ps(triangle.area);
    const __m128 zero_vector = _mm_setzero_ps();
    const bool positive_area = triangle.area > 0.0f;
#endif
    for (std::int32_t y = row_first; y <= row_last; ++y) {
        const float py = static_cast<float>(y) + 0.5f;
#if PSPRECOMP_GE_X86_SIMD
        const __m128 edge_y_term = _mm_mul_ps(
            _mm_sub_ps(_mm_set1_ps(py), triangle.edge_ay), triangle.edge_dx);
#endif
        for (std::int32_t x = triangle.min_x; x <= triangle.max_x; ++x) {
            const float px = static_cast<float>(x) + 0.5f;
            float l0{}, l1{}, l2{};
#if PSPRECOMP_GE_X86_SIMD
            const __m128 edges = _mm_sub_ps(
                _mm_mul_ps(_mm_sub_ps(_mm_set1_ps(px), triangle.edge_ax), triangle.edge_dy),
                edge_y_term);
            const __m128 outside = positive_area
                ? _mm_cmplt_ps(edges, zero_vector)
                : _mm_cmpgt_ps(edges, zero_vector);
            if ((_mm_movemask_ps(outside) & 0x7) != 0) continue;
            alignas(16) float barycentric[4];
            _mm_store_ps(barycentric, _mm_div_ps(edges, area_vector));
            l0 = barycentric[0];
            l1 = barycentric[1];
            l2 = barycentric[2];
#else
            const float e0 = edge_function(b, c, px, py);
            const float e1 = edge_function(c, a, px, py);
            const float e2 = edge_function(a, b, px, py);
            if (triangle.area > 0.0f) {
                if (e0 < 0.0f || e1 < 0.0f || e2 < 0.0f) continue;
            } else if (e0 > 0.0f || e1 > 0.0f || e2 > 0.0f) {
                continue;
            }
            divide3_same_denominator(e0, e1, e2, triangle.area, l0, l1, l2);
#endif
            const float denominator = l0 * a.inv_w + l1 * b.inv_w + l2 * c.inv_w;
            if (!finite_float(denominator) || std::fabs(denominator) < 1.0e-20f) continue;
            float texture_denominator = 1.0f;
            if (triangle.texture_enabled) {
                texture_denominator = l0 * a.q * a.inv_w + l1 * b.q * b.inv_w + l2 * c.q * c.inv_w;
                if (!finite_float(texture_denominator) || std::fabs(texture_denominator) < 1.0e-20f) continue;
            }
            const float z = l0 * a.z + l1 * b.z + l2 * c.z;
            if (triangle.early_depth &&
                !fragment_depth_prepass(memory, setup, x, y, z, row_stats)) continue;
            float u = 0.0f;
            float v = 0.0f;
            if (triangle.texture_enabled) {
                const float u_numerator = l0 * a.u * a.inv_w + l1 * b.u * b.inv_w + l2 * c.u * c.inv_w;
                const float v_numerator = l0 * a.v * a.inv_w + l1 * b.v * b.inv_w + l2 * c.v * c.inv_w;
                divide2_same_denominator(u_numerator, v_numerator, texture_denominator, u, v);
            }
            const Color color = triangle.flat_shading
                ? triangle.provoking_color
                : perspective_color(a, b, c, l0, l1, l2, denominator);
            write_fragment(memory, commands, setup, x, y, z, u, v, color,
                           row_stats, triangle.early_depth);
        }
    }
}

void rasterize_prepared_triangles(psprecomp::GuestMemory &memory,
                                  const std::array<std::uint32_t, 256> &commands,
                                  const FragmentSetup &setup,
                                  const std::vector<PreparedScreenTriangle> &triangles,
                                  GeRenderStats &stats) {
    if (triangles.empty()) return;
    if (software_raster_skipped(commands)) return;

    std::int32_t min_y = triangles.front().min_y;
    std::int32_t max_y = triangles.front().max_y;
    std::int64_t covered = 0;
    for (const PreparedScreenTriangle &triangle : triangles) {
        min_y = std::min(min_y, triangle.min_y);
        max_y = std::max(max_y, triangle.max_y);
        covered += static_cast<std::int64_t>(triangle.max_x - triangle.min_x + 1) *
                   static_cast<std::int64_t>(triangle.max_y - triangle.min_y + 1);
    }

    const bool phase_diag = ge_phase_diag_enabled();
    if (phase_diag) g_ge_triangle_count += triangles.size();

    const auto rasterize_serial = [&](GeRenderStats &target) {
        for (const PreparedScreenTriangle &triangle : triangles)
            rasterize_prepared_triangle_rows(memory, commands, setup, triangle,
                                             triangle.min_y, triangle.max_y, target);
    };

    RowWorkerPool &pool = RowWorkerPool::instance();
    if (covered < parallel_pixel_threshold() || pool.worker_count() <= 1u ||
        min_y >= max_y) {
        if (phase_diag) {
            PixelLoopTimer timer;
            rasterize_serial(stats);
        } else {
            rasterize_serial(stats);
        }
        return;
    }

    const unsigned slots = pool.worker_count();
    std::array<GeRenderStats, RowWorkerPool::kMaxThreads> partial{};
    const auto run_parallel = [&] {
        pool.run(min_y, max_y, [&](unsigned index,
                                   std::int32_t row_first, std::int32_t row_last) {
            // Every participant owns distinct rows.  Walking the complete draw
            // call in original triangle order therefore preserves blending and
            // depth order for every pixel while replacing thousands of
            // per-triangle joins with one draw-call join.
            for (const PreparedScreenTriangle &triangle : triangles) {
                if (row_last < triangle.min_y || row_first > triangle.max_y) continue;
                rasterize_prepared_triangle_rows(memory, commands, setup, triangle,
                                                 row_first, row_last, partial[index]);
            }
        });
    };
    if (phase_diag) {
        PixelLoopTimer timer;
        run_parallel();
    } else {
        run_parallel();
    }
    for (unsigned index = 0u; index < slots; ++index) {
        stats.pixels_tested += partial[index].pixels_tested;
        stats.pixels_written += partial[index].pixels_written;
    }
}

void rasterize_triangle(psprecomp::GuestMemory &memory,
                        const std::array<std::uint32_t, 256> &commands,
                        const FragmentSetup &setup,
                        const Vertex &a, const Vertex &b, const Vertex &c,
                        const Vertex &provoking, bool through, GeRenderStats &stats) {
    std::vector<PreparedScreenTriangle> triangles;
    triangles.reserve(4u);
    append_prepared_triangles(commands, setup, a, b, c, provoking, through, stats, triangles);
    rasterize_prepared_triangles(memory, commands, setup, triangles, stats);
}

void rasterize_point(psprecomp::GuestMemory &memory,
                     const std::array<std::uint32_t, 256> &commands,
                     const FragmentSetup &setup,
                     Vertex vertex, bool through, GeRenderStats &stats) {
    if (!through) {
        const bool depth_clip_enabled = (data24(commands[0x1Cu]) & 1u) != 0u;
        if (!point_inside_clip(vertex, depth_clip_enabled) || !viewport_transform(vertex, commands)) return;
    }
    record_screen_vertex(stats, vertex);
    const bool texture_enabled = (data24(commands[0x1Eu]) & 1u) != 0u && (data24(commands[0xD3u]) & 1u) == 0u;
    if (texture_enabled && (!std::isfinite(vertex.q) || std::fabs(vertex.q) < 1.0e-20f)) return;
    const float q = texture_enabled ? vertex.q : 1.0f;
    write_fragment(memory, commands, setup, static_cast<std::int32_t>(std::floor(vertex.x)),
                   static_cast<std::int32_t>(std::floor(vertex.y)), vertex.z,
                   vertex.u / q, vertex.v / q, vertex.color, stats);
}

void rasterize_line(psprecomp::GuestMemory &memory,
                    const std::array<std::uint32_t, 256> &commands,
                    const FragmentSetup &setup,
                    Vertex a, Vertex b, bool through, GeRenderStats &stats) {
    if (!through) {
        // A conservative first line path: fully clipped endpoints are rejected. Triangle
        // clipping is complete; homogeneous line clipping can be added independently.
        const bool depth_clip_enabled = (data24(commands[0x1Cu]) & 1u) != 0u;
        if (!point_inside_clip(a, depth_clip_enabled) || !point_inside_clip(b, depth_clip_enabled) ||
            !viewport_transform(a, commands) || !viewport_transform(b, commands)) return;
    }
    record_screen_vertex(stats, a);
    record_screen_vertex(stats, b);
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    const std::int32_t steps = static_cast<std::int32_t>(std::ceil(std::max(std::fabs(dx), std::fabs(dy))));
    if (steps <= 0) {
        rasterize_point(memory, commands, setup, a, true, stats);
        return;
    }
    if ((data24(commands[0xD3u]) & 1u) == 0u && (data24(commands[0x50u]) & 1u) == 0u)
        ++stats.flat_shaded_primitives;
    const FragmentSetup &line_setup = setup;
    const bool line_texture_enabled =
        line_setup.texture_enabled && (data24(commands[0xD3u]) & 1u) == 0u;
    const bool line_flat_shading =
        (data24(commands[0xD3u]) & 1u) == 0u && (data24(commands[0x50u]) & 1u) == 0u;
    for (std::int32_t i = 0; i <= steps; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(steps);
        const float one_minus_t = 1.0f - t;
        const float denominator = one_minus_t * a.inv_w + t * b.inv_w;
        if (std::fabs(denominator) < 1.0e-20f) continue;
        const float texture_denominator = line_texture_enabled
            ? one_minus_t * a.q * a.inv_w + t * b.q * b.inv_w
            : denominator;
        if (!std::isfinite(texture_denominator) || std::fabs(texture_denominator) < 1.0e-20f) continue;
        const float u = (one_minus_t * a.u * a.inv_w + t * b.u * b.inv_w) / texture_denominator;
        const float v = (one_minus_t * a.v * a.inv_w + t * b.v * b.inv_w) / texture_denominator;
        const Color color = line_flat_shading ? b.color : lerp_color(a.color, b.color, t);
        write_fragment(memory, commands, line_setup,
                       static_cast<std::int32_t>(std::floor(a.x + dx * t)),
                       static_cast<std::int32_t>(std::floor(a.y + dy * t)),
                       a.z + (b.z - a.z) * t, u, v, color, stats);
    }
}

std::uint32_t read_index(const psprecomp::GuestMemory &memory, std::uint32_t base,
                         std::uint32_t index_type, std::uint32_t element) {
    switch (index_type) {
    case 1u: return memory.aot_load8(base + element);
    case 2u: return memory.aot_load16(base + element * 2u);
    case 3u: return memory.aot_load32(base + element * 4u);
    default: return element;
    }
}

// Stage 42: GE index streams are contiguous by definition. Resolve guest RAM
// once per draw instead of canonicalizing and bounds-checking every 8/16/32-bit
// index. Manual little-endian assembly is alignment-safe and matches PSP RAM.
struct IndexStreamReader {
    const psprecomp::GuestMemory *memory{};
    const std::uint8_t *raw{};
    std::uint32_t base{};
    std::uint32_t type{};

    [[nodiscard]] std::uint32_t operator()(std::uint32_t element) const noexcept {
        if (type == 0u) return element;
        if (raw == nullptr) return read_index(*memory, base, type, element);
        if (type == 1u) return raw[element];
        if (type == 2u) {
            const std::size_t o = static_cast<std::size_t>(element) * 2u;
            return static_cast<std::uint32_t>(raw[o]) |
                (static_cast<std::uint32_t>(raw[o + 1u]) << 8u);
        }
        const std::size_t o = static_cast<std::size_t>(element) * 4u;
        return static_cast<std::uint32_t>(raw[o]) |
            (static_cast<std::uint32_t>(raw[o + 1u]) << 8u) |
            (static_cast<std::uint32_t>(raw[o + 2u]) << 16u) |
            (static_cast<std::uint32_t>(raw[o + 3u]) << 24u);
    }
};

IndexStreamReader make_index_reader(const psprecomp::GuestMemory &memory,
                                    std::uint32_t base, std::uint32_t index_type,
                                    std::uint32_t count) noexcept {
    std::size_t bytes = 0u;
    if (index_type >= 1u && index_type <= 3u) {
        const std::size_t width = index_type == 1u ? 1u : index_type == 2u ? 2u : 4u;
        if (count <= std::numeric_limits<std::size_t>::max() / width)
            bytes = static_cast<std::size_t>(count) * width;
    }
    return IndexStreamReader{&memory, bytes != 0u ? memory.raw_pointer(base, bytes) : nullptr,
                             base, index_type};
}

std::uint32_t index_size(std::uint32_t index_type) noexcept {
    switch (index_type) { case 1u: return 1u; case 2u: return 2u; case 3u: return 4u; default: return 0u; }
}

} // namespace

bool test_ge_bounding_box(const psprecomp::GuestMemory &memory,
                          const std::array<std::uint32_t, 256> &commands,
                          const GeTransformState &transform,
                          std::uint32_t vertex_address,
                          std::uint32_t index_address,
                          std::uint32_t count,
                          GeBoundingBoxResult &result,
                          std::string &error) {
    result = GeBoundingBoxResult{false, vertex_address, index_address};
    if (count == 0u) return true;

    VertexLayout layout{};
    if (!build_vertex_layout_cached(data24(commands[0x12u]), layout, error)) return false;
    const std::uint32_t isize = index_size(layout.index_type);
    if (isize == 0u)
        result.next_vertex_address = vertex_address + count * layout.stride;
    else
        result.next_index_address = index_address + count * isize;

    if (isize != 0u && !memory.contains(index_address, static_cast<std::size_t>(count) * isize)) {
        error = "GE BBOX index stream lies outside guest memory";
        return false;
    }

    const bool depth_clip_enabled = (data24(commands[0x1Cu]) & 1u) != 0u;
    const std::uint32_t plane_count = depth_clip_enabled ? 6u : 4u;
    std::array<bool, 6> all_outside{};
    all_outside.fill(true);

    bool through_visible = false;
    const IndexStreamReader bbox_indices =
        make_index_reader(memory, index_address, layout.index_type, count);
    for (std::uint32_t element = 0u; element < count; ++element) {
        const std::uint32_t index = bbox_indices(element);
        Vertex vertex{};
        if (!decode_vertex(memory, vertex_address + index * layout.stride, layout,
                           commands, transform, vertex, error)) return false;

        if (layout.through) {
            const std::uint32_t region1 = data24(commands[0x15u]);
            const std::uint32_t region2 = data24(commands[0x16u]);
            const float x1 = static_cast<float>(region1 & 0x3FFu);
            const float y1 = static_cast<float>((region1 >> 10u) & 0x3FFu);
            const float x2 = static_cast<float>(region2 == 0u ? 1023u : region2 & 0x3FFu);
            const float y2 = static_cast<float>(region2 == 0u ? 1023u : (region2 >> 10u) & 0x3FFu);
            through_visible = through_visible ||
                (vertex.x >= x1 && vertex.x <= x2 && vertex.y >= y1 && vertex.y <= y2);
            continue;
        }

        for (std::uint32_t plane = 0u; plane < plane_count; ++plane) {
            if (clip_distance(vertex, plane) >= 0.0f) all_outside[plane] = false;
        }
    }

    if (layout.through) {
        result.visible = through_visible;
    } else {
        result.visible = true;
        for (std::uint32_t plane = 0u; plane < plane_count; ++plane) {
            if (all_outside[plane]) {
                result.visible = false;
                break;
            }
        }
    }
    return true;
}

bool render_ge_primitive(psprecomp::GuestMemory &memory,
                         const std::array<std::uint32_t, 256> &commands,
                         const GeTransformState &transform,
                         std::uint32_t vertex_address,
                         std::uint32_t index_address,
                         std::uint32_t primitive_data,
                         GeRenderStats &stats,
                         std::string &error,
                         std::uint32_t logical_primitive_count,
                         std::uint64_t draw_state_revision,
                         std::uint64_t camera_state_revision,
                         std::uint64_t lighting_state_revision,
                         bool collect_diagnostic_stats) {
    GeRenderStatsCollectionScope stats_scope(collect_diagnostic_stats);
    stats.next_vertex_address = vertex_address;
    stats.next_index_address = index_address;
    if (collect_diagnostic_stats) ++stats.primitives;
    const std::uint32_t primitive = (primitive_data >> 16u) & 7u;
    const std::uint32_t count = primitive_data & 0xFFFFu;
    if (count == 0u) return true;

    VertexLayout layout;
    if (!build_vertex_layout_cached(data24(commands[0x12u]), layout, error)) return false;

    if (ge_phase_diag_enabled()) {
        ++g_ge_primitive_count;
        g_ge_vertex_count += count;
    }

    const bool gpu_backend_enabled = ge_gpu_backend_active();
    GeGpuDrawDescriptor gpu_draw{};
    {
    PhaseTimer draw_setup_timer(g_ge_draw_setup_ns);
    if (gpu_backend_enabled) {
        // Cached GE state reuse: most adjacent PRIM commands only
        // advance vertex/index pointers while texture, blend, depth, scissor and
        // framebuffer state stay unchanged. Re-decoding ~60 GE registers and all
        // eight mip descriptors for every draw was pure host overhead. The GE
        // interpreter supplies a monotonic revision that excludes VADDR/IADDR/PRIM.
        static thread_local std::uint64_t cached_draw_revision =
            std::numeric_limits<std::uint64_t>::max();
        static thread_local GeGpuDrawDescriptor cached_draw_state{};
        const bool reuse_draw_state = draw_state_revision != 0u &&
            cached_draw_revision == draw_state_revision;
        if (reuse_draw_state) {
            gpu_draw = cached_draw_state;
        } else {
            gpu_draw.vertex_type = data24(commands[0x12u]);
            gpu_draw.framebuffer_address = framebuffer_address(commands);
            gpu_draw.framebuffer_stride = data24(commands[0x9Du]) & 0x7FCu;
            gpu_draw.framebuffer_format = data24(commands[0xD2u]) & 3u;
            gpu_draw.texture_format = data24(commands[0xC3u]) & 0xFu;
            gpu_draw.texture_selected_level=selected_texture_level(commands);
            for(std::uint32_t level=0;level<8;++level){gpu_draw.texture_level_addresses[level]=texture_address(commands,level);gpu_draw.texture_level_buffer_widths[level]=std::max<std::uint32_t>(1u,data24(commands[0xA8u+level])&0x7FFu);const auto sz=data24(commands[0xB8u+level]);gpu_draw.texture_level_widths[level]=1u<<(sz&0xFu);gpu_draw.texture_level_heights[level]=1u<<((sz>>8u)&0xFu);}
            gpu_draw.texture_address=gpu_draw.texture_level_addresses[0];gpu_draw.texture_buffer_width=gpu_draw.texture_level_buffer_widths[0];gpu_draw.texture_width=gpu_draw.texture_level_widths[0];gpu_draw.texture_height=gpu_draw.texture_level_heights[0];
            const std::uint32_t gpu_texfunc = data24(commands[0xC9u]);
            gpu_draw.texture_function = gpu_texfunc & 7u;
            gpu_draw.texture_use_alpha = (gpu_texfunc & 0x100u) != 0u;
            gpu_draw.texture_double_color = (gpu_texfunc & 0x10000u) != 0u;
            gpu_draw.texture_env = data24(commands[0xCAu]) & 0x00FFFFFFu;
            gpu_draw.clut_address = clut_address(commands);
            const std::uint32_t clut_data = data24(commands[0xC5u]);
            gpu_draw.clut_format = clut_data & 3u;
            gpu_draw.clut_shift = (clut_data >> 2u) & 0x1Fu;
            gpu_draw.clut_mask = (clut_data >> 8u) & 0xFFu;
            gpu_draw.clut_start = ((clut_data >> 16u) & 0x1Fu) << 4u;
            const std::uint32_t texture_mode = data24(commands[0xC2u]);
            const std::uint32_t texture_filter = data24(commands[0xC6u]);
            const std::uint32_t texture_level = data24(commands[0xC8u]);
            gpu_draw.texture_swizzled = (texture_mode & 1u) != 0u;
            gpu_draw.texture_min_linear = (texture_filter & 1u) != 0u;
            gpu_draw.texture_mipmap_linear = (texture_filter & 2u) != 0u;
            gpu_draw.texture_mipmap_enabled = (texture_filter & 4u) != 0u;
            gpu_draw.texture_mag_linear = ((texture_filter >> 8u) & 1u) != 0u;
            gpu_draw.texture_linear = gpu_draw.texture_mag_linear;
            gpu_draw.texture_max_level = (texture_mode >> 16u) & 7u;
            gpu_draw.texture_level_mode = texture_level & 3u;
            gpu_draw.texture_level_offset16 = signed_texture_lod_offset16(texture_level);
            gpu_draw.texture_lod_slope = decode_float24(data24(commands[0xD0u]));
            const std::uint32_t texture_wrap = data24(commands[0xC7u]);
            gpu_draw.texture_clamp_u = (texture_wrap & 1u) != 0u;
            gpu_draw.texture_clamp_v = (texture_wrap & 0x100u) != 0u;
            // Same decode as make_fragment_setup(); the GPU path needs it too.
            const std::uint32_t gpu_scissor1 = data24(commands[0xD4u]);
            const std::uint32_t gpu_scissor2 = data24(commands[0xD5u]);
            gpu_draw.scissor_x0 = static_cast<std::int32_t>(gpu_scissor1 & 0x3FFu);
            gpu_draw.scissor_y0 = static_cast<std::int32_t>((gpu_scissor1 >> 10u) & 0x3FFu);
            gpu_draw.scissor_x1 = gpu_scissor2 == 0u
                ? static_cast<std::int32_t>(gpu_draw.framebuffer_stride - 1u)
                : static_cast<std::int32_t>(gpu_scissor2 & 0x3FFu);
            gpu_draw.scissor_y1 = gpu_scissor2 == 0u
                ? 271 : static_cast<std::int32_t>((gpu_scissor2 >> 10u) & 0x3FFu);
            gpu_draw.through = layout.through;
            gpu_draw.texture_enabled = (data24(commands[0x1Eu]) & 1u) != 0u;
            gpu_draw.blend_enabled = (data24(commands[0x21u]) & 1u) != 0u;
            const std::uint32_t blend_mode = data24(commands[0xDFu]);
            gpu_draw.blend_source_factor = blend_mode & 0xFu;
            gpu_draw.blend_dest_factor = (blend_mode >> 4u) & 0xFu;
            gpu_draw.blend_equation = (blend_mode >> 8u) & 7u;
            gpu_draw.blend_fix_source = data24(commands[0xE0u]) & 0x00FFFFFFu;
            gpu_draw.blend_fix_dest = data24(commands[0xE1u]) & 0x00FFFFFFu;
            gpu_draw.color_write_mask = data24(commands[0xE8u]) |
                ((data24(commands[0xE9u]) & 0xFFu) << 24u);
            const std::uint32_t alpha_test = data24(commands[0xDBu]);
            gpu_draw.alpha_test_enabled = (data24(commands[0x22u]) & 1u) != 0u;
            gpu_draw.alpha_function = alpha_test & 7u;
            gpu_draw.alpha_reference = (alpha_test >> 8u) & 0xFFu;
            gpu_draw.alpha_mask = (alpha_test >> 16u) & 0xFFu;
            gpu_draw.depth_test_enabled = (data24(commands[0x23u]) & 1u) != 0u;
            gpu_draw.depth_write_enabled = (data24(commands[0xE7u]) & 1u) == 0u;
            gpu_draw.depth_function = data24(commands[0xDEu]) & 7u;
            gpu_draw.fog_enabled = (data24(commands[0x1Fu]) & 1u) != 0u;
            gpu_draw.fog_color = data24(commands[0xCFu]) & 0x00FFFFFFu;
            gpu_draw.fog_end = decode_float24(data24(commands[0xCDu]));
            gpu_draw.fog_slope = decode_float24(data24(commands[0xCEu]));
            const std::uint32_t gpu_clear = data24(commands[0xD3u]);
            gpu_draw.clear_mode = (gpu_clear & 1u) != 0u;
            gpu_draw.clear_color = (gpu_clear & 0x100u) != 0u;
            gpu_draw.clear_alpha = (gpu_clear & 0x200u) != 0u;
            gpu_draw.clear_depth = (gpu_clear & 0x400u) != 0u;
            gpu_draw.texture_content_signature = 0u;
            gpu_draw.clut_checksum = 0u;
            if (draw_state_revision != 0u) {
                cached_draw_state = gpu_draw;
                cached_draw_revision = draw_state_revision;
            }
        }
        gpu_draw.primitive = primitive;
        gpu_draw.vertex_count = count;

        // CLUT bytes can be rewritten by guest code between lists without a GE
        // register change, so preserve conservative per-draw palette validation.
        gpu_draw.clut_checksum = 0u;
        if (gpu_draw.texture_format >= 4u && gpu_draw.texture_format <= 7u &&
            gpu_draw.clut_address != 0u) {
            // Only the window this draw can index: index = ((raw >> shift) &
            // mask) + start. Hashing all 256 entries folded in palette slots the
            // texture never reads, so an unrelated recolour elsewhere in the CLUT
            // minted a new cache key -- 35491 shared images and 850 MB decoded in
            // one session, against 587 and 75 MB.
            const std::uint32_t entry_bytes = gpu_draw.clut_format == 3u ? 4u : 2u;
            const std::uint32_t first = std::min(gpu_draw.clut_start, 255u);
            const std::uint32_t last = std::min(first + gpu_draw.clut_mask, 255u);
            const std::uint32_t offset_bytes = first * entry_bytes;
            const std::uint32_t clut_bytes = (last - first + 1u) * entry_bytes;
            if (const std::uint8_t *clut = memory.raw_pointer(
                    gpu_draw.clut_address + offset_bytes, clut_bytes)) {
                std::uint32_t checksum = 2166136261u;
                std::uint32_t offset = 0u;
                for (; offset + 4u <= clut_bytes; offset += 4u) {
                    const std::uint32_t word = static_cast<std::uint32_t>(clut[offset]) |
                        (static_cast<std::uint32_t>(clut[offset + 1u]) << 8u) |
                        (static_cast<std::uint32_t>(clut[offset + 2u]) << 16u) |
                        (static_cast<std::uint32_t>(clut[offset + 3u]) << 24u);
                    checksum ^= word;
                    checksum *= 16777619u;
                }
                if (offset < clut_bytes) {
                    std::uint32_t tail = clut[offset];
                    if (offset + 1u < clut_bytes) tail |= static_cast<std::uint32_t>(clut[offset + 1u]) << 8u;
                    checksum ^= tail;
                    checksum *= 16777619u;
                }
                gpu_draw.clut_checksum = checksum;
            }
        }
        // All texture identity fields, including the mutable CLUT checksum, are
        // final now. Hash them once; the backend will reuse these derived keys
        // throughout signature/cache/upload/accumulation for this PRIM.
        ge_gpu_backend_prepare_texture_keys(gpu_draw);
        gpu_draw.texture_content_signature = 0u;
        if (gpu_draw.texture_enabled && ge_gpu_backend_texture_signature_needed(gpu_draw)) {
            const bool feedback = ge_gpu_backend_is_framebuffer_feedback_texture(gpu_draw);
            const std::uint32_t signature_levels = feedback ? 1u :
                (gpu_draw.texture_mipmap_enabled
                    ? std::min<std::uint32_t>(8u, gpu_draw.texture_max_level + 1u) : 1u);
            std::uint64_t signature = 0xCBF29CE484222325ull;
            bool any_signature = false;
            for (std::uint32_t level = 0u; level < signature_levels; ++level) {
                const TextureSetup source = make_texture_setup_for_level(memory, commands, level);
                const std::uint64_t part = texture_source_signature(memory, source);
                if (part == 0u) continue;
                any_signature = true;
                signature ^= part + 0x9E3779B97F4A7C15ull + (signature << 6u) + (signature >> 2u);
            }
            gpu_draw.texture_content_signature = any_signature ? signature : 0u;
        }
        // Semantic pipe tag is resolved only after the mutable CLUT checksum is
        // known. Skin is exact from the PSP weight format; vehicle recognition
        // can learn the palette mutation VCS uses for recolourable cars.
        gpu_draw.shader_pipe = proper_shaders_classify_draw(
            gpu_draw, layout.weight_type != 0u);
        ge_gpu_backend_record_draw(gpu_draw);
        if (!gpu_draw.through && !gpu_draw.clear_mode &&
            primitive >= 3u && primitive <= 5u) {
            const std::array<float, 8> cloud_viewport{
                decode_float24(data24(commands[0x42u])),
                decode_float24(data24(commands[0x43u])),
                decode_float24(data24(commands[0x44u])),
                decode_float24(data24(commands[0x45u])),
                decode_float24(data24(commands[0x46u])),
                decode_float24(data24(commands[0x47u])),
                static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f,
                static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f};
            // VCS' authoritative camera origin. The affine GE view used by
            // individual passes is not guaranteed to encode this position as
            // a rigid inverse (reflections and camera-relative passes do not),
            // which made a world-space cloud slab orbit while only turning.
            constexpr std::uint32_t kVcsCameraPosition = 0x08BC87E0u;
            std::array<float, 3> cloud_camera_position{};
            if (memory.contains(kVcsCameraPosition, 12u)) {
                cloud_camera_position = {
                    std::bit_cast<float>(memory.load32(kVcsCameraPosition + 0u)),
                    std::bit_cast<float>(memory.load32(kVcsCameraPosition + 4u)),
                    std::bit_cast<float>(memory.load32(kVcsCameraPosition + 8u))};
            }
            ge_gpu_backend_observe_camera(transform.view, transform.projection,
                                          cloud_viewport, cloud_camera_position,
                                          gpu_draw, count);
        }
        if (!gpu_draw.clear_mode && primitive >= 3u && primitive <= 6u) {
            fps_overlay_observe_draw(gpu_draw, count);
            savedata_utility_ui_observe_draw(gpu_draw, count);
        }
        if (!gpu_draw.through && !gpu_draw.clear_mode &&
            primitive >= 3u && primitive <= 5u &&
            !project2dfx_observe_camera_hot(gpu_draw, count, camera_state_revision)) {
            project2dfx_observe_camera(
                transform.view, transform.projection,
                decode_float24(data24(commands[0x42u])),
                decode_float24(data24(commands[0x43u])),
                decode_float24(data24(commands[0x44u])),
                decode_float24(data24(commands[0x45u])),
                decode_float24(data24(commands[0x46u])),
                decode_float24(data24(commands[0x47u])),
                static_cast<float>(data24(commands[0x4Cu]) & 0xFFFFu) / 16.0f,
                static_cast<float>(data24(commands[0x4Du]) & 0xFFFFu) / 16.0f,
                1.0f, gpu_draw, count, camera_state_revision);
        }
    }
    }
    const std::uint32_t isize = index_size(layout.index_type);
    auto advance_stream = [&]() {
        if (isize == 0u) stats.next_vertex_address = vertex_address + count * layout.stride;
        else stats.next_index_address = index_address + count * isize;
    };
    if (isize != 0u && !memory.contains(index_address, static_cast<std::size_t>(count) * isize)) {
        error = "GE index stream lies outside guest memory";
        return false;
    }

    // Rectangle primitives are a through-mode construct. Unknown primitive families
    // still advance the stream so malformed or future lists do not desynchronize it.
    if (primitive > 6u || (!layout.through && primitive == 6u)) {
        if (collect_diagnostic_stats) ++stats.unsupported_primitives;
        advance_stream();
        error.clear();
        return true;
    }

    if (collect_diagnostic_stats && layout.weight_type != 0u) stats.skinned_vertices += count;
    if (collect_diagnostic_stats && layout.morph_count > 1u) stats.morphed_vertices += count;
    if (collect_diagnostic_stats && !layout.through && (data24(commands[0x17u]) & 1u) != 0u) stats.lit_vertices += count;
    const std::uint32_t uv_generation = data24(commands[0xC0u]) & 3u;
    if (!layout.through && (uv_generation == 1u || uv_generation == 2u))
        if (collect_diagnostic_stats) stats.generated_uv_vertices += count;

    // Decide the pure Vulkan path before resolving software-raster state.  The
    // Stage 39 hardware-transform path returns before touching FragmentSetup,
    // so decoding framebuffer/depth pointers and CPU fragment state here was
    // dead work on the dominant city draw path.
    const bool gpu_only_triangle_path = gpu_backend_enabled && software_raster_skipped(commands);

    // Decode the complete PSP mip chain into one contiguous allocation.  The
    // old vector-per-level path then copied the entire chain a second time in
    // ge_gpu_backend before Vulkan could see it.  Streaming bursts are exactly
    // where that extra allocation/copy hurts most.
    if (gpu_backend_enabled && gpu_draw.texture_enabled &&
        ge_gpu_backend_texture_needed(gpu_draw) &&
        !ge_gpu_backend_adopt_shared_texture(gpu_draw)) {
        PhaseTimer texture_timer(g_ge_texture_upload_ns);
        const bool framebuffer_feedback =
            ge_gpu_backend_is_framebuffer_feedback_texture(gpu_draw);
        const std::uint32_t level_count = framebuffer_feedback ? 1u :
            (gpu_draw.texture_mipmap_enabled
                ? std::min<std::uint32_t>(8u, gpu_draw.texture_max_level + 1u) : 1u);
        std::array<TextureSetup, 8> mip_setups{};
        std::size_t total_bytes = 0u;
        bool all = true;

        // DDS replacement. The renderer already hands the backend plain RGBA8
        // for every texture, so a replacement is simply a different buffer on
        // the same upload path -- no separate sampling route exists to go wrong.
        //
        // texture_width/height stay at the guest's values on purpose. The shader
        // samples with normalized coordinates whose scale comes from the guest's
        // texture matrix, not from the resident image, so leaving them alone lets
        // a 512x512 replacement stand in for a 64x64 original with the UVs still
        // landing where the game intends.
        bool replaced = false;
        if (!framebuffer_feedback) {
            const TextureSetup base = make_texture_setup_for_level(memory, commands, 0u);
            const std::uint64_t base_bytes = texture_source_byte_size(base);
            if (base_bytes != 0u && base_bytes <= std::numeric_limits<std::size_t>::max()) {
                const auto span = static_cast<std::size_t>(base_bytes);
                const std::uint32_t depth = base.format == 4u   ? 4u
                                            : base.format == 5u ? 8u
                                                                : 0u;
                TextureReplacement replacement{};
                if (texture_replacement_lookup(memory.raw_pointer(base.base, span), span,
                                               base.width, base.height, depth, replacement)) {
                    std::vector<std::byte> pixels(
                        replacement.rgba, replacement.rgba + replacement.size);
                    replaced = ge_gpu_backend_upload_decoded_texture(
                        gpu_draw, replacement.width, replacement.height, pixels);
                    // The guest's TEXFUNC decides whether the shader reads a
                    // texture's alpha at all. A palettized original the game
                    // declared opaque leaves that bit clear, and a replacement
                    // carrying real transparency would be flattened by it. Raise
                    // it only when the image actually has non-opaque pixels, so
                    // an opaque replacement changes nothing.
                    if (replaced && replacement.has_transparency)
                        gpu_draw.texture_use_alpha = true;
                }
            }
        }

        for (std::uint32_t level = 0u; !replaced && level < level_count; ++level) {
            mip_setups[level] = make_texture_setup_for_level(memory, commands, level);
            const std::uint64_t bytes = static_cast<std::uint64_t>(mip_setups[level].width) *
                                        mip_setups[level].height * 4ull;
            if (bytes > std::numeric_limits<std::size_t>::max() - total_bytes) {
                all = false;
                break;
            }
            total_bytes += static_cast<std::size_t>(bytes);
        }
        std::vector<std::byte> decoded;
        if (all && !replaced) {
            try { decoded.resize(total_bytes); } catch (...) { all = false; }
        }
        std::size_t offset = 0u;
        for (std::uint32_t level = 0u; all && !replaced && level < level_count; ++level) {
            const std::size_t bytes = static_cast<std::size_t>(mip_setups[level].width) *
                                      mip_setups[level].height * 4u;
            if (!decode_texture_rgba_into(memory, mip_setups[level],
                                          std::span<std::byte>(decoded).subspan(offset, bytes))) {
                all = false;
                break;
            }
            offset += bytes;
        }
        if (replaced) {
            // Already uploaded above.
        } else if (all) {
            (void)ge_gpu_backend_upload_decoded_texture_chain_packed(
                gpu_draw, mip_setups[0].width, mip_setups[0].height,
                level_count, std::move(decoded));
        } else {
            (void)ge_gpu_backend_upload_decoded_texture(gpu_draw, 0u, 0u, {});
        }
    }

    // Hardware-transform frontend. The CPU still decodes
    // PSP vertex formats (and skin/morph data when present), but projected
    // triangle draws no longer materialize clip polygons or screen-space
    // PreparedScreenTriangle objects. World/view/projection, viewport mapping,
    // clipping and rasterization stay on Vulkan.
    // Lit draws are admitted through the hybrid path: the CPU applies the world
    // matrix and the lighting, the GPU still does view/projection/viewport/clip.
    // Refusing them outright left 90.5% of city vertices on the legacy path.
    //
    // A normal is required, because that is what apply_lighting reads.
    // PSPRECOMP_GE_GPU_HW_LIT=0 restores the Stage 39 behaviour of refusing
    // every lit draw, for A/B in one binary.
    static const bool hw_lit_enabled = [] {
        const char *text = std::getenv("PSPRECOMP_GE_GPU_HW_LIT");
        return text == nullptr || (*text != '\0' && std::strcmp(text, "0") != 0);
    }();
    const bool lighting_on = (data24(commands[0x17u]) & 1u) != 0u;
    // A missing normal is not a reason to refuse: decode_vertex() defaults it to
    // {0,0,1} and lights the vertex anyway, so the hybrid does the same. Only
    // morph targets stay out, because their normals are blended per target and
    // that path is not worth replicating for the volume involved.
    const bool hw_lighting_on_cpu = lighting_on && hw_lit_enabled &&
        layout.morph_count == 1u;
    const bool hw_lighting_compatible = !lighting_on || hw_lighting_on_cpu;

    const bool hw_transform_eligible = gpu_only_triangle_path &&
        ge_gpu_backend_graphics_ready() && gpu_hardware_transform_enabled() &&
        !layout.through && primitive >= 3u && primitive <= 5u &&
        !gpu_draw.clear_mode && hw_lighting_compatible &&
        layout.position_type != 0u;

    if (hw_transform_eligible) {
        PreparedLighting prepared_lighting{};
        if (hw_lighting_on_cpu) {
            // Lighting registers normally stay unchanged for long runs of world draws.
            // Decoding/normalizing all four PSP lights per PRIM was pure serial GE work.
            // The profile supplies a monotonic lighting-state revision, so reuse the
            // prepared immutable state until one of the actual lighting/material
            // registers changes.  Tests/standalone callers pass revision 0 and keep
            // the exact uncached behavior.
            struct LightingCache {
                std::uint64_t revision{};
                bool has_vertex_color{};
                bool valid{};
                PreparedLighting state{};
            };
            static thread_local LightingCache lighting_cache;
            const bool has_vertex_color = layout.color_type >= 4u;
            if (lighting_state_revision != 0u && lighting_cache.valid &&
                lighting_cache.revision == lighting_state_revision &&
                lighting_cache.has_vertex_color == has_vertex_color) {
                prepared_lighting = lighting_cache.state;
            } else {
                prepared_lighting = prepare_lighting(has_vertex_color, commands);
                if (lighting_state_revision != 0u) {
                    lighting_cache.revision = lighting_state_revision;
                    lighting_cache.has_vertex_color = has_vertex_color;
                    lighting_cache.state = prepared_lighting;
                    lighting_cache.valid = true;
                }
            }
        }
        static thread_local std::vector<std::uint32_t> occurrence_indices;
        static thread_local std::vector<std::uint32_t> unique_indices;
        static thread_local std::vector<std::uint32_t> occurrence_remap;
        static thread_local std::vector<GeGpuVertex> decoded_vertices;
        static thread_local std::vector<GeGpuVertex> submitted_vertices;
        static thread_local std::vector<std::uint32_t> triangle_indices;
        occurrence_indices.clear();
        unique_indices.clear();
        occurrence_remap.clear();
        decoded_vertices.clear();
        submitted_vertices.clear();
        triangle_indices.clear();

        // Common VCS meshes are either non-indexed or index a compact contiguous
        // vertex range.  Do not manufacture 0..N identity vectors (nor a second
        // remap vector) for those cases.  Sparse streams keep the conservative
        // sorted-unique fallback.
        const bool indexed = isize != 0u;
        bool contiguous_decode = !indexed;
        bool sequential_index_stream = indexed;
        std::uint32_t first_index_value = 0u;
        std::uint32_t contiguous_first = 0u;
        std::uint32_t contiguous_count = indexed ? 0u : count;

        {
        PhaseTimer vertex_timer(g_ge_vertex_decode_ns);
        if (indexed) {
            occurrence_indices.reserve(count);
            const IndexStreamReader draw_indices =
                make_index_reader(memory, index_address, layout.index_type, count);
            std::uint32_t lower = std::numeric_limits<std::uint32_t>::max();
            std::uint32_t upper = 0u;
            for (std::uint32_t i = 0u; i < count; ++i) {
                const std::uint32_t index = draw_indices(i);
                occurrence_indices.push_back(index);
                if (i == 0u) {
                    first_index_value = index;
                } else if (index != first_index_value + i) {
                    sequential_index_stream = false;
                }
                lower = std::min(lower, index);
                upper = std::max(upper, index);
            }
            const std::uint64_t range64 = count == 0u ? 0u
                : static_cast<std::uint64_t>(upper) - lower + 1u;
            const bool compact_range = range64 != 0u && range64 <= 65536u &&
                range64 <= std::max<std::uint64_t>(64u, static_cast<std::uint64_t>(count) * 2u);
            if (compact_range) {
                contiguous_decode = true;
                contiguous_first = lower;
                contiguous_count = static_cast<std::uint32_t>(range64);
            } else {
                unique_indices = occurrence_indices;
                std::sort(unique_indices.begin(), unique_indices.end());
                unique_indices.erase(std::unique(unique_indices.begin(), unique_indices.end()),
                                     unique_indices.end());
                occurrence_remap.reserve(count);
                for (std::uint32_t index : occurrence_indices) {
                    const auto found = std::lower_bound(unique_indices.begin(), unique_indices.end(), index);
                    occurrence_remap.push_back(static_cast<std::uint32_t>(found - unique_indices.begin()));
                }
            }
        }
        }

        const std::size_t decode_count = contiguous_decode
            ? static_cast<std::size_t>(contiguous_count) : unique_indices.size();

        // Stage 45.4 city fast span.  The overwhelmingly common 0x0115 layout
        // used to call GuestMemory::raw_pointer() once for every decoded vertex.
        // A compact/non-indexed draw is contiguous by construction, so validate
        // the full byte range once.  The exact same span can now bypass CPU
        // conversion entirely on DX12 and be decoded by the vertex shader.
        const bool fast_0115_layout = layout.type == 0x000115u && !layout.through &&
            layout.morph_count == 1u && layout.weight_type == 0u &&
            layout.normal_type == 0u && uv_generation == 0u && layout.stride == 10u;
        // Stage 45.5 validates the compact source range once for projected draws.
        // The dominant 0x0115 path can then hand the untouched PSP bytes to DX12
        // without any per-vertex GuestMemory calls.
        const std::uint8_t *contiguous_raw = nullptr;
        std::size_t contiguous_raw_bytes = 0u;
        if (contiguous_decode && decode_count != 0u) {
            const std::uint64_t first_address64 = static_cast<std::uint64_t>(vertex_address) +
                static_cast<std::uint64_t>(contiguous_first) * layout.stride;
            const std::uint64_t byte_count64 = static_cast<std::uint64_t>(decode_count) * layout.stride;
            if (first_address64 <= std::numeric_limits<std::uint32_t>::max() &&
                byte_count64 <= std::numeric_limits<std::size_t>::max()) {
                contiguous_raw_bytes = static_cast<std::size_t>(byte_count64);
                contiguous_raw = memory.raw_pointer(
                    static_cast<std::uint32_t>(first_address64), contiguous_raw_bytes);
            }
        }
        const std::uint8_t *fast_0115_raw = fast_0115_layout ? contiguous_raw : nullptr;
        const std::size_t fast_0115_bytes = fast_0115_raw != nullptr ? contiguous_raw_bytes : 0u;
        Vec3 fast_0115_world_normal{};
        const Vec3 *fast_0115_world_normal_ptr = nullptr;
        if (fast_0115_raw != nullptr && hw_lighting_on_cpu) {
            fast_0115_world_normal = transform_normal_4x3(
                transform.world, Vec3{0.0f, 0.0f, 1.0f});
            if ((data24(commands[0x51u]) & 1u) != 0u)
                fast_0115_world_normal = fast_0115_world_normal * -1.0f;
            fast_0115_world_normal = normalized_or_001(fast_0115_world_normal);
            fast_0115_world_normal_ptr = &fast_0115_world_normal;
        }

        const GeGpuDrawDescriptor effective_draw = gpu_effective_draw_descriptor(gpu_draw);
        const bool sampled_texture_ready = effective_draw.texture_enabled &&
            ge_gpu_backend_texture_available(effective_draw);

        // Directional-only lighting on 0x0115 is affine in vertex colour and
        // can be performed exactly once in the VS, so the draw no longer needs
        // CPU world transform + lighting for every vertex. Keep diagnostic/
        // missing-texture colour overrides on the legacy CPU path so this fast
        // path never changes their intended output.
        std::array<float, 4> gpu_light_mul{1.0f, 1.0f, 1.0f, 1.0f};
        std::array<float, 4> gpu_light_add{};
        bool gpu_directional_lighting = false;
        if (fast_0115_raw != nullptr && hw_lighting_on_cpu &&
            fast_0115_world_normal_ptr != nullptr &&
            !gpu_force_white_vertex_colors_enabled() && !gpu_geometry_debug_colors_enabled() &&
            (!effective_draw.texture_enabled || sampled_texture_ready)) {
            gpu_directional_lighting = prepare_directional_lighting_affine(
                prepared_lighting, *fast_0115_world_normal_ptr, gpu_light_mul, gpu_light_add);
        }
        const bool cpu_lighting_effective = hw_lighting_on_cpu && !gpu_directional_lighting;
        const bool flat_shading = (data24(commands[0x50u]) & 1u) == 0u;
        GeGpuHardwareTransform hw =
            build_gpu_hardware_transform(commands, transform, cpu_lighting_effective,
                                         uv_generation == 1u || uv_generation == 2u);
        // D3D12 supports strips natively; triangle fans are expanded to a list.
        hw.primitive = primitive == 5u ? 3u : primitive;
        hw.vertex_color_affine = gpu_directional_lighting;
        hw.vertex_color_mul = gpu_light_mul;
        hw.vertex_color_add = gpu_light_add;
        hw.logical_prim_batches = std::max<std::uint32_t>(1u, logical_primitive_count);
        hw.unique_vertices_decoded = static_cast<std::uint32_t>(decode_count);
        hw.index_reuses = count > decode_count
            ? count - static_cast<std::uint32_t>(decode_count) : 0u;

        // The biggest 45.4 GE optimization: for the common unlit/smooth 0x0115
        // world stream, do not manufacture a 56-byte GeGpuVertex (or even the
        // compact 36-byte DX12 vertex) at all. Snapshot the original 10-byte PSP
        // records and let the DX12 VS unpack them. This removes format conversion,
        // color expansion and s16->float normalization from the serialized GE
        // thread while cutting upload traffic by another 72% versus the new 36B
        // stream (82% versus the original 56B stream).
        const bool packed_0115_candidate = packed_0115_gpu_decode_enabled() &&
            fast_0115_raw != nullptr && !cpu_lighting_effective && !flat_shading &&
            !gpu_force_white_vertex_colors_enabled() && !gpu_geometry_debug_colors_enabled() &&
            (!effective_draw.texture_enabled || sampled_texture_ready);
        if (packed_0115_candidate) {
            const auto occurrence_index = [&](std::size_t i) -> std::uint32_t {
                if (!indexed) return static_cast<std::uint32_t>(i);
                if (contiguous_decode) return occurrence_indices[i] - contiguous_first;
                return occurrence_remap[i];
            };
            bool needs_indices = true;
            // Stable DrawInstanced fast path. Non-indexed triangle lists were
            // already direct; indexed lists that are literally N,N+1,N+2...
            // are semantically identical and can drop their index stream too.
            // This keeps the 44.7 non-indexed D3D12 submission semantics while
            // avoiding both triangle-index construction and 10-byte vertex
            // expansion for identity-index meshes.
            const bool identity_indexed_triangle_list = indexed && sequential_index_stream &&
                contiguous_decode && contiguous_first == first_index_value && decode_count == count;
            if (direct_nonindexed_gpu_draw_enabled() &&
                (primitive == 3u || primitive == 4u) &&
                (primitive != 3u || (count % 3u) == 0u) && decode_count == count &&
                (!indexed || identity_indexed_triangle_list)) {
                needs_indices = false;
            }
            if (needs_indices) {
                const std::size_t triangle_count = primitive == 3u ? count / 3u
                    : (count > 2u ? count - 2u : 0u);
                triangle_indices.reserve(triangle_count * 3u);
                if (primitive == 3u) {
                    for (std::size_t i = 0u; i + 2u < count; i += 3u) {
                        triangle_indices.push_back(occurrence_index(i));
                        triangle_indices.push_back(occurrence_index(i + 1u));
                        triangle_indices.push_back(occurrence_index(i + 2u));
                    }
                } else if (primitive == 4u) {
                    // D3D12 natively supports triangle strips. Preserve the PSP
                    // strip index stream instead of manufacturing 3*(N-2)
                    // triangle-list indices on the CPU. IA handles alternating
                    // winding as part of strip assembly.
                    triangle_indices.reserve(count);
                    for (std::size_t i = 0u; i < count; ++i)
                        triangle_indices.push_back(occurrence_index(i));
                } else {
                    for (std::size_t i = 1u; i + 1u < count; ++i) {
                        triangle_indices.push_back(occurrence_index(0u));
                        triangle_indices.push_back(occurrence_index(i));
                        triangle_indices.push_back(occurrence_index(i + 1u));
                    }
                }
            }
            const std::size_t packed_triangle_count = primitive == 4u
                ? (count > 2u ? count - 2u : 0u)
                : (needs_indices ? triangle_indices.size() / 3u : count / 3u);
            bool accepted = false;
            {
                PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
                accepted = ge_gpu_backend_accumulate_hardware_packed_0115(
                    effective_draw, hw,
                    std::span<const std::byte>(
                        reinterpret_cast<const std::byte *>(fast_0115_raw), fast_0115_bytes),
                    static_cast<std::uint32_t>(decode_count), triangle_indices);
            }
            if (accepted) {
                if (collect_diagnostic_stats) stats.decoded_vertices += decode_count;
                if (collect_diagnostic_stats) stats.triangles += packed_triangle_count;
                advance_stream();
                return true;
            }
            triangle_indices.clear();
        }

        // Fallback for lit/skinned/morphed/sparse/debug draws and for any backend
        // that does not implement the packed 0x0115 stream.
        decoded_vertices.resize(decode_count);
        {
        PhaseTimer vertex_timer(g_ge_vertex_decode_ns);
        const auto decode_one = [&](std::size_t i, std::string &decode_error) -> bool {
            const std::uint32_t index = contiguous_decode
                ? contiguous_first + static_cast<std::uint32_t>(i) : unique_indices[i];
            if (fast_0115_raw != nullptr) {
                return decode_model_vertex_0115_for_gpu_fast(
                    memory, vertex_address + index * layout.stride, layout, transform, commands,
                    cpu_lighting_effective, cpu_lighting_effective ? &prepared_lighting : nullptr,
                    decoded_vertices[i], decode_error,
                    fast_0115_raw + i * static_cast<std::size_t>(layout.stride),
                    fast_0115_world_normal_ptr);
            }
            return decode_model_vertex_for_gpu(memory, vertex_address + index * layout.stride,
                                               layout, transform, commands,
                                               cpu_lighting_effective,
                                               cpu_lighting_effective ? &prepared_lighting : nullptr,
                                               uv_generation, decoded_vertices[i], decode_error);
        };

        RowWorkerPool &decode_pool = RowWorkerPool::instance();
        const bool expensive_vertex = cpu_lighting_effective || layout.weight_type != 0u;
        const bool parallel_decode = parallel_vertex_decode_enabled() &&
            decode_pool.worker_count() > 1u && decode_count >=
                parallel_vertex_decode_threshold(expensive_vertex) &&
            decode_count <= static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max());
        if (parallel_decode) {
            std::atomic<bool> decode_failed{false};
            std::array<std::string, RowWorkerPool::kMaxThreads> decode_errors{};
            const std::size_t vertices_per_participant = expensive_vertex ? 64u : 128u;
            const unsigned useful_participants = std::max(2u, std::min<unsigned>(
                std::min(decode_pool.worker_count(), parallel_vertex_decode_max_participants()),
                static_cast<unsigned>((decode_count + vertices_per_participant - 1u) /
                                      vertices_per_participant)));
            decode_pool.run(0, static_cast<std::int32_t>(decode_count - 1u),
                [&](unsigned participant, std::int32_t first, std::int32_t last) {
                    std::string &local_error = decode_errors[participant];
                    for (std::int32_t row = first; row <= last; ++row) {
                        if (decode_failed.load(std::memory_order_relaxed)) break;
                        if (!decode_one(static_cast<std::size_t>(row), local_error)) {
                            decode_failed.store(true, std::memory_order_relaxed);
                            break;
                        }
                    }
                }, useful_participants);
            if (decode_failed.load(std::memory_order_relaxed)) {
                for (const std::string &local_error : decode_errors) {
                    if (!local_error.empty()) { error = local_error; break; }
                }
                if (error.empty()) error = "parallel hardware vertex decode failed";
                return false;
            }
        } else {
            for (std::size_t i = 0u; i < decode_count; ++i) {
                if (!decode_one(i, error)) return false;
            }
        }
        if (collect_diagnostic_stats) stats.decoded_vertices += decoded_vertices.size();
        }

        const std::uint32_t alpha_control = pack_gpu_alpha_control(effective_draw);
        const std::uint32_t fog_control = pack_gpu_fog_control(effective_draw);
        std::uint32_t transform_control = 1u;
        if (hw.cull_enabled) transform_control |= 2u;
        if (hw.accept_counter_clockwise) transform_control |= 4u;
        if (hw.depth_clip_enabled) transform_control |= 8u;

        auto packed_to_color = [](std::uint32_t rgba) noexcept {
            return Color{static_cast<std::uint8_t>(rgba & 0xFFu),
                         static_cast<std::uint8_t>((rgba >> 8u) & 0xFFu),
                         static_cast<std::uint8_t>((rgba >> 16u) & 0xFFu),
                         static_cast<std::uint8_t>((rgba >> 24u) & 0xFFu)};
        };
        auto finalize_vertex = [&](GeGpuVertex vertex) {
            Color color = packed_to_color(vertex.rgba);
            if (gpu_force_white_vertex_colors_enabled()) {
                color = Color{255u, 255u, 255u, 255u};
            } else if (gpu_geometry_debug_colors_enabled()) {
                color = gpu_draw_debug_color(effective_draw);
            } else if (effective_draw.texture_enabled && !sampled_texture_ready) {
                color = gpu_texture_debug_color(effective_draw, color);
            }
            vertex.rgba = pack_gpu_color(color);
            vertex.alpha_control = alpha_control;
            vertex.texture_control = 0u;  // backend fills current texture function.
            vertex.texture_env = effective_draw.texture_env;
            vertex.fog_control = fog_control;
            vertex.transform_control = transform_control;
            return vertex;
        };

        // Stage 43: smooth shading no longer copies the entire decoded vertex
        // vector into a second submission vector. The decoded storage is private
        // to this draw, so finalize it in place. Flat shading still needs
        // triangle-local duplication because provoking-vertex colour semantics
        // intentionally assign one colour to all three compact model vertices.
        if (!flat_shading) {
            for (GeGpuVertex &vertex : decoded_vertices)
                vertex = finalize_vertex(vertex);

            // The most common model path -- non-indexed PSP TRIANGLES -- is
            // already an exact triangle stream. Do not manufacture and upload
            // 0,1,2,3,... uint32 indices merely to call vkCmdDrawIndexed. An
            // empty index span tells the backend to merge/submit it with vkCmdDraw.
            if (direct_nonindexed_gpu_draw_enabled() &&
                !indexed && primitive == 3u && (count % 3u) == 0u &&
                decoded_vertices.size() == count) {
                if (collect_diagnostic_stats) stats.triangles += count / 3u;
                {
                    PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
                    ge_gpu_backend_accumulate_hardware_triangles(
                        effective_draw, hw, decoded_vertices, {});
                }
                advance_stream();
                return true;
            }
        }
        const auto occurrence_index = [&](std::size_t i) -> std::uint32_t {
            if (!indexed) return static_cast<std::uint32_t>(i);
            if (contiguous_decode) return occurrence_indices[i] - contiguous_first;
            return occurrence_remap[i];
        };
        auto emit_triangle = [&](std::uint32_t ia, std::uint32_t ib, std::uint32_t ic) {
            if (flat_shading) {
                GeGpuVertex a = decoded_vertices[ia];
                GeGpuVertex b = decoded_vertices[ib];
                GeGpuVertex c = decoded_vertices[ic];
                const std::uint32_t flat_color = c.rgba;
                a.rgba = b.rgba = c.rgba = flat_color;
                const std::uint32_t base = static_cast<std::uint32_t>(submitted_vertices.size());
                submitted_vertices.push_back(finalize_vertex(a));
                submitted_vertices.push_back(finalize_vertex(b));
                submitted_vertices.push_back(finalize_vertex(c));
                triangle_indices.push_back(base + 0u);
                triangle_indices.push_back(base + 1u);
                triangle_indices.push_back(base + 2u);
                ++stats.flat_shaded_primitives;
            } else {
                triangle_indices.push_back(ia);
                triangle_indices.push_back(ib);
                triangle_indices.push_back(ic);
            }
            if (collect_diagnostic_stats) ++stats.triangles;
        };

        const std::size_t triangle_count = primitive == 3u ? count / 3u
            : (count > 2u ? count - 2u : 0u);
        triangle_indices.reserve(triangle_count * 3u);
        if (flat_shading) submitted_vertices.reserve(triangle_count * 3u);
        if (primitive == 3u) {
            for (std::size_t i = 0u; i + 2u < count; i += 3u)
                emit_triangle(occurrence_index(i), occurrence_index(i + 1u),
                              occurrence_index(i + 2u));
        } else if (primitive == 4u) {
            for (std::size_t i = 0u; i + 2u < count; ++i) {
                if ((i & 1u) == 0u)
                    emit_triangle(occurrence_index(i), occurrence_index(i + 1u),
                                  occurrence_index(i + 2u));
                else
                    emit_triangle(occurrence_index(i + 1u), occurrence_index(i),
                                  occurrence_index(i + 2u));
            }
        } else {
            for (std::size_t i = 1u; i + 1u < count; ++i)
                emit_triangle(occurrence_index(0u), occurrence_index(i),
                              occurrence_index(i + 1u));
        }

        {
            PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
            const std::span<const GeGpuVertex> upload_vertices = flat_shading
                ? std::span<const GeGpuVertex>(submitted_vertices)
                : std::span<const GeGpuVertex>(decoded_vertices);
            // This fallback explicitly materializes triangles above (including
            // strips/fans), so submit it as a list. Packed/raw fast paths return
            // earlier and preserve native strip topology.
            GeGpuHardwareTransform fallback_hw = hw;
            fallback_hw.primitive = 3u;
            ge_gpu_backend_accumulate_hardware_triangles(
                effective_draw, fallback_hw, upload_vertices, triangle_indices);
        }
        advance_stream();
        return true;
    }

    // Legacy/through/software paths really do consume FragmentSetup.  Resolve
    // and bind it only after the hardware-transform early return above.
    FragmentSetup setup = make_fragment_setup_cached(commands);
    {
        PhaseTimer bind_timer(g_ge_draw_setup_ns);
        bind_fragment_buffers(setup, memory, commands);
    }

    // Reused across draw calls.  A heavy VCS vblank issues about 1200 of them,
    // and each one used to allocate and free this vector plus the prepared
    // triangle list below.
    static thread_local std::vector<Vertex> vertices;
    {
        PhaseTimer vertex_timer(g_ge_vertex_decode_ns);
        vertices.clear();
        vertices.reserve(count);
        const IndexStreamReader draw_indices =
            make_index_reader(memory, index_address, layout.index_type, count);
        for (std::uint32_t i = 0u; i < count; ++i) {
            const std::uint32_t index = draw_indices(i);
            Vertex vertex{};
            if (!decode_vertex_optimized(memory, vertex_address + index * layout.stride, layout,
                                         commands, transform, vertex, error)) return false;
            record_clip_vertex(stats, vertex);
            if (layout.through) record_screen_vertex(stats, vertex);
            vertices.push_back(vertex);
        }
    }

    // Widescreen: shrink the 2D interface back into the game's 16:9 box.
    //
    // Ported from ThirteenAG's WidescreenFixesPack, plugin
    // GTAVCS.WidescreenFix -- MIT License, Copyright (c) 2018 ThirteenAG.
    // His CRect::CRect hook does the same arithmetic on the game's own HUD
    // rectangles, full-width ones exempted so backdrops keep covering.
    //
    // Here rather than in the backend because both renderers read this vector.
    // The software rasterizer keeps filling guest VRAM for targets the GPU does
    // not own, and VCS spends every other vblank replaying that VRAM through
    // composition quads -- so a correction applied only to the Vulkan vertices
    // produced two HUDs, corrected and stretched, alternating at 30 Hz.
    if (layout.through && gpu_backend_enabled && !vertices.empty()) {
        float min_x = vertices.front().x;
        float max_x = vertices.front().x;
        float max_y = vertices.front().y;
        for (const Vertex &vertex : vertices) {
            min_x = std::min(min_x, vertex.x);
            max_x = std::max(max_x, vertex.x);
            max_y = std::max(max_y, vertex.y);
        }
        ge_gpu_backend_note_through_extent(gpu_draw, max_x, max_y);

        const GeGpuWidescreenHud hud = ge_gpu_backend_widescreen_hud(gpu_draw);
        // Full-width draws are backdrops, fades and letterbox bars: they have to
        // keep covering the screen, so they stay stretched. Measured against the
        // real 480 px display, not against this target's own size.
        constexpr float kFullWidthFraction = 0.9f;
        const bool full_width = (max_x - min_x) * hud.display_scale_x >=
            kFullWidthFraction * 480.0f;
        // The ThirteenAG rectangle correction is a HUD/surface transform, not a
        // generic transform for every through-mode primitive.  Stage 45.3 also
        // moved POINTS/LINES/LINE_STRIP (0..2); VCS uses tiny helper primitives
        // around the radar and that exposed the grey line + magenta point seen
        // above the minimap.  Keep those auxiliary primitives in PSP coordinates
        // and only correct filled geometry (triangles/strips/fans/sprites).
        const bool widescreen_surface_primitive = primitive >= 3u && primitive <= 6u;
        if (widescreen_surface_primitive && hud.shrink != 1.0f && !setup.clear_mode && !full_width &&
            // Additive depth-tested 2D is not interface, it is the world drawn
            // in screen space: coronas, headlight glows, lens flares. The guest
            // already projected those from world positions, so they are correct
            // where they are. Shrinking them toward the middle of the screen
            // displaces each one in proportion to its distance from the centre,
            // and the shrunk clip rectangle then cuts the ones near the edges.
            //
            // Both conditions are needed. The depth test alone also describes
            // the radar, which is depth-tested and *is* interface -- exempting
            // it left the map unshrunk inside a shrunk frame. What separates the
            // two is the blend: a glow adds light with a fixed destination
            // factor, while the radar composites with ordinary source alpha.
            !(setup.depth_test_enabled && !setup.depth_write_enabled) &&
            // The composition quads are through-mode too, and they carry the
            // world. Shrinking them would pillarbox the picture and undo the
            // widened frustum instead of complementing it.
            !(setup.texture_enabled && ge_gpu_backend_is_framebuffer_feedback_texture(gpu_draw))) {
            for (Vertex &vertex : vertices)
                vertex.x = hud.source_center + (vertex.x - hud.source_center) / hud.shrink;
            // The clip rectangle has to move with the geometry, or the radar
            // keeps being masked where the radar used to be. Both renderers read
            // their scissor from here.
            // Rounded inwards, both ends: the rectangle is integral and the
            // shrunk geometry is not, so rounding to nearest can leave the clip
            // up to half a pixel wider than the quad it is meant to contain.
            // That slack showed as a one-pixel column of the radar map leaking
            // out either side of the radar's circular frame. Losing a pixel of
            // an edge that a mask covers anyway is the harmless direction.
            const auto shrink_scissor = [&](std::int32_t value, bool leading) {
                const float moved = hud.source_center +
                    (static_cast<float>(value) - hud.source_center) / hud.shrink;
                return static_cast<std::int32_t>(leading ? std::ceil(moved)
                                                         : std::floor(moved));
            };
            setup.scissor_x0 = shrink_scissor(setup.scissor_x0, true);
            setup.scissor_x1 = shrink_scissor(setup.scissor_x1, false);
            gpu_draw.scissor_x0 = setup.scissor_x0;
            gpu_draw.scissor_x1 = setup.scissor_x1;
            gpu_draw.widescreen_hud = true;
        }
    }

    // Stage 22 relic: every triangle-stream vertex was converted and copied into
    // the mapped Vulkan upload ring, and nothing ever read those bytes.  The
    // frame Vulkan draws is assembled from the prepared screen triangles below,
    // and finish_color_frame overwrites the very same region with them.  It cost
    // one allocation and one full vertex conversion per draw call plus about
    // 2 MB per frame of write-combined memcpy, all discarded.
    if (legacy_vertex_staging_enabled() && gpu_backend_enabled &&
        ge_gpu_backend_transfer_ready() &&
        primitive >= 3u && primitive <= 5u && !gpu_draw.clear_mode) {
        PhaseTimer stage_timer(g_ge_gpu_stage_ns);
        std::vector<GeGpuVertex> gpu_vertices;
        gpu_vertices.reserve(vertices.size());
        for (const Vertex &vertex : vertices) {
            const std::uint32_t rgba =
                static_cast<std::uint32_t>(vertex.color.r) |
                (static_cast<std::uint32_t>(vertex.color.g) << 8u) |
                (static_cast<std::uint32_t>(vertex.color.b) << 16u) |
                (static_cast<std::uint32_t>(vertex.color.a) << 24u);
            gpu_vertices.push_back(GeGpuVertex{
                vertex.x, vertex.y, vertex.z, vertex.w, rgba, vertex.u, vertex.v,
                pack_gpu_alpha_control(gpu_draw), 0u, gpu_draw.texture_env,
                vertex.fog_factor, pack_gpu_fog_control(gpu_draw), vertex.q,
            });
        }
        (void)ge_gpu_backend_stage_vertices(gpu_draw, gpu_vertices);
    }

    switch (primitive) {
    case 0u:
        for (const Vertex &vertex : vertices) {
            rasterize_point(memory, commands, setup, vertex, layout.through, stats);
            if (collect_diagnostic_stats) ++stats.points;
        }
        break;
    case 1u:
        for (std::size_t i = 0u; i + 1u < vertices.size(); i += 2u) {
            rasterize_line(memory, commands, setup, vertices[i], vertices[i + 1u], layout.through, stats);
            if (collect_diagnostic_stats) ++stats.lines;
        }
        break;
    case 2u:
        for (std::size_t i = 0u; i + 1u < vertices.size(); ++i) {
            rasterize_line(memory, commands, setup, vertices[i], vertices[i + 1u], layout.through, stats);
            if (collect_diagnostic_stats) ++stats.lines;
        }
        break;
    case 3u: {
        static thread_local std::vector<PreparedScreenTriangle> triangles; triangles.clear();
        {
            PhaseTimer prep_timer(g_ge_triangle_prep_ns);
            triangles.reserve(vertices.size() / 3u + 2u);
            for (std::size_t i = 0u; i + 2u < vertices.size(); i += 3u) {
                append_prepared_triangles(commands, setup,
                                          vertices[i], vertices[i + 1u], vertices[i + 2u],
                                          vertices[i + 2u], layout.through, stats, triangles, gpu_only_triangle_path);
                if (collect_diagnostic_stats) ++stats.triangles;
            }
        }
        if (gpu_backend_enabled) {
            PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
            accumulate_gpu_prepared_triangles(gpu_draw, triangles);
        }
        rasterize_prepared_triangles(memory, commands, setup, triangles, stats);
        break;
    }
    case 4u: {
        static thread_local std::vector<PreparedScreenTriangle> triangles; triangles.clear();
        {
            PhaseTimer prep_timer(g_ge_triangle_prep_ns);
            triangles.reserve(vertices.size() + 2u);
            for (std::size_t i = 0u; i + 2u < vertices.size(); ++i) {
                if ((i & 1u) == 0u)
                    append_prepared_triangles(commands, setup,
                                              vertices[i], vertices[i + 1u], vertices[i + 2u],
                                              vertices[i + 2u], layout.through, stats, triangles, gpu_only_triangle_path);
                else
                    append_prepared_triangles(commands, setup,
                                              vertices[i + 1u], vertices[i], vertices[i + 2u],
                                              vertices[i + 2u], layout.through, stats, triangles, gpu_only_triangle_path);
                if (collect_diagnostic_stats) ++stats.triangles;
            }
        }
        if (gpu_backend_enabled) {
            PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
            accumulate_gpu_prepared_triangles(gpu_draw, triangles);
        }
        rasterize_prepared_triangles(memory, commands, setup, triangles, stats);
        break;
    }
    case 5u: {
        static thread_local std::vector<PreparedScreenTriangle> triangles; triangles.clear();
        {
            PhaseTimer prep_timer(g_ge_triangle_prep_ns);
            triangles.reserve(vertices.size() + 2u);
            for (std::size_t i = 1u; i + 1u < vertices.size(); ++i) {
                append_prepared_triangles(commands, setup,
                                          vertices[0], vertices[i], vertices[i + 1u],
                                          vertices[i + 1u], layout.through, stats, triangles, gpu_only_triangle_path);
                if (collect_diagnostic_stats) ++stats.triangles;
            }
        }
        if (gpu_backend_enabled) {
            PhaseTimer accumulate_timer(g_ge_gpu_accumulate_ns);
            accumulate_gpu_prepared_triangles(gpu_draw, triangles);
        }
        rasterize_prepared_triangles(memory, commands, setup, triangles, stats);
        break;
    }
    case 6u:
        for (std::size_t i = 0u; i + 1u < vertices.size(); i += 2u) {
            if (gpu_backend_enabled)
                accumulate_gpu_rectangle(gpu_draw, vertices[i], vertices[i + 1u]);
            rasterize_rectangle(memory, commands, setup, vertices[i], vertices[i + 1u], stats);
            if (collect_diagnostic_stats) ++stats.rectangles;
        }
        break;
    default:
        if (collect_diagnostic_stats) ++stats.unsupported_primitives;
        break;
    }
    advance_stream();
    return true;
}

GePhaseTotals ge_phase_totals() noexcept {
    return GePhaseTotals{
        g_ge_pixel_ns, g_ge_triangle_count,
        g_ge_draw_setup_ns, g_ge_texture_upload_ns, g_ge_vertex_decode_ns,
        g_ge_gpu_stage_ns, g_ge_triangle_prep_ns, g_ge_gpu_accumulate_ns,
        g_ge_primitive_count, g_ge_vertex_count,
    };
}

void reset_ge_phase_totals() noexcept {
    g_ge_pixel_ns = 0u;
    g_ge_triangle_count = 0u;
    g_ge_draw_setup_ns = 0u;
    g_ge_texture_upload_ns = 0u;
    g_ge_vertex_decode_ns = 0u;
    g_ge_gpu_stage_ns = 0u;
    g_ge_triangle_prep_ns = 0u;
    g_ge_gpu_accumulate_ns = 0u;
    g_ge_primitive_count = 0u;
    g_ge_vertex_count = 0u;
}

} // namespace vcs
