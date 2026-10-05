#include "psprecomp/decoder.hpp"

namespace psprecomp {

bool DecodedInstruction::is_control_flow() const noexcept {
    switch (kind) {
    case OpcodeKind::Beq: case OpcodeKind::Bne: case OpcodeKind::Beql: case OpcodeKind::Bnel:
    case OpcodeKind::Blez: case OpcodeKind::Bgtz: case OpcodeKind::Blezl: case OpcodeKind::Bgtzl:
    case OpcodeKind::Bltz: case OpcodeKind::Bgez: case OpcodeKind::Bltzl: case OpcodeKind::Bgezl:
    case OpcodeKind::Bltzal: case OpcodeKind::Bgezal: case OpcodeKind::Bltzall: case OpcodeKind::Bgezall:
    case OpcodeKind::J: case OpcodeKind::Jal:
    case OpcodeKind::Bc1f: case OpcodeKind::Bc1t: case OpcodeKind::Bc1fl: case OpcodeKind::Bc1tl:
    case OpcodeKind::Bvf: case OpcodeKind::Bvt: case OpcodeKind::Bvfl: case OpcodeKind::Bvtl:
    case OpcodeKind::Jr: case OpcodeKind::Jalr: case OpcodeKind::Syscall:
        return true;
    default:
        return false;
    }
}

bool DecodedInstruction::has_delay_slot() const noexcept {
    switch (kind) {
    case OpcodeKind::Beq: case OpcodeKind::Bne: case OpcodeKind::Beql: case OpcodeKind::Bnel:
    case OpcodeKind::Blez: case OpcodeKind::Bgtz: case OpcodeKind::Blezl: case OpcodeKind::Bgtzl:
    case OpcodeKind::Bltz: case OpcodeKind::Bgez: case OpcodeKind::Bltzl: case OpcodeKind::Bgezl:
    case OpcodeKind::Bltzal: case OpcodeKind::Bgezal: case OpcodeKind::Bltzall: case OpcodeKind::Bgezall:
    case OpcodeKind::J: case OpcodeKind::Jal:
    case OpcodeKind::Bc1f: case OpcodeKind::Bc1t: case OpcodeKind::Bc1fl: case OpcodeKind::Bc1tl:
    case OpcodeKind::Bvf: case OpcodeKind::Bvt: case OpcodeKind::Bvfl: case OpcodeKind::Bvtl:
    case OpcodeKind::Jr: case OpcodeKind::Jalr:
        return true;
    default:
        return false;
    }
}

DecodedInstruction decode_allegrex(std::uint32_t word) {
    DecodedInstruction d{};
    d.word = word;
    d.rs = (word >> 21u) & 31u;
    d.rt = (word >> 16u) & 31u;
    d.rd = (word >> 11u) & 31u;
    d.sa = (word >> 6u) & 31u;
    d.immediate = static_cast<std::int16_t>(word & 0xFFFFu);
    d.target = word & 0x03FFFFFFu;
    const std::uint32_t op = word >> 26u;

    if (word == 0u) { d.kind = OpcodeKind::Nop; d.mnemonic = "nop"; return d; }
    switch (op) {
    case 0x00: {
        const std::uint32_t fn = word & 0x3Fu;
        switch (fn) {
        case 0x00: d.kind = OpcodeKind::Sll; d.mnemonic = "sll"; break;
        case 0x02:
            if (d.rs == 1u) { d.kind = OpcodeKind::Rotr; d.mnemonic = "rotr"; }
            else { d.kind = OpcodeKind::Srl; d.mnemonic = "srl"; }
            break;
        case 0x03: d.kind = OpcodeKind::Sra; d.mnemonic = "sra"; break;
        case 0x04: d.kind = OpcodeKind::Sllv; d.mnemonic = "sllv"; break;
        case 0x06:
            if (d.sa == 1u) { d.kind = OpcodeKind::Rotrv; d.mnemonic = "rotrv"; }
            else { d.kind = OpcodeKind::Srlv; d.mnemonic = "srlv"; }
            break;
        case 0x07: d.kind = OpcodeKind::Srav; d.mnemonic = "srav"; break;
        case 0x08: d.kind = OpcodeKind::Jr; d.mnemonic = "jr"; break;
        case 0x09: d.kind = OpcodeKind::Jalr; d.mnemonic = "jalr"; break;
        case 0x0A: d.kind = OpcodeKind::Movz; d.mnemonic = "movz"; break;
        case 0x0B: d.kind = OpcodeKind::Movn; d.mnemonic = "movn"; break;
        case 0x0C: d.kind = OpcodeKind::Syscall; d.mnemonic = "syscall"; break;
        case 0x0D: d.kind = OpcodeKind::Break; d.mnemonic = "break"; break;
        case 0x0F: d.kind = OpcodeKind::Sync; d.mnemonic = "sync"; break;
        case 0x10: d.kind = OpcodeKind::Mfhi; d.mnemonic = "mfhi"; break;
        case 0x11: d.kind = OpcodeKind::Mthi; d.mnemonic = "mthi"; break;
        case 0x12: d.kind = OpcodeKind::Mflo; d.mnemonic = "mflo"; break;
        case 0x13: d.kind = OpcodeKind::Mtlo; d.mnemonic = "mtlo"; break;
        case 0x16: d.kind = OpcodeKind::Clz; d.mnemonic = "clz"; break;
        case 0x17: d.kind = OpcodeKind::Clo; d.mnemonic = "clo"; break;
        case 0x18: d.kind = OpcodeKind::Mult; d.mnemonic = "mult"; break;
        case 0x19: d.kind = OpcodeKind::Multu; d.mnemonic = "multu"; break;
        case 0x1A: d.kind = OpcodeKind::Div; d.mnemonic = "div"; break;
        case 0x1B: d.kind = OpcodeKind::Divu; d.mnemonic = "divu"; break;
        // Allegrex multiply-accumulate into HI:LO.
        case 0x1C: d.kind = OpcodeKind::Madd; d.mnemonic = "madd"; break;
        case 0x1D: d.kind = OpcodeKind::Maddu; d.mnemonic = "maddu"; break;
        case 0x2E: d.kind = OpcodeKind::Msub; d.mnemonic = "msub"; break;
        case 0x2F: d.kind = OpcodeKind::Msubu; d.mnemonic = "msubu"; break;
        case 0x20: d.kind = OpcodeKind::Add; d.mnemonic = "add"; break;
        case 0x21: d.kind = OpcodeKind::Addu; d.mnemonic = "addu"; break;
        case 0x22: d.kind = OpcodeKind::Sub; d.mnemonic = "sub"; break;
        case 0x23: d.kind = OpcodeKind::Subu; d.mnemonic = "subu"; break;
        case 0x24: d.kind = OpcodeKind::And; d.mnemonic = "and"; break;
        case 0x25: d.kind = OpcodeKind::Or; d.mnemonic = "or"; break;
        case 0x26: d.kind = OpcodeKind::Xor; d.mnemonic = "xor"; break;
        case 0x27: d.kind = OpcodeKind::Nor; d.mnemonic = "nor"; break;
        case 0x2A: d.kind = OpcodeKind::Slt; d.mnemonic = "slt"; break;
        case 0x2B: d.kind = OpcodeKind::Sltu; d.mnemonic = "sltu"; break;
        case 0x2C: d.kind = OpcodeKind::Max; d.mnemonic = "max"; break;
        case 0x2D: d.kind = OpcodeKind::Min; d.mnemonic = "min"; break;
        default: d.kind = OpcodeKind::Unsupported; d.mnemonic = "special?"; break;
        }
        break;
    }
    case 0x01:
        switch (d.rt) {
        case 0x00: d.kind = OpcodeKind::Bltz; d.mnemonic = "bltz"; break;
        case 0x01: d.kind = OpcodeKind::Bgez; d.mnemonic = "bgez"; break;
        case 0x02: d.kind = OpcodeKind::Bltzl; d.mnemonic = "bltzl"; break;
        case 0x03: d.kind = OpcodeKind::Bgezl; d.mnemonic = "bgezl"; break;
        case 0x10: d.kind = OpcodeKind::Bltzal; d.mnemonic = "bltzal"; break;
        case 0x11: d.kind = OpcodeKind::Bgezal; d.mnemonic = "bgezal"; break;
        case 0x12: d.kind = OpcodeKind::Bltzall; d.mnemonic = "bltzall"; break;
        case 0x13: d.kind = OpcodeKind::Bgezall; d.mnemonic = "bgezall"; break;
        default: d.kind = OpcodeKind::Unsupported; d.mnemonic = "regimm?"; break;
        }
        break;
    case 0x02: d.kind = OpcodeKind::J; d.mnemonic = "j"; break;
    case 0x03: d.kind = OpcodeKind::Jal; d.mnemonic = "jal"; break;
    case 0x04: d.kind = OpcodeKind::Beq; d.mnemonic = "beq"; break;
    case 0x05: d.kind = OpcodeKind::Bne; d.mnemonic = "bne"; break;
    case 0x06: d.kind = OpcodeKind::Blez; d.mnemonic = "blez"; break;
    case 0x07: d.kind = OpcodeKind::Bgtz; d.mnemonic = "bgtz"; break;
    case 0x14: d.kind = OpcodeKind::Beql; d.mnemonic = "beql"; break;
    case 0x15: d.kind = OpcodeKind::Bnel; d.mnemonic = "bnel"; break;
    case 0x16: d.kind = OpcodeKind::Blezl; d.mnemonic = "blezl"; break;
    case 0x17: d.kind = OpcodeKind::Bgtzl; d.mnemonic = "bgtzl"; break;
    case 0x08: d.kind = OpcodeKind::Addi; d.mnemonic = "addi"; break;
    case 0x09: d.kind = OpcodeKind::Addiu; d.mnemonic = "addiu"; break;
    case 0x0A: d.kind = OpcodeKind::Slti; d.mnemonic = "slti"; break;
    case 0x0B: d.kind = OpcodeKind::Sltiu; d.mnemonic = "sltiu"; break;
    case 0x0C: d.kind = OpcodeKind::Andi; d.mnemonic = "andi"; break;
    case 0x0D: d.kind = OpcodeKind::Ori; d.mnemonic = "ori"; break;
    case 0x0E: d.kind = OpcodeKind::Xori; d.mnemonic = "xori"; break;
    case 0x0F: d.kind = OpcodeKind::Lui; d.mnemonic = "lui"; break;
    case 0x11: {
        if (d.rs == 0x00u) { d.kind = OpcodeKind::Mfc1; d.mnemonic = "mfc1"; }
        else if (d.rs == 0x04u) { d.kind = OpcodeKind::Mtc1; d.mnemonic = "mtc1"; }
        else if (d.rs == 0x02u) { d.kind = OpcodeKind::Cfc1; d.mnemonic = "cfc1"; }
        else if (d.rs == 0x06u) { d.kind = OpcodeKind::Ctc1; d.mnemonic = "ctc1"; }
        else if (d.rs == 0x08u) {
            switch (d.rt & 3u) {
            case 0u: d.kind = OpcodeKind::Bc1f; d.mnemonic = "bc1f"; break;
            case 1u: d.kind = OpcodeKind::Bc1t; d.mnemonic = "bc1t"; break;
            case 2u: d.kind = OpcodeKind::Bc1fl; d.mnemonic = "bc1fl"; break;
            case 3u: d.kind = OpcodeKind::Bc1tl; d.mnemonic = "bc1tl"; break;
            }
        } else if (d.rs == 0x10u) {
            const std::uint32_t fn = word & 0x3Fu;
            switch (fn) {
            case 0x00u: d.kind = OpcodeKind::AddS; d.mnemonic = "add.s"; break;
            case 0x01u: d.kind = OpcodeKind::SubS; d.mnemonic = "sub.s"; break;
            case 0x02u: d.kind = OpcodeKind::MulS; d.mnemonic = "mul.s"; break;
            case 0x03u: d.kind = OpcodeKind::DivS; d.mnemonic = "div.s"; break;
            case 0x04u: d.kind = OpcodeKind::SqrtS; d.mnemonic = "sqrt.s"; break;
            case 0x05u: d.kind = OpcodeKind::AbsS; d.mnemonic = "abs.s"; break;
            case 0x06u: d.kind = OpcodeKind::MovS; d.mnemonic = "mov.s"; break;
            case 0x07u: d.kind = OpcodeKind::NegS; d.mnemonic = "neg.s"; break;
            case 0x0Cu: d.kind = OpcodeKind::RoundWS; d.mnemonic = "round.w.s"; break;
            case 0x0Du: d.kind = OpcodeKind::TruncWS; d.mnemonic = "trunc.w.s"; break;
            case 0x0Eu: d.kind = OpcodeKind::CeilWS; d.mnemonic = "ceil.w.s"; break;
            case 0x0Fu: d.kind = OpcodeKind::FloorWS; d.mnemonic = "floor.w.s"; break;
            case 0x24u: d.kind = OpcodeKind::CvtWS; d.mnemonic = "cvt.w.s"; break;
            default:
                if ((fn & 0x30u) == 0x30u) { d.kind = OpcodeKind::FpuCompare; d.mnemonic = "c.cond.s"; }
                else { d.kind = OpcodeKind::Unsupported; d.mnemonic = "cop1.s?"; }
                break;
            }
        } else if (d.rs == 0x14u && (word & 0x3Fu) == 0x20u) {
            d.kind = OpcodeKind::CvtSW;
            d.mnemonic = "cvt.s.w";
        } else { d.kind = OpcodeKind::Unsupported; d.mnemonic = "cop1?"; }
        break;
    }
    case 0x1F: {
        const std::uint32_t fn = word & 0x3Fu;
        if (fn == 0x00u) {
            d.kind = OpcodeKind::Ext;
            d.mnemonic = "ext";
        } else if (fn == 0x04u) {
            d.kind = OpcodeKind::Ins;
            d.mnemonic = "ins";
        } else if (fn == 0x18u || fn == 0x20u) {
            // PSP Allegrex SPECIAL3 unary operations are selected by sa.
            switch (d.sa) {
            case 0x02u: d.kind = OpcodeKind::Wsbh; d.mnemonic = "wsbh"; break;
            case 0x03u: d.kind = OpcodeKind::Wsbw; d.mnemonic = "wsbw"; break;
            case 0x10u: d.kind = OpcodeKind::Seb; d.mnemonic = "seb"; break;
            case 0x14u: d.kind = OpcodeKind::Bitrev; d.mnemonic = "bitrev"; break;
            case 0x18u: d.kind = OpcodeKind::Seh; d.mnemonic = "seh"; break;
            default: d.kind = OpcodeKind::Unsupported; d.mnemonic = "allegrex0?"; break;
            }
        } else {
            d.kind = OpcodeKind::Unsupported;
            d.mnemonic = "special3?";
        }
        break;
    }
    case 0x12:
        if (d.rs == 0x03u) { d.kind = OpcodeKind::Mfv; d.mnemonic = "mfv"; }
        else if (d.rs == 0x07u) { d.kind = OpcodeKind::Mtv; d.mnemonic = "mtv"; }
        else if (d.rs == 0x08u) {
            switch (d.rt & 3u) {
            case 0u: d.kind = OpcodeKind::Bvf; d.mnemonic = "bvf"; break;
            case 1u: d.kind = OpcodeKind::Bvt; d.mnemonic = "bvt"; break;
            case 2u: d.kind = OpcodeKind::Bvfl; d.mnemonic = "bvfl"; break;
            case 3u: d.kind = OpcodeKind::Bvtl; d.mnemonic = "bvtl"; break;
            }
        } else { d.kind = OpcodeKind::Vfpu; d.mnemonic = "cop2/vfpu"; }
        break;
    case 0x20: d.kind = OpcodeKind::Lb; d.mnemonic = "lb"; break;
    case 0x21: d.kind = OpcodeKind::Lh; d.mnemonic = "lh"; break;
    case 0x22: d.kind = OpcodeKind::Lwl; d.mnemonic = "lwl"; break;
    case 0x23: d.kind = OpcodeKind::Lw; d.mnemonic = "lw"; break;
    case 0x24: d.kind = OpcodeKind::Lbu; d.mnemonic = "lbu"; break;
    case 0x25: d.kind = OpcodeKind::Lhu; d.mnemonic = "lhu"; break;
    case 0x26: d.kind = OpcodeKind::Lwr; d.mnemonic = "lwr"; break;
    case 0x28: d.kind = OpcodeKind::Sb; d.mnemonic = "sb"; break;
    case 0x29: d.kind = OpcodeKind::Sh; d.mnemonic = "sh"; break;
    case 0x2A: d.kind = OpcodeKind::Swl; d.mnemonic = "swl"; break;
    case 0x2B: d.kind = OpcodeKind::Sw; d.mnemonic = "sw"; break;
    case 0x2E: d.kind = OpcodeKind::Swr; d.mnemonic = "swr"; break;
    case 0x31: d.kind = OpcodeKind::Lwc1; d.mnemonic = "lwc1"; break;
    case 0x32: d.kind = OpcodeKind::Lvs; d.mnemonic = "lv.s"; break;
    case 0x39: d.kind = OpcodeKind::Swc1; d.mnemonic = "swc1"; break;
    case 0x3A: d.kind = OpcodeKind::Svs; d.mnemonic = "sv.s"; break;
    // PSP VFPU encodings occupy several Allegrex-specific opcode ranges.
    case 0x18: {
        const std::uint32_t operation = (word >> 23u) & 7u;
        if (operation == 0u || operation == 1u || operation == 7u) {
            d.kind = OpcodeKind::VfpuVec3;
            d.mnemonic = operation == 0u ? "vadd" : (operation == 1u ? "vsub" : "vdiv");
        } else {
            d.kind = OpcodeKind::Vfpu;
            d.mnemonic = "vfpu0";
        }
        break;
    }
    case 0x19: {
        const std::uint32_t operation = (word >> 23u) & 7u;
        if (operation == 0u) {
            d.kind = OpcodeKind::VfpuVec3;
            d.mnemonic = "vmul";
        } else if (operation == 1u) {
            d.kind = OpcodeKind::Vdot;
            d.mnemonic = "vdot";
        } else if (operation == 2u) {
            d.kind = OpcodeKind::Vscl;
            d.mnemonic = "vscl";
        } else if (operation == 4u) {
            d.kind = OpcodeKind::Vhdp;
            d.mnemonic = "vhdp";
        } else {
            d.kind = OpcodeKind::Vfpu;
            d.mnemonic = "vfpu1";
        }
        break;
    }
    case 0x1B: {
        const std::uint32_t operation = (word >> 23u) & 7u;
        if (operation == 0u) {
            d.kind = OpcodeKind::Vcmp;
            d.mnemonic = "vcmp";
        } else if (operation == 2u || operation == 3u) {
            d.kind = OpcodeKind::Vminmax;
            d.mnemonic = operation == 2u ? "vmin" : "vmax";
        } else if (operation >= 5u) {
            d.kind = OpcodeKind::VfpuCompare3;
            d.mnemonic = operation == 5u ? "vscmp" : (operation == 6u ? "vsge" : "vslt");
        } else {
            d.kind = OpcodeKind::Vfpu;
            d.mnemonic = "vfpu3";
        }
        break;
    }
    case 0x35:
    case 0x3D:
        d.kind = OpcodeKind::Vfpu;
        d.mnemonic = "vfpu";
        break;
    case 0x3F:
        d.kind = OpcodeKind::Vflush;
        d.mnemonic = "vflush";
        break;
    case 0x37: {
        const std::uint32_t type = (word >> 23u) & 7u;
        if (type <= 5u) {
            d.kind = OpcodeKind::Vpfx;
            d.mnemonic = type <= 1u ? "vpfxs" : (type <= 3u ? "vpfxt" : "vpfxd");
        } else if (type == 6u) {
            d.kind = OpcodeKind::Viim;
            d.mnemonic = "viim.s";
        } else {
            d.kind = OpcodeKind::Vfim;
            d.mnemonic = "vfim.s";
        }
        break;
    }
    case 0x2F:
        d.kind = OpcodeKind::Cache;
        d.mnemonic = "cache";
        break;
    case 0x34: {
        const std::uint32_t group = (word >> 21u) & 31u;
        const std::uint32_t operation = (word >> 16u) & 31u;
        // VFPU4Jump sub-op 3 is VCST: fill a vector with one of the
        // Allegrex VFPU hardware constants.
        if (group == 3u) {
            d.kind = OpcodeKind::Vcst;
            d.mnemonic = "vcst";
        } else if (group == 1u && operation == 18u) {
            d.kind = OpcodeKind::Vf2h;
            d.mnemonic = "vf2h";
        } else if (group == 1u && operation == 19u) {
            d.kind = OpcodeKind::Vh2f;
            d.mnemonic = "vh2f";
        } else if (group == 1u && operation >= 24u && operation <= 27u) {
            d.kind = OpcodeKind::Vx2i;
            static constexpr const char *names[4]{"vuc2i", "vc2i", "vus2i", "vs2i"};
            d.mnemonic = names[operation - 24u];
        } else if (group == 1u && operation >= 28u && operation <= 31u) {
            d.kind = OpcodeKind::Vi2x;
            static constexpr const char *names[4]{"vi2uc", "vi2c", "vi2us", "vi2s"};
            d.mnemonic = names[operation - 28u];
        } else if (group >= 16u && group <= 19u) {
            d.kind = OpcodeKind::Vf2i;
            static constexpr const char *names[4]{"vf2in", "vf2iz", "vf2iu", "vf2id"};
            d.mnemonic = names[group - 16u];
        } else if (group == 20u) {
            d.kind = OpcodeKind::Vi2f;
            d.mnemonic = "vi2f";
        } else if (group == 21u) {
            d.kind = OpcodeKind::Vcmov;
            d.mnemonic = ((word >> 19u) & 1u) == 0u ? "vcmovt" : "vcmovf";
        } else if (group == 2u && operation == 4u) {
            d.kind = OpcodeKind::Vocp;
            d.mnemonic = "vocp";
        } else if (group == 2u && (operation == 6u || operation == 7u)) {
            d.kind = OpcodeKind::VfpuHorizontal;
            d.mnemonic = operation == 6u ? "vfad" : "vavg";
        } else if (group == 0u) {
            switch (operation) {
            case 0u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vmov"; break;
            case 1u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vabs"; break;
            case 2u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vneg"; break;
            case 3u: d.kind = OpcodeKind::Vidt; d.mnemonic = "vidt"; break;
            case 4u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vsat0"; break;
            case 5u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vsat1"; break;
            case 6u: d.kind = OpcodeKind::VfpuVectorInit; d.mnemonic = "vzero"; break;
            case 7u: d.kind = OpcodeKind::VfpuVectorInit; d.mnemonic = "vone"; break;
            case 16u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vrcp"; break;
            case 17u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vrsq"; break;
            case 18u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vsin"; break;
            case 19u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vcos"; break;
            case 20u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vexp2"; break;
            case 21u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vlog2"; break;
            case 22u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vsqrt"; break;
            case 23u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vasin"; break;
            case 24u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vnrcp"; break;
            case 26u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vnsin"; break;
            case 28u: d.kind = OpcodeKind::VfpuUnary; d.mnemonic = "vrexp2"; break;
            default: d.kind = OpcodeKind::Vfpu; d.mnemonic = "vfpu4"; break;
            }
        } else {
            d.kind = OpcodeKind::Vfpu;
            d.mnemonic = "vfpu4";
        }
        break;
    }
    case 0x36:
        d.kind = OpcodeKind::Lvq;
        d.mnemonic = "lv.q";
        break;
    case 0x3C: {
        const std::uint32_t group = (word >> 21u) & 31u;
        if (group <= 3u) {
            d.kind = OpcodeKind::Vmmul;
            d.mnemonic = "vmmul";
        } else if (group >= 4u && group <= 15u) {
            d.kind = OpcodeKind::Vtfm;
            d.mnemonic = "vtfm";
        } else if (group >= 16u && group <= 19u) {
            d.kind = OpcodeKind::Vmscl;
            d.mnemonic = "vmscl";
        } else if (group >= 20u && group <= 23u) {
            const std::uint32_t size_code = ((word >> 7u) & 1u) | (((word >> 15u) & 1u) << 1u);
            d.kind = OpcodeKind::VcrossQuat;
            d.mnemonic = size_code == 2u ? "vcrsp" : (size_code == 3u ? "vqmul" : "vcrsp/vqmul");
        // Matrix-set encoding: VFPU6 subop 28.
        } else if (group == 28u) {
            const std::uint32_t matrix_operation = (word >> 16u) & 15u;
            if (matrix_operation == 0u) {
                d.kind = OpcodeKind::Vmmov;
                d.mnemonic = "vmmov";
            } else if (matrix_operation == 3u) {
                d.kind = OpcodeKind::VmidT;
                d.mnemonic = "vmidt";
            } else if (matrix_operation == 6u || matrix_operation == 7u) {
                d.kind = OpcodeKind::VfpuMatrixInit;
                d.mnemonic = matrix_operation == 6u ? "vmzero" : "vmone";
            } else {
                d.kind = OpcodeKind::Vfpu;
                d.mnemonic = "vfpu-matrix1";
            }
        } else if (group == 29u) {
            d.kind = OpcodeKind::Vrot;
            d.mnemonic = "vrot";
        } else {
            d.kind = OpcodeKind::Vfpu;
            d.mnemonic = "vfpu6";
        }
        break;
    }
    case 0x3E:
        d.kind = OpcodeKind::Svq;
        d.mnemonic = "sv.q";
        break;
    default: d.kind = OpcodeKind::Unsupported; d.mnemonic = "unknown"; break;
    }
    return d;
}

} // namespace psprecomp
