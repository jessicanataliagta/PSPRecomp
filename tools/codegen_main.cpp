#include "psprecomp/common.hpp"
#include "psprecomp/decoder.hpp"
#include "psprecomp/codegen_policy.hpp"
#include "psprecomp/elf32.hpp"
#include "psprecomp/program_analysis.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cctype>
#include <charconv>
#include <filesystem>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <optional>
#include <regex>
#include <string_view>
#include <set>
#include <sstream>
#include <vector>

namespace {
struct Function {
    std::string name;
    std::uint32_t address{};
    std::uint32_t size{};
};

std::uint32_t parse_hex(std::string text) {
    if (text.starts_with("0x") || text.starts_with("0X")) text.erase(0, 2);
    std::uint32_t value{};
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value, 16);
    if (result.ec != std::errc{}) throw psprecomp::Error("Invalid hex value: " + text);
    return value;
}

std::vector<Function> load_functions(const std::filesystem::path &path) {
    std::ifstream in(path);
    if (!in) throw psprecomp::Error("Cannot open function CSV: " + path.string());
    std::vector<Function> out;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(in, line)) {
        ++line_number;
        if (line.empty() || line[0] == '#' || line.starts_with("name,")) continue;
        std::stringstream stream(line);
        std::string name, address, size;
        if (!std::getline(stream, name, ',') || !std::getline(stream, address, ',') || !std::getline(stream, size)) {
            throw psprecomp::Error("Invalid function CSV line " + std::to_string(line_number));
        }
        out.push_back({name, parse_hex(address), parse_hex(size)});
    }
    return out;
}

