# CTW profile

Profile for GTA: Chinatown Wars (ULUS-10490, NTSC-U). The recompiled game boots, plays its intro with music and is
playable in free roam, with a Vulkan renderer, SDL3 window/audio and gamepad or keyboard input.

Neither the game nor its recompiled code is in this repository: you supply your own copy and generate the corpus
locally (steps below). `game/`, `analysis/`, `generated/` and `generated_overlays/` are ignored by Git.

## Quick start

With the game data prepared and the corpus generated (next three sections), from the repository root:

```bash
make        # configure on first use, build ctw_boot
make run    # build and play
```

`ctw_boot` looks up `profiles/ctw/game` and `configs/` relative to the current directory, so run it from the
repository root (`make run` does). Other targets: `make software` (CPU rasterizer), `make test`, `make clean`.

## Supported executable

| Field | Value |
|-------|-------|
| Disc ID | ULUS-10490 |
| Module | `CTW` v1.1 |
| Load base / entry | `0x08804000` / `0x08804040` |
| Decrypted ELF SHA-256 | `d6e426a7776a100ff9e1f1a04cee2c5d848f41c69e5d3c9c859978887dcaccc0` |

No EBOOT or commercial game asset is included.

## Prepare local game data

Copy the extracted UMD's `PSP_GAME` into `profiles/ctw/game/` (ignored by Git) and place the decrypted executable at
`profiles/ctw/game/PSP_GAME/SYSDIR/EBOOT_DECRYPTED.ELF`.

PSPRecomp does not include decryption code. PPSSPP 1.20+ can dump the executable from your own copy: set
`DumpFileTypes = 1` under `[General]` in `PSP/SYSTEM/ppsspp.ini` (or enable the EBOOT dump option in the developer
tools menu), boot the game, and take `PSP/SYSTEM/DUMP/ULUS10490_EBOOT.BIN`. It also works without a display:

```bash
SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy timeout 20 PPSSPPSDL "<CTW>.iso"
```

## Generate the corpus

Build the framework tools in Release first; an unoptimized `psp_recomp` takes far longer on this corpus.

```bash
cmake -S . -B out/framework-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DPSPRECOMP_PROFILE=""
cmake --build out/framework-release

python3 profiles/ctw/tools/build_overlays.py
```

