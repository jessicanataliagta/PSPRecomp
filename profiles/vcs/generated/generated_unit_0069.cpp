#include "psprecomp/runtime.hpp"
#include "generated_units.hpp"
#include "../host/vcs_frame_timing.hpp"
#include "vcs_resident_regions.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>

namespace psprecomp {
// PSPRECOMP_V811_COMPACT_DISPATCH: 64-slot occupancy masks + per-group base id.
static constexpr std::uint64_t kEntryMasks_recomp_unit_0069[64] = {
    0x4842211242050441ull, 0x0400A06202100485ull, 0x4520000008008402ull, 0x4812888054444AA9ull,
    0x1108028052000129ull, 0x00000D01001C82A4ull, 0x21482A2041000000ull, 0x2A00A08284400400ull,
    0xA005008004010404ull, 0x0200100414000080ull, 0xA100009284008024ull, 0x82A0010209052414ull,
    0xAA08284400400214ull, 0x000000534100E4ACull, 0x00190820A5004000ull, 0x08142A8AA8940008ull,
    0x90884AA008884000ull, 0x1400040811444442ull, 0x08044489048546AAull, 0x5415294400124021ull,
    0x408208010004A111ull, 0xA208000802086082ull, 0x245040A850884204ull, 0x0000000044110104ull,
    0x0840013082100800ull, 0x5400100084442111ull, 0x0411102100802555ull, 0x8104048589122000ull,
    0x0040120400002088ull, 0x0155429081041208ull, 0xA040000000040021ull, 0x00001C8490284010ull,
    0x0040800500308220ull, 0x0808061090280200ull, 0x0909000811084206ull, 0x4005000100E0582Aull,
    0x0001400D20822221ull, 0x05000C3084050000ull, 0x000C001400000058ull, 0x0000000000810000ull,
    0xC810803208200000ull, 0x108A105421151040ull, 0x540800400002410Aull, 0x022142220852200Bull,
    0x90A000008844A000ull, 0x1088000080042008ull, 0x0110800000000880ull, 0x0210280100004200ull,
    0x44420000812A0000ull, 0x0108440205004004ull, 0x00001000801A4200ull, 0x0200008004200404ull,
    0x8082000A80000020ull, 0x0101011054004248ull, 0x0004150001091000ull, 0x00800500300200A0ull,
    0xA4A88C0A08284020ull, 0x28A2AAA954B42004ull, 0x2210000800C90152ull, 0x4280800151141000ull,
    0x20088A9280800115ull, 0x024AA8AA5140100Aull, 0x05004111115148A1ull, 0x98AA10A142101530ull,
};
static constexpr std::uint16_t kEntryBases_recomp_unit_0069[64] = {
    1u, 16u, 28u, 36u, 55u, 67u, 79u, 89u, 100u, 109u, 115u, 126u, 140u, 153u, 167u, 177u,
    193u, 206u, 218u, 236u, 252u, 263u, 274u, 289u, 295u, 304u, 316u, 330u, 343u, 350u, 365u, 371u,
    382u, 391u, 401u, 413u, 426u, 438u, 448u, 455u, 457u, 467u, 482u, 492u, 507u, 517u, 524u, 529u,
    536u, 545u, 554u, 561u, 567u, 574u, 585u, 593u, 601u, 617u, 639u, 651u, 662u, 676u, 694u, 710u,
};
void recomp_unit_0069_entry(Runtime &rt, AllegrexContext &ctx, std::uint16_t direct_entry_id, GuestMemory::AotFastView &aot_mem) {
// PSPRECOMP_AOT_REGCACHE_BEGIN
// PSPRECOMP_AOT_REGCACHE_META gprs=4,5,29,31,16,6 fprs=12,13,14,20 gpr_occ=4045 fpr_occ=1326 gpr_total=5238 fpr_total=1593
    std::uint32_t aot_gpr_4 = ctx.gpr[4];
    std::uint32_t aot_gpr_5 = ctx.gpr[5];
    std::uint32_t aot_gpr_29 = ctx.gpr[29];
    std::uint32_t aot_gpr_31 = ctx.gpr[31];
    std::uint32_t aot_gpr_16 = ctx.gpr[16];
    std::uint32_t aot_gpr_6 = ctx.gpr[6];
    float aot_fpr_12 = ctx.fpr[12];
    float aot_fpr_13 = ctx.fpr[13];
    float aot_fpr_14 = ctx.fpr[14];
    float aot_fpr_20 = ctx.fpr[20];
    bool aot_regcache_valid = true;
#define AOT_REGCACHE_SYNC_OUT() do { if (aot_regcache_valid) { ctx.gpr[4] = aot_gpr_4; ctx.gpr[5] = aot_gpr_5; ctx.gpr[29] = aot_gpr_29; ctx.gpr[31] = aot_gpr_31; ctx.gpr[16] = aot_gpr_16; ctx.gpr[6] = aot_gpr_6; ctx.fpr[12] = aot_fpr_12; ctx.fpr[13] = aot_fpr_13; ctx.fpr[14] = aot_fpr_14; ctx.fpr[20] = aot_fpr_20; } } while (false)
#define AOT_REGCACHE_SYNC_IN() do { if (aot_regcache_valid) { aot_gpr_4 = ctx.gpr[4]; aot_gpr_5 = ctx.gpr[5]; aot_gpr_29 = ctx.gpr[29]; aot_gpr_31 = ctx.gpr[31]; aot_gpr_16 = ctx.gpr[16]; aot_gpr_6 = ctx.gpr[6]; aot_fpr_12 = ctx.fpr[12]; aot_fpr_13 = ctx.fpr[13]; aot_fpr_14 = ctx.fpr[14]; aot_fpr_20 = ctx.fpr[20]; } } while (false)
// PSPRECOMP_AOT_REGCACHE_END
    std::uint32_t jump_target = 0u;
    std::uint32_t local_transfers = 0u;
    std::uint32_t local_redispatch_rounds = 0u;
    std::uint32_t local_pc = ctx.pc;
    std::uint32_t entry_id = direct_entry_id;
LOCAL_DISPATCH:
    {
    if (entry_id == 0u) {
        const std::uint32_t entry_delta = local_pc - 0x08918000u;
        entry_id = 0u;
        if (entry_delta < 16384u && (entry_delta & 3u) == 0u) {
            const std::uint32_t entry_slot = entry_delta >> 2u;
            const std::uint32_t entry_group = entry_slot >> 6u;
            const std::uint64_t entry_mask = kEntryMasks_recomp_unit_0069[entry_group];
            const std::uint64_t entry_bit = 1ull << (entry_slot & 63u);
            if ((entry_mask & entry_bit) != 0u) {
                entry_id = static_cast<std::uint16_t>(
                    kEntryBases_recomp_unit_0069[entry_group] +
                    std::popcount(entry_mask & (entry_bit - 1ull)));
            }
        }
    }
    switch (entry_id) {
    case 1u: goto L_08918000;
    case 2u: goto L_08918018;
    case 3u: goto L_08918028;
    case 4u: goto L_08918040;
    case 5u: goto L_08918048;
    case 6u: goto L_08918064;
    case 7u: goto L_08918078;
    case 8u: goto L_08918084;
    case 9u: goto L_08918090;
    case 10u: goto L_089180A0;
    case 11u: goto L_089180B4;
    case 12u: goto L_089180C4;
    case 13u: goto L_089180D8;
    case 14u: goto L_089180EC;
    case 15u: goto L_089180F8;
    case 16u: goto L_08918100;
    case 17u: goto L_08918108;
    case 18u: goto L_0891811C;
    case 19u: goto L_08918128;
    case 20u: goto L_08918150;
    case 21u: goto L_08918164;
    case 22u: goto L_08918184;
    case 23u: goto L_08918194;
    case 24u: goto L_08918198;
    case 25u: goto L_089181B4;
    case 26u: goto L_089181BC;
    case 27u: goto L_089181E8;
    case 28u: goto L_08918204;
    case 29u: goto L_08918228;
    case 30u: goto L_0891823C;
    case 31u: goto L_0891826C;
    case 32u: goto L_089182D4;
    case 33u: goto L_089182E0;
    case 34u: goto L_089182E8;
    case 35u: goto L_089182F8;
    case 36u: goto L_08918300;
    case 37u: goto L_0891830C;
    case 38u: goto L_08918314;
    case 39u: goto L_0891831C;
    case 40u: goto L_08918324;
    case 41u: goto L_0891832C;
    case 42u: goto L_08918338;
    case 43u: goto L_08918348;
    case 44u: goto L_08918358;
    case 45u: goto L_08918368;
    case 46u: goto L_08918370;
    case 47u: goto L_08918378;
    case 48u: goto L_0891839C;
    case 49u: goto L_089183AC;
    case 50u: goto L_089183BC;
    case 51u: goto L_089183C4;
    case 52u: goto L_089183D0;
    case 53u: goto L_089183EC;
    case 54u: goto L_089183F8;
    case 55u: goto L_08918400;
    case 56u: goto L_0891840C;
    case 57u: goto L_08918414;
    case 58u: goto L_08918420;
    case 59u: goto L_08918464;
    case 60u: goto L_08918470;
    case 61u: goto L_08918478;
    case 62u: goto L_0891849C;
    case 63u: goto L_089184A4;
    case 64u: goto L_089184CC;
    case 65u: goto L_089184E0;
    case 66u: goto L_089184F0;
    case 67u: goto L_08918508;
    case 68u: goto L_08918514;
    case 69u: goto L_0891851C;
    case 70u: goto L_08918524;
    case 71u: goto L_0891853C;
    case 72u: goto L_08918548;
    case 73u: goto L_0891854C;
    case 74u: goto L_08918550;
    case 75u: goto L_08918580;
    case 76u: goto L_089185A0;
    case 77u: goto L_089185A8;
    case 78u: goto L_089185AC;
    case 79u: goto L_08918660;
    case 80u: goto L_08918678;
    case 81u: goto L_08918694;
    case 82u: goto L_089186A4;
    case 83u: goto L_089186AC;
    case 84u: goto L_089186B4;
    case 85u: goto L_089186CC;
    case 86u: goto L_089186D8;
    case 87u: goto L_089186E0;
    case 88u: goto L_089186F4;
    case 89u: goto L_08918728;
    case 90u: goto L_08918758;
    case 91u: goto L_08918768;
    case 92u: goto L_0891877C;
    case 93u: goto L_08918784;
    case 94u: goto L_0891879C;
    case 95u: goto L_089187B4;
    case 96u: goto L_089187BC;
    case 97u: goto L_089187E4;
    case 98u: goto L_089187EC;
    case 99u: goto L_089187F4;
    case 100u: goto L_08918808;
    case 101u: goto L_08918828;
    case 102u: goto L_08918840;
    case 103u: goto L_08918868;
    case 104u: goto L_0891889C;
    case 105u: goto L_089188C0;
    case 106u: goto L_089188C8;
    case 107u: goto L_089188F4;
    case 108u: goto L_089188FC;
    case 109u: goto L_0891891C;
    case 110u: goto L_08918968;
    case 111u: goto L_08918970;
    case 112u: goto L_08918988;
    case 113u: goto L_089189B0;
    case 114u: goto L_089189E4;
    case 115u: goto L_08918A08;
    case 116u: goto L_08918A14;
    case 117u: goto L_08918A3C;
    case 118u: goto L_08918A68;
    case 119u: goto L_08918A7C;
    case 120u: goto L_08918A84;
    case 121u: goto L_08918A90;
    case 122u: goto L_08918A9C;
    case 123u: goto L_08918AE0;
    case 124u: goto L_08918AF4;
    case 125u: goto L_08918AFC;
    case 126u: goto L_08918B08;
    case 127u: goto L_08918B10;
    case 128u: goto L_08918B28;
    case 129u: goto L_08918B34;
    case 130u: goto L_08918B40;
    case 131u: goto L_08918B48;
    case 132u: goto L_08918B60;
    case 133u: goto L_08918B6C;
    case 134u: goto L_08918B84;
    case 135u: goto L_08918BA0;
    case 136u: goto L_08918BD4;
    case 137u: goto L_08918BDC;
    case 138u: goto L_08918BE4;
    case 139u: goto L_08918BFC;
    case 140u: goto L_08918C08;
    case 141u: goto L_08918C10;
    case 142u: goto L_08918C24;
    case 143u: goto L_08918C58;
    case 144u: goto L_08918C88;
    case 145u: goto L_08918C98;
    case 146u: goto L_08918CAC;
    case 147u: goto L_08918CB4;
    case 148u: goto L_08918CCC;
    case 149u: goto L_08918CE4;
    case 150u: goto L_08918CEC;
    case 151u: goto L_08918CF4;
    case 152u: goto L_08918CFC;
    case 153u: goto L_08918D08;
    case 154u: goto L_08918D0C;
    case 155u: goto L_08918D14;
    case 156u: goto L_08918D1C;
    case 157u: goto L_08918D28;
    case 158u: goto L_08918D34;
    case 159u: goto L_08918D38;
    case 160u: goto L_08918D3C;
    case 161u: goto L_08918D60;
    case 162u: goto L_08918D78;
    case 163u: goto L_08918D80;
    case 164u: goto L_08918D84;
    case 165u: goto L_08918D90;
    case 166u: goto L_08918D98;
    case 167u: goto L_08918E38;
    case 168u: goto L_08918E60;
    case 169u: goto L_08918E68;
    case 170u: goto L_08918E74;
    case 171u: goto L_08918E7C;
    case 172u: goto L_08918E94;
    case 173u: goto L_08918EAC;
    case 174u: goto L_08918EC0;
    case 175u: goto L_08918ECC;
    case 176u: goto L_08918ED0;
    case 177u: goto L_08918F0C;
    case 178u: goto L_08918F48;
    case 179u: goto L_08918F50;
    case 180u: goto L_08918F5C;
    case 181u: goto L_08918F6C;
    case 182u: goto L_08918F74;
    case 183u: goto L_08918F7C;
    case 184u: goto L_08918F84;
    case 185u: goto L_08918F8C;
    case 186u: goto L_08918F9C;
    case 187u: goto L_08918FA4;
    case 188u: goto L_08918FAC;
    case 189u: goto L_08918FB4;
    case 190u: goto L_08918FC8;
    case 191u: goto L_08918FD0;
    case 192u: goto L_08918FEC;
    case 193u: goto L_08919038;
    case 194u: goto L_0891904C;
    case 195u: goto L_0891905C;
    case 196u: goto L_0891906C;
    case 197u: goto L_08919094;
    case 198u: goto L_0891909C;
    case 199u: goto L_089190A4;
    case 200u: goto L_089190AC;
    case 201u: goto L_089190B8;
    case 202u: goto L_089190CC;
    case 203u: goto L_089190DC;
    case 204u: goto L_089190F0;
    case 205u: goto L_089190FC;
    case 206u: goto L_08919104;
    case 207u: goto L_08919118;
    case 208u: goto L_08919128;
    case 209u: goto L_08919138;
    case 210u: goto L_08919148;
    case 211u: goto L_08919158;
    case 212u: goto L_08919160;
    case 213u: goto L_08919170;
    case 214u: goto L_0891918C;
    case 215u: goto L_089191A8;
    case 216u: goto L_089191E8;
    case 217u: goto L_089191F0;
    case 218u: goto L_08919204;
    case 219u: goto L_0891920C;
    case 220u: goto L_08919214;
    case 221u: goto L_0891921C;
    case 222u: goto L_08919224;
    case 223u: goto L_08919228;
    case 224u: goto L_08919238;
    case 225u: goto L_08919240;
    case 226u: goto L_08919248;
    case 227u: goto L_0891925C;
    case 228u: goto L_08919268;
    case 229u: goto L_08919280;
    case 230u: goto L_0891928C;
    case 231u: goto L_0891929C;
    case 232u: goto L_089192A8;
    case 233u: goto L_089192B8;
    case 234u: goto L_089192C8;
    case 235u: goto L_089192EC;
    case 236u: goto L_08919300;
    case 237u: goto L_08919314;
    case 238u: goto L_08919338;
    case 239u: goto L_08919344;
    case 240u: goto L_08919350;
    case 241u: goto L_08919388;
    case 242u: goto L_08919398;
    case 243u: goto L_089193A0;
    case 244u: goto L_089193AC;
    case 245u: goto L_089193B4;
    case 246u: goto L_089193C0;
    case 247u: goto L_089193C8;
    case 248u: goto L_089193D0;
    case 249u: goto L_089193E8;
    case 250u: goto L_089193F0;
    case 251u: goto L_089193F8;
    case 252u: goto L_08919400;
    case 253u: goto L_08919410;
    case 254u: goto L_08919420;
    case 255u: goto L_08919434;
    case 256u: goto L_0891943C;
    case 257u: goto L_08919448;
    case 258u: goto L_08919480;
    case 259u: goto L_089194AC;
    case 260u: goto L_089194C4;
    case 261u: goto L_089194DC;
    case 262u: goto L_089194F8;
    case 263u: goto L_08919504;
    case 264u: goto L_0891951C;
    case 265u: goto L_08919534;
    case 266u: goto L_08919538;
    case 267u: goto L_0891954C;
    case 268u: goto L_08919564;
    case 269u: goto L_0891958C;
    case 270u: goto L_089195CC;
    case 271u: goto L_089195E4;
    case 272u: goto L_089195F4;
    case 273u: goto L_089195FC;
    case 274u: goto L_08919608;
    case 275u: goto L_08919624;
    case 276u: goto L_08919638;
    case 277u: goto L_0891964C;
    case 278u: goto L_0891965C;
    case 279u: goto L_08919670;
    case 280u: goto L_08919678;
    case 281u: goto L_0891968C;
    case 282u: goto L_08919694;
    case 283u: goto L_0891969C;
    case 284u: goto L_089196B8;
    case 285u: goto L_089196D0;
    case 286u: goto L_089196D8;
    case 287u: goto L_089196E8;
    case 288u: goto L_089196F4;
    case 289u: goto L_08919708;
    case 290u: goto L_08919720;
    case 291u: goto L_08919740;
    case 292u: goto L_08919750;
    case 293u: goto L_08919768;
    case 294u: goto L_08919778;
    case 295u: goto L_0891982C;
    case 296u: goto L_08919850;
    case 297u: goto L_08919864;
    case 298u: goto L_0891987C;
    case 299u: goto L_08919890;
    case 300u: goto L_08919894;
    case 301u: goto L_089198A0;
    case 302u: goto L_089198D8;
    case 303u: goto L_089198EC;
    case 304u: goto L_08919900;
    case 305u: goto L_08919910;
    case 306u: goto L_08919920;
    case 307u: goto L_08919934;
    case 308u: goto L_08919948;
    case 309u: goto L_08919958;
    case 310u: goto L_08919968;
    case 311u: goto L_0891997C;
    case 312u: goto L_089199B0;
    case 313u: goto L_089199E8;
    case 314u: goto L_089199F0;
    case 315u: goto L_089199F8;
    case 316u: goto L_08919A00;
    case 317u: goto L_08919A08;
    case 318u: goto L_08919A10;
    case 319u: goto L_08919A18;
    case 320u: goto L_08919A20;
    case 321u: goto L_08919A28;
    case 322u: goto L_08919A34;
    case 323u: goto L_08919A5C;
    case 324u: goto L_08919A80;
    case 325u: goto L_08919A94;
    case 326u: goto L_08919AB0;
    case 327u: goto L_08919AC0;
    case 328u: goto L_08919AD0;
    case 329u: goto L_08919AE8;
    case 330u: goto L_08919B34;
    case 331u: goto L_08919B44;
    case 332u: goto L_08919B50;
    case 333u: goto L_08919B60;
    case 334u: goto L_08919B6C;
    case 335u: goto L_08919B7C;
    case 336u: goto L_08919B80;
    case 337u: goto L_08919B88;
    case 338u: goto L_08919B9C;
    case 339u: goto L_08919BA8;
    case 340u: goto L_08919BC8;
    case 341u: goto L_08919BE0;
    case 342u: goto L_08919BFC;
    case 343u: goto L_08919C0C;
    case 344u: goto L_08919C1C;
    case 345u: goto L_08919C34;
    case 346u: goto L_08919C88;
    case 347u: goto L_08919CA4;
    case 348u: goto L_08919CB0;
    case 349u: goto L_08919CD8;
    case 350u: goto L_08919D0C;
    case 351u: goto L_08919D24;
    case 352u: goto L_08919D30;
    case 353u: goto L_08919D48;
    case 354u: goto L_08919D60;
    case 355u: goto L_08919D7C;
    case 356u: goto L_08919D90;
    case 357u: goto L_08919D9C;
    case 358u: goto L_08919DA4;
    case 359u: goto L_08919DB8;
    case 360u: goto L_08919DC0;
    case 361u: goto L_08919DC8;
    case 362u: goto L_08919DD0;
    case 363u: goto L_08919DD8;
    case 364u: goto L_08919DE0;
    case 365u: goto L_08919E00;
    case 366u: goto L_08919E14;
    case 367u: goto L_08919E48;
    case 368u: goto L_08919ED8;
    case 369u: goto L_08919EF4;
    case 370u: goto L_08919EFC;
    case 371u: goto L_08919F10;
    case 372u: goto L_08919F38;
    case 373u: goto L_08919F4C;
    case 374u: goto L_08919F54;
    case 375u: goto L_08919F70;
    case 376u: goto L_08919F7C;
    case 377u: goto L_08919F88;
    case 378u: goto L_08919F9C;
    case 379u: goto L_08919FA8;
    case 380u: goto L_08919FAC;
    case 381u: goto L_08919FB0;
    case 382u: goto L_0891A014;
    case 383u: goto L_0891A024;
    case 384u: goto L_0891A03C;
    case 385u: goto L_0891A050;
    case 386u: goto L_0891A054;
    case 387u: goto L_0891A080;
    case 388u: goto L_0891A088;
    case 389u: goto L_0891A0BC;
    case 390u: goto L_0891A0D8;
    case 391u: goto L_0891A124;
    case 392u: goto L_0891A14C;
    case 393u: goto L_0891A154;
    case 394u: goto L_0891A170;
    case 395u: goto L_0891A17C;
    case 396u: goto L_0891A190;
    case 397u: goto L_0891A1A4;
    case 398u: goto L_0891A1A8;
    case 399u: goto L_0891A1CC;
    case 400u: goto L_0891A1EC;
    case 401u: goto L_0891A204;
    case 402u: goto L_0891A208;
    case 403u: goto L_0891A224;
    case 404u: goto L_0891A238;
    case 405u: goto L_0891A24C;
    case 406u: goto L_0891A260;
    case 407u: goto L_0891A270;
    case 408u: goto L_0891A28C;
    case 409u: goto L_0891A2C0;
    case 410u: goto L_0891A2CC;
    case 411u: goto L_0891A2E0;
    case 412u: goto L_0891A2EC;
    case 413u: goto L_0891A304;
    case 414u: goto L_0891A30C;
    case 415u: goto L_0891A314;
    case 416u: goto L_0891A32C;
    case 417u: goto L_0891A330;
    case 418u: goto L_0891A338;
    case 419u: goto L_0891A354;
    case 420u: goto L_0891A358;
    case 421u: goto L_0891A35C;
    case 422u: goto L_0891A380;
    case 423u: goto L_0891A3C0;
    case 424u: goto L_0891A3C8;
    case 425u: goto L_0891A3F8;
    case 426u: goto L_0891A400;
    case 427u: goto L_0891A414;
    case 428u: goto L_0891A424;
    case 429u: goto L_0891A434;
    case 430u: goto L_0891A444;
    case 431u: goto L_0891A45C;
    case 432u: goto L_0891A474;
    case 433u: goto L_0891A480;
    case 434u: goto L_0891A488;
    case 435u: goto L_0891A48C;
    case 436u: goto L_0891A4B8;
    case 437u: goto L_0891A4C0;
    case 438u: goto L_0891A540;
    case 439u: goto L_0891A548;
    case 440u: goto L_0891A568;
    case 441u: goto L_0891A57C;
    case 442u: goto L_0891A590;
    case 443u: goto L_0891A594;
    case 444u: goto L_0891A5A8;
    case 445u: goto L_0891A5AC;
    case 446u: goto L_0891A5E0;
    case 447u: goto L_0891A5E8;
    case 448u: goto L_0891A60C;
    case 449u: goto L_0891A610;
    case 450u: goto L_0891A618;
    case 451u: goto L_0891A688;
    case 452u: goto L_0891A690;
    case 453u: goto L_0891A6C8;
    case 454u: goto L_0891A6CC;
    case 455u: goto L_0891A740;
    case 456u: goto L_0891A75C;
    case 457u: goto L_0891A854;
    case 458u: goto L_0891A86C;
    case 459u: goto L_0891A884;
    case 460u: goto L_0891A890;
    case 461u: goto L_0891A894;
    case 462u: goto L_0891A8BC;
    case 463u: goto L_0891A8D0;
    case 464u: goto L_0891A8EC;
    case 465u: goto L_0891A8F8;
    case 466u: goto L_0891A8FC;
    case 467u: goto L_0891A918;
    case 468u: goto L_0891A930;
    case 469u: goto L_0891A940;
    case 470u: goto L_0891A948;
    case 471u: goto L_0891A950;
    case 472u: goto L_0891A960;
    case 473u: goto L_0891A974;
    case 474u: goto L_0891A988;
    case 475u: goto L_0891A990;
    case 476u: goto L_0891A998;
    case 477u: goto L_0891A9B0;
    case 478u: goto L_0891A9C4;
    case 479u: goto L_0891A9CC;
    case 480u: goto L_0891A9DC;
    case 481u: goto L_0891A9F0;
    case 482u: goto L_0891AA04;
    case 483u: goto L_0891AA0C;
    case 484u: goto L_0891AA20;
    case 485u: goto L_0891AA38;
    case 486u: goto L_0891AA44;
    case 487u: goto L_0891AA98;
    case 488u: goto L_0891AACC;
    case 489u: goto L_0891AAE8;
    case 490u: goto L_0891AAF0;
    case 491u: goto L_0891AAF8;
    case 492u: goto L_0891AB00;
    case 493u: goto L_0891AB04;
    case 494u: goto L_0891AB0C;
    case 495u: goto L_0891AB34;
    case 496u: goto L_0891AB44;
    case 497u: goto L_0891AB50;
    case 498u: goto L_0891AB58;
    case 499u: goto L_0891AB6C;
    case 500u: goto L_0891AB84;
    case 501u: goto L_0891AB94;
    case 502u: goto L_0891ABA4;
    case 503u: goto L_0891ABB8;
    case 504u: goto L_0891ABC0;
    case 505u: goto L_0891ABD4;
    case 506u: goto L_0891ABE4;
    case 507u: goto L_0891AC34;
    case 508u: goto L_0891AC3C;
    case 509u: goto L_0891AC48;
    case 510u: goto L_0891AC58;
    case 511u: goto L_0891AC6C;
    case 512u: goto L_0891AC7C;
    case 513u: goto L_0891ACD4;
    case 514u: goto L_0891ACDC;
    case 515u: goto L_0891ACF0;
    case 516u: goto L_0891ACFC;
    case 517u: goto L_0891AD0C;
    case 518u: goto L_0891AD34;
    case 519u: goto L_0891AD48;
    case 520u: goto L_0891AD7C;
    case 521u: goto L_0891ADCC;
    case 522u: goto L_0891ADDC;
    case 523u: goto L_0891ADF0;
    case 524u: goto L_0891AE1C;
    case 525u: goto L_0891AE2C;
    case 526u: goto L_0891AEBC;
    case 527u: goto L_0891AED0;
    case 528u: goto L_0891AEE0;
    case 529u: goto L_0891AF24;
    case 530u: goto L_0891AF38;
    case 531u: goto L_0891AF80;
    case 532u: goto L_0891AFAC;
    case 533u: goto L_0891AFB4;
    case 534u: goto L_0891AFD0;
    case 535u: goto L_0891AFE4;
    case 536u: goto L_0891B044;
    case 537u: goto L_0891B04C;
    case 538u: goto L_0891B054;
    case 539u: goto L_0891B060;
    case 540u: goto L_0891B07C;
    case 541u: goto L_0891B0C4;
    case 542u: goto L_0891B0D8;
    case 543u: goto L_0891B0E8;
    case 544u: goto L_0891B0F8;
    case 545u: goto L_0891B108;
    case 546u: goto L_0891B138;
    case 547u: goto L_0891B160;
    case 548u: goto L_0891B168;
    case 549u: goto L_0891B184;
    case 550u: goto L_0891B1A8;
    case 551u: goto L_0891B1B8;
    case 552u: goto L_0891B1CC;
    case 553u: goto L_0891B1E0;
    case 554u: goto L_0891B224;
    case 555u: goto L_0891B238;
    case 556u: goto L_0891B244;
    case 557u: goto L_0891B24C;
    case 558u: goto L_0891B250;
    case 559u: goto L_0891B27C;
    case 560u: goto L_0891B2B0;
    case 561u: goto L_0891B308;
    case 562u: goto L_0891B328;
    case 563u: goto L_0891B354;
    case 564u: goto L_0891B368;
    case 565u: goto L_0891B39C;
    case 566u: goto L_0891B3E4;
    case 567u: goto L_0891B414;
    case 568u: goto L_0891B47C;
    case 569u: goto L_0891B484;
    case 570u: goto L_0891B48C;
    case 571u: goto L_0891B4C4;
    case 572u: goto L_0891B4DC;
    case 573u: goto L_0891B4FC;
    case 574u: goto L_0891B50C;
    case 575u: goto L_0891B518;
    case 576u: goto L_0891B524;
    case 577u: goto L_0891B538;
    case 578u: goto L_0891B568;
    case 579u: goto L_0891B570;
    case 580u: goto L_0891B578;
    case 581u: goto L_0891B590;
    case 582u: goto L_0891B5A0;
    case 583u: goto L_0891B5C0;
    case 584u: goto L_0891B5E0;
    case 585u: goto L_0891B630;
    case 586u: goto L_0891B640;
    case 587u: goto L_0891B64C;
    case 588u: goto L_0891B660;
    case 589u: goto L_0891B6A0;
    case 590u: goto L_0891B6A8;
    case 591u: goto L_0891B6B0;
    case 592u: goto L_0891B6C8;
    case 593u: goto L_0891B714;
    case 594u: goto L_0891B71C;
    case 595u: goto L_0891B744;
    case 596u: goto L_0891B770;
    case 597u: goto L_0891B774;
    case 598u: goto L_0891B7A0;
    case 599u: goto L_0891B7A8;
    case 600u: goto L_0891B7DC;
    case 601u: goto L_0891B814;
    case 602u: goto L_0891B838;
    case 603u: goto L_0891B84C;
    case 604u: goto L_0891B854;
    case 605u: goto L_0891B86C;
    case 606u: goto L_0891B884;
    case 607u: goto L_0891B88C;
    case 608u: goto L_0891B8A8;
    case 609u: goto L_0891B8AC;
    case 610u: goto L_0891B8BC;
    case 611u: goto L_0891B8CC;
    case 612u: goto L_0891B8D4;
    case 613u: goto L_0891B8DC;
    case 614u: goto L_0891B8E8;
    case 615u: goto L_0891B8F4;
    case 616u: goto L_0891B8FC;
    case 617u: goto L_0891B908;
    case 618u: goto L_0891B934;
    case 619u: goto L_0891B948;
    case 620u: goto L_0891B950;
    case 621u: goto L_0891B954;
    case 622u: goto L_0891B95C;
    case 623u: goto L_0891B968;
    case 624u: goto L_0891B970;
    case 625u: goto L_0891B978;
    case 626u: goto L_0891B980;
    case 627u: goto L_0891B98C;
    case 628u: goto L_0891B994;
    case 629u: goto L_0891B99C;
    case 630u: goto L_0891B9A4;
    case 631u: goto L_0891B9AC;
    case 632u: goto L_0891B9B4;
    case 633u: goto L_0891B9BC;
    case 634u: goto L_0891B9C4;
    case 635u: goto L_0891B9D4;
    case 636u: goto L_0891B9DC;
    case 637u: goto L_0891B9EC;
    case 638u: goto L_0891B9F4;
    case 639u: goto L_0891BA04;
    case 640u: goto L_0891BA10;
    case 641u: goto L_0891BA18;
    case 642u: goto L_0891BA20;
    case 643u: goto L_0891BA40;
    case 644u: goto L_0891BA4C;
    case 645u: goto L_0891BA58;
    case 646u: goto L_0891BA5C;
    case 647u: goto L_0891BA8C;
    case 648u: goto L_0891BAD0;
    case 649u: goto L_0891BAE4;
    case 650u: goto L_0891BAF4;
    case 651u: goto L_0891BB30;
    case 652u: goto L_0891BB48;
    case 653u: goto L_0891BB50;
    case 654u: goto L_0891BB60;
    case 655u: goto L_0891BB70;
    case 656u: goto L_0891BB78;
    case 657u: goto L_0891BB80;
    case 658u: goto L_0891BBBC;
    case 659u: goto L_0891BBDC;
    case 660u: goto L_0891BBE4;
    case 661u: goto L_0891BBF8;
    case 662u: goto L_0891BC00;
    case 663u: goto L_0891BC08;
    case 664u: goto L_0891BC10;
    case 665u: goto L_0891BC20;
    case 666u: goto L_0891BC5C;
    case 667u: goto L_0891BC7C;
    case 668u: goto L_0891BC84;
    case 669u: goto L_0891BC90;
    case 670u: goto L_0891BC9C;
    case 671u: goto L_0891BCA4;
    case 672u: goto L_0891BCAC;
    case 673u: goto L_0891BCBC;
    case 674u: goto L_0891BCCC;
    case 675u: goto L_0891BCF4;
    case 676u: goto L_0891BD04;
    case 677u: goto L_0891BD0C;
    case 678u: goto L_0891BD30;
    case 679u: goto L_0891BD58;
    case 680u: goto L_0891BD60;
    case 681u: goto L_0891BD70;
    case 682u: goto L_0891BD78;
    case 683u: goto L_0891BD84;
    case 684u: goto L_0891BD8C;
    case 685u: goto L_0891BD94;
    case 686u: goto L_0891BD9C;
    case 687u: goto L_0891BDAC;
    case 688u: goto L_0891BDB4;
    case 689u: goto L_0891BDBC;
    case 690u: goto L_0891BDC4;
    case 691u: goto L_0891BDCC;
    case 692u: goto L_0891BDD8;
    case 693u: goto L_0891BDE4;
    case 694u: goto L_0891BE00;
    case 695u: goto L_0891BE14;
    case 696u: goto L_0891BE1C;
    case 697u: goto L_0891BE2C;
    case 698u: goto L_0891BE38;
    case 699u: goto L_0891BE40;
    case 700u: goto L_0891BE50;
    case 701u: goto L_0891BE58;
    case 702u: goto L_0891BE60;
    case 703u: goto L_0891BE70;
    case 704u: goto L_0891BE80;
    case 705u: goto L_0891BE90;
    case 706u: goto L_0891BEA0;
    case 707u: goto L_0891BEB8;
    case 708u: goto L_0891BEE0;
    case 709u: goto L_0891BEE8;
    case 710u: goto L_0891BF10;
    case 711u: goto L_0891BF14;
    case 712u: goto L_0891BF20;
    case 713u: goto L_0891BF28;
    case 714u: goto L_0891BF30;
    case 715u: goto L_0891BF50;
    case 716u: goto L_0891BF64;
    case 717u: goto L_0891BF78;
    case 718u: goto L_0891BF80;
    case 719u: goto L_0891BF94;
    case 720u: goto L_0891BF9C;
    case 721u: goto L_0891BFB0;
    case 722u: goto L_0891BFC4;
    case 723u: goto L_0891BFCC;
    case 724u: goto L_0891BFD4;
    case 725u: goto L_0891BFDC;
    case 726u: goto L_0891BFEC;
    case 727u: goto L_0891BFF0;
    case 728u: goto L_0891BFFC;
    default:
        if (local_transfers == 0u) rt.unsupported(ctx.pc, 0u, "invalid internal function entry");
        else ctx.pc = local_pc;
        AOT_REGCACHE_SYNC_OUT(); return;
    }
    }
// PSPRECOMP_V813_SHARED_JR_DISPATCH: one dynamic-JR reject/redispatch path per unit.
LOCAL_JR_DISPATCH:
    {
        const std::uint32_t local_delta_v813 = jump_target - 0x08918000u;
        if (local_delta_v813 >= 16384u || (local_delta_v813 & 3u) != 0u) {
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
L_08918000:
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = ctx.fpr[22] - aot_fpr_12;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918028;
      }
      goto L_08918018;
    }
L_08918018:
    aot_gpr_4 = (16585u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_20 = aot_fpr_20 + aot_fpr_12;
    goto L_08918028;
L_08918028:
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[24] <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918100;
      }
      goto L_08918040;
    }
L_08918040:
    aot_gpr_31 = (0x08918048u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[24]));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918048u) goto L_08918048;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918048:
    ctx.gpr[19] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20100)));
    ctx.gpr[18] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20104)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    ctx.gpr[7] = (ctx.gpr[19] | 0u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08918064u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 6u, 0x08918064u, 0x08B61F5Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 395u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 395u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 395u, 0x08B61F5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918064u) goto L_08918064;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918064:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20140)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20144)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x08918078u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918078u) goto L_08918078;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918078:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x08918084u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918084u) goto L_08918084;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918084:
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_31 = (0x08918090u);
    { const float fs = ctx.fpr[24]; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918090u) goto L_08918090;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918090:
    ctx.gpr[21] = (ctx.gpr[3] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[24]));
    aot_gpr_31 = (0x089180A0u);
    ctx.gpr[20] = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180A0u) goto L_089180A0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089180A0:
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    ctx.gpr[7] = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x089180B4u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 11u, 0x089180B4u, 0x08B61F5Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 395u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 395u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 395u, 0x08B61F5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180B4u) goto L_089180B4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089180B4:
    ctx.gpr[19] = (ctx.gpr[3] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    aot_gpr_31 = (0x089180C4u);
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180C4u) goto L_089180C4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089180C4:
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    ctx.gpr[7] = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x089180D8u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180D8u) goto L_089180D8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089180D8:
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    ctx.gpr[7] = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x089180ECu);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61EF4, 215u, 390u, 0x08B61EF4u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 390u, 0x08B61EF4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180ECu) goto L_089180EC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089180EC:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x089180F8u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089180F8u) goto L_089180F8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089180F8:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
      if (branch_taken) {
          goto L_089181B4;
      }
      goto L_08918100;
    }
