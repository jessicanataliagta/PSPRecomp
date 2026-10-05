#pragma once

// CTW code overlays: 166 fixed-address modules from OVERLAYS.PAK that the game
// loads into two slots (0x08B87460 and 0x08BB2480). Each is recompiled ahead of
// time (tools/build_overlays.py); at run time the resolver registers the corpus
// whose fingerprint matches the code resident in the slot being entered.

#include "psprecomp/runtime.hpp"

#include <array>
#include <cstdint>
#include <span>

namespace ctw {

struct OverlayInfo {
    const char *name;
    std::uint32_t base;
    std::uint32_t size;  // memory size, including .bss
    std::array<std::uint32_t, 16> fingerprint;  // first 64 bytes of the image
    void (*register_functions)(psprecomp::Runtime &runtime);
};

// Generated table (generated_overlays/overlay_table.cpp).
std::span<const OverlayInfo> overlays();

// Installs the slot resolver on `runtime`.
void install_overlay_manager(psprecomp::Runtime &runtime);

} // namespace ctw
