@echo off
setlocal EnableExtensions EnableDelayedExpansion

for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
set "PROFILE=%REPO%\profiles\vcs"
set "BUILD=%REPO%\out\vcs-release-ninja-clangcl"

if not exist "%REPO%\CMakeLists.txt" (
    echo ERROR: PSPRecomp root was not resolved correctly:
    echo   %REPO%
    pause
    exit /b 20
)
if not exist "%PROFILE%\CMakeLists.txt" (
    echo ERROR: VCS profile was not found:
    echo   %PROFILE%
    pause
    exit /b 21
)

rem ---------------------------------------------------------------------------
rem CRITICAL: ZIPs created in a UTC environment can extract on a UTC-3 Windows
rem machine with source mtimes several hours in the future. CMake/Ninja then
rem loops forever regenerating build.ninja because an input always remains newer
rem than the freshly written manifest.
rem
rem Clamp ONLY source files whose timestamp is actually > current local time.
rem Existing normal files and all out/build artifacts remain untouched.
rem ---------------------------------------------------------------------------
echo [0/7] Checking for future-dated source files...
call "%PROFILE%\FIX_FUTURE_TIMESTAMPS.bat"
if errorlevel 1 goto :FAIL

call "%PROFILE%\scripts\pick_jobs.bat"
if errorlevel 1 goto :FAIL
if defined PSPRECOMP_NINJA_JOBS set "JOBS=%PSPRECOMP_NINJA_JOBS%"
if not defined JOBS set "JOBS=2"

set "VSROOT="
for %%E in (Community Professional Enterprise BuildTools) do (
    if not defined VSROOT if exist "%ProgramFiles%\Microsoft Visual Studio\2022\%%E\Common7\Tools\VsDevCmd.bat" (
        set "VSROOT=%ProgramFiles%\Microsoft Visual Studio\2022\%%E"
    )
    if not defined VSROOT if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\%%E\Common7\Tools\VsDevCmd.bat" (
        set "VSROOT=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\%%E"
    )
)

if not defined VSROOT (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "!VSWHERE!" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "!VSWHERE!" (
        set "VSWHERE_OUT=%TEMP%\psprecomp_vswhere_%RANDOM%_%RANDOM%.txt"
        "!VSWHERE!" -latest -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "!VSWHERE_OUT!" 2>nul
        if exist "!VSWHERE_OUT!" (
            set /p VSROOT=<"!VSWHERE_OUT!"
            del /q "!VSWHERE_OUT!" >nul 2>nul
        )
    )
)
if not defined VSROOT goto :NO_VS
if not exist "%VSROOT%\Common7\Tools\VsDevCmd.bat" goto :NO_VS

echo Initializing VS2022 x64 environment...
echo   VS root: %VSROOT%
call "%VSROOT%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 goto :VS_ENV_FAIL

rem clang-cl only supplies the compiler frontend, not the Windows SDK headers or
rem MSVC STL, so VsDevCmd.bat above still has to set up INCLUDE/LIB. The actual
rem compiler and linker come from a standalone LLVM install (winget install
rem LLVM.LLVM), NOT the VS Build Tools' bundled Clang component: that one is
rem pinned to LLVM 19.1.5, whose clang-cl hangs 70+ minutes on
rem generated_unit_0103.cpp. LLVM 22.1.8 fixes it upstream
set "LLVM_BIN=%ProgramFiles%\LLVM\bin"
set "CLANG_CL_EXE=%LLVM_BIN%\clang-cl.exe"
set "LLD_LINK_EXE=%LLVM_BIN%\lld-link.exe"
if not exist "%CLANG_CL_EXE%" goto :NO_CLANG_CL
if not exist "%LLD_LINK_EXE%" goto :NO_LLD_LINK

set "CMAKE_EXE=%VSROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist "%CMAKE_EXE%" set "CMAKE_EXE="
if not defined CMAKE_EXE (
    for /f "delims=" %%I in ('where cmake.exe 2^>nul') do if not defined CMAKE_EXE set "CMAKE_EXE=%%~fI"
)
if not defined CMAKE_EXE if exist "%ProgramFiles%\CMake\bin\cmake.exe" set "CMAKE_EXE=%ProgramFiles%\CMake\bin\cmake.exe"
if not defined CMAKE_EXE goto :NO_CMAKE

set "NINJA_EXE=%VSROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if not exist "%NINJA_EXE%" set "NINJA_EXE="
if not defined NINJA_EXE (
    for /f "delims=" %%I in ('where ninja.exe 2^>nul') do if not defined NINJA_EXE set "NINJA_EXE=%%~fI"
)
if not defined NINJA_EXE goto :NO_NINJA

for %%I in ("%CMAKE_EXE%") do set "CTEST_EXE=%%~dpIctest.exe"
if not exist "%CTEST_EXE%" set "CTEST_EXE=ctest.exe"