The script extracts the 166 code overlays from the game, recompiles each into `generated_overlays/`, and generates
the main corpus into `generated/` with the overlays as reference sources (see [Code overlays](#code-overlays)).
The CTW build needs both directories. The result is about 320 MB of C++.

To inspect the executable on its own:

```bash
ELF=profiles/ctw/game/PSP_GAME/SYSDIR/EBOOT_DECRYPTED.ELF
./out/framework-release/psp_analyze "$ELF" profiles/ctw/analysis/ULUS10490_report.json
```

Analyzer findings: 13,845 functions (5,169 reached only through pointers in data sections: vtables, callbacks and
switch jump tables), 256 imports across 30 libraries, 0 unsupported instructions.

## Build

`make`, or directly:

```bash
cmake -S . -B out/ctw -G Ninja -DCMAKE_BUILD_TYPE=Release -DPSPRECOMP_PROFILE=ctw
cmake --build out/ctw --target ctw_boot
./out/ctw/bin/Release/ctw_boot
```

The first build compiles the whole corpus and takes several minutes. See [Build dependencies](#build-dependencies).

## Status

`ctw_boot` runs on the shared HLE library (kernel, I/O, system, display, GE, audio + ATRAC3+ music, controller,
savedata utility), the Vulkan renderer and the recompiled overlays. The game shows the legal screen and loading art,
plays the comic and Bink intro with music, runs the trunk L/R minigame and reaches free-roam gameplay (swimming, HUD,
radar, day/night clock). Scripted soak runs of up to 17 minutes of game time finish without errors or unimplemented
imports. Gameplay speed on the development machine (desktop CPU, RTX 5060): about 950 vblanks/s with Vulkan, 115
with the software rasterizer (60 = real time). CTW does not use `sceMpeg`; its movies are Bink, decoded by the game
itself.

Not implemented yet: the on-screen keyboard and network dialogs (`sceUtility`), a direct swapchain (frames are read
back and presented through SDL), and fog in the software rasterizer.

An import without an HLE implementation stops the run; `CTW_PERMISSIVE_HLE=1` logs each one once and returns 0
instead.

## Headless and scripted runs

Scripted input reaching gameplay headless (Cross every 4 s, then alternating L/R, then the stick):

```sh
CTW_HEADLESS=1 CTW_PERMISSIVE_HLE=1 \
PSPRECOMP_CTRL_PULSE_BUTTONS=0x4000 PSPRECOMP_CTRL_PULSE_START_VBLANK=1200 PSPRECOMP_CTRL_PULSE_END_VBLANK=1206 PSPRECOMP_CTRL_PULSE_REPEAT_EVERY=240 \
PSPRECOMP_CTRL_PULSE2_BUTTONS=0x100 PSPRECOMP_CTRL_PULSE2_START_VBLANK=9500 PSPRECOMP_CTRL_PULSE2_END_VBLANK=9502 PSPRECOMP_CTRL_PULSE2_REPEAT_EVERY=10 \
PSPRECOMP_CTRL_PULSE3_BUTTONS=0x200 PSPRECOMP_CTRL_PULSE3_START_VBLANK=9505 PSPRECOMP_CTRL_PULSE3_END_VBLANK=9507 PSPRECOMP_CTRL_PULSE3_REPEAT_EVERY=10 \
PSPRECOMP_CTRL_PULSE4_START_VBLANK=10600 PSPRECOMP_CTRL_PULSE4_END_VBLANK=16000 PSPRECOMP_CTRL_PULSE4_LX=200 PSPRECOMP_CTRL_PULSE4_LY=0 \
PSPRECOMP_STOP_VBLANK=16000 CTW_DUMP_VBLANKS=11000,15000 ./out/ctw/bin/Release/ctw_boot
```

CTW turns on kernel behaviours that VCS does not use (all off by default):

| Setting | Why CTW needs it |
|---------|------------------|
| `minimum_thread_delay_us = 200` | polling loops use `sceKernelDelayThread(0)` to let lower-priority threads run |
| `notify_device_callbacks` | `SysManager` waits for its power/UMD/memory-stick callbacks |
| `honor_wait_timeouts` | the resource manager marks itself idle when a 15 ms `WaitSema` times out; the loading screen is stopped through a timed `WaitThreadEnd` |
| `release_deleted_thread_stacks` | the main heap is sized from `sceKernelMaxFreeMemSize` after bootstrap threads exit-delete |
| `periodic_vblank_interrupts` | the vblank handler wakes worker threads (e.g. `memstick`) while nobody waits for vblank |

Scripted input for headless runs uses the shared controller's pulses, e.g. press Cross (0x4000) at vblanks 1000-1006:

```bash
PSPRECOMP_CTRL_PULSE_BUTTONS=0x4000 PSPRECOMP_CTRL_PULSE_START_VBLANK=1000 PSPRECOMP_CTRL_PULSE_END_VBLANK=1006 \
  CTW_DUMP_VBLANKS=1100 ./out/ctw/bin/Release/ctw_boot
```

PPSSPP is a useful oracle for divergences: `PPSSPPHeadless "<CTW>.iso" -l --timeout=90` logs every HLE call, which can
be compared with `CTW_IMPORT_TRACE`.

## Build dependencies

| Package (Arch / SteamOS) | Used for | Without it |
|--------------------------|----------|------------|
| `sdl3` | window, gamepad, audio output | headless only |
| `ffmpeg` | ATRAC3+ music (`psprecomp_hle_atrac`) | build fails (required) |
| `vulkan-headers`, `vulkan-icd-loader` | Vulkan renderer (`psprecomp_hle_gpu_vulkan`) | software rendering only |

`glslc` (shaderc) is only needed after editing `src/hle/shaders/`: run `python3 tools/compile_vulkan_shaders.py`
to refresh the embedded SPIR-V (`src/hle/ge_gpu_vulkan_shaders.inc`).

### Windows (MinGW)

Built and played on Windows with GCC 13 (MinGW-w64, UCRT) and Ninja, with the dependencies from vcpkg; no source
changes are needed. The VCS profile is a separate matter: it needs MSVC and DirectX 12.

```bash
vcpkg install "ffmpeg[avcodec,avformat,swresample]:x64-mingw-dynamic" sdl3:x64-mingw-dynamic \
  vulkan-headers:x64-mingw-dynamic vulkan-loader:x64-mingw-dynamic --host-triplet=x64-mingw-dynamic

V=<vcpkg root>
cmake -S . -B out/ctw -G Ninja -DCMAKE_BUILD_TYPE=Release -DPSPRECOMP_PROFILE=ctw \
  -DCMAKE_TOOLCHAIN_FILE=$V/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DVCPKG_HOST_TRIPLET=x64-mingw-dynamic \
  -DPKG_CONFIG_EXECUTABLE=$V/installed/x64-mingw-dynamic/tools/pkgconf/pkgconf.exe
cmake --build out/ctw
out/ctw/bin/Release/ctw_boot.exe     # from the repository root
```

- Keep the compiler's `bin` directory first on `PATH` when building and running. Git for Windows ships its own
  `libstdc++-6.dll`; if that one is found first, every GCC-built executable fails at startup with `0xc0000139`
  (entry point not found). vcpkg copies the FFmpeg, SDL3 and Vulkan DLLs next to the executables.
- `tools/build_overlays.py` needs Python. A corpus generated on another machine from the same executable works
  unchanged.
- The root `Makefile` is for Linux; use the CMake commands above.

## Playing

With SDL3 installed, `ctw_boot` opens a window, reads gamepads and the keyboard, and plays audio. It falls back to
headless when `CTW_HEADLESS=1` is set or no display is available.

| PSP | Gamepad (Steam Deck layout) | Keyboard |
|-----|-----------------------------|----------|
| Cross / Circle / Square / Triangle | A / B / X / Y | K or Space / L / J / I |
| L / R | LB or LT / RB or RT | Q / E |
| D-pad, analog | D-pad, left stick | arrows, WASD (Shift = half tilt) |
| Start / Select | Start / Back | Enter / Backspace |

`CTW_FULLSCREEN=1` starts fullscreen (F11 toggles), `CTW_SCALE=n` sets the window scale, `CTW_FILTER=nearest` keeps
pixels sharp, and `CTW_UNLIMITED=1` removes the 60 Hz frame limiter. `CTW_SPEED_REPORT=1` prints emulated vblanks per
wall-clock second (60 = real time).

## Renderer

Graphics render on the GPU through the shared Vulkan GE backend (`src/hle/ge_gpu_vulkan.cpp`, Vulkan 1.3: the Steam
Deck's RADV and current desktop drivers). Every PSP draw, render-to-texture pass and post effect is replayed at an
internal resolution of 480x272 times `CTW_RENDER_SCALE` (default 2; 3 gives 1440x816), with vertex transform on the
GPU. The displayed image is read back once per frame and presented through SDL, so there is no swapchain to set up
and the same path works windowed and headless. If Vulkan is unavailable the software rasterizer takes over.

| Variable | Effect |
|----------|--------|
| `CTW_RENDERER=software` | use the CPU rasterizer (480x272, slower; it has no fog, unlike the PSP and the GPU path) |
| `CTW_RENDER_SCALE=n` | internal resolution multiplier, 1-8 |
| `CTW_HW_TRANSFORM=0` | transform vertices on the CPU instead (identical output, more CPU time) |
| `CTW_GPU_SHADOW=1` | also run the CPU rasterizer into guest memory, for anything that reads VRAM back |
| `CTW_VK_VALIDATION=1` | enable `VK_LAYER_KHRONOS_validation` when installed |

At 1x the GPU output matches the software rasterizer to within edge rasterization (under 0.6% of pixels differ by more
than 8 levels), apart from fog. Bisecting a GPU/software difference down to one PRIM:
`PSPRECOMP_GE_DUMP_AT=vblank,n` (software: framebuffer after the n-th PRIM of that vblank) against
`PSPRECOMP_VK_MAX_DRAW=vblank+1,n` with `CTW_DUMP_VBLANKS=vblank+1` (GPU: replay only up to it);
`PSPRECOMP_VK_READBACK_TARGET=addr` presents another render target and `CTW_DUMP_VRAM=addr,stride,w,h` saves guest
memory, for render-to-texture passes.

## Music

`sceAtrac3plus` is the shared `psprecomp_hle_atrac` module (also used by VCS): the RIFF header the game streams in is
matched against the `.AT3` files on the disc (`WAD/*.AT3`: music, radio stations, ambience) and that file is decoded
with FFmpeg. `CTW_AUDIO_DUMP=out.wav` writes everything played, mixed by guest time, which is how it was verified
headless (bit-exact against a direct FFmpeg decode, at the right speed).

## Diagnostics

```bash
CTW_DUMP_VBLANKS=30,1200 ./out/ctw/bin/Release/ctw_boot        # displayed image (GPU or framebuffer) as PPMs in captures/
CTW_AUDIO_DUMP=captures/audio.wav ./out/ctw/bin/Release/ctw_boot # audio output as WAV
CTW_WINDOW_DUMP=600,shot.bmp ./out/ctw/bin/Release/ctw_boot     # what the window shows at the 600th present
CTW_PRESENT_LOG=1 ./out/ctw/bin/Release/ctw_boot                # source and brightness of every presented frame
CTW_IMPORT_TRACE=400 ./out/ctw/bin/Release/ctw_boot             # last 400 non-polling imports with thread/args
CTW_TRACE_SKIP=sceAudio,Atrac ...                                 # leave more imports out of that trace
CTW_BREAK_NULL_READ=1 ./out/ctw/bin/Release/ctw_boot            # backtrace at the first sceIoRead into NULL
PSPRECOMP_HLE_HISTOGRAM=1 ./out/ctw/bin/Release/ctw_boot       # import call counts
PSPRECOMP_WATCH_WRITE=0x... ./out/ctw/bin/Release/ctw_boot     # guest writes to an address
```

At exit it also prints thread states and the semaphore table.

## Code overlays

CTW loads code at runtime: 166 fixed-address ELF modules (HUD, PDA apps, minigames, mission scripts) packed in
`WAD/OVERLAY/CTW_PSP_UMD_MASTER_SCEA/OVERLAYS.PAK` and loaded into two slots, `0x08B87460` (114 overlays) and
`0x08BB2480` (52). The archive is a `PK02` sector table followed by `10SA` chunks (`[magic][compressed size]
[uncompressed size][zlib]`), each inflating to one overlay ELF whose single section is named `.overlay_<name>`.

```bash
python3 profiles/ctw/tools/build_overlays.py
```

extracts every overlay to `game/overlays/<name>.elf`, recompiles each with `psp_recomp --overlay` into
`generated_overlays/<name>/` (namespace `ovl_<name>`, units bucketed like the main corpus), writes
`generated_overlays/overlay_table.cpp` (slot, size, 64-byte fingerprint, registration function), and regenerates the
main corpus with the overlays as reference sources so main functions that only overlays call become entries.

At run time `host/ctw_overlays.cpp` installs a runtime overlay resolver: entering a slot whose resident bytes no
longer match the active overlay unregisters the slot and registers the matching corpus. Overlay entries are
registered as non-chainable so every entry passes that check.
