#include "psprecomp/runtime.hpp"
#include "generated_units.hpp"
#include "vcs_resident_regions.hpp"
#include "vcs_audio_commands.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>

namespace psprecomp {
// PSPRECOMP_V811_COMPACT_DISPATCH: 64-slot occupancy masks + per-group base id.
static constexpr std::uint64_t kEntryMasks_recomp_unit_0169[64] = {
    0x0500A035500A9405ull, 0x0020400188400044ull, 0x0014005020080040ull, 0x0210000003420400ull,
    0x0010000004030000ull, 0x0000000000000100ull, 0x0000000000000000ull, 0x0000000000000001ull,
    0x0001000000000000ull, 0x4500000081000000ull, 0x0000000000000000ull, 0x2040908940000020ull,
    0x0000000430148048ull, 0x1044220884200000ull, 0x0010000000000001ull, 0x0000000000000000ull,
    0x0810000100200000ull, 0x000000000A000000ull, 0x3084542108400000ull, 0x00000000C06108A8ull,
    0x0000000000002000ull, 0x1115000000000000ull, 0x0000000000000401ull, 0x0C21150842100000ull,
    0x000000003018422Aull, 0x0000000000000400ull, 0x2222A00000000000ull, 0x0800000000000080ull,
    0x0000004411088221ull, 0x0100000020000000ull, 0x0000000000000002ull, 0x0000000000401400ull,
    0x22A1084200000000ull, 0x0A108A8308454184ull, 0x0001000000000000ull, 0x0000000000080400ull,
    0x0000040280000000ull, 0x4210000000000000ull, 0x5418422A0C211508ull, 0x0000000000005084ull,
    0x0000004020000008ull, 0x2014000000000000ull, 0x0000000000000000ull, 0x06108A8421080000ull,
    0x00005084540C2115ull, 0x0800000800000000ull, 0x0044000000008801ull, 0x8000000022000000ull,
    0x0000000000000008ull, 0x8400028000000000ull, 0xAA52252040801044ull, 0x4A0D25A4004A8A91ull,
    0x8044114400000143ull, 0x904040E204448AA8ull, 0x4880225252012525ull, 0x2C0C00090A101220ull,
    0x0954AAA020002442ull, 0x5690010840020408ull, 0x102142A14252D26Aull, 0x2088110249400920ull,
    0x25454414CA040121ull, 0x292A40222148D212ull, 0x20100011210124A9ull, 0x1146409452928869ull,
};
static constexpr std::uint16_t kEntryBases_recomp_unit_0169[64] = {
    1u, 18u, 26u, 33u, 40u, 44u, 45u, 45u, 46u, 47u, 52u, 52u, 61u, 69u, 78u, 80u,
    80u, 84u, 86u, 97u, 106u, 107u, 112u, 114u, 125u, 134u, 135u, 141u, 143u, 152u, 154u, 155u,
    158u, 166u, 183u, 184u, 186u, 189u, 192u, 210u, 214u, 217u, 220u, 220u, 231u, 245u, 247u, 252u,
    255u, 256u, 260u, 276u, 297u, 308u, 325u, 343u, 356u, 372u, 384u, 405u, 418u, 436u, 455u, 468u,
};

// PSPRECOMP_V813_HOT_DENSE_DISPATCH: one indexed load on the measured hot units;
// masks remain for exact registration/correctness proof.
alignas(64) static constexpr std::uint16_t kEntryIdsV813_recomp_unit_0169[4093] = {
    1u, 0u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3u, 0u, 4u, 0u, 0u, 5u, 0u, 6u, 0u, 7u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8u, 0u, 9u, 0u,
    10u, 0u, 11u, 0u, 12u, 13u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 14u, 0u, 15u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 16u, 0u, 17u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 18u, 0u, 0u, 0u, 19u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 20u, 0u, 0u, 0u, 0u, 21u, 0u, 0u, 0u, 22u,
    23u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 24u, 0u, 0u, 0u, 0u, 0u, 0u, 25u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 26u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 27u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 28u, 0u, 0u,
    0u, 0u, 0u, 0u, 29u, 0u, 30u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 31u, 0u, 32u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 33u, 0u, 0u, 0u, 0u, 0u, 0u, 34u, 0u, 0u, 0u, 0u, 35u, 0u, 36u, 37u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 38u, 0u, 0u, 0u, 0u, 39u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 40u, 41u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 42u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 43u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 44u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    45u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 46u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 47u, 0u, 0u, 0u, 0u, 0u, 0u, 48u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 49u, 0u, 50u, 0u, 0u, 0u, 51u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 52u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 53u, 0u,
    54u, 0u, 0u, 55u, 0u, 0u, 0u, 56u, 0u, 0u, 0u, 0u, 57u, 0u, 0u, 58u, 0u, 0u, 0u, 0u, 0u, 0u, 59u, 0u, 0u, 0u, 0u, 0u, 0u, 60u, 0u, 0u,
    0u, 0u, 0u, 61u, 0u, 0u, 62u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 63u, 0u, 0u, 64u, 0u, 65u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 66u, 67u, 0u, 0u,
    0u, 0u, 68u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 69u, 0u, 0u, 0u, 0u, 70u, 0u, 0u, 0u, 0u, 71u,
    0u, 0u, 0u, 72u, 0u, 0u, 0u, 0u, 0u, 73u, 0u, 0u, 0u, 74u, 0u, 0u, 0u, 0u, 75u, 0u, 0u, 0u, 76u, 0u, 0u, 0u, 0u, 0u, 77u, 0u, 0u, 0u,
    78u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 79u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 80u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    81u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 82u, 0u, 0u, 0u, 0u, 0u, 0u, 83u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 84u, 0u, 85u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 86u, 0u, 0u, 0u, 0u, 87u, 0u, 0u, 0u, 0u,
    88u, 0u, 0u, 0u, 0u, 89u, 0u, 0u, 0u, 0u, 90u, 0u, 91u, 0u, 92u, 0u, 0u, 0u, 93u, 0u, 0u, 0u, 0u, 94u, 0u, 0u, 0u, 0u, 95u, 96u, 0u, 0u,
    0u, 0u, 0u, 97u, 0u, 98u, 0u, 99u, 0u, 0u, 0u, 100u, 0u, 0u, 0u, 0u, 101u, 0u, 0u, 0u, 0u, 102u, 103u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 104u, 105u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 106u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 107u, 0u, 108u, 0u, 109u, 0u, 0u, 0u, 110u, 0u, 0u, 0u, 111u, 0u, 0u, 0u,
    112u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 113u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 114u, 0u, 0u, 0u, 0u, 115u, 0u, 0u, 0u, 0u, 116u, 0u,
    0u, 0u, 0u, 117u, 0u, 0u, 0u, 0u, 118u, 0u, 119u, 0u, 120u, 0u, 0u, 0u, 121u, 0u, 0u, 0u, 0u, 122u, 0u, 0u, 0u, 0u, 123u, 124u, 0u, 0u, 0u, 0u,
    0u, 125u, 0u, 126u, 0u, 127u, 0u, 0u, 0u, 128u, 0u, 0u, 0u, 0u, 129u, 0u, 0u, 0u, 0u, 130u, 131u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 132u, 133u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 134u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 135u, 0u, 136u, 0u, 137u, 0u, 0u, 0u, 138u, 0u, 0u, 0u, 139u, 0u, 0u, 0u, 140u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 141u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 142u, 0u, 0u, 0u, 0u,
    143u, 0u, 0u, 0u, 0u, 144u, 0u, 0u, 0u, 145u, 0u, 0u, 0u, 0u, 0u, 146u, 0u, 0u, 0u, 147u, 0u, 0u, 0u, 0u, 148u, 0u, 0u, 0u, 149u, 0u, 0u, 0u,
    0u, 0u, 150u, 0u, 0u, 0u, 151u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 152u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 153u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 154u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 155u, 0u, 156u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 157u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 158u, 0u, 0u, 0u, 0u, 159u, 0u, 0u, 0u, 0u, 160u, 0u, 0u, 0u, 0u, 161u, 0u, 0u, 0u, 0u, 162u, 0u, 163u, 0u, 164u, 0u, 0u, 0u, 165u, 0u, 0u,
    0u, 0u, 166u, 0u, 0u, 0u, 0u, 167u, 168u, 0u, 0u, 0u, 0u, 0u, 169u, 0u, 170u, 0u, 171u, 0u, 0u, 0u, 172u, 0u, 0u, 0u, 0u, 173u, 0u, 0u, 0u, 0u,
    174u, 175u, 0u, 0u, 0u, 0u, 0u, 176u, 0u, 177u, 0u, 178u, 0u, 0u, 0u, 179u, 0u, 0u, 0u, 0u, 180u, 0u, 0u, 0u, 0u, 181u, 0u, 182u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 183u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 184u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 185u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 186u,
    0u, 187u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 188u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 189u, 0u, 0u, 0u, 0u, 190u, 0u, 0u, 0u, 0u, 191u, 0u,
    0u, 0u, 0u, 192u, 0u, 0u, 0u, 0u, 193u, 0u, 194u, 0u, 195u, 0u, 0u, 0u, 196u, 0u, 0u, 0u, 0u, 197u, 0u, 0u, 0u, 0u, 198u, 199u, 0u, 0u, 0u, 0u,
    0u, 200u, 0u, 201u, 0u, 202u, 0u, 0u, 0u, 203u, 0u, 0u, 0u, 0u, 204u, 0u, 0u, 0u, 0u, 205u, 206u, 0u, 0u, 0u, 0u, 0u, 207u, 0u, 208u, 0u, 209u, 0u,
    0u, 0u, 210u, 0u, 0u, 0u, 0u, 211u, 0u, 0u, 0u, 0u, 212u, 0u, 213u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 214u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 215u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 216u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 217u, 0u, 218u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 219u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 220u, 0u, 0u, 0u, 0u, 221u, 0u, 0u, 0u, 0u, 222u, 0u, 0u,
    0u, 0u, 223u, 0u, 0u, 0u, 0u, 224u, 0u, 225u, 0u, 226u, 0u, 0u, 0u, 227u, 0u, 0u, 0u, 0u, 228u, 0u, 0u, 0u, 0u, 229u, 230u, 0u, 0u, 0u, 0u, 0u,
    231u, 0u, 232u, 0u, 233u, 0u, 0u, 0u, 234u, 0u, 0u, 0u, 0u, 235u, 0u, 0u, 0u, 0u, 236u, 237u, 0u, 0u, 0u, 0u, 0u, 0u, 238u, 0u, 239u, 0u, 240u, 0u,
    0u, 0u, 241u, 0u, 0u, 0u, 0u, 242u, 0u, 0u, 0u, 0u, 243u, 0u, 244u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 245u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 246u, 0u, 0u, 0u, 0u,
    247u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 248u, 0u, 0u, 0u, 249u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 250u, 0u, 0u, 0u, 251u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 252u, 0u, 0u, 0u, 253u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 254u,
    0u, 0u, 0u, 255u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 256u, 0u, 257u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 258u, 0u, 0u, 0u, 0u, 259u,
    0u, 0u, 260u, 0u, 0u, 0u, 261u, 0u, 0u, 0u, 0u, 0u, 262u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 263u, 0u, 0u, 0u, 0u, 0u, 0u, 264u, 0u,
    0u, 0u, 0u, 0u, 0u, 265u, 0u, 0u, 266u, 0u, 267u, 0u, 0u, 268u, 0u, 0u, 0u, 269u, 0u, 0u, 270u, 0u, 271u, 0u, 0u, 272u, 0u, 273u, 0u, 274u, 0u, 275u,
    276u, 0u, 0u, 0u, 277u, 0u, 0u, 278u, 0u, 279u, 0u, 280u, 0u, 0u, 0u, 281u, 0u, 282u, 0u, 283u, 0u, 0u, 284u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 285u, 0u, 0u, 286u, 0u, 287u, 288u, 0u, 289u, 0u, 0u, 290u, 0u, 0u, 291u, 0u, 292u, 293u, 0u, 0u, 0u, 0u, 0u, 294u, 0u, 295u, 0u, 0u, 296u, 0u,
    297u, 298u, 0u, 0u, 0u, 0u, 299u, 0u, 300u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 301u, 0u, 0u, 0u, 302u, 0u, 303u, 0u, 0u, 0u, 304u, 0u, 0u, 0u, 0u, 0u, 305u, 0u, 0u, 0u, 306u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 307u,
    0u, 0u, 0u, 308u, 0u, 309u, 0u, 310u, 0u, 311u, 0u, 312u, 0u, 0u, 0u, 313u, 0u, 0u, 314u, 0u, 0u, 0u, 315u, 0u, 0u, 0u, 316u, 0u, 0u, 0u, 0u, 0u,
    0u, 317u, 0u, 0u, 0u, 318u, 319u, 320u, 0u, 0u, 0u, 0u, 0u, 0u, 321u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 322u, 0u, 0u, 0u, 0u, 0u, 323u, 0u, 0u, 324u,
    325u, 0u, 326u, 0u, 0u, 327u, 0u, 0u, 328u, 0u, 329u, 0u, 0u, 330u, 0u, 0u, 331u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 332u, 0u, 0u, 333u, 0u, 334u, 0u,
    0u, 335u, 0u, 0u, 336u, 0u, 337u, 0u, 0u, 338u, 0u, 0u, 0u, 339u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 340u, 0u, 0u, 0u, 341u, 0u, 0u, 342u, 0u,
    0u, 0u, 0u, 0u, 0u, 343u, 0u, 0u, 0u, 344u, 0u, 0u, 345u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 346u, 0u, 0u, 0u, 0u, 347u, 0u, 348u, 0u, 0u, 0u, 0u,
    349u, 0u, 0u, 350u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 351u, 352u, 0u, 0u, 0u, 0u, 0u, 0u, 353u, 354u, 0u, 355u, 0u, 0u,
    0u, 356u, 0u, 0u, 0u, 0u, 357u, 0u, 0u, 0u, 358u, 0u, 0u, 359u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 360u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 361u, 0u, 362u, 0u, 363u, 0u, 364u, 0u, 365u, 0u, 366u, 0u, 0u, 367u, 0u, 368u, 0u, 369u, 0u, 370u, 0u, 0u, 371u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 372u, 0u, 0u, 0u, 0u, 0u, 0u, 373u, 0u, 0u, 0u, 0u, 0u, 0u, 374u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 375u, 0u,
    0u, 0u, 0u, 376u, 0u, 0u, 0u, 0u, 377u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 378u, 0u, 0u, 379u, 0u, 380u, 381u, 0u, 382u, 0u, 383u, 0u,
    0u, 384u, 0u, 385u, 0u, 386u, 387u, 0u, 0u, 388u, 0u, 0u, 389u, 0u, 390u, 391u, 0u, 392u, 0u, 0u, 393u, 0u, 394u, 0u, 0u, 395u, 0u, 0u, 0u, 0u, 396u, 0u,
    397u, 0u, 0u, 0u, 0u, 398u, 0u, 399u, 0u, 400u, 0u, 0u, 0u, 0u, 401u, 0u, 402u, 0u, 0u, 0u, 0u, 403u, 0u, 0u, 0u, 0u, 0u, 0u, 404u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 405u, 0u, 0u, 406u, 0u, 0u, 407u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 408u, 0u, 409u, 0u, 0u, 410u, 0u, 0u, 411u, 0u,
    0u, 412u, 0u, 0u, 0u, 0u, 0u, 0u, 413u, 0u, 0u, 0u, 414u, 0u, 0u, 0u, 0u, 0u, 0u, 415u, 0u, 0u, 0u, 416u, 0u, 0u, 0u, 0u, 0u, 417u, 0u, 0u,
    418u, 0u, 0u, 0u, 0u, 419u, 0u, 0u, 420u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 421u, 0u, 0u, 0u, 0u, 0u, 0u, 422u, 0u, 423u, 0u, 0u, 424u, 425u,
    0u, 0u, 426u, 0u, 427u, 0u, 0u, 0u, 0u, 0u, 428u, 0u, 0u, 0u, 429u, 0u, 430u, 0u, 431u, 0u, 0u, 0u, 432u, 0u, 433u, 0u, 434u, 0u, 0u, 435u, 0u, 0u,
    0u, 436u, 0u, 0u, 437u, 0u, 0u, 0u, 0u, 438u, 0u, 0u, 439u, 0u, 440u, 441u, 0u, 0u, 0u, 442u, 0u, 0u, 443u, 0u, 444u, 0u, 0u, 0u, 0u, 445u, 0u, 0u,
    0u, 446u, 0u, 0u, 0u, 447u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 448u, 0u, 0u, 449u, 0u, 450u, 0u, 451u, 0u, 0u, 452u, 0u, 0u, 453u, 0u, 454u, 0u, 0u,
    455u, 0u, 0u, 456u, 0u, 457u, 0u, 458u, 0u, 0u, 459u, 0u, 0u, 460u, 0u, 0u, 461u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 462u, 0u, 0u, 0u, 0u, 463u, 0u, 0u,
    464u, 0u, 0u, 0u, 465u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 466u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 467u, 0u, 0u,
    468u, 0u, 0u, 469u, 0u, 470u, 471u, 0u, 0u, 0u, 0u, 472u, 0u, 0u, 0u, 473u, 0u, 474u, 0u, 0u, 475u, 0u, 0u, 476u, 0u, 477u, 0u, 0u, 478u, 0u, 479u, 0u,
    0u, 0u, 480u, 0u, 481u, 0u, 0u, 482u, 0u, 0u, 0u, 0u, 0u, 0u, 483u, 0u, 0u, 484u, 485u, 0u, 0u, 0u, 486u, 0u, 487u, 0u, 0u, 0u, 488u,
};
void recomp_unit_0169_entry(Runtime &rt, AllegrexContext &ctx, std::uint16_t direct_entry_id, GuestMemory::AotFastView &aot_mem) {
// PSPRECOMP_AOT_REGCACHE_BEGIN
// PSPRECOMP_AOT_REGCACHE_META gprs=4,29,5,6,31,7 fprs=12,13,14,15 gpr_occ=3132 fpr_occ=1583 gpr_total=4429 fpr_total=2623
    std::uint32_t aot_gpr_4 = ctx.gpr[4];
    std::uint32_t aot_gpr_29 = ctx.gpr[29];
    std::uint32_t aot_gpr_5 = ctx.gpr[5];
    std::uint32_t aot_gpr_6 = ctx.gpr[6];
    std::uint32_t aot_gpr_31 = ctx.gpr[31];
    std::uint32_t aot_gpr_7 = ctx.gpr[7];
    float aot_fpr_12 = ctx.fpr[12];
    float aot_fpr_13 = ctx.fpr[13];
    float aot_fpr_14 = ctx.fpr[14];
    float aot_fpr_15 = ctx.fpr[15];
    bool aot_regcache_valid = true;
#define AOT_REGCACHE_SYNC_OUT() do { if (aot_regcache_valid) { ctx.gpr[4] = aot_gpr_4; ctx.gpr[29] = aot_gpr_29; ctx.gpr[5] = aot_gpr_5; ctx.gpr[6] = aot_gpr_6; ctx.gpr[31] = aot_gpr_31; ctx.gpr[7] = aot_gpr_7; ctx.fpr[12] = aot_fpr_12; ctx.fpr[13] = aot_fpr_13; ctx.fpr[14] = aot_fpr_14; ctx.fpr[15] = aot_fpr_15; } } while (false)
#define AOT_REGCACHE_SYNC_IN() do { if (aot_regcache_valid) { aot_gpr_4 = ctx.gpr[4]; aot_gpr_29 = ctx.gpr[29]; aot_gpr_5 = ctx.gpr[5]; aot_gpr_6 = ctx.gpr[6]; aot_gpr_31 = ctx.gpr[31]; aot_gpr_7 = ctx.gpr[7]; aot_fpr_12 = ctx.fpr[12]; aot_fpr_13 = ctx.fpr[13]; aot_fpr_14 = ctx.fpr[14]; aot_fpr_15 = ctx.fpr[15]; } } while (false)
// PSPRECOMP_AOT_REGCACHE_END
    std::uint32_t jump_target = 0u;
    std::uint32_t local_transfers = 0u;
    std::uint32_t local_redispatch_rounds = 0u;
    std::uint32_t local_pc = ctx.pc;
    std::uint32_t entry_id = direct_entry_id;
LOCAL_DISPATCH:
    {
    if (entry_id == 0u) {
        const std::uint32_t entry_delta = local_pc - 0x08AA8004u;
        entry_id = (entry_delta < 16372u && (entry_delta & 3u) == 0u)
            ? kEntryIdsV813_recomp_unit_0169[entry_delta >> 2u] : 0u;
    }
    switch (entry_id) {
    case 1u: goto L_08AA8004;
    case 2u: goto L_08AA800C;
    case 3u: goto L_08AA802C;
    case 4u: goto L_08AA8034;
    case 5u: goto L_08AA8040;
    case 6u: goto L_08AA8048;
    case 7u: goto L_08AA8050;
    case 8u: goto L_08AA8074;
    case 9u: goto L_08AA807C;
    case 10u: goto L_08AA8084;
    case 11u: goto L_08AA808C;
    case 12u: goto L_08AA8094;
    case 13u: goto L_08AA8098;
    case 14u: goto L_08AA80B8;
    case 15u: goto L_08AA80C0;
    case 16u: goto L_08AA80E4;
    case 17u: goto L_08AA80EC;
    case 18u: goto L_08AA810C;
    case 19u: goto L_08AA811C;
    case 20u: goto L_08AA815C;
    case 21u: goto L_08AA8170;
    case 22u: goto L_08AA8180;
    case 23u: goto L_08AA8184;
    case 24u: goto L_08AA81BC;
    case 25u: goto L_08AA81D8;
    case 26u: goto L_08AA821C;
    case 27u: goto L_08AA8250;
    case 28u: goto L_08AA8278;
    case 29u: goto L_08AA8294;
    case 30u: goto L_08AA829C;
    case 31u: goto L_08AA82CC;
    case 32u: goto L_08AA82D4;
    case 33u: goto L_08AA832C;
    case 34u: goto L_08AA8348;
    case 35u: goto L_08AA835C;
    case 36u: goto L_08AA8364;
    case 37u: goto L_08AA8368;
    case 38u: goto L_08AA83D4;
    case 39u: goto L_08AA83E8;
    case 40u: goto L_08AA8444;
    case 41u: goto L_08AA8448;
    case 42u: goto L_08AA846C;
    case 43u: goto L_08AA84D4;
    case 44u: goto L_08AA8524;
    case 45u: goto L_08AA8704;
    case 46u: goto L_08AA88C4;
    case 47u: goto L_08AA8964;
    case 48u: goto L_08AA8980;
    case 49u: goto L_08AA89E4;
    case 50u: goto L_08AA89EC;
    case 51u: goto L_08AA89FC;
    case 52u: goto L_08AA8B18;
    case 53u: goto L_08AA8B7C;
    case 54u: goto L_08AA8B84;
    case 55u: goto L_08AA8B90;
    case 56u: goto L_08AA8BA0;
    case 57u: goto L_08AA8BB4;
    case 58u: goto L_08AA8BC0;
    case 59u: goto L_08AA8BDC;
    case 60u: goto L_08AA8BF8;
    case 61u: goto L_08AA8C10;
    case 62u: goto L_08AA8C1C;
    case 63u: goto L_08AA8C40;
    case 64u: goto L_08AA8C4C;
    case 65u: goto L_08AA8C54;
    case 66u: goto L_08AA8C74;
    case 67u: goto L_08AA8C78;
    case 68u: goto L_08AA8C8C;
    case 69u: goto L_08AA8D58;
    case 70u: goto L_08AA8D6C;
    case 71u: goto L_08AA8D80;
    case 72u: goto L_08AA8D90;
    case 73u: goto L_08AA8DA8;
    case 74u: goto L_08AA8DB8;
    case 75u: goto L_08AA8DCC;
    case 76u: goto L_08AA8DDC;
    case 77u: goto L_08AA8DF4;
    case 78u: goto L_08AA8E04;
    case 79u: goto L_08AA8ED4;
    case 80u: goto L_08AA9058;
    case 81u: goto L_08AA9084;
    case 82u: goto L_08AA90D4;
    case 83u: goto L_08AA90F0;
    case 84u: goto L_08AA9168;
    case 85u: goto L_08AA9170;
    case 86u: goto L_08AA925C;
    case 87u: goto L_08AA9270;
    case 88u: goto L_08AA9284;
    case 89u: goto L_08AA9298;
    case 90u: goto L_08AA92AC;
    case 91u: goto L_08AA92B4;
    case 92u: goto L_08AA92BC;
    case 93u: goto L_08AA92CC;
    case 94u: goto L_08AA92E0;
    case 95u: goto L_08AA92F4;
    case 96u: goto L_08AA92F8;
    case 97u: goto L_08AA9310;
    case 98u: goto L_08AA9318;
    case 99u: goto L_08AA9320;
    case 100u: goto L_08AA9330;
    case 101u: goto L_08AA9344;
    case 102u: goto L_08AA9358;
    case 103u: goto L_08AA935C;
    case 104u: goto L_08AA937C;
    case 105u: goto L_08AA9380;
    case 106u: goto L_08AA9438;
    case 107u: goto L_08AA95C4;
    case 108u: goto L_08AA95CC;
    case 109u: goto L_08AA95D4;
    case 110u: goto L_08AA95E4;
    case 111u: goto L_08AA95F4;
    case 112u: goto L_08AA9604;
    case 113u: goto L_08AA962C;
    case 114u: goto L_08AA9754;
    case 115u: goto L_08AA9768;
    case 116u: goto L_08AA977C;
    case 117u: goto L_08AA9790;
    case 118u: goto L_08AA97A4;
    case 119u: goto L_08AA97AC;
    case 120u: goto L_08AA97B4;
    case 121u: goto L_08AA97C4;
    case 122u: goto L_08AA97D8;
    case 123u: goto L_08AA97EC;
    case 124u: goto L_08AA97F0;
    case 125u: goto L_08AA9808;
    case 126u: goto L_08AA9810;
    case 127u: goto L_08AA9818;
    case 128u: goto L_08AA9828;
    case 129u: goto L_08AA983C;
    case 130u: goto L_08AA9850;
    case 131u: goto L_08AA9854;
    case 132u: goto L_08AA9874;
    case 133u: goto L_08AA9878;
    case 134u: goto L_08AA992C;
    case 135u: goto L_08AA9AB8;
    case 136u: goto L_08AA9AC0;
    case 137u: goto L_08AA9AC8;
    case 138u: goto L_08AA9AD8;
    case 139u: goto L_08AA9AE8;
    case 140u: goto L_08AA9AF8;
    case 141u: goto L_08AA9B20;
    case 142u: goto L_08AA9BF0;
    case 143u: goto L_08AA9C04;
    case 144u: goto L_08AA9C18;
    case 145u: goto L_08AA9C28;
    case 146u: goto L_08AA9C40;
    case 147u: goto L_08AA9C50;
    case 148u: goto L_08AA9C64;
    case 149u: goto L_08AA9C74;
    case 150u: goto L_08AA9C8C;
    case 151u: goto L_08AA9C9C;
    case 152u: goto L_08AA9D78;
    case 153u: goto L_08AA9DE4;
    case 154u: goto L_08AA9E08;
    case 155u: goto L_08AA9F2C;
    case 156u: goto L_08AA9F34;
    case 157u: goto L_08AA9F5C;
    case 158u: goto L_08AAA088;
    case 159u: goto L_08AAA09C;
    case 160u: goto L_08AAA0B0;
    case 161u: goto L_08AAA0C4;
    case 162u: goto L_08AAA0D8;
    case 163u: goto L_08AAA0E0;
    case 164u: goto L_08AAA0E8;
    case 165u: goto L_08AAA0F8;
    case 166u: goto L_08AAA10C;
    case 167u: goto L_08AAA120;
    case 168u: goto L_08AAA124;
    case 169u: goto L_08AAA13C;
    case 170u: goto L_08AAA144;
    case 171u: goto L_08AAA14C;
    case 172u: goto L_08AAA15C;
    case 173u: goto L_08AAA170;
    case 174u: goto L_08AAA184;
    case 175u: goto L_08AAA188;
    case 176u: goto L_08AAA1A0;
    case 177u: goto L_08AAA1A8;
    case 178u: goto L_08AAA1B0;
    case 179u: goto L_08AAA1C0;
    case 180u: goto L_08AAA1D4;
    case 181u: goto L_08AAA1E8;
    case 182u: goto L_08AAA1F0;
    case 183u: goto L_08AAA2C4;
    case 184u: goto L_08AAA32C;
    case 185u: goto L_08AAA350;
    case 186u: goto L_08AAA480;
    case 187u: goto L_08AAA488;
    case 188u: goto L_08AAA4AC;
    case 189u: goto L_08AAA5D4;
    case 190u: goto L_08AAA5E8;
    case 191u: goto L_08AAA5FC;
    case 192u: goto L_08AAA610;
    case 193u: goto L_08AAA624;
    case 194u: goto L_08AAA62C;
    case 195u: goto L_08AAA634;
    case 196u: goto L_08AAA644;
    case 197u: goto L_08AAA658;
    case 198u: goto L_08AAA66C;
    case 199u: goto L_08AAA670;
    case 200u: goto L_08AAA688;
    case 201u: goto L_08AAA690;
    case 202u: goto L_08AAA698;
    case 203u: goto L_08AAA6A8;
    case 204u: goto L_08AAA6BC;
    case 205u: goto L_08AAA6D0;
    case 206u: goto L_08AAA6D4;
    case 207u: goto L_08AAA6EC;
    case 208u: goto L_08AAA6F4;
    case 209u: goto L_08AAA6FC;
    case 210u: goto L_08AAA70C;
    case 211u: goto L_08AAA720;
    case 212u: goto L_08AAA734;
    case 213u: goto L_08AAA73C;
    case 214u: goto L_08AAA810;
    case 215u: goto L_08AAA878;
    case 216u: goto L_08AAA89C;
    case 217u: goto L_08AAA9CC;
    case 218u: goto L_08AAA9D4;
    case 219u: goto L_08AAA9F8;
    case 220u: goto L_08AAAB50;
    case 221u: goto L_08AAAB64;
    case 222u: goto L_08AAAB78;
    case 223u: goto L_08AAAB8C;
    case 224u: goto L_08AAABA0;
    case 225u: goto L_08AAABA8;
    case 226u: goto L_08AAABB0;
    case 227u: goto L_08AAABC0;
    case 228u: goto L_08AAABD4;
    case 229u: goto L_08AAABE8;
    case 230u: goto L_08AAABEC;
    case 231u: goto L_08AAAC04;
    case 232u: goto L_08AAAC0C;
    case 233u: goto L_08AAAC14;
    case 234u: goto L_08AAAC24;
    case 235u: goto L_08AAAC38;
    case 236u: goto L_08AAAC4C;
    case 237u: goto L_08AAAC50;
    case 238u: goto L_08AAAC6C;
    case 239u: goto L_08AAAC74;
    case 240u: goto L_08AAAC7C;
    case 241u: goto L_08AAAC8C;
    case 242u: goto L_08AAACA0;
    case 243u: goto L_08AAACB4;
    case 244u: goto L_08AAACBC;
    case 245u: goto L_08AAAD90;
    case 246u: goto L_08AAADF0;
    case 247u: goto L_08AAAE04;
    case 248u: goto L_08AAAE30;
    case 249u: goto L_08AAAE40;
    case 250u: goto L_08AAAECC;
    case 251u: goto L_08AAAEDC;
    case 252u: goto L_08AAAF68;
    case 253u: goto L_08AAAF78;
    case 254u: goto L_08AAB000;
    case 255u: goto L_08AAB010;
    case 256u: goto L_08AAB1A0;
    case 257u: goto L_08AAB1A8;
    case 258u: goto L_08AAB1EC;
    case 259u: goto L_08AAB200;
    case 260u: goto L_08AAB20C;
    case 261u: goto L_08AAB21C;
    case 262u: goto L_08AAB234;
    case 263u: goto L_08AAB260;
    case 264u: goto L_08AAB27C;
    case 265u: goto L_08AAB298;
    case 266u: goto L_08AAB2A4;
    case 267u: goto L_08AAB2AC;
    case 268u: goto L_08AAB2B8;
    case 269u: goto L_08AAB2C8;
    case 270u: goto L_08AAB2D4;
    case 271u: goto L_08AAB2DC;
    case 272u: goto L_08AAB2E8;
    case 273u: goto L_08AAB2F0;
    case 274u: goto L_08AAB2F8;
    case 275u: goto L_08AAB300;
    case 276u: goto L_08AAB304;
    case 277u: goto L_08AAB314;
    case 278u: goto L_08AAB320;
    case 279u: goto L_08AAB328;
    case 280u: goto L_08AAB330;
    case 281u: goto L_08AAB340;
    case 282u: goto L_08AAB348;
    case 283u: goto L_08AAB350;
    case 284u: goto L_08AAB35C;
    case 285u: goto L_08AAB38C;
    case 286u: goto L_08AAB398;
    case 287u: goto L_08AAB3A0;
    case 288u: goto L_08AAB3A4;
    case 289u: goto L_08AAB3AC;
    case 290u: goto L_08AAB3B8;
    case 291u: goto L_08AAB3C4;
    case 292u: goto L_08AAB3CC;
    case 293u: goto L_08AAB3D0;
    case 294u: goto L_08AAB3E8;
    case 295u: goto L_08AAB3F0;
    case 296u: goto L_08AAB3FC;
    case 297u: goto L_08AAB404;
    case 298u: goto L_08AAB408;
    case 299u: goto L_08AAB41C;
    case 300u: goto L_08AAB424;
    case 301u: goto L_08AAB48C;
    case 302u: goto L_08AAB49C;
    case 303u: goto L_08AAB4A4;
    case 304u: goto L_08AAB4B4;
    case 305u: goto L_08AAB4CC;
    case 306u: goto L_08AAB4DC;
    case 307u: goto L_08AAB500;
    case 308u: goto L_08AAB510;
    case 309u: goto L_08AAB518;
    case 310u: goto L_08AAB520;
    case 311u: goto L_08AAB528;
    case 312u: goto L_08AAB530;
    case 313u: goto L_08AAB540;
    case 314u: goto L_08AAB54C;
    case 315u: goto L_08AAB55C;
    case 316u: goto L_08AAB56C;
    case 317u: goto L_08AAB588;
    case 318u: goto L_08AAB598;
    case 319u: goto L_08AAB59C;
    case 320u: goto L_08AAB5A0;
    case 321u: goto L_08AAB5BC;
    case 322u: goto L_08AAB5DC;
    case 323u: goto L_08AAB5F4;
    case 324u: goto L_08AAB600;
    case 325u: goto L_08AAB604;
    case 326u: goto L_08AAB60C;
    case 327u: goto L_08AAB618;
    case 328u: goto L_08AAB624;
    case 329u: goto L_08AAB62C;
    case 330u: goto L_08AAB638;
    case 331u: goto L_08AAB644;
    case 332u: goto L_08AAB668;
    case 333u: goto L_08AAB674;
    case 334u: goto L_08AAB67C;
    case 335u: goto L_08AAB688;
    case 336u: goto L_08AAB694;
    case 337u: goto L_08AAB69C;
    case 338u: goto L_08AAB6A8;
    case 339u: goto L_08AAB6B8;
    case 340u: goto L_08AAB6E0;
    case 341u: goto L_08AAB6F0;
    case 342u: goto L_08AAB6FC;
    case 343u: goto L_08AAB718;
    case 344u: goto L_08AAB728;
    case 345u: goto L_08AAB734;
    case 346u: goto L_08AAB754;
    case 347u: goto L_08AAB768;
    case 348u: goto L_08AAB770;
    case 349u: goto L_08AAB784;
    case 350u: goto L_08AAB790;
    case 351u: goto L_08AAB7CC;
    case 352u: goto L_08AAB7D0;
    case 353u: goto L_08AAB7EC;
    case 354u: goto L_08AAB7F0;
    case 355u: goto L_08AAB7F8;
    case 356u: goto L_08AAB808;
    case 357u: goto L_08AAB81C;
    case 358u: goto L_08AAB82C;
    case 359u: goto L_08AAB838;
    case 360u: goto L_08AAB878;
    case 361u: goto L_08AAB898;
    case 362u: goto L_08AAB8A0;
    case 363u: goto L_08AAB8A8;
    case 364u: goto L_08AAB8B0;
    case 365u: goto L_08AAB8B8;
    case 366u: goto L_08AAB8C0;
    case 367u: goto L_08AAB8CC;
    case 368u: goto L_08AAB8D4;
    case 369u: goto L_08AAB8DC;
    case 370u: goto L_08AAB8E4;
    case 371u: goto L_08AAB8F0;
    case 372u: goto L_08AAB910;
    case 373u: goto L_08AAB92C;
    case 374u: goto L_08AAB948;
    case 375u: goto L_08AAB97C;
    case 376u: goto L_08AAB990;
    case 377u: goto L_08AAB9A4;
    case 378u: goto L_08AAB9D4;
    case 379u: goto L_08AAB9E0;
    case 380u: goto L_08AAB9E8;
    case 381u: goto L_08AAB9EC;
    case 382u: goto L_08AAB9F4;
    case 383u: goto L_08AAB9FC;
    case 384u: goto L_08AABA08;
    case 385u: goto L_08AABA10;
    case 386u: goto L_08AABA18;
    case 387u: goto L_08AABA1C;
    case 388u: goto L_08AABA28;
    case 389u: goto L_08AABA34;
    case 390u: goto L_08AABA3C;
    case 391u: goto L_08AABA40;
    case 392u: goto L_08AABA48;
    case 393u: goto L_08AABA54;
    case 394u: goto L_08AABA5C;
    case 395u: goto L_08AABA68;
    case 396u: goto L_08AABA7C;
    case 397u: goto L_08AABA84;
    case 398u: goto L_08AABA98;
    case 399u: goto L_08AABAA0;
    case 400u: goto L_08AABAA8;
    case 401u: goto L_08AABABC;
    case 402u: goto L_08AABAC4;
    case 403u: goto L_08AABAD8;
    case 404u: goto L_08AABAF4;
    case 405u: goto L_08AABB18;
    case 406u: goto L_08AABB24;
    case 407u: goto L_08AABB30;
    case 408u: goto L_08AABB5C;
    case 409u: goto L_08AABB64;
    case 410u: goto L_08AABB70;
    case 411u: goto L_08AABB7C;
    case 412u: goto L_08AABB88;
    case 413u: goto L_08AABBA4;
    case 414u: goto L_08AABBB4;
    case 415u: goto L_08AABBD0;
    case 416u: goto L_08AABBE0;
    case 417u: goto L_08AABBF8;
    case 418u: goto L_08AABC04;
    case 419u: goto L_08AABC18;
    case 420u: goto L_08AABC24;
    case 421u: goto L_08AABC4C;
    case 422u: goto L_08AABC68;
    case 423u: goto L_08AABC70;
    case 424u: goto L_08AABC7C;
    case 425u: goto L_08AABC80;
    case 426u: goto L_08AABC8C;
    case 427u: goto L_08AABC94;
    case 428u: goto L_08AABCAC;
    case 429u: goto L_08AABCBC;
    case 430u: goto L_08AABCC4;
    case 431u: goto L_08AABCCC;
    case 432u: goto L_08AABCDC;
    case 433u: goto L_08AABCE4;
    case 434u: goto L_08AABCEC;
    case 435u: goto L_08AABCF8;
    case 436u: goto L_08AABD08;
    case 437u: goto L_08AABD14;
    case 438u: goto L_08AABD28;
    case 439u: goto L_08AABD34;
    case 440u: goto L_08AABD3C;
    case 441u: goto L_08AABD40;
    case 442u: goto L_08AABD50;
    case 443u: goto L_08AABD5C;
    case 444u: goto L_08AABD64;
    case 445u: goto L_08AABD78;
    case 446u: goto L_08AABD88;
    case 447u: goto L_08AABD98;
    case 448u: goto L_08AABDBC;
    case 449u: goto L_08AABDC8;
    case 450u: goto L_08AABDD0;
    case 451u: goto L_08AABDD8;
    case 452u: goto L_08AABDE4;
    case 453u: goto L_08AABDF0;
    case 454u: goto L_08AABDF8;
    case 455u: goto L_08AABE04;
    case 456u: goto L_08AABE10;
    case 457u: goto L_08AABE18;
    case 458u: goto L_08AABE20;
    case 459u: goto L_08AABE2C;
    case 460u: goto L_08AABE38;
    case 461u: goto L_08AABE44;
    case 462u: goto L_08AABE64;
    case 463u: goto L_08AABE78;
    case 464u: goto L_08AABE84;
    case 465u: goto L_08AABE94;
    case 466u: goto L_08AABED4;
    case 467u: goto L_08AABEF8;
    case 468u: goto L_08AABF04;
    case 469u: goto L_08AABF10;
    case 470u: goto L_08AABF18;
    case 471u: goto L_08AABF1C;
    case 472u: goto L_08AABF30;
    case 473u: goto L_08AABF40;
    case 474u: goto L_08AABF48;
    case 475u: goto L_08AABF54;
    case 476u: goto L_08AABF60;
    case 477u: goto L_08AABF68;
    case 478u: goto L_08AABF74;
    case 479u: goto L_08AABF7C;
    case 480u: goto L_08AABF8C;
    case 481u: goto L_08AABF94;
    case 482u: goto L_08AABFA0;
    case 483u: goto L_08AABFBC;
    case 484u: goto L_08AABFC8;
    case 485u: goto L_08AABFCC;
    case 486u: goto L_08AABFDC;
    case 487u: goto L_08AABFE4;
    case 488u: goto L_08AABFF4;
    default:
        if (local_transfers == 0u) rt.unsupported(ctx.pc, 0u, "invalid internal function entry");
        else ctx.pc = local_pc;
        AOT_REGCACHE_SYNC_OUT(); return;
    }
    }
// PSPRECOMP_V813_SHARED_JR_DISPATCH: one dynamic-JR reject/redispatch path per unit.
LOCAL_JR_DISPATCH:
    {
        const std::uint32_t local_delta_v813 = jump_target - 0x08AA8004u;
        if (local_delta_v813 >= 16372u || (local_delta_v813 & 3u) != 0u) {
            ctx.pc = jump_target;
            AOT_REGCACHE_SYNC_OUT();
            // PSPRECOMP_V814_CONTINUATION_RETURN
            Runtime::AotTailContinuation aot_cont_v814{};
            if (rt.take_aot_tail_continuation(jump_target, aot_cont_v814)) {
#if defined(__clang__) && defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
                [[clang::musttail]] return aot_cont_v814.function(
                    rt, ctx, aot_cont_v814.entry_id, aot_mem);
#else
                aot_cont_v814.function(rt, ctx, aot_cont_v814.entry_id, aot_mem); return;
#endif
            }
            return;
        }
    }
    local_pc = jump_target;
    entry_id = 0u;
    goto LOCAL_DISPATCH;

// PSPRECOMP_V812_SHARED_SCHED_BLOCK: one scheduler-exact slow boundary per unit.
LOCAL_SCHED_BOUNDARY:
    ctx.pc = jump_target;
    AOT_REGCACHE_SYNC_OUT();
    if (local_redispatch_rounds < 7u && rt.continue_generated_local_dispatch(ctx)) {
        ++local_redispatch_rounds;
        AOT_REGCACHE_SYNC_IN();
        local_transfers = 0u;
        local_pc = ctx.pc;
        entry_id = 0u;
        goto LOCAL_DISPATCH;
    }
    return;
L_08AA8004:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA800C;
    }
L_08AA800C:
    aot_gpr_5 = (0u | 15u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(92)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(344));
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_6 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_7);
    jump_target = ctx.gpr[8];
    aot_gpr_31 = (0x08AA802Cu);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA802Cu) goto L_08AA802C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA802C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA8034;
    }