std::string safe_name(const std::string &name, std::uint32_t address) {
    std::string out;
    for (char c : name) out += (std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_';
    if (out.empty() || std::isdigit(static_cast<unsigned char>(out.front()))) {
        out = "fn_" + psprecomp::hex32(address).substr(2) + "_" + out;
    }
    return out;
}

std::string cpp_escape(const std::string &text) {
    std::string out;
    for (char c : text) {
        if (c == '\\') out += "\\\\";
        else if (c == '"') out += "\\\"";
        else out += c;
    }
    return out;
}

std::string reg(std::uint32_t index) {
    // Allegrex/MIPS $zero is immutable. Emitting a literal lets the host
    // compiler fold branches/arithmetic instead of materializing a load from
    // AllegrexContext for every source-register use of r0.
    if (index == 0u) return "0u";
    return "ctx.gpr[" + std::to_string(index) + "]";
}

std::string emit_regular(const psprecomp::DecodedInstruction &d, std::uint32_t pc) {
    const auto imm = static_cast<std::int32_t>(d.immediate);
    const auto uimm = static_cast<std::uint16_t>(d.immediate);
    std::ostringstream out;
    switch (d.kind) {
    case psprecomp::OpcodeKind::Nop: out << "    // nop\n"; break;
    case psprecomp::OpcodeKind::Sync:
    case psprecomp::OpcodeKind::Cache:
        out << psprecomp::codegen::memory_ordering_statement(d.kind);
        break;
    case psprecomp::OpcodeKind::Addi:
        // Trapping add-immediate: signed overflow raises the same exception as ADD.
        out << "    { const std::int64_t addi_sum = static_cast<std::int64_t>(static_cast<std::int32_t>("
            << reg(d.rs) << ")) + " << imm << ";\n"
            << "      if (addi_sum < std::numeric_limits<std::int32_t>::min() || addi_sum > std::numeric_limits<std::int32_t>::max()) { rt.arithmetic_overflow("
            << psprecomp::hex32(pc) << "u, " << psprecomp::hex32(d.word) << "u); return; }\n"
            << "      ctx.set_gpr(" << d.rt << ", static_cast<std::uint32_t>(addi_sum)); }\n";
        break;
    case psprecomp::OpcodeKind::Addiu: out << "    ctx.set_gpr(" << d.rt << ", " << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "));\n"; break;
    case psprecomp::OpcodeKind::Slti: out << "    ctx.set_gpr(" << d.rt << ", static_cast<std::int32_t>(" << reg(d.rs) << ") < " << imm << " ? 1u : 0u);\n"; break;
    case psprecomp::OpcodeKind::Sltiu: out << "    ctx.set_gpr(" << d.rt << ", " << reg(d.rs) << " < static_cast<std::uint32_t>(" << imm << ") ? 1u : 0u);\n"; break;
    case psprecomp::OpcodeKind::Andi: out << "    ctx.set_gpr(" << d.rt << ", " << reg(d.rs) << " & " << uimm << "u);\n"; break;
    case psprecomp::OpcodeKind::Ori: out << "    ctx.set_gpr(" << d.rt << ", " << reg(d.rs) << " | " << uimm << "u);\n"; break;
    case psprecomp::OpcodeKind::Xori: out << "    ctx.set_gpr(" << d.rt << ", " << reg(d.rs) << " ^ " << uimm << "u);\n"; break;
    case psprecomp::OpcodeKind::Lui: out << "    ctx.set_gpr(" << d.rt << ", " << uimm << "u << 16u);\n"; break;
    case psprecomp::OpcodeKind::Add:
        out << "    { const bool signed_ok = ctx.execute_signed_add(" << d.rd << "u, " << d.rs << "u, " << d.rt
            << "u);\n"
            << "      if (!signed_ok) { rt.arithmetic_overflow(" << psprecomp::hex32(pc) << "u, "
            << psprecomp::hex32(d.word) << "u); return; } }\n";
        break;
    case psprecomp::OpcodeKind::Addu: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " + " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Sub:
        out << "    { const bool signed_ok = ctx.execute_signed_sub(" << d.rd << "u, " << d.rs << "u, " << d.rt
            << "u);\n"
            << "      if (!signed_ok) { rt.arithmetic_overflow(" << psprecomp::hex32(pc) << "u, "
            << psprecomp::hex32(d.word) << "u); return; } }\n";
        break;
    case psprecomp::OpcodeKind::Subu: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " - " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::And: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " & " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Or: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " | " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Xor: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " ^ " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Nor: out << "    ctx.set_gpr(" << d.rd << ", ~(" << reg(d.rs) << " | " << reg(d.rt) << "));\n"; break;
    case psprecomp::OpcodeKind::Slt: out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::int32_t>(" << reg(d.rs) << ") < static_cast<std::int32_t>(" << reg(d.rt) << ") ? 1u : 0u);\n"; break;
    case psprecomp::OpcodeKind::Sltu: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << " < " << reg(d.rt) << " ? 1u : 0u);\n"; break;
    case psprecomp::OpcodeKind::Max: out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::int32_t>(" << reg(d.rs) << ") > static_cast<std::int32_t>(" << reg(d.rt) << ") ? " << reg(d.rs) << " : " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Min: out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::int32_t>(" << reg(d.rs) << ") < static_cast<std::int32_t>(" << reg(d.rt) << ") ? " << reg(d.rs) << " : " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Movz:
        out << "    if (" << reg(d.rt) << " == 0u) ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << ");\n";
        break;
    case psprecomp::OpcodeKind::Movn:
        out << "    if (" << reg(d.rt) << " != 0u) ctx.set_gpr(" << d.rd << ", " << reg(d.rs) << ");\n";
        break;
    case psprecomp::OpcodeKind::Sll: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rt) << " << " << d.sa << "u);\n"; break;
    case psprecomp::OpcodeKind::Srl: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rt) << " >> " << d.sa << "u);\n"; break;
    case psprecomp::OpcodeKind::Sra: out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(" << reg(d.rt) << ") >> " << d.sa << "u));\n"; break;
    case psprecomp::OpcodeKind::Rotr: out << "    ctx.set_gpr(" << d.rd << ", std::rotr(" << reg(d.rt) << ", " << d.sa << "));\n"; break;
    case psprecomp::OpcodeKind::Sllv: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rt) << " << (" << reg(d.rs) << " & 31u));\n"; break;
    case psprecomp::OpcodeKind::Srlv: out << "    ctx.set_gpr(" << d.rd << ", " << reg(d.rt) << " >> (" << reg(d.rs) << " & 31u));\n"; break;
    case psprecomp::OpcodeKind::Srav: out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(" << reg(d.rt) << ") >> (" << reg(d.rs) << " & 31u)));\n"; break;
    case psprecomp::OpcodeKind::Rotrv: out << "    ctx.set_gpr(" << d.rd << ", std::rotr(" << reg(d.rt) << ", static_cast<int>(" << reg(d.rs) << " & 31u)));\n"; break;
    case psprecomp::OpcodeKind::Clz:
        out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(std::countl_zero(" << reg(d.rs) << ")));\n";
        break;
    case psprecomp::OpcodeKind::Clo:
        out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(std::countl_one(" << reg(d.rs) << ")));\n";
        break;
    case psprecomp::OpcodeKind::Ext: {
        const std::uint32_t size = d.rd + 1u;
        const std::uint32_t mask = size == 32u ? 0xFFFFFFFFu : ((1u << size) - 1u);
        out << "    ctx.set_gpr(" << d.rt << ", (" << reg(d.rs) << " >> " << d.sa << "u) & " << psprecomp::hex32(mask) << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Ins: {
        const std::uint32_t size = d.rd >= d.sa ? d.rd - d.sa + 1u : 0u;
        const std::uint32_t source_mask = size == 32u ? 0xFFFFFFFFu : (size == 0u ? 0u : ((1u << size) - 1u));
        const std::uint32_t destination_mask = source_mask << d.sa;
        out << "    ctx.set_gpr(" << d.rt << ", (" << reg(d.rt) << " & ~" << psprecomp::hex32(destination_mask)
            << "u) | ((" << reg(d.rs) << " & " << psprecomp::hex32(source_mask) << "u) << " << d.sa << "u));\n";
        break;
    }
    case psprecomp::OpcodeKind::Seb:
        out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(" << reg(d.rt) << "))));\n";
        break;
    case psprecomp::OpcodeKind::Seh:
        out << "    ctx.set_gpr(" << d.rd << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(" << reg(d.rt) << "))));\n";
        break;
    case psprecomp::OpcodeKind::Bitrev:
        out << "    ctx.set_gpr(" << d.rd << ", [](std::uint32_t value) { "
               "value = ((value >> 1u) & 0x55555555u) | ((value & 0x55555555u) << 1u); "
               "value = ((value >> 2u) & 0x33333333u) | ((value & 0x33333333u) << 2u); "
               "value = ((value >> 4u) & 0x0F0F0F0Fu) | ((value & 0x0F0F0F0Fu) << 4u); "
               "value = ((value >> 8u) & 0x00FF00FFu) | ((value & 0x00FF00FFu) << 8u); "
               "return (value >> 16u) | (value << 16u); }(" << reg(d.rt) << "));\n";
        break;
    case psprecomp::OpcodeKind::Wsbh:
        out << "    ctx.set_gpr(" << d.rd << ", ((" << reg(d.rt) << " & 0x00FF00FFu) << 8u) | ((" << reg(d.rt) << " & 0xFF00FF00u) >> 8u));\n";
        break;
    case psprecomp::OpcodeKind::Wsbw:
        out << "    ctx.set_gpr(" << d.rd << ", ((" << reg(d.rt) << " & 0x000000FFu) << 24u) | ((" << reg(d.rt) << " & 0x0000FF00u) << 8u) | ((" << reg(d.rt) << " & 0x00FF0000u) >> 8u) | ((" << reg(d.rt) << " & 0xFF000000u) >> 24u));\n";
        break;
    case psprecomp::OpcodeKind::Lw: out << "    ctx.set_gpr(" << d.rt << ", rt.memory().aot_load32(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << ")));\n"; break;
    case psprecomp::OpcodeKind::Lwl: out << "    ctx.set_gpr(" << d.rt << ", rt.memory().aot_load_word_left(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), " << reg(d.rt) << "));\n"; break;
    case psprecomp::OpcodeKind::Lwr: out << "    ctx.set_gpr(" << d.rt << ", rt.memory().aot_load_word_right(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), " << reg(d.rt) << "));\n"; break;
    case psprecomp::OpcodeKind::Sw: out << "    rt.memory().aot_store32(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Swl: out << "    rt.memory().aot_store_word_left(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Swr: out << "    rt.memory().aot_store_word_right(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), " << reg(d.rt) << ");\n"; break;
    case psprecomp::OpcodeKind::Lh: out << "    ctx.set_gpr(" << d.rt << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(rt.memory().aot_load16(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "))))));\n"; break;
    case psprecomp::OpcodeKind::Lhu: out << "    ctx.set_gpr(" << d.rt << ", rt.memory().aot_load16(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << ")));\n"; break;
    case psprecomp::OpcodeKind::Sh: out << "    rt.memory().aot_store16(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), static_cast<std::uint16_t>(" << reg(d.rt) << "));\n"; break;
    case psprecomp::OpcodeKind::Lb: out << "    ctx.set_gpr(" << d.rt << ", static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(rt.memory().aot_load8(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "))))));\n"; break;
    case psprecomp::OpcodeKind::Lbu: out << "    ctx.set_gpr(" << d.rt << ", rt.memory().aot_load8(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << ")));\n"; break;
    case psprecomp::OpcodeKind::Sb: out << "    rt.memory().aot_store8(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), static_cast<std::uint8_t>(" << reg(d.rt) << "));\n"; break;
    case psprecomp::OpcodeKind::Lwc1: out << "    ctx.fpr[" << d.rt << "] = std::bit_cast<float>(rt.memory().aot_load32(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << ")));\n"; break;
    case psprecomp::OpcodeKind::Swc1: out << "    rt.memory().aot_store32(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << imm << "), std::bit_cast<std::uint32_t>(ctx.fpr[" << d.rt << "]));\n"; break;
    case psprecomp::OpcodeKind::Mfhi: out << "    ctx.set_gpr(" << d.rd << ", ctx.hi);\n"; break;
    case psprecomp::OpcodeKind::Mflo: out << "    ctx.set_gpr(" << d.rd << ", ctx.lo);\n"; break;
    case psprecomp::OpcodeKind::Mthi: out << "    ctx.hi = " << reg(d.rs) << ";\n"; break;
    case psprecomp::OpcodeKind::Mtlo: out << "    ctx.lo = " << reg(d.rs) << ";\n"; break;
    case psprecomp::OpcodeKind::Mult:
        out << "    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(" << reg(d.rs) << ")) * "
            << "static_cast<std::int64_t>(static_cast<std::int32_t>(" << reg(d.rt) << ")); "
            << "ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }\n";
        break;
    case psprecomp::OpcodeKind::Multu:
        out << "    { const std::uint64_t product = static_cast<std::uint64_t>(" << reg(d.rs) << ") * static_cast<std::uint64_t>(" << reg(d.rt) << "); "
            << "ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(product >> 32u); }\n";
        break;
    case psprecomp::OpcodeKind::Madd:
    case psprecomp::OpcodeKind::Msub: {
        const char op = d.kind == psprecomp::OpcodeKind::Madd ? '+' : '-';
        out << "    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(" << reg(d.rs) << ")) * "
            << "static_cast<std::int64_t>(static_cast<std::int32_t>(" << reg(d.rt) << ")); "
            << "const std::uint64_t accumulator = ((static_cast<std::uint64_t>(ctx.hi) << 32u) | ctx.lo) " << op
            << " static_cast<std::uint64_t>(product); "
            << "ctx.lo = static_cast<std::uint32_t>(accumulator); ctx.hi = static_cast<std::uint32_t>(accumulator >> 32u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Maddu:
    case psprecomp::OpcodeKind::Msubu: {
        const char op = d.kind == psprecomp::OpcodeKind::Maddu ? '+' : '-';
        out << "    { const std::uint64_t product = static_cast<std::uint64_t>(" << reg(d.rs) << ") * static_cast<std::uint64_t>(" << reg(d.rt) << "); "
            << "const std::uint64_t accumulator = ((static_cast<std::uint64_t>(ctx.hi) << 32u) | ctx.lo) " << op << " product; "
            << "ctx.lo = static_cast<std::uint32_t>(accumulator); ctx.hi = static_cast<std::uint32_t>(accumulator >> 32u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Div:
        // $zero is emitted as a literal 0u.  If it is also the divisor, do not
        // emit a syntactically present / or % expression at all: MSVC diagnoses
        // division by a compile-time zero even when it sits behind an ordinary
        // `if (divisor == 0)` guard.  This is also cheaper and exactly matches
        // Allegrex/MIPS divide-by-zero HI/LO semantics.
        if (d.rt == 0u) {
            out << "    { const std::int32_t dividend = static_cast<std::int32_t>(" << reg(d.rs) << "); "
                << "ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; "
                << "ctx.hi = static_cast<std::uint32_t>(dividend); }\n";
        } else {
            out << "    { const std::int32_t dividend = static_cast<std::int32_t>(" << reg(d.rs) << "); "
                << "const std::int32_t divisor = static_cast<std::int32_t>(" << reg(d.rt) << "); "
                << "if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } "
                << "else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } "
                << "else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }\n";
        }
        break;
    case psprecomp::OpcodeKind::Divu:
        if (d.rt == 0u) {
            out << "    { const std::uint32_t dividend = " << reg(d.rs) << "; "
                << "ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; }\n";
        } else {
            out << "    { const std::uint32_t dividend = " << reg(d.rs) << "; const std::uint32_t divisor = " << reg(d.rt) << "; "
                << "if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } "
                << "else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }\n";
        }
        break;
    case psprecomp::OpcodeKind::Mfc1:
        out << "    ctx.set_gpr(" << d.rt << ", std::bit_cast<std::uint32_t>(ctx.fpr[" << d.rd << "]));\n";
        break;
    case psprecomp::OpcodeKind::Mtc1:
        out << "    ctx.fpr[" << d.rd << "] = std::bit_cast<float>(" << reg(d.rt) << ");\n";
        break;
    case psprecomp::OpcodeKind::Cfc1:
        if (d.rd == 31u) out << "    ctx.set_gpr(" << d.rt << ", ctx.fcr31);\n";
        else out << "    rt.unsupported(" << psprecomp::hex32(pc) << "u, " << psprecomp::hex32(d.word) << "u, \"unsupported CFC1 control register\"); return;\n";
        break;
    case psprecomp::OpcodeKind::Ctc1:
        if (d.rd == 31u) out << "    ctx.fcr31 = " << reg(d.rt) << " & 0x0181FFFFu;\n";
        else out << "    rt.unsupported(" << psprecomp::hex32(pc) << "u, " << psprecomp::hex32(d.word) << "u, \"unsupported CTC1 control register\"); return;\n";
        break;
    case psprecomp::OpcodeKind::AddS:
        out << "    ctx.fpr[" << d.sa << "] = ctx.fpr[" << d.rd << "] + ctx.fpr[" << d.rt << "];\n";
        break;
    case psprecomp::OpcodeKind::SubS:
        out << "    ctx.fpr[" << d.sa << "] = ctx.fpr[" << d.rd << "] - ctx.fpr[" << d.rt << "];\n";
        break;
    case psprecomp::OpcodeKind::MulS:
        out << "    { const float fs = ctx.fpr[" << d.rd << "]; const float ft = ctx.fpr[" << d.rt << "]; "
            << "if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.set_fpr_bits(" << d.sa << ", 0x7FC00000u); "
            << "else ctx.fpr[" << d.sa << "] = fs * ft; }\n";
        break;
    case psprecomp::OpcodeKind::DivS:
        out << "    ctx.fpr[" << d.sa << "] = ctx.fpr[" << d.rd << "] / ctx.fpr[" << d.rt << "];\n";
        break;
    case psprecomp::OpcodeKind::SqrtS:
        out << "    ctx.fpr[" << d.sa << "] = std::sqrt(ctx.fpr[" << d.rd << "]);\n";
        break;
    case psprecomp::OpcodeKind::AbsS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpr_bits(" << d.rd << ") & 0x7FFFFFFFu);\n";
        break;
    case psprecomp::OpcodeKind::MovS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpr_bits(" << d.rd << "));\n";
        break;
    case psprecomp::OpcodeKind::NegS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpr_bits(" << d.rd << ") ^ 0x80000000u);\n";
        break;
    case psprecomp::OpcodeKind::RoundWS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpu_float_to_word(ctx.fpr[" << d.rd << "], 0u));\n";
        break;
    case psprecomp::OpcodeKind::TruncWS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpu_float_to_word(ctx.fpr[" << d.rd << "], 1u));\n";
        break;
    case psprecomp::OpcodeKind::CeilWS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpu_float_to_word(ctx.fpr[" << d.rd << "], 2u));\n";
        break;
    case psprecomp::OpcodeKind::FloorWS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpu_float_to_word(ctx.fpr[" << d.rd << "], 3u));\n";
        break;
    case psprecomp::OpcodeKind::CvtWS:
        out << "    ctx.set_fpr_bits(" << d.sa << ", ctx.fpu_float_to_word(ctx.fpr[" << d.rd << "], 4u));\n";
        break;
    case psprecomp::OpcodeKind::CvtSW:
        out << "    ctx.fpr[" << d.sa << "] = static_cast<float>(static_cast<std::int32_t>(ctx.fpr_bits(" << d.rd << ")));\n";
        break;
    case psprecomp::OpcodeKind::FpuCompare: {
        const std::uint32_t condition = d.word & 0xFu;
        const std::string fs = "ctx.fpr[" + std::to_string(d.rd) + "]";
        const std::string ft = "ctx.fpr[" + std::to_string(d.rt) + "]";
        const std::string unordered = "(std::isnan(" + fs + ") || std::isnan(" + ft + "))";
        std::string expression;
        switch (condition) {
        case 0u: case 8u: expression = "false"; break;
        case 1u: case 9u: expression = unordered; break;
        case 2u: case 10u: expression = "(!" + unordered + " && " + fs + " == " + ft + ")"; break;
        case 3u: case 11u: expression = "(" + unordered + " || " + fs + " == " + ft + ")"; break;
        case 4u: case 12u: expression = "(" + fs + " < " + ft + ")"; break;
        case 5u: case 13u: expression = "(" + unordered + " || " + fs + " < " + ft + ")"; break;
        case 6u: case 14u: expression = "(" + fs + " <= " + ft + ")"; break;
        default: expression = "(" + unordered + " || " + fs + " <= " + ft + ")"; break;
        }
        out << "    ctx.set_fpu_condition(" << expression << ");\n";
        break;
    }
    case psprecomp::OpcodeKind::Vflush:
        if ((d.word & 0xFFFF0000u) == 0xFFFF0000u) {
            out << "    // vflush: architectural no-op that retains VFPU prefixes\n";
        } else {
            out << "    ctx.eat_vfpu_prefixes(); // VFPU sync/no-op consumes prefixes\n";
        }
        break;
    case psprecomp::OpcodeKind::Vpfx: {
        const std::uint32_t control = (d.word >> 24u) & 3u;
        std::uint32_t data = d.word & 0x000FFFFFu;
        if (control == 2u) data &= 0x00000FFFu;
        out << "    ctx.vfpu_ctrl[" << control << "u] = " << psprecomp::hex32(data) << "u;\n";
        break;
    }
    case psprecomp::OpcodeKind::Viim: {
        const std::uint32_t destination = (d.word >> 16u) & 0x7Fu;
        const std::int32_t immediate = static_cast<std::int16_t>(d.word & 0xFFFFu);
        out << "    { const float vfpu_value[1]{static_cast<float>(" << immediate << ")};\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_value, " << destination << "u, 1u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vfim: {
        const std::uint32_t destination = (d.word >> 16u) & 0x7Fu;
        const std::uint32_t half = d.word & 0xFFFFu;
        out << "    { const std::uint16_t vfpu_half = " << half << "u;\n"
            << "      const std::uint32_t vfpu_sign = static_cast<std::uint32_t>(vfpu_half & 0x8000u) << 16u;\n"
            << "      std::uint32_t vfpu_exponent = (vfpu_half >> 10u) & 0x1Fu;\n"
            << "      std::uint32_t vfpu_mantissa = vfpu_half & 0x03FFu;\n"
            << "      std::uint32_t vfpu_bits = 0u;\n"
            << "      if (vfpu_exponent == 0u) {\n"
            << "        if (vfpu_mantissa == 0u) vfpu_bits = vfpu_sign;\n"
            << "        else {\n"
            << "          std::uint32_t shift = 0u;\n"
            << "          while ((vfpu_mantissa & 0x0400u) == 0u) { vfpu_mantissa <<= 1u; ++shift; }\n"
            << "          vfpu_mantissa &= 0x03FFu;\n"
            << "          vfpu_bits = vfpu_sign | ((113u - shift) << 23u) | (vfpu_mantissa << 13u);\n"
            << "        }\n"
            << "      } else if (vfpu_exponent == 31u) {\n"
            << "        vfpu_bits = vfpu_sign | 0x7F800000u | (vfpu_mantissa << 13u);\n"
            << "      } else {\n"
            << "        vfpu_bits = vfpu_sign | ((vfpu_exponent + 112u) << 23u) | (vfpu_mantissa << 13u);\n"
            << "      }\n"
            << "      const float vfpu_value[1]{std::bit_cast<float>(vfpu_bits)};\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_value, " << destination << "u, 1u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vf2h: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vf2h(" << destination << "u, " << source << "u, "
            << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vh2f: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t source_length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vh2f(" << destination << "u, " << source << "u, "
            << source_length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vf2i: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t immediate = (d.word >> 16u) & 31u;
        const std::uint32_t mode = (d.word >> 21u) & 31u;
        out << "    { float vfpu_s[4]{}; std::int32_t vfpu_d[4]{};\n"
            << "      ctx.read_vfpu_vector(vfpu_s, " << source << "u, " << length << "u);\n"
            << "      ctx.apply_vfpu_source_prefix(vfpu_s, " << length << "u, 0u);\n"
            << "      const float vfpu_scale = std::ldexp(1.0f, static_cast<int>(" << immediate << "u));\n"
            << "      for (std::uint32_t vfpu_i = 0; vfpu_i < " << length << "u; ++vfpu_i) {\n"
            << "        if (std::isnan(vfpu_s[vfpu_i])) { vfpu_d[vfpu_i] = std::numeric_limits<std::int32_t>::max(); continue; }\n"
            << "        const double vfpu_scaled = static_cast<double>(vfpu_s[vfpu_i] * vfpu_scale);\n"
            << "        if (vfpu_scaled > static_cast<double>(std::numeric_limits<std::int32_t>::max())) vfpu_d[vfpu_i] = std::numeric_limits<std::int32_t>::max();\n"
            << "        else if (vfpu_scaled <= static_cast<double>(std::numeric_limits<std::int32_t>::min())) vfpu_d[vfpu_i] = std::numeric_limits<std::int32_t>::min();\n"
            << "        else { double vfpu_rounded = 0.0;\n"
            << "          switch (" << mode << "u) {\n"
            << "          case 16u: vfpu_rounded = psprecomp::AllegrexContext::round_ties_to_even(vfpu_scaled); break;\n"
            << "          case 17u: vfpu_rounded = std::trunc(vfpu_scaled); break;\n"
            << "          case 18u: vfpu_rounded = std::ceil(vfpu_scaled); break;\n"
            << "          default: vfpu_rounded = std::floor(vfpu_scaled); break;\n"
            << "          }\n"
            << "          vfpu_d[vfpu_i] = static_cast<std::int32_t>(vfpu_rounded);\n"
            << "        }\n"
            << "      }\n"
            << "      const std::uint32_t vfpu_destination_prefix = ctx.vfpu_ctrl[2];\n"
            << "      for (std::uint32_t vfpu_i = 0; vfpu_i < " << length << "u; ++vfpu_i) {\n"
            << "        if (((vfpu_destination_prefix >> (8u + vfpu_i)) & 1u) == 0u)\n"
            << "          ctx.vfpu[psprecomp::AllegrexContext::vfpu_vector_lane_index(" << destination << "u, " << length << "u, vfpu_i)] = std::bit_cast<float>(static_cast<std::uint32_t>(vfpu_d[vfpu_i]));\n"
            << "      }\n"
            << "      ctx.eat_vfpu_prefixes(); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vi2f: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t immediate = (d.word >> 16u) & 31u;
        out << "    { float vfpu_s[4]{}, vfpu_d[4]{};\n"
            << "      ctx.read_vfpu_vector(vfpu_s, " << source << "u, " << length << "u);\n"
            << "      ctx.apply_vfpu_source_prefix(vfpu_s, " << length << "u, 0u);\n"
            << "      const float vfpu_scale = std::ldexp(1.0f, -static_cast<int>(" << immediate << "u));\n"
            << "      for (std::uint32_t vfpu_i = 0; vfpu_i < " << length << "u; ++vfpu_i) {\n"
            << "        const auto vfpu_integer = static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(vfpu_s[vfpu_i]));\n"
            << "        vfpu_d[vfpu_i] = static_cast<float>(vfpu_integer) * vfpu_scale;\n"
            << "      }\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_d, " << destination << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vx2i: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t source_length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t operation = (d.word >> 16u) & 3u;
        out << "    ctx.execute_vfpu_vx2i(" << destination << "u, " << source << "u, "
            << source_length << "u, " << operation << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vi2x: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        out << "    ctx.execute_vfpu_vi2x(" << (d.word & 0x7Fu) << "u, " << ((d.word >> 8u) & 0x7Fu) << "u, "
            << (size_code + 1u) << "u, " << ((d.word >> 16u) & 3u) << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Mtv:
        out << "    ctx.set_vfpu_scalar_bits(" << (d.word & 0xFFu) << "u, " << reg(d.rt) << ");\n";
        break;
    case psprecomp::OpcodeKind::Mfv:
        out << "    ctx.set_gpr(" << d.rt << ", ctx.vfpu_scalar_bits(" << (d.word & 0xFFu) << "u));\n";
        break;
    case psprecomp::OpcodeKind::VmidT: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t side = size_code + 1u;
        out << "    ctx.execute_vfpu_matrix_init(" << (d.word & 0x7Fu) << "u, " << side << "u, 3u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vmmov: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t side = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vmmov(" << destination << "u, " << source << "u, "
            << side << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuMatrixInit: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t side = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t operation = (d.word >> 16u) & 15u;
        out << "    ctx.execute_vfpu_matrix_init(" << destination << "u, " << side << "u, "
            << operation << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vidt: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t offset_mask = length >= 3u ? 3u : 1u;
        const std::uint32_t one_lane = destination & offset_mask;
        out << "    { float vfpu_value[4]{}; vfpu_value[" << one_lane << "u] = 1.0f;\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_value, " << destination
            << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vtfm: {
        const std::uint32_t side = ((d.word >> 23u) & 3u) + 1u;
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t input_length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    { float vfpu_matrix[16]{}, vfpu_target_raw[4]{}, vfpu_target[4]{}, vfpu_result[4]{};\n"
            << "      ctx.read_vfpu_matrix(vfpu_matrix, " << source << "u, " << side << "u);\n"
            << "      ctx.read_vfpu_vector(vfpu_target_raw, " << target << "u, " << side << "u);\n"
            << "      constexpr std::uint32_t vfpu_side = " << side << "u;\n"
            << "      constexpr std::uint32_t vfpu_input_length = " << input_length << "u;\n"
            << "      for (std::uint32_t i = 0; i < 4u; ++i) vfpu_target[i] = i < vfpu_input_length ? vfpu_target_raw[i] : 0.0f;\n"
            << "      if (vfpu_side - 1u >= vfpu_input_length) vfpu_target[vfpu_side - 1u] = 1.0f;\n"
            << "      for (std::uint32_t row = 0; row + 1u < vfpu_side; ++row) {\n"
            << "        float sum = 0.0f;\n"
            << "        for (std::uint32_t column = 0; column < vfpu_side; ++column) sum += vfpu_matrix[row * 4u + column] * vfpu_target[column];\n"
            << "        vfpu_result[row] = sum;\n"
            << "      }\n"
            << "      float vfpu_final_row[4]{vfpu_matrix[(vfpu_side - 1u) * 4u + 0u], vfpu_matrix[(vfpu_side - 1u) * 4u + 1u],\n"
            << "                              vfpu_matrix[(vfpu_side - 1u) * 4u + 2u], vfpu_matrix[(vfpu_side - 1u) * 4u + 3u]};\n"
            << "      ctx.apply_vfpu_source_prefix(vfpu_final_row, 4u, 0u);\n"
            << "      ctx.apply_vfpu_source_prefix(vfpu_target, 4u, 1u);\n"
            << "      for (std::uint32_t column = 0; column < 4u; ++column) vfpu_result[vfpu_side - 1u] += vfpu_final_row[column] * vfpu_target[column];\n"
            << "      const std::uint32_t vfpu_destination_prefix = ctx.vfpu_ctrl[2];\n"
            << "      const std::uint32_t vfpu_last_lane = vfpu_side - 1u;\n"
            << "      ctx.vfpu_ctrl[2] = ((vfpu_destination_prefix & (1u << 8u)) << vfpu_last_lane) |\n"
            << "                         ((vfpu_destination_prefix & 3u) << (vfpu_last_lane * 2u));\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_result, " << destination << "u, vfpu_side); }\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuVectorInit: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const bool one = (((d.word >> 16u) & 31u) == 7u);
        out << "    { float vfpu_value[4]{" << (one ? "1.0f, 1.0f, 1.0f, 1.0f" : "") << "};\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_value, " << destination
            << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vmmul: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t side = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    { float vfpu_s[16]{}, vfpu_t[16]{}, vfpu_d[16]{};\n"
            << "      ctx.read_vfpu_matrix(vfpu_s, " << source << "u, " << side << "u);\n"
            << "      ctx.read_vfpu_matrix(vfpu_t, " << target << "u, " << side << "u);\n"
            << "      for (std::uint32_t a = 0; a < " << side << "u; ++a) {\n"
            << "        for (std::uint32_t b = 0; b < " << side << "u; ++b) {\n"
            << "          float sum = 0.0f;\n"
            << "          for (std::uint32_t c = 0; c < " << side << "u; ++c) sum += vfpu_s[b * 4u + c] * vfpu_t[a * 4u + c];\n"
            << "          vfpu_d[a * 4u + b] = sum;\n"
            << "        }\n"
            << "      }\n"
            << "      ctx.write_vfpu_matrix(vfpu_d, " << destination << "u, " << side << "u);\n"
            << "      ctx.eat_vfpu_prefixes(); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vmscl: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t side = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vmscl(" << destination << "u, " << source << "u, "
            << target << "u, " << side << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vrot: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t immediate = (d.word >> 16u) & 31u;
        out << "    ctx.execute_vfpu_vrot(" << destination << "u, " << source << "u, "
            << length << "u, " << immediate << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vocp: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vocp(" << destination << "u, " << source << "u, "
            << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuHorizontal: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const bool average = ((d.word >> 16u) & 31u) == 7u;
        out << "    ctx.execute_vfpu_horizontal(" << destination << "u, " << source << "u, "
            << length << "u, " << (average ? "true" : "false") << ");\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuVec3: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        const std::uint32_t major = d.word >> 26u;
        const std::uint32_t operation = major == 0x19u ? 2u : ((d.word >> 23u) & 7u);
        const char *expression = operation == 0u ? "vfpu_s[i] + vfpu_t[i]"
                               : operation == 1u ? "vfpu_s[i] - vfpu_t[i]"
                               : operation == 2u ? "vfpu_s[i] * vfpu_t[i]"
                                                 : "vfpu_s[i] / vfpu_t[i]";
        out << "    { float vfpu_s[4]{}, vfpu_t[4]{}, vfpu_d[4]{};\n"
            << "      ctx.read_vfpu_vector_with_source_prefix(vfpu_s, " << source << "u, " << length << "u, 0u);\n"
            << "      ctx.read_vfpu_vector_with_source_prefix(vfpu_t, " << target << "u, " << length << "u, 1u);\n"
            << "      for (std::uint32_t i = 0; i < " << length << "u; ++i) vfpu_d[i] = " << expression << ";\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_d, " << destination << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vdot: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vdot(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vhdp: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vhdp(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::VcrossQuat: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    ctx.execute_vfpu_cross_quat(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vminmax: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        const bool maximum = ((d.word >> 23u) & 7u) == 3u;
        out << "    ctx.execute_vfpu_vminmax(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u, " << (maximum ? "true" : "false") << ");\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuCompare3: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        const std::uint32_t operation = (d.word >> 23u) & 7u;
        out << "    ctx.execute_vfpu_compare3(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u, " << operation << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vcmp: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        const std::uint32_t condition = d.word & 15u;
        out << "    ctx.execute_vfpu_vcmp(" << source << "u, " << target << "u, "
            << length << "u, " << condition << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::Vcmov: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t condition_index = (d.word >> 16u) & 7u;
        const bool move_if_false = ((d.word >> 19u) & 1u) != 0u;
        out << "    ctx.execute_vfpu_vcmov(" << destination << "u, " << source << "u, "
            << length << "u, " << condition_index << "u, "
            << (move_if_false ? "true" : "false") << ");\n";
        break;
    }
    case psprecomp::OpcodeKind::Vscl: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t target = (d.word >> 16u) & 0x7Fu;
        out << "    ctx.execute_vfpu_vscl(" << destination << "u, " << source << "u, "
            << target << "u, " << length << "u);\n";
        break;
    }
    case psprecomp::OpcodeKind::VfpuUnary: {
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t destination = d.word & 0x7Fu;
        const std::uint32_t source = (d.word >> 8u) & 0x7Fu;
        const std::uint32_t operation = (d.word >> 16u) & 31u;
        std::string expression;
        switch (operation) {
        case 0u: expression = "vfpu_s[i]"; break;
        case 1u: expression = "std::fabs(vfpu_s[i])"; break;
        case 2u: expression = "-vfpu_s[i]"; break;
        case 4u: expression = "vfpu_s[i] <= 0.0f ? 0.0f : (vfpu_s[i] > 1.0f ? 1.0f : vfpu_s[i])"; break;
        case 5u: expression = "vfpu_s[i] < -1.0f ? -1.0f : (vfpu_s[i] > 1.0f ? 1.0f : vfpu_s[i])"; break;
        case 16u: expression = "1.0f / vfpu_s[i]"; break;
        case 17u: expression = "1.0f / std::sqrt(vfpu_s[i])"; break;
        case 18u: expression = "std::sin(vfpu_s[i] * 1.57079632679489661923f)"; break;
        case 19u: expression = "std::cos(vfpu_s[i] * 1.57079632679489661923f)"; break;
        case 20u: expression = "std::exp2(vfpu_s[i])"; break;
        case 21u: expression = "std::log2(vfpu_s[i])"; break;
        case 22u: expression = "std::fabs(std::sqrt(vfpu_s[i]))"; break;
        case 23u: expression = "std::asin(vfpu_s[i]) * 0.63661977236758134308f"; break;
        case 24u: expression = "-1.0f / vfpu_s[i]"; break;
        case 26u: expression = "-std::sin(vfpu_s[i] * 1.57079632679489661923f)"; break;
        default: expression = "1.0f / std::exp2(vfpu_s[i])"; break;
        }
        out << "    { float vfpu_s[4]{}, vfpu_d[4]{};\n"
            << "      ctx.read_vfpu_vector_with_source_prefix(vfpu_s, " << source << "u, " << length << "u, 0u);\n"
            << "      for (std::uint32_t i = 0; i < " << length << "u; ++i) vfpu_d[i] = " << expression << ";\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_d, " << destination << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Vcst: {
        static constexpr std::uint32_t constant_bits[32] = {
            0x00000000u, 0x7F7FFFFFu, 0x3FB504F3u, 0x3F3504F3u,
            0x3F906EBAu, 0x3F22F983u, 0x3EA2F983u, 0x3F490FDBu,
            0x3FC90FDBu, 0x40490FDBu, 0x402DF854u, 0x3FB8AA3Bu,
            0x3EDE5BD9u, 0x3F317218u, 0x40135D8Eu, 0x40C90FDBu,
            0x3F060A92u, 0x3E9A209Bu, 0x40549A78u, 0x3F5DB3D7u,
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        };
        const std::uint32_t size_code = ((d.word >> 7u) & 1u) | (((d.word >> 15u) & 1u) << 1u);
        const std::uint32_t length = size_code + 1u;
        const std::uint32_t selector = (d.word >> 16u) & 31u;
        const std::uint32_t destination = d.word & 0x7Fu;
        out << "    { const float vfpu_constant = std::bit_cast<float>("
            << psprecomp::hex32(constant_bits[selector]) << "u);\n"
            << "      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};\n"
            << "      ctx.write_vfpu_vector_with_destination_prefix(vfpu_value, " << destination
            << "u, " << length << "u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Lvs: {
        const std::int32_t offset = static_cast<std::int16_t>(d.word & 0xFFFCu);
        const std::uint32_t scalar_register = ((d.word >> 16u) & 0x1Fu) | ((d.word & 3u) << 5u);
        out << "    ctx.set_vfpu_scalar_bits(" << scalar_register << "u, rt.memory().aot_load32("
            << reg(d.rs) << " + static_cast<std::uint32_t>(" << offset << ")));\n";
        break;
    }
    case psprecomp::OpcodeKind::Svs: {
        const std::int32_t offset = static_cast<std::int16_t>(d.word & 0xFFFCu);
        const std::uint32_t scalar_register = ((d.word >> 16u) & 0x1Fu) | ((d.word & 3u) << 5u);
        out << "    rt.memory().aot_store32(" << reg(d.rs) << " + static_cast<std::uint32_t>(" << offset
            << "), ctx.vfpu_scalar_bits(" << scalar_register << "u));\n";
        break;
    }
    case psprecomp::OpcodeKind::Lvq: {
        const std::int32_t offset = static_cast<std::int16_t>(d.word & 0xFFFCu);
        const std::uint32_t vector_register = ((d.word >> 16u) & 0x1Fu) | ((d.word & 1u) << 5u);
        out << "    { const std::uint32_t vfpu_address = " << reg(d.rs) << " + static_cast<std::uint32_t>(" << offset << ");\n"
            << "      float vfpu_value[4]{\n"
            << "        std::bit_cast<float>(rt.memory().aot_load32(vfpu_address + 0u)),\n"
            << "        std::bit_cast<float>(rt.memory().aot_load32(vfpu_address + 4u)),\n"
            << "        std::bit_cast<float>(rt.memory().aot_load32(vfpu_address + 8u)),\n"
            << "        std::bit_cast<float>(rt.memory().aot_load32(vfpu_address + 12u))};\n"
            << "      ctx.write_vfpu_vector(vfpu_value, " << vector_register << "u, 4u); }\n";
        break;
    }
    case psprecomp::OpcodeKind::Svq: {
        const std::int32_t offset = static_cast<std::int16_t>(d.word & 0xFFFCu);
        const std::uint32_t vector_register = ((d.word >> 16u) & 0x1Fu) | ((d.word & 1u) << 5u);
        out << "    { float vfpu_value[4]{}; ctx.read_vfpu_vector(vfpu_value, " << vector_register << "u, 4u);\n"
            << "      const std::uint32_t vfpu_address = " << reg(d.rs) << " + static_cast<std::uint32_t>(" << offset << ");\n"
            << "      rt.memory().aot_store32(vfpu_address + 0u, std::bit_cast<std::uint32_t>(vfpu_value[0]));\n"
            << "      rt.memory().aot_store32(vfpu_address + 4u, std::bit_cast<std::uint32_t>(vfpu_value[1]));\n"
            << "      rt.memory().aot_store32(vfpu_address + 8u, std::bit_cast<std::uint32_t>(vfpu_value[2]));\n"
            << "      rt.memory().aot_store32(vfpu_address + 12u, std::bit_cast<std::uint32_t>(vfpu_value[3])); }\n";
        break;
    }
    default:
        out << "    rt.unsupported(" << psprecomp::hex32(pc) << "u, " << psprecomp::hex32(d.word)
            << "u, \"" << cpp_escape(d.mnemonic) << " not lowered yet\"); return;\n";
        break;
    }
    return out.str();
}

bool is_branch(psprecomp::OpcodeKind kind) {
    using K = psprecomp::OpcodeKind;
    switch (kind) {
    case K::Beq: case K::Bne: case K::Beql: case K::Bnel:
    case K::Blez: case K::Bgtz: case K::Blezl: case K::Bgtzl:
    case K::Bltz: case K::Bgez: case K::Bltzl: case K::Bgezl:
    case K::Bltzal: case K::Bgezal: case K::Bltzall: case K::Bgezall:
    case K::Bc1f: case K::Bc1t: case K::Bc1fl: case K::Bc1tl:
    case K::Bvf: case K::Bvt: case K::Bvfl: case K::Bvtl:
        return true;
    default:
        return false;
    }
}

bool is_likely_branch(psprecomp::OpcodeKind kind) {
    using K = psprecomp::OpcodeKind;
    return kind == K::Beql || kind == K::Bnel || kind == K::Blezl || kind == K::Bgtzl ||
           kind == K::Bltzl || kind == K::Bgezl || kind == K::Bltzall || kind == K::Bgezall ||
           kind == K::Bc1fl || kind == K::Bc1tl || kind == K::Bvfl || kind == K::Bvtl;
}

bool is_link_branch(psprecomp::OpcodeKind kind) {
    using K = psprecomp::OpcodeKind;
    return kind == K::Bltzal || kind == K::Bgezal || kind == K::Bltzall || kind == K::Bgezall;
}

std::string branch_condition(const psprecomp::DecodedInstruction &d) {
    using K = psprecomp::OpcodeKind;
    switch (d.kind) {
    case K::Beq: case K::Beql: return reg(d.rs) + " == " + reg(d.rt);
    case K::Bne: case K::Bnel: return reg(d.rs) + " != " + reg(d.rt);
    case K::Blez: case K::Blezl: return "static_cast<std::int32_t>(" + reg(d.rs) + ") <= 0";
    case K::Bgtz: case K::Bgtzl: return "static_cast<std::int32_t>(" + reg(d.rs) + ") > 0";
    case K::Bltz: case K::Bltzl: case K::Bltzal: case K::Bltzall:
        return "static_cast<std::int32_t>(" + reg(d.rs) + ") < 0";
    case K::Bgez: case K::Bgezl: case K::Bgezal: case K::Bgezall:
        return "static_cast<std::int32_t>(" + reg(d.rs) + ") >= 0";
    case K::Bc1f: case K::Bc1fl: return "!ctx.fpu_condition()";
    case K::Bc1t: case K::Bc1tl: return "ctx.fpu_condition()";
    case K::Bvf: case K::Bvfl: {
        const std::uint32_t condition_index = (d.word >> 18u) & 7u;
        return "((ctx.vfpu_ctrl[3] >> " + std::to_string(condition_index) + "u) & 1u) == 0u";
    }
    case K::Bvt: case K::Bvtl: {
        const std::uint32_t condition_index = (d.word >> 18u) & 7u;
        return "((ctx.vfpu_ctrl[3] >> " + std::to_string(condition_index) + "u) & 1u) != 0u";
    }
    default: return "false";
    }
}

std::string generated_unit_cpp_name(std::uint32_t unit) {
    std::ostringstream name;
    name << "recomp_unit_" << std::setfill('0') << std::setw(4) << unit;
    return name.str();
}

std::string generated_unit_cpp_entry_name(std::uint32_t unit) {
    return generated_unit_cpp_name(unit) + "_entry";
}

std::string direct_unit_chain_expression(
    std::uint32_t unit, std::uint32_t target,
    const std::map<std::uint32_t, std::uint16_t> *direct_entry_ids) {
    if (direct_entry_ids != nullptr) {
        const auto found = direct_entry_ids->find(target);
        if (found != direct_entry_ids->end() && found->second != 0u) {
            return "rt.invoke_chained_direct<&" + generated_unit_cpp_entry_name(unit) + ", " +
                std::to_string(unit) + "u, " + std::to_string(found->second) + "u, " +
                psprecomp::hex32(target) + "u>(ctx, &aot_mem)";
        }
    }
    // Every emitted entry has an id, so a target without one has no generated
    // unit to name (e.g. a JAL past the end of the executable segment). Hand it
    // to the generic per-PC dispatcher instead.
    (void)unit;
    return "(ctx.pc = " + psprecomp::hex32(target) + "u, rt.invoke_chained_call(ctx, &aot_mem))";
}

void emit_target(std::ostringstream &body, std::uint32_t target,
                 const std::set<std::uint32_t> &labels, const char *indent,
                 std::uint32_t executable_base = 0u,
                 std::uint32_t unit_span_bytes = 0u,
                 const std::map<std::uint32_t, std::uint16_t> *direct_entry_ids = nullptr,
                 const std::set<std::uint32_t> *import_stubs = nullptr) {
    if (labels.contains(target)) {
        body << indent << "goto L_" << psprecomp::hex32(target).substr(2) << ";\n";
        return;
    }

    // A fixed J/JAL to a PSP import must return to the outer dispatcher.
    // Trying the generated-unit chain first is guaranteed to fail because import
    // registration deliberately poisons/replaces that exact PC, and these stubs
    // are frequently hot in real titles. Emit the minimal correct handoff directly.
    if (import_stubs != nullptr && import_stubs->contains(target)) {
        body << indent << "ctx.pc = " << psprecomp::hex32(target) << "u; return;\n";
        return;
    }

    // A fixed-span AOT partition can split one guest CFG across neighbouring
    // C++ translation units. knows the destination bucket for every
    // fixed direct edge, so the common case bypasses Runtime's large per-PC
    // chain table and indexes the tiny unit table instead. A bucket containing
    // an import/HLE/host replacement is marked overridden at registration time
    // and invoke_chained_unit() falls back to the exact per-PC lookup there.
    if (unit_span_bytes != 0u && target >= executable_base) {
        const std::uint32_t unit = (target - executable_base) / unit_span_bytes;
        body << indent << "(void)" << direct_unit_chain_expression(unit, target, direct_entry_ids)
             << "; return;\n";
    } else {
        body << indent << "ctx.pc = " << psprecomp::hex32(target)
             << "u; (void)rt.invoke_chained_call(ctx, &aot_mem); return;\n";
    }
}


struct GeneratedFunctionInput {
    std::string name;
    std::uint32_t address{};
    std::set<std::uint32_t> instructions;
    std::set<std::uint32_t> entry_labels;
    std::uint32_t executable_base{};
    std::uint32_t unit_span_bytes{};
    const std::map<std::uint32_t, std::uint16_t> *direct_entry_ids{};
    const std::set<std::uint32_t> *import_stubs{};
};

std::string emit_function_source(const GeneratedFunctionInput &function,
                                 const psprecomp::GuestMemory &memory,
                                 const std::string &cpp_name) {
    std::ostringstream body;

    // Unit entry dispatch.  A `switch (ctx.pc)` over several hundred sparse
    // guest addresses compiles to a branch tree, and every outer dispatch pays
    // for it.  When the entries fit a bounded window we emit a dense
    // offset -> contiguous id table instead, so the switch covers 1..N with no
    // holes and lowers to a single jump table: two loads and one indirect jump.
    // The guarded offset/alignment test keeps the accepted PC set identical to
    // the exact comparisons it replaces.
    constexpr std::uint64_t kMaximumDenseSlots = 8192u;
    const bool dense_dispatch = !function.entry_labels.empty() &&
        (static_cast<std::uint64_t>(*function.entry_labels.rbegin() - *function.entry_labels.begin()) / 4u + 1u)
            <= kMaximumDenseSlots;
    const std::uint32_t dense_base = function.entry_labels.empty() ? 0u : *function.entry_labels.begin();
    const std::uint32_t dense_span = function.entry_labels.empty()
        ? 0u
        : (*function.entry_labels.rbegin() - dense_base) + 4u;
    const std::string table_name = "kEntryIds_" + cpp_name;

    if (dense_dispatch) {
        std::map<std::uint32_t, std::uint32_t> id_by_label;
        std::uint32_t next_id = 1u;
        for (const auto label : function.entry_labels) id_by_label[label] = next_id++;

        body << "static const std::uint16_t " << table_name << "[" << (dense_span / 4u) << "] = {";
        for (std::uint32_t offset = 0u; offset < dense_span; offset += 4u) {
            const auto found = id_by_label.find(dense_base + offset);
            if ((offset / 4u) % 32u == 0u) body << "\n   ";
            body << ' ' << (found != id_by_label.end() ? found->second : 0u) << ',';
        }
        body << "\n};\n";

        body << "void " << cpp_name << "_entry(Runtime &rt, AllegrexContext &ctx, std::uint16_t direct_entry_id, GuestMemory::AotFastView &aot_mem) {\n"
             << "    std::uint32_t jump_target = 0u;\n"
             << "    std::uint32_t local_transfers = 0u;\n"
             << "    std::uint32_t local_pc = ctx.pc;\n"
             << "    std::uint32_t entry_id = direct_entry_id;\n"
             << "LOCAL_DISPATCH:\n"
             << "    {\n"
             << "    if (entry_id == 0u) {\n"
             << "        const std::uint32_t entry_delta = local_pc - " << psprecomp::hex32(dense_base) << "u;\n"
             << "        entry_id = (entry_delta < " << dense_span
             << "u && (entry_delta & 3u) == 0u) ? " << table_name << "[entry_delta >> 2u] : 0u;\n"
             << "    }\n"
             << "    switch (entry_id) {\n";
        for (const auto label : function.entry_labels) {
            body << "    case " << id_by_label[label] << "u: goto L_"
                 << psprecomp::hex32(label).substr(2) << ";\n";
        }
        body << "    default:\n"
             << "        if (local_transfers == 0u) rt.unsupported(ctx.pc, 0u, \"invalid internal function entry\");\n"
             // Local transfers advance local_pc without touching AllegrexContext.
             // Leaving the unit through this default arm hands control back to the
             // outer dispatcher, for which ctx.pc is the only statement of where to
             // resume -- so it must be materialized here.  Without this store the
             // dispatcher re-enters at the unit's original entry PC and executes
             // the wrong code.  local_transfers == 0u needs no store because no
             // local jump has happened yet and ctx.pc is still current.
             << "        else ctx.pc = local_pc;\n"
             << "        return;\n"
             << "    }\n"
             << "    }\n";
    } else {
        body << "void " << cpp_name << "_entry(Runtime &rt, AllegrexContext &ctx, std::uint16_t direct_entry_id, GuestMemory::AotFastView &aot_mem) {\n"
             << "    (void)direct_entry_id;\n"
             << "    std::uint32_t jump_target = 0u;\n"
             << "    std::uint32_t local_transfers = 0u;\n"
             << "LOCAL_DISPATCH:\n"
             << "    switch (ctx.pc) {\n";
        for (const auto label : function.entry_labels) {
            body << "    case " << psprecomp::hex32(label) << "u: goto L_"
                 << psprecomp::hex32(label).substr(2) << ";\n";
        }
        body << "    default:\n"
             << "        if (local_transfers == 0u) rt.unsupported(ctx.pc, 0u, \"invalid internal function entry\");\n"
             // Local transfers advance local_pc without touching AllegrexContext.
             // Leaving the unit through this default arm hands control back to the
             // outer dispatcher, for which ctx.pc is the only statement of where to
             // resume -- so it must be materialized here.  Without this store the
             // dispatcher re-enters at the unit's original entry PC and executes
             // the wrong code.  local_transfers == 0u needs no store because no
             // local jump has happened yet and ctx.pc is still current.
             << "        else ctx.pc = local_pc;\n"
             << "        return;\n"
             << "    }\n";
    }

    for (const auto block_start : function.entry_labels) {
        body << "L_" << psprecomp::hex32(block_start).substr(2) << ":\n";
        std::uint32_t pc = block_start;
        while (function.instructions.contains(pc)) {
            if (pc != block_start && function.entry_labels.contains(pc)) {
                body << "    goto L_" << psprecomp::hex32(pc).substr(2) << ";\n";
                break;
            }

            const auto decoded = psprecomp::decode_allegrex(memory.load32(pc));
            if (decoded.has_delay_slot()) {
                const auto slot = psprecomp::decode_allegrex(memory.load32(pc + 4u));
                if (slot.is_control_flow()) {
                    body << "    rt.unsupported(" << psprecomp::hex32(pc + 4u) << "u, "
                         << psprecomp::hex32(slot.word)
                         << "u, \"control flow in delay slot\"); return;\n";
                    break;
                }

                if (is_branch(decoded.kind)) {
                    const std::uint32_t target = pc + 4u + static_cast<std::uint32_t>(
                        static_cast<std::int32_t>(decoded.immediate) * 4);
                    const std::uint32_t fallthrough = pc + 8u;
                    const bool likely_branch = is_likely_branch(decoded.kind);
                    const std::string condition = branch_condition(decoded);
                    if (is_link_branch(decoded.kind)) {
                        body << "    ctx.set_gpr(31, " << psprecomp::hex32(pc + 8u) << "u);\n";
                    }
                    if (likely_branch) {
                        body << "    if (" << condition << ") {\n"
                             << emit_regular(slot, pc + 4u);
                        emit_target(body, target, function.entry_labels, "        ", function.executable_base, function.unit_span_bytes, function.direct_entry_ids, function.import_stubs);
                        body << "    }\n";
                        emit_target(body, fallthrough, function.entry_labels, "    ", function.executable_base, function.unit_span_bytes, function.direct_entry_ids, function.import_stubs);
                    } else {
                        body << "    { const bool branch_taken = " << condition << ";\n"
                             << emit_regular(slot, pc + 4u)
                             << "      if (branch_taken) {\n";
                        emit_target(body, target, function.entry_labels, "          ", function.executable_base, function.unit_span_bytes, function.direct_entry_ids, function.import_stubs);
                        body << "      }\n";
                        emit_target(body, fallthrough, function.entry_labels, "      ", function.executable_base, function.unit_span_bytes, function.direct_entry_ids, function.import_stubs);
                        body << "    }\n";
                    }
                } else if (decoded.kind == psprecomp::OpcodeKind::J ||
                           decoded.kind == psprecomp::OpcodeKind::Jal) {
                    const std::uint32_t target = ((pc + 4u) & 0xF0000000u) | (decoded.target << 2u);
                    if (decoded.kind == psprecomp::OpcodeKind::Jal) {
                        body << "    ctx.set_gpr(31, " << psprecomp::hex32(pc + 8u) << "u);\n";
                    }
                    body << emit_regular(slot, pc + 4u);
                    if (decoded.kind == psprecomp::OpcodeKind::J) {
                        emit_target(body, target, function.entry_labels, "    ", function.executable_base, function.unit_span_bytes, function.direct_entry_ids, function.import_stubs);
                    } else if (function.entry_labels.contains(target)) {
                        // Fixed same-unit JAL: the destination is already a C++
                        // label. Going through ctx.pc + LOCAL_DISPATCH needlessly
                        // re-decodes a dense entry id and burns the local-transfer
                        // counter. The delay slot and $ra write have already run.
                        body << "    goto L_" << psprecomp::hex32(target).substr(2) << ";\n";
                    } else {
                        // Cross-unit call. PSP import stubs are deliberate outer-
                        // dispatcher boundaries; do not pay a guaranteed-failing
                        // generated-unit chain attempt before reaching them.
                        const std::uint32_t return_pc = pc + 8u;
                        const bool target_is_import = function.import_stubs != nullptr &&
                            function.import_stubs->contains(target);
                        if (target_is_import) {
                            body << "    ctx.pc = " << psprecomp::hex32(target) << "u;\n"
                                 << "    return;\n";
                            // The block ends here; `continue` would re-emit this
                            // same PC forever.
                            break;
                        }
                        // Otherwise run the callee inline and resume locally only
                        // if it came back to our return address.
                        const bool direct_unit = function.unit_span_bytes != 0u &&
                            target >= function.executable_base;
                        const std::uint32_t target_unit = direct_unit
                            ? (target - function.executable_base) / function.unit_span_bytes : 0u;
                        if (!direct_unit)
                            body << "    ctx.pc = " << psprecomp::hex32(target) << "u;\n";
                        if (function.entry_labels.contains(return_pc)) {
                            body << "    if (";
                            if (direct_unit)
                                body << direct_unit_chain_expression(target_unit, target, function.direct_entry_ids);
                            else
                                body << "rt.invoke_chained_call(ctx, &aot_mem)";
                            body << " && ctx.pc == " << psprecomp::hex32(return_pc)
                                 << "u) goto L_" << psprecomp::hex32(return_pc).substr(2) << ";\n";
                        } else {
                            if (direct_unit)
                                body << "    (void)" << direct_unit_chain_expression(target_unit, target, function.direct_entry_ids) << ";\n";
                            else
                                body << "    (void)rt.invoke_chained_call(ctx, &aot_mem);\n";
                        }
                        body << "    return;\n";
                    }
                } else {
                    const std::uint32_t link = decoded.rd == 0u ? 31u : decoded.rd;
                    body << "    jump_target = " << reg(decoded.rs) << ";\n";
                    if (decoded.kind == psprecomp::OpcodeKind::Jalr) {
                        body << "    ctx.set_gpr(" << link << ", " << psprecomp::hex32(pc + 8u) << "u);\n";
                    }
                    body << emit_regular(slot, pc + 4u);
                    if (decoded.kind == psprecomp::OpcodeKind::Jalr) {
                        // Indirect call: same bounded chaining as a direct one.
                        // The runtime rejects import wrappers, so a JALR into
                        // the kernel still leaves through the dispatcher.
                        const std::uint32_t return_pc = pc + 8u;
                        body << "    ctx.pc = jump_target;\n";
                        if (function.entry_labels.contains(return_pc)) {
                            body << "    if (rt.invoke_chained_call(ctx, &aot_mem) && ctx.pc == "
                                 << psprecomp::hex32(return_pc) << "u) goto L_"
                                 << psprecomp::hex32(return_pc).substr(2) << ";\n";
                        } else {
                            body << "    (void)rt.invoke_chained_call(ctx, &aot_mem);\n";
                        }
                        body << "    return;\n";
                    } else {
                        // Plain JR, including `jr $ra`.  A return that lands in
                        // this unit continues locally; otherwise the chained
                        // caller one frame up recognizes its return address.
                        body << "    local_pc = jump_target;\n"
                             << "    if (++local_transfers < 2048u) { entry_id = 0u; goto LOCAL_DISPATCH; }\n"
                             << "    ctx.pc = jump_target;\n"
                             << "    return;\n";
                    }
                }
                break;
            }

            if (decoded.kind == psprecomp::OpcodeKind::Break) {
                // GCC emits `break 7` after a zero-divisor check; reaching one is a guest fault.
                body << "    rt.stop(\"Guest BREAK code " << ((decoded.word >> 6u) & 0xFFFFFu) << " at "
                     << psprecomp::hex32(pc) << "\"); return;\n";
                break;
            }
            if (decoded.kind == psprecomp::OpcodeKind::Syscall ||
                decoded.kind == psprecomp::OpcodeKind::Vfpu ||
                decoded.kind == psprecomp::OpcodeKind::Unsupported) {
                body << "    rt.unsupported(" << psprecomp::hex32(pc) << "u, "
                     << psprecomp::hex32(decoded.word) << "u, \""
                     << cpp_escape(decoded.mnemonic) << " not lowered yet\"); return;\n";
                break;
            }

            body << emit_regular(decoded, pc);
            const std::uint32_t next = pc + 4u;
            if (!function.instructions.contains(next)) {
                body << "    ctx.pc = " << psprecomp::hex32(next) << "u; return;\n";
                break;
            }
            if (function.entry_labels.contains(next)) {
                body << "    goto L_" << psprecomp::hex32(next).substr(2) << ";\n";
                break;
            }
            pc = next;
        }
    }
    body << "}\n\n";
    body << "void " << cpp_name << "(Runtime &rt, AllegrexContext &ctx) {\n"
         << "    auto aot_mem = rt.memory().aot_fast_view();\n"
         << "    " << cpp_name << "_entry(rt, ctx, 0u, aot_mem);\n}\n\n";
    return body.str();
}

void write_import_wrappers(std::ostream &out, const std::vector<psprecomp::PspImport> &imports) {
    for (std::size_t i = 0; i < imports.size(); ++i) {
        out << "static void import_" << i << "(Runtime &rt, AllegrexContext &ctx) {\n"
            << "    const RuntimeExecutionContextToken caller_context = capture_runtime_execution_context();\n"
            << "    const std::uint32_t import_pc = ctx.pc;\n"
            << "    const std::uint32_t return_address = ctx.gpr[31];\n"
            << "    rt.invoke_import_cached(" << i << "u, \"" << cpp_escape(imports[i].library) << "\", "
            << psprecomp::hex32(imports[i].nid) << "u, ctx);\n"
            << "    if (!rt.stopped() && runtime_execution_context_matches(caller_context) &&\n"
            << "        ctx.pc == import_pc) ctx.pc = return_address;\n"
            << "}\n\n";
    }
}

int generate_manual(const std::filesystem::path &elf_path,
                    const std::filesystem::path &csv_path,
                    const std::filesystem::path &output_path) {
    const auto elf = psprecomp::Elf32Image::from_file(elf_path);
    auto functions = load_functions(csv_path);
    const std::uint32_t load_base = psprecomp::kDefaultPspUserLoadBase;
    if (elf.is_psp_prx()) {
        for (auto &function : functions) if (function.address < load_base) function.address += load_base;
    }

    psprecomp::GuestMemory memory;
    (void)elf.load_and_relocate(memory, load_base);
    std::vector<psprecomp::PspImport> imports;
    if (const auto module = elf.find_module_info(memory, load_base)) imports = elf.scan_imports(memory, *module);

    std::ofstream out(output_path);
    if (!out) throw psprecomp::Error("Cannot create generated source");
    out << "#include \"psprecomp/runtime.hpp\"\n#include <bit>\n#include <cmath>\n#include <cstdint>\n#include <limits>\n\nnamespace psprecomp {\n";

    std::vector<GeneratedFunctionInput> generated;
    for (const auto &function : functions) {
        GeneratedFunctionInput input{function.name, function.address, {}, {}};
        const std::uint32_t end = function.address + function.size;
        for (std::uint32_t pc = function.address; pc < end;) {
            input.instructions.insert(pc);
            input.entry_labels.insert(pc);
            const auto decoded = psprecomp::decode_allegrex(memory.load32(pc));
            pc += decoded.has_delay_slot() ? 8u : 4u;
        }
        out << emit_function_source(input, memory, safe_name(input.name, input.address));
        generated.push_back(std::move(input));
    }

    write_import_wrappers(out, imports);
    out << "void register_generated_functions(Runtime &runtime) {\n";
    for (const auto &function : generated) {
        const auto cpp_name = safe_name(function.name, function.address);
        for (const auto label : function.entry_labels) {
            out << "    runtime.register_function(" << psprecomp::hex32(label) << "u, &"
                << cpp_name << ", \"" << cpp_escape(function.name) << "\");\n";
        }
    }
    for (std::size_t i = 0; i < imports.size(); ++i) {
        out << "    runtime.register_function(" << psprecomp::hex32(imports[i].stub_address)
            << "u, &import_" << i << ", \"" << cpp_escape(imports[i].library)
            << "::" << psprecomp::hex32(imports[i].nid) << "\");\n";
    }
    out << "}\n} // namespace psprecomp\n";
    std::cout << "Generated " << functions.size() << " manual functions, " << imports.size()
              << " import wrappers into " << output_path.string() << "\n";
    return 0;
}


// The generated corpus uses architectural register numbers known
// at code-generation time.  Keeping those writes behind AllegrexContext::set_gpr()
// leaves a call/branch-shaped abstraction at more than half a million static
// sites.  MSVC cannot reliably inline a tiny helper into the enormous AOT unit
// functions even under LTCG, so lower constant writes in the emitted source:
//   ctx.set_gpr(8, expr) -> ctx.gpr[8] = (expr)
//   ctx.set_gpr(0, expr) -> (void)(expr)
// The second form preserves source/memory evaluation (including any guest
// fault/side effect) while discarding only the architectural write to $zero.
// This pass is deliberately limited to --auto output; manual fixtures retain the
// readable helper form used by their source-level tests.
std::string lower_constant_gpr_writes(std::string text) {
    constexpr std::string_view needle = "ctx.set_gpr(";
    std::size_t search = 0u;
    while ((search = text.find(needle, search)) != std::string::npos) {
        const std::size_t open = search + needle.size() - 1u;
        std::size_t cursor = open + 1u;
        while (cursor < text.size() && std::isspace(static_cast<unsigned char>(text[cursor]))) ++cursor;
        const std::size_t index_begin = cursor;
        while (cursor < text.size() && std::isdigit(static_cast<unsigned char>(text[cursor]))) ++cursor;
        if (cursor == index_begin) { search += needle.size(); continue; }
        const std::uint32_t index = static_cast<std::uint32_t>(
            std::strtoul(text.substr(index_begin, cursor - index_begin).c_str(), nullptr, 10));
        while (cursor < text.size() && std::isspace(static_cast<unsigned char>(text[cursor]))) ++cursor;
        if (cursor >= text.size() || text[cursor] != ',') { search += needle.size(); continue; }
        const std::size_t expression_begin = cursor + 1u;

        int paren_depth = 1;
        bool in_string = false;
        bool in_char = false;
        bool escaped = false;
        std::size_t close = std::string::npos;
        for (cursor = expression_begin; cursor < text.size(); ++cursor) {
            const char ch = text[cursor];
            if (escaped) { escaped = false; continue; }
            if ((in_string || in_char) && ch == '\\') { escaped = true; continue; }
            if (!in_char && ch == '"') { in_string = !in_string; continue; }
            if (!in_string && ch == '\'') { in_char = !in_char; continue; }
            if (in_string || in_char) continue;
            if (ch == '(') ++paren_depth;
            else if (ch == ')' && --paren_depth == 0) { close = cursor; break; }
        }
        if (close == std::string::npos) { search += needle.size(); continue; }

        std::size_t first = expression_begin;
        std::size_t last = close;
        while (first < last && std::isspace(static_cast<unsigned char>(text[first]))) ++first;
        while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1u]))) --last;
        const std::string expression = text.substr(first, last - first);
        std::string replacement;
        if (index == 0u) {
            replacement = "(void)(" + expression + ")";
        } else if (index < 32u) {
            replacement = "ctx.gpr[" + std::to_string(index) + "] = (" + expression + ")";
        } else {
            // Defensive fallback.  Automatic Allegrex decoding should never emit
            // an out-of-range architectural GPR, but preserve validation if it does.
            search = close + 1u;
            continue;
        }
        text.replace(search, close - search + 1u, replacement);
        search += replacement.size();
    }
    return text;
}

std::string lower_aot_memory_accesses(std::string text) {
    // the outer generated-unit wrapper creates one AotFastView and
    // compile-time direct chains pass it by reference across unit boundaries.
    // Lower ordinary aligned byte/half/word accesses to that shared view.
    static const std::regex access_pattern(
        R"(rt\.memory\(\)\.aot_(load|store)(8|16|32)\()" );
    return std::regex_replace(text, access_pattern, "aot_mem.aot_$1$2(");
}

std::string lower_constant_fpr_accesses(std::string text) {
    // All Allegrex FPR operands emitted by the decoder are architectural
    // constants in [0,31]. Avoid helper calls/bounds branches in enormous AOT
    // functions so MSVC can keep float lanes in registers across basic blocks.
    const std::regex read_pattern(R"(ctx\.fpr_bits\(([0-9]+)\))");
    std::smatch match;
    std::string lowered;
    lowered.reserve(text.size());
    std::string::const_iterator begin = text.cbegin();
    while (std::regex_search(begin, text.cend(), match, read_pattern)) {
        lowered.append(begin, match[0].first);
        const auto index = static_cast<std::uint32_t>(std::strtoul(match[1].str().c_str(), nullptr, 10));
        if (index < 32u)
            lowered += "std::bit_cast<std::uint32_t>(ctx.fpr[" + std::to_string(index) + "])";
        else
            lowered.append(match[0].first, match[0].second);
        begin = match[0].second;
    }
    lowered.append(begin, text.cend());
    text = std::move(lowered);

    constexpr std::string_view needle = "ctx.set_fpr_bits(";
    std::size_t search = 0u;
    while ((search = text.find(needle, search)) != std::string::npos) {
        std::size_t cursor = search + needle.size();
        while (cursor < text.size() && std::isspace(static_cast<unsigned char>(text[cursor]))) ++cursor;
        const std::size_t index_begin = cursor;
        while (cursor < text.size() && std::isdigit(static_cast<unsigned char>(text[cursor]))) ++cursor;
        if (cursor == index_begin) { search += needle.size(); continue; }
        const auto index = static_cast<std::uint32_t>(
            std::strtoul(text.substr(index_begin, cursor - index_begin).c_str(), nullptr, 10));
        while (cursor < text.size() && std::isspace(static_cast<unsigned char>(text[cursor]))) ++cursor;
        if (index >= 32u || cursor >= text.size() || text[cursor] != ',') {
            search += needle.size(); continue;
        }
        const std::size_t expression_begin = cursor + 1u;
        int depth = 1;
        bool in_string = false, in_char = false, escaped = false;
        std::size_t close = std::string::npos;
        for (cursor = expression_begin; cursor < text.size(); ++cursor) {
            const char ch = text[cursor];
            if (escaped) { escaped = false; continue; }
            if ((in_string || in_char) && ch == '\\') { escaped = true; continue; }
            if (!in_char && ch == '"') { in_string = !in_string; continue; }
            if (!in_string && ch == '\'') { in_char = !in_char; continue; }
            if (in_string || in_char) continue;
            if (ch == '(') ++depth;
            else if (ch == ')' && --depth == 0) { close = cursor; break; }
        }
        if (close == std::string::npos) { search += needle.size(); continue; }
        std::size_t first = expression_begin, last = close;
        while (first < last && std::isspace(static_cast<unsigned char>(text[first]))) ++first;
        while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1u]))) --last;
        const std::string expression = text.substr(first, last - first);
        const std::string replacement = "ctx.fpr[" + std::to_string(index) +
            "] = std::bit_cast<float>(" + expression + ")";
        text.replace(search, close - search + 1u, replacement);
        search += replacement.size();
    }
    return text;
}

