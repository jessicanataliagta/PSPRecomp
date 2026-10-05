#include "psprecomp/hle/display.hpp"

#include "psprecomp/common.hpp"
#include "psprecomp/hle/kernel.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

// Moved verbatim from the VCS profile host (profiles/vcs/host/vcs_profile.cpp);
// presentation and pacing are reached through g_display_hooks.

namespace psprecomp::hle {
namespace {

std::uint64_t parse_environment_u64(const char *name, std::uint64_t fallback = 0u) {
    const char *text = std::getenv(name);
    if (text == nullptr || *text == '\0') return fallback;
    char *end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 0);
    return end != text && *end == '\0' ? static_cast<std::uint64_t>(value) : fallback;
}

} // namespace

DisplayHooks g_display_hooks{};
bool periodic_vblank_interrupts{};
DisplayState display_state{};

std::uint32_t display_refresh_hz() {
    return g_display_hooks.refresh_hz != nullptr ? g_display_hooks.refresh_hz() : 60u;
}

std::uint64_t display_vblank_period_us() {
    // 16683 us is the PSP's 59.94 Hz period; faster virtual displays scale it.
    const std::uint64_t refresh = display_refresh_hz();
    return std::max<std::uint64_t>(1u, (16683u * 60u + refresh / 2u) / refresh);
}

void reset_display(const DisplayHooks &hooks) {
    g_display_hooks = hooks;
    display_state = DisplayState{};
    set_periodic_sub_interrupt(30u, 15u, periodic_vblank_interrupts ? display_vblank_period_us() : 0u);
}