L_08AA8034:
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < 2 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < 3 ? 1u : 0u);
      if (branch_taken) {
          goto L_08AA807C;
      }
      goto L_08AA8040;
    }
L_08AA8040:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[20]) < 0;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA8048;
    }
L_08AA8048:
    if (static_cast<std::int32_t>(ctx.gpr[20]) > 0) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
        goto L_08AA8098;
    }
    goto L_08AA8050;
L_08AA8050:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
    aot_gpr_5 = (0u | 13u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(92)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(344));
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_6 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_7);
    jump_target = ctx.gpr[8];
    aot_gpr_31 = (0x08AA8074u);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8074u) goto L_08AA8074;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA8074:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA807C;
    }
L_08AA807C:
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < 4 ? 1u : 0u);
      if (branch_taken) {
          goto L_08AA80C0;
      }
      goto L_08AA8084;
    }
L_08AA8084:
    if (aot_gpr_4 != 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
        goto L_08AA80EC;
    }
    goto L_08AA808C;
L_08AA808C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA8094;
    }
L_08AA8094:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
    goto L_08AA8098;
L_08AA8098:
    aot_gpr_5 = (0u | 15u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(92)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(344));
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_6 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_7);
    jump_target = ctx.gpr[8];
    aot_gpr_31 = (0x08AA80B8u);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA80B8u) goto L_08AA80B8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA80B8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA80C0;
    }
