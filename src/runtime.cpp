#include "psprecomp/runtime.hpp"
#include "psprecomp/common.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace psprecomp {

// See Runtime::invoke_chained_direct(). Diagnostics deliberately make this
// sticky: once instrumentation has been requested, correctness matters more
// than returning to the production fast path later in the process.
bool g_runtime_chain_observers_active = false;
std::uint64_t g_runtime_starvation_interval_fast = 0u;
std::uint64_t g_runtime_thread_switch_generation_fast = 0u;

namespace {
using RuntimePostImportHook = void (*)(Runtime &, AllegrexContext &);
RuntimePostImportHook g_post_import_hook = nullptr;
std::int32_t g_runtime_thread_uid = -1;
std::array<char, 64> g_runtime_thread_name{};
std::uint32_t g_runtime_dispatch_pc = 0u;
RuntimeHeartbeatHook g_heartbeat_hook = nullptr;
std::uint64_t g_heartbeat_interval = 0u;
RuntimeStarvationHook g_starvation_hook = nullptr;
std::uint64_t g_starvation_interval = 0u;
RuntimePreDispatchHook g_pre_dispatch_hook = nullptr;
RuntimePostDispatchHook g_post_dispatch_hook = nullptr;
RuntimePreChainedCallHook g_pre_chained_call_hook = nullptr;
RuntimePostChainedCallHook g_post_chained_call_hook = nullptr;
}

// Execution counter for a handful of guest addresses, armed by
// PSPRECOMP_COUNT_PC as a comma-separated list.
//
// Answering "is this routine ever reached?" needed an instrument that costs
// nothing: the existing chain tracer pushes a frame on every chained call and
// perturbed the run enough that it died before reaching the window under
// investigation.  This is a linear scan over at most eight addresses, and it
// covers both entry paths -- the outer dispatch loop and invoke_chained_call --
// because a routine reached only through chaining never appears in the outer
// dispatch profile.
namespace {
constexpr std::size_t kMaxCountedPcs = 8u;
std::array<std::uint32_t, kMaxCountedPcs> g_counted_pcs{};
std::array<std::uint64_t, kMaxCountedPcs> g_counted_pc_hits{};
std::size_t g_counted_pc_size = 0u;
bool g_counted_pcs_loaded = false;

void load_counted_pcs() {
    if (g_counted_pcs_loaded) return;
    g_counted_pcs_loaded = true;
    const char *text = std::getenv("PSPRECOMP_COUNT_PC");
    if (text == nullptr || *text == '\0') return;
    const std::string list(text);
    std::size_t begin = 0u;
    while (begin < list.size() && g_counted_pc_size < kMaxCountedPcs) {
        const std::size_t comma = list.find(',', begin);
        const std::string item = list.substr(begin, comma == std::string::npos ? std::string::npos : comma - begin);
        char *end = nullptr;
        const unsigned long value = std::strtoul(item.c_str(), &end, 0);
        if (end != item.c_str() && *end == '\0') g_counted_pcs[g_counted_pc_size++] = static_cast<std::uint32_t>(value);
        if (comma == std::string::npos) break;
        begin = comma + 1u;
    }
    if (g_counted_pc_size != 0u) g_runtime_chain_observers_active = true;
}

inline void count_pc(std::uint32_t pc) noexcept {
    for (std::size_t index = 0u; index < g_counted_pc_size; ++index)
        if (g_counted_pcs[index] == pc) { ++g_counted_pc_hits[index]; return; }
}
}

void report_counted_pcs() {
    for (std::size_t index = 0u; index < g_counted_pc_size; ++index)
        std::cerr << "[count-pc] pc=" << hex32(g_counted_pcs[index])
                  << " hits=" << g_counted_pc_hits[index] << "\n";
}

void set_runtime_heartbeat_hook(RuntimeHeartbeatHook hook, std::uint64_t interval) noexcept {
    g_heartbeat_hook = interval != 0u ? hook : nullptr;
    g_heartbeat_interval = interval;
}

void set_runtime_starvation_hook(RuntimeStarvationHook hook, std::uint64_t interval) noexcept {
    g_starvation_hook = interval != 0u ? hook : nullptr;
    g_starvation_interval = interval;
    g_runtime_starvation_interval_fast = g_starvation_hook != nullptr ? interval : 0u;
}

void set_runtime_pre_dispatch_hook(RuntimePreDispatchHook hook) noexcept {
    g_pre_dispatch_hook = hook;
}
void set_runtime_post_dispatch_hook(RuntimePostDispatchHook hook) noexcept {
    g_post_dispatch_hook = hook;
}
void set_runtime_pre_chained_call_hook(RuntimePreChainedCallHook hook) noexcept {
    g_pre_chained_call_hook = hook;
    if (hook != nullptr) g_runtime_chain_observers_active = true;
}
void set_runtime_post_chained_call_hook(RuntimePostChainedCallHook hook) noexcept {
    g_post_chained_call_hook = hook;
    if (hook != nullptr) g_runtime_chain_observers_active = true;
}

void set_runtime_post_import_hook(RuntimePostImportHook hook) noexcept { g_post_import_hook = hook; }
void set_runtime_thread_identity(std::int32_t uid, const std::string &name) noexcept {
    if (uid != g_runtime_thread_uid) ++g_runtime_thread_switch_generation_fast;
    g_runtime_thread_uid = uid;
    g_runtime_thread_name.fill('\0');
    const std::size_t count = std::min(name.size(), g_runtime_thread_name.size() - 1u);
    std::memcpy(g_runtime_thread_name.data(), name.data(), count);
}
std::int32_t runtime_thread_uid() noexcept { return g_runtime_thread_uid; }
const char *runtime_thread_name() noexcept { return g_runtime_thread_name.data(); }
std::uint32_t runtime_dispatch_pc() noexcept { return g_runtime_dispatch_pc; }
RuntimeExecutionContextToken capture_runtime_execution_context() noexcept {
    return RuntimeExecutionContextToken{g_runtime_thread_uid, g_runtime_thread_switch_generation_fast};
}
bool runtime_execution_context_matches(RuntimeExecutionContextToken token) noexcept {
    return token.thread_uid == g_runtime_thread_uid &&
           token.switch_generation == g_runtime_thread_switch_generation_fast;
}
std::uint64_t runtime_thread_switch_generation() noexcept {
    return g_runtime_thread_switch_generation_fast;
}
bool runtime_thread_switch_generation_matches(std::uint64_t generation) noexcept {
    return generation == g_runtime_thread_switch_generation_fast;
}

Runtime::Runtime(std::uint32_t ram_size) : memory_(ram_size) {
    // Most commercial PSP titles use a few hundred import stubs. Seed a small
    // binding table so first use of a late-numbered import does not reallocate
    // in the middle of guest execution; larger profiles can still grow it.
    import_bindings_.resize(256u, nullptr);
    hle_histogram_enabled_ = std::getenv("PSPRECOMP_HLE_HISTOGRAM") != nullptr;
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    track_dispatch_counters_ = false;
#else
    track_dispatch_counters_ =
        std::getenv("PSPRECOMP_REPORT_DISPATCH_COUNT") != nullptr ||
        std::getenv("PSPRECOMP_PROFILE_DISPATCH") != nullptr ||
        std::getenv("PSPRECOMP_COUNT_PC") != nullptr;
#endif
    // PSPRECOMP_NO_CHAIN disables cross-unit chaining outright;
    // PSPRECOMP_CHAIN_DEPTH tunes how deep it may nest without a rebuild.
    chain_depth_limit_ = 48u;
    if (const char *depth = std::getenv("PSPRECOMP_CHAIN_DEPTH")) {
        char *end = nullptr;
        const unsigned long value = std::strtoul(depth, &end, 0);
        if (end != depth && *end == '\0' && value <= 4096u)
            chain_depth_limit_ = static_cast<std::uint32_t>(value);
    }
    if (std::getenv("PSPRECOMP_NO_CHAIN") != nullptr) chain_depth_limit_ = 0u;
}

