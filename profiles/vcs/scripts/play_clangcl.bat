@echo off
setlocal EnableExtensions
for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
for %%I in ("%~dp0..") do set "PROFILE=%%~fI"

set "BIN=%REPO%\out\vcs-release-ninja-clangcl\bin\Release\VCSNative.exe"
if not exist "%BIN%" set "BIN=%REPO%\out\vcs-fast-clangcl\bin\Release\VCSNative.exe"
if not exist "%BIN%" (
  echo VCSNative.exe not found. Run BUILD_VCS_WIN_DX12_CLANGCL.bat or BUILD_VCS_FAST_CLANGCL.bat first.
  exit /b 3
)

set "GAME=%~1"
if "%GAME%"=="" if exist "%PROFILE%\game\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%PROFILE%\game"
if "%GAME%"=="" if exist "%~dp0PSP_DATA\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%~dp0PSP_DATA"
if "%GAME%"=="" if exist "%REPO%\out\vcs-dev\bin\Release\PSP_DATA\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF" set "GAME=%REPO%\out\vcs-dev\bin\Release\PSP_DATA"
if "%GAME%"=="" (
  echo Game root not found. Pass it as the first argument or run prepare_game.ps1.
  exit /b 4
)
set "ELF=%GAME%\PSP_GAME\SYSDIR\EBOOT_DECRYPTED.ELF"
if not exist "%ELF%" (
  echo Missing %ELF%
  exit /b 5
)
if not exist "%GAME%\PSP_GAME\USRDIR\RUNDATA\PSP\MOVIES\LOGO.PMF" (
  echo The VCS game root is incomplete: LOGO.PMF is missing.
  exit /b 6
)
if not exist "%GAME%\PSP_GAME\USRDIR\RUNDATA\PSP\MOVIES\TITLES.PMF" (
  echo The VCS game root is incomplete: TITLES.PMF is missing.
  exit /b 6
)

set "PSPRECOMP_CONFIG=%PROFILE%\config\VCSNative.ini"
set "PSPRECOMP_GE_BACKEND=directx12"
set "PSPRECOMP_GE_GPU_TELEMETRY=0"
set "PSPRECOMP_GE_GPU_REPORT=0"
set "PSPRECOMP_GE_ASYNC=0"
set "PSPRECOMP_GE_GPU_SKIP_SOFTWARE_RASTER="
set "PSPRECOMP_CHAIN_DEPTH="
set "PSPRECOMP_TIME_TICK_DISPATCHES="
set "PSPRECOMP_GE_GPU_HW_CULL=1"
set "PSPRECOMP_DX12_DEBUG=0"
set "PSPRECOMP_DX12_GE_READBACK=0"
set "PSPRECOMP_DX12_GE_STRICT=0"
set "PSPRECOMP_GE_PARALLEL_VERTEX_DECODE=0"
set "PSPRECOMP_RASTER_THREADS=4"
set "PSPRECOMP_GE_DIRECT_NONINDEXED_DRAW=1"
set "PSPRECOMP_DX12_PACKED_0115=1"
set "PSPRECOMP_DX12_NATIVE_INDEXED_DRAW=1"
set "PSPRECOMP_DX12_BATCH_MERGE=1"
set "PSPRECOMP_ENABLE_FAST_088B1554=1"
set "PSPRECOMP_GE_GPU_DUAL_FRAME=0"

echo Launching clang-cl + lld-link build: %BIN%
"%BIN%" "%ELF%" "%GAME%"
exit /b %ERRORLEVEL%