L_08AA80C0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
    aot_gpr_5 = (0u | 14u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(92)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(344));
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_6 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_7);
    jump_target = ctx.gpr[8];
    aot_gpr_31 = (0x08AA80E4u);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA80E4u) goto L_08AA80E4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA80E4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA810C;
      }
      goto L_08AA80EC;
    }
L_08AA80EC:
    aot_gpr_5 = (0u | 16u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(92)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(344));
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_6 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_7);
    jump_target = ctx.gpr[8];
    aot_gpr_31 = (0x08AA810Cu);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA810Cu) goto L_08AA810C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA810C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(120)));
    ctx.gpr[16] = (0u | 0u);
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[26];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(120), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08AA811C;
L_08AA811C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(132)));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(ctx.fpr[20]));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_5);
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vscl_ct<0u, 0u, 28u, 3u>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[17] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_4 = (0u | 68u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    aot_gpr_7 = (0u | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[28]));
    ctx.gpr[8] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    aot_gpr_31 = (0x08AA815Cu);
    ctx.gpr[11] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0062.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 20u, 0x08AA815Cu, 0x088FF060u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0062_entry(rt, ctx, 716u, aot_mem);
#else
        recomp_unit_0062_entry(rt, ctx, 716u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0062_entry, 62u, 716u, 0x088FF060u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA815Cu) goto L_08AA815C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA815C:
    ctx.gpr[16] = (ctx.gpr[16] + static_cast<std::uint32_t>(1));
    ctx.gpr[16] = (ctx.gpr[16] & 255u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[16]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AA811C;
      }
      goto L_08AA8170;
    }
L_08AA8170:
    ctx.gpr[22] = (ctx.gpr[22] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[22]) < 12 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[21] = (ctx.gpr[21] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0168_entry, 168u, 627u, 0x08AA7F94u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_08AA8180;
    }
L_08AA8180:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(372)));
    goto L_08AA8184;
L_08AA8184:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(368)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(380)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(4));
    aot_gpr_7 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(376)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(372), aot_gpr_4);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(4));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(380), aot_gpr_6);
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(368), aot_gpr_5);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(376), aot_gpr_7);
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0168_entry, 168u, 618u, 0x08AA7ED8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_08AA81BC;
    }
L_08AA81BC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(388)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_29 + static_cast<std::uint32_t>(128))))));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(388), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0168_entry, 168u, 606u, 0x08AA7DF0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_08AA81D8;
    }
L_08AA81D8:
    { std::uint32_t aot_run_words[15]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(392), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      ctx.fpr[26] = std::bit_cast<float>(aot_run_words[3]);
      ctx.fpr[28] = std::bit_cast<float>(aot_run_words[4]);
      ctx.gpr[16] = aot_run_words[5];
      ctx.gpr[17] = aot_run_words[6];
      ctx.gpr[18] = aot_run_words[7];
      ctx.gpr[19] = aot_run_words[8];
      ctx.gpr[20] = aot_run_words[9];
      ctx.gpr[21] = aot_run_words[10];
      ctx.gpr[22] = aot_run_words[11];
      ctx.gpr[23] = aot_run_words[12];
      ctx.gpr[30] = aot_run_words[13];
      aot_gpr_31 = aot_run_words[14];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(464));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA821C:
    { const std::uint32_t vfpu_address = aot_gpr_6 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<4u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(16);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(32);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<2u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(48);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<3u, 4u>(vfpu_value); }
    { float vfpu_value[4]{1.0f, 1.0f, 1.0f, 1.0f};
      ctx.write_vfpu_vector_with_destination_prefix_ct<100u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<5u, 32u, 4u, 4u>();
    ctx.execute_vfpu_vdot_ct<37u, 33u, 4u, 4u>();
    ctx.execute_vfpu_vdot_ct<69u, 34u, 4u, 4u>();
    jump_target = aot_gpr_31;
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<5u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8250:
    { const std::uint32_t vfpu_address = aot_gpr_6 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<4u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(16);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<5u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(32);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<6u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(48);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<7u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vtfm_ct<0u, 36u, 1u, 3u, 3u>();
    ctx.execute_vfpu_vec3_ct<0u, 0u, 7u, 3u, 0u>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8278:
    aot_gpr_4 = (18303u << 16u);
    aot_fpr_13 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_4 | 64000u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_13)) && aot_fpr_12 == aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[0] = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_08AA829C;
      }
      goto L_08AA8294;
    }
L_08AA8294:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA82CC;
      }
      goto L_08AA829C;
    }
L_08AA829C:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7792)));
    aot_gpr_4 = (51071u << 16u);
    aot_fpr_15 = aot_fpr_12 - aot_fpr_13;
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7796)));
    aot_gpr_4 = (aot_gpr_4 | 64000u);
    ctx.fpr[16] = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = aot_fpr_14 - aot_fpr_13;
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_15; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_12 = aot_fpr_14 / aot_fpr_12;
    ctx.fpr[0] = aot_fpr_12 + ctx.fpr[0];
    goto L_08AA82CC;
L_08AA82CC:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA82D4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    { const std::uint32_t aot_run_words[7]{std::bit_cast<std::uint32_t>(ctx.fpr[20]), std::bit_cast<std::uint32_t>(ctx.fpr[22]), ctx.gpr[16], ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words); }
    ctx.gpr[16] = (aot_gpr_7 | 0u);
    ctx.gpr[17] = (aot_gpr_6 | 0u);
    ctx.gpr[18] = (aot_gpr_5 | 0u);
    aot_gpr_6 = (aot_gpr_4 | 0u);
    ctx.gpr[19] = (ctx.gpr[8] & 255u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7792)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2988)));
    ctx.fpr[22] = aot_fpr_12 + aot_fpr_13;
    ctx.fpr[20] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7796)));
    aot_gpr_4 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(32304));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(2656));
    aot_gpr_31 = (0x08AA832Cu);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    goto L_08AA8250;
L_08AA832C:
    { const std::uint32_t vfpu_address = aot_gpr_29 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[18] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= ctx.fpr[22])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA8444;
      }
      goto L_08AA8348;
    }
L_08AA8348:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < ctx.fpr[20])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16256u << 16u);
      if (branch_taken) {
          goto L_08AA8368;
      }
      goto L_08AA835C;
    }
L_08AA835C:
    { const bool branch_taken = ctx.gpr[19] != 0u;
      if (branch_taken) {
          goto L_08AA8444;
      }
      goto L_08AA8364;
    }
L_08AA8364:
    aot_gpr_4 = (16256u << 16u);
    goto L_08AA8368;
L_08AA8368:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_13 / aot_fpr_12;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(0)));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(4)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(ctx.gpr[18] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(0)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(ctx.gpr[18] + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2980)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16976)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_13)) && aot_fpr_12 == aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (17036u << 16u);
      if (branch_taken) {
          goto L_08AA83E8;
      }
      goto L_08AA83D4;
    }
L_08AA83D4:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16976)));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = aot_fpr_13 / aot_fpr_12;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2980), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2976), std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_08AA83E8;
L_08AA83E8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2976)));
    aot_gpr_4 = (17392u << 16u);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    aot_fpr_15 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_15 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_15)));
    aot_fpr_14 = aot_fpr_15 / aot_fpr_14;
    aot_gpr_4 = (17288u << 16u);
    ctx.gpr[2] = (0u | 1u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(0)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_08AA8448;
      }
      goto L_08AA8444;
    }
L_08AA8444:
    ctx.gpr[2] = (0u | 0u);
    goto L_08AA8448;
L_08AA8448:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.gpr[16] = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      ctx.gpr[18] = aot_run_words[4];
      ctx.gpr[19] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA846C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-64));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), aot_gpr_31);
    aot_gpr_4 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(32304));
    aot_gpr_4 = (aot_gpr_5 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(92)));
    aot_gpr_4 = (17723u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 32768u);
    aot_fpr_15 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_14; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(96)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(2656));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    aot_gpr_31 = (0x08AA84D4u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    goto L_08AA8250;
L_08AA84D4:
    { const std::uint32_t vfpu_address = aot_gpr_29 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_13 / aot_fpr_12;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_4 = (16608u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[0] = ctx.fpr[0] + aot_fpr_13;
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8524:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(10212)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    ctx.gpr[10] = (16128u << 16u);
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_fpr_15 = std::bit_cast<float>(ctx.gpr[10]);
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[11] = (ctx.gpr[11] << 16u);
    ctx.gpr[11] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[11]) >> 16u));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[11]));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(12)));
    ctx.gpr[10] = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.gpr[2] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[11] = (ctx.gpr[11] << 16u);
    ctx.gpr[11] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[11]) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[2]));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[11]));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(0)));
    ctx.gpr[2] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(1)));
    ctx.gpr[3] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(2)));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[3]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(ctx.gpr[8]));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    ctx.gpr[8] = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[11] = (18176u << 16u);
    ctx.gpr[11] = (ctx.gpr[11] | 128u);
    ctx.gpr[2] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[2] = (ctx.gpr[2] << 16u);
    aot_fpr_13 = std::bit_cast<float>(ctx.gpr[11]);
    ctx.gpr[2] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[2]) >> 16u));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[2]));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(12)));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.gpr[2] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[11] = (ctx.gpr[11] << 16u);
    ctx.gpr[11] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[11]) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[11]));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[11]));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(ctx.gpr[9] + static_cast<std::uint32_t>(0)));
    ctx.gpr[2] = (aot_mem.aot_direct_load8(ctx.gpr[9] + static_cast<std::uint32_t>(1)));
    ctx.gpr[3] = (aot_mem.aot_direct_load8(ctx.gpr[9] + static_cast<std::uint32_t>(2)));
    ctx.gpr[9] = (aot_mem.aot_direct_load8(ctx.gpr[9] + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[3]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(ctx.gpr[9]));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[8] = (aot_gpr_4 + static_cast<std::uint32_t>(32));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.fpr[18] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[9] = (ctx.gpr[9] << 16u);
    ctx.gpr[9] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[9]) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[11] = (ctx.gpr[11] << 16u);
    ctx.gpr[11] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[11]) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store16(ctx.gpr[8] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.gpr[9] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(0)));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(1)));
    ctx.gpr[2] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(2)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store8(ctx.gpr[8] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_7 = (aot_gpr_7 << 16u);
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_7) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_7));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_gpr_7 = (aot_gpr_7 << 16u);
    aot_gpr_7 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_7) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(1)));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(2)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[8]));
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_6));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8704:
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(10212)));
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    ctx.gpr[9] = (16128u << 16u);
    ctx.fpr[0] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    ctx.fpr[2] = std::bit_cast<float>(ctx.gpr[9]);
    ctx.gpr[10] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    ctx.gpr[10] = (ctx.gpr[10] << 16u);
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.fpr[1] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[2]));
    ctx.gpr[10] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[10]) >> 16u));
    ctx.gpr[9] = (ctx.gpr[11] << 16u);
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[2]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[10]));
    ctx.gpr[9] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[9]) >> 16u));
    ctx.gpr[10] = (std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[18]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[9]));
    ctx.gpr[3] = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.gpr[9] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(0)));
    ctx.fpr[19] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[19]));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(1)));
    ctx.gpr[2] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(2)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[9]));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[2]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[11]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.gpr[11] = (ctx.gpr[3] << 16u);
    aot_gpr_7 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    ctx.gpr[11] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[11]) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[11]));
    ctx.gpr[11] = (18176u << 16u);
    ctx.gpr[11] = (ctx.gpr[11] | 128u);
    ctx.gpr[9] = (ctx.gpr[9] << 16u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.gpr[11]);
    ctx.gpr[9] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[9]) >> 16u));
    ctx.fpr[18] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    ctx.gpr[9] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(0)));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(1)));
    ctx.gpr[3] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[2] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(2)));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(ctx.gpr[8] + static_cast<std::uint32_t>(3)));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[9]));
    ctx.fpr[18] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[11]));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[2]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(ctx.gpr[8]));
    ctx.gpr[8] = (ctx.gpr[3] << 16u);
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    aot_gpr_7 = (aot_gpr_4 + static_cast<std::uint32_t>(32));
    ctx.gpr[8] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[8]) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(ctx.gpr[8]));
    ctx.gpr[8] = (ctx.gpr[9] << 16u);
    ctx.gpr[8] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[8]) >> 16u));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_7 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[11]));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    ctx.gpr[2] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[9] = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(1)));
    ctx.gpr[11] = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(2)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(3)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[2]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(ctx.gpr[8]));
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(ctx.gpr[9]));
    ctx.gpr[8] = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store8(aot_gpr_7 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (ctx.gpr[2] << 16u);
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 16u));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_6));
    aot_gpr_6 = (ctx.gpr[8] << 16u);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 16u));
    ctx.gpr[8] = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_4 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(1)));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(2)));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(3)));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(ctx.gpr[8]));
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA88C4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_gpr_7 = (0u | 1u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), aot_gpr_7);
    aot_gpr_5 = (aot_gpr_5 << 16u);
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 16u));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_7)) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(1)));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(2)));
    aot_gpr_7 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(19), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[8])) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.fpr[16] = aot_fpr_12 - aot_fpr_14;
    ctx.fpr[17] = aot_fpr_13 - aot_fpr_15;
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(16), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_gpr_7 << 6u);
    aot_gpr_7 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_fpr_14 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_15 = aot_fpr_13 + aot_fpr_15;
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_7) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(17), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (2238u << 16u);
    aot_gpr_5 = (ctx.lo);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-272));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), ctx.gpr[16]);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.gpr[16] = (aot_gpr_6 + aot_gpr_4);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(18), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), aot_gpr_31);
    aot_gpr_31 = (0x08AA8964u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_089D1B1C, 115u, 292u, 0x089D1B1Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31, aot_fpr_12, aot_fpr_13, aot_fpr_14, aot_fpr_15); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0115_entry, 115u, 292u, 0x089D1B1Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8964u) goto L_08AA8964;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA8964:
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    aot_gpr_4 = (ctx.gpr[16] | 0u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    aot_gpr_7 = (aot_gpr_6 | 0u);
    ctx.gpr[8] = (aot_gpr_6 | 0u);
    aot_gpr_31 = (0x08AA8980u);
    ctx.gpr[9] = (aot_gpr_6 | 0u);
    goto L_08AA8524;
L_08AA8980:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_6 = (2238u << 16u);
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(12016));
    aot_gpr_4 = (aot_gpr_4 & 65535u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_6 = (aot_gpr_4 & 65535u);
    aot_gpr_7 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.gpr[8] = (aot_gpr_6 + static_cast<std::uint32_t>(2));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_7));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AA89EC;
      }
      goto L_08AA89E4;
    }
