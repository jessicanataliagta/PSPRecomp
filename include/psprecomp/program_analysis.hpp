#pragma once

#include "psprecomp/elf32.hpp"

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace psprecomp {

struct ExecutableRange {
    std::uint32_t start{};
    std::uint32_t end{};
};

struct FunctionAnalysis {
    std::uint32_t entry{};
    std::set<std::uint32_t> labels; // all decoded instruction addresses (delay slots excluded)
    std::set<std::uint32_t> entry_labels; // function entry and basic-block/return continuations
    std::set<std::uint32_t> direct_calls;
    std::set<std::uint32_t> indirect_call_sites;
    std::size_t basic_block_count{};
    std::size_t unsupported_instruction_count{};
    bool truncated{};
};

struct ProgramAnalysis {
    std::vector<ExecutableRange> executable_ranges;
    std::map<std::uint32_t, std::string> seeds;
    std::vector<FunctionAnalysis> functions;
    std::set<std::uint32_t> covered_labels;
    std::set<std::uint32_t> covered_entry_labels;
    std::size_t overlapping_label_count{};
};

[[nodiscard]] bool is_executable_address(const std::vector<ExecutableRange> &ranges,
                                         std::uint32_t address) noexcept;

// Code address ranges: executable sections when present, else executable segments.
[[nodiscard]] std::vector<ExecutableRange> code_ranges(const Elf32Image &elf, std::uint32_t load_base);

// `extra_seeds` adds entry points found elsewhere, e.g. host functions that
// only runtime-loaded overlays call (see collect_references_into).
[[nodiscard]] ProgramAnalysis analyze_program(const Elf32Image &elf,
                                              const GuestMemory &memory,
                                              std::uint32_t load_base,
                                              std::size_t max_instructions_per_function = 131072u,
                                              const std::map<std::uint32_t, std::string> &extra_seeds = {});

// Adds to `seeds` every address in `target_ranges` that `source` (loaded in
// `memory`) references: JAL targets and LUI+ADDIU/ORI pairs in its code and
// aligned pointers in its data sections.
void collect_references_into(const Elf32Image &source, const GuestMemory &memory, std::uint32_t source_load_base,
                             const std::vector<ExecutableRange> &target_ranges,
                             std::map<std::uint32_t, std::string> &seeds);

// Analyses a fixed-address code overlay loaded into `memory` next to its host
// executable. Seeds come from the overlay itself plus host references into the
// overlay's range (calls, materialized pointers, data pointers).
[[nodiscard]] ProgramAnalysis analyze_overlay(const Elf32Image &overlay, const Elf32Image &host,
                                              const GuestMemory &memory, std::uint32_t host_load_base,
                                              std::size_t max_instructions_per_function = 131072u);

} // namespace psprecomp