void install_display_hle(psprecomp::Runtime &runtime) {
    runtime.register_hle("sceDisplay", 0x0E20F177u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t mode = ctx.gpr[4];
            const std::uint32_t width = ctx.gpr[5];
            const std::uint32_t height = ctx.gpr[6];
            if (mode != 0u || width == 0u || width > 480u || height == 0u || height > 272u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            display_state.mode = mode;
            display_state.width = width;
            display_state.height = height;
            if (std::getenv("PSPRECOMP_DISPLAY_DIAG") != nullptr) {
                std::cerr << "[display] mode=" << mode << " " << width << "x" << height << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xDEA197D4u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            if (ctx.gpr[4] != 0u) {
                if (!rt.memory().contains(ctx.gpr[4], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[4], display_state.mode);
            }
            if (ctx.gpr[5] != 0u) {
                if (!rt.memory().contains(ctx.gpr[5], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[5], display_state.width);
            }
            if (ctx.gpr[6] != 0u) {
                if (!rt.memory().contains(ctx.gpr[6], 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                rt.memory().store32(ctx.gpr[6], display_state.height);
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0x289D82FEu,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t address = ctx.gpr[4];
            const std::uint32_t stride = ctx.gpr[5];
            const std::uint32_t format = ctx.gpr[6];
            const std::uint32_t sync = ctx.gpr[7];
            if (address != 0u && !rt.memory().contains(address, 4u)) {
                ctx.set_gpr(2, 0x800200D3u);
                return;
            }
            if (stride != 0u && (stride < display_state.width || stride > 2048u)) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            if (format > 3u || sync > 1u) {
                ctx.set_gpr(2, 0x80000107u);
                return;
            }
            display_state.frame_buffer = address;
            display_state.buffer_width = stride;
            display_state.pixel_format = format;
            display_state.sync_mode = sync;
            if (std::getenv("PSPRECOMP_DISPLAY_DIAG") != nullptr) {
                std::cerr << "[display] framebuffer=" << psprecomp::hex32(address)
                          << " stride=" << stride << " format=" << format
                          << " sync=" << sync << "\n";
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xEEDA2E54u,
        [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
            const std::uint32_t address_out = ctx.gpr[4];
            const std::uint32_t stride_out = ctx.gpr[5];
            const std::uint32_t format_out = ctx.gpr[6];
            const std::uint32_t sync = ctx.gpr[7];
            if (sync > 1u) { ctx.set_gpr(2, 0x80000107u); return; }
            for (const auto [ptr, value] : std::array<std::pair<std::uint32_t, std::uint32_t>, 3>{
                     std::pair{address_out, display_state.frame_buffer},
                     std::pair{stride_out, display_state.buffer_width},
                     std::pair{format_out, display_state.pixel_format}}) {
                if (ptr != 0u) {
                    if (!rt.memory().contains(ptr, 4u)) { ctx.set_gpr(2, 0x800200D3u); return; }
                    rt.memory().store32(ptr, value);
                }
            }
            set_success(ctx);
        });
    runtime.register_hle("sceDisplay", 0xDBA6C4C4u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.fpr[0] = 59.94005994f *
                (static_cast<float>(display_refresh_hz()) / 60.0f);
        });
    runtime.register_hle("sceDisplay", 0x9C6EAAD7u,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            ctx.set_gpr(2, static_cast<std::uint32_t>(
                (virtual_time_us * display_refresh_hz()) / 1000000u));
        });
    runtime.register_hle("sceDisplay", 0x4D4E10ECu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) {
            const std::uint64_t period = display_vblank_period_us();
            const std::uint64_t blank = std::max<std::uint64_t>(
                1u, (731u * 60u) / display_refresh_hz());
            const std::uint64_t phase = virtual_time_us % period;
            ctx.set_gpr(2, phase < blank ? 1u : 0u);
        });
    auto wait_vblank = [](psprecomp::Runtime &rt, psprecomp::AllegrexContext &ctx) {
        // The display consumes the completed GE frame.  This is a real PSP
        // visibility boundary: allow guest/GE overlap during the frame, then
        // wait only here before framebuffer presentation and vblank callbacks.
        if (g_display_hooks.before_vblank_wait != nullptr && !g_display_hooks.before_vblank_wait(rt, ctx)) return;
        ++display_vblank_index;
        if (g_display_hooks.on_vblank != nullptr && !g_display_hooks.on_vblank(rt, ctx)) return;
        static const std::uint64_t stop_vblank =
            parse_environment_u64("PSPRECOMP_STOP_VBLANK");
        if (stop_vblank != 0u && display_vblank_index >= stop_vblank) {
            ctx.set_gpr(2, 0u);
            rt.stop("VBlank diagnostic stop at " + std::to_string(display_vblank_index));
            return;
        }
        const std::uint64_t period = display_vblank_period_us();
        const std::uint32_t delay = static_cast<std::uint32_t>(period - (virtual_time_us % period));
        const auto current = thread_table.threads.find(thread_table.current_uid);
        if (current == thread_table.threads.end()) {
            ctx.set_gpr(2, 0x80020198u);
            return;
        }

        const auto interrupt = sub_interrupts.find(sub_interrupt_key(30u, 15u));
        if (periodic_sub_interrupt_active() || interrupt == sub_interrupts.end() ||
            !interrupt->second.enabled || interrupt->second.handler == 0u) {
            (void)delay_current_thread(rt, ctx, delay);
            return;
        }

        const psprecomp::AllegrexContext resume = make_wait_context(ctx);
        async_return_frames[thread_table.current_uid].push_back(
            AsyncReturnFrame{AsyncReturnKind::SubInterrupt, resume});

        psprecomp::AllegrexContext handler = ctx;
        handler.set_gpr(4, 15u);
        handler.set_gpr(5, interrupt->second.argument);
        handler.set_gpr(31, 0x00000004u);
        handler.pc = interrupt->second.handler;
        current->second.state = ThreadState::Delayed;
        current->second.suspended_context = handler;
        current->second.delay_until_us = virtual_time_us + delay;
        current->second.delay_sequence = thread_table.next_delay_sequence++;
        interrupt->second.occurred = true;
        if (std::getenv("PSPRECOMP_GE_DIAG") != nullptr) {
            std::cerr << "[intr] schedule vblank uid=" << thread_table.current_uid
                      << " handler=" << psprecomp::hex32(handler.pc)
                      << " resume=" << psprecomp::hex32(resume.pc) << "\n";
        }
        if (!activate_next_thread(ctx, "vblank-wait"))
            rt.stop("PSP scheduler deadlock while waiting for VBlank interrupt");
    };
    runtime.register_hle("sceDisplay", 0x36CDFADEu, wait_vblank);
    runtime.register_hle("sceDisplay", 0x8EB9EC49u, wait_vblank);
    runtime.register_hle("sceDisplay", 0x984C27E7u, wait_vblank);
    runtime.register_hle("sceDisplay", 0x46F186C3u, wait_vblank);
    runtime.register_hle("sceDisplay", 0xB4F378FAu,
        [](psprecomp::Runtime &, psprecomp::AllegrexContext &ctx) { ctx.set_gpr(2, 1u); });
}

} // namespace psprecomp::hle
