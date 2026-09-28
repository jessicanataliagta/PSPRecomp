@echo off
setlocal EnableExtensions
for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
for %%I in ("%~dp0..") do set "PROFILE=%%~fI"

set "BIN=%REPO%\out\vcs-release-ninja-clangcl-v812\bin\Release\VCSNative.profile.exe"
if not exist "%BIN%" set "BIN=%REPO%\out\vcs-release-ninja-clangcl-v812\bin\Release\VCSNative.exe"
if not exist "%BIN%" set "BIN=%REPO%\out\vcs-release-ninja\bin\Release\VCSNative.exe"
if not exist "%BIN%" set "BIN=%REPO%\out\vcs-release\bin\Release\VCSNative.exe"
if not exist "%BIN%" set "BIN=%REPO%\out\vcs-fast\bin\Release\VCSNative.exe"
if not exist "%BIN%" (
  echo VCSNative.exe not found. Build the VCS profile first.
  echo.
  pause
  exit /b 3
)

set "GAME=%~1"
if "%GAME%"=="" if exist "%PROFILE%\game\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%PROFILE%\game"
if "%GAME%"=="" if exist "%REPO%\out\vcs-release-ninja-clangcl-v812\bin\Release\PSP_DATA\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%REPO%\out\vcs-release-ninja-clangcl-v812\bin\Release\PSP_DATA"
if "%GAME%"=="" if exist "%REPO%\out\vcs-release-ninja\bin\Release\PSP_DATA\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%REPO%\out\vcs-release-ninja\bin\Release\PSP_DATA"
if "%GAME%"=="" (
  echo Game root not found. PSP_DATA was not found beside VCSNative.exe.
  echo Pass PSP_DATA as the first argument or run prepare_game.ps1.
  echo.
  pause
  exit /b 4
)
set "ELF=%GAME%\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF"
if not exist "%ELF%" (
  echo Missing %ELF%
  echo.
  pause
  exit /b 5
)

set "PSPRECOMP_CONFIG=%PROFILE%\config\VCSNative.ini"
set "PSPRECOMP_GE_BACKEND=directx12"
set "PSPRECOMP_GE_GPU_SKIP_SOFTWARE_RASTER=1"
set "PSPRECOMP_GE_GPU_HW_CULL=1"
set "PSPRECOMP_TIME_TICK_DISPATCHES=4096"
set "PSPRECOMP_DX12_DEBUG=0"
set "PSPRECOMP_DX12_GE_READBACK=0"
set "PSPRECOMP_DX12_GE_STRICT=0"
set "PSPRECOMP_DX12_TEXTURE_UPLOAD_RING=1"
set "PSPRECOMP_GE_ASYNC=0"
set "PSPRECOMP_V8155_ENABLE_ASYNC="
set "PSPRECOMP_DX12_EXECUTE_INDIRECT=0"
set "PSPRECOMP_GUEST_HOTSPOT=1"
set "PSPRECOMP_FRAME_LIMIT=0"
set "PSPRECOMP_FRAME_TIME_DIAG="
set "PSPRECOMP_GE_PHASE_DIAG=1"
set "PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=0"
set "PSPRECOMP_GE_PARALLEL_VERTEX_THRESHOLD=768"
set "PSPRECOMP_GE_PARALLEL_VERTEX_MAX_WORKERS=6"
set "PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW=1"
set "PSPRECOMP_DX12_PACKED_0115=1"
set "PSPRECOMP_DX12_NATIVE_INDEXED_DRAW=1"
set "PSPRECOMP_DX12_BATCH_MERGE=1"
set "PSPRECOMP_CHAIN_DEPTH=1024"
set "PSPRECOMP_ENABLE_FAST_088B1554=1"
set "PSPRECOMP_GE_GPU_DUAL_FRAME=0"

echo Starting VCS CPU/GE profiling run.
echo Game root: %GAME%
echo Drive through the city for 30-60 seconds, then close the game normally.
echo The profiler intentionally lowers FPS while collecting hotspots.
echo.
"%BIN%" "%ELF%" "%GAME%"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
  echo.
  echo VCSNative exited with error code %RESULT%.
  pause
)
exit /b %RESULT%