L_08AA89E4:
    aot_gpr_31 = (0x08AA89ECu);
    goto L_08AA8BDC;
L_08AA89EC:
    ctx.gpr[16] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA89FC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_7 = (17536u << 16u);
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_7);
    aot_gpr_7 = (16585u << 16u);
    aot_gpr_7 = (aot_gpr_7 | 4059u);
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[16] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[16] = fs * ft; }
    ctx.fpr[18] = std::bit_cast<float>(aot_gpr_7);
    ctx.fpr[16] = ctx.fpr[16] / ctx.fpr[18];
    aot_gpr_7 = (0u | 1u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), aot_gpr_7);
    aot_gpr_5 = (aot_gpr_5 << 16u);
    ctx.gpr[9] = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 16u));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[9])) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[10] = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(1)));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(2)));
    ctx.gpr[9] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(3), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(0), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_gpr_7 = (2247u << 16u);
    aot_gpr_6 = (aot_gpr_6 & 1023u);
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(-8224));
    ctx.gpr[8] = (2247u << 16u);
    aot_gpr_6 = (aot_gpr_6 << 2u);
    ctx.gpr[8] = (ctx.gpr[8] + static_cast<std::uint32_t>(-12320));
    aot_gpr_7 = (aot_gpr_6 + aot_gpr_7);
    aot_gpr_6 = (aot_gpr_6 + ctx.gpr[8]);
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_7 + static_cast<std::uint32_t>(0)));
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[16] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[16] = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[19] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[19] = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    aot_fpr_14 = aot_fpr_12 + ctx.fpr[16];
    ctx.fpr[18] = aot_fpr_12 - ctx.fpr[16];
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[10])) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.fpr[2] = aot_fpr_13 - ctx.fpr[17];
    ctx.fpr[1] = aot_fpr_14 - ctx.fpr[19];
    ctx.fpr[16] = ctx.fpr[18] - ctx.fpr[19];
    aot_gpr_6 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_5)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[17];
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[19];
    ctx.fpr[18] = ctx.fpr[18] + ctx.fpr[19];
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(1), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_5 = (2238u << 16u);
    aot_gpr_6 = (ctx.lo);
    aot_gpr_4 = (ctx.gpr[9] << 6u);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-272));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(2), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    ctx.fpr[3] = ctx.fpr[2] - ctx.fpr[0];
    aot_fpr_15 = aot_fpr_13 - ctx.fpr[0];
    aot_gpr_5 = (aot_gpr_29 | 0u);
    ctx.fpr[17] = ctx.fpr[2] + ctx.fpr[0];
    aot_gpr_6 = (aot_gpr_29 | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    aot_gpr_7 = (aot_gpr_29 | 0u);
    ctx.fpr[19] = aot_fpr_13 + ctx.fpr[0];
    ctx.gpr[8] = (aot_gpr_29 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08AA8B18u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    goto L_08AA8704;
L_08AA8B18:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_6 = (2238u << 16u);
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(12016));
    aot_gpr_4 = (aot_gpr_4 & 65535u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_6 = (aot_gpr_4 & 65535u);
    aot_gpr_7 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.gpr[8] = (aot_gpr_6 + static_cast<std::uint32_t>(2));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_7));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AA8B84;
      }
      goto L_08AA8B7C;
    }
L_08AA8B7C:
    aot_gpr_31 = (0x08AA8B84u);
    goto L_08AA8BDC;
L_08AA8B84:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8B90:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08AA8BA0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8896)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 56u, 0x08AA8BA0u, 0x08890830u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 122u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 122u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 122u, 0x08890830u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8BA0u) goto L_08AA8BA0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA8BA0:
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 / ctx.fpr[0];
    aot_gpr_31 = (0x08AA8BB4u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(10220), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08AA8BC0;
L_08AA8BB4:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8BC0:
    aot_gpr_4 = (18303u << 16u);
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_4 | 64000u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(10216), std::bit_cast<std::uint32_t>(aot_fpr_12));
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(10212), std::bit_cast<std::uint32_t>(aot_fpr_13));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8BDC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    { const std::uint32_t aot_run_words[3]{ctx.gpr[16], ctx.gpr[17], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) <= 0;
      if (branch_taken) {
          goto L_08AA8C78;
      }
      goto L_08AA8BF8;
    }
L_08AA8BF8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984)));
    ctx.gpr[17] = (2238u << 16u);
    ctx.gpr[16] = (2238u << 16u);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(-272));
    { const bool branch_taken = aot_gpr_5 == 0u;
    ctx.gpr[16] = (ctx.gpr[16] + static_cast<std::uint32_t>(12016));
      if (branch_taken) {
          goto L_08AA8C54;
      }
      goto L_08AA8C10;
    }
L_08AA8C10:
    aot_gpr_4 = (0u | 4u);
    aot_gpr_31 = (0x08AA8C1Cu);
    aot_gpr_5 = (0u | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08861668, 23u, 203u, 0x08861668u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0023_entry, 23u, 203u, 0x08861668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8C1Cu) goto L_08AA8C1C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA8C1C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_gpr_4 = (0u | 3u);
    aot_gpr_7 = (aot_gpr_5 + aot_gpr_5);
    aot_gpr_6 = (aot_gpr_5 << 2u);
    ctx.gpr[8] = (aot_gpr_5 + aot_gpr_7);
    ctx.gpr[8] = (ctx.gpr[8] + ctx.gpr[8]);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08AA8C40u);
    aot_gpr_7 = (ctx.gpr[16] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0186.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 63u, 0x08AA8C40u, 0x08AEE294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0186_entry(rt, ctx, 495u, aot_mem);
#else
        recomp_unit_0186_entry(rt, ctx, 495u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0186_entry, 186u, 495u, 0x08AEE294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8C40u) goto L_08AA8C40;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA8C40:
    aot_gpr_4 = (0u | 4u);
    aot_gpr_31 = (0x08AA8C4Cu);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08861668, 23u, 203u, 0x08861668u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0023_entry, 23u, 203u, 0x08861668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8C4Cu) goto L_08AA8C4C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AA8C4C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA8C74;
      }
      goto L_08AA8C54;
    }
L_08AA8C54:
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_6 = (aot_gpr_4 << 2u);
    ctx.gpr[8] = (aot_gpr_4 + aot_gpr_5);
    ctx.gpr[8] = (ctx.gpr[8] + ctx.gpr[8]);
    aot_gpr_4 = (0u | 3u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08AA8C74u);
    aot_gpr_7 = (ctx.gpr[16] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0186.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 66u, 0x08AA8C74u, 0x08AEE294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0186_entry(rt, ctx, 495u, aot_mem);
#else
        recomp_unit_0186_entry(rt, ctx, 495u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0186_entry, 186u, 495u, 0x08AEE294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA8C74u) goto L_08AA8C74;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA8C74:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), 0u);
    goto L_08AA8C78;
L_08AA8C78:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA8C8C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-112));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), ctx.gpr[16]);
    ctx.gpr[16] = (aot_gpr_4 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[19]);
    ctx.gpr[19] = (aot_gpr_7 << 16u);
    aot_gpr_4 = (16128u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), std::bit_cast<std::uint32_t>(ctx.fpr[20]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[18]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(88), ctx.gpr[20]);
    ctx.fpr[20] = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[17] = (aot_gpr_5 & 255u);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    ctx.gpr[19] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[19]) >> 16u));
    ctx.gpr[20] = (ctx.gpr[8] & 255u);
    { const std::uint32_t aot_run_words[3]{ctx.gpr[21], ctx.gpr[22], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(92), aot_run_words); }
    ctx.fpr[17] = aot_fpr_12 - aot_fpr_15;
    ctx.fpr[18] = aot_fpr_13 - ctx.fpr[16];
    aot_gpr_5 = (16256u << 16u);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    { const std::uint32_t aot_run_words[4]{std::bit_cast<std::uint32_t>(ctx.fpr[17]), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(ctx.fpr[18])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_fpr_12 = std::bit_cast<float>(0u);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_13), std::bit_cast<std::uint32_t>(aot_fpr_13), std::bit_cast<std::uint32_t>(ctx.fpr[18])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    aot_gpr_4 = (48896u << 16u);
    ctx.fpr[18] = std::bit_cast<float>(aot_gpr_5);
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    ctx.fpr[19] = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_4 = (0u | 0u);
    ctx.fpr[18] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[18])));
    ctx.fpr[19] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[19])));
    goto L_08AA8D58;
L_08AA8D58:
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[0] < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[2] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(16)));
      if (branch_taken) {
          goto L_08AA8D80;
      }
      goto L_08AA8D6C;
    }
L_08AA8D6C:
    ctx.fpr[0] = ctx.fpr[0] / aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[1] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[1] = fs * ft; }
    ctx.fpr[0] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    goto L_08AA8D80;
L_08AA8D80:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[0] <= ctx.fpr[18])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA8DA8;
      }
      goto L_08AA8D90;
    }
L_08AA8D90:
    ctx.fpr[0] = ctx.fpr[0] - ctx.fpr[18];
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = ctx.fpr[0] / aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.fpr[0] = aot_fpr_13 - ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA8DA8;
L_08AA8DA8:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[2] < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA8DCC;
      }
      goto L_08AA8DB8;
    }
L_08AA8DB8:
    ctx.fpr[0] = ctx.fpr[2] / ctx.fpr[16];
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[2] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA8DCC;
L_08AA8DCC:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[2] <= ctx.fpr[19])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA8DF4;
      }
      goto L_08AA8DDC;
    }
L_08AA8DDC:
    ctx.fpr[0] = ctx.fpr[2] - ctx.fpr[19];
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = ctx.fpr[0] / ctx.fpr[16];
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    ctx.fpr[0] = aot_fpr_13 - ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA8DF4;
L_08AA8DF4:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_4) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08AA8D58;
      }
      goto L_08AA8E04;
    }
L_08AA8E04:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.gpr[21] = (2236u << 16u);
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[22] = (ctx.gpr[21] + static_cast<std::uint32_t>(-8736));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AA8ED4u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AA8ED4:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[16])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[20]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(28), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(44), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(60), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(20), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(21), static_cast<std::uint8_t>(aot_gpr_5));
    ctx.gpr[8] = (18176u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    aot_fpr_13 = std::bit_cast<float>(ctx.gpr[8]);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(22), static_cast<std::uint8_t>(aot_gpr_6));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(23), static_cast<std::uint8_t>(aot_gpr_7));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(36), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(37), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(38), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(39), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[20];
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(52), static_cast<std::uint8_t>(aot_gpr_4));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(53), static_cast<std::uint8_t>(aot_gpr_5));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(54), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(55), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    aot_fpr_15 = aot_fpr_15 + ctx.fpr[20];
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[21] + static_cast<std::uint32_t>(-8736), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[16] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[16] = fs * ft; }
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    ctx.fpr[16] = ctx.fpr[16] + ctx.fpr[20];
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[18] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[18] = fs * ft; }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    aot_fpr_14 = ctx.fpr[17] + ctx.fpr[20];
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_15 = ctx.fpr[18] + ctx.fpr[20];
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[20];
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_5 = (ctx.gpr[22] | 0u);
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_6));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (0u | 5u);
    aot_gpr_6 = (0u | 4u);
    aot_gpr_31 = (0x08AA9058u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(aot_gpr_7));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0186.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 80u, 0x08AA9058u, 0x08AEE120u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0186_entry(rt, ctx, 486u, aot_mem);
#else
        recomp_unit_0186_entry(rt, ctx, 486u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0186_entry, 186u, 486u, 0x08AEE120u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA9058u) goto L_08AA9058;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA9058:
    { std::uint32_t aot_run_words[9]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.gpr[16] = aot_run_words[1];
      ctx.gpr[17] = aot_run_words[2];
      ctx.gpr[18] = aot_run_words[3];
      ctx.gpr[19] = aot_run_words[4];
      ctx.gpr[20] = aot_run_words[5];
      ctx.gpr[21] = aot_run_words[6];
      ctx.gpr[22] = aot_run_words[7];
      aot_gpr_31 = aot_run_words[8];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA9084:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-112));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[20]);
    ctx.gpr[20] = (aot_gpr_4 & 255u);
    aot_gpr_4 = (16403u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 13107u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), ctx.gpr[17]);
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[17] = (aot_gpr_7 << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[18]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[19]);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[17])) ? 0x00800000u : 0u);
    ctx.gpr[19] = (aot_gpr_5 & 255u);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    ctx.gpr[17] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[17]) >> 16u));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), ctx.gpr[16]);
    { const std::uint32_t aot_run_words[3]{ctx.gpr[21], ctx.gpr[22], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(88), aot_run_words); }
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.gpr[16] = (ctx.gpr[8] & 255u);
      if (branch_taken) {
          goto L_08AA9170;
      }
      goto L_08AA90D4;
    }
L_08AA90D4:
    aot_gpr_4 = (16294u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26214u);
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[17])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9168;
      }
      goto L_08AA90F0;
    }
L_08AA90F0:
    ctx.fpr[17] = aot_fpr_14 - ctx.fpr[17];
    aot_gpr_4 = (17279u << 16u);
    ctx.fpr[19] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = ctx.fpr[17]; const float ft = ctx.fpr[19]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[20])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[20] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    ctx.gpr[20] = (ctx.gpr[20] & 255u);
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[19] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    ctx.gpr[19] = (ctx.gpr[19] & 255u);
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[18] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    ctx.gpr[18] = (ctx.gpr[18] & 255u);
    aot_gpr_4 = (ctx.lo);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    ctx.gpr[17] = (aot_gpr_4 << 16u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[17] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[17]) >> 16u));
      if (branch_taken) {
          goto L_08AA9170;
      }
      goto L_08AA9168;
    }
L_08AA9168:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9604;
      }
      goto L_08AA9170;
    }
L_08AA9170:
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[19] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[18] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[19]) ^ 0x80000000u);
    ctx.fpr[18] = ctx.fpr[18] - ctx.fpr[17];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[18]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = aot_fpr_12 + ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fpr[2] = ctx.fpr[17] - ctx.fpr[19];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[1] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[1] = fs * ft; }
    ctx.fpr[1] = aot_fpr_12 + ctx.fpr[1];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    ctx.fpr[1] = ctx.fpr[19] + ctx.fpr[17];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    ctx.fpr[3] = aot_fpr_12 + ctx.fpr[3];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    ctx.fpr[17] = ctx.fpr[19] - ctx.fpr[17];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[18]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9298;
      }
      goto L_08AA925C;
    }
L_08AA925C:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9298;
      }
      goto L_08AA9270;
    }
L_08AA9270:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9298;
      }
      goto L_08AA9284;
    }
L_08AA9284:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA92B4;
      }
      goto L_08AA9298;
    }
L_08AA9298:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AA92BC;
    }
    goto L_08AA92AC;
L_08AA92AC:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
      if (branch_taken) {
          goto L_08AA92F8;
      }
      goto L_08AA92B4;
    }
L_08AA92B4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9604;
      }
      goto L_08AA92BC;
    }
L_08AA92BC:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_13)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AA92F8;
    }
    goto L_08AA92CC;
L_08AA92CC:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_13)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AA92F8;
    }
    goto L_08AA92E0;
L_08AA92E0:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9318;
      }
      goto L_08AA92F4;
    }
L_08AA92F4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    goto L_08AA92F8;
L_08AA92F8:
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_13)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
        goto L_08AA9320;
    }
    goto L_08AA9310;
L_08AA9310:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
      if (branch_taken) {
          goto L_08AA935C;
      }
      goto L_08AA9318;
    }
L_08AA9318:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9604;
      }
      goto L_08AA9320;
    }
L_08AA9320:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AA935C;
    }
    goto L_08AA9330;
L_08AA9330:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AA935C;
    }
    goto L_08AA9344;
L_08AA9344:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA95CC;
      }
      goto L_08AA9358;
    }
L_08AA9358:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    goto L_08AA935C;
L_08AA935C:
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_13 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_13)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
      if (branch_taken) {
          goto L_08AA95D4;
      }
      goto L_08AA937C;
    }
L_08AA937C:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    goto L_08AA9380;
L_08AA9380:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.gpr[21] = (2236u << 16u);
    ctx.fpr[19] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[19]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[22] = (ctx.gpr[21] + static_cast<std::uint32_t>(-8736));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[18]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (aot_gpr_6 << 16u);
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AA9438u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AA9438:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[20])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[16]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(28), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(44), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(60), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(20), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(21), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(22), static_cast<std::uint8_t>(aot_gpr_6));
    ctx.gpr[8] = (18176u << 16u);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(23), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_13 = std::bit_cast<float>(ctx.gpr[8]);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(36), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(37), static_cast<std::uint8_t>(aot_gpr_5));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.gpr[8] = (16128u << 16u);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(38), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_15 = std::bit_cast<float>(ctx.gpr[8]);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(39), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(52), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_14 = aot_fpr_14 + aot_fpr_15;
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(53), static_cast<std::uint8_t>(aot_gpr_5));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[16] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[16] = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(54), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(55), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.fpr[16] = ctx.fpr[16] + aot_fpr_15;
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[21] + static_cast<std::uint32_t>(-8736), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[18] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[18] = fs * ft; }
    ctx.fpr[17] = ctx.fpr[17] + aot_fpr_15;
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[19] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[19] = fs * ft; }
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    ctx.fpr[2] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    ctx.fpr[18] = ctx.fpr[18] + aot_fpr_15;
    ctx.fpr[19] = ctx.fpr[19] + aot_fpr_15;
    { const float fs = ctx.fpr[2]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[17] = ctx.fpr[0] + aot_fpr_15;
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[18]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = aot_fpr_13 + aot_fpr_15;
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[19]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_6 = (0u | 4u);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (0u | 5u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_7));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (ctx.gpr[22] | 0u);
    aot_gpr_31 = (0x08AA95C4u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(aot_gpr_7));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0186.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 107u, 0x08AA95C4u, 0x08AEE120u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0186_entry(rt, ctx, 486u, aot_mem);
#else
        recomp_unit_0186_entry(rt, ctx, 486u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0186_entry, 186u, 486u, 0x08AEE120u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA95C4u) goto L_08AA95C4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA95C4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9604;
      }
      goto L_08AA95CC;
    }
L_08AA95CC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9604;
      }
      goto L_08AA95D4;
    }
L_08AA95D4:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[18] <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
        goto L_08AA9380;
    }
    goto L_08AA95E4;
L_08AA95E4:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[17] <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
        goto L_08AA9380;
    }
    goto L_08AA95F4;
L_08AA95F4:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
        goto L_08AA9380;
    }
    goto L_08AA9604;
