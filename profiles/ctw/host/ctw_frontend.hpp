#pragma once

// Host frontend for CTW: an SDL3 window presenting the PSP framebuffer, gamepad
// and keyboard input for sceCtrl, and audio output for sceAudio. Runs headless
// (no window, silent, scripted input only) when CTW_HEADLESS=1, when SDL cannot
// open a display, or when the build has no SDL3.

#include "psprecomp/guest_memory.hpp"
#include "psprecomp/hle/audio.hpp"
#include "psprecomp/hle/ctrl.hpp"

#include <cstddef>
#include <cstdint>

namespace ctw {

// Opens the window, audio device and gamepads. Returns false when running headless.
bool frontend_start();
void frontend_shutdown();
[[nodiscard]] bool frontend_active();

// An RGBA8 image rendered by the GPU backend, at its internal resolution.
struct GpuImage {
    const std::byte *rgba{};
    std::uint32_t width{};
    std::uint32_t height{};
};

// Presents `gpu` when given, else the guest's displayed framebuffer, and pumps
// events. Returns false once the user closed the window.
bool frontend_present(const psprecomp::GuestMemory &memory, const GpuImage *gpu = nullptr);

// Hooks for the shared HLE; all are safe (no-op / neutral) while headless.
psprecomp::hle::CtrlHooks frontend_ctrl_hooks();
psprecomp::hle::AudioHooks frontend_audio_hooks();

} // namespace ctw
