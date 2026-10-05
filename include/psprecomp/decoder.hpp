#pragma once

#include <cstdint>
#include <string>

namespace psprecomp {

enum class OpcodeKind {
    Nop,
    Addi, Addiu, Slti, Sltiu, Andi, Ori, Xori, Lui,
    Add, Addu, Sub, Subu, And, Or, Xor, Nor, Slt, Sltu, Max, Min, Movz, Movn,
    Sll, Srl, Sra, Rotr, Sllv, Srlv, Srav, Rotrv, Clz, Clo, Ext, Ins, Seb, Seh, Bitrev, Wsbh, Wsbw, Sync,
    Lw, Lwl, Lwr, Sw, Swl, Swr, Lh, Lhu, Sh, Lb, Lbu, Sb, Lwc1, Swc1, Cache,
    Beq, Bne, Beql, Bnel, Blez, Bgtz, Blezl, Bgtzl, Bltz, Bgez, Bltzl, Bgezl, Bltzal, Bgezal, Bltzall, Bgezall, J, Jal, Jr, Jalr,
    Mfhi, Mflo, Mthi, Mtlo,
    Mult, Multu, Div, Divu, Madd, Maddu, Msub, Msubu, Break,
    Mfc1, Mtc1, Cfc1, Ctc1,
    AddS, SubS, MulS, DivS, SqrtS, AbsS, MovS, NegS,
    RoundWS, TruncWS, CeilWS, FloorWS, CvtWS, CvtSW, FpuCompare,
    Bc1f, Bc1t, Bc1fl, Bc1tl, Bvf, Bvt, Bvfl, Bvtl,
    Mfv, Mtv, Vpfx, Viim, Vfim, Vh2f, Vf2h, Vf2i, Vi2f, Vx2i, Vi2x, VmidT, Vmmov, VfpuMatrixInit, Vidt, Vcst, Vocp, VfpuHorizontal, Vrot, Vtfm, VfpuVectorInit, VfpuVec3, Vscl, Vdot, Vhdp, Vcmp, Vminmax, VfpuCompare3, Vcmov, VfpuUnary, Vmmul, Vmscl, VcrossQuat, Lvs, Svs, Lvq, Svq,
    Syscall,
    Vflush,
    Vfpu,
    Unsupported,
};

struct DecodedInstruction {
    OpcodeKind kind{OpcodeKind::Unsupported};
    std::uint32_t word{};
    std::uint32_t rs{};
    std::uint32_t rt{};
    std::uint32_t rd{};
    std::uint32_t sa{};
    std::int16_t immediate{};
    std::uint32_t target{};
    std::string mnemonic;

    [[nodiscard]] bool is_control_flow() const noexcept;
    [[nodiscard]] bool has_delay_slot() const noexcept;
};

[[nodiscard]] DecodedInstruction decode_allegrex(std::uint32_t word);

} // namespace psprecomp