L_08918100:
    aot_gpr_31 = (0x08918108u);
    aot_fpr_12 = aot_fpr_20 + ctx.fpr[22];
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918108u) goto L_08918108;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918108:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20092)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20096)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x0891811Cu);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891811Cu) goto L_0891811C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891811C:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x08918128u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918128u) goto L_08918128;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918128:
    aot_gpr_4 = (16457u << 16u);
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16457u << 16u);
      if (branch_taken) {
          goto L_08918164;
      }
      goto L_08918150;
    }
L_08918150:
    aot_gpr_4 = (16585u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    aot_fpr_20 = aot_fpr_20 - aot_fpr_12;
      if (branch_taken) {
          goto L_08918194;
      }
      goto L_08918164;
    }
L_08918164:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16256u << 16u);
      if (branch_taken) {
          goto L_08918198;
      }
      goto L_08918184;
    }
L_08918184:
    aot_gpr_4 = (16585u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_20 = aot_fpr_20 + aot_fpr_12;
    goto L_08918194;
L_08918194:
    aot_gpr_4 = (16256u << 16u);
    goto L_08918198;
L_08918198:
    { const float fs = ctx.fpr[24]; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_fpr_13 = aot_fpr_13 - ctx.fpr[24];
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_089181B4;
L_089181B4:
    aot_gpr_31 = (0x089181BCu);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 26u, 0x089181BCu, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089181BCu) goto L_089181BC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089181BC:
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2256), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_4 = (16457u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16457u << 16u);
      if (branch_taken) {
          goto L_08918204;
      }
      goto L_089181E8;
    }
L_089181E8:
    aot_gpr_4 = (16585u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_0891823C;
      }
      goto L_08918204;
    }
L_08918204:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16585u << 16u);
      if (branch_taken) {
          goto L_0891823C;
      }
      goto L_08918228;
    }
L_08918228:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891823C;
L_0891823C:
    { std::uint32_t aot_run_words[10]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      aot_gpr_16 = aot_run_words[3];
      ctx.gpr[17] = aot_run_words[4];
      ctx.gpr[18] = aot_run_words[5];
      ctx.gpr[19] = aot_run_words[6];
      ctx.gpr[20] = aot_run_words[7];
      ctx.gpr[21] = aot_run_words[8];
      aot_gpr_31 = aot_run_words[9];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0891826C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-544));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(516), ctx.gpr[19]);
    ctx.gpr[19] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(684)));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_6 = (128u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (ctx.gpr[19] & aot_gpr_6);
    aot_gpr_6 = (17008u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(492), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_6);
    { const std::uint32_t aot_run_words[5]{std::bit_cast<std::uint32_t>(ctx.fpr[22]), std::bit_cast<std::uint32_t>(ctx.fpr[24]), aot_gpr_16, ctx.gpr[17], ctx.gpr[18]};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(496), aot_run_words); }
    { const std::uint32_t aot_run_words[6]{ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], ctx.gpr[30], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(520), aot_run_words); }
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_gpr_16 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_089182E8;
      }
      goto L_089182D4;
    }
L_089182D4:
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    aot_gpr_31 = (0x089182E0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 33u, 0x089182E0u, 0x0890D0A0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 274u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 274u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 274u, 0x0890D0A0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089182E0u) goto L_089182E0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089182E0:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08918300;
      }
      goto L_089182E8;
    }
L_089182E8:
    aot_gpr_4 = (16u << 16u);
    aot_gpr_4 = (ctx.gpr[19] & aot_gpr_4);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918314;
      }
      goto L_089182F8;
    }
L_089182F8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_089183AC;
      }
      goto L_08918300;
    }
L_08918300:
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    aot_gpr_31 = (0x0891830Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 37u, 0x0891830Cu, 0x08910338u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 49u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 49u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 49u, 0x08910338u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891830Cu) goto L_0891830C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891830C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (128u << 16u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918314;
    }
L_08918314:
    aot_gpr_31 = (0x0891831Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 39u, 0x0891831Cu, 0x0891283Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 629u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 629u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 629u, 0x0891283Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891831Cu) goto L_0891831C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891831C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_089183AC;
      }
      goto L_08918324;
    }
L_08918324:
    aot_gpr_31 = (0x0891832Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 41u, 0x0891832Cu, 0x0890D2E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 299u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 299u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 299u, 0x0890D2E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891832Cu) goto L_0891832C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891832C:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_089183AC;
      }
      goto L_08918338;
    }
L_08918338:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(1360)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08918368;
      }
      goto L_08918348;
    }
L_08918348:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (0u | 4u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08918368;
      }
      goto L_08918358;
    }
L_08918358:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (0u | 5u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_089183AC;
      }
      goto L_08918368;
    }
L_08918368:
    aot_gpr_31 = (0x08918370u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 46u, 0x08918370u, 0x0890D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 319u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 319u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 319u, 0x0890D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918370u) goto L_08918370;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918370:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_089183EC;
      }
      goto L_08918378;
    }
L_08918378:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-24276)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (ctx.gpr[19] & aot_gpr_4);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_089183C4;
      }
      goto L_0891839C;
    }
L_0891839C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (0u | 11u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_089183C4;
      }
      goto L_089183AC;
    }
L_089183AC:
    aot_gpr_4 = (512u << 16u);
    aot_gpr_4 = (ctx.gpr[19] & aot_gpr_4);
    if (aot_gpr_4 != 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2276)));
        goto L_08918400;
    }
    goto L_089183BC;
L_089183BC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918478;
      }
      goto L_089183C4;
    }
L_089183C4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x089183D0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 52u, 0x089183D0u, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089183D0u) goto L_089183D0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089183D0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-24276)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_089183EC;
    }
L_089183EC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x089183F8u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 54u, 0x089183F8u, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089183F8u) goto L_089183F8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089183F8:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (16u << 16u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918400;
    }
L_08918400:
    aot_gpr_5 = (0u | 2u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08918478;
      }
      goto L_0891840C;
    }
L_0891840C:
    aot_gpr_31 = (0x08918414u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 57u, 0x08918414u, 0x0890D3A4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 308u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 308u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 308u, 0x0890D3A4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918414u) goto L_08918414;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918414:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918478;
      }
      goto L_08918420;
    }
L_08918420:
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 1u>();
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 0u, 0u, 3u>();
    aot_gpr_5 = (ctx.vfpu_scalar_bits_ct<28u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (17352u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_5);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918478;
      }
      goto L_08918464;
    }
