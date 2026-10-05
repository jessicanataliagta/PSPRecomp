#include "psprecomp/hle/system.hpp"

#include "psprecomp/hle/kernel.hpp"

#include <chrono>
#include <cstdint>
#include <functional>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp).

namespace psprecomp::hle {

void install_system_hle(psprecomp::Runtime &runtime) {
    // sceUtilityGetSystemParamInt(id, int *value): the console settings of a
    // US-English PSP. VCS does not import it.
    runtime.register_hle("sceUtility", 0xA5DA2406u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            std::uint32_t value = 0u;
            switch (ctx.gpr[4]) {
            case 2u: value = 0u; break;   // ad hoc channel: automatic
            case 3u: value = 0u; break;   // WLAN power save: off
            case 4u: value = 1u; break;   // date format: MM/DD/YYYY
            case 5u: value = 1u; break;   // time format: 12 hour
            case 6u: value = 0u; break;   // time zone: UTC offset in minutes
            case 7u: value = 0u; break;   // daylight saving: off
            case 8u: value = 1u; break;   // language: English
            case 9u: value = 1u; break;   // confirm button: cross
            case 10u: value = 0u; break;  // parental level: unlocked
            default: ctx.set_gpr(2, 0x80110103u); return;  // invalid system parameter id
            }
            if (!rt.memory().contains(ctx.gpr[5], 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            rt.memory().store32(ctx.gpr[5], value);
            set_success(ctx);
        });
    // sceUtilityLoadModule / sceUtilityUnloadModule (AV codecs, network): the
    // host implements those libraries directly.
    runtime.register_hle("sceUtility", 0x2A2B3DE0u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUtility", 0xE49BFE92u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // scePowerSetClockFrequency(pll, cpu, bus): host speed is independent of it.
    runtime.register_hle("scePower", 0xEBD177D6u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // sceNpDrmSetLicenseeKey: no DRM-protected content is decrypted on the host.
    runtime.register_hle("scePspNpDrm_user", 0xA1336091u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // Native bring-up is intentionally offline.  Report the physical WLAN
    // switch as off rather than claiming a connected/powered radio.
    runtime.register_hle("sceWlanDrv", 0xD7763699u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0u); });
    // sceUmdGetErrorStat: the virtual UMD never reports a read error.
    runtime.register_hle("sceUmdUser", 0x20628E6Fu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // sceUmdWaitDriveStatWithTimer: the virtual UMD is always ready, as for
    // sceUmdWaitDriveStat below.
    runtime.register_hle("sceUmdUser", 0x56202973u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // scePowerRegisterCallback(slot, cbid): report AC power, battery present, 100%.
    runtime.register_hle("scePower", 0x04B7766Eu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (notify_device_callbacks)
                (void)notify_callback(static_cast<std::int32_t>(ctx.gpr[5]), 0x1000u | 0x80u | 100u);
            set_success(ctx);
        });
    runtime.register_hle("scePower", 0xDFA8BAF8u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    // sceUmdRegisterUMDCallBack(cbid): the virtual disc is present, initialised and ready.
    runtime.register_hle("sceUmdUser", 0xAEE7404Du,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            if (notify_device_callbacks)
                (void)notify_callback(static_cast<std::int32_t>(ctx.gpr[4]), 0x02u | 0x10u | 0x20u);
            set_success(ctx);
        });
    runtime.register_hle("sceUmdUser", 0xBD2BDE07u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0x46EBB729u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 1u); });
    runtime.register_hle("sceUmdUser", 0x6B4A146Cu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 0x32u); });
    runtime.register_hle("sceUmdUser", 0x8EF08FCEu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("sceUmdUser", 0xC6183D47u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });
    runtime.register_hle("LoadExecForUser", 0x4AC57943u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { set_success(ctx); });

