#include "vcs_runtime_log.hpp"
#include "vcs_config.hpp"
#include "vcs_tier2_superblocks.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>

namespace vcs {
namespace {

struct RuntimeLogState {
    std::mutex mutex;
    std::ofstream file;
    std::filesystem::path path;
    bool enabled{};
    bool flush_every_line{true};
};

RuntimeLogState &state() {
    static RuntimeLogState s;
    return s;
}

std::string timestamp_now() {
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const std::time_t t = clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return out.str();
}

}

void runtime_log_initialize(const VcsConfiguration &configuration) {
    RuntimeLogState &s = state();
    std::lock_guard<std::mutex> guard(s.mutex);
    s.enabled = configuration.diagnostics.log_to_file;
    s.flush_every_line = configuration.diagnostics.flush_every_line;
    s.path.clear();
    if (s.file.is_open()) s.file.close();
    if (!s.enabled) return;
    std::filesystem::path file_name = configuration.diagnostics.log_file;
    if (file_name.empty()) file_name = "VCSNative.log";
    s.path = configuration.executable_directory / file_name;
    s.file.open(s.path, std::ios::out | std::ios::trunc);
    if (!s.file) {
        s.enabled = false;
        s.path.clear();
        return;
    }
    s.file << "VCSNative runtime log\n";
    s.file << "stage=perf-v8.15.5-hard-force-async-ge-2026-08-27\n"; // PSPRECOMP_V815_RUNTIME_LOG
    // PSPRECOMP_V8155_RUNTIME_LOG
    s.file << "config=" << configuration.source_path.string() << '\n';
    s.file << "started=" << timestamp_now() << '\n';
    s.file << "perf_telemetry=" << (configuration.diagnostics.perf_telemetry ? 1 : 0)
           << " interval_vblanks=" << configuration.diagnostics.perf_telemetry_interval_vblanks
           << "\n";
    s.file << "save_repro_testing=" << (configuration.testing.save_repro ? 1 : 0)
           << " default=0 internal_only=1\n";
    s.file << "guest_hotspot=" << (configuration.diagnostics.guest_hotspot_profile ? 1 : 0)
           << " sample_stride=256 interval_vblanks=300"
#if defined(PSPRECOMP_RUNTIME_CHAIN_TELEMETRY)
           << " chain_telemetry_compiled=1\n";
#else
           << " chain_telemetry_compiled=0\n";
#endif
    s.file << "tier2_superblocks=" << (tier2_superblocks_enabled() ? 1 : 0)
           << " version=4 clusters=7 mask=0x" << std::hex << tier2_cluster_mask() << std::dec
           << " hot_blocks=1060 static_fused_calls=36 static_fused_tail=13 hooks=25"
           << " entity_leaf_inline_sites=13 geometry_inline_leaf_sites=0 direct_generated_leaf_sites=5"
           << " unwind_fix=1 reentry_guard=1 dataflow=1 vfpu_block32=133 mem_runs=35 mem_words=287"
           << " append32=51 advance32=89 simd_mat4=4 simd_matvec=19"
           << " gpr_shadow_clusters=4 gpr_shadow_regs=24 gpr_shadow_occurrences=3461 geometry_shadow=0"
           << " perf_layer=16 cpu_lean_revision=2 cpu_aggressive_revision=10 correctness_revision=8272 recovery_from_821=1"
           << " save_transaction=1 save_lifecycle_v82=1 sas_endflag_live=1 sas_loop_history_restore=1"
           << " atrac_virtual_source=1 atrac_stall_diag=1 atrac_stream_resident_status=1 atrac_nonloop_resident=-2 atrac_loop_resident=-3 output2_success_zero=1 output2_master_watermark=1 output2_late_catchup=0 save_repro_checkpoint=1 save_repro_trace=1 save_repro_hotkey_f8=1 save_repro_trace_hotkey_f10=1 save_repro_auto_restore=1 save_repro_dispatch_sample_stride=64 save_repro_passive_until_f8=1 save_repro_hle_hotpath=0 save_repro_ui_hotkeys=1 save_repro_f10_async_fallback=1 save_repro_collector_partial_bundle=1 save_repro_internal_ini_gate=1 save_repro_default_enabled=0 save_exitdelete_semantics=1 save_repro_legacy_exitdelete_repair=1 save_partition_reuse=1 news_atrac_v825_guard=1 runtime_chain_telemetry_default=0 arch_fastmem=1 aot_direct_fastmem_default=1 aot_hard_fastmem=1 aot_branchless_mem=1 aot_vfpu_block32=19455 aot_scalar_run_blocks=10665 aot_scalar_run_words=53173 aot_load_runs=5633 aot_store_runs=5032 vfpu_default_dest_fast=1 vfpu_vmscl_default_fast=1 vfpu_default_fastlane=1 vfpu_vec3_fastlane_sites=2469 vfpu_unary_fastlane_sites=1607 vfpu_vdot_default_fast=1 vfpu_vscl_default_fast=1 radio_atrac_identity_guard=1 atrac_direct_header_validation=1 atrac_identity_cache=1 atrac_header_only_producer_hint=1 vfpu_vtfm_ct=1 vfpu_vtfm_ct_sites=337 vfpu_vi2f_ct=1 vfpu_vi2f_ct_sites=164 vfpu_vcmp_default_fast=1 vfpu_vcmp_ct_sites=558 vfpu_vcmov_default_fast=1 vfpu_vcmov_ct_sites=461 vfpu_cross_default_fast=1 vfpu_cross_ct_sites=217 vfpu_helper_ct2=1 trusted_chain=1 trusted_chain_sites=45968 trusted_chain_poisonable_sites=3240 compact_leaf_dispatch=1 compact_leaf_targets=4 compact_leaf_sites=1885 outer_entry_dispatch=1 outer_aot_view_reuse=1 v87_tiny_leaf_reverted=1 aot_register_residency=1 aot_register_residency_units=230 aot_gpr_resident_slots=6 aot_fpr_resident_slots=4 aot_gpr_resident_occurrences=914095 aot_fpr_resident_occurrences=146813 aot_resident_region_weight=6 resident_regions=1 resident_region_targets=83 resident_region_sites=11171 resident_region_files=220 resident_region_sync_out_removed=11171 resident_region_sync_in_removed=11171 resident_region_resident_args=53234 resident_region_ctx_args=26183 resident_region_scheduler_guard=1 resident_region_scheduler_fallback_sites=11171"
           << " tier2_direct_fastmem=1 tier2_direct_mem_sites=1969 tier2_deep_telemetry_default=0 geometry_fusion_rollback=1"
           << " entity_leaf_inline=1 entity_leaf_scheduler_accounting=1 entity_leaf_resume_pc_fix=1"
           << " ge_async_default=0 parallel_vertex_decode_default=0 ge_async_opt_in=PSPRECOMP_V8155_ENABLE_ASYNC"
           << " v5_vfpu_fast_quarantined=1 native_vfpu_088b1780_v4=1"
           << " dx12_execute_indirect_default=nvidia indirect_buffer_mb=16"
           << " ge_async_quarantined=1 ge_async_present_safepoint=1 ge_async_hard_force=0 parallel_vertex_decode_quarantined=1 legacy_perf_env_ignored=1"
           << " amd_uma_compat=1 uma_msaa_guard=1 packed0115_amd_stride=12"
           << " pipe_shadow_failsafe=1 material_shadow_scalar_probe=1"
           << " draw_distance_common_world=1 draw_distance_fog_scale=1\n\n";
    if (s.flush_every_line) s.file.flush();
}

void runtime_log_shutdown() noexcept {
    RuntimeLogState &s = state();
    std::lock_guard<std::mutex> guard(s.mutex);
    if (s.file.is_open()) {
        s.file << '\n' << '[' << timestamp_now() << "] shutdown\n";
        s.file.flush();
        s.file.close();
    }
    s.path.clear();
    s.enabled = false;
}

bool runtime_log_enabled() noexcept {
    RuntimeLogState &s = state();
    std::lock_guard<std::mutex> guard(s.mutex);
    return s.enabled && s.file.is_open();
}

std::filesystem::path runtime_log_path() {
    RuntimeLogState &s = state();
    std::lock_guard<std::mutex> guard(s.mutex);
    return s.path;
}

void runtime_log_line(std::string_view line) {
    RuntimeLogState &s = state();
    std::lock_guard<std::mutex> guard(s.mutex);
    if (!s.enabled || !s.file.is_open()) return;
    s.file << '[' << timestamp_now() << "] " << line << '\n';
    if (s.flush_every_line) s.file.flush();
}

void runtime_log_error(std::string_view category, std::string_view message) {
    std::ostringstream out;
    out << category << ": " << message;
    runtime_log_line(out.str());
}

} // namespace vcs