L_08918464:
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(692), aot_gpr_4);
    aot_gpr_31 = (0x08918470u);
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(692));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08A92F78, 163u, 419u, 0x08A92F78u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0163_entry, 163u, 419u, 0x08A92F78u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918470u) goto L_08918470;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918470:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (512u << 16u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918478;
    }
L_08918478:
    aot_gpr_5 = (17008u << 16u);
    ctx.gpr[17] = (0u | 0u);
    ctx.fpr[22] = std::bit_cast<float>(aot_gpr_5);
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[20] = (0u | 0u);
    ctx.gpr[21] = (0u | 0u);
    ctx.gpr[22] = (0u | 0u);
    aot_gpr_31 = (0x0891849Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 62u, 0x0891849Cu, 0x0891283Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 629u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 629u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 629u, 0x0891283Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891849Cu) goto L_0891849C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891849C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_089184CC;
      }
      goto L_089184A4;
    }
L_089184A4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(76)));
    aot_gpr_5 = (16320u << 16u);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7764)));
    aot_gpr_4 = (aot_gpr_4 >> 22u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_4 = (aot_gpr_4 & 4u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918A7C;
      }
      goto L_089184CC;
    }
L_089184CC:
    aot_gpr_4 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(1914)));
    ctx.gpr[30] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[30]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918A7C;
      }
      goto L_089184E0;
    }
L_089184E0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2276)));
    aot_gpr_5 = (0u | 1u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_08918524;
      }
      goto L_089184F0;
    }
L_089184F0:
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(2276)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_08918524;
      }
      goto L_08918508;
    }
L_08918508:
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_31 = (0x08918514u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918514u) goto L_08918514;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918514:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08918524;
      }
      goto L_0891851C;
    }
L_0891851C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918524;
    }
L_08918524:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
    ctx.gpr[17] = (0u | 0u);
      if (branch_taken) {
          goto L_0891854C;
      }
      goto L_0891853C;
    }
L_0891853C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_08918550;
      }
      goto L_08918548;
    }
L_08918548:
    ctx.gpr[17] = (0u | 1u);
    goto L_0891854C;
L_0891854C:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    goto L_08918550;
L_08918550:
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-24276)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1360)));
    ctx.gpr[23] = (ctx.gpr[17] & 255u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    ctx.gpr[17] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (ctx.gpr[19] & ctx.gpr[17]);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_089185A0;
      }
      goto L_08918580;
    }
L_08918580:
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1252)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_089185AC;
      }
      goto L_089185A0;
    }
L_089185A0:
    { const bool branch_taken = ctx.gpr[23] == 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089185A8;
    }
L_089185A8:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    goto L_089185AC;
L_089185AC:
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(352), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(160), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(164), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(356), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(160)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(164)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(136), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(140), aot_gpr_6);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(136)));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(140)));
    aot_fpr_12 = ctx.fpr[16] - aot_fpr_14;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(144), std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[17] = ctx.fpr[17] - ctx.fpr[15];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(148), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(368), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(152), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(156), aot_gpr_5);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(152)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(156)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(372), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(128), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(132), aot_gpr_5);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(128)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(132)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2120)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_089187BC;
      }
      goto L_08918660;
    }
L_08918660:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089187BC;
      }
      goto L_08918678;
    }
L_08918678:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_gpr_4 = (17204u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089187BC;
      }
      goto L_08918694;
    }
L_08918694:
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(1920)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_5) < 2 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2120)));
      if (branch_taken) {
          goto L_089186CC;
      }
      goto L_089186A4;
    }
L_089186A4:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_5) < 0;
      if (branch_taken) {
          goto L_08918758;
      }
      goto L_089186AC;
    }
L_089186AC:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_5) > 0;
      if (branch_taken) {
          goto L_089186F4;
      }
      goto L_089186B4;
    }
L_089186B4:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(176));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918758;
      }
      goto L_089186CC;
    }
L_089186CC:
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_5) < 3 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_5 = (static_cast<std::int32_t>(aot_gpr_5) < 4 ? 1u : 0u);
      if (branch_taken) {
          goto L_08918728;
      }
      goto L_089186D8;
    }
L_089186D8:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08918758;
      }
      goto L_089186E0;
    }
L_089186E0:
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(176));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918758;
      }
      goto L_089186F4;
    }
L_089186F4:
    aot_gpr_5 = (49024u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(176));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918758;
      }
      goto L_08918728;
    }
L_08918728:
    aot_gpr_5 = (49024u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(176));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    goto L_08918758;
L_08918758:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(176)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(180)));
    aot_gpr_31 = (0x08918768u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 91u, 0x08918768u, 0x08964540u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 28u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 28u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 28u, 0x08964540u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918768u) goto L_08918768;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918768:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    aot_gpr_31 = (0x0891877Cu);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 92u, 0x0891877Cu, 0x08964540u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 28u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 28u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 28u, 0x08964540u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891877Cu) goto L_0891877C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891877C:
    aot_gpr_31 = (0x08918784u);
    aot_fpr_12 = ctx.fpr[24] - ctx.fpr[0];
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 93u, 0x08918784u, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918784u) goto L_08918784;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918784:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089187B4;
      }
      goto L_0891879C;
    }
L_0891879C:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13) ^ 0x80000000u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089187BC;
      }
      goto L_089187B4;
    }
L_089187B4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089187BC;
    }
L_089187BC:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(468)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_gpr_5 & 128u);
    aot_gpr_6 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_6 = (aot_gpr_6 & 255u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x089187E4u);
    aot_gpr_5 = (ctx.gpr[7] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 97u, 0x089187E4u, 0x089068C8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 571u, 0x089068C8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089187E4u) goto L_089187E4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089187E4:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_089188F4;
      }
      goto L_089187EC;
    }
L_089187EC:
    { const bool branch_taken = ctx.gpr[23] == 0u;
      if (branch_taken) {
          goto L_08918808;
      }
      goto L_089187F4;
    }
L_089187F4:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[22] = (1024u << 16u);
      if (branch_taken) {
          goto L_08918A7C;
      }
      goto L_08918808;
    }
L_08918808:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (0u | 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(2228));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    if (aot_gpr_4 != aot_gpr_5) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
        goto L_0891889C;
    }
    goto L_08918828;
L_08918828:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(2192)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_16;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918840;
    }
L_08918840:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = ctx.fpr[22]; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918868;
    }
L_08918868:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_20 = std::sqrt(aot_fpr_20);
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    ctx.gpr[22] = (ctx.gpr[17] | 0u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[21] = (0u | 1u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_0891889C;
    }
L_0891889C:
    { const float fs = aot_fpr_20; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089188C0;
    }
L_089188C0:
    { const bool branch_taken = ctx.gpr[21] != 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089188C8;
    }
L_089188C8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_20 = std::sqrt(aot_fpr_20);
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[22] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089188F4;
    }
L_089188F4:
    { const bool branch_taken = ctx.gpr[21] != 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089188FC;
    }
L_089188FC:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (0u | 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(2228));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_0891891C;
    }
L_0891891C:
    aot_gpr_5 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(1784)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(468)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    ctx.gpr[8] = (ctx.gpr[7] & 128u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    ctx.gpr[11] = (0u < ctx.gpr[8] ? 1u : 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), 0u);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(48));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(208));
    ctx.gpr[7] = (aot_gpr_29 + static_cast<std::uint32_t>(240));
    ctx.gpr[11] = (ctx.gpr[11] & 255u);
    ctx.gpr[8] = (0u | 1u);
    ctx.gpr[9] = (0u | 1u);
    ctx.gpr[10] = (0u | 0u);
    aot_gpr_31 = (0x08918968u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0036.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 110u, 0x08918968u, 0x0889786Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0036_entry(rt, ctx, 698u, aot_mem);
#else
        recomp_unit_0036_entry(rt, ctx, 698u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0036_entry, 36u, 698u, 0x0889786Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918968u) goto L_08918968;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918968:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918970;
    }
L_08918970:
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(2192)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_16;
    aot_gpr_4 = (ctx.gpr[30] << 2u);
      if (branch_taken) {
          goto L_089189E4;
      }
      goto L_08918988;
    }
L_08918988:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = ctx.fpr[22]; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089189B0;
    }
L_089189B0:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_20 = std::sqrt(aot_fpr_20);
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    ctx.gpr[22] = (ctx.gpr[17] | 0u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[20] = (0u | 1u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_089189E4;
    }
L_089189E4:
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(1929))))));
    aot_gpr_6 = (aot_gpr_5 << 5u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_5 = (aot_gpr_6 - aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_31 = (0x08918A08u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0143.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 115u, 0x08918A08u, 0x08A40D34u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0143_entry(rt, ctx, 159u, aot_mem);
#else
        recomp_unit_0143_entry(rt, ctx, 159u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0143_entry, 143u, 159u, 0x08A40D34u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918A08u) goto L_08918A08;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918A08:
    aot_gpr_4 = (ctx.gpr[2] | ctx.gpr[20]);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918A14;
    }
L_08918A14:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = aot_fpr_20; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918A68;
      }
      goto L_08918A3C;
    }
L_08918A3C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_20 = std::sqrt(aot_fpr_20);
    aot_gpr_4 = (ctx.gpr[30] << 2u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1784)));
    ctx.gpr[22] = (ctx.gpr[17] | 0u);
    ctx.gpr[20] = (0u | 1u);
    goto L_08918A68;
L_08918A68:
    aot_gpr_4 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(1914)));
    ctx.gpr[30] = (ctx.gpr[30] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[30]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_089184E0;
      }
      goto L_08918A7C;
    }
L_08918A7C:
    { const bool branch_taken = ctx.gpr[18] == 0u;
      if (branch_taken) {
          goto L_08918A9C;
      }
      goto L_08918A84;
    }
L_08918A84:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918A9C;
      }
      goto L_08918A90;
    }
L_08918A90:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[18];
      if (branch_taken) {
          goto L_08918AFC;
      }
      goto L_08918A9C;
    }
L_08918A9C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
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
    aot_gpr_5 = (16800u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    aot_gpr_5 = (0u | 1u);
    ctx.gpr[7] = (0u | 6u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 1u);
    ctx.gpr[11] = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_31 = (0x08918AE0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0037.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 123u, 0x08918AE0u, 0x08899758u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0037_entry(rt, ctx, 287u, aot_mem);
#else
        recomp_unit_0037_entry(rt, ctx, 287u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0037_entry, 37u, 287u, 0x08899758u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918AE0u) goto L_08918AE0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918AE0:
    ctx.gpr[20] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_29 + static_cast<std::uint32_t>(96))))));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918B10;
      }
      goto L_08918AF4;
    }
L_08918AF4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918EC0;
      }
      goto L_08918AFC;
    }
L_08918AFC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08918B08u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 126u, 0x08918B08u, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918B08u) goto L_08918B08;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918B08:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[22] | 0u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918B10;
    }
L_08918B10:
    aot_gpr_4 = (ctx.gpr[20] << 2u);
    aot_gpr_4 = (aot_gpr_29 + aot_gpr_4);
    ctx.gpr[21] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(64)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918B48;
      }
      goto L_08918B28;
    }
L_08918B28:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[21];
      if (branch_taken) {
          goto L_08918B48;
      }
      goto L_08918B34;
    }
L_08918B34:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08918B40u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 130u, 0x08918B40u, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918B40u) goto L_08918B40;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918B40:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (1024u << 16u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918B48;
    }
L_08918B48:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[21] + static_cast<std::uint32_t>(565)));
    ctx.gpr[22] = (0u | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[22]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918EAC;
      }
      goto L_08918B60;
    }
L_08918B60:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2120)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918CEC;
      }
      goto L_08918B6C;
    }
L_08918B6C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918CEC;
      }
      goto L_08918B84;
    }
L_08918B84:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_gpr_4 = (17204u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (ctx.gpr[21] + static_cast<std::uint32_t>(48));
      if (branch_taken) {
          goto L_08918CEC;
      }
      goto L_08918BA0;
    }
L_08918BA0:
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2120)));
    aot_fpr_14 = aot_fpr_14 - ctx.fpr[15];
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(1920)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(244), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_5) < 2 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(248), std::bit_cast<std::uint32_t>(aot_fpr_14));
      if (branch_taken) {
          goto L_08918BFC;
      }
      goto L_08918BD4;
    }
L_08918BD4:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_5) < 0;
      if (branch_taken) {
          goto L_08918C88;
      }
      goto L_08918BDC;
    }
L_08918BDC:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_5) > 0;
      if (branch_taken) {
          goto L_08918C24;
      }
      goto L_08918BE4;
    }
L_08918BE4:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(256));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918C88;
      }
      goto L_08918BFC;
    }
L_08918BFC:
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_5) < 3 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_5 = (static_cast<std::int32_t>(aot_gpr_5) < 4 ? 1u : 0u);
      if (branch_taken) {
          goto L_08918C58;
      }
      goto L_08918C08;
    }
L_08918C08:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08918C88;
      }
      goto L_08918C10;
    }
L_08918C10:
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(256));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918C88;
      }
      goto L_08918C24;
    }
L_08918C24:
    aot_gpr_5 = (49024u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(272));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(256));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918C88;
      }
      goto L_08918C58;
    }
L_08918C58:
    aot_gpr_5 = (49024u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(272));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(256));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    goto L_08918C88;
L_08918C88:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(256)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(260)));
    aot_gpr_31 = (0x08918C98u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 145u, 0x08918C98u, 0x08964540u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 28u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 28u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 28u, 0x08964540u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918C98u) goto L_08918C98;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918C98:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(244)));
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(248)));
    aot_gpr_31 = (0x08918CACu);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 146u, 0x08918CACu, 0x08964540u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 28u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 28u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 28u, 0x08964540u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918CACu) goto L_08918CAC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918CAC:
    aot_gpr_31 = (0x08918CB4u);
    aot_fpr_12 = ctx.fpr[22] - ctx.fpr[0];
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 147u, 0x08918CB4u, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918CB4u) goto L_08918CB4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918CB4:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918CE4;
      }
      goto L_08918CCC;
    }
L_08918CCC:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1888)));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13) ^ 0x80000000u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08918CEC;
      }
      goto L_08918CE4;
    }
L_08918CE4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918CEC;
    }
L_08918CEC:
    { const bool branch_taken = ctx.gpr[22] != 0u;
      if (branch_taken) {
          goto L_08918CFC;
      }
      goto L_08918CF4;
    }
L_08918CF4:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[23] = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(900)));
      if (branch_taken) {
          goto L_08918D0C;
      }
      goto L_08918CFC;
    }
L_08918CFC:
    aot_gpr_5 = (ctx.gpr[22] + static_cast<std::uint32_t>(-1));
    aot_gpr_31 = (0x08918D08u);
    aot_gpr_4 = (ctx.gpr[21] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_compact_generated_leaf<&vcs_compact_leaf_08AFEF7C, 190u, 0x08AFEF7Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918D08u) goto L_08918D08;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918D08:
    ctx.gpr[23] = (ctx.gpr[2] | 0u);
    goto L_08918D0C;
L_08918D0C:
    { const bool branch_taken = ctx.gpr[23] == 0u;
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918D14;
    }
L_08918D14:
    { const bool branch_taken = ctx.gpr[18] != 0u;
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918D1C;
    }
L_08918D1C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    { const bool branch_taken = ctx.gpr[23] != aot_gpr_4;
    ctx.gpr[17] = (0u | 0u);
      if (branch_taken) {
          goto L_08918D38;
      }
      goto L_08918D28;
    }
L_08918D28:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2080)));
    if (aot_gpr_4 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[23] + static_cast<std::uint32_t>(1360)));
        goto L_08918D3C;
    }
    goto L_08918D34;
L_08918D34:
    ctx.gpr[17] = (0u | 1u);
    goto L_08918D38;
L_08918D38:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[23] + static_cast<std::uint32_t>(1360)));
    goto L_08918D3C;
L_08918D3C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-24276)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    ctx.gpr[30] = (ctx.gpr[17] & 255u);
    ctx.gpr[17] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (ctx.gpr[19] & ctx.gpr[17]);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08918D78;
      }
      goto L_08918D60;
    }
L_08918D60:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[23] + static_cast<std::uint32_t>(1252)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
      if (branch_taken) {
          goto L_08918D84;
      }
      goto L_08918D78;
    }
L_08918D78:
    { const bool branch_taken = ctx.gpr[30] == 0u;
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918D80;
    }
L_08918D80:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_08918D84;
L_08918D84:
    aot_gpr_5 = (ctx.gpr[23] | 0u);
    aot_gpr_31 = (0x08918D90u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 165u, 0x08918D90u, 0x089068C8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 571u, 0x089068C8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918D90u) goto L_08918D90;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918D90:
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_gpr_4 = (ctx.gpr[21] + static_cast<std::uint32_t>(48));
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918D98;
    }
L_08918D98:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(448), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(320), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(324), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(452), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(320)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(324)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(296), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(300), aot_gpr_6);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(296)));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(300)));
    aot_fpr_12 = ctx.fpr[16] - aot_fpr_14;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(304), std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[17] = ctx.fpr[17] - ctx.fpr[15];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(308), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(464), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(312), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(316), aot_gpr_5);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(312)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(316)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(468), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(288), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(292), aot_gpr_5);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(288)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(292)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), std::bit_cast<std::uint32_t>(aot_fpr_14));
    { const bool branch_taken = ctx.gpr[30] != 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
      if (branch_taken) {
          goto L_08918E68;
      }
      goto L_08918E38;
    }
L_08918E38:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    { const float fs = aot_fpr_20; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
        goto L_08918E7C;
    }
    goto L_08918E60;
L_08918E60:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08918E94;
      }
      goto L_08918E68;
    }
L_08918E68:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08918E74u);
    aot_gpr_5 = (ctx.gpr[23] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 170u, 0x08918E74u, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918E74u) goto L_08918E74;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918E74:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (1024u << 16u);
      if (branch_taken) {
          goto L_08918ED0;
      }
      goto L_08918E7C;
    }
L_08918E7C:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_20 = std::sqrt(aot_fpr_20);
    ctx.gpr[18] = (ctx.gpr[23] | 0u);
    goto L_08918E94;
L_08918E94:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[21] + static_cast<std::uint32_t>(565)));
    ctx.gpr[22] = (ctx.gpr[22] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[22]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918B60;
      }
      goto L_08918EAC;
    }
L_08918EAC:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_29 + static_cast<std::uint32_t>(96))))));
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[20]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918B10;
      }
      goto L_08918EC0;
    }
L_08918EC0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08918ECCu);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 175u, 0x08918ECCu, 0x08910440u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 69u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 69u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 69u, 0x08910440u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918ECCu) goto L_08918ECC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918ECC:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    goto L_08918ED0;
L_08918ED0:
    { std::uint32_t aot_run_words[13]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(492), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      aot_gpr_16 = aot_run_words[3];
      ctx.gpr[17] = aot_run_words[4];
      ctx.gpr[18] = aot_run_words[5];
      ctx.gpr[19] = aot_run_words[6];
      ctx.gpr[20] = aot_run_words[7];
      ctx.gpr[21] = aot_run_words[8];
      ctx.gpr[22] = aot_run_words[9];
      ctx.gpr[23] = aot_run_words[10];
      ctx.gpr[30] = aot_run_words[11];
      aot_gpr_31 = aot_run_words[12];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(544));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08918F0C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-208));
    { const std::uint32_t aot_run_words[12]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(ctx.fpr[22]), aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], ctx.gpr[30], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(160), aot_run_words); }
    aot_gpr_31 = (0x08918F48u);
    ctx.gpr[19] = (aot_gpr_4 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 178u, 0x08918F48u, 0x08906DE4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 620u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 620u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 620u, 0x08906DE4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918F48u) goto L_08918F48;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918F48:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08918FAC;
      }
      goto L_08918F50;
    }
L_08918F50:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(2196)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08918FAC;
      }
      goto L_08918F5C;
    }
L_08918F5C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(2276)));
    aot_gpr_5 = (0u | 2u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08918FAC;
      }
      goto L_08918F6C;
    }
L_08918F6C:
    aot_gpr_31 = (0x08918F74u);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 182u, 0x08918F74u, 0x0890D498u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 324u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 324u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 324u, 0x0890D498u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918F74u) goto L_08918F74;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918F74:
    aot_gpr_31 = (0x08918F7Cu);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 183u, 0x08918F7Cu, 0x0890D654u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 344u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 344u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 344u, 0x0890D654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918F7Cu) goto L_08918F7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918F7C:
    aot_gpr_31 = (0x08918F84u);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0073.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 184u, 0x08918F84u, 0x089292D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0073_entry(rt, ctx, 287u, aot_mem);
#else
        recomp_unit_0073_entry(rt, ctx, 287u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0073_entry, 73u, 287u, 0x089292D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918F84u) goto L_08918F84;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08918F84:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08918FA4;
      }
      goto L_08918F8C;
    }
L_08918F8C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(1360)));
    aot_gpr_5 = (0u | 18u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08918FB4;
      }
      goto L_08918F9C;
    }
L_08918F9C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_089192A8;
      }
      goto L_08918FA4;
    }
L_08918FA4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_08918FAC;
    }
L_08918FAC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_08918FB4;
    }
L_08918FB4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(1752)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_089192A8;
      }
      goto L_08918FC8;
    }
L_08918FC8:
    aot_gpr_31 = (0x08918FD0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08918FD0u) goto L_08918FD0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08918FD0:
    aot_gpr_4 = (ctx.gpr[2] & 65535u);
    aot_gpr_5 = (0u | 100u);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_4); const std::int32_t divisor = static_cast<std::int32_t>(aot_gpr_5); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_4 = (ctx.hi);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 10 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919148;
      }
      goto L_08918FEC;
    }
L_08918FEC:
    ctx.gpr[17] = (0u + static_cast<std::uint32_t>(-1));
    ctx.gpr[18] = (0u | 0u);
    aot_gpr_4 = (ctx.gpr[19] + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (16672u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    aot_gpr_5 = (0u | 1u);
    ctx.gpr[7] = (0u | 6u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 1u);
    ctx.gpr[11] = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_31 = (0x08919038u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0037.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 193u, 0x08919038u, 0x08899758u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0037_entry(rt, ctx, 287u, aot_mem);
#else
        recomp_unit_0037_entry(rt, ctx, 287u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0037_entry, 37u, 287u, 0x08899758u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919038u) goto L_08919038;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919038:
    aot_gpr_16 = (0u | 0u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_29 + static_cast<std::uint32_t>(48))))));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_16) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (15820u << 16u);
      if (branch_taken) {
          goto L_089190F0;
      }
      goto L_0891904C;
    }