    // sceRtc. None of it existed, and the saved-game list needs it: it turns
    // each save's timestamp into a tick to sort and display it, so opening the
    // load menu stopped the runtime on a missing import.
    //
    // A PSP tick is microseconds since 0001-01-01 00:00:00, and ScePspDateTime
    // is year, month, day, hour, minute, second as 16-bit fields followed by a
    // 32-bit microsecond -- 16 bytes.
    {
        // Howard Hinnant's civil-date algorithms, which are exact over the
        // whole proleptic Gregorian range rather than only near the epoch.
        const auto days_from_civil = [](std::int64_t y, unsigned m, unsigned d) -> std::int64_t {
            y -= m <= 2;
            const std::int64_t era = (y >= 0 ? y : y - 399) / 400;
            const unsigned yoe = static_cast<unsigned>(y - era * 400);
            const unsigned doy = (153u * (m + (m > 2 ? -3 : 9)) + 2u) / 5u + d - 1u;
            const unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
            return era * 146097 + static_cast<std::int64_t>(doe) - 719468;
        };
        const auto civil_from_days = [](std::int64_t z, int &y, unsigned &m, unsigned &d) {
            z += 719468;
            const std::int64_t era = (z >= 0 ? z : z - 146096) / 146097;
            const unsigned doe = static_cast<unsigned>(z - era * 146097);
            const unsigned yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
            const std::int64_t yr = static_cast<std::int64_t>(yoe) + era * 400;
            const unsigned doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);
            const unsigned mp = (5u * doy + 2u) / 153u;
            d = doy - (153u * mp + 2u) / 5u + 1u;
            m = mp + (mp < 10u ? 3u : -9u);
            y = static_cast<int>(yr + (m <= 2u ? 1 : 0));
        };
        // Days from 0001-01-01 to 1970-01-01.
        constexpr std::int64_t kDaysToUnixEpoch = 719162;
        constexpr std::uint64_t kMicrosecondsPerDay = 86400ull * 1000000ull;

        struct RtcHelpers {
            std::function<std::uint64_t(psprecomp::Runtime &, std::uint32_t)> read_tick;
            std::function<void(psprecomp::Runtime &, std::uint32_t, std::uint64_t)> write_date;
        };
        static RtcHelpers helpers;
        helpers.read_tick = [days_from_civil](psprecomp::Runtime &rt, std::uint32_t address) -> std::uint64_t {
            const std::uint32_t year = rt.memory().load16(address + 0u);
            const std::uint32_t month = rt.memory().load16(address + 2u);
            const std::uint32_t day = rt.memory().load16(address + 4u);
            const std::uint32_t hour = rt.memory().load16(address + 6u);
            const std::uint32_t minute = rt.memory().load16(address + 8u);
            const std::uint32_t second = rt.memory().load16(address + 10u);
            const std::uint32_t microsecond = rt.memory().load32(address + 12u);
            const std::int64_t days = days_from_civil(static_cast<std::int64_t>(year),
                                                      month == 0u ? 1u : month,
                                                      day == 0u ? 1u : day) + kDaysToUnixEpoch;
            return static_cast<std::uint64_t>(days) * kMicrosecondsPerDay +
                   (hour * 3600ull + minute * 60ull + second) * 1000000ull + microsecond;
        };
        helpers.write_date = [civil_from_days](psprecomp::Runtime &rt, std::uint32_t address,
                                               std::uint64_t tick) {
            const std::uint64_t day_index = tick / kMicrosecondsPerDay;
            const std::uint64_t remainder = tick % kMicrosecondsPerDay;
            int year = 1;
            unsigned month = 1u;
            unsigned day = 1u;
            civil_from_days(static_cast<std::int64_t>(day_index) - kDaysToUnixEpoch, year, month, day);
            rt.memory().store16(address + 0u, static_cast<std::uint16_t>(year));
            rt.memory().store16(address + 2u, static_cast<std::uint16_t>(month));
            rt.memory().store16(address + 4u, static_cast<std::uint16_t>(day));
            rt.memory().store16(address + 6u, static_cast<std::uint16_t>(remainder / 3600000000ull));
            rt.memory().store16(address + 8u, static_cast<std::uint16_t>((remainder / 60000000ull) % 60ull));
            rt.memory().store16(address + 10u, static_cast<std::uint16_t>((remainder / 1000000ull) % 60ull));
            rt.memory().store32(address + 12u, static_cast<std::uint32_t>(remainder % 1000000ull));
        };
        const auto current_tick = []() -> std::uint64_t {
            // The wall clock, not the guest's virtual time: a save stamped with
            // the emulated uptime would read as the year 1 in the list.
            const auto now = std::chrono::system_clock::now().time_since_epoch();
            const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
            return static_cast<std::uint64_t>(kDaysToUnixEpoch) * kMicrosecondsPerDay +
                   static_cast<std::uint64_t>(micros);
        };
        static const auto tick_now = current_tick;