set "NINJA_STATUS=[%%f/%%t %%p ^| %%e elapsed ^| %%r running] "
set "BOOTFIX_STAMP=%BUILD%\.vcs_tier2_bootfix_20260816_v1"
set "SUPERBLOCK_STAMP=%BUILD%\.vcs_tier2_v4_150fps_buildfix3_20260816"
set "AMD_COMPAT_STAMP=%BUILD%\.vcs_dx12_amd_uma_compat_20260816"
set "PERF_V5_STAMP=%BUILD%\.vcs_perf_v5_async_vfpu_20260816"
set "PERF_V5_STALLFIX_STAMP=%BUILD%\.vcs_perf_v5_async_stallfix_20260817"
set "PERF_V5_PRESENTFIX_STAMP=%BUILD%\.vcs_perf_v5_async_presentfix_20260817"
set "PERF_V5_SYNC_RECOVERY_STAMP=%BUILD%\.vcs_perf_v5_sync_recovery_20260817"
set "PERF_V5_STABLE_RECOVERY2_STAMP=%BUILD%\.vcs_perf_v5_stable_recovery2_20260817"
set "PERF_V6_ENTITY_LEAF_STAMP=%BUILD%\.vcs_perf_v6_entity_leaf_inline_20260817"
set "PERF_V6_ENTITY_LEAF_FIX1_STAMP=%BUILD%\.vcs_perf_v6_entity_leaf_inline_crashfix1_20260817"
set "PERF_V7_ARCH_FASTMEM_STAMP=%BUILD%\.vcs_perf_v7_arch_fastmem_20260817"
set "PERF_V8_CPU_FUSION_STAMP=%BUILD%\.vcs_perf_v8_cpu_fusion_20260817"
set "PERF_V81_CPU_LEAN_STAMP=%BUILD%\.vcs_perf_v81_cpu_lean_20260817"
set "PERF_V82_CPU_RUNTIME_LEAN_STAMP=%BUILD%\.vcs_perf_v82_cpu_runtime_lean_20260817"
set "CORRECTNESS_V822_STAMP=%BUILD%\.vcs_correctness_v822_recovery_20260817"
set "CORRECTNESS_V823_NEWS_STAMP=%BUILD%\.vcs_correctness_v823_news_audio_20260817"
set "CORRECTNESS_V824_NEWS_PACING_STAMP=%BUILD%\.vcs_correctness_v824_news_pacing_20260818"
set "CORRECTNESS_V825_NEWS_ATRAC_STAMP=%BUILD%\.vcs_correctness_v825_news_atrac_stream_20260818"
set "CORRECTNESS_V826_SAVE_REPRO_STAMP=%BUILD%\.vcs_correctness_v826_save_repro_20260818"
set "CORRECTNESS_V826A_SAVE_REPRO_STAMP=%BUILD%\.vcs_correctness_v826a_save_repro_passive_20260818"
set "CORRECTNESS_V826B_SAVE_REPRO_STAMP=%BUILD%\.vcs_correctness_v826b_save_repro_trace_20260818"
set "CORRECTNESS_V827_SAVE_THREAD_STAMP=%BUILD%\.vcs_correctness_v827_save_thread_lifecycle_20260818"
set "CORRECTNESS_V827A_SAVE_REPRO_GATE_STAMP=%BUILD%\.vcs_correctness_v827a_save_repro_gate_20260818"
set "PERF_V84_AGGRESSIVE_CPU_STAMP=%BUILD%\.vcs_perf_v84_aggressive_cpu_direct_20260818"
set "PERF_V85_VFPU_FASTLANE_STAMP=%BUILD%\.vcs_perf_v85_aggressive_vfpu_fastlane_20260818"
set "PERF_V86_RADIO_VFPU_CT2_STAMP=%BUILD%\.vcs_perf_v86_radio_identity_vfpu_ct2_20260818"
set "PERF_V88_TRUSTED_DISPATCH_STAMP=%BUILD%\.vcs_perf_v88_extreme_cpu_trusted_dispatch_20260818"
set "PERF_V89_REGCACHE_STAMP=%BUILD%\.vcs_perf_v89_extreme_cpu_register_residency_20260818"
set "PERF_V810_RESIDENT_REGIONS_STAMP=%BUILD%\.vcs_perf_v8101_resident_regions_scheduler_safe_20260818"
if not defined PSPRECOMP_TIER2_DEEP_TELEMETRY set "PSPRECOMP_TIER2_DEEP_TELEMETRY=OFF"
if not defined PSPRECOMP_RUNTIME_CHAIN_TELEMETRY set "PSPRECOMP_RUNTIME_CHAIN_TELEMETRY=OFF"
if /I "%PSPRECOMP_TIER2_DEEP_TELEMETRY%"=="1" set "PSPRECOMP_TIER2_DEEP_TELEMETRY=ON"
if /I "%PSPRECOMP_TIER2_DEEP_TELEMETRY%"=="0" set "PSPRECOMP_TIER2_DEEP_TELEMETRY=OFF"
if /I "%PSPRECOMP_RUNTIME_CHAIN_TELEMETRY%"=="1" set "PSPRECOMP_RUNTIME_CHAIN_TELEMETRY=ON"
if /I "%PSPRECOMP_RUNTIME_CHAIN_TELEMETRY%"=="0" set "PSPRECOMP_RUNTIME_CHAIN_TELEMETRY=OFF"

