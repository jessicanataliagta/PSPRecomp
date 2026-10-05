#include "ctw_overlays.hpp"

#include "psprecomp/common.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace ctw {
namespace {

struct Slot {
    std::uint32_t base{};
    std::uint32_t end{};
    const OverlayInfo *active{};
};

std::vector<Slot> slots;

bool resident(const psprecomp::Runtime &runtime, const OverlayInfo &overlay) {
    for (std::size_t word = 0; word < overlay.fingerprint.size(); ++word)
        if (runtime.memory().load32(overlay.base + static_cast<std::uint32_t>(word * 4u)) != overlay.fingerprint[word])
            return false;
    return true;
}

void resolve(psprecomp::Runtime &runtime, std::uint32_t pc) {
    const auto slot = std::find_if(slots.begin(), slots.end(),
        [pc](const Slot &item) { return pc >= item.base && pc < item.end; });
    if (slot == slots.end()) return;
    if (slot->active != nullptr && resident(runtime, *slot->active)) return;

    runtime.unregister_functions(slot->base, slot->end);
    slot->active = nullptr;
    for (const OverlayInfo &overlay : overlays()) {
        if (overlay.base != slot->base || !resident(runtime, overlay)) continue;
        overlay.register_functions(runtime);
        slot->active = &overlay;
        static const bool quiet = std::getenv("CTW_QUIET_OVERLAYS") != nullptr;
        if (!quiet)
            std::cout << "[overlay] slot " << psprecomp::hex32(slot->base) << " -> " << overlay.name
                      << " (entered at " << psprecomp::hex32(pc) << ")\n";
        return;
    }
    std::cerr << "[overlay] slot " << psprecomp::hex32(slot->base) << " holds code matching no known overlay"
              << " (entered at " << psprecomp::hex32(pc) << ")\n";
}

} // namespace

void install_overlay_manager(psprecomp::Runtime &runtime) {
    slots.clear();
    for (const OverlayInfo &overlay : overlays()) {
        auto slot = std::find_if(slots.begin(), slots.end(),
            [&overlay](const Slot &item) { return item.base == overlay.base; });
        if (slot == slots.end()) slot = slots.insert(slots.end(), Slot{overlay.base, overlay.base, nullptr});
        slot->end = std::max(slot->end, overlay.base + overlay.size);
    }
    if (slots.empty()) return;
    std::uint32_t low = slots.front().base, high = slots.front().end;
    for (const Slot &slot : slots) {
        low = std::min(low, slot.base);
        high = std::max(high, slot.end);
    }
    runtime.set_code_overlay_resolver(low, high, &resolve);
}

} // namespace ctw