L_0891904C:
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    ctx.gpr[20] = (aot_gpr_29 | 0u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[22] = (0u | 2u);
    goto L_0891905C;
L_0891905C:
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[21] + static_cast<std::uint32_t>(612)));
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[22];
      if (branch_taken) {
          goto L_089190A4;
      }
      goto L_0891906C;
    }
L_0891906C:
    aot_gpr_4 = (ctx.gpr[21] + static_cast<std::uint32_t>(320));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 0u, 0u, 3u>();
    ctx.execute_vfpu_unary_ct<28u, 28u, 1u, 22u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<28u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089190A4;
      }
      goto L_08919094;
    }
L_08919094:
    aot_gpr_31 = (0x0891909Cu);
    aot_gpr_4 = (ctx.gpr[21] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0191.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 198u, 0x0891909Cu, 0x08B01AA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0191_entry(rt, ctx, 439u, aot_mem);
#else
        recomp_unit_0191_entry(rt, ctx, 439u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0191_entry, 191u, 439u, 0x08B01AA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891909Cu) goto L_0891909C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891909C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_089190AC;
      }
      goto L_089190A4;
    }
L_089190A4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_089190DC;
      }
      goto L_089190AC;
    }
L_089190AC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(852)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_089190DC;
      }
      goto L_089190B8;
    }
L_089190B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(352)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(144)));
    aot_gpr_4 = (ctx.gpr[18] < aot_gpr_4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_089190DC;
      }
      goto L_089190CC;
    }
L_089190CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(352)));
    ctx.gpr[17] = (aot_gpr_16 << 16u);
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(144)));
    ctx.gpr[17] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[17]) >> 16u));
    goto L_089190DC;
L_089190DC:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_29 + static_cast<std::uint32_t>(48))))));
    aot_gpr_16 = (aot_gpr_16 + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_16) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_0891905C;
      }
      goto L_089190F0;
    }
L_089190F0:
    aot_gpr_4 = (ctx.gpr[18] < static_cast<std::uint32_t>(2001) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_gpr_4 = (0u + static_cast<std::uint32_t>(-1));
      if (branch_taken) {
          goto L_08919138;
      }
      goto L_089190FC;
    }
L_089190FC:
    { const bool branch_taken = ctx.gpr[17] == aot_gpr_4;
      if (branch_taken) {
          goto L_08919138;
      }
      goto L_08919104;
    }
L_08919104:
    aot_gpr_4 = (ctx.gpr[17] << 2u);
    aot_gpr_4 = (aot_gpr_29 + aot_gpr_4);
    ctx.gpr[17] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(16)));
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_08919138;
      }
      goto L_08919118;
    }
L_08919118:
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    aot_gpr_5 = (0u | 18u);
    aot_gpr_31 = (0x08919128u);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 208u, 0x08919128u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919128u) goto L_08919128;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919128:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(5000));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[19] + static_cast<std::uint32_t>(1752), aot_gpr_4);
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_08919138;
    }
L_08919138:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(5000));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[19] + static_cast<std::uint32_t>(1752), aot_gpr_4);
      if (branch_taken) {
          goto L_089192A8;
      }
      goto L_08919148;
    }
L_08919148:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(2196)));
    aot_gpr_5 = (0u | 37u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_089192A8;
      }
      goto L_08919158;
    }
L_08919158:
    aot_gpr_31 = (0x08919160u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919160u) goto L_08919160;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919160:
    aot_gpr_4 = (ctx.gpr[2] & 65535u);
    aot_gpr_4 = (aot_gpr_4 & 7u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_089192A8;
      }
      goto L_08919170;
    }
L_08919170:
    aot_gpr_4 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(156), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load16(ctx.gpr[19] + static_cast<std::uint32_t>(1914)));
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (16964u << 16u);
      if (branch_taken) {
          goto L_08919280;
      }
      goto L_0891918C;
    }
L_0891918C:
    ctx.gpr[30] = (ctx.gpr[19] + static_cast<std::uint32_t>(48));
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[18] = (aot_gpr_29 + static_cast<std::uint32_t>(80));
    ctx.gpr[23] = (0u | 4u);
    ctx.gpr[22] = (0u | 5u);
    ctx.gpr[21] = (0u | 19u);
    ctx.gpr[20] = (0u | 20u);
    goto L_089191A8;
L_089191A8:
    aot_gpr_16 = (ctx.gpr[17] << 2u);
    aot_gpr_16 = (ctx.gpr[19] + aot_gpr_16);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = ctx.gpr[30] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 1u>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[18] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = ctx.gpr[18] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 0u, 0u, 3u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<28u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_089191F0;
      }
      goto L_089191E8;
    }
L_089191E8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08919280;
      }
      goto L_089191F0;
    }
L_089191F0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1360)));
    aot_gpr_4 = (aot_gpr_4 & 65535u);
    if (aot_gpr_4 == ctx.gpr[23]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
        goto L_08919228;
    }
    goto L_08919204;
L_08919204:
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[22];
    aot_gpr_5 = (0u | 18u);
      if (branch_taken) {
          goto L_08919224;
      }
      goto L_0891920C;
    }
L_0891920C:
    if (aot_gpr_4 == aot_gpr_5) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
        goto L_08919228;
    }
    goto L_08919214;
L_08919214:
    if (aot_gpr_4 == ctx.gpr[21]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
        goto L_08919228;
    }
    goto L_0891921C;
L_0891921C:
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[20];
      if (branch_taken) {
          goto L_08919268;
      }
      goto L_08919224;
    }
L_08919224:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    goto L_08919228;
L_08919228:
    aot_gpr_5 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(2276)));
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08919268;
      }
      goto L_08919238;
    }
L_08919238:
    aot_gpr_31 = (0x08919240u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 225u, 0x08919240u, 0x08906E38u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 627u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 627u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 627u, 0x08906E38u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919240u) goto L_08919240;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919240:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08919268;
      }
      goto L_08919248;
    }
L_08919248:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    aot_gpr_5 = (0u | 37u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(2196)));
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_08919268;
      }
      goto L_0891925C;
    }
L_0891925C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1784)));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(156), aot_gpr_4);
      if (branch_taken) {
          goto L_08919280;
      }
      goto L_08919268;
    }
L_08919268:
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (aot_mem.aot_direct_load16(ctx.gpr[19] + static_cast<std::uint32_t>(1914)));
    ctx.gpr[17] = (ctx.gpr[17] & 65535u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_089191A8;
      }
      goto L_08919280;
    }
L_08919280:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(156)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891929C;
      }
      goto L_0891928C;
    }
L_0891928C:
    aot_gpr_6 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x0891929Cu);
    aot_gpr_5 = (0u | 37u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 231u, 0x0891929Cu, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891929Cu) goto L_0891929C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891929C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(5000));
    aot_mem.aot_direct_store32(ctx.gpr[19] + static_cast<std::uint32_t>(1752), aot_gpr_4);
    goto L_089192A8;
L_089192A8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(2228)));
    ctx.gpr[17] = (0u | 5u);
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[17];
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_089192B8;
    }
L_089192B8:
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_20 = std::bit_cast<float>(0u);
    aot_gpr_31 = (0x089192C8u);
    ctx.fpr[22] = std::bit_cast<float>(aot_gpr_4);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20998, 199u, 113u, 0x08B20998u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 113u, 0x08B20998u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089192C8u) goto L_089192C8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089192C8:
    aot_fpr_12 = ctx.fpr[22] - aot_fpr_20;
    aot_gpr_4 = (16128u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[0]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_20 + aot_fpr_12;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891943C;
      }
      goto L_089192EC;
    }
L_089192EC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(1740)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_08919300;
    }
L_08919300:
    aot_gpr_4 = (aot_mem.aot_direct_load16(ctx.gpr[19] + static_cast<std::uint32_t>(1914)));
    ctx.gpr[18] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (16358u << 16u);
      if (branch_taken) {
          goto L_08919434;
      }
      goto L_08919314;
    }
L_08919314:
    aot_gpr_4 = (aot_gpr_4 | 26214u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_16 = (ctx.gpr[19] + static_cast<std::uint32_t>(48));
    aot_gpr_4 = (16262u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 2706u);
    ctx.gpr[22] = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    ctx.fpr[22] = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[23] = (0u | 4000u);
    ctx.gpr[20] = (ctx.gpr[19] | 0u);
    goto L_08919338;
L_08919338:
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(1784)));
    { const bool branch_taken = ctx.gpr[21] == 0u;
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_08919344;
    }
L_08919344:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(2228)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[17];
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_08919350;
    }
L_08919350:
    aot_gpr_4 = (ctx.gpr[21] + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_16 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 1u>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[22] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = ctx.gpr[22] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 0u, 0u, 3u>();
    ctx.execute_vfpu_unary_ct<28u, 28u, 1u, 22u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<28u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_08919388;
    }
L_08919388:
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    aot_gpr_31 = (0x08919398u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0105.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 242u, 0x08919398u, 0x089AB048u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0105_entry(rt, ctx, 627u, aot_mem);
#else
        recomp_unit_0105_entry(rt, ctx, 627u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0105_entry, 105u, 627u, 0x089AB048u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919398u) goto L_08919398;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919398:
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_gpr_4 = (ctx.gpr[21] | 0u);
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_089193A0;
    }
L_089193A0:
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    aot_gpr_31 = (0x089193ACu);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0105.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 244u, 0x089193ACu, 0x089AB048u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0105_entry(rt, ctx, 627u, aot_mem);
#else
        recomp_unit_0105_entry(rt, ctx, 627u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0105_entry, 105u, 627u, 0x089AB048u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089193ACu) goto L_089193AC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089193AC:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_089193B4;
    }
L_089193B4:
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x089193C0u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 246u, 0x089193C0u, 0x0890C4D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 72u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 72u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 72u, 0x0890C4D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089193C0u) goto L_089193C0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089193C0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_089193C8;
    }
L_089193C8:
    aot_gpr_31 = (0x089193D0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089193D0u) goto L_089193D0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089193D0:
    aot_gpr_4 = (ctx.gpr[2] & 65535u);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_4); const std::int32_t divisor = static_cast<std::int32_t>(ctx.gpr[23]); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[19] + static_cast<std::uint32_t>(1360)));
    ctx.gpr[30] = (ctx.hi);
    aot_gpr_31 = (0x089193E8u);
    ctx.gpr[30] = (ctx.gpr[30] + static_cast<std::uint32_t>(10000));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 249u, 0x089193E8u, 0x08910B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 194u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 194u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 194u, 0x08910B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089193E8u) goto L_089193E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089193E8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_089193F0;
    }
L_089193F0:
    aot_gpr_31 = (0x089193F8u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(1360)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 251u, 0x089193F8u, 0x08910B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 194u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 194u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 194u, 0x08910B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089193F8u) goto L_089193F8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089193F8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08919420;
      }
      goto L_08919400;
    }
L_08919400:
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    aot_gpr_31 = (0x08919410u);
    aot_gpr_6 = (ctx.gpr[30] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 253u, 0x08919410u, 0x0890C42Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 65u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 65u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 65u, 0x0890C42Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919410u) goto L_08919410;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919410:
    aot_gpr_4 = (ctx.gpr[21] | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x08919420u);
    aot_gpr_6 = (ctx.gpr[30] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0066.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 254u, 0x08919420u, 0x0890C42Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0066_entry(rt, ctx, 65u, aot_mem);
#else
        recomp_unit_0066_entry(rt, ctx, 65u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0066_entry, 66u, 65u, 0x0890C42Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919420u) goto L_08919420;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919420:
    aot_gpr_4 = (aot_mem.aot_direct_load16(ctx.gpr[19] + static_cast<std::uint32_t>(1914)));
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08919338;
      }
      goto L_08919434;
    }
L_08919434:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08919448;
      }
      goto L_0891943C;
    }
L_0891943C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(200));
    aot_mem.aot_direct_store32(ctx.gpr[19] + static_cast<std::uint32_t>(1740), aot_gpr_4);
    goto L_08919448;
L_08919448:
    { std::uint32_t aot_run_words[12]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(160), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      ctx.gpr[18] = aot_run_words[4];
      ctx.gpr[19] = aot_run_words[5];
      ctx.gpr[20] = aot_run_words[6];
      ctx.gpr[21] = aot_run_words[7];
      ctx.gpr[22] = aot_run_words[8];
      ctx.gpr[23] = aot_run_words[9];
      ctx.gpr[30] = aot_run_words[10];
      aot_gpr_31 = aot_run_words[11];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(208));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08919480:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-96));
    { const std::uint32_t aot_run_words[8]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(ctx.fpr[22]), std::bit_cast<std::uint32_t>(ctx.fpr[24]), std::bit_cast<std::uint32_t>(ctx.fpr[26]), aot_gpr_16, ctx.gpr[17], ctx.gpr[18], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(52), aot_run_words); }
    aot_gpr_31 = (0x089194ACu);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 259u, 0x089194ACu, 0x08906DE4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 620u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 620u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 620u, 0x08906DE4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089194ACu) goto L_089194AC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089194AC:
    ctx.gpr[17] = (2236u << 16u);
    aot_gpr_4 = (16128u << 16u);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(32304));
    ctx.fpr[26] = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = ctx.gpr[2] == 0u;
    ctx.gpr[18] = (ctx.gpr[17] + static_cast<std::uint32_t>(112));
      if (branch_taken) {
          goto L_08919708;
      }
      goto L_089194C4;
    }
L_089194C4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x089194DCu);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089194DCu) goto L_089194DC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089194DC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(36)));
    aot_gpr_5 = (aot_gpr_16 | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(24));
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = ctx.gpr[7];
    aot_gpr_31 = (0x089194F8u);
    aot_gpr_4 = (ctx.gpr[2] + aot_gpr_6);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089194F8u) goto L_089194F8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089194F8:
    aot_gpr_4 = (0u | 5u);
    if (ctx.gpr[2] == aot_gpr_4) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
        goto L_08919538;
    }
    goto L_08919504;
L_08919504:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x0891951Cu);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891951Cu) goto L_0891951C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891951C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 1024u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919708;
      }
      goto L_08919534;
    }
L_08919534:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    goto L_08919538;
L_08919538:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x0891954Cu);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891954Cu) goto L_0891954C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891954C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(36)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(8));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x08919564u);
    aot_gpr_4 = (ctx.gpr[2] + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919564u) goto L_08919564;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919564:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-30640)));
    aot_gpr_5 = (ctx.gpr[2] << 6u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(44))))));
    aot_gpr_4 = (aot_gpr_4 & 4u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_gpr_4 = (16457u << 16u);
      if (branch_taken) {
          goto L_08919708;
      }
      goto L_0891958C;
    }
L_0891958C:
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2264)));
    ctx.fpr[22] = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (17204u << 16u);
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[24] = aot_fpr_12 / aot_fpr_13;
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x089195CCu);
    { const float fs = ctx.fpr[24]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[24] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[24] = fs * ft; }
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089195CCu) goto L_089195CC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089195CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 1024u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
      if (branch_taken) {
          goto L_089195F4;
      }
      goto L_089195E4;
    }
L_089195E4:
    aot_gpr_4 = (15948u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = ctx.fpr[24]; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[24] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[24] = fs * ft; }
    goto L_089195F4;
L_089195F4:
    aot_gpr_31 = (0x089195FCu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 273u, 0x089195FCu, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089195FCu) goto L_089195FC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089195FC:
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2256), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_31 = (0x08919608u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 274u, 0x08919608u, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919608u) goto L_08919608;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919608:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_13 = aot_fpr_12 + ctx.fpr[22];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919638;
      }
      goto L_08919624;
    }
L_08919624:
    aot_gpr_4 = (16585u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    aot_fpr_20 = aot_fpr_20 - aot_fpr_13;
      if (branch_taken) {
          goto L_0891965C;
      }
      goto L_08919638;
    }
L_08919638:
    aot_fpr_13 = aot_fpr_12 - ctx.fpr[22];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891965C;
      }
      goto L_0891964C;
    }
L_0891964C:
    aot_gpr_4 = (16585u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_20 = aot_fpr_20 + aot_fpr_13;
    goto L_0891965C;
L_0891965C:
    aot_fpr_13 = aot_fpr_20 - aot_fpr_12;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= ctx.fpr[24])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919678;
      }
      goto L_08919670;
    }
L_08919670:
    { const bool branch_taken = 0u == 0u;
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[24];
      if (branch_taken) {
          goto L_0891969C;
      }
      goto L_08919678;
    }
L_08919678:
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[24]) ^ 0x80000000u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < ctx.fpr[24])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919694;
      }
      goto L_0891968C;
    }
L_0891968C:
    { const bool branch_taken = 0u == 0u;
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[24];
      if (branch_taken) {
          goto L_0891969C;
      }
      goto L_08919694;
    }
L_08919694:
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = aot_fpr_12 + ctx.fpr[24];
    goto L_0891969C;
L_0891969C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2256), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x089196B8u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089196B8u) goto L_089196B8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_089196B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 1024u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919708;
      }
      goto L_089196D0;
    }
L_089196D0:
    aot_gpr_31 = (0x089196D8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 286u, 0x089196D8u, 0x08907AECu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 839u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 839u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 839u, 0x08907AECu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089196D8u) goto L_089196D8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089196D8:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x089196E8u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 287u, 0x089196E8u, 0x08AD120Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 227u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 227u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 227u, 0x08AD120Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089196E8u) goto L_089196E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089196E8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919708;
      }
      goto L_089196F4;
    }
L_089196F4:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_fpr_12 = aot_fpr_12 - ctx.fpr[24];
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2256), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08919708;
L_08919708:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919740;
      }
      goto L_08919720;
    }
L_08919720:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(792)));
    aot_gpr_4 = (16384u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(796)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(792), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(796), std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_08919740;
L_08919740:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 17u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08919778;
      }
      goto L_08919750;
    }
L_08919750:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x08919768u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919768u) goto L_08919768;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919768:
    aot_gpr_6 = (aot_gpr_16 + static_cast<std::uint32_t>(792));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08919778u);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0006.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 294u, 0x08919778u, 0x0881EF14u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0006_entry(rt, ctx, 517u, aot_mem);
#else
        recomp_unit_0006_entry(rt, ctx, 517u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0006_entry, 6u, 517u, 0x0881EF14u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919778u) goto L_08919778;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919778:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(792)));
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(796)));
    { const float fs = ctx.fpr[15]; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = aot_fpr_13 - aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_13; const float ft = ctx.fpr[16]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = ctx.fpr[15]; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(468)));
    aot_gpr_5 = (16384u << 16u);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919968;
      }
      goto L_0891982C;
    }
L_0891982C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2220)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-33));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[15] <= aot_fpr_14)) ? 0x00800000u : 0u);
    aot_gpr_6 = (0u | 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.gpr[7] = (ctx.gpr[7] & aot_gpr_5);
      if (branch_taken) {
          goto L_08919890;
      }
      goto L_08919850;
    }
L_08919850:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2212)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[15] < ctx.fpr[16])) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_6 = (0u | 1u);
        goto L_08919894;
    }
    goto L_08919864;
L_08919864:
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2224)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[15] <= ctx.fpr[16])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_gpr_6 = (0u | 1u);
        goto L_08919894;
    }
    goto L_0891987C;
L_0891987C:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2216)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[15] < ctx.fpr[16])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919894;
      }
      goto L_08919890;
    }
L_08919890:
    aot_gpr_6 = (0u | 1u);
    goto L_08919894;
L_08919894:
    aot_gpr_6 = (aot_gpr_6 & 255u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), ctx.gpr[7]);
      if (branch_taken) {
          goto L_08919968;
      }
      goto L_089198A0;
    }
L_089198A0:
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    ctx.fpr[15] = std::bit_cast<float>(0u);
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.fpr[16] = ctx.fpr[16] + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    ctx.fpr[17] = ctx.fpr[17] + aot_fpr_13;
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_6 = (0u | 0u);
    aot_gpr_5 = (ctx.gpr[7] & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(ctx.fpr[16]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] < aot_fpr_14)) ? 0x00800000u : 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(ctx.fpr[17]));
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2224)));
      if (branch_taken) {
          goto L_089198EC;
      }
      goto L_089198D8;
    }
L_089198D8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (0u | 1u);
    aot_fpr_12 = aot_fpr_14 - aot_fpr_12;
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_08919910;
      }
      goto L_089198EC;
    }
L_089198EC:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2212)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[16] <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919910;
      }
      goto L_08919900;
    }
L_08919900:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (0u | 1u);
    aot_fpr_12 = aot_fpr_14 - aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08919910;
L_08919910:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[17] < ctx.fpr[15])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919934;
      }
      goto L_08919920;
    }
L_08919920:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (0u | 1u);
    aot_fpr_13 = ctx.fpr[15] - aot_fpr_13;
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_08919958;
      }
      goto L_08919934;
    }
L_08919934:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2216)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[17] <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919958;
      }
      goto L_08919948;
    }
L_08919948:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (0u | 1u);
    aot_fpr_13 = aot_fpr_14 - aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_08919958;
L_08919958:
    aot_gpr_4 = (aot_gpr_6 & 1u);
    aot_gpr_4 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_5 | aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    goto L_08919968;
L_08919968:
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 < ctx.fpr[26])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16256u << 16u);
      if (branch_taken) {
          goto L_089199B0;
      }
      goto L_0891997C;
    }
L_0891997C:
    // VCS_HIGH_FPS_ROOT_MOTION: match CTimer's elapsed-time floor removal.
    { const float scale = vcs::game_root_motion_scale(aot_fpr_14, ctx.fpr[26], vcs::unlocked_game_timing_enabled());
      aot_fpr_12 *= scale;
      aot_fpr_13 *= scale; }
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_4 << 5u);
    aot_gpr_5 = (0u - aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    ctx.gpr[17] = (aot_gpr_5 + aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[17] = (ctx.gpr[17] + ctx.gpr[18]);
      if (branch_taken) {
          goto L_089199E8;
      }
      goto L_089199B0;
    }
L_089199B0:
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = ctx.fpr[15] / aot_fpr_14;
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_4 << 5u);
    aot_gpr_5 = (0u - aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    ctx.gpr[17] = (aot_gpr_5 + aot_gpr_4);
    ctx.gpr[17] = (ctx.gpr[17] + ctx.gpr[18]);
    goto L_089199E8;
L_089199E8:
    aot_gpr_31 = (0x089199F0u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0101.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 314u, 0x089199F0u, 0x089990E0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0101_entry(rt, ctx, 119u, aot_mem);
#else
        recomp_unit_0101_entry(rt, ctx, 119u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0101_entry, 101u, 119u, 0x089990E0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x089199F0u) goto L_089199F0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_089199F0:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08919A08;
      }
      goto L_089199F8;
    }