echo ================================================================
echo VCS - NINJA PERFORMANCE INCREMENTAL BUILD ^(clang-cl + lld-link^)
echo.
echo Repository:       %REPO%
echo Build tree:       %BUILD%
echo Visual Studio:    %VSROOT%
echo Compiler:         %CLANG_CL_EXE%
echo Linker:           %LLD_LINK_EXE%
echo CMake:            %CMAKE_EXE%
echo Ninja:            %NINJA_EXE%
echo Ninja workers:    %JOBS%
echo clang-cl /MP:     OFF ^(Ninja owns compile parallelism^)
echo Generated AOT:    O3; V8.9 register residency + V8.10.1 scheduler-safe resident regions
echo Correctness:      V8.10.1 scheduler-safe CPU over V8.6 radio + V8.2.7A SAVE + V8.2.5 NEWS
echo Host/core LTCG:   ON
echo AVX2/fast paths:  ON
echo Tier2 deep diag:  %PSPRECOMP_TIER2_DEEP_TELEMETRY%
echo Chain telemetry:   %PSPRECOMP_RUNTIME_CHAIN_TELEMETRY%
echo ================================================================
echo.
echo [0b2/7] Building V8.10.1 SCHEDULER-SAFE RESIDENT REGIONS over V8.9 baseline...
set "PYTHON3_CMD="
py -3 -c "import sys; raise SystemExit(0 if sys.version_info.major == 3 else 1)" >nul 2>&1
if not errorlevel 1 set "PYTHON3_CMD=py -3"
if not defined PYTHON3_CMD (
    python -c "import sys; raise SystemExit(0 if sys.version_info.major == 3 else 1)" >nul 2>&1
    if not errorlevel 1 set "PYTHON3_CMD=python"
)
if not defined PYTHON3_CMD goto :NO_PYTHON3
echo   Python 3:        %PYTHON3_CMD%
echo [0b-regcanon/7] Restoring canonical architectural register file before Tier-2/AOT passes...
%PYTHON3_CMD% "%REPO%\tools\optimize_generated_register_residency.py" "%PROFILE%\generated" --strip
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v810_resident_regions.py" --strip
if errorlevel 1 goto :FAIL

echo [0b/7] Reapplying BOOTFIX-safe Tier-2 transforms (OPT1 semantic transforms disabled)...
call "%PROFILE%\APPLY_TIER2_EXTREME.bat"
if errorlevel 1 goto :FAIL

%PYTHON3_CMD% "%PROFILE%\tools\build_tier2_superblocks.py" "%PROFILE%"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v84_cpu.py" "%PROFILE%"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v85_vfpu.py" "%PROFILE%"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v88_compact_leaves.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v88_trusted_chain.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tools\optimize_generated_v810_resident_regions.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%REPO%\tools\optimize_generated_register_residency.py" "%PROFILE%\generated" --gprs 6 --fprs 4 --resident-weight 6 --stats "%PROFILE%\generated\v89_register_residency_manifest.json"
if errorlevel 1 goto :FAIL
rem V8.2.7 checker includes the protected V8.2 CPU/Geometry, V8.2.5 NEWS,
rem V8.2.6 passive checkpoint contract and the corrected PSP ExitDelete lifecycle.
rem Older revision checkers pin exact stage strings and must not gate this stage.
%PYTHON3_CMD% "%PROFILE%\tests\check_v827_save_thread_lifecycle.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v827a_internal_save_repro_gate.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v84_aggressive_cpu.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v85_aggressive_vfpu.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v86_radio_vfpu_ct2.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v88_extreme_cpu_trusted_dispatch.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v89_register_residency.py"
if errorlevel 1 goto :FAIL
%PYTHON3_CMD% "%PROFILE%\tests\check_v810_resident_regions.py"
if errorlevel 1 goto :FAIL