L_08AA9604:
    { std::uint32_t aot_run_words[8]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      ctx.gpr[22] = aot_run_words[6];
      aot_gpr_31 = aot_run_words[7];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA962C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-112));
    ctx.gpr[9] = (17536u << 16u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.gpr[9]);
    ctx.gpr[9] = (16585u << 16u);
    ctx.gpr[9] = (ctx.gpr[9] | 4059u);
    { const float fs = ctx.fpr[18]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[19] = std::bit_cast<float>(ctx.gpr[9]);
    ctx.fpr[17] = ctx.fpr[17] / ctx.fpr[19];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_7 << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[20]);
    ctx.gpr[20] = (aot_gpr_4 & 255u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_gpr_4 = (2247u << 16u);
    aot_gpr_7 = (aot_gpr_7 & 1023u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-8224));
    aot_gpr_7 = (aot_gpr_7 << 2u);
    ctx.fpr[18] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_7 + aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[19]);
    ctx.gpr[19] = (aot_gpr_5 & 255u);
    { const float fs = aot_fpr_14; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    aot_gpr_5 = (2247u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-12320));
    aot_gpr_5 = (aot_gpr_7 + aot_gpr_5);
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.fpr[2] = aot_fpr_12 - ctx.fpr[17];
    { const float fs = ctx.fpr[19]; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[17];
    ctx.fpr[3] = ctx.fpr[2] - ctx.fpr[0];
    ctx.fpr[1] = aot_fpr_13 - aot_fpr_14;
    ctx.fpr[4] = aot_fpr_12 + ctx.fpr[0];
    ctx.fpr[2] = ctx.fpr[2] + ctx.fpr[0];
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    aot_fpr_12 = aot_fpr_12 - ctx.fpr[0];
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(ctx.fpr[3]), std::bit_cast<std::uint32_t>(ctx.fpr[2]), std::bit_cast<std::uint32_t>(ctx.fpr[4])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    ctx.fpr[17] = aot_fpr_13 + aot_fpr_15;
    ctx.fpr[16] = ctx.fpr[1] + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = ctx.fpr[1] - aot_fpr_15;
    aot_fpr_13 = aot_fpr_13 - aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[18]);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_6 = (16256u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[16] = std::bit_cast<float>(aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), ctx.gpr[16]);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.gpr[17] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[17]) >> 16u));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_14)) ? 0x00800000u : 0u);
    ctx.gpr[16] = (ctx.gpr[8] & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    { const std::uint32_t aot_run_words[3]{ctx.gpr[21], ctx.gpr[22], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(88), aot_run_words); }
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_14));
      if (branch_taken) {
          goto L_08AA9790;
      }
      goto L_08AA9754;
    }
L_08AA9754:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9790;
      }
      goto L_08AA9768;
    }
L_08AA9768:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9790;
      }
      goto L_08AA977C;
    }
L_08AA977C:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA97AC;
      }
      goto L_08AA9790;
    }
L_08AA9790:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AA97B4;
    }
    goto L_08AA97A4;
L_08AA97A4:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
      if (branch_taken) {
          goto L_08AA97F0;
      }
      goto L_08AA97AC;
    }
L_08AA97AC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9AF8;
      }
      goto L_08AA97B4;
    }
L_08AA97B4:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AA97F0;
    }
    goto L_08AA97C4;
L_08AA97C4:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AA97F0;
    }
    goto L_08AA97D8;
L_08AA97D8:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9810;
      }
      goto L_08AA97EC;
    }
L_08AA97EC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    goto L_08AA97F0;
L_08AA97F0:
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
        goto L_08AA9818;
    }
    goto L_08AA9808;
L_08AA9808:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
      if (branch_taken) {
          goto L_08AA9854;
      }
      goto L_08AA9810;
    }
L_08AA9810:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9AF8;
      }
      goto L_08AA9818;
    }
L_08AA9818:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AA9854;
    }
    goto L_08AA9828;
L_08AA9828:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AA9854;
    }
    goto L_08AA983C;
L_08AA983C:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9AC0;
      }
      goto L_08AA9850;
    }
L_08AA9850:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    goto L_08AA9854;
L_08AA9854:
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
      if (branch_taken) {
          goto L_08AA9AC8;
      }
      goto L_08AA9874;
    }
L_08AA9874:
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    goto L_08AA9878;
L_08AA9878:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.gpr[21] = (2236u << 16u);
    ctx.fpr[19] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[19]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[22] = (ctx.gpr[21] + static_cast<std::uint32_t>(-8736));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[18]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (aot_gpr_6 << 16u);
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_6 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AA992Cu);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AA992C:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[20])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[16]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(28), static_cast<std::uint16_t>(aot_gpr_4));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(44), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(60), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(20), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(21), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(22), static_cast<std::uint8_t>(aot_gpr_6));
    ctx.gpr[8] = (18176u << 16u);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(23), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_13 = std::bit_cast<float>(ctx.gpr[8]);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(36), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(37), static_cast<std::uint8_t>(aot_gpr_5));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.gpr[8] = (16128u << 16u);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(38), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_15 = std::bit_cast<float>(ctx.gpr[8]);
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(39), static_cast<std::uint8_t>(aot_gpr_7));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(52), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_14 = aot_fpr_14 + aot_fpr_15;
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(53), static_cast<std::uint8_t>(aot_gpr_5));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[16] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[16] = fs * ft; }
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(54), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_mem.aot_direct_store8(ctx.gpr[22] + static_cast<std::uint32_t>(55), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.fpr[16] = ctx.fpr[16] + aot_fpr_15;
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[21] + static_cast<std::uint32_t>(-8736), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[16]));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[18] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[18] = fs * ft; }
    ctx.fpr[17] = ctx.fpr[17] + aot_fpr_15;
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[19] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[19] = fs * ft; }
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    ctx.fpr[2] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    ctx.fpr[18] = ctx.fpr[18] + aot_fpr_15;
    ctx.fpr[19] = ctx.fpr[19] + aot_fpr_15;
    { const float fs = ctx.fpr[2]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[17] = ctx.fpr[0] + aot_fpr_15;
    ctx.fpr[16] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[18]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = aot_fpr_13 + aot_fpr_15;
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[19]));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_6 = (0u | 4u);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (0u | 5u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_5));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_7));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (ctx.gpr[22] | 0u);
    aot_gpr_31 = (0x08AA9AB8u);
    aot_mem.aot_direct_store16(ctx.gpr[22] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(aot_gpr_7));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0186.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 135u, 0x08AA9AB8u, 0x08AEE120u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0186_entry(rt, ctx, 486u, aot_mem);
#else
        recomp_unit_0186_entry(rt, ctx, 486u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0186_entry, 186u, 486u, 0x08AEE120u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AA9AB8u) goto L_08AA9AB8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AA9AB8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9AF8;
      }
      goto L_08AA9AC0;
    }
L_08AA9AC0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AA9AF8;
      }
      goto L_08AA9AC8;
    }
L_08AA9AC8:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[18] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
        goto L_08AA9878;
    }
    goto L_08AA9AD8;
L_08AA9AD8:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[17] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
        goto L_08AA9878;
    }
    goto L_08AA9AE8;
L_08AA9AE8:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
        goto L_08AA9878;
    }
    goto L_08AA9AF8;
L_08AA9AF8:
    { std::uint32_t aot_run_words[8]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      ctx.gpr[22] = aot_run_words[6];
      aot_gpr_31 = aot_run_words[7];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA9B20:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-112));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_5 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), ctx.gpr[16]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[19]);
    ctx.gpr[16] = (aot_gpr_4 & 255u);
    ctx.gpr[19] = (aot_gpr_7 << 16u);
    aot_gpr_5 = (16128u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), std::bit_cast<std::uint32_t>(ctx.fpr[20]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[18]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(88), ctx.gpr[20]);
    ctx.fpr[20] = std::bit_cast<float>(aot_gpr_5);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    ctx.gpr[19] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[19]) >> 16u));
    ctx.gpr[20] = (ctx.gpr[8] & 255u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(92), ctx.gpr[21]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(96), aot_gpr_31);
    ctx.fpr[17] = aot_fpr_12 - aot_fpr_15;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), 0u);
    ctx.fpr[18] = aot_fpr_13 - ctx.fpr[16];
    aot_gpr_6 = (16256u << 16u);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    { const std::uint32_t aot_run_words[4]{std::bit_cast<std::uint32_t>(ctx.fpr[17]), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(ctx.fpr[18])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_fpr_12 = std::bit_cast<float>(0u);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_13), std::bit_cast<std::uint32_t>(aot_fpr_13), std::bit_cast<std::uint32_t>(ctx.fpr[18])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    aot_gpr_7 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    aot_gpr_5 = (48896u << 16u);
    ctx.fpr[18] = std::bit_cast<float>(aot_gpr_6);
    ctx.fpr[17] = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    ctx.fpr[19] = std::bit_cast<float>(aot_gpr_7);
    aot_gpr_5 = (0u | 0u);
    ctx.fpr[18] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[18])));
    ctx.fpr[19] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[19])));
    goto L_08AA9BF0;
L_08AA9BF0:
    ctx.fpr[0] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[0] < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[2] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(16)));
      if (branch_taken) {
          goto L_08AA9C18;
      }
      goto L_08AA9C04;
    }
L_08AA9C04:
    ctx.fpr[0] = ctx.fpr[0] / aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[1] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[1] = fs * ft; }
    ctx.fpr[0] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    goto L_08AA9C18;
L_08AA9C18:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[0] <= ctx.fpr[18])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9C40;
      }
      goto L_08AA9C28;
    }
L_08AA9C28:
    ctx.fpr[0] = ctx.fpr[0] - ctx.fpr[18];
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = ctx.fpr[0] / aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.fpr[0] = aot_fpr_13 - ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA9C40;
L_08AA9C40:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[2] < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9C64;
      }
      goto L_08AA9C50;
    }
L_08AA9C50:
    ctx.fpr[0] = ctx.fpr[2] / ctx.fpr[16];
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[2] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA9C64;
L_08AA9C64:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[2] <= ctx.fpr[19])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AA9C8C;
      }
      goto L_08AA9C74;
    }
L_08AA9C74:
    ctx.fpr[0] = ctx.fpr[2] - ctx.fpr[19];
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = ctx.fpr[0] / ctx.fpr[16];
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    ctx.fpr[0] = aot_fpr_13 - ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08AA9C8C;
L_08AA9C8C:
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(1));
    aot_gpr_7 = (static_cast<std::int32_t>(aot_gpr_5) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_7 != 0u;
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08AA9BF0;
      }
      goto L_08AA9C9C;
    }
L_08AA9C9C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    ctx.gpr[21] = (ctx.gpr[16] | 0u);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    ctx.gpr[16] = (aot_gpr_4 << 6u);
    aot_gpr_4 = (2238u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-272));
    ctx.gpr[16] = (ctx.gpr[16] + aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AA9D78u);
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AA9D78:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[21])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[20]));
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    ctx.gpr[11] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    ctx.gpr[10] = (ctx.gpr[16] + ctx.gpr[10]);
    aot_gpr_4 = (ctx.lo);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    goto L_08AA9DE4;
L_08AA9DE4:
    aot_mem.aot_direct_store16(ctx.gpr[10] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[11]));
    aot_mem.aot_direct_store8(ctx.gpr[10] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[10] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[10] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_7));
    aot_mem.aot_direct_store8(ctx.gpr[10] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(ctx.gpr[8]));
    ctx.gpr[9] = (ctx.gpr[9] + static_cast<std::uint32_t>(1));
    ctx.gpr[2] = (static_cast<std::int32_t>(ctx.gpr[9]) < 4 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[2] != 0u;
    ctx.gpr[10] = (ctx.gpr[10] + static_cast<std::uint32_t>(16));
      if (branch_taken) {
          goto L_08AA9DE4;
      }
      goto L_08AA9E08;
    }
L_08AA9E08:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_gpr_5 = (18176u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_5);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_5));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[20];
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_5));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_5));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[20];
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_5));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_5));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[20];
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_5));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[20];
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_7 = (2238u << 16u);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(12016));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_6));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_6 = (aot_gpr_4 << 2u);
    aot_gpr_6 = (aot_gpr_6 & 65535u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_7);
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[20];
    aot_gpr_7 = (aot_gpr_6 & 65535u);
    ctx.gpr[8] = (aot_gpr_7 + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[9] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_6));
    ctx.gpr[9] = (aot_gpr_7 + static_cast<std::uint32_t>(2));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AA9F34;
      }
      goto L_08AA9F2C;
    }
L_08AA9F2C:
    aot_gpr_31 = (0x08AA9F34u);
    goto L_08AA8BDC;
L_08AA9F34:
    { std::uint32_t aot_run_words[8]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.gpr[16] = aot_run_words[1];
      ctx.gpr[17] = aot_run_words[2];
      ctx.gpr[18] = aot_run_words[3];
      ctx.gpr[19] = aot_run_words[4];
      ctx.gpr[20] = aot_run_words[5];
      ctx.gpr[21] = aot_run_words[6];
      aot_gpr_31 = aot_run_words[7];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AA9F5C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-96));
    { const std::uint32_t aot_run_words[7]{ctx.gpr[16], ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words); }
    ctx.fpr[17] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[16] = (aot_gpr_4 & 255u);
    ctx.gpr[17] = (aot_gpr_5 & 255u);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    ctx.gpr[19] = (aot_gpr_7 << 16u);
    ctx.gpr[19] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[19]) >> 16u));
    ctx.gpr[20] = (ctx.gpr[8] & 255u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), 0u);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[19] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[18] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[19]) ^ 0x80000000u);
    ctx.fpr[18] = ctx.fpr[18] - aot_fpr_14;
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[18]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = aot_fpr_12 + ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fpr[2] = aot_fpr_14 - ctx.fpr[19];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[1] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[1] = fs * ft; }
    ctx.fpr[1] = aot_fpr_12 + ctx.fpr[1];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    ctx.fpr[1] = ctx.fpr[19] + aot_fpr_14;
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    ctx.fpr[3] = aot_fpr_12 + ctx.fpr[3];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    aot_fpr_14 = ctx.fpr[19] - aot_fpr_14;
    { const float fs = aot_fpr_15; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[18]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[17]));
      if (branch_taken) {
          goto L_08AAA0C4;
      }
      goto L_08AAA088;
    }
L_08AAA088:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA0C4;
      }
      goto L_08AAA09C;
    }
L_08AAA09C:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA0C4;
      }
      goto L_08AAA0B0;
    }
L_08AAA0B0:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA0E0;
      }
      goto L_08AAA0C4;
    }
L_08AAA0C4:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAA0E8;
    }
    goto L_08AAA0D8;
L_08AAA0D8:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
      if (branch_taken) {
          goto L_08AAA124;
      }
      goto L_08AAA0E0;
    }
L_08AAA0E0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA488;
      }
      goto L_08AAA0E8;
    }
L_08AAA0E8:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAA124;
    }
    goto L_08AAA0F8;
L_08AAA0F8:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAA124;
    }
    goto L_08AAA10C;
L_08AAA10C:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA144;
      }
      goto L_08AAA120;
    }
L_08AAA120:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    goto L_08AAA124;
L_08AAA124:
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
        goto L_08AAA14C;
    }
    goto L_08AAA13C;
L_08AAA13C:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
      if (branch_taken) {
          goto L_08AAA188;
      }
      goto L_08AAA144;
    }
L_08AAA144:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA488;
      }
      goto L_08AAA14C;
    }
L_08AAA14C:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAA188;
    }
    goto L_08AAA15C;
L_08AAA15C:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAA188;
    }
    goto L_08AAA170;
L_08AAA170:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA1A8;
      }
      goto L_08AAA184;
    }
L_08AAA184:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    goto L_08AAA188;
L_08AAA188:
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAA1B0;
    }
    goto L_08AAA1A0;
L_08AAA1A0:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
      if (branch_taken) {
          goto L_08AAA1F0;
      }
      goto L_08AAA1A8;
    }
L_08AAA1A8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA488;
      }
      goto L_08AAA1B0;
    }
L_08AAA1B0:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA1F0;
    }
    goto L_08AAA1C0;
L_08AAA1C0:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA1F0;
    }
    goto L_08AAA1D4;
L_08AAA1D4:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA1F0;
    }
    goto L_08AAA1E8;
L_08AAA1E8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA488;
      }
      goto L_08AAA1F0;
    }
L_08AAA1F0:
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[21] = (ctx.gpr[16] | 0u);
    ctx.gpr[16] = (aot_gpr_4 << 6u);
    aot_gpr_4 = (2238u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-272));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[16] = (ctx.gpr[16] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AAA2C4u);
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AAA2C4:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[21])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[20]));
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    ctx.gpr[10] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    ctx.gpr[9] = (ctx.gpr[16] + ctx.gpr[9]);
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    goto L_08AAA32C;
L_08AAA32C:
    aot_mem.aot_direct_store16(ctx.gpr[9] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.gpr[8] = (ctx.gpr[8] + static_cast<std::uint32_t>(1));
    ctx.gpr[11] = (static_cast<std::int32_t>(ctx.gpr[8]) < 4 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[11] != 0u;
    ctx.gpr[9] = (ctx.gpr[9] + static_cast<std::uint32_t>(16));
      if (branch_taken) {
          goto L_08AAA32C;
      }
      goto L_08AAA350;
    }
L_08AAA350:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_gpr_4 = (18176u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (16128u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_7 = (2238u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_6));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_6 = (aot_gpr_4 << 2u);
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(12016));
    aot_gpr_6 = (aot_gpr_6 & 65535u);
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_7);
    aot_gpr_7 = (aot_gpr_6 & 65535u);
    ctx.gpr[9] = (aot_gpr_7 + static_cast<std::uint32_t>(2));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    ctx.gpr[8] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(ctx.gpr[8]));
    ctx.gpr[8] = (aot_gpr_7 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAA488;
      }
      goto L_08AAA480;
    }
L_08AAA480:
    aot_gpr_31 = (0x08AAA488u);
    goto L_08AA8BDC;
L_08AAA488:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAA4AC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-96));
    ctx.gpr[9] = (17536u << 16u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.gpr[9]);
    ctx.gpr[9] = (16585u << 16u);
    ctx.gpr[9] = (ctx.gpr[9] | 4059u);
    { const float fs = ctx.fpr[18]; const float ft = ctx.fpr[17]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[19] = std::bit_cast<float>(ctx.gpr[9]);
    ctx.fpr[17] = ctx.fpr[17] / ctx.fpr[19];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[19]);
    ctx.gpr[19] = (aot_gpr_7 << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), ctx.gpr[16]);
    ctx.gpr[16] = (aot_gpr_4 & 255u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[17]));
    aot_gpr_7 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_gpr_4 = (2247u << 16u);
    aot_gpr_7 = (aot_gpr_7 & 1023u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-8224));
    aot_gpr_7 = (aot_gpr_7 << 2u);
    ctx.fpr[18] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), 0u);
    aot_gpr_4 = (aot_gpr_7 + aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_5 & 255u);
    { const float fs = aot_fpr_14; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    aot_gpr_5 = (2247u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-12320));
    aot_gpr_5 = (aot_gpr_7 + aot_gpr_5);
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.fpr[2] = aot_fpr_12 - ctx.fpr[17];
    { const float fs = ctx.fpr[19]; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[17];
    ctx.fpr[3] = ctx.fpr[2] - ctx.fpr[0];
    ctx.fpr[1] = aot_fpr_13 - aot_fpr_14;
    ctx.fpr[4] = aot_fpr_12 + ctx.fpr[0];
    ctx.fpr[2] = ctx.fpr[2] + ctx.fpr[0];
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_15; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    aot_fpr_12 = aot_fpr_12 - ctx.fpr[0];
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(ctx.fpr[3]), std::bit_cast<std::uint32_t>(ctx.fpr[2]), std::bit_cast<std::uint32_t>(ctx.fpr[4])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    ctx.fpr[17] = aot_fpr_13 + aot_fpr_15;
    ctx.fpr[16] = ctx.fpr[1] + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = ctx.fpr[1] - aot_fpr_15;
    aot_fpr_13 = aot_fpr_13 - aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[18]);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_6 = (16256u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[16] = std::bit_cast<float>(aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[20]);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[18]));
    ctx.gpr[19] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[19]) >> 16u));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_14)) ? 0x00800000u : 0u);
    ctx.gpr[20] = (ctx.gpr[8] & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(88), ctx.gpr[21]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(92), aot_gpr_31);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(aot_fpr_14));
      if (branch_taken) {
          goto L_08AAA610;
      }
      goto L_08AAA5D4;
    }