L_089199F8:
    aot_gpr_31 = (0x08919A00u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0101.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 316u, 0x08919A00u, 0x08999134u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0101_entry(rt, ctx, 124u, aot_mem);
#else
        recomp_unit_0101_entry(rt, ctx, 124u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0101_entry, 101u, 124u, 0x08999134u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919A00u) goto L_08919A00;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919A00:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919A08;
    }
L_08919A08:
    aot_gpr_31 = (0x08919A10u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_compact_generated_leaf<&vcs_compact_leaf_08960424, 87u, 0x08960424u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919A10u) goto L_08919A10;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919A10:
    { const bool branch_taken = aot_gpr_16 != ctx.gpr[2];
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919A18;
    }
L_08919A18:
    aot_gpr_31 = (0x08919A20u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 320u, 0x08919A20u, 0x08906FA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 668u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 668u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 668u, 0x08906FA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919A20u) goto L_08919A20;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919A20:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919A28;
    }
L_08919A28:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_31 = (0x08919A34u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 322u, 0x08919A34u, 0x0890675Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 544u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 544u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 544u, 0x0890675Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919A34u) goto L_08919A34;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919A34:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1944)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    ctx.fpr[22] = aot_fpr_12 + aot_fpr_13;
    ctx.fpr[22] = std::sqrt(ctx.fpr[22]);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_31 = (0x08919A5Cu);
    aot_fpr_12 = aot_fpr_20 - aot_fpr_12;
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 323u, 0x08919A5Cu, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919A5Cu) goto L_08919A5C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919A5C:
    aot_gpr_4 = (49097u << 16u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (48991u << 16u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    aot_gpr_4 = (aot_gpr_4 | 26355u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.fpr[24] = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_08919A94;
      }
      goto L_08919A80;
    }
L_08919A80:
    aot_gpr_4 = (16457u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
      if (branch_taken) {
          goto L_08919AC0;
      }
      goto L_08919A94;
    }
L_08919A94:
    aot_gpr_4 = (16329u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919AC0;
      }
      goto L_08919AB0;
    }
L_08919AB0:
    aot_gpr_4 = (16457u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    goto L_08919AC0;
L_08919AC0:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= ctx.fpr[24])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16223u << 16u);
      if (branch_taken) {
          goto L_08919B34;
      }
      goto L_08919AD0;
    }
L_08919AD0:
    aot_gpr_4 = (aot_gpr_4 | 26355u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919B34;
      }
      goto L_08919AE8;
    }
L_08919AE8:
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_20));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 18u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1940), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_20));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    { const float vfpu_constant = std::bit_cast<float>(0x3F22F983u);
      const float vfpu_value[4]{vfpu_constant, vfpu_constant, vfpu_constant, vfpu_constant};
      ctx.write_vfpu_vector_with_destination_prefix_ct<32u, 1u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<64u, 0u, 32u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<1u, 64u, 1u, 19u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<1u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[22]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(1944), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_08919B34;
L_08919B34:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08919B44u);
    aot_gpr_6 = (0u | 6u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 331u, 0x08919B44u, 0x08908C10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 216u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 216u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 216u, 0x08908C10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919B44u) goto L_08919B44;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919B44:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08919B80;
      }
      goto L_08919B50;
    }
L_08919B50:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08919B60u);
    aot_gpr_6 = (0u | 217u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 333u, 0x08919B60u, 0x08908C10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 216u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 216u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 216u, 0x08908C10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919B60u) goto L_08919B60;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919B60:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08919B80;
      }
      goto L_08919B6C;
    }
L_08919B6C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08919B7Cu);
    aot_gpr_6 = (0u | 44u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 335u, 0x08919B7Cu, 0x08908C10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 216u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 216u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 216u, 0x08908C10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919B7Cu) goto L_08919B7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919B7C:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    goto L_08919B80;
L_08919B80:
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919B9C;
      }
      goto L_08919B88;
    }
L_08919B88:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(16)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < ctx.fpr[26])) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919B9C;
    }
L_08919B9C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_31 = (0x08919BA8u);
    aot_fpr_12 = aot_fpr_20 - aot_fpr_12;
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 339u, 0x08919BA8u, 0x08A8F0B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 580u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 580u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 580u, 0x08A8F0B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919BA8u) goto L_08919BA8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919BA8:
    aot_gpr_4 = (16351u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26355u);
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
      if (branch_taken) {
          goto L_08919BE0;
      }
      goto L_08919BC8;
    }
L_08919BC8:
    aot_gpr_4 = (16457u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_20 = aot_fpr_20 - aot_fpr_12;
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_20));
      if (branch_taken) {
          goto L_08919C0C;
      }
      goto L_08919BE0;
    }
L_08919BE0:
    aot_gpr_4 = (49119u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26355u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16457u << 16u);
      if (branch_taken) {
          goto L_08919C0C;
      }
      goto L_08919BFC;
    }
L_08919BFC:
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_20 = aot_fpr_20 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_20));
    goto L_08919C0C;
L_08919C0C:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 <= ctx.fpr[24])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16223u << 16u);
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919C1C;
    }
L_08919C1C:
    aot_gpr_4 = (aot_gpr_4 | 26355u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (17204u << 16u);
      if (branch_taken) {
          goto L_08919CB0;
      }
      goto L_08919C34;
    }
L_08919C34:
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (16457u << 16u);
    { const float fs = aot_fpr_20; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (aot_gpr_4 | 4059u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[22] = aot_fpr_12 / aot_fpr_13;
    aot_gpr_4 = (49024u << 16u);
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(728)));
    ctx.gpr[17] = (aot_gpr_29 + static_cast<std::uint32_t>(40));
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_6 = (0u | 1u);
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[7] + static_cast<std::uint32_t>(16)));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    aot_gpr_31 = (0x08919C88u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(8));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0089.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 346u, 0x08919C88u, 0x08968D2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0089_entry(rt, ctx, 220u, aot_mem);
#else
        recomp_unit_0089_entry(rt, ctx, 220u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0089_entry, 89u, 220u, 0x08968D2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919C88u) goto L_08919C88;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919C88:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(732)));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(16)));
    aot_gpr_6 = (0u | 1u);
    aot_gpr_31 = (0x08919CA4u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(8));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0089.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 347u, 0x08919CA4u, 0x08968D2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0089_entry(rt, ctx, 220u, aot_mem);
#else
        recomp_unit_0089_entry(rt, ctx, 220u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0089_entry, 89u, 220u, 0x08968D2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919CA4u) goto L_08919CA4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919CA4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(464)));
    aot_gpr_4 = (aot_gpr_4 | 8192u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(464), aot_gpr_4);
    goto L_08919CB0;
L_08919CB0:
    { std::uint32_t aot_run_words[8]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(52), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      ctx.fpr[26] = std::bit_cast<float>(aot_run_words[3]);
      aot_gpr_16 = aot_run_words[4];
      ctx.gpr[17] = aot_run_words[5];
      ctx.gpr[18] = aot_run_words[6];
      aot_gpr_31 = aot_run_words[7];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08919CD8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-272));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(456)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(244), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_gpr_5 & 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(236), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(240), std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    { const std::uint32_t aot_run_words[4]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(248), aot_run_words); }
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08919D24;
      }
      goto L_08919D0C;
    }
L_08919D0C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919DC8;
      }
      goto L_08919D24;
    }
L_08919D24:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2120)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08919DC0;
      }
      goto L_08919D30;
    }
L_08919D30:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(92)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(216));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_6;
    aot_gpr_31 = (0x08919D48u);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_5);
    ctx.pc = jump_target;
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_call(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919D48u) goto L_08919D48;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919D48:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (aot_gpr_4 & 1024u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08919DA4;
      }
      goto L_08919D60;
    }
L_08919D60:
    ctx.gpr[19] = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    aot_gpr_31 = (0x08919D7Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0022.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 355u, 0x08919D7Cu, 0x0885FA00u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0022_entry(rt, ctx, 921u, aot_mem);
#else
        recomp_unit_0022_entry(rt, ctx, 921u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0022_entry, 22u, 921u, 0x0885FA00u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919D7Cu) goto L_08919D7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919D7C:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_31 = (0x08919D90u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0023.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 356u, 0x08919D90u, 0x08860094u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0023_entry(rt, ctx, 4u, aot_mem);
#else
        recomp_unit_0023_entry(rt, ctx, 4u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0023_entry, 23u, 4u, 0x08860094u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919D90u) goto L_08919D90;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919D90:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(320));
      if (branch_taken) {
          goto L_08919DD0;
      }
      goto L_08919D9C;
    }
L_08919D9C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A014;
      }
      goto L_08919DA4;
    }
L_08919DA4:
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x08919DB8u);
    aot_fpr_14 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 359u, 0x08919DB8u, 0x08A6669Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 537u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 537u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 537u, 0x08A6669Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919DB8u) goto L_08919DB8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919DB8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_08919DC0;
    }
L_08919DC0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_08919DC8;
    }
L_08919DC8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_08919DD0;
    }
L_08919DD0:
    aot_gpr_31 = (0x08919DD8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919DD8u) goto L_08919DD8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919DD8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
    ctx.gpr[18] = (aot_gpr_29 + static_cast<std::uint32_t>(48));
      if (branch_taken) {
          goto L_08919EFC;
      }
      goto L_08919DE0;
    }
L_08919DE0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 4u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919EFC;
      }
      goto L_08919E00;
    }
L_08919E00:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08919EFC;
      }
      goto L_08919E14;
    }
L_08919E14:
    { const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.gpr[19] = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
    aot_gpr_4 = (16261u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 7864u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(104), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(48));
    aot_gpr_31 = (0x08919E48u);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0163.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 367u, 0x08919E48u, 0x08A931D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0163_entry(rt, ctx, 464u, aot_mem);
#else
        recomp_unit_0163_entry(rt, ctx, 464u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0163_entry, 163u, 464u, 0x08A931D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919E48u) goto L_08919E48;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919E48:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(112));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<2u, 4u>(vfpu_value); }
    ctx.execute_vfpu_cross_quat_ct<0u, 1u, 2u, 3u>();
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(80));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(320));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 0u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
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
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(112));
    { const std::uint32_t vfpu_address = aot_gpr_6 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 0u, 0u, 3u>();
    aot_gpr_6 = (ctx.vfpu_scalar_bits_ct<28u>());
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_6);
    { const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
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
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_6);
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
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_31 = (0x08919ED8u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_compact_generated_leaf<&vcs_compact_leaf_08A931B8, 163u, 0x08A931B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919ED8u) goto L_08919ED8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919ED8:
    { const std::uint32_t vfpu_address = ctx.gpr[17] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(120), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x08919EF4u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 369u, 0x08919EF4u, 0x08A666C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 538u, 0x08A666C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919EF4u) goto L_08919EF4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919EF4:
    { const bool branch_taken = 0u == 0u;
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
      if (branch_taken) {
          goto L_08919FB0;
      }
      goto L_08919EFC;
    }
L_08919EFC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    ctx.gpr[19] = (aot_gpr_29 + static_cast<std::uint32_t>(128));
    aot_gpr_6 = (aot_gpr_16 + static_cast<std::uint32_t>(1312));
    aot_gpr_31 = (0x08919F10u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 371u, 0x08919F10u, 0x08905F10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 407u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 407u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 407u, 0x08905F10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F10u) goto L_08919F10;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08919F10:
    { const std::uint32_t vfpu_address = ctx.gpr[19] + static_cast<std::uint32_t>(0);
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
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 4u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08919FAC;
      }
      goto L_08919F38;
    }
L_08919F38:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_5 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08919FAC;
      }
      goto L_08919F4C;
    }
L_08919F4C:
    aot_gpr_31 = (0x08919F54u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F54u) goto L_08919F54;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919F54:
    ctx.gpr[19] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20084)));
    ctx.gpr[18] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20088)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    ctx.gpr[7] = (ctx.gpr[19] | 0u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08919F70u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F70u) goto L_08919F70;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919F70:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x08919F7Cu);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F7Cu) goto L_08919F7C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919F7C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    aot_gpr_31 = (0x08919F88u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F88u) goto L_08919F88;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919F88:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    ctx.gpr[7] = (ctx.gpr[19] | 0u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08919F9Cu);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919F9Cu) goto L_08919F9C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919F9C:
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x08919FA8u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08919FA8u) goto L_08919FA8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08919FA8:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08919FAC;
L_08919FAC:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
    goto L_08919FB0;
L_08919FB0:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1944)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(320)));
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[15];
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(324)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_fpr_12 = aot_fpr_12 - ctx.fpr[16];
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_fpr_13 = aot_fpr_14 - ctx.fpr[17];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(120)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2256)));
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[18]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2260)));
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2256), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(120)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = ctx.fpr[15] + aot_fpr_12;
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(2260), std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_0891A170;
      }
      goto L_0891A014;
    }
L_0891A014:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(311)));
    aot_gpr_5 = (0u | 26u);
    if (aot_gpr_4 != aot_gpr_5) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
        goto L_0891A154;
    }
    goto L_0891A024;
L_0891A024:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(288)));
    aot_fpr_20 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_20)) && aot_fpr_12 == aot_fpr_20)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(288)));
        goto L_0891A054;
    }
    goto L_0891A03C;
L_0891A03C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(292)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_20)) && aot_fpr_12 == aot_fpr_20)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
        goto L_0891A154;
    }
    goto L_0891A050;
L_0891A050:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(288)));
    goto L_0891A054;
L_0891A054:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(292)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(144), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[15] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[15] = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(148), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_13 = aot_fpr_14 + ctx.fpr[15];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_0891A088;
      }
      goto L_0891A080;
    }
L_0891A080:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(144), std::bit_cast<std::uint32_t>(aot_fpr_12));
      if (branch_taken) {
          goto L_0891A0BC;
      }
      goto L_0891A088;
    }
L_0891A088:
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    ctx.execute_vfpu_unary_ct<0u, 0u, 1u, 17u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<0u>());
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = ctx.fpr[15]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(144), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(148)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(148), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891A0BC;
L_0891A0BC:
    aot_gpr_5 = (47747u << 16u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_5 = (aot_gpr_5 | 4719u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0891A0D8u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_5);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 390u, 0x0891A0D8u, 0x08A6669Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 537u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 537u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 537u, 0x08A6669Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A0D8u) goto L_0891A0D8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A0D8:
    aot_gpr_4 = (15523u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(148)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[15] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[15] = fs * ft; }
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1940)));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1944)));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[15] = ctx.fpr[15] + ctx.fpr[16];
    aot_fpr_13 = aot_fpr_13 + ctx.fpr[17];
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_0891A14C;
      }
      goto L_0891A124;
    }
L_0891A124:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(148)));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_13 = aot_fpr_14 - aot_fpr_13;
    aot_fpr_12 = ctx.fpr[16] - aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891A14C;
L_0891A14C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A170;
      }
      goto L_0891A154;
    }
L_0891A154:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(320)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1944)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(324)));
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    aot_fpr_14 = aot_fpr_14 - ctx.fpr[15];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_14));
    goto L_0891A170;
L_0891A170:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891A28C;
      }
      goto L_0891A17C;
    }
L_0891A17C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 4u);
    if (aot_gpr_4 == 0u) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
        goto L_0891A1A8;
    }
    goto L_0891A190;
L_0891A190:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 8u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891A28C;
      }
      goto L_0891A1A4;
    }
L_0891A1A4:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    goto L_0891A1A8;
L_0891A1A8:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_13 = aot_fpr_12 + aot_fpr_13;
    aot_fpr_13 = std::sqrt(aot_fpr_13);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 57u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_0891A204;
      }
      goto L_0891A1CC;
    }
L_0891A1CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 4u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    if (aot_gpr_4 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
        goto L_0891A208;
    }
    goto L_0891A1EC;
L_0891A1EC:
    aot_gpr_4 = (15107u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 | 4719u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
      if (branch_taken) {
          goto L_0891A260;
      }
      goto L_0891A204;
    }
L_0891A204:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    goto L_0891A208;
L_0891A208:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 4u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891A24C;
      }
      goto L_0891A224;
    }
L_0891A224:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891A260;
      }
      goto L_0891A238;
    }
L_0891A238:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1300)));
    aot_gpr_5 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891A260;
      }
      goto L_0891A24C;
    }
L_0891A24C:
    aot_gpr_4 = (15395u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    goto L_0891A260;
L_0891A260:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A28C;
      }
      goto L_0891A270;
    }
L_0891A270:
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    { const float fs = ctx.fpr[15]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891A28C;
L_0891A28C:
    { const std::uint32_t vfpu_address = ctx.gpr[17] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_fpr_12 = aot_fpr_12 + aot_fpr_14;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x0891A2C0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 409u, 0x0891A2C0u, 0x08A666C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 538u, 0x08A666C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A2C0u) goto L_0891A2C0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A2C0:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(4199)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_0891A2CC;
    }
L_0891A2CC:
    aot_gpr_4 = (15820u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_31 = (0x0891A2E0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 411u, 0x0891A2E0u, 0x0890BE60u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 984u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 984u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 984u, 0x0890BE60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A2E0u) goto L_0891A2E0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A2E0:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_0891A2EC;
    }
L_0891A2EC:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(900))))));
    aot_gpr_4 = (aot_gpr_4 & 30u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    ctx.fpr[22] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(44)));
      if (branch_taken) {
          goto L_0891A338;
      }
      goto L_0891A304;
    }
L_0891A304:
    aot_gpr_31 = (0x0891A30Cu);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(864));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0166.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 414u, 0x0891A30Cu, 0x08A9FB14u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0166_entry(rt, ctx, 586u, aot_mem);
#else
        recomp_unit_0166_entry(rt, ctx, 586u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0166_entry, 166u, 586u, 0x08A9FB14u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A30Cu) goto L_0891A30C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A30C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891A338;
      }
      goto L_0891A314;
    }
L_0891A314:
    ctx.fpr[22] = ctx.fpr[22] - aot_fpr_20;
    aot_fpr_20 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[22] < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A330;
      }
      goto L_0891A32C;
    }
L_0891A32C:
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    goto L_0891A330;
L_0891A330:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(ctx.fpr[22]));
      if (branch_taken) {
          goto L_0891A35C;
      }
      goto L_0891A338;
    }
L_0891A338:
    ctx.fpr[22] = ctx.fpr[22] + aot_fpr_20;
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[22] <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A358;
      }
      goto L_0891A354;
    }
L_0891A354:
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891A358;
L_0891A358:
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(44), std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    goto L_0891A35C;
L_0891A35C:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(236), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      ctx.gpr[18] = aot_run_words[4];
      ctx.gpr[19] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(272));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0891A380:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-912));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(872), aot_gpr_16);
    aot_gpr_5 = (16384u << 16u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(864), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(868), std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    { const std::uint32_t aot_run_words[8]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(876), aot_run_words); }
    aot_gpr_31 = (0x0891A3C0u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_5);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 423u, 0x0891A3C0u, 0x08910B38u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 195u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 195u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 195u, 0x08910B38u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A3C0u) goto L_0891A3C0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A3C0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891A414;
      }
      goto L_0891A3C8;
    }
L_0891A3C8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_6 = (aot_gpr_4 & 256u);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (32768u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_5 & aot_gpr_4);
    aot_gpr_6 = (0u < aot_gpr_6 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891A400;
      }
      goto L_0891A3F8;
    }
L_0891A3F8:
    aot_gpr_31 = (0x0891A400u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 426u, 0x0891A400u, 0x089117B8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 403u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 403u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 403u, 0x089117B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A400u) goto L_0891A400;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A400:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-129));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891B7A8;
      }
      goto L_0891A414;
    }
L_0891A414:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 58u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891A434;
      }
      goto L_0891A424;
    }
L_0891A424:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 57u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891A444;
      }
      goto L_0891A434;
    }
L_0891A434:
    aot_gpr_4 = (16358u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26214u);
    { const bool branch_taken = 0u == 0u;
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_0891A488;
      }
      goto L_0891A444;
    }
L_0891A444:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_4 = (aot_gpr_4 ^ 68u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (15363u << 16u);
      if (branch_taken) {
          goto L_0891A48C;
      }
      goto L_0891A45C;
    }
L_0891A45C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2092)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A480;
      }
      goto L_0891A474;
    }
L_0891A474:
    aot_gpr_4 = (16256u << 16u);
    { const bool branch_taken = 0u == 0u;
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
      if (branch_taken) {
          goto L_0891A488;
      }
      goto L_0891A480;
    }
L_0891A480:
    aot_gpr_4 = (16384u << 16u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    goto L_0891A488;
L_0891A488:
    aot_gpr_4 = (15363u << 16u);
    goto L_0891A48C;
L_0891A48C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(208)));
    aot_gpr_4 = (aot_gpr_4 | 4719u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_gpr_4 = (2238u << 16u);
    ctx.gpr[7] = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    aot_gpr_5 = (aot_gpr_16 | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(14064));
    aot_gpr_31 = (0x0891A4B8u);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 436u, 0x0891A4B8u, 0x08A8F64Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 644u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 644u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 644u, 0x08A8F64Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A4B8u) goto L_0891A4B8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A4B8:
    if (ctx.gpr[2] == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
        goto L_0891B71C;
    }
    goto L_0891A4C0;
L_0891A4C0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_5 = (1024u << 16u);
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_5 = (aot_gpr_6 + aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(20)));
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(12)));
    ctx.fpr[22] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
    aot_gpr_5 = (16448u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    ctx.fpr[22] = ctx.fpr[22] - aot_fpr_12;
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(144));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    ctx.gpr[7] = (0u | 1u);
    ctx.gpr[8] = (0u | 1u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_31 = (0x0891A540u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 438u, 0x0891A540u, 0x08891DD4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 287u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 287u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 287u, 0x08891DD4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A540u) goto L_0891A540;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A540:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891A610;
      }
      goto L_0891A548;
    }
L_0891A548:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 4u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    if (aot_gpr_4 == 0u) {
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
        goto L_0891A5AC;
    }
    goto L_0891A568;
L_0891A568:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    if (aot_gpr_4 == aot_gpr_5) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
        goto L_0891A594;
    }
    goto L_0891A57C;