if exist "%BUILD%" if not exist "%SUPERBLOCK_STAMP%" (
  echo.
  echo [0c-super/7] Tier2 V4 BUILDFIX3 - invalidating Geometry + runtime log once...
  rem BUILDFIX3 only changes Geometry codegen/shadow policy and runtime metadata.
  rem Keep every already-valid V4 object so Ninja does not repeat the expensive build.
  del /s /q "%BUILD%\*vcs_tier2_cluster_geometry*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%AMD_COMPAT_STAMP%" (
  echo.
  echo [0c-amd/7] DX12 AMD/UMA compatibility - invalidating backend + runtime log once...
  rem Do not touch generated/Tier2 objects: this compatibility revision only changes host DX12 policy.
  del /s /q "%BUILD%\*ge_gpu_backend_dx12*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V5_STAMP%" (
  echo.
  echo [0c-v5/7] V5 ASYNC/VFPU - invalidating hot clusters + changed host objects once...
  rem Hooks/generated units are unchanged. Rebuild only the five modified Tier2 cluster TUs and host policy/backend/log.
  del /s /q "%BUILD%\*vcs_tier2_cluster_entity*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_matrix*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_physics*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_world*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_edge43*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_config*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_native_fast_paths*.obj" >nul 2>&1
  del /s /q "%BUILD%\*ge_gpu_backend_dx12*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V5_STALLFIX_STAMP%" (
  echo.
  echo [0c-v5fix/7] V5 GE ASYNC STALL-RACE FIX - invalidating profile + runtime log once...
  rem Hotfix only changes the async GE scheduler/telemetry. Keep all Tier2 and DX12 objects.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V5_PRESENTFIX_STAMP%" (
  echo.
  echo [0c-v5present/7] V5 GE ASYNC PRESENTFIX - invalidating profile + runtime log once...
  rem Presentation safe-point fix only changes async GE/display scheduling + metadata.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V5_SYNC_RECOVERY_STAMP%" (
  echo.
  echo [0c-v5sync/7] V5 SYNC RECOVERY - restoring proven GE scheduler + safe defaults once...
  rem Only scheduler/config/log changed. Preserve expensive Tier2/DX12 objects.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_config*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V5_STABLE_RECOVERY2_STAMP%" (
  echo.
  echo [0c-v5stable2/7] V5 STABLE RECOVERY2 - quarantining parallel decode + V5 VFPU/native experiments once...
  rem Restore only the five V5-modified Tier2 clusters and native/config/log objects.
  rem Geometry and Boundary remain untouched to avoid the prior MSVC compile-time cliff.
  del /s /q "%BUILD%\*vcs_tier2_cluster_entity*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_matrix*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_physics*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_world*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_edge43*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_config*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_native_fast_paths*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V6_ENTITY_LEAF_STAMP%" (
  echo.
  echo [0c-v6entity/7] V6 ENTITY LEAF INLINE - invalidating Entity + runtime log once...
  rem V6 changes only the Entity Tier2 TU, its generator, and runtime metadata.
  rem Keep Geometry and every generated AOT object to preserve the stable incremental build.
  del /s /q "%BUILD%\*vcs_tier2_cluster_entity*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V6_ENTITY_LEAF_FIX1_STAMP%" (
  echo.
  echo [0c-v6entityfix1/7] V6 ENTITY LEAF INLINE CRASHFIX1 - publishing continuation PC before scheduler boundary...
  rem Only Entity codegen and runtime metadata changed. No Geometry/AOT rebuild.
  del /s /q "%BUILD%\*vcs_tier2_cluster_entity*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V8_CPU_FUSION_STAMP%" (
  echo.
  echo [0c-v8cpu/7] V8 CPU FUSION - invalidating Tier2 clusters + hook units only...
  rem V8 does not alter the generic AOT memory ABI. Keep the 234-unit corpus cached;
  rem only Tier2 clusters and their 10 hook units need fresh codegen.
  del /s /q "%BUILD%\*vcs_tier2_cluster*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0043*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0044*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0084*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0085*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0086*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0129*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0154*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0155*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0157*.obj" >nul 2>&1
  del /s /q "%BUILD%\*generated_unit_0158*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V7_ARCH_FASTMEM_STAMP%" (
  echo.
  echo [0c-v7fastmem/7] V7 ARCH FASTMEM - one-time AOT memory-model rebuild...
  rem This is intentionally not a micro hotfix: guest_memory.hpp is inlined into every
  rem generated unit so all AOT objects must see the direct-fastmem address model.
  rem Geometry/Boundary keep their exact V4 Tier2 source; they only recompile against
  rem the new memory view. This one-time rebuild is required for a global CPU change.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster*.obj" >nul 2>&1
  del /s /q "%BUILD%\*guest_memory*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V81_CPU_LEAN_STAMP%" (
  echo.
  echo [0c-v81lean/7] V8.1 CPU LEAN - rolling back Geometry code bloat, keeping direct-fastmem...
  rem V8 runtime data shows the extra Geometry closure regresses heavy city CPU time.
  rem The lean direct-memory view is now pointer-only, so rebuild the seven small Tier2 objects.
  rem Generated AOT and DX12 objects are intentionally preserved.
  del /s /q "%BUILD%\*vcs_tier2_cluster*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V82_CPU_RUNTIME_LEAN_STAMP%" (
  echo.
  echo [0c-v82runtime/7] V8.2 CPU RUNTIME LEAN - one-time native-chain rebuild...
  rem runtime.hpp is inline in every generated AOT unit. Rebuild the corpus once so
  rem compile-time-known chains lose per-call diagnostic branches in production.
  rem Keep the build tree itself: Ninja reuses every unaffected dependency and cache.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster*.obj" >nul 2>&1
  del /s /q "%BUILD%\*runtime*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V822_STAMP%" (
  echo.
  echo [0c-v822fix/7] V8.2.2 CORRECTNESS RECOVERY - rebuilding VCS host correctness objects...
  rem V8.2.2 is based on the stable V8.2 runtime. Replace any V8.2.1 host
  rem objects but keep generated AOT, Tier2 and DX12 objects intact.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V823_NEWS_STAMP%" (
  echo.
  echo [0c-v823news/7] V8.2.3 NEWS AUDIO - rebuilding audio/profile correctness objects...
  rem Output2 ABI and host watermark only: keep generated AOT, Tier2 and DX12.
  del /s /q "%BUILD%\*audio_output*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V824_NEWS_PACING_STAMP%" (
  echo.
  echo [0c-v824news/7] V8.2.4 NEWS PACING - rebuilding profile/runtime correctness objects...
  rem Output2 guest-time pacing only. Keep generated AOT, Tier2, DX12 and the
  rem V8.2.3 host watermark object intact.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V825_NEWS_ATRAC_STAMP%" (
  echo.
  echo [0c-v825news/7] V8.2.5 NEWS ATRAC STREAM - rebuilding profile/runtime correctness objects...
  rem ATRAC remain-frame semantics plus rollback of the rejected V8.2.4
  rem Output2 catch-up experiment. Keep AOT, Tier2, DX12 and host watermark.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V826_SAVE_REPRO_STAMP%" (
  echo.
  echo [0c-v826save/7] V8.2.6 SAVE REPRO CAPTURE - rebuilding only checkpoint/trace host objects...
  rem No runtime.hpp, generated AOT, Tier2, Geometry or DX12 changes.  runtime.cpp
  rem only exposes current HLE identity to the diagnostic trace.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*runtime.cpp.obj" >nul 2>&1
  del /s /q "%BUILD%\*main.cpp.obj" >nul 2>&1
)


if exist "%BUILD%" if not exist "%CORRECTNESS_V826A_SAVE_REPRO_STAMP%" (
  echo.
  echo [0c-v826a/7] V8.2.6A SAVE REPRO PASSIVE RECOVERY - removing passive HLE instrumentation...
  rem Revert core runtime.cpp to the exact V8.2.5 hot path.  Diagnostic F8/F10
  rem edges are queued by the Win32 window thread and consumed once per vblank.
  rem Rebuild only files touched by this recovery; keep generated AOT/Tier2/DX12.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*runtime.cpp.obj" >nul 2>&1
  del /s /q "%BUILD%\*display_window*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V826B_SAVE_REPRO_STAMP%" (
  echo.
  echo [0c-v826b/7] V8.2.6B SAVE REPRO TRACE HOTFIX - rebuilding trace host objects only...
  rem Before F8/restore the runtime path remains V8.2.5/V8.2.6A.  The F10
  rem async-key fallback is active only after trace capture has been armed.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V827_SAVE_THREAD_STAMP%" (
  echo.
  echo [0c-v827save/7] V8.2.7 SAVE THREAD LIFECYCLE - rebuilding profile/runtime-log objects only...
  rem Fix sceKernelExitDeleteThread stack/object reclamation and migrate the
  rem already captured V8.2.6 checkpoint. Generated AOT/Tier2/DX12 are unchanged.
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%CORRECTNESS_V827A_SAVE_REPRO_GATE_STAMP%" (
  echo.
  echo [0c-v827a/7] V8.2.7A INTERNAL SAVE_REPRO GATE - rebuilding host config/diagnostic objects only...
  rem Normal gameplay has SAVE_REPRO disabled in VCSNative.ini. The internal
  rem restore script opts in explicitly; generated AOT/Tier2/DX12 are unchanged.
  del /s /q "%BUILD%\*vcs_config*.obj" >nul 2>&1
  del /s /q "%BUILD%\*display_window*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V84_AGGRESSIVE_CPU_STAMP%" (
  echo.
  echo [0c-v84cpu/7] V8.4 AGGRESSIVE CPU DIRECT - rebuilding the automatic AOT corpus once...
  rem V8.4 changes every translated memory site and LV.Q/SV.Q lowering. This is
  rem intentionally a full generated-AOT rebuild; DX12/media/correctness objects stay cached.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*main*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_codegen_main*.obj" >nul 2>&1
  del /s /q "%BUILD%\*codegen_main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V85_VFPU_FASTLANE_STAMP%" (
  echo.
  echo [0c-v85vfpu/7] V8.5 AGGRESSIVE VFPU FASTLANE - rebuilding generated AOT/VFPU helpers once...
  rem V8.5 changes generic VFPU lowering and the checked-in generated corpus only.
  rem Scheduler/timing, DX12, savedata, ATRAC and host I/O objects stay cached.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vfpu_tier2_tests*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_codegen_main*.obj" >nul 2>&1
  del /s /q "%BUILD%\*codegen_main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V86_RADIO_VFPU_CT2_STAMP%" (
  echo.
  echo [0c-v86/7] V8.6 RADIO IDENTITY + VFPU CT2 - rebuilding affected CPU/profile objects once...
  rem VFPU CT2 touches 100+ generated units and Allegrex helper templates; radio identity
  rem changes only vcs_profile. Keep DX12, media backends and scheduler objects cached.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vfpu_tier2_tests*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_profile*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_codegen_main*.obj" >nul 2>&1
  del /s /q "%BUILD%\*codegen_main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V88_TRUSTED_DISPATCH_STAMP%" (
  echo.
  echo [0c-v88/7] V8.8 EXTREME CPU TRUSTED DISPATCH - rebuilding AOT/core/Tier2 once...
  rem V8.8 changes nearly every static chain edge, Runtime outer dispatch and four compact leaves.
  rem Force one deterministic rebuild so an existing V8.7 checkout cannot retain stale objects.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_tier2_cluster_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_compact_leaves*.obj" >nul 2>&1
  del /s /q "%BUILD%\*runtime*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_codegen_main*.obj" >nul 2>&1
  del /s /q "%BUILD%\*codegen_main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V89_REGCACHE_STAMP%" (
  echo.
  echo [0c-v89/7] V8.9 EXTREME CPU REGISTER RESIDENCY - rebuilding generated AOT once...
  rem V8.9 changes register ownership in 230 generated units. Force those objects
  rem plus the runtime log to rebuild while keeping DX12/media/profile objects cached.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
  del /s /q "%BUILD%\*codegen_main*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%PERF_V810_RESIDENT_REGIONS_STAMP%" (
  echo.
  echo [0c-v810/7] V8.10.1 SCHEDULER-SAFE RESIDENT REGIONS - rebuilding AOT/regions/runtime once...
  rem V8.10 rewrites 11k cross-unit leaf calls and changes resident-register selection.
  del /s /q "%BUILD%\*generated_unit_*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_resident_regions*.obj" >nul 2>&1
  del /s /q "%BUILD%\*vcs_runtime_log*.obj" >nul 2>&1
)

if exist "%BUILD%" if not exist "%BOOTFIX_STAMP%" (
  echo.
  echo [0c/7] BOOTFIX revision changed - invalidating stale .obj/.pch once...
  del /s /q "%BUILD%\*.obj" >nul 2>&1
  del /s /q "%BUILD%\*.pch" >nul 2>&1
)

echo.
echo [1/7] Configuring persistent Ninja Release tree ^(clang-cl + lld-link^)...
"%CMAKE_EXE%" -S "%REPO%" -B "%BUILD%" -G Ninja ^
  "-DCMAKE_MAKE_PROGRAM=%NINJA_EXE%" ^
  "-DCMAKE_C_COMPILER=%CLANG_CL_EXE%" ^
  "-DCMAKE_CXX_COMPILER=%CLANG_CL_EXE%" ^
  "-DCMAKE_LINKER=%LLD_LINK_EXE%" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DPSPRECOMP_PROFILE=vcs ^
  -DPSPRECOMP_WINDOWS_GPU_BACKEND=DX12 ^
  -DPSPRECOMP_GENERATED_OPT_LEVEL=3 ^
  -DPSPRECOMP_LTO=ON ^
  -DPSPRECOMP_NATIVE_AVX2=ON ^
  -DPSPRECOMP_AOT_ASSUME_NO_WRITE_WATCH=ON ^
  -DPSPRECOMP_AOT_PRODUCTION_FASTPATHS=ON ^
  -DPSPRECOMP_RUNTIME_CHAIN_TELEMETRY=%PSPRECOMP_RUNTIME_CHAIN_TELEMETRY% ^
  -DPSPRECOMP_MSVC_CGTHREADS=0 ^
  -DPSPRECOMP_MSVC_MP_JOBS=1 ^
  -DPSPRECOMP_PROFILE_GUIDED_AOT=ON ^
  -DPSPRECOMP_HOT_GENERATED_OPT_LEVEL=3 ^
  -DPSPRECOMP_GENERATED_INLINE_LEVEL=0 ^
  -DPSPRECOMP_HOT_GENERATED_INLINE_LEVEL=3 ^
  -DPSPRECOMP_VCS_AOT_LTO=OFF ^
  -DPSPRECOMP_VCS_TIER2_DEEP_TELEMETRY=%PSPRECOMP_TIER2_DEEP_TELEMETRY% ^
  -DPSPRECOMP_BUILD_TESTS=ON ^
  -DPSPRECOMP_BUILD_PROFILE_TESTS=ON
if errorlevel 1 goto :FAIL

echo.
echo [2/7] Building VCSNative with Ninja...
"%CMAKE_EXE%" --build "%BUILD%" --parallel %JOBS% --target VCSNative
if errorlevel 1 goto :FAIL
>"%BOOTFIX_STAMP%" echo VCS Tier2 BOOTFIX 2026-08-16 v1
>"%SUPERBLOCK_STAMP%" echo VCS Tier2 V4 BUILDFIX3 2026-08-16
>"%AMD_COMPAT_STAMP%" echo VCS DX12 AMD UMA COMPAT 2026-08-16
>"%PERF_V5_STAMP%" echo VCS PERF V5 ASYNC VFPU 2026-08-16
>"%PERF_V5_STALLFIX_STAMP%" echo VCS PERF V5 GE ASYNC STALL-RACE FIX 2026-08-17
>"%PERF_V5_PRESENTFIX_STAMP%" echo VCS PERF V5 GE ASYNC PRESENTFIX 2026-08-17
>"%PERF_V5_SYNC_RECOVERY_STAMP%" echo VCS PERF V5 SYNC RECOVERY 2026-08-17
>"%PERF_V5_STABLE_RECOVERY2_STAMP%" echo VCS PERF V5 STABLE RECOVERY2 2026-08-17
>"%PERF_V6_ENTITY_LEAF_STAMP%" echo VCS PERF V6 ENTITY LEAF INLINE 2026-08-17
>"%PERF_V6_ENTITY_LEAF_FIX1_STAMP%" echo VCS PERF V6 ENTITY LEAF INLINE CRASHFIX1 2026-08-17
>"%PERF_V7_ARCH_FASTMEM_STAMP%" echo VCS PERF V7 ARCH FASTMEM 2026-08-17
>"%PERF_V8_CPU_FUSION_STAMP%" echo VCS PERF V8 CPU FUSION 2026-08-17
>"%PERF_V81_CPU_LEAN_STAMP%" echo VCS PERF V8.1 CPU LEAN 2026-08-17
>"%PERF_V82_CPU_RUNTIME_LEAN_STAMP%" echo VCS PERF V8.2 CPU RUNTIME LEAN 2026-08-17
>"%CORRECTNESS_V822_STAMP%" echo VCS V8.2.2 CORRECTNESS RECOVERY 2026-08-17
>"%CORRECTNESS_V823_NEWS_STAMP%" echo VCS V8.2.3 NEWS AUDIO FIX 2026-08-17
>"%CORRECTNESS_V824_NEWS_PACING_STAMP%" echo VCS V8.2.4 NEWS PACING FIX 2026-08-18
>"%CORRECTNESS_V825_NEWS_ATRAC_STAMP%" echo VCS V8.2.5 NEWS ATRAC STREAM FIX 2026-08-18
>"%CORRECTNESS_V826_SAVE_REPRO_STAMP%" echo VCS V8.2.6 SAVE REPRO CAPTURE 2026-08-18
>"%CORRECTNESS_V826A_SAVE_REPRO_STAMP%" echo VCS V8.2.6A SAVE REPRO PASSIVE RECOVERY 2026-08-18
>"%CORRECTNESS_V826B_SAVE_REPRO_STAMP%" echo VCS V8.2.6B SAVE REPRO TRACE HOTFIX 2026-08-18
>"%CORRECTNESS_V827_SAVE_THREAD_STAMP%" echo VCS V8.2.7 SAVE THREAD LIFECYCLE FIX 2026-08-18
>"%CORRECTNESS_V827A_SAVE_REPRO_GATE_STAMP%" echo VCS V8.2.7A INTERNAL SAVE_REPRO GATE 2026-08-18
>"%PERF_V84_AGGRESSIVE_CPU_STAMP%" echo VCS PERF V8.4 AGGRESSIVE CPU DIRECT 2026-08-18
>"%PERF_V85_VFPU_FASTLANE_STAMP%" echo VCS PERF V8.5 AGGRESSIVE VFPU FASTLANE 2026-08-18
>"%PERF_V86_RADIO_VFPU_CT2_STAMP%" echo VCS PERF V8.6 RADIO IDENTITY VFPU CT2 2026-08-18
>"%PERF_V88_TRUSTED_DISPATCH_STAMP%" echo VCS PERF V8.8 EXTREME CPU TRUSTED DISPATCH 2026-08-18
>"%PERF_V89_REGCACHE_STAMP%" echo VCS PERF V8.9 EXTREME CPU REGISTER RESIDENCY 2026-08-18
>"%PERF_V810_RESIDENT_REGIONS_STAMP%" echo VCS PERF V8.10.1 SCHEDULER SAFE RESIDENT REGIONS 2026-08-18

echo.
echo [2b/7] Building tests and DX12 probes...
"%CMAKE_EXE%" --build "%BUILD%" --parallel %JOBS% --target ^
  psprecomp_tests vcs_profile_tests vcs_config_tests audio_resampler_tests vfpu_tier2_tests ^
  vcs_bootstrap_paths_tests vcs_dx12_probe vcs_dx12_ge_probe
if errorlevel 1 goto :FAIL

echo.
echo [3/7] Running regression tests...
"%CTEST_EXE%" --test-dir "%BUILD%" --output-on-failure
if errorlevel 1 goto :TEST_FAIL

set "BIN=%BUILD%\bin\Release"
if not exist "%BIN%\VCSNative.exe" (
    echo ERROR: VCSNative.exe was not produced:
    echo   %BIN%\VCSNative.exe
    goto :FAIL
)

echo.
echo [4/7] DX12 device/swapchain probe...
"%BIN%\vcs_dx12_probe.exe"
if errorlevel 1 goto :DX12_FAIL

echo.
echo [5/7] GE compatibility probe...
set "PSPRECOMP_DX12_GE_STRICT=1"
set "PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=0"
set "PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW=0"
set "PSPRECOMP_DX12_PACKED_0115=0"
set "PSPRECOMP_DX12_NATIVE_INDEXED_DRAW=0"
set "PSPRECOMP_DX12_BATCH_MERGE=0"
"%BIN%\vcs_dx12_ge_probe.exe"
if errorlevel 1 goto :GE_FAIL
set "PSPRECOMP_DX12_GE_STRICT="

echo.
echo [6/7] GE production probe...
set "PSPRECOMP_DX12_GE_STRICT=1"
set "PSPRECOMP_GE_GPU_HW_CULL=1"
set "PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=0"
set "PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW=1"
set "PSPRECOMP_DX12_PACKED_0115=1"
set "PSPRECOMP_DX12_NATIVE_INDEXED_DRAW=1"
set "PSPRECOMP_DX12_BATCH_MERGE=1"
"%BIN%\vcs_dx12_ge_probe.exe"
if errorlevel 1 goto :GE_PROD_FAIL
set "PSPRECOMP_DX12_GE_STRICT="

echo.
echo [7/7] Installing current VCS config...
copy /Y "%PROFILE%\config\VCSNative.ini" "%BIN%\VCSNative.ini" >nul
if errorlevel 1 goto :FAIL

echo.
echo ================================================================
echo NINJA BUILD OK ^(clang-cl + lld-link^)
echo EXE:
echo   %BIN%\VCSNative.exe
echo.
echo Later runs reuse:
echo   %BUILD%
echo ================================================================
exit /b 0

:NO_VS
echo ERROR: Visual Studio 2022 with Desktop C++ tools was not found.
pause
exit /b 2
:VS_ENV_FAIL
echo ERROR: VsDevCmd.bat failed to initialize x64.
pause
exit /b 3
:NO_CLANG_CL
echo ERROR: clang-cl.exe was not found at "%LLVM_BIN%".
echo Install standalone LLVM ^(winget install LLVM.LLVM^) - the VS Build Tools'
echo bundled Clang component is not sufficient ^(pinned to LLVM 19.1.5, which
echo hangs 70+ minutes on generated_unit_0103.cpp^).
pause
exit /b 4
:NO_LLD_LINK
echo ERROR: lld-link.exe was not found at "%LLVM_BIN%".
echo Install standalone LLVM ^(winget install LLVM.LLVM^).
pause
exit /b 13
:NO_CMAKE
echo ERROR: CMake was not found.
pause
exit /b 5
:NO_NINJA
echo ERROR: ninja.exe was not found.
pause
exit /b 6

:NO_PYTHON3
echo ERROR: Python 3 was not found.
echo The Tier-2 superblock generator requires Python 3.
echo Tried: py -3 and python.
pause
exit /b 7
:TEST_FAIL
echo ERROR: regression tests failed.
pause
exit /b 8
:DX12_FAIL
echo ERROR: DX12 probe failed.
pause
exit /b 9
:GE_FAIL
echo ERROR: compatibility GE probe failed.
pause
exit /b 10
:GE_PROD_FAIL
echo ERROR: production GE probe failed.
pause
exit /b 11
:FAIL
echo.
echo ERROR: Ninja VCS build failed.
echo Send the FIRST real error shown above.
pause
exit /b 12