L_08AAA5D4:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA610;
      }
      goto L_08AAA5E8;
    }
L_08AAA5E8:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA610;
      }
      goto L_08AAA5FC;
    }
L_08AAA5FC:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA62C;
      }
      goto L_08AAA610;
    }
L_08AAA610:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAA634;
    }
    goto L_08AAA624;
L_08AAA624:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
      if (branch_taken) {
          goto L_08AAA670;
      }
      goto L_08AAA62C;
    }
L_08AAA62C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA9D4;
      }
      goto L_08AAA634;
    }
L_08AAA634:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAA670;
    }
    goto L_08AAA644;
L_08AAA644:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAA670;
    }
    goto L_08AAA658;
L_08AAA658:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA690;
      }
      goto L_08AAA66C;
    }
L_08AAA66C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    goto L_08AAA670;
L_08AAA670:
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
        goto L_08AAA698;
    }
    goto L_08AAA688;
L_08AAA688:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
      if (branch_taken) {
          goto L_08AAA6D4;
      }
      goto L_08AAA690;
    }
L_08AAA690:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA9D4;
      }
      goto L_08AAA698;
    }
L_08AAA698:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAA6D4;
    }
    goto L_08AAA6A8;
L_08AAA6A8:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAA6D4;
    }
    goto L_08AAA6BC;
L_08AAA6BC:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAA6F4;
      }
      goto L_08AAA6D0;
    }
L_08AAA6D0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    goto L_08AAA6D4;
L_08AAA6D4:
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAA6FC;
    }
    goto L_08AAA6EC;
L_08AAA6EC:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
      if (branch_taken) {
          goto L_08AAA73C;
      }
      goto L_08AAA6F4;
    }
L_08AAA6F4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA9D4;
      }
      goto L_08AAA6FC;
    }
L_08AAA6FC:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA73C;
    }
    goto L_08AAA70C;
L_08AAA70C:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA73C;
    }
    goto L_08AAA720;
L_08AAA720:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAA73C;
    }
    goto L_08AAA734;
L_08AAA734:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAA9D4;
      }
      goto L_08AAA73C;
    }
L_08AAA73C:
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[21] = (ctx.gpr[16] | 0u);
    ctx.gpr[16] = (aot_gpr_4 << 6u);
    aot_gpr_4 = (2238u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-272));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[16] = (ctx.gpr[16] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AAA810u);
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AAA810:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[21])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[20]));
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_4 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 8u));
    ctx.gpr[10] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    ctx.gpr[9] = (ctx.gpr[16] + ctx.gpr[9]);
    aot_gpr_5 = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[18])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[19])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_5) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (ctx.lo);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 8u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_6));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_gpr_7 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    goto L_08AAA878;
L_08AAA878:
    aot_mem.aot_direct_store16(ctx.gpr[9] + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(ctx.gpr[10]));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[9] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_7));
    ctx.gpr[8] = (ctx.gpr[8] + static_cast<std::uint32_t>(1));
    ctx.gpr[11] = (static_cast<std::int32_t>(ctx.gpr[8]) < 4 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[11] != 0u;
    ctx.gpr[9] = (ctx.gpr[9] + static_cast<std::uint32_t>(16));
      if (branch_taken) {
          goto L_08AAA878;
      }
      goto L_08AAA89C;
    }
L_08AAA89C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_gpr_4 = (18176u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (16128u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + aot_fpr_14;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = ctx.fpr[16]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_7 = (2238u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_6));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_6 = (aot_gpr_4 << 2u);
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(12016));
    aot_gpr_6 = (aot_gpr_6 & 65535u);
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_7);
    aot_gpr_7 = (aot_gpr_6 & 65535u);
    ctx.gpr[9] = (aot_gpr_7 + static_cast<std::uint32_t>(2));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    ctx.gpr[8] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(ctx.gpr[8]));
    ctx.gpr[8] = (aot_gpr_7 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAA9D4;
      }
      goto L_08AAA9CC;
    }
L_08AAA9CC:
    aot_gpr_31 = (0x08AAA9D4u);
    goto L_08AA8BDC;
L_08AAA9D4:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAA9F8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-128));
    { const std::uint32_t aot_run_words[15]{std::bit_cast<std::uint32_t>(ctx.fpr[20]), std::bit_cast<std::uint32_t>(ctx.fpr[22]), std::bit_cast<std::uint32_t>(ctx.fpr[24]), std::bit_cast<std::uint32_t>(ctx.fpr[26]), std::bit_cast<std::uint32_t>(ctx.fpr[28]), std::bit_cast<std::uint32_t>(ctx.fpr[30]), ctx.gpr[16], ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words); }
    ctx.gpr[16] = (aot_gpr_4 & 255u);
    ctx.gpr[17] = (aot_gpr_5 & 255u);
    ctx.gpr[18] = (aot_gpr_6 & 255u);
    ctx.gpr[19] = (aot_gpr_7 & 255u);
    ctx.gpr[20] = (ctx.gpr[8] & 255u);
    ctx.gpr[21] = (ctx.gpr[9] & 255u);
    ctx.gpr[22] = (ctx.gpr[10] & 255u);
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(128)));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2984), 0u);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[24] = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[19]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[26] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[19] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[26]) ^ 0x80000000u);
    ctx.fpr[19] = ctx.fpr[19] - ctx.fpr[24];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[19]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    ctx.fpr[0] = aot_fpr_12 + ctx.fpr[0];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fpr[2] = ctx.fpr[24] - ctx.fpr[26];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[1] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[1] = fs * ft; }
    ctx.fpr[1] = aot_fpr_12 + ctx.fpr[1];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(ctx.fpr[1]));
    ctx.fpr[1] = ctx.fpr[26] + ctx.fpr[24];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    ctx.fpr[3] = aot_fpr_12 + ctx.fpr[3];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    ctx.fpr[3] = ctx.fpr[26] - ctx.fpr[24];
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_15;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[2]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[1]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = ctx.fpr[16]; const float ft = ctx.fpr[19]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.fpr[28] = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[28]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[28]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(ctx.fpr[28]));
    aot_gpr_4 = (16256u << 16u);
    ctx.fpr[30] = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(ctx.fpr[30]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(ctx.fpr[30]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(ctx.fpr[30]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(ctx.fpr[30]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(ctx.fpr[28]));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[20] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < ctx.fpr[28])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[18]));
      if (branch_taken) {
          goto L_08AAAB8C;
      }
      goto L_08AAAB50;
    }
L_08AAAB50:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[28])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAAB8C;
      }
      goto L_08AAAB64;
    }
L_08AAAB64:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[28])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAAB8C;
      }
      goto L_08AAAB78;
    }
L_08AAAB78:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[28])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAABA8;
      }
      goto L_08AAAB8C;
    }
L_08AAAB8C:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[28])) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAABB0;
    }
    goto L_08AAABA0;
L_08AAABA0:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
      if (branch_taken) {
          goto L_08AAABEC;
      }
      goto L_08AAABA8;
    }
L_08AAABA8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB1A8;
      }
      goto L_08AAABB0;
    }
L_08AAABB0:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < ctx.fpr[28])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAABEC;
    }
    goto L_08AAABC0;
L_08AAABC0:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < ctx.fpr[28])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
        goto L_08AAABEC;
    }
    goto L_08AAABD4;
L_08AAABD4:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 < ctx.fpr[28])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAAC0C;
      }
      goto L_08AAABE8;
    }
L_08AAABE8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8744)));
    goto L_08AAABEC;
L_08AAABEC:
    aot_fpr_15 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_15 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_15)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_15)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
        goto L_08AAAC14;
    }
    goto L_08AAAC04;
L_08AAAC04:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
      if (branch_taken) {
          goto L_08AAAC50;
      }
      goto L_08AAAC0C;
    }
L_08AAAC0C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB1A8;
      }
      goto L_08AAAC14;
    }
L_08AAAC14:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_15)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAAC50;
    }
    goto L_08AAAC24;
L_08AAAC24:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_15)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
        goto L_08AAAC50;
    }
    goto L_08AAAC38;
L_08AAAC38:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_15)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAAC74;
      }
      goto L_08AAAC4C;
    }
L_08AAAC4C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-8740)));
    goto L_08AAAC50;
L_08AAAC50:
    aot_fpr_15 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_14)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
        goto L_08AAAC7C;
    }
    goto L_08AAAC6C;
L_08AAAC6C:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
      if (branch_taken) {
          goto L_08AAACBC;
      }
      goto L_08AAAC74;
    }
L_08AAAC74:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB1A8;
      }
      goto L_08AAAC7C;
    }
L_08AAAC7C:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAACBC;
    }
    goto L_08AAAC8C;
L_08AAAC8C:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAACBC;
    }
    goto L_08AAACA0;
L_08AAACA0:
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_15 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
        goto L_08AAACBC;
    }
    goto L_08AAACB4;
L_08AAACB4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB1A8;
      }
      goto L_08AAACBC;
    }
L_08AAACBC:
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[23] = (ctx.gpr[16] | 0u);
    ctx.gpr[16] = (aot_gpr_4 << 6u);
    aot_gpr_4 = (2238u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-272));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[16] = (ctx.gpr[16] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(24), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(40), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(56), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(26), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(42), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (aot_gpr_4 << 16u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 16u));
    aot_gpr_31 = (0x08AAAD90u);
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(58), static_cast<std::uint16_t>(aot_gpr_4));
    goto L_08AA8278;
L_08AAAD90:
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_7 = (16128u << 16u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[26]) ^ 0x80000000u);
    aot_gpr_5 = (0u | 0u);
    ctx.fpr[17] = std::bit_cast<float>(ctx.gpr[23]);
    aot_gpr_4 = (0u | 0u);
    ctx.fpr[18] = std::bit_cast<float>(ctx.gpr[19]);
    aot_gpr_5 = (ctx.gpr[16] + aot_gpr_5);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    ctx.fpr[19] = std::bit_cast<float>(ctx.gpr[17]);
    ctx.fpr[0] = std::bit_cast<float>(ctx.gpr[20]);
    ctx.fpr[2] = std::bit_cast<float>(ctx.gpr[18]);
    ctx.fpr[1] = std::bit_cast<float>(ctx.gpr[21]);
    aot_fpr_12 = aot_fpr_12 - ctx.fpr[24];
    aot_fpr_13 = ctx.fpr[24] - ctx.fpr[26];
    aot_fpr_14 = ctx.fpr[26] + ctx.fpr[24];
    aot_fpr_15 = ctx.fpr[26] - ctx.fpr[24];
    ctx.fpr[16] = std::bit_cast<float>(aot_gpr_7);
    ctx.fpr[17] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[17])));
    ctx.fpr[18] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[18])));
    ctx.fpr[19] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[19])));
    ctx.fpr[0] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[0])));
    ctx.fpr[2] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[2])));
    ctx.fpr[1] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[1])));
    goto L_08AAADF0;
L_08AAADF0:
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(12), static_cast<std::uint16_t>(aot_gpr_6));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_7 = (static_cast<std::int32_t>(aot_gpr_4) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_7 != 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16));
      if (branch_taken) {
          goto L_08AAADF0;
      }
      goto L_08AAAE04;
    }
L_08AAAE04:
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[4] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[4] = fs * ft; }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(67), static_cast<std::uint8_t>(ctx.gpr[22]));
    ctx.gpr[22] = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(67)));
    ctx.fpr[3] = ctx.fpr[3] + ctx.fpr[4];
    { const float fs = ctx.fpr[3]; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    ctx.fpr[3] = ctx.fpr[3] + ctx.fpr[16];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[3] <= ctx.fpr[28])) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[3] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[28]));
        goto L_08AAAE30;
    }
    goto L_08AAAE30;
L_08AAAE30:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[3] <= ctx.fpr[30])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[3] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[30]));
        goto L_08AAAE40;
    }
    goto L_08AAAE40;
L_08AAAE40:
    ctx.fpr[4] = ctx.fpr[30] - ctx.fpr[3];
    { const float fs = ctx.fpr[17]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[5] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[5] = fs * ft; }
    { const float fs = ctx.fpr[19]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[6] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[6] = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = ctx.fpr[4]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[7] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[7] = fs * ft; }
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[4]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[8] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[8] = fs * ft; }
    { const float fs = ctx.fpr[2]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[5] = ctx.fpr[5] + ctx.fpr[7];
    { const float fs = ctx.fpr[1]; const float ft = ctx.fpr[4]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[4] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[4] = fs * ft; }
    ctx.fpr[6] = ctx.fpr[6] + ctx.fpr[8];
    ctx.fpr[5] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[5]));
    ctx.fpr[3] = ctx.fpr[3] + ctx.fpr[4];
    ctx.fpr[6] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[6]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[5]));
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[4] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[4] = fs * ft; }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    ctx.fpr[3] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[3]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[6]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[4];
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    { const float fs = aot_fpr_13; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(ctx.gpr[22]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= ctx.fpr[28])) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[28]));
        goto L_08AAAECC;
    }
    goto L_08AAAECC;
L_08AAAECC:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= ctx.fpr[30])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[30]));
        goto L_08AAAEDC;
    }
    goto L_08AAAEDC;
L_08AAAEDC:
    ctx.fpr[3] = ctx.fpr[30] - aot_fpr_13;
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[4] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[4] = fs * ft; }
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[5] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[5] = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[6] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[6] = fs * ft; }
    { const float fs = ctx.fpr[0]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[7] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[7] = fs * ft; }
    { const float fs = ctx.fpr[2]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.fpr[4] = ctx.fpr[4] + ctx.fpr[6];
    { const float fs = ctx.fpr[1]; const float ft = ctx.fpr[3]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    ctx.fpr[5] = ctx.fpr[5] + ctx.fpr[7];
    ctx.fpr[4] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[4]));
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[3];
    ctx.fpr[5] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[5]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[4]));
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[5]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[3];
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(20), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(21), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(22), static_cast<std::uint8_t>(aot_gpr_6));
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[16];
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(23), static_cast<std::uint8_t>(ctx.gpr[22]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 <= ctx.fpr[28])) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_14 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[28]));
        goto L_08AAAF68;
    }
    goto L_08AAAF68;
L_08AAAF68:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 <= ctx.fpr[30])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_14 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[30]));
        goto L_08AAAF78;
    }
    goto L_08AAAF78;
L_08AAAF78:
    aot_fpr_13 = ctx.fpr[30] - aot_fpr_14;
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[3] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[3] = fs * ft; }
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[4] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[4] = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[5] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[5] = fs * ft; }
    { const float fs = ctx.fpr[0]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[6] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[6] = fs * ft; }
    { const float fs = ctx.fpr[2]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    { const float fs = aot_fpr_15; const float ft = ctx.fpr[20]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    ctx.fpr[3] = ctx.fpr[3] + ctx.fpr[5];
    { const float fs = ctx.fpr[1]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[4] = ctx.fpr[4] + ctx.fpr[6];
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    ctx.fpr[3] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[3]));
    aot_fpr_13 = aot_fpr_14 + aot_fpr_13;
    ctx.fpr[4] = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[4]));
    aot_fpr_12 = aot_fpr_15 + aot_fpr_12;
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[3]));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[4]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[16];
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(36), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(37), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(38), static_cast<std::uint8_t>(aot_gpr_6));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= ctx.fpr[28])) ? 0x00800000u : 0u);
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(39), static_cast<std::uint8_t>(ctx.gpr[22]));
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[28]));
        goto L_08AAB000;
    }
    goto L_08AAB000;
L_08AAB000:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= ctx.fpr[30])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[30]));
        goto L_08AAB010;
    }
    goto L_08AAB010;
L_08AAB010:
    aot_fpr_13 = ctx.fpr[30] - aot_fpr_12;
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    { const float fs = ctx.fpr[18]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[18] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[18] = fs * ft; }
    { const float fs = ctx.fpr[0]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[0] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[0] = fs * ft; }
    { const float fs = ctx.fpr[2]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[18];
    { const float fs = ctx.fpr[1]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + ctx.fpr[0];
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_15 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(64), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_15));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(65), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(66), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(65)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(66)));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(52), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(53), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(54), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store8(ctx.gpr[16] + static_cast<std::uint32_t>(55), static_cast<std::uint8_t>(ctx.gpr[22]));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_gpr_4 = (18176u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[16];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_15 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_15 = fs * ft; }
    aot_fpr_15 = aot_fpr_15 + ctx.fpr[16];
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_15));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[16];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(16), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(18), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[16];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(32), static_cast<std::uint16_t>(aot_gpr_4));
    aot_fpr_15 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = aot_fpr_15; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(34), static_cast<std::uint16_t>(aot_gpr_4));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992)));
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[16];
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_4);
    aot_gpr_5 = (aot_gpr_4 + aot_gpr_5);
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_7 = (2238u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(48), static_cast<std::uint16_t>(aot_gpr_6));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_6 = (aot_gpr_4 << 2u);
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(12016));
    aot_gpr_6 = (aot_gpr_6 & 65535u);
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[16];
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_7);
    aot_gpr_7 = (aot_gpr_6 & 65535u);
    ctx.gpr[9] = (aot_gpr_7 + static_cast<std::uint32_t>(2));
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    ctx.gpr[8] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store16(ctx.gpr[16] + static_cast<std::uint32_t>(50), static_cast<std::uint16_t>(ctx.gpr[8]));
    ctx.gpr[8] = (aot_gpr_7 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(2), static_cast<std::uint16_t>(ctx.gpr[8]));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(3));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(4), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(6), static_cast<std::uint16_t>(aot_gpr_7));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(8), static_cast<std::uint16_t>(aot_gpr_6));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(10), static_cast<std::uint16_t>(ctx.gpr[9]));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2992), aot_gpr_4);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 96 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB1A8;
      }
      goto L_08AAB1A0;
    }
L_08AAB1A0:
    aot_gpr_31 = (0x08AAB1A8u);
    goto L_08AA8BDC;
L_08AAB1A8:
    { std::uint32_t aot_run_words[15]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      ctx.fpr[26] = std::bit_cast<float>(aot_run_words[3]);
      ctx.fpr[28] = std::bit_cast<float>(aot_run_words[4]);
      ctx.fpr[30] = std::bit_cast<float>(aot_run_words[5]);
      ctx.gpr[16] = aot_run_words[6];
      ctx.gpr[17] = aot_run_words[7];
      ctx.gpr[18] = aot_run_words[8];
      ctx.gpr[19] = aot_run_words[9];
      ctx.gpr[20] = aot_run_words[10];
      ctx.gpr[21] = aot_run_words[11];
      ctx.gpr[22] = aot_run_words[12];
      ctx.gpr[23] = aot_run_words[13];
      aot_gpr_31 = aot_run_words[14];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(128));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB1EC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[16]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08AAB200u);
    ctx.gpr[16] = (aot_gpr_4 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B58974, 213u, 150u, 0x08B58974u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0213_entry, 213u, 150u, 0x08B58974u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB200u) goto L_08AAB200;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB200:
    aot_gpr_4 = (ctx.gpr[16] | 0u);
    aot_gpr_31 = (0x08AAB20Cu);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    goto L_08AAB21C;
