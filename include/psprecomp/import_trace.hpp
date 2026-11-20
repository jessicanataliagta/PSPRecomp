#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

namespace psprecomp {

// One HLE import call as seen by the runtime dispatcher.
struct ImportTraceEntry {
    std::uint64_t sequence{};
    std::int32_t thread_uid{-1};
    std::uint32_t nid{};
    std::uint32_t return_address{};
    std::string_view library{};
};

// The most recent HLE import calls, oldest first.  Recording is always on and
// costs a few stores per call; it exists so that a hang report can show what a
// guest thread stuck in a polling loop keeps calling.  Library names point at
// the string literals the generated import stubs pass in.
[[nodiscard]] std::vector<ImportTraceEntry> recent_import_calls();

} // namespace psprecomp