// VFPU operands in automatic AOT are encoded literals. Convert the
// generic lane/prefix helpers into compile-time templates so row/column mapping,
// scalar-vs-control selection and 1..4-lane loops disappear before MSVC sees the
// giant translation units. Dynamic-size matrix helpers remain untouched.
std::string lower_constant_vfpu_accesses(std::string text) {
    const std::regex read_prefix(
        R"(ctx\.read_vfpu_vector_with_source_prefix\(([A-Za-z0-9_]+), ([0-9]+)u, ([1-4])u, ([01])u\);)" );
    text = std::regex_replace(text, read_prefix,
        "ctx.read_vfpu_vector_with_source_prefix_ct<$2u, $3u, $4u>($1);");

    const std::regex read_plain(
        R"(ctx\.read_vfpu_vector\(([A-Za-z0-9_]+), ([0-9]+)u, ([1-4])u\);)" );
    text = std::regex_replace(text, read_plain,
        "ctx.read_vfpu_vector_ct<$2u, $3u>($1);");

    const std::regex write_prefix(
        R"(ctx\.write_vfpu_vector_with_destination_prefix\(([A-Za-z0-9_]+), ([0-9]+)u, ([1-4])u\);)" );
    text = std::regex_replace(text, write_prefix,
        "ctx.write_vfpu_vector_with_destination_prefix_ct<$2u, $3u>($1);");

    const std::regex write_plain(
        R"(ctx\.write_vfpu_vector\(([A-Za-z0-9_]+), ([0-9]+)u, ([1-4])u\);)" );
    text = std::regex_replace(text, write_plain,
        "ctx.write_vfpu_vector_ct<$2u, $3u>($1);");

    const std::regex source_prefix_plain(
        R"(ctx\.apply_vfpu_source_prefix\(([A-Za-z0-9_]+), ([1-4])u, ([01])u\);)" );
    text = std::regex_replace(text, source_prefix_plain,
        "ctx.apply_vfpu_source_prefix_ct<$2u, $3u>($1);");

    const std::regex fpu_to_word(
        R"(ctx\.fpu_float_to_word\(([^,\n]+), ([0-3])u\))" );
    text = std::regex_replace(text, fpu_to_word,
        "ctx.fpu_float_to_word_ct<$2u>($1)");

    const std::regex vcmp(
        R"(ctx\.execute_vfpu_vcmp\(([0-9]+)u, ([0-9]+)u, ([1-4])u, ([0-9]+)u\);)" );
    text = std::regex_replace(text, vcmp,
        "ctx.execute_vfpu_vcmp_ct<$1u, $2u, $3u, $4u>();");

    const std::regex vcmov(
        R"(ctx\.execute_vfpu_vcmov\(([0-9]+)u, ([0-9]+)u, ([1-4])u, ([0-7])u, (true|false)\);)" );
    text = std::regex_replace(text, vcmov,
        "ctx.execute_vfpu_vcmov_ct<$1u, $2u, $3u, $4u, $5>();");

    const std::regex vdot(
        R"(ctx\.execute_vfpu_vdot\(([0-9]+)u, ([0-9]+)u, ([0-9]+)u, ([1-4])u\);)" );
    text = std::regex_replace(text, vdot,
        "ctx.execute_vfpu_vdot_ct<$1u, $2u, $3u, $4u>();");

    const std::regex vscl(
        R"(ctx\.execute_vfpu_vscl\(([0-9]+)u, ([0-9]+)u, ([0-9]+)u, ([1-4])u\);)" );
    text = std::regex_replace(text, vscl,
        "ctx.execute_vfpu_vscl_ct<$1u, $2u, $3u, $4u>();");

    const std::regex scalar_read(R"(ctx\.vfpu_scalar_bits\(([0-9]+)u\))");
    text = std::regex_replace(text, scalar_read,
        "ctx.vfpu_scalar_bits_ct<$1u>()");

    // set_vfpu_scalar_bits second arguments in generated code stay on one line.
    const std::regex scalar_write(
        R"(ctx\.set_vfpu_scalar_bits\(([0-9]+)u, ([^;\n]+)\);)" );
    text = std::regex_replace(text, scalar_write,
        "ctx.set_vfpu_scalar_bits_ct<$1u>($2);");
    return text;
}

