#pragma once

// GTA Vice City Stories (PSP, ULUS-10160 v1.03) - the game's own DISPLAY-page
// preferences forced from VCSNative.ini:
//
//   [Game]
//   Subtitles = true|false   ; SUBTITLES row of the pause menu's DISPLAY page
//   Hud       = true|false   ; HUD MODE row (health/armour/money/weapon/clock;
//                            ; the radar has its own RADAR MODE setting)
//
// A key that is absent leaves the game's own value (menu / save) untouched.
//
// Where the values live (load base 0x08804000, gp = 0x08BB1D60, i.e. the data
// segment start 0x08BA9D70 + 0x7FF0 from the module info):
//
//   DisplayPrefs  = *(u32 *)0x08BB3454   (gp+0x16F4), heap object, 0x50 bytes
//   +0x04  vtable 0x08BA6250             (set by the constructor 0x089C6328)
//   +0x08  u8 ShowSubtitles  1 = on      (getter 0x089C6D68, setter 0x089C6D70)
//   +0x0C  u32 Brightness    default 288 (getter 0x089C6D78) - not forced
//   +0x13  u8 RadarMode                  (getter 0x089C6D88) - not forced
//   +0x18  u8 HudMode        1 = on      (getter 0x089C6DBC, setter 0x089C6DC4)
//
// Evidence, read from the ELF: the object is allocated (0x50 bytes) and
// published to gp+0x16F4 at 0x08B65E44, its defaults are written by
// 0x089C6D00 (+0x08 = 0, +0x18 = 1). The front end's per-GameHook value
// switch at 0x089C68E0 (jump table 0x08B7F038) dispatches HOOK_SHOW_SUBTITLES
// (3) to the +0x08 getter and HOOK_HUD_MODE (6) to the +0x18 getter. The HUD
// draw at 0x089BE060 skips drawing CHud's message buffer (CHud+0x18) when
// +0x08 is 0. The layout matches the VCSPC project's docs/VCS_ADDRESSES.md
// ("The game's own SUBTITLES and HUD MODE"), which confirmed both by writing
// them in a running game. Addresses only; no VCSPC code is used here.
//
// The game rewrites these bytes on boot (0x08934828 resets subtitles to 0),
// when a save restores its preferences and when the player toggles the menu
// row, so the value is re-checked once per vblank and written back only when
// it differs. With a key present the INI wins over the in-game menu.

#include "psprecomp/common.hpp"
#include "psprecomp/guest_memory.hpp"
#include "vcs_config.hpp"
#include "vcs_runtime_log.hpp"

#include <cstdint>
#include <optional>
#include <sstream>

namespace vcs {

namespace game_options_detail {

inline constexpr std::uint32_t kGuestGp = 0x08BB1D60u;
inline constexpr std::uint32_t kDisplayPrefsPointer = kGuestGp + 0x16F4u; // 0x08BB3454
inline constexpr std::uint32_t kDisplayPrefsVtable = 0x08BA6250u;
inline constexpr std::uint32_t kDisplayPrefsSize = 0x50u;
inline constexpr std::uint32_t kShowSubtitlesOffset = 0x08u;
inline constexpr std::uint32_t kHudModeOffset = 0x18u;

struct ForcedByte {
    const char *name;
    std::uint32_t offset;
    std::uint64_t writes{};
};

struct State {
    bool announced{false};
    std::uint32_t last_object{0u};
    ForcedByte subtitles{"Subtitles", kShowSubtitlesOffset};
    ForcedByte hud{"Hud", kHudModeOffset};
};

inline State &state() noexcept {
    static State instance{};
    return instance;
}

inline void force_byte(psprecomp::GuestMemory &memory, std::uint32_t object,
                       ForcedByte &field, const std::optional<bool> &wanted,
                       std::uint64_t vblank_index) {
    if (!wanted.has_value()) return;
    const std::uint32_t address = object + field.offset;
    const std::uint8_t desired = *wanted ? 1u : 0u;
    const std::uint8_t current = memory.load8(address);
    if (current == desired) return;
    memory.store8(address, desired);
    ++field.writes;
    // Logged on every write the game provoked (boot, save load, menu toggle),
    // which is rare; the cap only guards against a writer fighting us per frame.
    if (field.writes <= 32u || (field.writes % 1024u) == 0u) {
        std::ostringstream line;
        line << "game-options apply " << field.name
             << " addr=" << psprecomp::hex32(address)
             << " read=" << static_cast<unsigned>(current)
             << " wrote=" << static_cast<unsigned>(desired)
             << " writes=" << field.writes
             << " vblank=" << vblank_index;
        runtime_log_line(line.str());
    }
}

} // namespace game_options_detail

// Called once per displayed vblank from the guest thread (next to
// draw_distance_vblank_tick), so guest memory is not being mutated
// concurrently by translated code.
inline void game_options_vblank_tick(psprecomp::GuestMemory &memory,
                                     std::uint32_t guest_gp,
                                     std::uint64_t vblank_index) noexcept {
    using namespace game_options_detail;
    const GameConfiguration &config = vcs_configuration().game;
    if (!config.subtitles.has_value() && !config.hud.has_value()) return;
    State &s = state();
    try {
        if (!s.announced) {
            s.announced = true;
            std::ostringstream line;
            line << "game-options config subtitles="
                 << (config.subtitles ? (*config.subtitles ? "on" : "off") : "untouched")
                 << " hud=" << (config.hud ? (*config.hud ? "on" : "off") : "untouched")
                 << " prefs_ptr=" << psprecomp::hex32(kDisplayPrefsPointer)
                 << " guest_gp=" << psprecomp::hex32(guest_gp)
                 << (guest_gp == kGuestGp ? "" : " (unexpected gp; absolute address still used)");
            runtime_log_line(line.str());
        }
        if (!memory.contains(kDisplayPrefsPointer, sizeof(std::uint32_t))) return;
        const std::uint32_t object = memory.load32(kDisplayPrefsPointer);
        if (object == 0u || !memory.contains(object, kDisplayPrefsSize)) return;
        // Only touch a live, constructed DisplayPrefs: the destructor swaps the
        // vtable and clears the global, the constructor sets this one.
        if (memory.load32(object + 0x04u) != kDisplayPrefsVtable) return;
        if (object != s.last_object) {
            s.last_object = object;
            std::ostringstream line;
            line << "game-options DisplayPrefs object=" << psprecomp::hex32(object)
                 << " subtitles=" << static_cast<unsigned>(memory.load8(object + kShowSubtitlesOffset))
                 << " hud=" << static_cast<unsigned>(memory.load8(object + kHudModeOffset))
                 << " vblank=" << vblank_index;
            runtime_log_line(line.str());
        }
        force_byte(memory, object, s.subtitles, config.subtitles, vblank_index);
        force_byte(memory, object, s.hud, config.hud, vblank_index);
    } catch (...) {
        // Guest memory accessors throw on bad addresses; never let an INI
        // convenience take the frame down.
    }
}

} // namespace vcs