L_0891A57C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    aot_gpr_5 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(852)));
    if (aot_gpr_4 != aot_gpr_5) {
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
        goto L_0891A5AC;
    }
    goto L_0891A590;
L_0891A590:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(144)));
    goto L_0891A594;
L_0891A594:
    aot_gpr_5 = (32u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    if (aot_gpr_4 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
        goto L_0891A5E8;
    }
    goto L_0891A5A8;
L_0891A5A8:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
    goto L_0891A5AC;
L_0891A5AC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(120)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(12)));
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A60C;
      }
      goto L_0891A5E0;
    }
L_0891A5E0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891A610;
      }
      goto L_0891A5E8;
    }
L_0891A5E8:
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (32768u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_5 & aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891B7A8;
      }
      goto L_0891A60C;
    }
L_0891A60C:
    ctx.gpr[17] = (0u | 1u);
    goto L_0891A610;
L_0891A610:
    { const bool branch_taken = ctx.gpr[17] != 0u;
      if (branch_taken) {
          goto L_0891A6CC;
      }
      goto L_0891A618;
    }
L_0891A618:
    aot_gpr_4 = (16025u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 39322u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 0u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(144));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[22]));
    ctx.gpr[7] = (0u | 1u);
    ctx.gpr[8] = (0u | 1u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_31 = (0x0891A688u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 451u, 0x0891A688u, 0x08891DD4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 287u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 287u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 287u, 0x08891DD4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A688u) goto L_0891A688;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A688:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891A6CC;
      }
      goto L_0891A690;
    }
L_0891A690:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(120)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(12)));
    aot_fpr_13 = aot_fpr_13 + aot_fpr_14;
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A6CC;
      }
      goto L_0891A6C8;
    }
L_0891A6C8:
    ctx.gpr[17] = (0u | 1u);
    goto L_0891A6CC;
L_0891A6CC:
    aot_gpr_4 = (16128u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (17279u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-23440)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11008)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11056)));
    { const float fs = ctx.fpr[15]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[15] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[15] = fs * ft; }
    aot_fpr_14 = aot_fpr_14 + ctx.fpr[15];
    { const float fs = aot_fpr_13; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_14));
    ctx.gpr[18] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11012)));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11060)));
    { const float fs = ctx.fpr[17]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[17] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[17] = fs * ft; }
    ctx.fpr[15] = ctx.fpr[16] + ctx.fpr[17];
    { const float fs = aot_fpr_13; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[15] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[15] = fs * ft; }
    aot_fpr_14 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[15]));
    ctx.gpr[19] = (std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.fpr[18] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11016)));
    ctx.fpr[19] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(11064)));
    { const float fs = ctx.fpr[19]; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = ctx.fpr[18] + aot_fpr_12;
    { const float fs = aot_fpr_13; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    ctx.gpr[20] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x0891A740u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A740u) goto L_0891A740;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A740:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (0u | 0u);
    ctx.gpr[23] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20068)));
    ctx.gpr[22] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20072)));
    ctx.gpr[7] = (ctx.gpr[23] | 0u);
    aot_gpr_31 = (0x0891A75Cu);
    aot_gpr_6 = (ctx.gpr[22] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60E5C, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[9], ctx.gpr[10], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A75Cu) goto L_0891A75C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A75C:
    ctx.gpr[1] = (ctx.gpr[3] << 1u);
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[3]) >> 31u));
    aot_gpr_4 = (ctx.gpr[2] >> 31u);
    aot_gpr_4 = (ctx.gpr[1] | aot_gpr_4);
    aot_gpr_6 = (aot_gpr_4 + ctx.gpr[22]);
    ctx.gpr[7] = (aot_gpr_6 < ctx.gpr[22] ? 1u : 0u);
    aot_gpr_4 = (ctx.gpr[7] + aot_gpr_5);
    aot_gpr_5 = (aot_gpr_4 + ctx.gpr[23]);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_4 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(148), static_cast<std::uint8_t>(ctx.gpr[18]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(149), static_cast<std::uint8_t>(ctx.gpr[19]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(150), static_cast<std::uint8_t>(ctx.gpr[20]));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(151), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    ctx.gpr[18] = (aot_gpr_4 & 256u);
    ctx.gpr[18] = (0u < ctx.gpr[18] ? 1u : 0u);
    aot_gpr_4 = (15948u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(86))))));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(12)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(32));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 0u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_5);
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vscl_ct<0u, 0u, 28u, 3u>();
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(80));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 0u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(160));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(160), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(176));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_6 = (1024u << 16u);
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_6);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_31 = (0x0891A854u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0189.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 457u, 0x0891A854u, 0x08AF8668u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0189_entry(rt, ctx, 115u, aot_mem);
#else
        recomp_unit_0189_entry(rt, ctx, 115u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0189_entry, 189u, 115u, 0x08AF8668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A854u) goto L_0891A854;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A854:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_4 = (aot_gpr_4 ^ 68u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    if (aot_gpr_4 == 0u) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
        goto L_0891A894;
    }
    goto L_0891A86C;
L_0891A86C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(168)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(176)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
        goto L_0891A894;
    }
    goto L_0891A884;
L_0891A884:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0891A890u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 460u, 0x0891A890u, 0x08908D60u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 240u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 240u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 240u, 0x08908D60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A890u) goto L_0891A890;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A890:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(104)));
    goto L_0891A894;
L_0891A894:
    aot_gpr_4 = (16140u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(176)));
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    { const float fs = aot_fpr_20; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A8D0;
      }
      goto L_0891A8BC;
    }
L_0891A8BC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-129));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891A8F8;
      }
      goto L_0891A8D0;
    }
L_0891A8D0:
    aot_gpr_4 = (16128u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_20; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (16243u << 16u);
      if (branch_taken) {
          goto L_0891A8FC;
      }
      goto L_0891A8EC;
    }
L_0891A8EC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_4 = (aot_gpr_4 | 128u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    goto L_0891A8F8;
L_0891A8F8:
    aot_gpr_4 = (16243u << 16u);
    goto L_0891A8FC;
L_0891A8FC:
    aot_gpr_4 = (aot_gpr_4 | 13107u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_20; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891A930;
      }
      goto L_0891A918;
    }
L_0891A918:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (32768u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891A940;
      }
      goto L_0891A930;
    }
L_0891A930:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (32768u << 16u);
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    goto L_0891A940;
L_0891A940:
    { const bool branch_taken = ctx.gpr[18] == 0u;
      if (branch_taken) {
          goto L_0891A9C4;
      }
      goto L_0891A948;
    }
L_0891A948:
    aot_gpr_31 = (0x0891A950u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A950u) goto L_0891A950;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A950:
    ctx.gpr[19] = (ctx.gpr[3] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x0891A960u);
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A960u) goto L_0891A960;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A960:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20060)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20064)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x0891A974u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A974u) goto L_0891A974;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A974:
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    ctx.gpr[7] = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x0891A988u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B6256C, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A988u) goto L_0891A988;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A988:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[2]) <= 0;
      if (branch_taken) {
          goto L_0891A9B0;
      }
      goto L_0891A990;
    }
L_0891A990:
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_0891A9B0;
      }
      goto L_0891A998;
    }
L_0891A998:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_31 = (0x0891A9B0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 477u, 0x0891A9B0u, 0x089117B8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 403u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 403u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 403u, 0x089117B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A9B0u) goto L_0891A9B0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891A9B0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-129));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
      if (branch_taken) {
          goto L_0891AA20;
      }
      goto L_0891A9C4;
    }
L_0891A9C4:
    aot_gpr_31 = (0x0891A9CCu);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A9CCu) goto L_0891A9CC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A9CC:
    ctx.gpr[19] = (ctx.gpr[3] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x0891A9DCu);
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A9DCu) goto L_0891A9DC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A9DC:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20052)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20056)));
    aot_gpr_5 = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x0891A9F0u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61FD0, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 400u, 0x08B61FD0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891A9F0u) goto L_0891A9F0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891A9F0:
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    ctx.gpr[7] = (ctx.gpr[3] | 0u);
    aot_gpr_31 = (0x0891AA04u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B6256C, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AA04u) goto L_0891AA04;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891AA04:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[2]) >= 0;
      if (branch_taken) {
          goto L_0891AA20;
      }
      goto L_0891AA0C;
    }
L_0891AA0C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (aot_gpr_5 | 256u);
    aot_gpr_31 = (0x0891AA20u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_5);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 484u, 0x0891AA20u, 0x089117B8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 403u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 403u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 403u, 0x089117B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AA20u) goto L_0891AA20;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AA20:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    if (aot_gpr_4 == 0u) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(160)));
        goto L_0891AA44;
    }
    goto L_0891AA38;
L_0891AA38:
    aot_gpr_4 = (0u | 35u);
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(311), static_cast<std::uint8_t>(aot_gpr_4));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(160)));
    goto L_0891AA44;
L_0891AA44:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(96)));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(164)));
    ctx.fpr[15] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(100)));
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
    aot_fpr_14 = aot_fpr_14 - ctx.fpr[15];
    aot_gpr_4 = (2238u << 16u);
    ctx.fpr[16] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(176)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_12; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) ctx.fpr[18] = std::bit_cast<float>(0x7FC00000u); else ctx.fpr[18] = fs * ft; }
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(14064));
    ctx.fpr[17] = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(144)));
    { const float fs = aot_fpr_14; const float ft = aot_fpr_14; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(180), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(184), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_12 = ctx.fpr[16] - ctx.fpr[17];
    aot_fpr_13 = ctx.fpr[18] + aot_fpr_13;
    aot_fpr_13 = std::sqrt(aot_fpr_13);
    ctx.fpr[15] = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_13) || std::isnan(ctx.fpr[15])) && aot_fpr_13 == ctx.fpr[15])) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891AB58;
      }
      goto L_0891AA98;
    }
L_0891AA98:
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(280)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(272), 0u);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(280), aot_gpr_4);
    ctx.gpr[17] = (aot_gpr_29 + static_cast<std::uint32_t>(208));
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12) ^ 0x80000000u);
        goto L_0891AACC;
    }
    goto L_0891AACC;
L_0891AACC:
    aot_gpr_4 = (15897u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 39322u);
    aot_fpr_14 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891AAF8;
      }
      goto L_0891AAE8;
    }
L_0891AAE8:
    aot_gpr_31 = (0x0891AAF0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 490u, 0x0891AAF0u, 0x08B6033Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 36u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 36u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 36u, 0x08B6033Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AAF0u) goto L_0891AAF0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AAF0:
    { const bool branch_taken = 0u == 0u;
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
      if (branch_taken) {
          goto L_0891AB04;
      }
      goto L_0891AAF8;
    }
L_0891AAF8:
    aot_gpr_31 = (0x0891AB00u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0088.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 492u, 0x0891AB00u, 0x089644F8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0088_entry(rt, ctx, 26u, aot_mem);
#else
        recomp_unit_0088_entry(rt, ctx, 26u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0088_entry, 88u, 26u, 0x089644F8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB00u) goto L_0891AB00;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AB00:
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_0891AB04;
L_0891AB04:
    aot_gpr_31 = (0x0891AB0Cu);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0022.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 494u, 0x0891AB0Cu, 0x0885F9B0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0022_entry(rt, ctx, 917u, aot_mem);
#else
        recomp_unit_0022_entry(rt, ctx, 917u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0022_entry, 22u, 917u, 0x0885F9B0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB0Cu) goto L_0891AB0C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AB0C:
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_4 = (16256u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(256), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_13)};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(260), aot_run_words); }
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(208));
    aot_gpr_31 = (0x0891AB34u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0023.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 495u, 0x0891AB34u, 0x088603A0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0023_entry(rt, ctx, 10u, aot_mem);
#else
        recomp_unit_0023_entry(rt, ctx, 10u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0023_entry, 23u, 10u, 0x088603A0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB34u) goto L_0891AB34;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AB34:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(280)));
    aot_gpr_4 = (aot_gpr_4 & 1u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891AB58;
      }
      goto L_0891AB44;
    }
L_0891AB44:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(272)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891AB58;
      }
      goto L_0891AB50;
    }
L_0891AB50:
    aot_gpr_31 = (0x0891AB58u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(272)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0117.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 498u, 0x0891AB58u, 0x089D89E0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0117_entry(rt, ctx, 129u, aot_mem);
#else
        recomp_unit_0117_entry(rt, ctx, 129u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0117_entry, 117u, 129u, 0x089D89E0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB58u) goto L_0891AB58;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AB58:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_31 = (0x0891AB6Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 499u, 0x0891AB6Cu, 0x08A66CD8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 600u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 600u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 600u, 0x08A66CD8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB6Cu) goto L_0891AB6C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AB6C:
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(148)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(149)));
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(188));
    ctx.gpr[7] = (aot_mem.aot_direct_load8(aot_gpr_29 + static_cast<std::uint32_t>(150)));
    aot_gpr_31 = (0x0891AB84u);
    ctx.gpr[8] = (0u | 100u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08A3550C, 140u, 317u, 0x08A3550Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0140_entry, 140u, 317u, 0x08A3550Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AB84u) goto L_0891AB84;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891AB84:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2244)));
    aot_gpr_5 = (0u | 1u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891ABA4;
      }
      goto L_0891AB94;
    }
L_0891AB94:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 58u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891ACDC;
      }
      goto L_0891ABA4;
    }
L_0891ABA4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 >> 6u);
    aot_gpr_4 = (aot_gpr_4 & 15u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_5 = (0u | 7u);
      if (branch_taken) {
          goto L_0891ABC0;
      }
      goto L_0891ABB8;
    }
L_0891ABB8:
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891AC3C;
      }
      goto L_0891ABC0;
    }
L_0891ABC0:
    ctx.gpr[17] = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0891ABD4u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0055.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 505u, 0x0891ABD4u, 0x088E3E88u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0055_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0055_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0055_entry, 55u, 571u, 0x088E3E88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ABD4u) goto L_0891ABD4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891ABD4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(1344)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(192)));
    aot_gpr_31 = (0x0891ABE4u);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(196)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0005.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 506u, 0x0891ABE4u, 0x08818130u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0005_entry(rt, ctx, 10u, aot_mem);
#else
        recomp_unit_0005_entry(rt, ctx, 10u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0005_entry, 5u, 10u, 0x08818130u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ABE4u) goto L_0891ABE4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891ABE4:
    aot_gpr_4 = (16575u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 2621u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = ctx.fpr[0] + aot_fpr_12;
    ctx.gpr[2] = (0u | 600u);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(288));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(188));
    aot_gpr_4 = (0u | 37u);
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(288), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(292), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(200), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(296), std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891AC34u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[2]);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 507u, 0x0891AC34u, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AC34u) goto L_0891AC34;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AC34:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891ACD4;
      }
      goto L_0891AC3C;
    }
L_0891AC3C:
    aot_gpr_5 = (0u | 3u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891ACD4;
      }
      goto L_0891AC48;
    }
L_0891AC48:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 58u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891ACD4;
      }
      goto L_0891AC58;
    }
L_0891AC58:
    ctx.gpr[17] = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0891AC6Cu);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0055.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 511u, 0x0891AC6Cu, 0x088E3E88u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0055_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0055_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0055_entry, 55u, 571u, 0x088E3E88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AC6Cu) goto L_0891AC6C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AC6C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(1344)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(192)));
    aot_gpr_31 = (0x0891AC7Cu);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(196)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0005.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 512u, 0x0891AC7Cu, 0x08818130u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0005_entry(rt, ctx, 10u, aot_mem);
#else
        recomp_unit_0005_entry(rt, ctx, 10u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0005_entry, 5u, 10u, 0x08818130u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AC7Cu) goto L_0891AC7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AC7C:
    aot_gpr_4 = (16575u << 16u);
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_4 | 2621u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(304), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_fpr_13 = ctx.fpr[0] + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(308), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (16384u << 16u);
    ctx.gpr[2] = (0u | 1200u);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(304));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(200), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(312), std::bit_cast<std::uint32_t>(aot_fpr_14));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(188));
    aot_gpr_4 = (0u | 37u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891ACD4u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[2]);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 513u, 0x0891ACD4u, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ACD4u) goto L_0891ACD4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891ACD4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891B0D8;
      }
      goto L_0891ACDC;
    }
L_0891ACDC:
    ctx.gpr[17] = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0891ACF0u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0055.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 515u, 0x0891ACF0u, 0x088E3E88u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0055_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0055_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0055_entry, 55u, 571u, 0x088E3E88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ACF0u) goto L_0891ACF0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891ACF0:
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    aot_gpr_31 = (0x0891ACFCu);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0163.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 516u, 0x0891ACFCu, 0x08A931D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0163_entry(rt, ctx, 464u, aot_mem);
#else
        recomp_unit_0163_entry(rt, ctx, 464u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0163_entry, 163u, 464u, 0x08A931D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ACFCu) goto L_0891ACFC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891ACFC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(1344)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(192)));
    aot_gpr_31 = (0x0891AD0Cu);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(196)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0005.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 517u, 0x0891AD0Cu, 0x08818130u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0005_entry(rt, ctx, 10u, aot_mem);
#else
        recomp_unit_0005_entry(rt, ctx, 10u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0005_entry, 5u, 10u, 0x08818130u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AD0Cu) goto L_0891AD0C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AD0C:
    aot_gpr_4 = (16575u << 16u);
    aot_fpr_13 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_4 | 2621u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(320), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = ctx.fpr[0] + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(324), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(200), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x0891AD34u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(328), std::bit_cast<std::uint32_t>(aot_fpr_13));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AD34u) goto L_0891AD34;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891AD34:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20044)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20048)));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x0891AD48u);
    aot_gpr_5 = (0u | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60E5C, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[9], ctx.gpr[10], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AD48u) goto L_0891AD48;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891AD48:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20036)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20040)));
    ctx.gpr[1] = (ctx.gpr[3] << 1u);
    aot_gpr_6 = (ctx.gpr[2] >> 31u);
    ctx.gpr[7] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[3]) >> 31u));
    aot_gpr_6 = (ctx.gpr[1] | aot_gpr_6);
    ctx.gpr[8] = (aot_gpr_6 + aot_gpr_4);
    ctx.gpr[9] = (ctx.gpr[8] < aot_gpr_4 ? 1u : 0u);
    aot_gpr_6 = (ctx.gpr[9] + ctx.gpr[7]);
    aot_gpr_4 = (ctx.gpr[8] | 0u);
    aot_gpr_4 = (aot_gpr_4 | 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_5 = (aot_gpr_6 + aot_gpr_5);
      if (branch_taken) {
          goto L_0891ADDC;
      }
      goto L_0891AD7C;
    }
L_0891AD7C:
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(320));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_4);
    { const std::uint32_t vfpu_address = aot_gpr_16 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vscl_ct<0u, 0u, 28u, 3u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(352));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (14979u << 16u);
    aot_gpr_5 = (aot_gpr_5 | 4719u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
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
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(336));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_31 = (0x0891ADCCu);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_compact_generated_leaf<&vcs_compact_leaf_08A931B8, 163u, 0x08A931B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891ADCCu) goto L_0891ADCC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891ADCC:
    aot_gpr_4 = (47747u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 4719u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(328), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891ADDC;
L_0891ADDC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (0u | 77u);
    aot_gpr_5 = (aot_gpr_5 & 1u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_4 = (0u | 76u);
        goto L_0891ADF0;
    }
    goto L_0891ADF0;
L_0891ADF0:
    ctx.gpr[2] = (0u | 700u);
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(320));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(188));
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891AE1Cu);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[2]);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 524u, 0x0891AE1Cu, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AE1Cu) goto L_0891AE1C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AE1C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2244)));
    aot_gpr_5 = (0u | 5u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891B0D8;
      }
      goto L_0891AE2C;
    }
L_0891AE2C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(368));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    { float vfpu_value[4]{}; vfpu_value[0u] = 1.0f;
      ctx.write_vfpu_vector_with_destination_prefix_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vdot_ct<28u, 1u, 1u, 3u>();
    ctx.execute_vfpu_vcmp_ct<28u, 32u, 1u, 3u>();
    ctx.execute_vfpu_unary_ct<28u, 28u, 1u, 17u>();
    ctx.execute_vfpu_vscl_ct<1u, 1u, 28u, 3u>();
    ctx.execute_vfpu_vcmov_ct<1u, 0u, 3u, 0u, false>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<1u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (16672u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<1u>(aot_gpr_5);
    ctx.execute_vfpu_unary_ct<1u, 1u, 1u, 16u>();
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vscl_ct<0u, 0u, 1u, 3u>();
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_4 = (15523u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(376), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (0u | 5u);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(384), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (0u | 6u);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(385), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (0u | 9u);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(386), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (0u | 10u);
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(387), static_cast<std::uint8_t>(aot_gpr_4));
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B0D8;
      }
      goto L_0891AEBC;
    }
L_0891AEBC:
    aot_gpr_4 = (aot_gpr_29 + ctx.gpr[17]);
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(384)));
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_31 = (0x0891AED0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0055.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 527u, 0x0891AED0u, 0x088E3E88u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0055_entry(rt, ctx, 571u, aot_mem);
#else
        recomp_unit_0055_entry(rt, ctx, 571u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0055_entry, 55u, 571u, 0x088E3E88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AED0u) goto L_0891AED0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AED0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(1344)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(192)));
    aot_gpr_31 = (0x0891AEE0u);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(196)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0005.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 528u, 0x0891AEE0u, 0x08818130u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0005_entry(rt, ctx, 10u, aot_mem);
#else
        recomp_unit_0005_entry(rt, ctx, 10u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0005_entry, 5u, 10u, 0x08818130u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AEE0u) goto L_0891AEE0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AEE0:
    aot_gpr_4 = (16575u << 16u);
    aot_gpr_5 = (15820u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 2621u);
    aot_gpr_5 = (aot_gpr_5 | 52429u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_4 = (0u | 1u);
    aot_fpr_12 = ctx.fpr[0] + aot_fpr_12;
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_6 = (3840u << 16u);
    aot_gpr_4 = (aot_gpr_4 << (ctx.gpr[17] & 31u));
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_6);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    aot_gpr_5 = (aot_gpr_5 >> 24u);
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_4);
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_fpr_12 = aot_fpr_12 - aot_fpr_13;
      if (branch_taken) {
          goto L_0891AFB4;
      }
      goto L_0891AF24;
    }
L_0891AF24:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(200)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891AFB4;
      }
      goto L_0891AF38;
    }