        runtime.register_hle("sceRtc", 0xC41C2853u, // sceRtcGetTickResolution
            [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
                ctx.set_gpr(2, 1000000u);
            });
        runtime.register_hle("sceRtc", 0x3F7AD767u, // sceRtcGetCurrentTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick = tick_now();
                rt.memory().store32(ctx.gpr[4], static_cast<std::uint32_t>(tick));
                rt.memory().store32(ctx.gpr[4] + 4u, static_cast<std::uint32_t>(tick >> 32u));
                set_success(ctx);
            });
        runtime.register_hle("sceRtc", 0x6FF40ACCu, // sceRtcGetTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick = helpers.read_tick(rt, ctx.gpr[4]);
                rt.memory().store32(ctx.gpr[5], static_cast<std::uint32_t>(tick));
                rt.memory().store32(ctx.gpr[5] + 4u, static_cast<std::uint32_t>(tick >> 32u));
                set_success(ctx);
            });
        runtime.register_hle("sceRtc", 0x7ED29E40u, // sceRtcSetTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const std::uint64_t tick =
                    static_cast<std::uint64_t>(rt.memory().load32(ctx.gpr[5])) |
                    (static_cast<std::uint64_t>(rt.memory().load32(ctx.gpr[5] + 4u)) << 32u);
                helpers.write_date(rt, ctx.gpr[4], tick);
                set_success(ctx);
            });
        const auto current_clock = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            helpers.write_date(rt, ctx.gpr[4], tick_now());
            set_success(ctx);
        };
        runtime.register_hle("sceRtc", 0x4CFA57B0u, current_clock); // sceRtcGetCurrentClock
        runtime.register_hle("sceRtc", 0xE7C27D1Bu, current_clock); // ...LocalTime
        // No time zone is modelled: the host clock is already local, so both
        // conversions are the identity rather than a wrong offset.
        const auto copy_date = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            for (std::uint32_t offset = 0u; offset < 16u; offset += 4u)
                rt.memory().store32(ctx.gpr[4] + offset, rt.memory().load32(ctx.gpr[5] + offset));
            set_success(ctx);
        };
        runtime.register_hle("sceRtc", 0x34885E0Du, copy_date); // ConvertUtcToLocalTime
        runtime.register_hle("sceRtc", 0x779242A2u, copy_date); // ConvertLocalTimeToUTC
        runtime.register_hle("sceRtc", 0x9ED0AE87u, // sceRtcCompareTick
            [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
                const auto load = [&](std::uint32_t address) {
                    return static_cast<std::uint64_t>(rt.memory().load32(address)) |
                           (static_cast<std::uint64_t>(rt.memory().load32(address + 4u)) << 32u);
                };
                const std::uint64_t first = load(ctx.gpr[4]);
                const std::uint64_t second = load(ctx.gpr[5]);
                ctx.set_gpr(2, first < second ? 0xFFFFFFFFu : (first > second ? 1u : 0u));
            });
    }
}

} // namespace psprecomp::hle