L_08AAB20C:
    ctx.gpr[16] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB21C:
    aot_gpr_7 = (aot_gpr_4 | 0u);
    ctx.gpr[8] = (aot_gpr_5 | 0u);
    aot_gpr_4 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_6 = (0u | 0u);
    { const bool branch_taken = ctx.gpr[8] == 0u;
    aot_gpr_5 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_08AAB260;
      }
      goto L_08AAB234;
    }
L_08AAB234:
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_7 + static_cast<std::uint32_t>(0)));
    ctx.gpr[9] = (aot_gpr_4 | 0u);
    aot_gpr_6 = (aot_gpr_6 + ctx.gpr[8]);
    ctx.gpr[8] = (aot_gpr_6 << 10u);
    aot_gpr_6 = (aot_gpr_6 + ctx.gpr[8]);
    ctx.gpr[8] = (aot_gpr_6 >> 6u);
    aot_gpr_4 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_7 = (aot_gpr_7 + static_cast<std::uint32_t>(1));
    aot_gpr_6 = (aot_gpr_6 ^ ctx.gpr[8]);
    { const bool branch_taken = ctx.gpr[9] != 0u;
    aot_gpr_5 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_08AAB234;
      }
      goto L_08AAB260;
    }
L_08AAB260:
    aot_gpr_4 = (aot_gpr_6 << 3u);
    aot_gpr_4 = (aot_gpr_6 + aot_gpr_4);
    aot_gpr_5 = (aot_gpr_4 >> 11u);
    ctx.gpr[2] = (aot_gpr_4 ^ aot_gpr_5);
    aot_gpr_4 = (ctx.gpr[2] << 15u);
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (ctx.gpr[2] + aot_gpr_4);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB27C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(23248));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_31);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_08AAB2B8;
      }
      goto L_08AAB298;
    }
L_08AAB298:
    aot_gpr_5 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-2935)));
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08AAB2B8;
      }
      goto L_08AAB2A4;
    }
L_08AAB2A4:
    aot_gpr_31 = (0x08AAB2ACu);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    ctx.pc = 0x08B736A4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB2AC:
    aot_gpr_4 = (0u | 1u);
    { const bool branch_taken = ctx.gpr[2] == aot_gpr_4;
      if (branch_taken) {
          goto L_08AAB2E8;
      }
      goto L_08AAB2B8;
    }
L_08AAB2B8:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956)));
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08AAB2DC;
      }
      goto L_08AAB2C8;
    }
L_08AAB2C8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AAB2F0;
      }
      goto L_08AAB2D4;
    }
L_08AAB2D4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB314;
      }
      goto L_08AAB2DC;
    }
L_08AAB2DC:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940), 0u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_08AAB350;
      }
      goto L_08AAB2E8;
    }
L_08AAB2E8:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB350;
      }
      goto L_08AAB2F0;
    }
L_08AAB2F0:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08AAB304;
      }
      goto L_08AAB2F8;
    }
L_08AAB2F8:
    aot_gpr_31 = (0x08AAB300u);
    aot_gpr_4 = (aot_gpr_5 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 275u, 0x08AAB300u, 0x08939138u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 278u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 278u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 278u, 0x08939138u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB300u) goto L_08AAB300;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB300:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940)));
    goto L_08AAB304;
L_08AAB304:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940), aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB350;
      }
      goto L_08AAB314;
    }
L_08AAB314:
    aot_gpr_5 = (0u | 1u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08AAB348;
      }
      goto L_08AAB320;
    }
L_08AAB320:
    aot_gpr_31 = (0x08AAB328u);
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(8));
    goto L_08AAB5BC;
L_08AAB328:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AAB340;
      }
      goto L_08AAB330;
    }
L_08AAB330:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956), 0u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940), 0u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_08AAB350;
      }
      goto L_08AAB340;
    }
L_08AAB340:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB350;
      }
      goto L_08AAB348;
    }
L_08AAB348:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2940), 0u);
    ctx.gpr[2] = (0u | 0u);
    goto L_08AAB350;
L_08AAB350:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB35C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-288));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(260), ctx.gpr[17]);
    ctx.gpr[17] = (2237u << 16u);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(23248));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(20)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(256), ctx.gpr[16]);
    ctx.gpr[16] = (0u | 111u);
    { const std::uint32_t aot_run_words[3]{ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(264), aot_run_words); }
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[16];
    aot_gpr_4 = (17235u << 16u);
      if (branch_taken) {
          goto L_08AAB424;
      }
      goto L_08AAB38C;
    }
L_08AAB38C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB3A4;
      }
      goto L_08AAB398;
    }
L_08AAB398:
    aot_gpr_31 = (0x08AAB3A0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 287u, 0x08AAB3A0u, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB3A0u) goto L_08AAB3A0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB3A0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    goto L_08AAB3A4;
L_08AAB3A4:
    aot_gpr_31 = (0x08AAB3ACu);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0044_entry, 44u, 439u, 0x088B666Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB3ACu) goto L_08AAB3AC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB3AC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(4)));
    aot_gpr_31 = (0x08AAB3B8u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 290u, 0x08AAB3B8u, 0x08B5831Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 63u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 63u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 63u, 0x08B5831Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB3B8u) goto L_08AAB3B8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB3B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB3D0;
      }
      goto L_08AAB3C4;
    }
L_08AAB3C4:
    aot_gpr_31 = (0x08AAB3CCu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 292u, 0x08AAB3CCu, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB3CCu) goto L_08AAB3CC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB3CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    goto L_08AAB3D0;
L_08AAB3D0:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(52)));
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(24), aot_gpr_5);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x08AAB3E8u);
    aot_gpr_6 = (0u | 511u);
    ctx.pc = 0x08B73694u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB3E8:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[2]) >= 0;
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(8), ctx.gpr[2]);
      if (branch_taken) {
          goto L_08AAB4B4;
      }
      goto L_08AAB3F0;
    }
L_08AAB3F0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB408;
      }
      goto L_08AAB3FC;
    }
L_08AAB3FC:
    aot_gpr_31 = (0x08AAB404u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 297u, 0x08AAB404u, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB404u) goto L_08AAB404;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB404:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    goto L_08AAB408;
L_08AAB408:
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(56), aot_gpr_5);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AAB41Cu);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB41C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB5A0;
      }
      goto L_08AAB424;
    }
L_08AAB424:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(18756));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_4);
    aot_gpr_4 = (20527u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(14896));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_4);
    aot_gpr_4 = (18271u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(20563));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_4);
    aot_gpr_4 = (12101u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(19777));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_4);
    aot_gpr_4 = (17490u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(21333));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_4);
    aot_gpr_4 = (47u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(21065));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 3u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (2232u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-552));
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_31 = (0x08AAB48Cu);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 301u, 0x08AAB48Cu, 0x08B58238u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 45u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 45u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 45u, 0x08B58238u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB48Cu) goto L_08AAB48C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB48C:
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-18560), static_cast<std::uint8_t>(0u));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB520;
      }
      goto L_08AAB49C;
    }
L_08AAB49C:
    aot_gpr_31 = (0x08AAB4A4u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 303u, 0x08AAB4A4u, 0x08938F04u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 250u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 250u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 250u, 0x08938F04u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB4A4u) goto L_08AAB4A4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB4A4:
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(4), ctx.gpr[2]);
    aot_gpr_4 = (0u | 1u);
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-18560), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_08AAB518;
      }
      goto L_08AAB4B4;
    }
L_08AAB4B4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(20)));
    ctx.gpr[18] = (2240u << 16u);
    ctx.gpr[19] = (0u | 1u);
    aot_gpr_5 = (static_cast<std::int32_t>(aot_gpr_4) < 9 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(-4992));
      if (branch_taken) {
          goto L_08AAB528;
      }
      goto L_08AAB4CC;
    }
L_08AAB4CC:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_5 = (0u | 0u);
    aot_gpr_31 = (0x08AAB4DCu);
    aot_gpr_6 = (4u << 16u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B5814C, 213u, 28u, 0x08B5814Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_7, ctx.gpr[8], ctx.gpr[9], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0213_entry, 213u, 28u, 0x08B5814Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB4DCu) goto L_08AAB4DC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB4DC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(20)));
    aot_gpr_6 = (2240u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 11u);
    aot_gpr_5 = (0u + aot_gpr_5);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-23432));
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08AAB500u);
    aot_gpr_6 = (0u | 2048u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 307u, 0x08AAB500u, 0x089645DCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 36u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 36u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 36u, 0x089645DCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB500u) goto L_08AAB500;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB500:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (0u | 2048u);
    aot_gpr_31 = (0x08AAB510u);
    aot_gpr_6 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 308u, 0x08AAB510u, 0x08938F7Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 258u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 258u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 258u, 0x08938F7Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB510u) goto L_08AAB510;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB510:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936), static_cast<std::uint8_t>(ctx.gpr[19]));
      if (branch_taken) {
          goto L_08AAB59C;
      }
      goto L_08AAB518;
    }
L_08AAB518:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB5A0;
      }
      goto L_08AAB520;
    }
L_08AAB520:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB5A0;
      }
      goto L_08AAB528;
    }
L_08AAB528:
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[16];
      if (branch_taken) {
          goto L_08AAB54C;
      }
      goto L_08AAB530;
    }
L_08AAB530:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08AAB540u);
    aot_gpr_6 = (0u | 4096u);
    ctx.pc = 0x08B73654u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB540:
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936), static_cast<std::uint8_t>(0u));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2935), static_cast<std::uint8_t>(ctx.gpr[19]));
      if (branch_taken) {
          goto L_08AAB59C;
      }
      goto L_08AAB54C;
    }
L_08AAB54C:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_5 = (0u | 0u);
    aot_gpr_31 = (0x08AAB55Cu);
    aot_gpr_6 = (4u << 16u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B5814C, 213u, 28u, 0x08B5814Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_7, ctx.gpr[8], ctx.gpr[9], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0213_entry, 213u, 28u, 0x08B5814Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB55Cu) goto L_08AAB55C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB55C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (0u | 0u);
    aot_gpr_31 = (0x08AAB56Cu);
    aot_gpr_6 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 316u, 0x08AAB56Cu, 0x08938F7Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 258u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 258u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 258u, 0x08938F7Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB56Cu) goto L_08AAB56C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB56C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_6 = (0u | 4096u);
    aot_gpr_7 = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_31 = (0x08AAB588u);
    ctx.gpr[9] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 317u, 0x08AAB588u, 0x089390ACu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 274u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 274u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 274u, 0x089390ACu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB588u) goto L_08AAB588;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB588:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956), ctx.gpr[2]);
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-2928));
    aot_gpr_31 = (0x08AAB598u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 318u, 0x08AAB598u, 0x08939328u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 314u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 314u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 314u, 0x08939328u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB598u) goto L_08AAB598;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB598:
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936), static_cast<std::uint8_t>(0u));
    goto L_08AAB59C;
L_08AAB59C:
    ctx.gpr[2] = (ctx.gpr[19] | 0u);
    goto L_08AAB5A0;
L_08AAB5A0:
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(256), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(288));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB5BC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_6 = (2237u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(23248));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (0u | 111u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_6 != aot_gpr_5;
      if (branch_taken) {
          goto L_08AAB60C;
      }
      goto L_08AAB5DC;
    }
L_08AAB5DC:
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_31 = (0x08AAB5F4u);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    ctx.pc = 0x08B736A4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB5F4:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB604;
      }
      goto L_08AAB600;
    }
L_08AAB600:
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2935), static_cast<std::uint8_t>(0u));
    goto L_08AAB604;
L_08AAB604:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
      if (branch_taken) {
          goto L_08AAB638;
      }
      goto L_08AAB60C;
    }
L_08AAB60C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956)));
    { const bool branch_taken = aot_gpr_5 != 0u;
      if (branch_taken) {
          goto L_08AAB62C;
      }
      goto L_08AAB618;
    }
L_08AAB618:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AAB624u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB624:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08AAB638;
      }
      goto L_08AAB62C;
    }
L_08AAB62C:
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_31 = (0x08AAB638u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 330u, 0x08AAB638u, 0x08939174u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 282u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 282u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 282u, 0x08939174u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB638u) goto L_08AAB638;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB638:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB644:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[16]);
    ctx.gpr[16] = (2237u << 16u);
    ctx.gpr[16] = (ctx.gpr[16] + static_cast<std::uint32_t>(23248));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(20)));
    aot_gpr_4 = (0u | 111u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 != aot_gpr_4;
      if (branch_taken) {
          goto L_08AAB688;
      }
      goto L_08AAB668;
    }
L_08AAB668:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(8)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_08AAB6A8;
      }
      goto L_08AAB674;
    }
L_08AAB674:
    aot_gpr_31 = (0x08AAB67Cu);
    ctx.pc = 0x08B7363Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB67C:
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(8), aot_gpr_4);
      if (branch_taken) {
          goto L_08AAB6A8;
      }
      goto L_08AAB688;
    }
L_08AAB688:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(4)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AAB6A8;
      }
      goto L_08AAB694;
    }
L_08AAB694:
    aot_gpr_31 = (0x08AAB69Cu);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-18560), static_cast<std::uint8_t>(0u));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 337u, 0x08AAB69Cu, 0x08938F40u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 254u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 254u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 254u, 0x08938F40u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB69Cu) goto L_08AAB69C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB69C:
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_4 = (0u | 1u);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-18560), static_cast<std::uint8_t>(aot_gpr_4));
    goto L_08AAB6A8;
L_08AAB6A8:
    ctx.gpr[16] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB6B8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_6 = (2237u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(23248));
    aot_gpr_7 = (aot_gpr_5 | 0u);
    ctx.gpr[9] = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (aot_gpr_4 | 0u);
    ctx.gpr[8] = (0u | 111u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = ctx.gpr[9] != ctx.gpr[8];
    aot_gpr_4 = (aot_gpr_7 | 0u);
      if (branch_taken) {
          goto L_08AAB6FC;
      }
      goto L_08AAB6E0;
    }
L_08AAB6E0:
    aot_gpr_7 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(8)));
    aot_gpr_31 = (0x08AAB6F0u);
    aot_gpr_6 = (aot_gpr_7 | 0u);
    ctx.pc = 0x08B73654u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB6F0:
    aot_gpr_4 = (0u | 1u);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2935), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_08AAB728;
      }
      goto L_08AAB6FC;
    }
L_08AAB6FC:
    aot_gpr_7 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_6 + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (aot_gpr_7 | 0u);
    aot_gpr_7 = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_31 = (0x08AAB718u);
    ctx.gpr[9] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 343u, 0x08AAB718u, 0x089390ACu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 274u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 274u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 274u, 0x089390ACu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB718u) goto L_08AAB718;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB718:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956), ctx.gpr[2]);
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-2928));
    aot_gpr_31 = (0x08AAB728u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 344u, 0x08AAB728u, 0x08939328u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 314u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 314u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 314u, 0x08939328u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB728u) goto L_08AAB728;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB728:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB734:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_7 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(20)));
    aot_gpr_6 = (0u | 111u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_7 != aot_gpr_6;
      if (branch_taken) {
          goto L_08AAB770;
      }
      goto L_08AAB754;
    }
L_08AAB754:
    aot_gpr_6 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    aot_gpr_7 = (0u | 0u);
    aot_gpr_31 = (0x08AAB768u);
    ctx.gpr[8] = (0u | 0u);
    ctx.pc = 0x08B7369Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB768:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB784;
      }
      goto L_08AAB770;
    }
L_08AAB770:
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_31 = (0x08AAB784u);
    aot_gpr_6 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 349u, 0x08AAB784u, 0x08938F7Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 258u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 258u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 258u, 0x08938F7Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB784u) goto L_08AAB784;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB784:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB790:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(7701)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), std::bit_cast<std::uint32_t>(ctx.fpr[24]));
    ctx.fpr[24] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[24] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[24])));
    aot_gpr_5 = (16544u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_4 = (17150u << 16u);
    aot_fpr_13 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[20]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[24] < aot_fpr_12)) ? 0x00800000u : 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_31);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[20] = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_08AAB7D0;
      }
      goto L_08AAB7CC;
    }
L_08AAB7CC:
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_08AAB7D0;
L_08AAB7D0:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(7700)));
    ctx.fpr[22] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[22] = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(ctx.fpr[22])));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[22] < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08AAB7F0;
      }
      goto L_08AAB7EC;
    }
L_08AAB7EC:
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_08AAB7F0;
L_08AAB7F0:
    aot_gpr_31 = (0x08AAB7F8u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0033.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 355u, 0x08AAB7F8u, 0x0888A208u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0033_entry(rt, ctx, 600u, aot_mem);
#else
        recomp_unit_0033_entry(rt, ctx, 600u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0033_entry, 33u, 600u, 0x0888A208u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB7F8u) goto L_08AAB7F8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB7F8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-25912)));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
      if (branch_taken) {
          goto L_08AAB81C;
      }
      goto L_08AAB808;
    }
L_08AAB808:
    aot_fpr_13 = ctx.fpr[24] / ctx.fpr[20];
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_08AAB82C;
      }
      goto L_08AAB81C;
    }
L_08AAB81C:
    aot_fpr_13 = ctx.fpr[22] / ctx.fpr[20];
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08AAB82C;
L_08AAB82C:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2960), aot_gpr_4);
    aot_gpr_31 = (0x08AAB838u);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(7696));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0001.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 359u, 0x08AAB838u, 0x0880A704u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0001_entry(rt, ctx, 559u, aot_mem);
#else
        recomp_unit_0001_entry(rt, ctx, 559u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0001_entry, 1u, 559u, 0x0880A704u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB838u) goto L_08AAB838;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB838:
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[2]);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    aot_fpr_12 = aot_fpr_12 / ctx.fpr[20];
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2960)));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_13)));
    { const float fs = aot_fpr_13; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2960), aot_gpr_4);
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      ctx.fpr[20] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB878:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(20)));
    aot_gpr_4 = (0u | 111u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 == aot_gpr_4;
      if (branch_taken) {
          goto L_08AAB8E4;
      }
      goto L_08AAB898;
    }
L_08AAB898:
    aot_gpr_31 = (0x08AAB8A0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0205.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 362u, 0x08AAB8A0u, 0x08B3AAE8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0205_entry(rt, ctx, 733u, aot_mem);
#else
        recomp_unit_0205_entry(rt, ctx, 733u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0205_entry, 205u, 733u, 0x08B3AAE8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB8A0u) goto L_08AAB8A0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB8A0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AAB8B8;
      }
      goto L_08AAB8A8;
    }
L_08AAB8A8:
    aot_gpr_31 = (0x08AAB8B0u);
    aot_gpr_4 = (0u | 1000u);
    ctx.pc = 0x08B7340Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB8B0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AAB898;
      }
      goto L_08AAB8B8;
    }
L_08AAB8B8:
    aot_gpr_31 = (0x08AAB8C0u);
    ctx.pc = 0x08B73114u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB8C0:
    aot_gpr_4 = (ctx.gpr[2] & 32u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB8E4;
      }
      goto L_08AAB8CC;
    }
L_08AAB8CC:
    aot_gpr_31 = (0x08AAB8D4u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 368u, 0x08AAB8D4u, 0x08939268u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 298u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 298u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 298u, 0x08939268u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB8D4u) goto L_08AAB8D4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB8D4:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AAB8E4;
      }
      goto L_08AAB8DC;
    }