L_0891AF38:
    aot_gpr_4 = (~(aot_gpr_4 | 0u));
    aot_gpr_6 = (61696u << 16u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_4 = (aot_gpr_4 << 24u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_6);
    ctx.gpr[8] = (15300u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(200), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_5 & aot_gpr_4);
    ctx.gpr[8] = (ctx.gpr[8] | 39846u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    aot_fpr_20 = std::bit_cast<float>(ctx.gpr[8]);
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(388));
    aot_gpr_5 = (0u | 255u);
    aot_gpr_6 = (0u | 255u);
    ctx.gpr[7] = (0u | 255u);
    aot_gpr_31 = (0x0891AF80u);
    ctx.gpr[8] = (0u | 255u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08A3550C, 140u, 317u, 0x08A3550Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0140_entry, 140u, 317u, 0x08A3550Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AF80u) goto L_0891AF80;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891AF80:
    aot_gpr_4 = (0u | 44u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(368));
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[8] = (ctx.gpr[2] | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891AFACu);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 532u, 0x0891AFACu, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891AFACu) goto L_0891AFAC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891AFAC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891B0C4;
      }
      goto L_0891AFB4;
    }
L_0891AFB4:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_6 = (3840u << 16u);
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_6);
    aot_gpr_5 = (aot_gpr_5 >> 24u);
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_4);
    { const bool branch_taken = aot_gpr_5 != 0u;
      if (branch_taken) {
          goto L_0891B0C4;
      }
      goto L_0891AFD0;
    }
L_0891AFD0:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(200)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B0C4;
      }
      goto L_0891AFE4;
    }
L_0891AFE4:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    aot_gpr_4 = (aot_gpr_4 & 15u);
    aot_gpr_4 = (aot_gpr_4 << 24u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(200), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_5 | aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(400), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(404), std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[3] = (16025u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(408), std::bit_cast<std::uint32_t>(aot_fpr_13));
    ctx.gpr[2] = (0u | 400u);
    ctx.gpr[3] = (ctx.gpr[3] | 39322u);
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(192));
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[3]);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(400));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(188));
    aot_gpr_4 = (0u | 75u);
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891B044u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[2]);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 536u, 0x0891B044u, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B044u) goto L_0891B044;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B044:
    { const bool branch_taken = ctx.gpr[17] == 0u;
    aot_gpr_4 = (0u | 1u);
      if (branch_taken) {
          goto L_0891B054;
      }
      goto L_0891B04C;
    }
L_0891B04C:
    { const bool branch_taken = ctx.gpr[17] != aot_gpr_4;
      if (branch_taken) {
          goto L_0891B0C4;
      }
      goto L_0891B054;
    }
L_0891B054:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(7768));
    aot_gpr_31 = (0x0891B060u);
    ctx.gpr[19] = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(96)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B060u) goto L_0891B060;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B060:
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20284)));
    ctx.gpr[20] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20288)));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (0u | 0u);
    ctx.gpr[7] = (ctx.gpr[21] | 0u);
    aot_gpr_31 = (0x0891B07Cu);
    aot_gpr_6 = (ctx.gpr[20] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60E5C, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[9], ctx.gpr[10], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 177u, 0x08B60E5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B07Cu) goto L_0891B07C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B07C:
    ctx.gpr[1] = (ctx.gpr[3] << 1u);
    aot_gpr_4 = (ctx.gpr[2] >> 31u);
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[3]) >> 31u));
    aot_gpr_4 = (ctx.gpr[1] | aot_gpr_4);
    aot_gpr_6 = (aot_gpr_4 + ctx.gpr[20]);
    ctx.gpr[7] = (aot_gpr_6 < ctx.gpr[20] ? 1u : 0u);
    aot_gpr_4 = (ctx.gpr[7] + aot_gpr_5);
    aot_gpr_5 = (aot_gpr_4 + ctx.gpr[21]);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_4 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_5);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x0891B0C4u);
    aot_gpr_6 = (0u | 179u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0128.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 541u, 0x0891B0C4u, 0x08A05F80u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0128_entry(rt, ctx, 503u, aot_mem);
#else
        recomp_unit_0128_entry(rt, ctx, 503u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0128_entry, 128u, 503u, 0x08A05F80u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B0C4u) goto L_0891B0C4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B0C4:
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1));
    ctx.gpr[17] = (ctx.gpr[17] & 255u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891AEBC;
      }
      goto L_0891B0D8;
    }
L_0891B0D8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(464)));
    aot_gpr_4 = (aot_gpr_4 & 16u);
    if (aot_gpr_4 == 0u) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
        goto L_0891B250;
    }
    goto L_0891B0E8;
L_0891B0E8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 57u);
    if (aot_gpr_4 == aot_gpr_5) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
        goto L_0891B250;
    }
    goto L_0891B0F8;
L_0891B0F8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 58u);
    if (aot_gpr_4 == aot_gpr_5) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
        goto L_0891B250;
    }
    goto L_0891B108;
L_0891B108:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(208)));
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_gpr_4 = (15185u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 | 46872u);
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_14)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
        goto L_0891B250;
    }
    goto L_0891B138;
L_0891B138:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(464)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-17));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(464), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(416));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_31 = (0x0891B160u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0055.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 547u, 0x0891B160u, 0x088E3F34u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0055_entry(rt, ctx, 581u, aot_mem);
#else
        recomp_unit_0055_entry(rt, ctx, 581u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0055_entry, 55u, 581u, 0x088E3F34u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B160u) goto L_0891B160;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B160:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891B24C;
      }
      goto L_0891B168;
    }
L_0891B168:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1252)));
    aot_gpr_4 = (16800u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B1A8;
      }
      goto L_0891B184;
    }
L_0891B184:
    ctx.gpr[10] = (16752u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[10]);
    aot_gpr_5 = (0u | 0u);
    aot_gpr_6 = (0u | 46u);
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_31 = (0x0891B1A8u);
    ctx.gpr[9] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0057.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 550u, 0x0891B1A8u, 0x088EB674u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0057_entry(rt, ctx, 692u, aot_mem);
#else
        recomp_unit_0057_entry(rt, ctx, 692u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0057_entry, 57u, 692u, 0x088EB674u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B1A8u) goto L_0891B1A8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B1A8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_4 = (aot_gpr_4 & 4096u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (16204u << 16u);
      if (branch_taken) {
          goto L_0891B1E0;
      }
      goto L_0891B1B8;
    }
L_0891B1B8:
    aot_gpr_6 = (50298u << 16u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_31 = (0x0891B1CCu);
    aot_gpr_5 = (0u | 16u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0026.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 552u, 0x0891B1CCu, 0x0886D370u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0026_entry(rt, ctx, 181u, aot_mem);
#else
        recomp_unit_0026_entry(rt, ctx, 181u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0026_entry, 26u, 181u, 0x0886D370u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B1CCu) goto L_0891B1CC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B1CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-4097));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(456), aot_gpr_4);
    aot_gpr_4 = (16204u << 16u);
    goto L_0891B1E0;
L_0891B1E0:
    aot_fpr_20 = std::bit_cast<float>(0u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(424)));
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(432), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_fpr_13 = aot_fpr_12 - aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(436), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(416));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(432));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(148));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(424), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(440), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_4 = (0u | 8u);
    ctx.gpr[7] = (0u | 50u);
    aot_gpr_31 = (0x0891B224u);
    ctx.gpr[9] = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0079.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 554u, 0x0891B224u, 0x089421CCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0079_entry(rt, ctx, 352u, aot_mem);
#else
        recomp_unit_0079_entry(rt, ctx, 352u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0079_entry, 79u, 352u, 0x089421CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B224u) goto L_0891B224;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B224:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x0891B238u);
    aot_fpr_14 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 555u, 0x0891B238u, 0x08A6669Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 537u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 537u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 537u, 0x08A6669Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B238u) goto L_0891B238;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B238:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0891B244u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 556u, 0x0891B244u, 0x08908D60u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 240u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 240u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 240u, 0x08908D60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B244u) goto L_0891B244;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B244:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891B7A8;
      }
      goto L_0891B24C;
    }
L_0891B24C:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    goto L_0891B250;
L_0891B250:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(208)));
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_gpr_4 = (15363u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 | 4719u);
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_20 = std::bit_cast<float>(0u);
      if (branch_taken) {
          goto L_0891B2B0;
      }
      goto L_0891B27C;
    }
L_0891B27C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (16153u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 39322u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (2238u << 16u);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(14064));
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(144)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_14 <= aot_fpr_12)) ? 0x00800000u : 0u);
    if (((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
        goto L_0891B328;
    }
    goto L_0891B2B0;
L_0891B2B0:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (16230u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26214u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    ctx.set_vfpu_scalar_bits_ct<32u>(aot_gpr_5);
    ctx.execute_vfpu_unary_ct<64u, 0u, 1u, 21u>();
    ctx.execute_vfpu_vec3_ct<0u, 32u, 64u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<32u, 0u, 1u, 20u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<32u>());
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_20));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(320));
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
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(448));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_31 = (0x0891B308u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 561u, 0x0891B308u, 0x08A666C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 538u, 0x08A666C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B308u) goto L_0891B308;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B308:
    aot_gpr_4 = (0u | 0u);
    aot_gpr_4 = (aot_gpr_4 & 1u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_6 = (0u + static_cast<std::uint32_t>(-2));
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_6);
    aot_gpr_4 = (aot_gpr_5 | aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(456), aot_gpr_4);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    goto L_0891B328;
L_0891B328:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(208)));
    aot_fpr_12 = aot_fpr_12 / aot_fpr_13;
    aot_gpr_4 = (15107u << 16u);
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (aot_gpr_4 | 4719u);
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_14; const float ft = ctx.fpr[15]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_14 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_14 = fs * ft; }
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_14)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B518;
      }
      goto L_0891B354;
    }
L_0891B354:
    aot_fpr_12 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_20) || std::isnan(aot_fpr_12)) && aot_fpr_20 == aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B39C;
      }
      goto L_0891B368;
    }
L_0891B368:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7676)));
    aot_gpr_4 = (16230u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 26214u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<0u>(aot_gpr_4);
    ctx.set_vfpu_scalar_bits_ct<32u>(aot_gpr_5);
    ctx.execute_vfpu_unary_ct<64u, 0u, 1u, 21u>();
    ctx.execute_vfpu_vec3_ct<0u, 32u, 64u, 1u, 2u>();
    ctx.execute_vfpu_unary_ct<32u, 0u, 1u, 20u>();
    aot_gpr_4 = (ctx.vfpu_scalar_bits_ct<32u>());
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    goto L_0891B39C;
L_0891B39C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(320));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(464));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(464)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(464), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(468)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(468), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(472)));
    aot_gpr_4 = (48588u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B4DC;
      }
      goto L_0891B3E4;
    }
L_0891B3E4:
    aot_gpr_4 = (48163u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(472), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(7768));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(96)));
    aot_fpr_20 = std::bit_cast<float>(0u);
    aot_gpr_6 = (16256u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_6 = (0u | 164u);
    aot_gpr_31 = (0x0891B414u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0128.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 567u, 0x0891B414u, 0x08A05F80u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0128_entry(rt, ctx, 503u, aot_mem);
#else
        recomp_unit_0128_entry(rt, ctx, 503u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0128_entry, 128u, 503u, 0x08A05F80u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B414u) goto L_0891B414;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B414:
    aot_gpr_4 = (16396u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    ctx.set_vfpu_scalar_bits_ct<28u>(aot_gpr_4);
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(464));
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
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(496));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_5 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<1u, 4u>(vfpu_value); }
    ctx.execute_vfpu_vec3_ct<0u, 0u, 1u, 3u, 0u>();
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(480));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(512));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(528), std::bit_cast<std::uint32_t>(aot_fpr_20));
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(512), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(528));
    aot_gpr_31 = (0x0891B47Cu);
    aot_gpr_5 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0189.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 568u, 0x0891B47Cu, 0x08AF8668u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0189_entry(rt, ctx, 115u, aot_mem);
#else
        recomp_unit_0189_entry(rt, ctx, 115u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0189_entry, 189u, 115u, 0x08AF8668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B47Cu) goto L_0891B47C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B47C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891B48C;
      }
      goto L_0891B484;
    }
L_0891B484:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(528)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(520), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891B48C;
L_0891B48C:
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_4 = (15820u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(544), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(548), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(552), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(544));
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(512));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(148));
    aot_gpr_4 = (0u | 8u);
    ctx.gpr[7] = (0u | 200u);
    aot_gpr_31 = (0x0891B4C4u);
    ctx.gpr[9] = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0079.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 571u, 0x0891B4C4u, 0x089421CCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0079_entry(rt, ctx, 352u, aot_mem);
#else
        recomp_unit_0079_entry(rt, ctx, 352u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0079_entry, 79u, 352u, 0x089421CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B4C4u) goto L_0891B4C4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B4C4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(80));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-20080), aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(100));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-20076), aot_gpr_4);
      if (branch_taken) {
          goto L_0891B50C;
      }
      goto L_0891B4DC;
    }
L_0891B4DC:
    aot_gpr_4 = (48419u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(472)));
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B50C;
      }
      goto L_0891B4FC;
    }
L_0891B4FC:
    aot_gpr_4 = (48291u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 55050u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(472), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891B50C;
L_0891B50C:
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(464));
    aot_gpr_31 = (0x0891B518u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0152.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 575u, 0x0891B518u, 0x08A666C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0152_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0152_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0152_entry, 152u, 538u, 0x08A666C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B518u) goto L_0891B518;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B518:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20076)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B640;
      }
      goto L_0891B524;
    }
L_0891B524:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20076)));
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891B640;
      }
      goto L_0891B538;
    }
L_0891B538:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(560));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(576), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(560), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(576));
    aot_gpr_31 = (0x0891B568u);
    aot_gpr_5 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0189.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 578u, 0x0891B568u, 0x08AF8668u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0189_entry(rt, ctx, 115u, aot_mem);
#else
        recomp_unit_0189_entry(rt, ctx, 115u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0189_entry, 189u, 115u, 0x08AF8668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B568u) goto L_0891B568;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B568:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891B578;
      }
      goto L_0891B570;
    }
L_0891B570:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(576)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(568), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891B578;
L_0891B578:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(568)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_13)) && aot_fpr_12 == aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B640;
      }
      goto L_0891B590;
    }
L_0891B590:
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-20076), 0u);
      if (branch_taken) {
          goto L_0891B640;
      }
      goto L_0891B5A0;
    }
L_0891B5A0:
    aot_gpr_4 = (16192u << 16u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(560));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    ctx.gpr[18] = (aot_gpr_29 + static_cast<std::uint32_t>(608));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = ctx.gpr[18] + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_gpr_31 = (0x0891B5C0u);
    ctx.fpr[22] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20) ^ 0x80000000u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20998, 199u, 113u, 0x08B20998u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 113u, 0x08B20998u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B5C0u) goto L_0891B5C0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B5C0:
    aot_fpr_12 = aot_fpr_20 - ctx.fpr[22];
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[0]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = ctx.fpr[22] + aot_fpr_12;
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(608)));
    aot_fpr_12 = aot_fpr_13 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(608), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (0x0891B5E0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20998, 199u, 113u, 0x08B20998u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 113u, 0x08B20998u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B5E0u) goto L_0891B5E0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B5E0:
    aot_fpr_12 = aot_fpr_20 - ctx.fpr[22];
    { const float fs = aot_fpr_12; const float ft = ctx.fpr[0]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    aot_fpr_12 = ctx.fpr[22] + aot_fpr_12;
    aot_fpr_14 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(612)));
    aot_fpr_12 = aot_fpr_14 + aot_fpr_12;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(612), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(0u);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_12), std::bit_cast<std::uint32_t>(aot_fpr_12)};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(592), aot_run_words); }
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(592));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(148));
    aot_gpr_4 = (0u | 37u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[9] = (0u | 0u);
    ctx.gpr[10] = (0u | 0u);
    ctx.gpr[11] = (0u | 0u);
    aot_gpr_31 = (0x0891B630u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0063.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 585u, 0x0891B630u, 0x0890043Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0063_entry(rt, ctx, 90u, aot_mem);
#else
        recomp_unit_0063_entry(rt, ctx, 90u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0063_entry, 63u, 90u, 0x0890043Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B630u) goto L_0891B630;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B630:
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[17]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891B5A0;
      }
      goto L_0891B640;
    }
L_0891B640:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20080)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B714;
      }
      goto L_0891B64C;
    }
L_0891B64C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20080)));
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891B714;
      }
      goto L_0891B660;
    }
L_0891B660:
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(624), static_cast<std::uint8_t>(0u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(625), static_cast<std::uint8_t>(0u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(626), static_cast<std::uint8_t>(0u));
    aot_mem.aot_direct_store8(aot_gpr_29 + static_cast<std::uint32_t>(627), static_cast<std::uint8_t>(0u));
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(640));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(656), std::bit_cast<std::uint32_t>(aot_fpr_12));
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(640), aot_run_words);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_13 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_14 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_4 = (aot_gpr_29 + static_cast<std::uint32_t>(656));
    aot_gpr_31 = (0x0891B6A0u);
    aot_gpr_5 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0189.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 589u, 0x0891B6A0u, 0x08AF8668u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0189_entry(rt, ctx, 115u, aot_mem);
#else
        recomp_unit_0189_entry(rt, ctx, 115u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0189_entry, 189u, 115u, 0x08AF8668u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B6A0u) goto L_0891B6A0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B6A0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891B6B0;
      }
      goto L_0891B6A8;
    }
L_0891B6A8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(656)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(648), std::bit_cast<std::uint32_t>(aot_fpr_12));
    goto L_0891B6B0;
L_0891B6B0:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(648)));
    aot_fpr_13 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0891B714;
      }
      goto L_0891B6C8;
    }
L_0891B6C8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(648)));
    aot_fpr_14 = std::bit_cast<float>(0u);
    aot_gpr_4 = (16000u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-20080), 0u);
    aot_fpr_13 = aot_fpr_12 + aot_fpr_13;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(672), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_4 = (16528u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(676), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_5 = (aot_gpr_29 + static_cast<std::uint32_t>(640));
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(648), std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(680), std::bit_cast<std::uint32_t>(aot_fpr_14));
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(672));
    ctx.gpr[8] = (aot_gpr_29 + static_cast<std::uint32_t>(624));
    aot_gpr_4 = (0u | 9u);
    ctx.gpr[7] = (0u | 1500u);
    aot_gpr_31 = (0x0891B714u);
    ctx.gpr[9] = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0079.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 593u, 0x0891B714u, 0x089421CCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0079_entry(rt, ctx, 352u, aot_mem);
#else
        recomp_unit_0079_entry(rt, ctx, 352u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0079_entry, 79u, 352u, 0x089421CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B714u) goto L_0891B714;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B714:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891B7A0;
      }
      goto L_0891B71C;
    }
L_0891B71C:
    aot_gpr_5 = (64512u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    if (aot_gpr_4 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
        goto L_0891B774;
    }
    goto L_0891B744;
L_0891B744:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (32768u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_5 & aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    aot_gpr_31 = (0x0891B770u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 596u, 0x0891B770u, 0x089117B8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 403u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 403u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 403u, 0x089117B8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B770u) goto L_0891B770;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B770:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(236)));
    goto L_0891B774;
L_0891B774:
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(472)));
    aot_gpr_5 = (32768u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(236), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_6 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-129));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(472), aot_gpr_4);
    goto L_0891B7A0;
L_0891B7A0:
    aot_gpr_31 = (0x0891B7A8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 599u, 0x0891B7A8u, 0x08912A70u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 672u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 672u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 672u, 0x08912A70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B7A8u) goto L_0891B7A8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B7A8:
    { std::uint32_t aot_run_words[11]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(864), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[22] = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      ctx.gpr[18] = aot_run_words[4];
      ctx.gpr[19] = aot_run_words[5];
      ctx.gpr[20] = aot_run_words[6];
      ctx.gpr[21] = aot_run_words[7];
      ctx.gpr[22] = aot_run_words[8];
      ctx.gpr[23] = aot_run_words[9];
      aot_gpr_31 = aot_run_words[10];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(912));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0891B7DC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-64));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), ctx.gpr[18]);
    ctx.gpr[17] = (aot_gpr_5 | 0u);
    ctx.gpr[18] = (aot_gpr_6 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_16);
    { const std::uint32_t aot_run_words[6]{ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(28), aot_run_words); }
    aot_gpr_31 = (0x0891B814u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 601u, 0x0891B814u, 0x089082A8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 51u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 51u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 51u, 0x089082A8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B814u) goto L_0891B814;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B814:
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
    aot_gpr_4 = (ctx.gpr[19] << 5u);
    aot_gpr_5 = (ctx.gpr[19] << 2u);
    aot_gpr_4 = (aot_gpr_4 - aot_gpr_5);
    ctx.gpr[20] = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(4)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[17];
    ctx.gpr[21] = (0u | 3u);
      if (branch_taken) {
          goto L_0891B8FC;
      }
      goto L_0891B838;
    }
L_0891B838:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[17]) < 40 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 + ctx.gpr[18]);
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[20] + static_cast<std::uint32_t>(16), aot_gpr_4);
      if (branch_taken) {
          goto L_0891B88C;
      }
      goto L_0891B84C;
    }
L_0891B84C:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[17]) <= 0;
    aot_gpr_5 = (2234u << 16u);
      if (branch_taken) {
          goto L_0891B88C;
      }
      goto L_0891B854;
    }
L_0891B854:
    aot_gpr_4 = (ctx.gpr[17] << 2u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(13440));
    ctx.gpr[17] = (aot_gpr_4 + aot_gpr_5);
    ctx.gpr[17] = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[17]) < 0;
      if (branch_taken) {
          goto L_0891B88C;
      }
      goto L_0891B86C;
    }
L_0891B86C:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(16)));
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_5) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    if (ctx.gpr[7] != 0u) {
    aot_gpr_6 = (aot_gpr_5 | 0u);
        goto L_0891B884;
    }
    goto L_0891B884;
L_0891B884:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(16), aot_gpr_6);
      if (branch_taken) {
          goto L_0891B8AC;
      }
      goto L_0891B88C;
    }
L_0891B88C:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(16)));
    aot_gpr_6 = (2u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-31073));
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_5) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    if (ctx.gpr[7] != 0u) {
    aot_gpr_6 = (aot_gpr_5 | 0u);
        goto L_0891B8A8;
    }
    goto L_0891B8A8;
L_0891B8A8:
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(16), aot_gpr_6);
    goto L_0891B8AC;
L_0891B8AC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 10000 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891B8D4;
      }
      goto L_0891B8BC;
    }
