# VCS profile

This profile builds `VCSNative.exe` for the supported GTA: Vice City Stories PSP executable. It contains the title-specific generated AOT corpus, HLE/profile code, DX12 renderer integration, audio/input support and configuration required by the current native build.

## Quick build on Windows

Run `BUILD_VCS.bat` from this directory for the normal performance build. Generated AOT stays at the maximum MSVC optimization level with AVX2, compiler `/MP`, MSBuild `/m` and persistent incremental objects. Host/runtime code may use whole-program optimization, but the 234 generated AOT translation units are explicitly compiled with `/GL-` so the final linker does not have to hold the entire generated corpus as LTCG IR. The current runtime fast paths remain enabled.

`BUILD_VCS_FAST.bat` remains the corresponding fast incremental pipeline: generated AOT at O2 with LTCG disabled. No separate experimental max-performance build is used.

The launchers automatically locate CMake from PATH, Visual Studio 2022 (including the bundled CMake component), `vswhere`, or a standard standalone CMake installation. `BUILD_VCS_FAST.bat` remains the quickest development/debug-oriented build.

For an alternate toolchain that provide build using LLVM's clang-cl/lld-link instead of MSVC, see [MSVC+clang (clang-cl / lld-link) build](#msvcclang-clang-cl--lld-link-build) below.

For the single-configuration Ninja workflow used during incremental renderer and
generated-code development, see [`../../docs/VCS_NINJA_BUILD.md`](../../docs/VCS_NINJA_BUILD.md).


## Source layout

```text
config/       VCS profile configuration and executable metadata
data/         Redistributable generated profile data
generated/    AOT C++ generated from the supported executable
host/         VCS HLE, bootstrap, renderer, audio, input and native fast paths
scripts/      Maintained Windows build/run/benchmark scripts
tests/        Profile regression tests
tools/        VCS-specific generator and maintenance tools
third_party/  Bundled dependencies and license notices
progress/     Local handoffs/history; ignored by Git
```

## Supported executable

The profile targets the ULUS-10160 PSP release currently used by the generated corpus. No EBOOT or commercial game asset is included.

## Prepare local game data

The runtime expects a decrypted ELF and the game's `PSP_GAME/USRDIR` data from your own copy. This repository does not include decryption code.

Use:

```powershell
.\profiles\vcs\tools\prepare_game.ps1 `
  -ExtractedUmdRoot D:\VCS_EXTRACTED `
  -DecryptedElf D:\local\EBOOT_DECRYPTED.ELF
```

The default destination is `profiles/vcs/game`, which is ignored by Git.

## Build on Windows

Fast development build:

```text
profiles\vcs\scripts\build_fast.bat
```

Optimized release build:

```text
profiles\vcs\scripts\build_release.bat
```

Run with the default local game directory:

```text
profiles\vcs\scripts\play.bat
```

Or pass a game root explicitly:

```text
profiles\vcs\scripts\play.bat D:\VCS_GAME_ROOT
```

For an uncapped CPU/GE measurement, use `profiles\vcs\scripts\bench.bat`.

### MSVC+clang (clang-cl / lld-link) build

An alternate Windows toolchain is available using LLVM's clang-cl compiler and lld-link linker instead of MSVC's cl.exe/link.exe. This pipeline currently provides a performance uplift compared to the standard MSVC build.

This setup requires a **standalone LLVM installation** (e.g. `winget install LLVM.LLVM` or https://releases.llvm.org/), do not use Clang bundled with Visual Studio Build Tools as it is an older release with a known compiler hang. A VS2022 Build Tools install is still required to supply the Windows SDK and MSVC STL headers. Because clang-cl does not integrate cleanly with MSBuild, these scripts configure the build using the Ninja generator.

Fast development build (O2, LTO disabled):

```text
profiles\vcs\BUILD_VCS_FAST_CLANGCL.bat

```

Optimized release build (O3, PGO, AVX2, tier-2 transforms, automated ctest/DX12 probes):

```text
profiles\vcs\BUILD_VCS_WIN_DX12_CLANGCL.bat

```

Run with the default local game directory:

```text
profiles\vcs\PLAY_VCS_CLANGCL.bat

```

*Note:* Both scripts output to isolated build folders (`out\vcs-fast-clangcl` and `out\vcs-release-ninja-clangcl`). The play script automatically uses the release executable first, falling back to the fast build if missing.

### Windows link memory

The generated VCS corpus is intentionally excluded from MSVC whole-program IR in the normal release build. Later AOT cross-unit optimizations make whole-program analysis of all generated units unnecessarily expensive in linker memory. This does not disable per-unit `/Ox`; it only prevents those generated units from being deferred to link-time code generation. Host/runtime LTCG remains available, and the release linker prints LTCG status while it runs.

## Resolution configuration

`profiles/vcs/config/VCSNative.ini` exposes both the presentation resolution and the internal render resolution.

`[Display] ResolutionMode` accepts `PSP`, `Desktop` or `Custom`. `Custom` uses `Width` and `Height`. With `Fullscreen=true`, VCSNative uses a borderless desktop-sized window; custom width/height still define the logical output surface used by the presentation/widescreen configuration.

`[Rendering] InternalResolutionMode` accepts `PSP`, `Scale`, `Desktop` or `Custom`. `Scale` uses `InternalScale`; `Custom` uses `InternalWidth` and `InternalHeight`. These settings control the native render target independently from the presentation window.

## Regenerate VCS AOT code

The VCS profile keeps an address-aware generator target separate from the generic framework generator:

```text
vcs_recomp
```

Its source lives in `profiles/vcs/tools/vcs_codegen_main.cpp`. Profile-only lowering and measured native leaves belong there or under `host/`; they do not belong in the root PSPRecomp generator/runtime.

## Development history

Old stage reports, validation notes and handoffs are stored under `profiles/vcs/progress`. That directory is intentionally ignored by the repository so development history does not pollute the public source tree.

### Internal SAVE_REPRO diagnostics

`VCSNative.ini` ships with `[Testing] SaveRepro=false`. This keeps the F8 checkpoint and F10 trace harness completely unavailable during normal play. Developers may temporarily set it to `true` for controlled debugging. `RESTORE_SAVE_REPRO.bat` explicitly opts the harness in for its own process and does not require changing the normal INI. These checkpoints are diagnostic snapshots, not supported gameplay save states.