bool write_text_if_changed(const std::filesystem::path &path, const std::string &text) {
    if (std::ifstream existing(path, std::ios::binary); existing) {
        const std::string old((std::istreambuf_iterator<char>(existing)), std::istreambuf_iterator<char>());
        if (old == text) return false;
    }
    const auto temporary = path.string() + ".tmp";
    {
        std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
        if (!out) throw psprecomp::Error("Cannot create generated file: " + path.string());
        out.write(text.data(), static_cast<std::streamsize>(text.size()));
    }
    std::filesystem::rename(temporary, path);
    return true;
}

// Overlay mode: `elf_path` is the host executable, `overlay_path` a fixed-address
// code overlay. Units are bucketed from the host's code base so they line up
// with the host corpus, the host's import stubs are honoured, no import
// wrappers are emitted, and everything is wrapped in `cpp_namespace`.
struct OverlayOptions {
    std::filesystem::path overlay_path;
    std::string cpp_namespace;
};

int generate_auto(const std::filesystem::path &elf_path,
                  const std::filesystem::path &output_dir,
                  std::uint32_t load_base,
                  std::uint32_t unit_span_bytes,
                  const OverlayOptions *overlay = nullptr,
                  const std::vector<std::filesystem::path> &reference_overlays = {}) {
    const auto elf = psprecomp::Elf32Image::from_file(elf_path);
    psprecomp::GuestMemory memory;
    (void)elf.load_and_relocate(memory, load_base);
    std::vector<psprecomp::PspImport> imports;
    if (const auto module = elf.find_module_info(memory, load_base)) imports = elf.scan_imports(memory, *module);
    std::set<std::uint32_t> import_stubs;
    for (const auto &import : imports) import_stubs.insert(import.stub_address);
    std::optional<psprecomp::Elf32Image> overlay_elf;
    if (overlay != nullptr) {
        overlay_elf.emplace(psprecomp::Elf32Image::from_file(overlay->overlay_path));
        (void)overlay_elf->load_and_relocate(memory, 0u);
    }
    // Host functions that only overlays call are entry points too. Overlays may
    // share addresses with each other, so each is loaded and scanned in turn;
    // none overlaps the host's own code.
    std::map<std::uint32_t, std::string> overlay_references;
    if (!reference_overlays.empty()) {
        const auto host_ranges = psprecomp::code_ranges(elf, load_base);
        for (const auto &path : reference_overlays) {
            const auto referencing = psprecomp::Elf32Image::from_file(path);
            (void)referencing.load_and_relocate(memory, 0u);
            psprecomp::collect_references_into(referencing, memory, 0u, host_ranges, overlay_references);
        }
        for (auto &[address, source] : overlay_references) source = "overlay_reference";
    }
    const auto program = overlay_elf ? psprecomp::analyze_overlay(*overlay_elf, elf, memory, load_base)
                                     : psprecomp::analyze_program(elf, memory, load_base, 131072u, overlay_references);
    if (program.executable_ranges.empty()) throw psprecomp::Error("ELF has no executable ranges");
    if (overlay_elf) imports.clear();  // the host corpus owns the import wrappers

    std::filesystem::create_directories(output_dir);
    const std::uint32_t executable_base = overlay_elf
        ? psprecomp::code_ranges(elf, load_base).front().start
        : program.executable_ranges.front().start;
    const std::string ns_open = overlay != nullptr ? "namespace " + overlay->cpp_namespace + " {\n" : "";
    const std::string ns_close = overlay != nullptr ? "} // namespace " + overlay->cpp_namespace + "\n" : "";

    // Emit every discovered guest instruction once. Function seeds remain analysis
    // metadata and dispatcher entries, but overlapping CFGs no longer duplicate C++.
    std::set<std::uint32_t> global_entries = program.covered_entry_labels;
    for (const auto &[seed, source] : program.seeds) {
        (void)source;
        if (program.covered_labels.contains(seed)) global_entries.insert(seed);
    }
    for (const auto pc : program.covered_labels) {
        const std::uint32_t bucket_start = executable_base + ((pc - executable_base) / unit_span_bytes) * unit_span_bytes;
        if (pc == bucket_start || !program.covered_labels.contains(pc - 4u)) global_entries.insert(pc);
    }

    struct Unit {
        std::uint32_t bucket{};
        std::set<std::uint32_t> instructions;
        std::set<std::uint32_t> entries;
    };
    std::map<std::uint32_t, Unit> units_by_bucket;
    for (const auto pc : program.covered_labels) {
        const std::uint32_t bucket = (pc - executable_base) / unit_span_bytes;
        auto &unit = units_by_bucket[bucket];
        unit.bucket = bucket;
        unit.instructions.insert(pc);
    }
    for (const auto entry : global_entries) {
        const std::uint32_t bucket = (entry - executable_base) / unit_span_bytes;
        auto it = units_by_bucket.find(bucket);
        if (it != units_by_bucket.end() && it->second.instructions.contains(entry)) it->second.entries.insert(entry);
    }

    std::vector<Unit> units;
    units.reserve(units_by_bucket.size());
    for (auto &[bucket, unit] : units_by_bucket) {
        (void)bucket;
        if (!unit.entries.empty()) units.push_back(std::move(unit));
    }

    // assign the same compact entry ids used by each unit's dense
    // dispatcher globally before emission. Fixed cross-unit edges can pass the
    // destination id directly and skip the target unit's PC->entry table lookup.
    std::map<std::uint32_t, std::uint16_t> direct_entry_ids;
    for (const auto &unit : units) {
        std::uint16_t id = 1u;
        for (const auto label : unit.entries) direct_entry_ids[label] = id++;
    }

    // External declarations let fixed cross-unit edges become native direct
    // calls. Under MSVC /GL + /LTCG the linker can optimize across these TUs
    // instead of forcing every known edge through a function-pointer branch.
    const auto units_header_path = output_dir / "generated_units.hpp";
    std::ostringstream units_header;
    units_header << "#pragma once\n\n#include <cstdint>\n#include \"psprecomp/guest_memory.hpp\"\n\nnamespace psprecomp {\nclass Runtime;\nstruct AllegrexContext;\n" << ns_open;
    for (const auto &unit : units) {
        units_header << "void " << generated_unit_cpp_name(unit.bucket)
                     << "(Runtime &, AllegrexContext &);\n";
        units_header << "void " << generated_unit_cpp_entry_name(unit.bucket)
                     << "(Runtime &, AllegrexContext &, std::uint16_t, GuestMemory::AotFastView &);\n";
    }
    units_header << ns_close << "} // namespace psprecomp\n";
    (void)write_text_if_changed(units_header_path, units_header.str());

    std::set<std::filesystem::path> expected_cpp;
    std::size_t rewritten_units = 0u;
    std::size_t registered_entries = 0u;
    for (const auto &unit : units) {
        std::ostringstream suffix;
        suffix << std::setfill('0') << std::setw(4) << unit.bucket;
        const std::string suffix_text = suffix.str();
        const auto path = output_dir / ("generated_unit_" + suffix_text + ".cpp");
        expected_cpp.insert(path.filename());

        const GeneratedFunctionInput generated_unit{
            "recomp_unit_" + suffix_text,
            executable_base + unit.bucket * unit_span_bytes,
            unit.instructions,
            unit.entries,
            executable_base,
            unit_span_bytes,
            &direct_entry_ids,
            &import_stubs,
        };

        std::ostringstream out;
        out << "#include \"psprecomp/runtime.hpp\"\n#include \"generated_units.hpp\"\n#include <bit>\n#include <cmath>\n#include <cstdint>\n#include <limits>\n\nnamespace psprecomp {\n" << ns_open;
        // The register-cache lowering passes (per-basic-block GPR/FPR caches and
        // the cross-unit hot-register cache) are deliberately absent.  They kept
        // large numbers of guest registers live in C++ locals and in a second
        // long-lived object alongside AllegrexContext; inside the ~10,000-line
        // single functions emitted here, that drove MSVC's optimizer past the
        // point where it converges.  Affected units never finished compiling and
        // each grew past 2 GB, exhausting system memory during a normal build.
        // This pipeline matches the last configuration observed booting on
        // hardware.
        out << lower_constant_vfpu_accesses(
                lower_aot_memory_accesses(
                    lower_constant_fpr_accesses(
                        lower_constant_gpr_writes(emit_function_source(generated_unit, memory, generated_unit.name)))));
        out << "void register_generated_unit_" << unit.bucket << "(Runtime &runtime) {\n";
        out << "    runtime.register_generated_unit(" << unit.bucket << "u, "
            << psprecomp::hex32(generated_unit.address) << "u, " << unit_span_bytes
            << "u, &" << generated_unit.name << ", &"
            << generated_unit_cpp_entry_name(unit.bucket) << ");\n";
        for (const auto label : unit.entries) {
            // Overlay entries get a namespaced name: anything not starting with
            // "recomp_unit_" is non-chainable, so entering an overlay always passes
            // the runtime's overlay resolver.
            out << "    runtime.register_function(" << psprecomp::hex32(label) << "u, &"
                << generated_unit.name << ", \""
                << (overlay != nullptr ? overlay->cpp_namespace + "::" : std::string{})
                << generated_unit.name << "\");\n";
            ++registered_entries;
        }
        out << "}\n" << ns_close << "} // namespace psprecomp\n";
        rewritten_units += write_text_if_changed(path, out.str()) ? 1u : 0u;
    }

    const auto registry_path = output_dir / "generated_registry.cpp";
    expected_cpp.insert(registry_path.filename());
    std::ostringstream registry;
    registry << "#include \"psprecomp/runtime.hpp\"\n#include <cstdint>\n\nnamespace psprecomp {\n" << ns_open;
    for (const auto &unit : units) registry << "void register_generated_unit_" << unit.bucket << "(Runtime &runtime);\n";
    registry << "\n";
    write_import_wrappers(registry, imports);
    registry << "void register_generated_functions(Runtime &runtime) {\n";
    for (const auto &unit : units) registry << "    register_generated_unit_" << unit.bucket << "(runtime);\n";
    for (std::size_t i = 0; i < imports.size(); ++i) {
        registry << "    runtime.register_function(" << psprecomp::hex32(imports[i].stub_address)
                 << "u, &import_" << i << ", \"" << cpp_escape(imports[i].library)
                 << "::" << psprecomp::hex32(imports[i].nid) << "\");\n";
    }
    registry << "}\n" << ns_close << "} // namespace psprecomp\n";
    const bool registry_rewritten = write_text_if_changed(registry_path, registry.str());

    for (const auto &entry : std::filesystem::directory_iterator(output_dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".cpp" ||
            !entry.path().filename().string().starts_with("generated_")) continue;
        if (!expected_cpp.contains(entry.path().filename())) std::filesystem::remove(entry.path());
    }

    std::ostringstream report;
    report << "{\n"
           << "  \"mode\": \"automatic_global_cfg\",\n"
           << "  \"emission\": \"single_copy_basic_blocks\",\n"
           << "  \"partitioning\": \"fixed_executable_address_span\",\n"
           << "  \"discovered_function_seeds\": " << program.seeds.size() << ",\n"
           << "  \"unique_emitted_instructions\": " << program.covered_labels.size() << ",\n"
           << "  \"unique_registered_block_entries\": " << registered_entries << ",\n"
           << "  \"translation_units\": " << units.size() << ",\n"
           << "  \"rewritten_translation_units\": " << rewritten_units << ",\n"
           << "  \"registry_rewritten\": " << (registry_rewritten ? "true" : "false") << ",\n"
           << "  \"import_wrappers\": " << imports.size() << ",\n"
           << "  \"unit_span_bytes\": " << unit_span_bytes << "\n"
           << "}\n";
    write_text_if_changed(output_dir / "auto_codegen_report.json", report.str());

    std::cout << "Automatic global codegen completed\n"
              << "  function seeds:    " << program.seeds.size() << "\n"
              << "  emitted code PCs:  " << program.covered_labels.size() << "\n"
              << "  registered blocks: " << registered_entries << "\n"
              << "  source units:      " << units.size() << "\n"
              << "  rewritten units:   " << rewritten_units << "\n"
              << "  import wrappers:   " << imports.size() << "\n"
              << "  output:            " << output_dir.string() << "\n";
    return 0;
}

} // namespace