L_0891B8BC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 20000 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B8D4;
      }
      goto L_0891B8CC;
    }
L_0891B8CC:
    aot_gpr_4 = (0u | 9999u);
    aot_mem.aot_direct_store32(ctx.gpr[20] + static_cast<std::uint32_t>(16), aot_gpr_4);
    goto L_0891B8D4;
L_0891B8D4:
    aot_gpr_31 = (0x0891B8DCu);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0143.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 613u, 0x0891B8DCu, 0x08A40FCCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0143_entry(rt, ctx, 212u, aot_mem);
#else
        recomp_unit_0143_entry(rt, ctx, 212u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0143_entry, 143u, 212u, 0x08A40FCCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B8DCu) goto L_0891B8DC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B8DC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(8)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[21];
      if (branch_taken) {
          goto L_0891BA4C;
      }
      goto L_0891B8E8;
    }
L_0891B8E8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) <= 0;
      if (branch_taken) {
          goto L_0891BA4C;
      }
      goto L_0891B8F4;
    }
L_0891B8F4:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[20] + static_cast<std::uint32_t>(8), 0u);
      if (branch_taken) {
          goto L_0891BA4C;
      }
      goto L_0891B8FC;
    }
L_0891B8FC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(4)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B970;
      }
      goto L_0891B908;
    }
L_0891B908:
    aot_gpr_4 = (ctx.gpr[19] ^ 4u);
    aot_gpr_5 = (ctx.gpr[19] ^ 5u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_6 = (ctx.gpr[19] ^ 6u);
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_gpr_5 = (aot_gpr_6 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B954;
      }
      goto L_0891B934;
    }
L_0891B934:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(16)));
    ctx.gpr[18] = (ctx.gpr[18] + aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[18] < static_cast<std::uint32_t>(10000) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    aot_gpr_4 = (ctx.gpr[18] < static_cast<std::uint32_t>(20000) ? 1u : 0u);
      if (branch_taken) {
          goto L_0891B954;
      }
      goto L_0891B948;
    }
L_0891B948:
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891B954;
      }
      goto L_0891B950;
    }
L_0891B950:
    ctx.gpr[18] = (0u | 9999u);
    goto L_0891B954;
L_0891B954:
    aot_gpr_31 = (0x0891B95Cu);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(4)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B1FD70, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0198_entry, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B95Cu) goto L_0891B95C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B95C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(96)));
    aot_gpr_31 = (0x0891B968u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 623u, 0x0891B968u, 0x08907F9Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 922u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 922u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 922u, 0x08907F9Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B968u) goto L_0891B968;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B968:
    aot_gpr_31 = (0x0891B970u);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0143.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 624u, 0x0891B970u, 0x08A40C70u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0143_entry(rt, ctx, 148u, aot_mem);
#else
        recomp_unit_0143_entry(rt, ctx, 148u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0143_entry, 143u, 148u, 0x08A40C70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B970u) goto L_0891B970;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B970:
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_0891B9F4;
      }
      goto L_0891B978;
    }
L_0891B978:
    aot_gpr_31 = (0x0891B980u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B1FD70, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0198_entry, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B980u) goto L_0891B980;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B980:
    ctx.gpr[22] = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(96)));
    aot_gpr_31 = (0x0891B98Cu);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B1FD70, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0198_entry, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B98Cu) goto L_0891B98C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891B98C:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[22]) < 0;
    ctx.gpr[23] = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(100)));
      if (branch_taken) {
          goto L_0891B9A4;
      }
      goto L_0891B994;
    }
L_0891B994:
    aot_gpr_31 = (0x0891B99Cu);
    aot_gpr_4 = (ctx.gpr[22] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 629u, 0x0891B99Cu, 0x08AD3698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 607u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 607u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 607u, 0x08AD3698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B99Cu) goto L_0891B99C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B99C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891B9BC;
      }
      goto L_0891B9A4;
    }
L_0891B9A4:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[23]) < 0;
      if (branch_taken) {
          goto L_0891B9F4;
      }
      goto L_0891B9AC;
    }
L_0891B9AC:
    aot_gpr_31 = (0x0891B9B4u);
    aot_gpr_4 = (ctx.gpr[23] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 632u, 0x0891B9B4u, 0x08AD3698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 607u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 607u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 607u, 0x08AD3698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B9B4u) goto L_0891B9B4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B9B4:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891B9F4;
      }
      goto L_0891B9BC;
    }
L_0891B9BC:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[22]) < 0;
      if (branch_taken) {
          goto L_0891B9D4;
      }
      goto L_0891B9C4;
    }
L_0891B9C4:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-20280));
    aot_gpr_4 = (ctx.gpr[22] | 0u);
    aot_gpr_31 = (0x0891B9D4u);
    aot_gpr_5 = (0u | 4u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 635u, 0x0891B9D4u, 0x08AD3258u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 546u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 546u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 546u, 0x08AD3258u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B9D4u) goto L_0891B9D4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B9D4:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[23]) < 0;
      if (branch_taken) {
          goto L_0891B9EC;
      }
      goto L_0891B9DC;
    }
L_0891B9DC:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-20280));
    aot_gpr_4 = (ctx.gpr[23] | 0u);
    aot_gpr_31 = (0x0891B9ECu);
    aot_gpr_5 = (0u | 4u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 637u, 0x0891B9ECu, 0x08AD3258u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 546u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 546u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 546u, 0x08AD3258u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B9ECu) goto L_0891B9EC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B9EC:
    aot_gpr_31 = (0x0891B9F4u);
    aot_gpr_4 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 638u, 0x0891B9F4u, 0x08AD3610u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 598u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 598u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 598u, 0x08AD3610u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891B9F4u) goto L_0891B9F4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891B9F4:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0891BA04u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0143.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 639u, 0x0891BA04u, 0x08A40B6Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0143_entry(rt, ctx, 131u, aot_mem);
#else
        recomp_unit_0143_entry(rt, ctx, 131u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0143_entry, 143u, 131u, 0x08A40B6Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BA04u) goto L_0891BA04;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BA04:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(1929))))));
    { const bool branch_taken = ctx.gpr[19] != aot_gpr_4;
      if (branch_taken) {
          goto L_0891BA4C;
      }
      goto L_0891BA10;
    }
L_0891BA10:
    aot_gpr_31 = (0x0891BA18u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 641u, 0x0891BA18u, 0x08910B38u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 195u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 195u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 195u, 0x08910B38u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BA18u) goto L_0891BA18;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BA18:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BA4C;
      }
      goto L_0891BA20;
    }
L_0891BA20:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    aot_gpr_31 = (0x0891BA40u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B1FD70, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0198_entry, 198u, 733u, 0x08B1FD70u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BA40u) goto L_0891BA40;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BA40:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(96)));
    aot_gpr_31 = (0x0891BA4Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0064.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 644u, 0x0891BA4Cu, 0x08907E38u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0064_entry(rt, ctx, 897u, aot_mem);
#else
        recomp_unit_0064_entry(rt, ctx, 897u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0064_entry, 64u, 897u, 0x08907E38u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BA4Cu) goto L_0891BA4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BA4C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(8)));
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[21];
      if (branch_taken) {
          goto L_0891BA5C;
      }
      goto L_0891BA58;
    }
L_0891BA58:
    aot_mem.aot_direct_store32(ctx.gpr[20] + static_cast<std::uint32_t>(8), 0u);
    goto L_0891BA5C;
L_0891BA5C:
    ctx.gpr[2] = (ctx.gpr[19] | 0u);
    { std::uint32_t aot_run_words[9]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      ctx.gpr[22] = aot_run_words[6];
      ctx.gpr[23] = aot_run_words[7];
      aot_gpr_31 = aot_run_words[8];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(64));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0891BA8C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(236)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_4 | 0u);
    aot_gpr_4 = (ctx.gpr[8] & 256u);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    ctx.gpr[8] = (aot_gpr_4 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_4 = (aot_gpr_5 | 0u);
    aot_gpr_16 = (aot_gpr_6 | 0u);
    { const std::uint32_t aot_run_words[5]{ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_run_words); }
    { const bool branch_taken = ctx.gpr[8] == 0u;
    ctx.gpr[18] = (ctx.gpr[7] | 0u);
      if (branch_taken) {
          goto L_0891BAE4;
      }
      goto L_0891BAD0;
    }
L_0891BAD0:
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_6 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0891BAE4u);
    ctx.gpr[7] = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0067.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 649u, 0x0891BAE4u, 0x089119D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0067_entry(rt, ctx, 425u, aot_mem);
#else
        recomp_unit_0067_entry(rt, ctx, 425u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0067_entry, 67u, 425u, 0x089119D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BAE4u) goto L_0891BAE4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BAE4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 17u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BB70;
      }
      goto L_0891BAF4;
    }
L_0891BAF4:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[17] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20252)));
    ctx.gpr[20] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20256)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 31u));
    aot_gpr_6 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_31 = (0x0891BB30u);
    aot_gpr_6 = (aot_gpr_6 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 651u, 0x0891BB30u, 0x08B60E84u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 178u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 178u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 178u, 0x08B60E84u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BB30u) goto L_0891BB30;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BB30:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20220)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20224)));
    ctx.gpr[9] = (ctx.gpr[3] & ctx.gpr[21]);
    ctx.gpr[8] = (ctx.gpr[2] & ctx.gpr[20]);
    { const bool branch_taken = ctx.gpr[8] != aot_gpr_6;
      if (branch_taken) {
          goto L_0891BB50;
      }
      goto L_0891BB48;
    }
L_0891BB48:
    { const bool branch_taken = ctx.gpr[9] == ctx.gpr[7];
      if (branch_taken) {
          goto L_0891BB70;
      }
      goto L_0891BB50;
    }
L_0891BB50:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (0u | 6u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BB70;
      }
      goto L_0891BB60;
    }
L_0891BB60:
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(0), 0u);
    aot_gpr_4 = (0u | 44u);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[18] + static_cast<std::uint32_t>(0), aot_gpr_4);
      if (branch_taken) {
          goto L_0891BB70;
      }
      goto L_0891BB70;
    }
L_0891BB70:
    aot_gpr_31 = (0x0891BB78u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BB78u) goto L_0891BB78;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BB78:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BC00;
      }
      goto L_0891BB80;
    }
L_0891BB80:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[17] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20252)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20256)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 31u));
    ctx.gpr[8] = (aot_gpr_4 | 0u);
    aot_gpr_5 = (ctx.gpr[7] | 0u);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_31 = (0x0891BBBCu);
    aot_gpr_6 = (ctx.gpr[8] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 658u, 0x0891BBBCu, 0x08B60E84u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 178u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 178u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 178u, 0x08B60E84u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BBBCu) goto L_0891BBBC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BBBC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20028)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20032)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20220)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20224)));
    aot_gpr_5 = (ctx.gpr[3] & aot_gpr_5);
    aot_gpr_4 = (ctx.gpr[2] & aot_gpr_4);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_6;
      if (branch_taken) {
          goto L_0891BBE4;
      }
      goto L_0891BBDC;
    }
L_0891BBDC:
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[7];
      if (branch_taken) {
          goto L_0891BC00;
      }
      goto L_0891BBE4;
    }
L_0891BBE4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-2));
    aot_gpr_5 = (aot_gpr_4 < static_cast<std::uint32_t>(6) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0891BC00;
      }
      goto L_0891BBF8;
    }
L_0891BBF8:
    aot_gpr_4 = (0u | 33u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(0), aot_gpr_4);
    goto L_0891BC00;
L_0891BC00:
    aot_gpr_31 = (0x0891BC08u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BC08u) goto L_0891BC08;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BC08:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BC9C;
      }
      goto L_0891BC10;
    }
L_0891BC10:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(1360)));
    ctx.gpr[19] = (0u | 6u);
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[19];
      if (branch_taken) {
          goto L_0891BC9C;
      }
      goto L_0891BC20;
    }
L_0891BC20:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[17] + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20252)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20256)));
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_4) >> 31u));
    ctx.gpr[8] = (aot_gpr_4 | 0u);
    aot_gpr_5 = (ctx.gpr[7] | 0u);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_31 = (0x0891BC5Cu);
    aot_gpr_6 = (ctx.gpr[8] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 666u, 0x0891BC5Cu, 0x08B60E84u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 178u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 178u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 178u, 0x08B60E84u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BC5Cu) goto L_0891BC5C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BC5C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20020)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20024)));
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20220)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-20224)));
    aot_gpr_5 = (ctx.gpr[3] & aot_gpr_5);
    aot_gpr_4 = (ctx.gpr[2] & aot_gpr_4);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_6;
      if (branch_taken) {
          goto L_0891BC84;
      }
      goto L_0891BC7C;
    }
L_0891BC7C:
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[7];
      if (branch_taken) {
          goto L_0891BC9C;
      }
      goto L_0891BC84;
    }
L_0891BC84:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[19];
      if (branch_taken) {
          goto L_0891BC9C;
      }
      goto L_0891BC90;
    }
L_0891BC90:
    aot_gpr_4 = (0u | 32u);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(0), aot_gpr_4);
      if (branch_taken) {
          goto L_0891BC9C;
      }
      goto L_0891BC9C;
    }
L_0891BC9C:
    aot_gpr_31 = (0x0891BCA4u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BCA4u) goto L_0891BCA4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BCA4:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891BD0C;
      }
      goto L_0891BCAC;
    }
L_0891BCAC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (0u | 18u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BD0C;
      }
      goto L_0891BCBC;
    }
L_0891BCBC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (0u | 214u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BD0C;
      }
      goto L_0891BCCC;
    }
L_0891BCCC:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[17] + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    ctx.gpr[17] = (ctx.gpr[17] + aot_gpr_4);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (0u | 26u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891BD04;
      }
      goto L_0891BCF4;
    }
L_0891BCF4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (0u | 24u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BD0C;
      }
      goto L_0891BD04;
    }
L_0891BD04:
    aot_gpr_4 = (0u | 122u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(0), aot_gpr_4);
    goto L_0891BD0C;
L_0891BD0C:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0891BD30:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    { const std::uint32_t aot_run_words[4]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    aot_gpr_31 = (0x0891BD58u);
    aot_gpr_6 = (0u | 172u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 679u, 0x0891BD58u, 0x08908C10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 216u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 216u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 216u, 0x08908C10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BD58u) goto L_0891BD58;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BD58:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BD8C;
      }
      goto L_0891BD60;
    }
L_0891BD60:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(80)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0891BD70u);
    aot_gpr_6 = (0u | 218u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 681u, 0x0891BD70u, 0x08908C10u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 216u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 216u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 216u, 0x08908C10u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BD70u) goto L_0891BD70;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BD70:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BD8C;
      }
      goto L_0891BD78;
    }
L_0891BD78:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1736)));
    if (aot_gpr_4 != 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1736)));
        goto L_0891BD9C;
    }
    goto L_0891BD84;
L_0891BD84:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891BDBC;
      }
      goto L_0891BD8C;
    }
L_0891BD8C:
    aot_gpr_31 = (0x0891BD94u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 685u, 0x0891BD94u, 0x08909950u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 426u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 426u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 426u, 0x08909950u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BD94u) goto L_0891BD94;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BD94:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BD9C;
    }
L_0891BD9C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891BDBC;
      }
      goto L_0891BDAC;
    }
L_0891BDAC:
    aot_gpr_31 = (0x0891BDB4u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08909B60, 65u, 454u, 0x08909B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0065_entry, 65u, 454u, 0x08909B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BDB4u) goto L_0891BDB4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BDB4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891BDD8;
      }
      goto L_0891BDBC;
    }
L_0891BDBC:
    aot_gpr_31 = (0x0891BDC4u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 690u, 0x0891BDC4u, 0x089098ECu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 416u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 416u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 416u, 0x089098ECu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BDC4u) goto L_0891BDC4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BDC4:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BDD8;
      }
      goto L_0891BDCC;
    }
L_0891BDCC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(860)));
    aot_gpr_4 = (aot_gpr_4 | 2u);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(860), aot_gpr_4);
    goto L_0891BDD8;
L_0891BDD8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 6u, 0x0891C04Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BDE4;
    }
L_0891BDE4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_5 = (0u | 3u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 >> 1u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BE1C;
      }
      goto L_0891BE00;
    }
L_0891BE00:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_5 = (aot_gpr_29 | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(816));
    aot_gpr_31 = (0x0891BE14u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0123.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 695u, 0x0891BE14u, 0x089F3910u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0123_entry(rt, ctx, 608u, aot_mem);
#else
        recomp_unit_0123_entry(rt, ctx, 608u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0123_entry, 123u, 608u, 0x089F3910u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BE14u) goto L_0891BE14;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BE14:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0891BE2C;
      }
      goto L_0891BE1C;
    }
L_0891BE1C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_29 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    goto L_0891BE2C;
L_0891BE2C:
    aot_gpr_4 = (aot_gpr_16 + static_cast<std::uint32_t>(816));
    aot_gpr_31 = (0x0891BE38u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0123.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 698u, 0x0891BE38u, 0x089F3B64u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0123_entry(rt, ctx, 628u, aot_mem);
#else
        recomp_unit_0123_entry(rt, ctx, 628u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0123_entry, 123u, 628u, 0x089F3B64u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BE38u) goto L_0891BE38;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BE38:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BE60;
      }
      goto L_0891BE40;
    }
L_0891BE40:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_4 = (aot_gpr_4 & 32u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BE50;
    }
L_0891BE50:
    aot_gpr_31 = (0x0891BE58u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08909B60, 65u, 454u, 0x08909B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0065_entry, 65u, 454u, 0x08909B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BE58u) goto L_0891BE58;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BE58:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BE60;
    }
L_0891BE60:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(464)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BE70;
    }
L_0891BE70:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BE80;
    }
L_0891BE80:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(456)));
    aot_gpr_4 = (aot_gpr_4 & 512u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BE90;
    }
L_0891BE90:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 36u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BEA0;
    }
L_0891BEA0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1736)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_gpr_4 = (aot_gpr_4 - aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1000) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 13u, 0x0891C0CCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BEB8;
    }
L_0891BEB8:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(1929))))));
    ctx.gpr[19] = (0u | 173u);
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_31 = (0x0891BEE0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0069->0143.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0069_entry, 708u, 0x0891BEE0u, 0x08A40D58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0143_entry(rt, ctx, 161u, aot_mem);
#else
        recomp_unit_0143_entry(rt, ctx, 161u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0143_entry, 143u, 161u, 0x08A40D58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BEE0u) goto L_0891BEE0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0891BEE0:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0891BF14;
      }
      goto L_0891BEE8;
    }
L_0891BEE8:
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(1929))))));
    aot_gpr_5 = (aot_gpr_4 << 5u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 - aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1396));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (0u | 32u);
    { const bool branch_taken = aot_gpr_4 == aot_gpr_5;
      if (branch_taken) {
          goto L_0891BF14;
      }
      goto L_0891BF10;
    }
L_0891BF10:
    ctx.gpr[17] = (0u | 1u);
    goto L_0891BF14;
L_0891BF14:
    ctx.gpr[17] = (ctx.gpr[17] & 255u);
    aot_gpr_31 = (0x0891BF20u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0890D44C, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0066_entry, 66u, 315u, 0x0890D44Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BF20u) goto L_0891BF20;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BF20:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0891BFCC;
      }
      goto L_0891BF28;
    }
L_0891BF28:
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_0891BFCC;
      }
      goto L_0891BF30;
    }
L_0891BF30:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(72)));
    aot_gpr_4 = (aot_gpr_4 & 14u);
    aot_gpr_4 = (aot_gpr_4 ^ 6u);
    aot_gpr_4 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891BF9C;
      }
      goto L_0891BF50;
    }
L_0891BF50:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1364)));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(23))))));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 49 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0891BF78;
      }
      goto L_0891BF64;
    }
L_0891BF64:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_5 = (0u | 6u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(1360)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BF80;
      }
      goto L_0891BF78;
    }
L_0891BF78:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[19] = (0u | 158u);
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BF80;
    }
L_0891BF80:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1364)));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(23))))));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 47 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BF94;
    }
L_0891BF94:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[19] = (0u | 56u);
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BF9C;
    }
L_0891BF9C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1364)));
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(23))))));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < 50 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891BFC4;
      }
      goto L_0891BFB0;
    }
L_0891BFB0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(1716)));
    aot_gpr_5 = (0u | 207u);
    aot_gpr_4 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(aot_mem.aot_direct_load16(aot_gpr_4 + static_cast<std::uint32_t>(86))))));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BFC4;
    }
L_0891BFC4:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[19] = (0u | 158u);
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BFCC;
    }
L_0891BFCC:
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BFD4;
    }
L_0891BFD4:
    aot_gpr_31 = (0x0891BFDCu);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B20934, 199u, 110u, 0x08B20934u>(ctx, &aot_mem, ctx.pc, ctx.hi, ctx.lo, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[28], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0199_entry, 199u, 110u, 0x08B20934u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0891BFDCu) goto L_0891BFDC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0891BFDC:
    aot_gpr_4 = (ctx.gpr[2] & 65535u);
    aot_gpr_4 = (aot_gpr_4 & 1u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0891BFF0;
      }
      goto L_0891BFEC;
    }
L_0891BFEC:
    ctx.gpr[19] = (0u | 158u);
    goto L_0891BFF0;
L_0891BFF0:
    aot_gpr_4 = (0u | 173u);
    { const bool branch_taken = ctx.gpr[19] == aot_gpr_4;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0070_entry, 70u, 3u, 0x0891C01Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0891BFFC;
    }
L_0891BFFC:
    ctx.gpr[8] = (16512u << 16u);
    ctx.pc = 0x0891C000u; AOT_REGCACHE_SYNC_OUT(); return;
}

#undef AOT_REGCACHE_SYNC_IN
#undef AOT_REGCACHE_SYNC_OUT

void recomp_unit_0069(Runtime &rt, AllegrexContext &ctx) {
    auto aot_mem = rt.memory().aot_fast_view();
    recomp_unit_0069_entry(rt, ctx, 0u, aot_mem);
}

void register_generated_unit_69(Runtime &runtime) {
    runtime.register_generated_unit(69u, 0x08918000u, 16384u, &recomp_unit_0069, &recomp_unit_0069_entry);
    // PSPRECOMP_V812_COMPACT_REGISTRATION: same entry set, looped once in Runtime instead of source-expanded calls.
    runtime.register_generated_entry_mask(0x08918000u, &recomp_unit_0069, "recomp_unit_0069",
                                          kEntryMasks_recomp_unit_0069, 64u);
}
} // namespace psprecomp