namespace {
// The six registers the dispatch error messages print are rarely enough to tell
// which operand produced a bad guest address.  Note that the hot-register cache
// (r2/r4-r7/r29/r31) is only materialized back into AllegrexContext by the
// generated outer wrapper on a normal return -- an exception unwinds past that
// flush, so exactly those six read stale here.  Every other register, including
// the callee-saved ones, is current.
void append_gpr_dump(std::ostringstream &message, const AllegrexContext &ctx) {
    static const char *const kGprNames[32] = {
        "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};
    message << "\n  gpr:";
    for (std::size_t index = 0u; index < 32u; ++index) {
        if (index % 8u == 0u) message << "\n   ";
        message << ' ' << kGprNames[index] << '=' << hex32(ctx.gpr[index]);
    }
    message << "\n  (r2/r4-r7/r29/r31 may be stale: hot-register cache not flushed on unwind)";
}
}

bool Runtime::run_starvation_boundary(AllegrexContext &ctx) {
    const std::uint64_t interval = g_runtime_starvation_interval_fast;
    if (g_starvation_hook == nullptr || interval == 0u) return true;
    // Preserve excess work if a context switch deferred several boundaries.
    dispatches_since_import_ -= std::min(dispatches_since_import_, interval);
    const RuntimeExecutionContextToken before = capture_runtime_execution_context();
    g_starvation_hook(*this, ctx);
    const bool same_context = runtime_execution_context_matches(before);
    if (!same_context) chain_context_invalidated_ = true;
    return same_context;
}

bool Runtime::account_dispatch_work(AllegrexContext &ctx, bool allow_preemption) {
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    if (track_dispatch_counters_) ++dispatch_work_count_;
#endif
    const std::uint64_t interval = g_runtime_starvation_interval_fast;
    if (interval == 0u) return true;
    ++dispatches_since_import_;
    if (!allow_preemption || dispatches_since_import_ < interval) return true;
    return run_starvation_boundary(ctx);
}

bool Runtime::invoke_chained_call(AllegrexContext &ctx, GuestMemory::AotFastView *shared_aot_mem) {
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    count_pc(ctx.pc);
#endif
    if (chain_depth_ >= chain_depth_limit_) return false;

    // Indirect jalr/vcall targets usually land in ordinary generated code too.
    // Prefer the tiny dense unit table. Units overlapped by a host/import
    // replacement are poisoned and fall back to exact PC lookup. Ownership is
    // still guarded on every native chain frame because a nested descendant
    // can cross the scheduler boundary even when this immediate unit is clean.
    // dynamic JR/JALR chains used to canonicalize the same target
    // twice: once in lookup_generated_unit() and again to locate the compact
    // direct-entry function. Do the dense generated-unit probe once and reuse
    // its unit index for both function and entry lookup.
    RecompiledFunction function = nullptr;
    RecompiledEntryFunction entry_function = nullptr;
    if (generated_unit_layout_valid_ && generated_unit_span_ != 0u) {
        const std::uint32_t canonical_pc = memory_.canonical(ctx.pc);
        if (canonical_pc >= generated_unit_base_) {
            const std::uint32_t unit_delta = canonical_pc - generated_unit_base_;
            const std::uint32_t unit_index = generated_unit_span_ == 16384u
                ? (unit_delta >> 14u) : (unit_delta / generated_unit_span_);
            if (unit_index < kGeneratedUnitFastCapacity) {
                function = generated_units_[unit_index];
                if (function != nullptr && shared_aot_mem != nullptr &&
                    generated_unit_disabled_[unit_index] == 0u)
                    entry_function = generated_unit_entries_[unit_index];
            }
        }
    }
    if (function == nullptr) {
        const std::uint32_t delta = memory_.canonical(ctx.pc) - direct_base_;
        if ((delta & 3u) != 0u) return false;
        const std::size_t index = static_cast<std::size_t>(delta) / 4u;
        if (index >= direct_chainable_.size()) return false;
        function = direct_chainable_[index];
        if (function == nullptr) return false;
    }

    const std::uint32_t target_pc = ctx.pc;
    const std::uint32_t native_depth = chain_depth_;
    // Always guard execution-context ownership. Even a clean generated unit can
    // reach a nested direct chain whose scheduler boundary switches PSP thread.
    // guarded every native chain frame; accidentally weakened
    // that invariant for the dense generated-unit path.
    // Generation alone is sufficient: it increments on every PSP thread
    // ownership change. Avoid constructing/checking a two-field token on every
    // dynamic native chain boundary in the city hot path.
    const std::uint64_t caller_generation = g_runtime_thread_switch_generation_fast;
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    if (g_pre_chained_call_hook != nullptr)
        g_pre_chained_call_hook(*this, ctx, target_pc, native_depth);
#endif
    struct DepthGuard {
        std::uint32_t &depth;
        explicit DepthGuard(std::uint32_t &value) : depth(value) { ++depth; }
        ~DepthGuard() { --depth; }
    } guard(chain_depth_);
    if (entry_function != nullptr && shared_aot_mem != nullptr) {
        entry_function(*this, ctx, 0u, *shared_aot_mem);
    } else {
        function(*this, ctx);
    }
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    if (g_post_chained_call_hook != nullptr)
        g_post_chained_call_hook(*this, ctx, target_pc, native_depth);
    if (track_dispatch_counters_) ++chained_dispatches_;
#endif

    // Dynamic targets may enter a profile/native function which switches PSP
    // ownership without passing through run_starvation_boundary(). Keep the
    // generation guard here even in production; compile-time direct chains use
    // chain_context_invalidated_ and retain their cheaper hot path.
    if (caller_generation != g_runtime_thread_switch_generation_fast) {
        (void)account_dispatch_work(ctx, false);
        return false;
    }
    return account_dispatch_work(ctx, true);
}

bool Runtime::invoke_chained_unit(AllegrexContext &ctx, std::uint32_t unit_index,
                                  GuestMemory::AotFastView *shared_aot_mem) {
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    count_pc(ctx.pc);
#endif
    if (chain_depth_ >= chain_depth_limit_) return false;
    if (unit_index >= kGeneratedUnitFastCapacity || !generated_unit_layout_valid_) return false;
    RecompiledFunction function = generated_units_[unit_index];
    if (function == nullptr) return false;

    const std::uint32_t target_pc = ctx.pc;
    const std::uint32_t native_depth = chain_depth_;
    const std::uint64_t caller_generation = g_runtime_thread_switch_generation_fast;
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    if (g_pre_chained_call_hook != nullptr)
        g_pre_chained_call_hook(*this, ctx, target_pc, native_depth);
#endif
    struct DepthGuard {
        std::uint32_t &depth;
        explicit DepthGuard(std::uint32_t &value) : depth(value) { ++depth; }
        ~DepthGuard() { --depth; }
    } guard(chain_depth_);
    if (shared_aot_mem != nullptr && generated_unit_entries_[unit_index] != nullptr) {
        generated_unit_entries_[unit_index](*this, ctx, 0u, *shared_aot_mem);
    } else {
        function(*this, ctx);
    }
#if !defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    if (g_post_chained_call_hook != nullptr)
        g_post_chained_call_hook(*this, ctx, target_pc, native_depth);
    if (track_dispatch_counters_) ++chained_dispatches_;
#endif

    // A descendant scheduler boundary may have switched the PSP context while
    // this frame was active. Never let a stale native caller resume it.
    if (caller_generation != g_runtime_thread_switch_generation_fast) {
        (void)account_dispatch_work(ctx, false);
        return false;
    }
    return account_dispatch_work(ctx, true);
}

void Runtime::register_generated_unit(std::uint32_t unit_index,
                                      std::uint32_t unit_address,
                                      std::uint32_t unit_span,
                                      RecompiledFunction function,
                                      RecompiledEntryFunction entry_function) {
    if (function == nullptr || unit_span == 0u || !generated_unit_layout_valid_) return;
    if (unit_index >= kGeneratedUnitFastCapacity) return;
    const std::uint32_t canonical_address = memory_.canonical(unit_address);
    const std::uint64_t offset = static_cast<std::uint64_t>(unit_index) * unit_span;
    if (canonical_address < offset) return;
    const std::uint32_t base = canonical_address - static_cast<std::uint32_t>(offset);
    if (generated_unit_span_ == 0u) {
        generated_unit_base_ = base;
        generated_unit_span_ = unit_span;
    } else if (generated_unit_base_ != base || generated_unit_span_ != unit_span) {
        // Mixed automatic-codegen layouts are unsupported. Permanently disable
        // the optional dense path for this Runtime; exact PC dispatch remains.
        generated_units_.fill(nullptr);
        generated_unit_disabled_.fill(0u);
        generated_unit_base_ = 0u;
        generated_unit_span_ = 0u;
        generated_unit_layout_valid_ = false;
        return;
    }
    if (generated_unit_disabled_[unit_index] == 0u)
        generated_units_[unit_index] = function;
    generated_unit_entries_[unit_index] = entry_function;
}

bool Runtime::invoke_isolated_aot(std::uint32_t address, AllegrexContext &ctx) {
    const RecompiledFunction function = lookup_function(address);
    if (function == nullptr) return false;
    ctx.pc = address;
    function(*this, ctx);
    ctx.gpr[0] = 0u;
    return true;
}

std::vector<std::pair<std::string, std::uint64_t>> Runtime::hle_histogram() const {
    std::vector<std::pair<std::string, std::uint64_t>> entries(hle_histogram_.begin(), hle_histogram_.end());
    std::sort(entries.begin(), entries.end(), [](const auto &left, const auto &right) {
        if (left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    return entries;
}

void Runtime::report_hle_histogram(std::size_t limit) const {
    if (!hle_histogram_enabled_) return;
    const auto entries = hle_histogram();
    std::uint64_t total = 0u;
    for (const auto &entry : entries) total += entry.second;
    std::cerr << "[hle-histogram] unique=" << entries.size() << " calls=" << total << "\n";
    for (std::size_t index = 0; index < std::min(limit, entries.size()); ++index) {
        const auto &entry = entries[index];
        std::cerr << "[hle-histogram] " << entry.first << " count=" << entry.second
                  << " name=" << nids_.resolve(entry.first.substr(0, entry.first.find(':')),
                                               static_cast<std::uint32_t>(
                                                   std::strtoul(entry.first.substr(entry.first.find(':') + 1u).c_str(),
                                                                nullptr, 16)))
                                        .value_or("unknown")
                  << "\n";
    }
}
NidRegistry &Runtime::nids() noexcept { return nids_; }
const NidRegistry &Runtime::nids() const noexcept { return nids_; }

void Runtime::register_function(std::uint32_t address, RecompiledFunction function, std::string name) {
    if (!function) throw Error("Attempted to register a null recompiled function");
    functions_[address] = FunctionEntry{function, std::move(name)};
    const std::uint32_t c = memory_.canonical(address);
    if ((c & 3u) != 0u || c < GuestMemory::kPhysicalBase) return;

    // The direct table covers only the window that actually holds registered
    // code.  Spanning the whole 32 MiB of guest RAM made it a 64 MiB array that
    // every outer dispatch probed at an effectively random offset, so the hot
    // path took a cache/TLB miss per dispatch.  Growing on demand keeps it at
    // the size of the executable image instead.
    constexpr std::size_t chunk = 1024u * 1024u / 4u;
    const auto shift_forward = [](std::vector<RecompiledFunction> &table, std::size_t slots) {
        std::vector<RecompiledFunction> grown(table.size() + slots, nullptr);
        std::copy(table.begin(), table.end(), grown.begin() + static_cast<std::ptrdiff_t>(slots));
        table = std::move(grown);
    };
    if (direct_functions_.empty()) {
        direct_base_ = c & ~0xFFFFFu;  // 1 MiB-aligned window start
        direct_functions_.assign(chunk, nullptr);
        direct_chainable_.assign(chunk, nullptr);
    } else if (c < direct_base_) {
        const std::size_t slots = static_cast<std::size_t>(direct_base_ - (c & ~0xFFFFFu)) / 4u;
        shift_forward(direct_functions_, slots);
        shift_forward(direct_chainable_, slots);
        direct_base_ = c & ~0xFFFFFu;
    }
    const std::size_t index = static_cast<std::size_t>(c - direct_base_) / 4u;
    if (index >= direct_functions_.size()) {
        const std::size_t grown = ((index / chunk) + 1u) * chunk;
        direct_functions_.resize(grown, nullptr);
        direct_chainable_.resize(grown, nullptr);
    }
    direct_functions_[index] = function;
    // Import wrappers and HLE entries are deliberately excluded: chaining must
    // never run one inline, because they are where thread switches happen.
    // Assign unconditionally -- an import stub can also be covered by a unit,
    // and a later import registration has to clear the earlier unit pointer or
    // chaining would keep running the raw code instead of the wrapper.
    const auto &registered = functions_[address].name;
    const bool chainable = registered.starts_with("recomp_unit_");
    direct_chainable_[index] = chainable ? function : nullptr;

    if (!chainable && generated_unit_span_ != 0u && c >= generated_unit_base_) {
        const std::uint32_t unit_delta = c - generated_unit_base_;
        const std::size_t unit_index = static_cast<std::size_t>(unit_delta / generated_unit_span_);
        if (unit_index < kGeneratedUnitFastCapacity) {
            generated_unit_disabled_[unit_index] = 1u;
            generated_units_[unit_index] = nullptr;
        }
    }
}

void Runtime::unregister_functions(std::uint32_t start, std::uint32_t end) {
    const std::uint32_t low = memory_.canonical(start);
    const std::uint32_t high = memory_.canonical(end);
    if (high <= low) return;
    std::erase_if(functions_, [&](const auto &entry) {
        const std::uint32_t c = memory_.canonical(entry.first);
        return c >= low && c < high;
    });
    for (std::uint32_t c = low & ~3u; c < high; c += 4u) {
        const std::uint32_t delta = c - direct_base_;
        const std::size_t index = static_cast<std::size_t>(delta) / 4u;
        if (c < direct_base_ || index >= direct_functions_.size()) continue;
        direct_functions_[index] = nullptr;
        direct_chainable_[index] = nullptr;
    }
    if (generated_unit_span_ != 0u && high > generated_unit_base_) {
        const std::uint32_t first = low > generated_unit_base_ ? (low - generated_unit_base_) / generated_unit_span_ : 0u;
        const std::uint32_t last = (high - 1u - generated_unit_base_) / generated_unit_span_;
        for (std::uint32_t unit = first; unit <= last && unit < kGeneratedUnitFastCapacity; ++unit) {
            generated_units_[unit] = nullptr;
            generated_unit_entries_[unit] = nullptr;
        }
    }
}

void Runtime::set_code_overlay_resolver(std::uint32_t low, std::uint32_t high,
                                        CodeOverlayResolver resolver) noexcept {
    code_overlay_low_ = GuestMemory::canonical(low);
    code_overlay_span_ = high > low ? GuestMemory::canonical(high) - code_overlay_low_ : 0u;
    code_overlay_resolver_ = resolver;
}

Runtime::RecompiledFunction Runtime::lookup_function(std::uint32_t address) const noexcept {
    const std::uint32_t c = memory_.canonical(address);
    const std::uint32_t delta = c - direct_base_;
    if ((delta & 3u) == 0u) {
        const std::size_t index = static_cast<std::size_t>(delta) / 4u;
        if (index < direct_functions_.size()) return direct_functions_[index];
    }
    const auto found = functions_.find(address);
    return found != functions_.end() ? found->second.function : nullptr;
}

Runtime::RecompiledFunction Runtime::lookup_generated_unit(std::uint32_t address) const noexcept {
    if (!generated_unit_layout_valid_ || generated_unit_span_ == 0u) return nullptr;
    const std::uint32_t c = memory_.canonical(address);
    if (c < generated_unit_base_) return nullptr;
    const std::uint32_t delta = c - generated_unit_base_;
    const std::size_t unit_index = generated_unit_span_ == 16384u
        ? static_cast<std::size_t>(delta >> 14u)
        : static_cast<std::size_t>(delta / generated_unit_span_);
    if (unit_index >= kGeneratedUnitFastCapacity) return nullptr;
    return generated_units_[unit_index];
}

const Runtime::FunctionEntry *Runtime::lookup_entry(std::uint32_t address) const noexcept {
    const auto found = functions_.find(address);
    return found != functions_.end() ? &found->second : nullptr;
}

std::string Runtime::hle_key(std::string_view library, std::uint32_t nid) {
    std::string key;
    key.reserve(library.size() + 11u);
    key.append(library);
    key.push_back(':');
    key.append(hex32(nid));
    return key;
}

void Runtime::register_hle(std::string library, std::uint32_t nid, HleFunction function) {
    hle_[std::move(library)][nid] = std::move(function);
}

bool Runtime::has_function(std::uint32_t address) const { return lookup_function(address) != nullptr; }
std::size_t Runtime::function_count() const noexcept { return functions_.size(); }

void Runtime::set_game_root(std::filesystem::path root) { game_root_ = std::filesystem::weakly_canonical(std::move(root)); }
const std::filesystem::path &Runtime::game_root() const noexcept { return game_root_; }

std::filesystem::path Runtime::translate_path(const std::string &psp_path) const {
    std::string relative = psp_path;
    const auto colon = relative.find(':');
    if (colon != std::string::npos) relative.erase(0, colon + 1u);
    while (!relative.empty() && (relative.front() == '/' || relative.front() == '\\')) relative.erase(relative.begin());
    std::replace(relative.begin(), relative.end(), '\\', '/');
    std::filesystem::path clean;
    for (const auto &part : std::filesystem::path(relative)) {
        if (part == "..") throw Error("Rejected PSP path traversal: " + psp_path);
        if (part != ".") clean /= part;
    }
    return game_root_ / clean;
}

void Runtime::run(std::uint32_t entry, std::uint64_t max_dispatches) {
    stopped_ = false;
    stop_reason_.clear();
    cpu_.pc = entry;
    load_counted_pcs();
    const bool profile_dispatch = std::getenv("PSPRECOMP_PROFILE_DISPATCH") != nullptr;
    const auto parse_environment_u64 = [](const char *name, std::uint64_t fallback = 0u) {
        const char *text = std::getenv(name);
        if (text == nullptr || *text == '\0') return fallback;
        char *end = nullptr;
        const unsigned long long value = std::strtoull(text, &end, 0);
        return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
    };
    const std::uint64_t profile_dispatch_start =
        parse_environment_u64("PSPRECOMP_PROFILE_DISPATCH_START");
    const std::uint64_t profile_dispatch_count =
        parse_environment_u64("PSPRECOMP_PROFILE_DISPATCH_COUNT");
    std::unordered_map<std::uint32_t, std::uint64_t> dispatch_counts;
    if (profile_dispatch) dispatch_counts.reserve(4096u);
    struct DispatchSnapshot {
        std::uint32_t pc{};
        std::uint32_t a0{};
        std::uint32_t a1{};
        std::uint32_t a2{};
        std::uint32_t a3{};
        std::uint32_t sp{};
        std::uint32_t ra{};
        std::int32_t thread_uid{-1};
        std::array<char, 64> thread_name{};
    };
    std::array<DispatchSnapshot, 64> recent_dispatches{};
    std::size_t recent_count = 0u;
    std::size_t recent_next = 0u;
    const bool trace_on_error = std::getenv("PSPRECOMP_TRACE_ON_ERROR") != nullptr;
    const bool world_stream_diag = std::getenv("PSPRECOMP_WORLD_STREAM_DIAG") != nullptr;
    const bool request_alloc_diag = std::getenv("PSPRECOMP_REQUEST_ALLOC_DIAG") != nullptr;
    const bool world_stream_stop_at_callback = std::getenv("PSPRECOMP_WORLD_STREAM_STOP_AT_CALLBACK") != nullptr;
    const std::uint32_t world_stream_manager = static_cast<std::uint32_t>(
        parse_environment_u64("PSPRECOMP_WORLD_STREAM_MANAGER", 0x08E91200u));
    bool world_stream_active_known = false;
    std::uint32_t world_stream_previous_active = 0u;
    const bool heap_diag = std::getenv("PSPRECOMP_HEAP_DIAG") != nullptr;
    const bool file_object_diag = std::getenv("PSPRECOMP_FILE_OBJECT_DIAG") != nullptr;
    const bool file_object_stop_on_null = std::getenv("PSPRECOMP_FILE_OBJECT_STOP_ON_NULL") != nullptr;
    std::uint32_t trace_pc = 0u;
    bool trace_pc_enabled = false;
    if (const char *trace_pc_text = std::getenv("PSPRECOMP_TRACE_PC")) {
        char *end = nullptr;
        trace_pc = static_cast<std::uint32_t>(std::strtoul(trace_pc_text, &end, 0));
        trace_pc_enabled = end != trace_pc_text && *end == '\0';
    }
    const bool trace_dispatch = std::getenv("PSPRECOMP_TRACE") != nullptr;
    const bool strict_pc_progress = std::getenv("PSPRECOMP_STRICT_PC_PROGRESS") != nullptr;
    const bool diagnostic_dispatch = profile_dispatch || trace_on_error || world_stream_diag ||
        request_alloc_diag || world_stream_stop_at_callback || heap_diag || file_object_diag || file_object_stop_on_null ||
        trace_pc_enabled || trace_dispatch;
    const bool deferred_profile_only = profile_dispatch && profile_dispatch_start != 0u &&
        !trace_on_error && !world_stream_diag && !request_alloc_diag && !world_stream_stop_at_callback && !heap_diag &&
        !file_object_diag && !file_object_stop_on_null && !trace_pc_enabled && !trace_dispatch;
    if (deferred_profile_only) {
        const std::uint64_t profile_end = profile_dispatch_count == 0u ||
            profile_dispatch_start > std::numeric_limits<std::uint64_t>::max() - profile_dispatch_count
            ? max_dispatches
            : std::min(max_dispatches, profile_dispatch_start + profile_dispatch_count);
        std::uint64_t dispatch = 0u;
        auto execute_once = [&]() {
            const std::uint32_t before = cpu_.pc;
            chain_context_invalidated_ = false;
            resolve_code_overlay(before);
            const std::int32_t dispatch_thread_uid = g_runtime_thread_uid;
            RecompiledFunction function = lookup_function(before);
            if (function == nullptr) {
                stop("No recompiled function registered at " + hex32(before));
                return;
            }
            g_runtime_dispatch_pc = before;
            count_pc(before);
            if (g_pre_dispatch_hook != nullptr)
                g_pre_dispatch_hook(*this, cpu_, before, dispatch_thread_uid);
            try {
                function(*this, cpu_);
            } catch (const Error &e) {
                const FunctionEntry *function_entry = lookup_entry(before);
                const std::string name = function_entry != nullptr ? function_entry->name : "unknown";
                std::ostringstream message;
                message << e.what() << " while executing " << name
                        << " dispatch=" << hex32(before) << " guest_pc=" << hex32(cpu_.pc)
                        << " (a0=" << hex32(cpu_.gpr[4]) << ", a1=" << hex32(cpu_.gpr[5])
                        << ", a2=" << hex32(cpu_.gpr[6]) << ", a3=" << hex32(cpu_.gpr[7])
                        << ", sp=" << hex32(cpu_.gpr[29]) << ", ra=" << hex32(cpu_.gpr[31]) << ")";
                append_gpr_dump(message, cpu_);
                throw Error(message.str());
            }
            cpu_.gpr[0] = 0u;
            if (!stopped_ && g_post_dispatch_hook != nullptr)
                g_post_dispatch_hook(*this, cpu_, before, dispatch_thread_uid);
            if (!stopped_) (void)account_dispatch_work(cpu_, true);
            if (!stopped_ && strict_pc_progress && cpu_.pc == before) {
                const FunctionEntry *function_entry = lookup_entry(before);
                const std::string name = function_entry != nullptr ? function_entry->name : "unknown";
                stop("Recompiled function returned without changing PC: " + name + " at " + hex32(before));
            }
        };
        for (; dispatch < std::min(profile_dispatch_start, max_dispatches) && !stopped_; ++dispatch)
            execute_once();
        for (; dispatch < profile_end && !stopped_; ++dispatch) {
            ++dispatch_counts[cpu_.pc];
            execute_once();
        }
        std::vector<std::pair<std::uint32_t, std::uint64_t>> hot(dispatch_counts.begin(), dispatch_counts.end());
        std::sort(hot.begin(), hot.end(), [](const auto &left, const auto &right) {
            return left.second > right.second;
        });
        std::cerr << "[profile-window] start=" << profile_dispatch_start
                  << " executed=" << dispatch
                  << " sampled=" << (dispatch >= profile_dispatch_start ? dispatch - profile_dispatch_start : 0u)
                  << " pc=" << hex32(cpu_.pc) << " unique=" << hot.size() << "\n";
        const std::size_t count = std::min<std::size_t>(30u, hot.size());
        for (std::size_t index = 0; index < count; ++index) {
            const FunctionEntry *profile_entry = lookup_entry(hot[index].first);
            std::cerr << "[profile-window] " << hex32(hot[index].first)
                      << " count=" << hot[index].second
                      << " name=" << (profile_entry != nullptr ? profile_entry->name : "unknown") << "\n";
        }
        if (!stopped_) stop("Dispatch profile window complete at " + hex32(cpu_.pc));
        return;
    }
    if (!diagnostic_dispatch) {
        const std::uint64_t progress_every = parse_environment_u64("PSPRECOMP_PROGRESS_EVERY");
        std::uint64_t next_progress = progress_every;
        const RuntimeHeartbeatHook heartbeat = g_heartbeat_hook;
        const std::uint64_t heartbeat_every = heartbeat != nullptr ? g_heartbeat_interval : 0u;
        std::uint64_t next_heartbeat = heartbeat_every;
        const RuntimeStarvationHook starvation = g_starvation_hook;
        const std::uint64_t starvation_every = starvation != nullptr ? g_starvation_interval : 0u;
        std::uint64_t executed_dispatches = 0u;
        for (; executed_dispatches < max_dispatches && !stopped_; ++executed_dispatches) {
            const std::uint32_t before = cpu_.pc;
            chain_context_invalidated_ = false;
            resolve_code_overlay(before);
            const std::int32_t dispatch_thread_uid = g_runtime_thread_uid;
            // Most outer dispatches are ordinary AOT PCs.  Resolve those
            // through the tiny generated-unit table first; the old per-PC
            // table spans several MiB and an effectively random probe here can
            // cost a cache/TLB miss at every outer return.  Any unit containing
            // a host/import override is poisoned at registration and falls
            // back to exact per-PC dispatch automatically.
            RecompiledFunction function = lookup_generated_unit(before);
            if (function == nullptr) function = lookup_function(before);
            if (function == nullptr) {
                stop("No recompiled function registered at " + hex32(before));
                break;
            }
            g_runtime_dispatch_pc = before;
            count_pc(before);
            if (g_pre_dispatch_hook != nullptr)
                g_pre_dispatch_hook(*this, cpu_, before, dispatch_thread_uid);
            try {
                function(*this, cpu_);
            } catch (const Error &e) {
                const FunctionEntry *function_entry = lookup_entry(before);
                const std::string name = function_entry != nullptr ? function_entry->name : "unknown";
                std::ostringstream message;
                message << e.what() << " while executing " << name
                        << " dispatch=" << hex32(before) << " guest_pc=" << hex32(cpu_.pc)
                        << " (a0=" << hex32(cpu_.gpr[4]) << ", a1=" << hex32(cpu_.gpr[5])
                        << ", a2=" << hex32(cpu_.gpr[6]) << ", a3=" << hex32(cpu_.gpr[7])
                        << ", sp=" << hex32(cpu_.gpr[29]) << ", ra=" << hex32(cpu_.gpr[31]) << ")";
                append_gpr_dump(message, cpu_);
                throw Error(message.str());
            }
            // `$zero` is now enforced at every generated/HLE write site. Do not
            // dirty the context register cache line after every outer dispatch.
            if (!stopped_ && g_post_dispatch_hook != nullptr)
                g_post_dispatch_hook(*this, cpu_, before, dispatch_thread_uid);
            if (progress_every != 0u && executed_dispatches + 1u >= next_progress) {
                std::cerr << "[progress] dispatch=" << (executed_dispatches + 1u)
                          << " pc=" << hex32(cpu_.pc) << "\n";
                next_progress += progress_every;
            }
            if (heartbeat_every != 0u && executed_dispatches + 1u >= next_heartbeat) {
                heartbeat(executed_dispatches + 1u, cpu_.pc);
                next_heartbeat += heartbeat_every;
            }
            if (starvation_every != 0u && starvation != nullptr)
                (void)account_dispatch_work(cpu_, true);
            if (!stopped_ && strict_pc_progress && cpu_.pc == before) {
                const FunctionEntry *function_entry = lookup_entry(before);
                const std::string name = function_entry != nullptr ? function_entry->name : "unknown";
                stop("Recompiled function returned without changing PC: " + name + " at " + hex32(before));
            }
        }
        if (std::getenv("PSPRECOMP_REPORT_DISPATCH_COUNT") != nullptr) {
            std::cerr << "[dispatch-count] executed=" << executed_dispatches
                      << " chained=" << chained_dispatches_
                      << " total=" << (executed_dispatches + chained_dispatches_)
                      << " pc=" << hex32(cpu_.pc) << "\n";
        }
        if (!stopped_ && max_dispatches != 0u)
            stop("Dispatch limit reached at " + hex32(cpu_.pc));
        return;
    }
    for (std::uint64_t dispatch = 0; dispatch < max_dispatches && !stopped_; ++dispatch) {
        if (profile_dispatch) ++dispatch_counts[cpu_.pc];
        const std::uint32_t before = cpu_.pc;
        chain_context_invalidated_ = false;
        resolve_code_overlay(before);
        RecompiledFunction function = lookup_function(before);
        const FunctionEntry *function_entry = lookup_entry(before);
        if (function == nullptr) {
            stop("No recompiled function registered at " + hex32(before));
            break;
        }
        g_runtime_dispatch_pc = before;
        count_pc(before);
        DispatchSnapshot current_snapshot{};
        current_snapshot.pc = before;
        current_snapshot.a0 = cpu_.gpr[4];
        current_snapshot.a1 = cpu_.gpr[5];
        current_snapshot.a2 = cpu_.gpr[6];
        current_snapshot.a3 = cpu_.gpr[7];
        current_snapshot.sp = cpu_.gpr[29];
        current_snapshot.ra = cpu_.gpr[31];
        current_snapshot.thread_uid = g_runtime_thread_uid;
        current_snapshot.thread_name = g_runtime_thread_name;
        recent_dispatches[recent_next] = current_snapshot;
        recent_next = (recent_next + 1u) % recent_dispatches.size();
        recent_count = std::min(recent_count + 1u, recent_dispatches.size());
        if (before == 0x089345B0u && heap_diag) {
            constexpr std::uint32_t manager = 0x08BC6500u;
            std::cerr << "[heapdiag] callback entered manager=" << hex32(manager)
                      << " global=" << hex32(memory_.load32(0x08BADEF8u)) << "\n";
            for (std::uint32_t offset = 0; offset < 0x140u; offset += 16u) {
                std::cerr << "[heapdiag] " << hex32(manager + offset);
                for (std::uint32_t word = 0; word < 16u; word += 4u)
                    std::cerr << " " << hex32(memory_.load32(manager + offset + word));
                std::cerr << "\n";
            }
            std::uint32_t block = memory_.load32(manager + 8u);
            for (std::uint32_t index = 0u; block != 0u && index < 64u; ++index) {
                if (!memory_.contains(block, 16u)) {
                    std::cerr << "[heapdiag-free] invalid=" << hex32(block) << "\n";
                    break;
                }
                const std::uint32_t size = memory_.load32(block + 0u);
                const std::uint32_t prev = memory_.load32(block + 8u);
                const std::uint32_t next = memory_.load32(block + 12u);
                std::cerr << "[heapdiag-free] index=" << index << " block=" << hex32(block)
                          << " size=" << hex32(size) << " prev=" << hex32(prev)
                          << " next=" << hex32(next) << " end=" << hex32(block + size) << "\n";
                block = next;
            }
        }
        if (request_alloc_diag &&
            (before == 0x089390ACu || before == 0x08939114u ||
             before == 0x08939590u || before == 0x089395D4u ||
             before == 0x0893961Cu || before == 0x089396D8u ||
             before == 0x089397CCu || before == 0x08956258u)) {
            const RuntimeExecutionContextToken token = capture_runtime_execution_context();
            std::uint32_t manager = 0u;
            if (before == 0x08939590u) manager = cpu_.gpr[4];
            else if (before == 0x089395D4u || before == 0x0893961Cu ||
                     before == 0x089396D8u) manager = cpu_.gpr[16];
            else if ((before == 0x089390ACu || before == 0x08939114u) &&
                     memory_.contains(cpu_.gpr[28] + 5908u, 4u))
                manager = memory_.load32(cpu_.gpr[28] + 5908u);
            else if (before == 0x089397CCu) manager = cpu_.gpr[4];
            std::cerr << "[reqalloc] pc=" << hex32(before)
                      << " uid=" << token.thread_uid
                      << " gen=" << token.switch_generation
                      << " v0=" << hex32(cpu_.gpr[2])
                      << " a0=" << hex32(cpu_.gpr[4])
                      << " a1=" << hex32(cpu_.gpr[5])
                      << " a2=" << hex32(cpu_.gpr[6])
                      << " a3=" << hex32(cpu_.gpr[7])
                      << " t0=" << hex32(cpu_.gpr[8])
                      << " t1=" << hex32(cpu_.gpr[9])
                      << " s0=" << hex32(cpu_.gpr[16])
                      << " s1=" << hex32(cpu_.gpr[17])
                      << " s2=" << hex32(cpu_.gpr[18])
                      << " sp=" << hex32(cpu_.gpr[29])
                      << " ra=" << hex32(cpu_.gpr[31]);
            if (manager != 0u && memory_.contains(manager + 6912u, 4u)) {
                const std::uint32_t free_head = memory_.load32(manager + 6900u);
                const std::uint32_t active_head = memory_.load32(manager + 6908u);
                std::cerr << " manager=" << hex32(manager)
                          << " free_head=" << hex32(free_head)
                          << " active_head=" << hex32(active_head);
            }
            std::uint32_t request = 0u;
            if (before == 0x089397CCu) request = cpu_.gpr[5];
            else if (before == 0x089396D8u) request = cpu_.gpr[18];
            else if (before == 0x08956258u || before == 0x08939114u) request = cpu_.gpr[2];
            if (request != 0u && memory_.contains(request, 52u)) {
                std::cerr << " req=" << hex32(request)
                          << " prev=" << hex32(memory_.load32(request + 0u))
                          << " next=" << hex32(memory_.load32(request + 4u))
                          << " size=" << memory_.load32(request + 8u)
                          << " source=" << hex32(memory_.load32(request + 16u))
                          << " offset=" << memory_.load32(request + 20u)
                          << " remaining=" << memory_.load32(request + 24u)
                          << " progressed=" << memory_.load32(request + 28u)
                          << " callback=" << hex32(memory_.load32(request + 48u));
            }
            std::cerr << "\n";
        }
        if (world_stream_diag &&
            (before == 0x08953990u || before == 0x08955134u ||
             before == 0x08955E7Cu || before == 0x08956258u ||
             before == 0x089563C0u || before == 0x08956408u ||
             before == 0x089569C0u || before == 0x089569E0u)) {
            std::cerr << "[worlddiag] pc=" << hex32(before)
                      << " uid=" << g_runtime_thread_uid
                      << " a0=" << hex32(cpu_.gpr[4])
                      << " a1=" << hex32(cpu_.gpr[5])
                      << " ra=" << hex32(cpu_.gpr[31]);
            if (before == 0x089563C0u && memory_.contains(cpu_.gpr[4], 1040u)) {
                const std::uint32_t manager = cpu_.gpr[4];
                std::cerr << " req=" << hex32(cpu_.gpr[5])
                          << " f596=" << hex32(memory_.load32(manager + 596u))
                          << " f600=" << hex32(memory_.load32(manager + 600u))
                          << " event=" << memory_.load32(manager + 616u)
                          << " active=" << hex32(memory_.load32(manager + 628u))
                          << " mode=" << memory_.load32(manager + 636u)
                          << " work=" << hex32(memory_.load32(manager + 640u))
                          << " limit=" << memory_.load32(manager + 648u)
                          << " stack=" << memory_.load32(manager + 1036u);
            }
            std::uint32_t manager = world_stream_manager;
            if (before == 0x08955E7Cu || before == 0x08956258u) manager = cpu_.gpr[17];
            else if (before == 0x08956408u) manager = cpu_.gpr[16];
            else if (before == 0x089563C0u) manager = cpu_.gpr[4];
            if (manager != 0u && memory_.contains(manager + 628u, 4u)) {
                std::cerr << " manager=" << hex32(manager)
                          << " active_before=" << hex32(memory_.load32(manager + 628u))
                          << " pending_v0=" << hex32(cpu_.gpr[2]);
            }
            std::cerr << "\n";
            if (world_stream_stop_at_callback && before == 0x089563C0u) {
                stop("World-stream diagnostic stop at callback " + hex32(before));
                break;
            }
        }
        if (file_object_diag &&
            (before == 0x08938F04u || before == 0x08938F7Cu || before == 0x089394A4u ||
             before == 0x08955DCCu || before == 0x08955DFCu || before == 0x08955E58u ||
             before == 0x08955E7Cu)) {
            const std::uint32_t manager =
                before == 0x08955DCCu ? cpu_.gpr[4] :
                ((before == 0x08955DFCu || before == 0x08955E58u || before == 0x08955E7Cu) ? cpu_.gpr[17] : 0u);
            const std::uint32_t object = before == 0x08938F7Cu ? cpu_.gpr[4] :
                (before == 0x089394A4u ? cpu_.gpr[5] : 0u);
            const bool seek_pc = before == 0x08938F7Cu || before == 0x089394A4u;
            bool manager_changed = false;
            static std::uint32_t previous_manager = 0u;
            static std::uint32_t previous_file_object = 0xFFFFFFFFu;
            static std::uint32_t previous_active = 0xFFFFFFFFu;
            static std::uint32_t previous_mode = 0xFFFFFFFFu;
            std::uint32_t manager_file_object = 0u;
            std::uint32_t manager_active = 0u;
            std::uint32_t manager_mode = 0u;
            if (manager != 0u && memory_.contains(manager, 652u)) {
                manager_file_object = memory_.load32(manager + 624u);
                manager_active = memory_.load32(manager + 628u);
                manager_mode = memory_.load32(manager + 636u);
                manager_changed = manager != previous_manager ||
                    manager_file_object != previous_file_object ||
                    manager_active != previous_active || manager_mode != previous_mode;
                previous_manager = manager;
                previous_file_object = manager_file_object;
                previous_active = manager_active;
                previous_mode = manager_mode;
            }
            const bool should_log = before == 0x08938F04u ||
                (seek_pc && object == 0u) || manager_changed;
            if (should_log) {
                std::cerr << "[fileobj] pc=" << hex32(before)
                          << " uid=" << g_runtime_thread_uid
                          << " name=" << g_runtime_thread_name.data()
                          << " v0=" << hex32(cpu_.gpr[2])
                          << " a0=" << hex32(cpu_.gpr[4])
                          << " a1=" << hex32(cpu_.gpr[5])
                          << " a2=" << hex32(cpu_.gpr[6])
                          << " a3=" << hex32(cpu_.gpr[7])
                          << " s0=" << hex32(cpu_.gpr[16])
                          << " s1=" << hex32(cpu_.gpr[17])
                          << " sp=" << hex32(cpu_.gpr[29])
                          << " ra=" << hex32(cpu_.gpr[31]);
                if (before == 0x08938F04u && memory_.contains(cpu_.gpr[4])) {
                    try { std::cerr << " path=\"" << memory_.read_c_string(cpu_.gpr[4], 512u) << "\""; } catch (...) {}
                }
                if (manager != 0u && memory_.contains(manager, 652u)) {
                    std::cerr << " manager=" << hex32(manager)
                              << " f596=" << hex32(memory_.load32(manager + 596u))
                              << " f600=" << hex32(memory_.load32(manager + 600u))
                              << " event=" << memory_.load32(manager + 616u)
                              << " fileobj=" << hex32(manager_file_object)
                              << " active=" << hex32(manager_active)
                              << " mode=" << manager_mode
                              << " work=" << hex32(memory_.load32(manager + 640u))
                              << " offset=" << memory_.load32(manager + 644u)
                              << " length=" << memory_.load32(manager + 648u);
                }
                if (object != 0u && memory_.contains(object, 16u)) {
                    std::cerr << " object=" << hex32(object)
                              << " words=" << hex32(memory_.load32(object + 0u))
                              << "," << hex32(memory_.load32(object + 4u))
                              << "," << hex32(memory_.load32(object + 8u))
                              << "," << hex32(memory_.load32(object + 12u));
                }
                std::cerr << "\n";
            }
            if (file_object_stop_on_null && seek_pc && object == 0u) {
                stop("File-object diagnostic stop before null seek at " + hex32(before));
                break;
            }
        }
        if (trace_pc_enabled && before == trace_pc) {
            std::cerr << "[trace-pc] " << hex32(before) << " " << (function_entry != nullptr ? function_entry->name : std::string("unknown"))
                      << " v0=" << hex32(cpu_.gpr[2])
                      << " a0=" << hex32(cpu_.gpr[4])
                      << " a1=" << hex32(cpu_.gpr[5])
                      << " a2=" << hex32(cpu_.gpr[6])
                      << " a3=" << hex32(cpu_.gpr[7])
                      << " s0=" << hex32(cpu_.gpr[16])
                      << " s1=" << hex32(cpu_.gpr[17])
                      << " s2=" << hex32(cpu_.gpr[18])
                      << " s3=" << hex32(cpu_.gpr[19])
                      << " sp=" << hex32(cpu_.gpr[29])
                      << " ra=" << hex32(cpu_.gpr[31]);
            if (memory_.contains(cpu_.gpr[4])) {
                try { std::cerr << " a0str=\"" << memory_.read_c_string(cpu_.gpr[4], 256u) << "\""; } catch (...) {}
            }
            if (memory_.contains(cpu_.gpr[5])) {
                try { std::cerr << " a1str=\"" << memory_.read_c_string(cpu_.gpr[5], 256u) << "\""; } catch (...) {}
            }
            if (std::getenv("PSPRECOMP_TRACE_MEM") != nullptr) {
                for (const auto &[label, address] : std::array<std::pair<const char *, std::uint32_t>, 2>{{{"a0mem", cpu_.gpr[4]}, {"a1mem", cpu_.gpr[5]}}}) {
                    if (!memory_.contains(address, 32u)) continue;
                    std::cerr << " " << label << "=";
                    for (std::uint32_t offset = 0; offset < 32u; offset += 4u)
                        std::cerr << (offset == 0u ? "" : ",") << hex32(memory_.load32(address + offset));
                }
            }
            std::cerr << "\n";
        }
        if (std::getenv("PSPRECOMP_TRACE") != nullptr) {
            std::cerr << "[dispatch] " << hex32(before) << " " << (function_entry != nullptr ? function_entry->name : std::string("unknown"))
                      << " a0=" << hex32(cpu_.gpr[4])
                      << " a1=" << hex32(cpu_.gpr[5])
                      << " a2=" << hex32(cpu_.gpr[6])
                      << " a3=" << hex32(cpu_.gpr[7])
                      << " sp=" << hex32(cpu_.gpr[29])
                      << " ra=" << hex32(cpu_.gpr[31]) << "\n";
        }
        try {
            function(*this, cpu_);
        } catch (const Error &e) {
            if (trace_on_error) {
                std::cerr << "[recent-dispatches] oldest-to-newest count=" << recent_count << "\n";
                const std::size_t first = (recent_next + recent_dispatches.size() - recent_count) % recent_dispatches.size();
                for (std::size_t index = 0; index < recent_count; ++index) {
                    const DispatchSnapshot &snapshot = recent_dispatches[(first + index) % recent_dispatches.size()];
                    std::cerr << "[recent] uid=" << snapshot.thread_uid
                              << " name=" << snapshot.thread_name.data()
                              << " pc=" << hex32(snapshot.pc)
                              << " a0=" << hex32(snapshot.a0)
                              << " a1=" << hex32(snapshot.a1)
                              << " a2=" << hex32(snapshot.a2)
                              << " a3=" << hex32(snapshot.a3)
                              << " sp=" << hex32(snapshot.sp)
                              << " ra=" << hex32(snapshot.ra) << "\n";
                }
            }
            std::ostringstream message;
            message << e.what() << " while executing " << (function_entry != nullptr ? function_entry->name : std::string("unknown")) << " dispatch=" << hex32(before) << " guest_pc=" << hex32(cpu_.pc)
                    << " (a0=" << hex32(cpu_.gpr[4]) << ", a1=" << hex32(cpu_.gpr[5])
                    << ", a2=" << hex32(cpu_.gpr[6]) << ", a3=" << hex32(cpu_.gpr[7])
                    << ", sp=" << hex32(cpu_.gpr[29]) << ", ra=" << hex32(cpu_.gpr[31]) << ")";
            append_gpr_dump(message, cpu_);
            throw Error(message.str());
        }
        cpu_.gpr[0] = 0u;
        if (world_stream_diag && world_stream_manager != 0u &&
            memory_.contains(world_stream_manager + 628u, 4u)) {
            const std::uint32_t active = memory_.load32(world_stream_manager + 628u);
            if (!world_stream_active_known) {
                world_stream_previous_active = active;
                world_stream_active_known = true;
            } else if (active != world_stream_previous_active) {
                std::cerr << "[worlddiag-transition] dispatch_pc=" << hex32(before)
                          << " uid_before=" << current_snapshot.thread_uid
                          << " uid_after=" << g_runtime_thread_uid
                          << " manager=" << hex32(world_stream_manager)
                          << " active=" << hex32(world_stream_previous_active)
                          << "->" << hex32(active)
                          << " next_pc=" << hex32(cpu_.pc);
                if (active != 0u && memory_.contains(active, 52u)) {
                    std::cerr << " source=" << hex32(memory_.load32(active + 16u))
                              << " offset=" << memory_.load32(active + 20u)
                              << " remaining=" << memory_.load32(active + 24u)
                              << " progressed=" << memory_.load32(active + 28u)
                              << " callback=" << hex32(memory_.load32(active + 48u));
                }
                std::cerr << "\n";
                world_stream_previous_active = active;
            }
            if (before == 0x08955E7Cu || before == 0x08956258u ||
                before == 0x08956408u || before == 0x089563C0u) {
                std::cerr << "[worlddiag-after] dispatch_pc=" << hex32(before)
                          << " uid_before=" << current_snapshot.thread_uid
                          << " uid_after=" << g_runtime_thread_uid
                          << " manager=" << hex32(world_stream_manager)
                          << " active_after=" << hex32(active)
                          << " next_pc=" << hex32(cpu_.pc) << "\n";
            }
        }
        if (!stopped_ && g_post_dispatch_hook != nullptr)
            g_post_dispatch_hook(*this, cpu_, before, current_snapshot.thread_uid);
        // Starvation preemption must behave identically under diagnostics, or a
        // traced run would stall where a production run makes progress.
        if (g_starvation_hook != nullptr && g_starvation_interval != 0u)
            (void)account_dispatch_work(cpu_, true);
        // A recompiled unit may explicitly redispatch to its own entry through
        // a guest jump table or self-loop.  This is legal PSP control flow and
        // must not be confused with a missing PC write.  The dispatch cap still
        // bounds genuine infinite loops.  Keep the old assertion available as
        // an opt-in diagnostic when investigating generated-code fallthrough.
        if (!stopped_ && cpu_.pc == before && strict_pc_progress) {
            if (trace_on_error) {
                std::cerr << "[unchanged-pc] pc=" << hex32(before)
                          << " function=" << (function_entry != nullptr ? function_entry->name : std::string("unknown"))
                          << " v0=" << hex32(cpu_.gpr[2])
                          << " a0=" << hex32(cpu_.gpr[4])
                          << " a1=" << hex32(cpu_.gpr[5])
                          << " a2=" << hex32(cpu_.gpr[6])
                          << " a3=" << hex32(cpu_.gpr[7])
                          << " t0=" << hex32(cpu_.gpr[8])
                          << " t1=" << hex32(cpu_.gpr[9])
                          << " t2=" << hex32(cpu_.gpr[10])
                          << " t3=" << hex32(cpu_.gpr[11])
                          << " sp=" << hex32(cpu_.gpr[29])
                          << " ra=" << hex32(cpu_.gpr[31]) << "\n";
            }
            stop("Recompiled function returned without changing PC: " + (function_entry != nullptr ? function_entry->name : std::string("unknown")) + " at " + hex32(before));
        }
    }
    if (!stopped_ && max_dispatches != 0u) {
        if (trace_on_error) {
            std::cerr << "[recent-dispatches] dispatch-limit oldest-to-newest count=" << recent_count << "\n";
            const std::size_t first = (recent_next + recent_dispatches.size() - recent_count) % recent_dispatches.size();
            for (std::size_t index = 0; index < recent_count; ++index) {
                const DispatchSnapshot &snapshot = recent_dispatches[(first + index) % recent_dispatches.size()];
                std::cerr << "[recent] uid=" << snapshot.thread_uid
                          << " name=" << snapshot.thread_name.data()
                          << " pc=" << hex32(snapshot.pc)
                          << " a0=" << hex32(snapshot.a0)
                          << " a1=" << hex32(snapshot.a1)
                          << " a2=" << hex32(snapshot.a2)
                          << " a3=" << hex32(snapshot.a3)
                          << " sp=" << hex32(snapshot.sp)
                          << " ra=" << hex32(snapshot.ra) << "\n";
            }
        }
        if (profile_dispatch) {
            std::vector<std::pair<std::uint32_t, std::uint64_t>> hot(dispatch_counts.begin(), dispatch_counts.end());
            std::sort(hot.begin(), hot.end(), [](const auto &left, const auto &right) {
                return left.second > right.second;
            });
            std::cerr << "[profile] dispatch limit pc=" << hex32(cpu_.pc)
                      << " unique=" << hot.size() << "\n";
            const std::size_t count = std::min<std::size_t>(20u, hot.size());
            for (std::size_t index = 0; index < count; ++index) {
                const auto function = functions_.find(hot[index].first);
                std::cerr << "[profile] " << hex32(hot[index].first)
                          << " count=" << hot[index].second
                          << " name=" << (function != functions_.end() ? function->second.name : "unknown")
                          << "\n";
            }
        }
        stop("Dispatch limit reached at " + hex32(cpu_.pc));
    }
}

void Runtime::stop(std::string reason) { stopped_ = true; stop_reason_ = std::move(reason); }
bool Runtime::stopped() const noexcept { return stopped_; }
const std::string &Runtime::stop_reason() const noexcept { return stop_reason_; }

void Runtime::unsupported(std::uint32_t pc, std::uint32_t instruction, const std::string &reason) {
    stop("Unsupported Allegrex instruction " + hex32(instruction) + " at " + hex32(pc) + ": " + reason);
}

void Runtime::arithmetic_overflow(std::uint32_t pc, std::uint32_t instruction) {
    stop("Allegrex arithmetic overflow for " + hex32(instruction) + " at " + hex32(pc));
}

void Runtime::register_native_fast_path(std::uint32_t address, NativeFastPath function) {
    const std::uint32_t canonical_address = memory_.canonical(address);
    if (!function) {
        native_fast_paths_.erase(canonical_address);
        return;
    }
    native_fast_paths_[canonical_address] = std::move(function);
}

void Runtime::invoke_native_fast_path(std::uint32_t address, AllegrexContext &ctx) {
    const std::uint32_t canonical_address = memory_.canonical(address);
    const std::uint32_t current_pc = memory_.canonical(ctx.pc);

    // A profile may use this entry at an indirect call site whose target varies.
    // If the current target is not the registered native leaf, preserve the old
    // low-overhead direct-chainable fallback without exposing profile details.
    if (current_pc != canonical_address) {
        if (current_pc < direct_base_) return;
        const std::uint32_t delta = current_pc - direct_base_;
        if ((delta & 3u) != 0u) return;
        const std::size_t index = static_cast<std::size_t>(delta) / 4u;
        if (index >= direct_chainable_.size()) return;
        const RecompiledFunction target_function = direct_chainable_[index];
        if (target_function == nullptr) return;
        target_function(*this, ctx);
        ctx.gpr[0] = 0u;
        return;
    }

    const auto fast = native_fast_paths_.find(canonical_address);
    if (fast != native_fast_paths_.end()) {
        fast->second(*this, ctx);
        ctx.gpr[0] = 0u;
        return;
    }

    // Missing profile registration is safe: execute the generated AOT body.
    const RecompiledFunction original = lookup_function(canonical_address);
    if (original == nullptr)
        throw Error("Missing AOT function for native fast path " + hex32(canonical_address));
    original(*this, ctx);
    ctx.gpr[0] = 0u;
}

void Runtime::invoke_import_cached(std::uint32_t slot, std::string_view library,
                                   std::uint32_t nid, AllegrexContext &ctx) {
    if (hle_histogram_enabled_) ++hle_histogram_[hle_key(library, nid)];

    const HleFunction *bound = slot < import_bindings_.size() ? import_bindings_[slot] : nullptr;
    if (bound == nullptr) {
        const auto library_it = hle_.find(library);
        const auto function_it = library_it != hle_.end()
            ? library_it->second.find(nid) : HleLibrary::const_iterator{};
        if (library_it == hle_.end() || function_it == library_it->second.end()) {
            const std::string library_name(library);
            const auto name = nids_.resolve(library_name, nid).value_or(hex32(nid));
            stop("Missing HLE import " + library_name + "::" + name);
            return;
        }
        if (slot >= import_bindings_.size()) import_bindings_.resize(static_cast<std::size_t>(slot) + 1u, nullptr);
        bound = &function_it->second;
        import_bindings_[slot] = bound;
    }

    (*bound)(*this, ctx);
    if (!stopped_ && g_post_import_hook != nullptr) g_post_import_hook(*this, ctx);
}

void Runtime::invoke_import(std::string_view library, std::uint32_t nid, AllegrexContext &ctx) {
    // PSPRECOMP_HLE_HISTOGRAM distinguishes a genuine synchronous workload from
    // a kernel-wait livelock: real translated work barely calls into the HLE,
    // while a thread spinning on an operation the host never completes shows up
    // as a huge count on one or two wait imports.
    if (hle_histogram_enabled_) ++hle_histogram_[hle_key(library, nid)];
    const auto library_it = hle_.find(library);
    const auto function_it = library_it != hle_.end()
        ? library_it->second.find(nid) : HleLibrary::const_iterator{};
    if (library_it == hle_.end() || function_it == library_it->second.end()) {
        const std::string library_name(library);
        const auto name = nids_.resolve(library_name, nid).value_or(hex32(nid));
        stop("Missing HLE import " + library_name + "::" + name);
        return;
    }
    function_it->second(*this, ctx);
    if (!stopped_ && g_post_import_hook != nullptr) g_post_import_hook(*this, ctx);
}

AllegrexContext &Runtime::cpu() noexcept { return cpu_; }
const AllegrexContext &Runtime::cpu() const noexcept { return cpu_; }

} // namespace psprecomp
