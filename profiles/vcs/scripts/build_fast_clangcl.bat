@echo off
setlocal EnableExtensions EnableDelayedExpansion
for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
set "BUILD=%REPO%\out\vcs-fast-clangcl"
call "%~dp0pick_jobs.bat"

rem Uses Ninja (like BUILD_VCS_WIN_DX12_CLANGCL.bat) because clang-cl doesn't 
rem integrate with MSBuild. Lower opt level and no LTO speed up rebuilds. 
rem Uses standalone LLVM 22 (see scripts\build_release_ninja_clangcl.bat).
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

echo Initializing VS2022 x64 environment (headers/libs only - compiler is clang-cl)...
call "%VSROOT%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 goto :VS_ENV_FAIL

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

echo ================================================================
echo VCS - FAST INCREMENTAL BUILD (clang-cl + lld-link)
echo Compiler: %CLANG_CL_EXE%
echo Linker:   %LLD_LINK_EXE%
echo CMake: %CMAKE_EXE% ^| Ninja: %NINJA_EXE% ^| Workers: %JOBS% ^| LTO: OFF
echo ================================================================
echo Reapplying profile-guided Tier-2 source transforms...
call "%REPO%\profiles\vcs\APPLY_TIER2_EXTREME.bat"
if errorlevel 1 goto :FAIL

"%CMAKE_EXE%" -S "%REPO%" -B "%BUILD%" -G Ninja ^
  "-DCMAKE_MAKE_PROGRAM=%NINJA_EXE%" ^
  "-DCMAKE_C_COMPILER=%CLANG_CL_EXE%" ^
  "-DCMAKE_CXX_COMPILER=%CLANG_CL_EXE%" ^
  "-DCMAKE_LINKER=%LLD_LINK_EXE%" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DPSPRECOMP_PROFILE=vcs ^
  -DPSPRECOMP_WINDOWS_GPU_BACKEND=DX12 ^
  -DPSPRECOMP_GENERATED_OPT_LEVEL=2 ^
  -DPSPRECOMP_LTO=OFF ^
  -DPSPRECOMP_VCS_AOT_LTO=OFF ^
  -DPSPRECOMP_BUILD_TESTS=ON ^
  -DPSPRECOMP_BUILD_PROFILE_TESTS=ON
if errorlevel 1 goto :FAIL

"%CMAKE_EXE%" --build "%BUILD%" --parallel %JOBS% --target ^
  VCSNative psprecomp_tests vcs_config_tests audio_resampler_tests vfpu_tier2_tests vcs_bootstrap_paths_tests vcs_dx12_probe vcs_dx12_ge_probe
if errorlevel 1 goto :FAIL

copy /Y "%REPO%\profiles\vcs\config\VCSNative.ini" "%BUILD%\bin\Release\VCSNative.ini" >nul
echo BUILD FAST OK (clang-cl + lld-link): %BUILD%\bin\Release\VCSNative.exe
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
echo Install standalone LLVM (winget install LLVM.LLVM).
pause
exit /b 4
:NO_LLD_LINK
echo ERROR: lld-link.exe was not found at "%LLVM_BIN%".
echo Install standalone LLVM (winget install LLVM.LLVM).
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
:FAIL
echo ERROR: VCS fast clang-cl build failed. Send the FIRST real compiler error above.
pause
exit /b 7