int main(int argc, char **argv) {
    try {
        if (argc >= 4 && std::string_view(argv[2]) == "--auto") {
            const std::uint32_t load_base = argc >= 5
                ? static_cast<std::uint32_t>(std::stoul(argv[4], nullptr, 0))
                : psprecomp::kDefaultPspUserLoadBase;
            const std::uint32_t unit_span = argc >= 6
                ? static_cast<std::uint32_t>(std::stoul(argv[5], nullptr, 0))
                : 0x20000u;
            if (unit_span == 0u || (unit_span & 3u) != 0u) throw psprecomp::Error("unit_span_bytes must be non-zero and 4-byte aligned");
            // Any further arguments are overlay ELFs whose references into this
            // executable seed its analysis.
            std::vector<std::filesystem::path> reference_overlays;
            for (int index = 6; index < argc; ++index) reference_overlays.emplace_back(argv[index]);
            return generate_auto(argv[1], argv[3], load_base, unit_span, nullptr, reference_overlays);
        }
        if (argc >= 6 && std::string_view(argv[2]) == "--overlay") {
            if (argc > 8) {
                std::cerr << "Usage: psp_recomp <host ELF> --overlay <overlay ELF> <generated_dir> <c++ namespace> "
                             "[host_load_base_hex] [unit_span_bytes]\n";
                return 2;
            }
            const std::uint32_t load_base = argc >= 7
                ? static_cast<std::uint32_t>(std::stoul(argv[6], nullptr, 0))
                : psprecomp::kDefaultPspUserLoadBase;
            const std::uint32_t unit_span = argc >= 8
                ? static_cast<std::uint32_t>(std::stoul(argv[7], nullptr, 0))
                : 0x20000u;
            if (unit_span == 0u || (unit_span & 3u) != 0u) throw psprecomp::Error("unit_span_bytes must be non-zero and 4-byte aligned");
            const OverlayOptions options{argv[3], argv[5]};
            return generate_auto(argv[1], argv[4], load_base, unit_span, &options);
        }
        if (argc == 4) return generate_manual(argv[1], argv[2], argv[3]);
        std::cerr << "Usage:\n"
                  << "  psp_recomp <ELF> <functions.csv> <generated_manifest.cpp>\n"
                  << "  psp_recomp <ELF> --auto <generated_dir> [load_base_hex] [unit_span_bytes] [overlay ELF...]\n"
                  << "  psp_recomp <host ELF> --overlay <overlay ELF> <generated_dir> <c++ namespace> "
                     "[host_load_base_hex] [unit_span_bytes]\n";
        return 2;
    } catch (const std::exception &e) {
        std::cerr << "psp_recomp error: " << e.what() << "\n";
        return 1;
    }
}