L_08AAB8DC:
    aot_gpr_31 = (0x08AAB8E4u);
    aot_gpr_4 = (0u | 32u);
    ctx.pc = 0x08B7311Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB8E4:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB8F0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-2920));
    aot_gpr_5 = (0u | 512u);
    aot_gpr_6 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[16]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08AAB910u);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B734C4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB910:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(8356), ctx.gpr[2]);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-2908));
    aot_gpr_5 = (0u | 0u);
    aot_gpr_6 = (0u | 1u);
    aot_gpr_7 = (0u | 1u);
    aot_gpr_31 = (0x08AAB92Cu);
    ctx.gpr[8] = (0u | 0u);
    ctx.pc = 0x08B7341Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB92C:
    aot_gpr_4 = (2232u << 16u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(10224), ctx.gpr[2]);
    aot_gpr_5 = (0u | 512u);
    aot_gpr_6 = (0u | 0u);
    aot_gpr_7 = (0u | 0u);
    aot_gpr_31 = (0x08AAB948u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(9912));
    ctx.pc = 0x08B734C4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB948:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(10228), ctx.gpr[2]);
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    ctx.gpr[16] = (2237u << 16u);
    aot_mem.aot_direct_store32(ctx.gpr[16] + static_cast<std::uint32_t>(23248), aot_gpr_4);
    aot_gpr_4 = (2232u << 16u);
    aot_gpr_5 = (2219u << 16u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(4144)));
    aot_gpr_7 = (0u | 32768u);
    ctx.gpr[8] = (0u | 16384u);
    ctx.gpr[9] = (0u | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(9932));
    aot_gpr_31 = (0x08AAB97Cu);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-17372));
    ctx.pc = 0x08B734B4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB97C:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(8332), ctx.gpr[2]);
    aot_gpr_6 = (ctx.gpr[16] + static_cast<std::uint32_t>(23248));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08AAB990u);
    aot_gpr_5 = (0u | 28u);
    ctx.pc = 0x08B73464u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB990:
    ctx.gpr[2] = (0u | 1u);
    ctx.gpr[16] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AAB9A4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), ctx.gpr[19]);
    ctx.gpr[19] = (aot_gpr_7 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), ctx.gpr[18]);
    aot_gpr_7 = (0u | 111u);
    ctx.gpr[18] = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (aot_gpr_5 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[16]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 != aot_gpr_7;
    ctx.gpr[16] = (aot_gpr_6 | 0u);
      if (branch_taken) {
          goto L_08AABA1C;
      }
      goto L_08AAB9D4;
    }
L_08AAB9D4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AAB9EC;
      }
      goto L_08AAB9E0;
    }
L_08AAB9E0:
    aot_gpr_31 = (0x08AAB9E8u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 380u, 0x08AAB9E8u, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB9E8u) goto L_08AAB9E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AAB9E8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    goto L_08AAB9EC;
L_08AAB9EC:
    aot_gpr_31 = (0x08AAB9F4u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0044_entry, 44u, 278u, 0x088B5BECu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AAB9F4u) goto L_08AAB9F4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AAB9F4:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08AABA1C;
      }
      goto L_08AAB9FC;
    }
L_08AAB9FC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5876)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AABA10;
      }
      goto L_08AABA08;
    }
L_08AABA08:
    aot_gpr_31 = (0x08AABA10u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 466u, 0x08B65E44u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AABA10u) goto L_08AABA10;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABA10:
    aot_gpr_31 = (0x08AABA18u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5876)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0112.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 386u, 0x08AABA18u, 0x089C6F60u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0112_entry(rt, ctx, 679u, aot_mem);
#else
        recomp_unit_0112_entry(rt, ctx, 679u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0112_entry, 112u, 679u, 0x089C6F60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AABA18u) goto L_08AABA18;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AABA18:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    goto L_08AABA1C;
L_08AABA1C:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABA48;
      }
      goto L_08AABA28;
    }
L_08AABA28:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[16] = (0u | 0u);
      if (branch_taken) {
          goto L_08AABA40;
      }
      goto L_08AABA34;
    }
L_08AABA34:
    aot_gpr_31 = (0x08AABA3Cu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 390u, 0x08AABA3Cu, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AABA3Cu) goto L_08AABA3C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AABA3C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    goto L_08AABA40;
L_08AABA40:
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(52), 0u);
    aot_mem.aot_direct_store8(ctx.gpr[18] + static_cast<std::uint32_t>(0), static_cast<std::uint8_t>(0u));
    goto L_08AABA48;
L_08AABA48:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8336)));
    aot_gpr_31 = (0x08AABA54u);
    aot_gpr_5 = (0u | 0u);
    ctx.pc = 0x08B734FCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABA54:
    { const bool branch_taken = ctx.gpr[19] != 0u;
      if (branch_taken) {
          goto L_08AABA68;
      }
      goto L_08AABA5C;
    }
L_08AABA5C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8336)));
    aot_gpr_31 = (0x08AABA68u);
    aot_gpr_5 = (0u | 1u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABA68:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_5 = (0u | 16u);
    aot_gpr_6 = (0u | 0u);
    aot_gpr_31 = (0x08AABA7Cu);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABA7C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AABAD8;
      }
      goto L_08AABA84;
    }
L_08AABA84:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_5 = (0u | 8u);
    aot_gpr_6 = (0u | 0u);
    aot_gpr_31 = (0x08AABA98u);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABA98:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08AABAC4;
      }
      goto L_08AABAA0;
    }
L_08AABAA0:
    // READY belongs to the current source, not to every future request. A
    // different cutscene must open its own file, even for a preload request.
    if (vcs::stream_request_needs_new_source(
            aot_mem.aot_direct_load32(0x08BD5AE4u), ctx.gpr[17])) goto L_08AABAC4;
    { const bool branch_taken = ctx.gpr[19] != 0u;
      if (branch_taken) {
          goto L_08AABAD8;
      }
      goto L_08AABAA8;
    }
L_08AABAA8:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2948), ctx.gpr[17]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2944), ctx.gpr[16]);
    aot_gpr_31 = (0x08AABABCu);
    aot_gpr_5 = (0u | 2u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABABC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AABAD8;
      }
      goto L_08AABAC4;
    }
L_08AABAC4:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2948), ctx.gpr[17]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2944), ctx.gpr[16]);
    aot_gpr_31 = (0x08AABAD8u);
    aot_gpr_5 = vcs::stream_open_event_bits(aot_mem.aot_direct_load32(0x08BD5AE4u), ctx.gpr[17]);
    vcs::trace_stream_request(aot_mem.aot_direct_load32(0x08BD5AE4u), ctx.gpr[17], aot_gpr_5);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABAD8:
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      ctx.gpr[16] = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABAF4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(12), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABB18u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABB18:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8336)));
    aot_gpr_31 = (0x08AABB24u);
    aot_gpr_5 = (0u | 1u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABB24:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABB30:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(12), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_5 = (0u | 8u);
    aot_gpr_6 = (0u | 0u);
    aot_gpr_31 = (0x08AABB5Cu);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABB5C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08AABB70;
      }
      goto L_08AABB64;
    }
L_08AABB64:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABB70u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABB70:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8336)));
    aot_gpr_31 = (0x08AABB7Cu);
    aot_gpr_5 = (0u | 1u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABB7C:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABB88:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_5 = (0u | 10u);
    aot_gpr_6 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08AABBA4u);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABBA4:
    ctx.gpr[2] = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABBB4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_5 = (0u | 8u);
    aot_gpr_6 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08AABBD0u);
    aot_gpr_7 = (0u | 0u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABBD0:
    ctx.gpr[2] = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABBE0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(10224)));
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08AABBF8u);
    aot_gpr_6 = (0u | 0u);
    ctx.pc = 0x08B734BCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABBF8:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABC04:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(10224)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08AABC18u);
    aot_gpr_5 = (0u | 1u);
    ctx.pc = 0x08B734A4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABC18:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08AABC24:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-128));
    { const std::uint32_t aot_run_words[7]{ctx.gpr[16], ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(88), aot_run_words); }
    aot_gpr_31 = (0x08AABC4Cu);
    aot_gpr_4 = (0u | 512u);
    ctx.pc = 0x08B73384u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABC4C:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    ctx.gpr[20] = (0u | 0u);
    ctx.gpr[19] = (0u + static_cast<std::uint32_t>(-1));
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[17] = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    ctx.gpr[16] = (0u | 0u);
    goto L_08AABC68;
L_08AABC68:
    aot_gpr_31 = (0x08AABC70u);
    goto L_08AAB878;
L_08AABC70:
    aot_gpr_4 = (ctx.gpr[18] - ctx.gpr[17]);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) >= 0;
      if (branch_taken) {
          goto L_08AABC80;
      }
      goto L_08AABC7C;
    }
L_08AABC7C:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(6144));
    goto L_08AABC80;
L_08AABC80:
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 512 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AABC94;
      }
      goto L_08AABC8C;
    }
L_08AABC8C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0170_entry, 170u, 54u, 0x08AAC324u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_08AABC94;
    }
L_08AABC94:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    aot_gpr_7 = (aot_gpr_29 + static_cast<std::uint32_t>(8));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_31 = (0x08AABCACu);
    aot_gpr_6 = (0u | 1u);
    ctx.pc = 0x08B73484u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABCAC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 4u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABCC4;
      }
      goto L_08AABCBC;
    }
L_08AABCBC:
    ctx.gpr[20] = (0u | 0u);
    ctx.gpr[16] = (0u | 0u);
    goto L_08AABCC4;
L_08AABCC4:
    { const bool branch_taken = ctx.gpr[20] != 0u;
      if (branch_taken) {
          goto L_08AABDF8;
      }
      goto L_08AABCCC;
    }
L_08AABCCC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 5u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABD78;
      }
      goto L_08AABCDC;
    }
L_08AABCDC:
    aot_gpr_31 = (0x08AABCE4u);
    goto L_08AAB27C;
L_08AABCE4:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AABD78;
      }
      goto L_08AABCEC;
    }
L_08AABCEC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABCF8u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-31));
    ctx.pc = 0x08B734FCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABCF8:
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_08AABD5C;
      }
      goto L_08AABD08;
    }
L_08AABD08:
    ctx.gpr[16] = (2237u << 16u);
    aot_gpr_31 = (0x08AABD14u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[16] + static_cast<std::uint32_t>(23248)));
    ctx.pc = 0x08B7328Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABD14:
    aot_gpr_4 = (ctx.gpr[16] + static_cast<std::uint32_t>(23248));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (0u | 111u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08AABD50;
      }
      goto L_08AABD28;
    }
L_08AABD28:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_gpr_5 = (2237u << 16u);
      if (branch_taken) {
          goto L_08AABD40;
      }
      goto L_08AABD34;
    }
L_08AABD34:
    aot_gpr_31 = (0x08AABD3Cu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0217.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 440u, 0x08AABD3Cu, 0x08B68290u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0217_entry(rt, ctx, 34u, aot_mem);
#else
        recomp_unit_0217_entry(rt, ctx, 34u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0217_entry, 217u, 34u, 0x08B68290u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AABD3Cu) goto L_08AABD3C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AABD3C:
    aot_gpr_5 = (2237u << 16u);
    goto L_08AABD40;
L_08AABD40:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5896)));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(24)));
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(52), aot_gpr_5);
    goto L_08AABD50;
L_08AABD50:
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_5 = (2237u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(23248), aot_gpr_4);
    goto L_08AABD5C;
L_08AABD5C:
    aot_gpr_31 = (0x08AABD64u);
    goto L_08AAB644;
L_08AABD64:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    ctx.gpr[19] = (0u + static_cast<std::uint32_t>(-1));
    ctx.gpr[20] = (0u | 0u);
    ctx.gpr[16] = (0u | 0u);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-25908), static_cast<std::uint8_t>(0u));
    goto L_08AABD78;
L_08AABD78:
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) >= 0;
      if (branch_taken) {
          goto L_08AABDF8;
      }
      goto L_08AABD88;
    }
L_08AABD88:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 1u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABDF8;
      }
      goto L_08AABD98;
    }
L_08AABD98:
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2948)));
    ctx.gpr[21] = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2944)));
    aot_mem.aot_direct_store32(ctx.gpr[21] + static_cast<std::uint32_t>(20), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_mem.aot_direct_store32(ctx.gpr[21] + static_cast<std::uint32_t>(24), aot_gpr_5);
    aot_gpr_31 = (0x08AABDBCu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    ctx.pc = 0x08B734FCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABDBC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABDC8u);
    aot_gpr_5 = (0u | 16u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABDC8:
    aot_gpr_31 = (0x08AABDD0u);
    aot_mem.aot_direct_store32(ctx.gpr[21] + static_cast<std::uint32_t>(16), 0u);
    goto L_08AAB35C;
L_08AABDD0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AABDE4;
      }
      goto L_08AABDD8;
    }
L_08AABDD8:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[20] = (0u | 1u);
      if (branch_taken) {
          goto L_08AABDF8;
      }
      goto L_08AABDE4;
    }
L_08AABDE4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABDF0u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABDF0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0170_entry, 170u, 54u, 0x08AAC324u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_08AABDF8;
    }
L_08AABDF8:
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
    { const bool branch_taken = ctx.gpr[20] == 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_4);
      if (branch_taken) {
          goto L_08AABFF4;
      }
      goto L_08AABE04;
    }
L_08AABE04:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08AABE20;
      }
      goto L_08AABE10;
    }
L_08AABE10:
    aot_gpr_31 = (0x08AABE18u);
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(12));
    goto L_08AAB5BC;
L_08AABE18:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08AABFF4;
      }
      goto L_08AABE20;
    }
L_08AABE20:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABE38;
      }
      goto L_08AABE2C;
    }
L_08AABE2C:
    aot_gpr_4 = (0u | 2048u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_4);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-2936), static_cast<std::uint8_t>(0u));
    goto L_08AABE38;
L_08AABE38:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
    aot_gpr_4 = (2237u << 16u);
      if (branch_taken) {
          goto L_08AABFBC;
      }
      goto L_08AABE44;
    }
L_08AABE44:
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(23248));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(16)));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956), 0u);
    aot_gpr_7 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    aot_gpr_6 = (aot_gpr_6 + aot_gpr_7);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) >= 0;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), aot_gpr_6);
      if (branch_taken) {
          goto L_08AABF7C;
      }
      goto L_08AABE64;
    }
L_08AABE64:
    aot_gpr_4 = (2240u << 16u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-4992));
    aot_gpr_31 = (0x08AABE78u);
    aot_gpr_6 = (4u << 16u);
    ctx.pc = 0x08B73274u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABE78:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_08AABF68;
      }
      goto L_08AABE84;
    }
L_08AABE84:
    ctx.gpr[20] = (2237u << 16u);
    aot_mem.aot_direct_store32(ctx.gpr[20] + static_cast<std::uint32_t>(23248), aot_gpr_4);
    aot_gpr_31 = (0x08AABE94u);
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(20));
    ctx.pc = 0x08B732C4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABE94:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (1411u << 16u);
    aot_gpr_4 = (aot_gpr_4 << 12u);
    aot_gpr_4 = (0u + aot_gpr_4);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(13104));
    { const std::uint32_t dividend = aot_gpr_5; const std::uint32_t divisor = aot_gpr_4; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2932), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(23248)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(64), 0u);
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(56));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(60));
    ctx.gpr[19] = (ctx.lo);
    aot_gpr_31 = (0x08AABED4u);
    aot_gpr_7 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    ctx.pc = 0x08B732BCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABED4:
    aot_gpr_7 = (ctx.gpr[20] + static_cast<std::uint32_t>(23248));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_7 + static_cast<std::uint32_t>(24)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const std::uint32_t dividend = aot_gpr_4; const std::uint32_t divisor = aot_gpr_5; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(23248)));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(24));
    aot_gpr_5 = (ctx.hi);
    aot_gpr_31 = (0x08AABEF8u);
    aot_mem.aot_direct_store32(aot_gpr_7 + static_cast<std::uint32_t>(24), aot_gpr_5);
    ctx.pc = 0x08B7327Cu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABEF8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABF40;
      }
      goto L_08AABF04;
    }
L_08AABF04:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08AABF1C;
      }
      goto L_08AABF10;
    }
L_08AABF10:
    aot_gpr_31 = (0x08AABF18u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0169->0077.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0169_entry, 470u, 0x08AABF18u, 0x08939138u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0077_entry(rt, ctx, 278u, aot_mem);
#else
        recomp_unit_0077_entry(rt, ctx, 278u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0077_entry, 77u, 278u, 0x08939138u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08AABF18u) goto L_08AABF18;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08AABF18:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-2956), 0u);
    goto L_08AABF1C;
L_08AABF1C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_31 = (0x08AABF30u);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), aot_gpr_4);
    goto L_08AAB734;
L_08AABF30:
    ctx.gpr[16] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    aot_gpr_31 = (0x08AABF40u);
    aot_gpr_5 = (ctx.gpr[16] | 0u);
    goto L_08AAB6B8;
L_08AABF40:
    { const bool branch_taken = ctx.gpr[16] == 0u;
      if (branch_taken) {
          goto L_08AABF60;
      }
      goto L_08AABF48;
    }
L_08AABF48:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABF54u);
    aot_gpr_5 = (0u | 8u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABF54:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABF60u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-17));
    ctx.pc = 0x08B734FCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABF60:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AABF74;
      }
      goto L_08AABF68;
    }
L_08AABF68:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABF74u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABF74:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08AABFCC;
      }
      goto L_08AABF7C;
    }
L_08AABF7C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_31 = (0x08AABF8Cu);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    ctx.pc = 0x08B732A4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABF8C:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[2]) >= 0;
      if (branch_taken) {
          goto L_08AABFA0;
      }
      goto L_08AABF94;
    }
L_08AABF94:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABFA0u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABFA0:
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(12)));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23248));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (aot_gpr_6 + aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), aot_gpr_4);
      if (branch_taken) {
          goto L_08AABFCC;
      }
      goto L_08AABFBC;
    }
L_08AABFBC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(8356)));
    aot_gpr_31 = (0x08AABFC8u);
    aot_gpr_5 = (0u | 4u);
    ctx.pc = 0x08B73424u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABFC8:
    ctx.gpr[16] = (0u | 0u);
    goto L_08AABFCC;
L_08AABFCC:
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
    ctx.gpr[20] = (0u | 0u);
      if (branch_taken) {
          goto L_08AABFF4;
      }
      goto L_08AABFDC;
    }
L_08AABFDC:
    { const bool branch_taken = ctx.gpr[16] != 0u;
      if (branch_taken) {
          goto L_08AABFF4;
      }
      goto L_08AABFE4;
    }
L_08AABFE4:
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(23248)));
    aot_gpr_31 = (0x08AABFF4u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    ctx.pc = 0x08B732B4u;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08AABFF4:
    aot_gpr_4 = (ctx.gpr[16] | ctx.gpr[20]);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0170_entry, 170u, 13u, 0x08AAC0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0170_entry, 170u, 1u, 0x08AAC000u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
    }
}

#undef AOT_REGCACHE_SYNC_IN
#undef AOT_REGCACHE_SYNC_OUT

void recomp_unit_0169(Runtime &rt, AllegrexContext &ctx) {
    auto aot_mem = rt.memory().aot_fast_view();
    recomp_unit_0169_entry(rt, ctx, 0u, aot_mem);
}

void register_generated_unit_169(Runtime &runtime) {
    runtime.register_generated_unit(169u, 0x08AA8000u, 16384u, &recomp_unit_0169, &recomp_unit_0169_entry);
    // PSPRECOMP_V812_COMPACT_REGISTRATION: same entry set, looped once in Runtime instead of source-expanded calls.
    runtime.register_generated_entry_mask(0x08AA8004u, &recomp_unit_0169, "recomp_unit_0169",
                                          kEntryMasks_recomp_unit_0169, 64u);
}
} // namespace psprecomp
