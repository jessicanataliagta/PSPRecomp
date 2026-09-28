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
static constexpr std::uint64_t kEntryMasks_recomp_unit_0001[64] = {
    0x020C120281A24011ull, 0x8048008409010812ull, 0x2280240010100320ull, 0x0812020080050401ull,
    0x4101120082022401ull, 0x1040448020808900ull, 0x0410112008202240ull, 0x0104044802080890ull,
    0x0914012000820224ull, 0x0209001004002820ull, 0x0240412022401010ull, 0x1040448021012021ull,
    0x0410112008202240ull, 0x1000900404008240ull, 0x1024040400824004ull, 0x020C120280644802ull,
    0x2121112000042A12ull, 0xA8004542A14A0021ull, 0x1428400585282850ull, 0x0A10080220088881ull,
    0x25444450212A2245ull, 0x450A140624882204ull, 0x886A94A145052801ull, 0x6A10840AAA528514ull,
    0x50A28008100AAA86ull, 0x51401522084104AAull, 0x10508942420A9528ull, 0x2B4D534280022362ull,
    0x102A00A32405458Aull, 0x8800802D42480600ull, 0x5510816932A2284Aull, 0x0050A00A08A8A852ull,
    0x020A005155113114ull, 0x100504402A204B4Aull, 0xA28884852A820020ull, 0x6AA4D41324121354ull,
    0x440040AA86604081ull, 0xA106282290049084ull, 0xAA90028120050210ull, 0x0002000435154402ull,
    0xA014080000000000ull, 0x30984C2494209110ull, 0x0522A2191554C261ull, 0x5488080250152008ull,
    0x000204924920108Dull, 0x401544A801002102ull, 0x2001281480018104ull, 0x512449891004000Aull,
    0x9292100201889294ull, 0x0D52501802460922ull, 0x2A9010A494A90804ull, 0x408092288A810086ull,
    0x2441085222888852ull, 0x4854928AA0425421ull, 0x0824804122404229ull, 0x8084408451082490ull,
    0xA488506A91004124ull, 0xA491510814842010ull, 0x4808484101195081ull, 0x8814808145448022ull,
    0x4850408890480432ull, 0x044A20114909208Aull, 0x144511488A54A525ull, 0x945492A524020C51ull,
};
static constexpr std::uint16_t kEntryBases_recomp_unit_0001[64] = {
    1u, 15u, 26u, 36u, 45u, 56u, 66u, 76u, 86u, 97u, 105u, 115u, 126u, 136u, 144u, 153u,
    166u, 179u, 195u, 211u, 222u, 241u, 257u, 277u, 298u, 315u, 332u, 350u, 372u, 390u, 403u, 425u,
    441u, 458u, 474u, 490u, 514u, 529u, 545u, 559u, 571u, 576u, 594u, 616u, 630u, 644u, 657u, 668u,
    683u, 700u, 718u, 736u, 751u, 768u, 788u, 802u, 816u, 834u, 850u, 865u, 880u, 895u, 911u, 933u,
};
void recomp_unit_0001_entry(Runtime &rt, AllegrexContext &ctx, std::uint16_t direct_entry_id, GuestMemory::AotFastView &aot_mem) {
// PSPRECOMP_AOT_REGCACHE_BEGIN
// PSPRECOMP_AOT_REGCACHE_META gprs=4,5,31,29,16,6 fprs=12,20,22,13 gpr_occ=4683 fpr_occ=299 gpr_total=6242 fpr_total=374
    std::uint32_t aot_gpr_4 = ctx.gpr[4];
    std::uint32_t aot_gpr_5 = ctx.gpr[5];
    std::uint32_t aot_gpr_31 = ctx.gpr[31];
    std::uint32_t aot_gpr_29 = ctx.gpr[29];
    std::uint32_t aot_gpr_16 = ctx.gpr[16];
    std::uint32_t aot_gpr_6 = ctx.gpr[6];
    float aot_fpr_12 = ctx.fpr[12];
    float aot_fpr_20 = ctx.fpr[20];
    float aot_fpr_22 = ctx.fpr[22];
    float aot_fpr_13 = ctx.fpr[13];
    bool aot_regcache_valid = true;
#define AOT_REGCACHE_SYNC_OUT() do { if (aot_regcache_valid) { ctx.gpr[4] = aot_gpr_4; ctx.gpr[5] = aot_gpr_5; ctx.gpr[31] = aot_gpr_31; ctx.gpr[29] = aot_gpr_29; ctx.gpr[16] = aot_gpr_16; ctx.gpr[6] = aot_gpr_6; ctx.fpr[12] = aot_fpr_12; ctx.fpr[20] = aot_fpr_20; ctx.fpr[22] = aot_fpr_22; ctx.fpr[13] = aot_fpr_13; } } while (false)
#define AOT_REGCACHE_SYNC_IN() do { if (aot_regcache_valid) { aot_gpr_4 = ctx.gpr[4]; aot_gpr_5 = ctx.gpr[5]; aot_gpr_31 = ctx.gpr[31]; aot_gpr_29 = ctx.gpr[29]; aot_gpr_16 = ctx.gpr[16]; aot_gpr_6 = ctx.gpr[6]; aot_fpr_12 = ctx.fpr[12]; aot_fpr_20 = ctx.fpr[20]; aot_fpr_22 = ctx.fpr[22]; aot_fpr_13 = ctx.fpr[13]; } } while (false)
// PSPRECOMP_AOT_REGCACHE_END
    std::uint32_t jump_target = 0u;
    std::uint32_t local_transfers = 0u;
    std::uint32_t local_redispatch_rounds = 0u;
    std::uint32_t local_pc = ctx.pc;
    std::uint32_t entry_id = direct_entry_id;
LOCAL_DISPATCH:
    {
    if (entry_id == 0u) {
        const std::uint32_t entry_delta = local_pc - 0x08808000u;
        entry_id = 0u;
        if (entry_delta < 16384u && (entry_delta & 3u) == 0u) {
            const std::uint32_t entry_slot = entry_delta >> 2u;
            const std::uint32_t entry_group = entry_slot >> 6u;
            const std::uint64_t entry_mask = kEntryMasks_recomp_unit_0001[entry_group];
            const std::uint64_t entry_bit = 1ull << (entry_slot & 63u);
            if ((entry_mask & entry_bit) != 0u) {
                entry_id = static_cast<std::uint16_t>(
                    kEntryBases_recomp_unit_0001[entry_group] +
                    std::popcount(entry_mask & (entry_bit - 1ull)));
            }
        }
    }
    switch (entry_id) {
    case 1u: goto L_08808000;
    case 2u: goto L_08808010;
    case 3u: goto L_08808038;
    case 4u: goto L_08808044;
    case 5u: goto L_08808054;
    case 6u: goto L_0880805C;
    case 7u: goto L_08808060;
    case 8u: goto L_0880807C;
    case 9u: goto L_08808084;
    case 10u: goto L_088080A4;
    case 11u: goto L_088080B0;
    case 12u: goto L_088080C8;
    case 13u: goto L_088080CC;
    case 14u: goto L_088080E4;
    case 15u: goto L_08808104;
    case 16u: goto L_08808110;
    case 17u: goto L_0880812C;
    case 18u: goto L_08808140;
    case 19u: goto L_08808160;
    case 20u: goto L_0880816C;
    case 21u: goto L_08808188;
    case 22u: goto L_0880819C;
    case 23u: goto L_088081CC;
    case 24u: goto L_088081D8;
    case 25u: goto L_088081FC;
    case 26u: goto L_08808214;
    case 27u: goto L_08808220;
    case 28u: goto L_08808224;
    case 29u: goto L_08808250;
    case 30u: goto L_08808270;
    case 31u: goto L_088082A8;
    case 32u: goto L_088082B4;
    case 33u: goto L_088082DC;
    case 34u: goto L_088082E4;
    case 35u: goto L_088082F4;
    case 36u: goto L_08808300;
    case 37u: goto L_08808328;
    case 38u: goto L_08808340;
    case 39u: goto L_08808348;
    case 40u: goto L_0880837C;
    case 41u: goto L_088083A4;
    case 42u: goto L_088083C4;
    case 43u: goto L_088083D0;
    case 44u: goto L_088083EC;
    case 45u: goto L_08808400;
    case 46u: goto L_08808428;
    case 47u: goto L_08808434;
    case 48u: goto L_08808444;
    case 49u: goto L_08808464;
    case 50u: goto L_0880847C;
    case 51u: goto L_088084A4;
    case 52u: goto L_088084B0;
    case 53u: goto L_088084C0;
    case 54u: goto L_088084E0;
    case 55u: goto L_088084F8;
    case 56u: goto L_08808520;
    case 57u: goto L_0880852C;
    case 58u: goto L_0880853C;
    case 59u: goto L_0880855C;
    case 60u: goto L_08808574;
    case 61u: goto L_0880859C;
    case 62u: goto L_088085A8;
    case 63u: goto L_088085B8;
    case 64u: goto L_088085D8;
    case 65u: goto L_088085F0;
    case 66u: goto L_08808618;
    case 67u: goto L_08808624;
    case 68u: goto L_08808634;
    case 69u: goto L_08808654;
    case 70u: goto L_0880866C;
    case 71u: goto L_08808694;
    case 72u: goto L_088086A0;
    case 73u: goto L_088086B0;
    case 74u: goto L_088086D0;
    case 75u: goto L_088086E8;
    case 76u: goto L_08808710;
    case 77u: goto L_0880871C;
    case 78u: goto L_0880872C;
    case 79u: goto L_0880874C;
    case 80u: goto L_08808764;
    case 81u: goto L_0880878C;
    case 82u: goto L_08808798;
    case 83u: goto L_088087A8;
    case 84u: goto L_088087C8;
    case 85u: goto L_088087E0;
    case 86u: goto L_08808808;
    case 87u: goto L_08808814;
    case 88u: goto L_08808824;
    case 89u: goto L_08808844;
    case 90u: goto L_0880885C;
    case 91u: goto L_08808894;
    case 92u: goto L_088088A0;
    case 93u: goto L_088088C8;
    case 94u: goto L_088088D0;
    case 95u: goto L_088088E0;
    case 96u: goto L_088088EC;
    case 97u: goto L_08808914;
    case 98u: goto L_0880892C;
    case 99u: goto L_08808934;
    case 100u: goto L_08808968;
    case 101u: goto L_08808990;
    case 102u: goto L_088089C0;
    case 103u: goto L_088089CC;
    case 104u: goto L_088089E4;
    case 105u: goto L_08808A10;
    case 106u: goto L_08808A30;
    case 107u: goto L_08808A58;
    case 108u: goto L_08808A64;
    case 109u: goto L_08808A74;
    case 110u: goto L_08808A94;
    case 111u: goto L_08808AA0;
    case 112u: goto L_08808AB8;
    case 113u: goto L_08808AD8;
    case 114u: goto L_08808AE4;
    case 115u: goto L_08808B00;
    case 116u: goto L_08808B14;
    case 117u: goto L_08808B34;
    case 118u: goto L_08808B40;
    case 119u: goto L_08808B60;
    case 120u: goto L_08808B74;
    case 121u: goto L_08808B9C;
    case 122u: goto L_08808BA8;
    case 123u: goto L_08808BB8;
    case 124u: goto L_08808BD8;
    case 125u: goto L_08808BF0;
    case 126u: goto L_08808C18;
    case 127u: goto L_08808C24;
    case 128u: goto L_08808C34;
    case 129u: goto L_08808C54;
    case 130u: goto L_08808C6C;
    case 131u: goto L_08808C94;
    case 132u: goto L_08808CA0;
    case 133u: goto L_08808CB0;
    case 134u: goto L_08808CD0;
    case 135u: goto L_08808CE8;
    case 136u: goto L_08808D18;
    case 137u: goto L_08808D24;
    case 138u: goto L_08808D3C;
    case 139u: goto L_08808D68;
    case 140u: goto L_08808D88;
    case 141u: goto L_08808DB0;
    case 142u: goto L_08808DBC;
    case 143u: goto L_08808DF0;
    case 144u: goto L_08808E08;
    case 145u: goto L_08808E38;
    case 146u: goto L_08808E44;
    case 147u: goto L_08808E5C;
    case 148u: goto L_08808E88;
    case 149u: goto L_08808EA8;
    case 150u: goto L_08808EC8;
    case 151u: goto L_08808ED4;
    case 152u: goto L_08808EF0;
    case 153u: goto L_08808F04;
    case 154u: goto L_08808F2C;
    case 155u: goto L_08808F38;
    case 156u: goto L_08808F48;
    case 157u: goto L_08808F54;
    case 158u: goto L_08808F58;
    case 159u: goto L_08808F7C;
    case 160u: goto L_08808F84;
    case 161u: goto L_08808FA4;
    case 162u: goto L_08808FB0;
    case 163u: goto L_08808FC8;
    case 164u: goto L_08808FCC;
    case 165u: goto L_08808FE4;
    case 166u: goto L_08809004;
    case 167u: goto L_08809010;
    case 168u: goto L_08809024;
    case 169u: goto L_0880902C;
    case 170u: goto L_08809034;
    case 171u: goto L_08809048;
    case 172u: goto L_08809094;
    case 173u: goto L_088090A0;
    case 174u: goto L_088090B0;
    case 175u: goto L_088090C0;
    case 176u: goto L_088090D4;
    case 177u: goto L_088090E0;
    case 178u: goto L_088090F4;
    case 179u: goto L_08809100;
    case 180u: goto L_08809114;
    case 181u: goto L_08809144;
    case 182u: goto L_0880914C;
    case 183u: goto L_08809158;
    case 184u: goto L_08809160;
    case 185u: goto L_08809174;
    case 186u: goto L_0880917C;
    case 187u: goto L_08809184;
    case 188u: goto L_08809198;
    case 189u: goto L_088091A0;
    case 190u: goto L_088091A8;
    case 191u: goto L_088091B8;
    case 192u: goto L_088091EC;
    case 193u: goto L_088091F4;
    case 194u: goto L_088091FC;
    case 195u: goto L_08809210;
    case 196u: goto L_08809218;
    case 197u: goto L_0880922C;
    case 198u: goto L_08809234;
    case 199u: goto L_0880924C;
    case 200u: goto L_08809254;
    case 201u: goto L_08809260;
    case 202u: goto L_08809268;
    case 203u: goto L_0880927C;
    case 204u: goto L_08809280;
    case 205u: goto L_08809288;
    case 206u: goto L_088092B8;
    case 207u: goto L_088092CC;
    case 208u: goto L_088092D4;
    case 209u: goto L_088092E8;
    case 210u: goto L_088092F0;
    case 211u: goto L_08809300;
    case 212u: goto L_0880931C;
    case 213u: goto L_0880932C;
    case 214u: goto L_0880933C;
    case 215u: goto L_0880934C;
    case 216u: goto L_08809374;
    case 217u: goto L_08809384;
    case 218u: goto L_088093AC;
    case 219u: goto L_088093D0;
    case 220u: goto L_088093E4;
    case 221u: goto L_088093EC;
    case 222u: goto L_08809400;
    case 223u: goto L_08809408;
    case 224u: goto L_08809418;
    case 225u: goto L_08809424;
    case 226u: goto L_08809434;
    case 227u: goto L_08809444;
    case 228u: goto L_0880944C;
    case 229u: goto L_08809454;
    case 230u: goto L_08809460;
    case 231u: goto L_08809474;
    case 232u: goto L_08809490;
    case 233u: goto L_08809498;
    case 234u: goto L_088094A8;
    case 235u: goto L_088094B8;
    case 236u: goto L_088094C8;
    case 237u: goto L_088094D8;
    case 238u: goto L_088094E0;
    case 239u: goto L_088094E8;
    case 240u: goto L_088094F4;
    case 241u: goto L_08809508;
    case 242u: goto L_08809524;
    case 243u: goto L_08809534;
    case 244u: goto L_0880954C;
    case 245u: goto L_0880955C;
    case 246u: goto L_08809568;
    case 247u: goto L_08809574;
    case 248u: goto L_08809584;
    case 249u: goto L_08809588;
    case 250u: goto L_088095A8;
    case 251u: goto L_088095B0;
    case 252u: goto L_088095C4;
    case 253u: goto L_088095CC;
    case 254u: goto L_088095E0;
    case 255u: goto L_088095E8;
    case 256u: goto L_088095F8;
    case 257u: goto L_08809600;
    case 258u: goto L_0880962C;
    case 259u: goto L_08809634;
    case 260u: goto L_08809640;
    case 261u: goto L_08809648;
    case 262u: goto L_08809660;
    case 263u: goto L_08809668;
    case 264u: goto L_08809678;
    case 265u: goto L_08809680;
    case 266u: goto L_08809694;
    case 267u: goto L_0880969C;
    case 268u: goto L_088096A8;
    case 269u: goto L_088096B0;
    case 270u: goto L_088096BC;
    case 271u: goto L_088096C4;
    case 272u: goto L_088096CC;
    case 273u: goto L_088096D4;
    case 274u: goto L_088096D8;
    case 275u: goto L_088096EC;
    case 276u: goto L_088096FC;
    case 277u: goto L_08809708;
    case 278u: goto L_08809710;
    case 279u: goto L_08809720;
    case 280u: goto L_08809728;
    case 281u: goto L_0880973C;
    case 282u: goto L_08809744;
    case 283u: goto L_08809750;
    case 284u: goto L_08809758;
    case 285u: goto L_08809764;
    case 286u: goto L_0880976C;
    case 287u: goto L_08809774;
    case 288u: goto L_0880977C;
    case 289u: goto L_08809784;
    case 290u: goto L_0880978C;
    case 291u: goto L_088097A8;
    case 292u: goto L_088097BC;
    case 293u: goto L_088097D0;
    case 294u: goto L_088097E4;
    case 295u: goto L_088097EC;
    case 296u: goto L_088097F4;
    case 297u: goto L_088097F8;
    case 298u: goto L_08809804;
    case 299u: goto L_08809808;
    case 300u: goto L_0880981C;
    case 301u: goto L_08809824;
    case 302u: goto L_0880982C;
    case 303u: goto L_08809834;
    case 304u: goto L_0880983C;
    case 305u: goto L_08809844;
    case 306u: goto L_0880984C;
    case 307u: goto L_08809870;
    case 308u: goto L_0880988C;
    case 309u: goto L_088098BC;
    case 310u: goto L_088098C4;
    case 311u: goto L_088098D4;
    case 312u: goto L_088098DC;
    case 313u: goto L_088098F0;
    case 314u: goto L_088098F8;
    case 315u: goto L_08809904;
    case 316u: goto L_0880990C;
    case 317u: goto L_08809914;
    case 318u: goto L_0880991C;
    case 319u: goto L_08809928;
    case 320u: goto L_08809940;
    case 321u: goto L_08809958;
    case 322u: goto L_0880996C;
    case 323u: goto L_08809984;
    case 324u: goto L_08809994;
    case 325u: goto L_088099A0;
    case 326u: goto L_088099A8;
    case 327u: goto L_088099B0;
    case 328u: goto L_088099D8;
    case 329u: goto L_088099E0;
    case 330u: goto L_088099F0;
    case 331u: goto L_088099F8;
    case 332u: goto L_08809A0C;
    case 333u: goto L_08809A14;
    case 334u: goto L_08809A20;
    case 335u: goto L_08809A28;
    case 336u: goto L_08809A30;
    case 337u: goto L_08809A3C;
    case 338u: goto L_08809A44;
    case 339u: goto L_08809A4C;
    case 340u: goto L_08809A64;
    case 341u: goto L_08809A78;
    case 342u: goto L_08809A84;
    case 343u: goto L_08809A98;
    case 344u: goto L_08809AA0;
    case 345u: goto L_08809AAC;
    case 346u: goto L_08809ABC;
    case 347u: goto L_08809AD0;
    case 348u: goto L_08809AD8;
    case 349u: goto L_08809AF0;
    case 350u: goto L_08809B04;
    case 351u: goto L_08809B14;
    case 352u: goto L_08809B18;
    case 353u: goto L_08809B20;
    case 354u: goto L_08809B24;
    case 355u: goto L_08809B34;
    case 356u: goto L_08809B44;
    case 357u: goto L_08809B7C;
    case 358u: goto L_08809B84;
    case 359u: goto L_08809B98;
    case 360u: goto L_08809BA0;
    case 361u: goto L_08809BA4;
    case 362u: goto L_08809BB0;
    case 363u: goto L_08809BB8;
    case 364u: goto L_08809BC0;
    case 365u: goto L_08809BC8;
    case 366u: goto L_08809BCC;
    case 367u: goto L_08809BD8;
    case 368u: goto L_08809BE0;
    case 369u: goto L_08809BE4;
    case 370u: goto L_08809BEC;
    case 371u: goto L_08809BF4;
    case 372u: goto L_08809C04;
    case 373u: goto L_08809C0C;
    case 374u: goto L_08809C1C;
    case 375u: goto L_08809C20;
    case 376u: goto L_08809C28;
    case 377u: goto L_08809C38;
    case 378u: goto L_08809C40;
    case 379u: goto L_08809C48;
    case 380u: goto L_08809C68;
    case 381u: goto L_08809C74;
    case 382u: goto L_08809C80;
    case 383u: goto L_08809C84;
    case 384u: goto L_08809C94;
    case 385u: goto L_08809C9C;
    case 386u: goto L_08809CC4;
    case 387u: goto L_08809CCC;
    case 388u: goto L_08809CD4;
    case 389u: goto L_08809CF0;
    case 390u: goto L_08809D24;
    case 391u: goto L_08809D28;
    case 392u: goto L_08809D4C;
    case 393u: goto L_08809D58;
    case 394u: goto L_08809D64;
    case 395u: goto L_08809D78;
    case 396u: goto L_08809D80;
    case 397u: goto L_08809D88;
    case 398u: goto L_08809D8C;
    case 399u: goto L_08809D94;
    case 400u: goto L_08809DBC;
    case 401u: goto L_08809DEC;
    case 402u: goto L_08809DFC;
    case 403u: goto L_08809E04;
    case 404u: goto L_08809E0C;
    case 405u: goto L_08809E18;
    case 406u: goto L_08809E2C;
    case 407u: goto L_08809E34;
    case 408u: goto L_08809E44;
    case 409u: goto L_08809E54;
    case 410u: goto L_08809E5C;
    case 411u: goto L_08809E64;
    case 412u: goto L_08809E70;
    case 413u: goto L_08809E74;
    case 414u: goto L_08809E80;
    case 415u: goto L_08809E8C;
    case 416u: goto L_08809E94;
    case 417u: goto L_08809E98;
    case 418u: goto L_08809EA0;
    case 419u: goto L_08809EBC;
    case 420u: goto L_08809ED0;
    case 421u: goto L_08809EE0;
    case 422u: goto L_08809EE8;
    case 423u: goto L_08809EF0;
    case 424u: goto L_08809EF8;
    case 425u: goto L_08809F04;
    case 426u: goto L_08809F10;
    case 427u: goto L_08809F18;
    case 428u: goto L_08809F2C;
    case 429u: goto L_08809F34;
    case 430u: goto L_08809F3C;
    case 431u: goto L_08809F4C;
    case 432u: goto L_08809F54;
    case 433u: goto L_08809F5C;
    case 434u: goto L_08809F6C;
    case 435u: goto L_08809F84;
    case 436u: goto L_08809F8C;
    case 437u: goto L_08809FB4;
    case 438u: goto L_08809FBC;
    case 439u: goto L_08809FD0;
    case 440u: goto L_08809FD8;
    case 441u: goto L_0880A008;
    case 442u: goto L_0880A010;
    case 443u: goto L_0880A020;
    case 444u: goto L_0880A030;
    case 445u: goto L_0880A034;
    case 446u: goto L_0880A040;
    case 447u: goto L_0880A050;
    case 448u: goto L_0880A060;
    case 449u: goto L_0880A068;
    case 450u: goto L_0880A070;
    case 451u: goto L_0880A078;
    case 452u: goto L_0880A080;
    case 453u: goto L_0880A090;
    case 454u: goto L_0880A098;
    case 455u: goto L_0880A0C4;
    case 456u: goto L_0880A0CC;
    case 457u: goto L_0880A0E4;
    case 458u: goto L_0880A104;
    case 459u: goto L_0880A10C;
    case 460u: goto L_0880A118;
    case 461u: goto L_0880A120;
    case 462u: goto L_0880A124;
    case 463u: goto L_0880A12C;
    case 464u: goto L_0880A138;
    case 465u: goto L_0880A154;
    case 466u: goto L_0880A164;
    case 467u: goto L_0880A16C;
    case 468u: goto L_0880A174;
    case 469u: goto L_0880A198;
    case 470u: goto L_0880A1A8;
    case 471u: goto L_0880A1C0;
    case 472u: goto L_0880A1C8;
    case 473u: goto L_0880A1F0;
    case 474u: goto L_0880A214;
    case 475u: goto L_0880A244;
    case 476u: goto L_0880A25C;
    case 477u: goto L_0880A264;
    case 478u: goto L_0880A26C;
    case 479u: goto L_0880A274;
    case 480u: goto L_0880A280;
    case 481u: goto L_0880A288;
    case 482u: goto L_0880A29C;
    case 483u: goto L_0880A2A8;
    case 484u: goto L_0880A2BC;
    case 485u: goto L_0880A2CC;
    case 486u: goto L_0880A2DC;
    case 487u: goto L_0880A2E4;
    case 488u: goto L_0880A2F4;
    case 489u: goto L_0880A2FC;
    case 490u: goto L_0880A308;
    case 491u: goto L_0880A310;
    case 492u: goto L_0880A318;
    case 493u: goto L_0880A320;
    case 494u: goto L_0880A324;
    case 495u: goto L_0880A330;
    case 496u: goto L_0880A344;
    case 497u: goto L_0880A350;
    case 498u: goto L_0880A368;
    case 499u: goto L_0880A374;
    case 500u: goto L_0880A380;
    case 501u: goto L_0880A384;
    case 502u: goto L_0880A390;
    case 503u: goto L_0880A3A8;
    case 504u: goto L_0880A3B0;
    case 505u: goto L_0880A3B8;
    case 506u: goto L_0880A3BC;
    case 507u: goto L_0880A3C8;
    case 508u: goto L_0880A3D4;
    case 509u: goto L_0880A3DC;
    case 510u: goto L_0880A3E4;
    case 511u: goto L_0880A3EC;
    case 512u: goto L_0880A3F4;
    case 513u: goto L_0880A3F8;
    case 514u: goto L_0880A400;
    case 515u: goto L_0880A41C;
    case 516u: goto L_0880A438;
    case 517u: goto L_0880A454;
    case 518u: goto L_0880A458;
    case 519u: goto L_0880A464;
    case 520u: goto L_0880A468;
    case 521u: goto L_0880A47C;
    case 522u: goto L_0880A484;
    case 523u: goto L_0880A48C;
    case 524u: goto L_0880A494;
    case 525u: goto L_0880A49C;
    case 526u: goto L_0880A4B8;
    case 527u: goto L_0880A4E8;
    case 528u: goto L_0880A4F8;
    case 529u: goto L_0880A508;
    case 530u: goto L_0880A51C;
    case 531u: goto L_0880A530;
    case 532u: goto L_0880A53C;
    case 533u: goto L_0880A548;
    case 534u: goto L_0880A570;
    case 535u: goto L_0880A57C;
    case 536u: goto L_0880A584;
    case 537u: goto L_0880A594;
    case 538u: goto L_0880A5AC;
    case 539u: goto L_0880A5B4;
    case 540u: goto L_0880A5C4;
    case 541u: goto L_0880A5C8;
    case 542u: goto L_0880A5E0;
    case 543u: goto L_0880A5F4;
    case 544u: goto L_0880A5FC;
    case 545u: goto L_0880A610;
    case 546u: goto L_0880A624;
    case 547u: goto L_0880A640;
    case 548u: goto L_0880A648;
    case 549u: goto L_0880A674;
    case 550u: goto L_0880A680;
    case 551u: goto L_0880A69C;
    case 552u: goto L_0880A6A4;
    case 553u: goto L_0880A6D0;
    case 554u: goto L_0880A6DC;
    case 555u: goto L_0880A6E4;
    case 556u: goto L_0880A6EC;
    case 557u: goto L_0880A6F4;
    case 558u: goto L_0880A6FC;
    case 559u: goto L_0880A704;
    case 560u: goto L_0880A728;
    case 561u: goto L_0880A738;
    case 562u: goto L_0880A740;
    case 563u: goto L_0880A748;
    case 564u: goto L_0880A750;
    case 565u: goto L_0880A760;
    case 566u: goto L_0880A768;
    case 567u: goto L_0880A770;
    case 568u: goto L_0880A774;
    case 569u: goto L_0880A788;
    case 570u: goto L_0880A7C4;
    case 571u: goto L_0880A8AC;
    case 572u: goto L_0880A8C8;
    case 573u: goto L_0880A8D0;
    case 574u: goto L_0880A8F4;
    case 575u: goto L_0880A8FC;
    case 576u: goto L_0880A910;
    case 577u: goto L_0880A920;
    case 578u: goto L_0880A930;
    case 579u: goto L_0880A93C;
    case 580u: goto L_0880A954;
    case 581u: goto L_0880A968;
    case 582u: goto L_0880A970;
    case 583u: goto L_0880A97C;
    case 584u: goto L_0880A988;
    case 585u: goto L_0880A994;
    case 586u: goto L_0880A9A8;
    case 587u: goto L_0880A9AC;
    case 588u: goto L_0880A9B8;
    case 589u: goto L_0880A9CC;
    case 590u: goto L_0880A9D0;
    case 591u: goto L_0880A9DC;
    case 592u: goto L_0880A9F0;
    case 593u: goto L_0880A9F4;
    case 594u: goto L_0880AA00;
    case 595u: goto L_0880AA14;
    case 596u: goto L_0880AA18;
    case 597u: goto L_0880AA24;
    case 598u: goto L_0880AA38;
    case 599u: goto L_0880AA3C;
    case 600u: goto L_0880AA48;
    case 601u: goto L_0880AA50;
    case 602u: goto L_0880AA58;
    case 603u: goto L_0880AA60;
    case 604u: goto L_0880AA68;
    case 605u: goto L_0880AA70;
    case 606u: goto L_0880AA80;
    case 607u: goto L_0880AA8C;
    case 608u: goto L_0880AA90;
    case 609u: goto L_0880AAA4;
    case 610u: goto L_0880AAB4;
    case 611u: goto L_0880AABC;
    case 612u: goto L_0880AAC4;
    case 613u: goto L_0880AAD4;
    case 614u: goto L_0880AAE0;
    case 615u: goto L_0880AAE8;
    case 616u: goto L_0880AB0C;
    case 617u: goto L_0880AB34;
    case 618u: goto L_0880AB40;
    case 619u: goto L_0880AB48;
    case 620u: goto L_0880AB50;
    case 621u: goto L_0880AB70;
    case 622u: goto L_0880AB78;
    case 623u: goto L_0880AB84;
    case 624u: goto L_0880ABAC;
    case 625u: goto L_0880ABCC;
    case 626u: goto L_0880ABDC;
    case 627u: goto L_0880ABE8;
    case 628u: goto L_0880ABF0;
    case 629u: goto L_0880ABF8;
    case 630u: goto L_0880AC00;
    case 631u: goto L_0880AC08;
    case 632u: goto L_0880AC0C;
    case 633u: goto L_0880AC1C;
    case 634u: goto L_0880AC30;
    case 635u: goto L_0880AC54;
    case 636u: goto L_0880AC60;
    case 637u: goto L_0880AC6C;
    case 638u: goto L_0880AC78;
    case 639u: goto L_0880AC84;
    case 640u: goto L_0880AC90;
    case 641u: goto L_0880AC9C;
    case 642u: goto L_0880ACA8;
    case 643u: goto L_0880ACC4;
    case 644u: goto L_0880AD04;
    case 645u: goto L_0880AD20;
    case 646u: goto L_0880AD34;
    case 647u: goto L_0880AD60;
    case 648u: goto L_0880AD8C;
    case 649u: goto L_0880AD94;
    case 650u: goto L_0880AD9C;
    case 651u: goto L_0880ADA8;
    case 652u: goto L_0880ADB8;
    case 653u: goto L_0880ADC0;
    case 654u: goto L_0880ADC8;
    case 655u: goto L_0880ADD0;
    case 656u: goto L_0880ADF8;
    case 657u: goto L_0880AE08;
    case 658u: goto L_0880AE20;
    case 659u: goto L_0880AE3C;
    case 660u: goto L_0880AE40;
    case 661u: goto L_0880AE7C;
    case 662u: goto L_0880AE88;
    case 663u: goto L_0880AE90;
    case 664u: goto L_0880AEAC;
    case 665u: goto L_0880AEB4;
    case 666u: goto L_0880AEC0;
    case 667u: goto L_0880AEF4;
    case 668u: goto L_0880AF04;
    case 669u: goto L_0880AF0C;
    case 670u: goto L_0880AF48;
    case 671u: goto L_0880AF70;
    case 672u: goto L_0880AF80;
    case 673u: goto L_0880AF8C;
    case 674u: goto L_0880AF9C;
    case 675u: goto L_0880AFA0;
    case 676u: goto L_0880AFAC;
    case 677u: goto L_0880AFB8;
    case 678u: goto L_0880AFC8;
    case 679u: goto L_0880AFD4;
    case 680u: goto L_0880AFE0;
    case 681u: goto L_0880AFF0;
    case 682u: goto L_0880AFF8;
    case 683u: goto L_0880B008;
    case 684u: goto L_0880B010;
    case 685u: goto L_0880B01C;
    case 686u: goto L_0880B024;
    case 687u: goto L_0880B030;
    case 688u: goto L_0880B03C;
    case 689u: goto L_0880B04C;
    case 690u: goto L_0880B05C;
    case 691u: goto L_0880B060;
    case 692u: goto L_0880B084;
    case 693u: goto L_0880B0B0;
    case 694u: goto L_0880B0C4;
    case 695u: goto L_0880B0D0;
    case 696u: goto L_0880B0DC;
    case 697u: goto L_0880B0E4;
    case 698u: goto L_0880B0F0;
    case 699u: goto L_0880B0FC;
    case 700u: goto L_0880B104;
    case 701u: goto L_0880B114;
    case 702u: goto L_0880B120;
    case 703u: goto L_0880B12C;
    case 704u: goto L_0880B144;
    case 705u: goto L_0880B148;
    case 706u: goto L_0880B158;
    case 707u: goto L_0880B164;
    case 708u: goto L_0880B18C;
    case 709u: goto L_0880B190;
    case 710u: goto L_0880B1B0;
    case 711u: goto L_0880B1B8;
    case 712u: goto L_0880B1C4;
    case 713u: goto L_0880B1D0;
    case 714u: goto L_0880B1D8;
    case 715u: goto L_0880B1E0;
    case 716u: goto L_0880B1E8;
    case 717u: goto L_0880B1EC;
    case 718u: goto L_0880B208;
    case 719u: goto L_0880B22C;
    case 720u: goto L_0880B240;
    case 721u: goto L_0880B24C;
    case 722u: goto L_0880B254;
    case 723u: goto L_0880B25C;
    case 724u: goto L_0880B268;
    case 725u: goto L_0880B270;
    case 726u: goto L_0880B27C;
    case 727u: goto L_0880B288;
    case 728u: goto L_0880B294;
    case 729u: goto L_0880B29C;
    case 730u: goto L_0880B2B0;
    case 731u: goto L_0880B2D0;
    case 732u: goto L_0880B2DC;
    case 733u: goto L_0880B2E4;
    case 734u: goto L_0880B2EC;
    case 735u: goto L_0880B2F4;
    case 736u: goto L_0880B304;
    case 737u: goto L_0880B308;
    case 738u: goto L_0880B31C;
    case 739u: goto L_0880B340;
    case 740u: goto L_0880B35C;
    case 741u: goto L_0880B364;
    case 742u: goto L_0880B36C;
    case 743u: goto L_0880B37C;
    case 744u: goto L_0880B38C;
    case 745u: goto L_0880B394;
    case 746u: goto L_0880B3A4;
    case 747u: goto L_0880B3B0;
    case 748u: goto L_0880B3BC;
    case 749u: goto L_0880B3DC;
    case 750u: goto L_0880B3F8;
    case 751u: goto L_0880B404;
    case 752u: goto L_0880B410;
    case 753u: goto L_0880B418;
    case 754u: goto L_0880B42C;
    case 755u: goto L_0880B43C;
    case 756u: goto L_0880B44C;
    case 757u: goto L_0880B45C;
    case 758u: goto L_0880B464;
    case 759u: goto L_0880B474;
    case 760u: goto L_0880B484;
    case 761u: goto L_0880B490;
    case 762u: goto L_0880B498;
    case 763u: goto L_0880B4AC;
    case 764u: goto L_0880B4C0;
    case 765u: goto L_0880B4D8;
    case 766u: goto L_0880B4E8;
    case 767u: goto L_0880B4F4;
    case 768u: goto L_0880B500;
    case 769u: goto L_0880B514;
    case 770u: goto L_0880B528;
    case 771u: goto L_0880B530;
    case 772u: goto L_0880B538;
    case 773u: goto L_0880B544;
    case 774u: goto L_0880B558;
    case 775u: goto L_0880B574;
    case 776u: goto L_0880B57C;
    case 777u: goto L_0880B584;
    case 778u: goto L_0880B58C;
    case 779u: goto L_0880B59C;
    case 780u: goto L_0880B5A4;
    case 781u: goto L_0880B5B0;
    case 782u: goto L_0880B5BC;
    case 783u: goto L_0880B5C8;
    case 784u: goto L_0880B5D0;
    case 785u: goto L_0880B5D8;
    case 786u: goto L_0880B5EC;
    case 787u: goto L_0880B5F8;
    case 788u: goto L_0880B600;
    case 789u: goto L_0880B60C;
    case 790u: goto L_0880B614;
    case 791u: goto L_0880B624;
    case 792u: goto L_0880B638;
    case 793u: goto L_0880B658;
    case 794u: goto L_0880B664;
    case 795u: goto L_0880B674;
    case 796u: goto L_0880B680;
    case 797u: goto L_0880B698;
    case 798u: goto L_0880B6BC;
    case 799u: goto L_0880B6C8;
    case 800u: goto L_0880B6D4;
    case 801u: goto L_0880B6EC;
    case 802u: goto L_0880B710;
    case 803u: goto L_0880B71C;
    case 804u: goto L_0880B728;
    case 805u: goto L_0880B734;
    case 806u: goto L_0880B74C;
    case 807u: goto L_0880B760;
    case 808u: goto L_0880B770;
    case 809u: goto L_0880B778;
    case 810u: goto L_0880B788;
    case 811u: goto L_0880B79C;
    case 812u: goto L_0880B7B8;
    case 813u: goto L_0880B7C8;
    case 814u: goto L_0880B7DC;
    case 815u: goto L_0880B7FC;
    case 816u: goto L_0880B808;
    case 817u: goto L_0880B814;
    case 818u: goto L_0880B820;
    case 819u: goto L_0880B838;
    case 820u: goto L_0880B860;
    case 821u: goto L_0880B870;
    case 822u: goto L_0880B87C;
    case 823u: goto L_0880B884;
    case 824u: goto L_0880B88C;
    case 825u: goto L_0880B894;
    case 826u: goto L_0880B898;
    case 827u: goto L_0880B8B0;
    case 828u: goto L_0880B8B8;
    case 829u: goto L_0880B8CC;
    case 830u: goto L_0880B8DC;
    case 831u: goto L_0880B8E8;
    case 832u: goto L_0880B8F4;
    case 833u: goto L_0880B8FC;
    case 834u: goto L_0880B910;
    case 835u: goto L_0880B934;
    case 836u: goto L_0880B948;
    case 837u: goto L_0880B95C;
    case 838u: goto L_0880B968;
    case 839u: goto L_0880B970;
    case 840u: goto L_0880B98C;
    case 841u: goto L_0880B9A0;
    case 842u: goto L_0880B9B0;
    case 843u: goto L_0880B9B8;
    case 844u: goto L_0880B9C0;
    case 845u: goto L_0880B9D0;
    case 846u: goto L_0880B9DC;
    case 847u: goto L_0880B9E8;
    case 848u: goto L_0880B9F4;
    case 849u: goto L_0880B9FC;
    case 850u: goto L_0880BA00;
    case 851u: goto L_0880BA1C;
    case 852u: goto L_0880BA30;
    case 853u: goto L_0880BA38;
    case 854u: goto L_0880BA40;
    case 855u: goto L_0880BA4C;
    case 856u: goto L_0880BA50;
    case 857u: goto L_0880BA60;
    case 858u: goto L_0880BA80;
    case 859u: goto L_0880BA98;
    case 860u: goto L_0880BAAC;
    case 861u: goto L_0880BAB8;
    case 862u: goto L_0880BACC;
    case 863u: goto L_0880BAEC;
    case 864u: goto L_0880BAF8;
    case 865u: goto L_0880BB04;
    case 866u: goto L_0880BB14;
    case 867u: goto L_0880BB3C;
    case 868u: goto L_0880BB48;
    case 869u: goto L_0880BB58;
    case 870u: goto L_0880BB60;
    case 871u: goto L_0880BB68;
    case 872u: goto L_0880BB78;
    case 873u: goto L_0880BB80;
    case 874u: goto L_0880BB9C;
    case 875u: goto L_0880BBBC;
    case 876u: goto L_0880BBC8;
    case 877u: goto L_0880BBD0;
    case 878u: goto L_0880BBEC;
    case 879u: goto L_0880BBFC;
    case 880u: goto L_0880BC04;
    case 881u: goto L_0880BC10;
    case 882u: goto L_0880BC14;
    case 883u: goto L_0880BC28;
    case 884u: goto L_0880BC4C;
    case 885u: goto L_0880BC58;
    case 886u: goto L_0880BC70;
    case 887u: goto L_0880BC7C;
    case 888u: goto L_0880BC8C;
    case 889u: goto L_0880BC9C;
    case 890u: goto L_0880BCB8;
    case 891u: goto L_0880BCD0;
    case 892u: goto L_0880BCD8;
    case 893u: goto L_0880BCEC;
    case 894u: goto L_0880BCF8;
    case 895u: goto L_0880BD04;
    case 896u: goto L_0880BD0C;
    case 897u: goto L_0880BD1C;
    case 898u: goto L_0880BD34;
    case 899u: goto L_0880BD40;
    case 900u: goto L_0880BD4C;
    case 901u: goto L_0880BD60;
    case 902u: goto L_0880BD6C;
    case 903u: goto L_0880BD78;
    case 904u: goto L_0880BD80;
    case 905u: goto L_0880BD90;
    case 906u: goto L_0880BDB4;
    case 907u: goto L_0880BDC4;
    case 908u: goto L_0880BDCC;
    case 909u: goto L_0880BDD8;
    case 910u: goto L_0880BDE8;
    case 911u: goto L_0880BE00;
    case 912u: goto L_0880BE08;
    case 913u: goto L_0880BE14;
    case 914u: goto L_0880BE20;
    case 915u: goto L_0880BE28;
    case 916u: goto L_0880BE34;
    case 917u: goto L_0880BE3C;
    case 918u: goto L_0880BE48;
    case 919u: goto L_0880BE50;
    case 920u: goto L_0880BE58;
    case 921u: goto L_0880BE64;
    case 922u: goto L_0880BE6C;
    case 923u: goto L_0880BE7C;
    case 924u: goto L_0880BE8C;
    case 925u: goto L_0880BE98;
    case 926u: goto L_0880BEA0;
    case 927u: goto L_0880BEB0;
    case 928u: goto L_0880BEC0;
    case 929u: goto L_0880BEC8;
    case 930u: goto L_0880BED8;
    case 931u: goto L_0880BEE8;
    case 932u: goto L_0880BEF0;
    case 933u: goto L_0880BF00;
    case 934u: goto L_0880BF10;
    case 935u: goto L_0880BF18;
    case 936u: goto L_0880BF28;
    case 937u: goto L_0880BF2C;
    case 938u: goto L_0880BF44;
    case 939u: goto L_0880BF68;
    case 940u: goto L_0880BF74;
    case 941u: goto L_0880BF80;
    case 942u: goto L_0880BF88;
    case 943u: goto L_0880BF94;
    case 944u: goto L_0880BF9C;
    case 945u: goto L_0880BFA4;
    case 946u: goto L_0880BFB0;
    case 947u: goto L_0880BFBC;
    case 948u: goto L_0880BFC8;
    case 949u: goto L_0880BFD0;
    case 950u: goto L_0880BFD8;
    case 951u: goto L_0880BFE8;
    case 952u: goto L_0880BFF0;
    case 953u: goto L_0880BFFC;
    default:
        if (local_transfers == 0u) rt.unsupported(ctx.pc, 0u, "invalid internal function entry");
        else ctx.pc = local_pc;
        AOT_REGCACHE_SYNC_OUT(); return;
    }
    }
// PSPRECOMP_V813_SHARED_JR_DISPATCH: one dynamic-JR reject/redispatch path per unit.
LOCAL_JR_DISPATCH:
    {
        const std::uint32_t local_delta_v813 = jump_target - 0x08808000u;
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
L_08808000:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808010:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808038u);
    ctx.gpr[7] = (ctx.gpr[17] + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 3u, 0x08808038u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808038u) goto L_08808038;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808038:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808044u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808044u) goto L_08808044;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808044:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_4 = (aot_gpr_4 & 256u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880805C;
      }
      goto L_08808054;
    }
L_08808054:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 1u);
      if (branch_taken) {
          goto L_08808060;
      }
      goto L_0880805C;
    }
L_0880805C:
    aot_gpr_4 = (0u | 0u);
    goto L_08808060;
L_08808060:
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(524)));
    aot_gpr_4 = (aot_gpr_4 & 255u);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 ^ aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(518)));
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
      if (branch_taken) {
          goto L_08808084;
      }
      goto L_0880807C;
    }
L_0880807C:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_088080CC;
      }
      goto L_08808084;
    }
L_08808084:
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(518));
    aot_gpr_6 = (aot_mem.aot_direct_load16(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (aot_gpr_6 + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[7]));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(521)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_6) < 9 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
      if (branch_taken) {
          goto L_088080B0;
      }
      goto L_088080A4;
    }
L_088080A4:
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_088080CC;
      }
      goto L_088080B0;
    }
L_088080B0:
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(518)));
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_5) < 21 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_088080CC;
      }
      goto L_088080C8;
    }
L_088080C8:
    aot_mem.aot_direct_store16(aot_gpr_16 + static_cast<std::uint32_t>(518), static_cast<std::uint16_t>(0u));
    goto L_088080CC;
L_088080CC:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088080E4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08808104u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 15u, 0x08808104u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808104u) goto L_08808104;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808104:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808110u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808110u) goto L_08808110;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808110:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x0880812Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 17u, 0x0880812Cu, 0x08ACD4BCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 442u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 442u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 442u, 0x08ACD4BCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880812Cu) goto L_0880812C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880812C:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808140:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08808160u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 19u, 0x08808160u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808160u) goto L_08808160;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808160:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x0880816Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880816Cu) goto L_0880816C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880816C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808188u);
    aot_gpr_5 = (0u | 3u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 21u, 0x08808188u, 0x08ACD4BCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 442u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 442u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 442u, 0x08ACD4BCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808188u) goto L_08808188;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808188:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880819C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 4u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_31);
    aot_gpr_31 = (0x088081CCu);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 23u, 0x088081CCu, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088081CCu) goto L_088081CC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088081CC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088081D8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088081D8u) goto L_088081D8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088081D8:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(ctx.gpr[17] + static_cast<std::uint32_t>(4), aot_run_words);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_gpr_4 = (49864u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_12) || std::isnan(aot_fpr_13)) && aot_fpr_12 == aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_16 = (ctx.gpr[2] | 0u);
      if (branch_taken) {
          goto L_08808214;
      }
      goto L_088081FC;
    }
L_088081FC:
    aot_gpr_4 = (50042u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 <= aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_08808224;
      }
      goto L_08808214;
    }
L_08808214:
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_gpr_31 = (0x08808220u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 27u, 0x08808220u, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808220u) goto L_08808220;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808220:
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_08808224;
L_08808224:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_12)};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 4u);
    aot_gpr_31 = (0x08808250u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 29u, 0x08808250u, 0x08ACDF58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 661u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 661u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 661u, 0x08ACDF58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808250u) goto L_08808250;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808250:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808270:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 5u);
    { const std::uint32_t aot_run_words[4]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(ctx.fpr[24]), std::bit_cast<std::uint32_t>(ctx.fpr[26])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words); }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), aot_gpr_31);
    aot_gpr_31 = (0x088082A8u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 31u, 0x088082A8u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088082A8u) goto L_088082A8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088082A8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088082B4u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088082B4u) goto L_088082B4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088082B4:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(ctx.gpr[17] + static_cast<std::uint32_t>(4), aot_run_words);
      ctx.fpr[14] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[15] = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_fpr_22 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(16)));
      if (branch_taken) {
          goto L_088082E4;
      }
      goto L_088082DC;
    }
L_088082DC:
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    goto L_088082E4;
L_088082E4:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < ctx.fpr[15])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
        goto L_08808300;
    }
    goto L_088082F4;
L_088082F4:
    aot_fpr_22 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    goto L_08808300;
L_08808300:
    aot_gpr_4 = (16128u << 16u);
    ctx.fpr[26] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_fpr_20 = aot_fpr_12 + ctx.fpr[26];
    aot_fpr_22 = aot_fpr_13 + ctx.fpr[24];
    ctx.fpr[14] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_20; const float ft = ctx.fpr[14]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_20 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_20 = fs * ft; }
    { const float fs = aot_fpr_22; const float ft = ctx.fpr[14]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_22 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_22 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x08808328u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 37u, 0x08808328u, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808328u) goto L_08808328;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808328:
    aot_fpr_13 = aot_fpr_20 - ctx.fpr[26];
    ctx.fpr[14] = aot_fpr_22 - ctx.fpr[24];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[14] < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
      if (branch_taken) {
          goto L_08808348;
      }
      goto L_08808340;
    }
L_08808340:
    { const bool branch_taken = 0u == 0u;
    ctx.fpr[14] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_08808348;
      }
      goto L_08808348;
    }
L_08808348:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(aot_fpr_13)};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 4u);
    aot_gpr_31 = (0x0880837Cu);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 40u, 0x0880837Cu, 0x08ACE418u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 736u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 736u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 736u, 0x08ACE418u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880837Cu) goto L_0880837C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880837C:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      ctx.fpr[26] = std::bit_cast<float>(aot_run_words[3]);
      aot_gpr_16 = aot_run_words[4];
      ctx.gpr[17] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088083A4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x088083C4u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 42u, 0x088083C4u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088083C4u) goto L_088083C4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088083C4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088083D0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088083D0u) goto L_088083D0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088083D0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088083ECu);
    aot_gpr_5 = (0u | 6u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 44u, 0x088083ECu, 0x08ACD4BCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 442u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 442u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 442u, 0x08ACD4BCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088083ECu) goto L_088083EC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088083EC:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808400:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808428u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 46u, 0x08808428u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808428u) goto L_08808428;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808428:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808434u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808434u) goto L_08808434;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808434:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808444u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808444u) goto L_08808444;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808444:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 8u);
    aot_gpr_31 = (0x08808464u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 49u, 0x08808464u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808464u) goto L_08808464;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808464:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880847C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x088084A4u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 51u, 0x088084A4u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088084A4u) goto L_088084A4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088084A4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088084B0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088084B0u) goto L_088084B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088084B0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088084C0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088084C0u) goto L_088084C0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088084C0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 9u);
    aot_gpr_31 = (0x088084E0u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 54u, 0x088084E0u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088084E0u) goto L_088084E0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088084E0:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088084F8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808520u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 56u, 0x08808520u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808520u) goto L_08808520;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808520:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x0880852Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880852Cu) goto L_0880852C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880852C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x0880853Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880853Cu) goto L_0880853C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880853C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 11u);
    aot_gpr_31 = (0x0880855Cu);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 59u, 0x0880855Cu, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880855Cu) goto L_0880855C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880855C:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808574:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880859Cu);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 61u, 0x0880859Cu, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880859Cu) goto L_0880859C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880859C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088085A8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088085A8u) goto L_088085A8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088085A8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088085B8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088085B8u) goto L_088085B8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088085B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 12u);
    aot_gpr_31 = (0x088085D8u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 64u, 0x088085D8u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088085D8u) goto L_088085D8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088085D8:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088085F0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808618u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 66u, 0x08808618u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808618u) goto L_08808618;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808618:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808624u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808624u) goto L_08808624;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808624:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808634u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808634u) goto L_08808634;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808634:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 16u);
    aot_gpr_31 = (0x08808654u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 69u, 0x08808654u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808654u) goto L_08808654;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808654:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880866C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808694u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 71u, 0x08808694u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808694u) goto L_08808694;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808694:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088086A0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088086A0u) goto L_088086A0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088086A0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088086B0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088086B0u) goto L_088086B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088086B0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 17u);
    aot_gpr_31 = (0x088086D0u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 74u, 0x088086D0u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088086D0u) goto L_088086D0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088086D0:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088086E8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808710u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 76u, 0x08808710u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808710u) goto L_08808710;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808710:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x0880871Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880871Cu) goto L_0880871C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880871C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x0880872Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880872Cu) goto L_0880872C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880872C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 18u);
    aot_gpr_31 = (0x0880874Cu);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 79u, 0x0880874Cu, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880874Cu) goto L_0880874C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880874C:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808764:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880878Cu);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 81u, 0x0880878Cu, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880878Cu) goto L_0880878C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880878C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808798u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808798u) goto L_08808798;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808798:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15960)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088087A8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 445u, 0x08B65D28u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088087A8u) goto L_088087A8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088087A8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 21u);
    aot_gpr_31 = (0x088087C8u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 84u, 0x088087C8u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088087C8u) goto L_088087C8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088087C8:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088087E0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808808u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 86u, 0x08808808u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808808u) goto L_08808808;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808808:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808814u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808814u) goto L_08808814;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808814:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808824u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808824u) goto L_08808824;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808824:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 22u);
    aot_gpr_31 = (0x08808844u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 89u, 0x08808844u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808844u) goto L_08808844;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808844:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880885C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 5u);
    { const std::uint32_t aot_run_words[4]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(ctx.fpr[24]), std::bit_cast<std::uint32_t>(ctx.fpr[26])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words); }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), aot_gpr_31);
    aot_gpr_31 = (0x08808894u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 91u, 0x08808894u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808894u) goto L_08808894;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808894:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088088A0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088088A0u) goto L_088088A0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088088A0:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(ctx.gpr[17] + static_cast<std::uint32_t>(4), aot_run_words);
      ctx.fpr[14] = std::bit_cast<float>(aot_run_words[0]);
      ctx.fpr[15] = std::bit_cast<float>(aot_run_words[1]);
      aot_fpr_12 = std::bit_cast<float>(aot_run_words[2]);
    }
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_fpr_22 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(16)));
      if (branch_taken) {
          goto L_088088D0;
      }
      goto L_088088C8;
    }
L_088088C8:
    aot_fpr_20 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    goto L_088088D0;
L_088088D0:
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_13 < ctx.fpr[15])) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
        goto L_088088EC;
    }
    goto L_088088E0;
L_088088E0:
    aot_fpr_22 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[15]));
    ctx.fpr[24] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    goto L_088088EC;
L_088088EC:
    aot_gpr_4 = (16128u << 16u);
    ctx.fpr[26] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_fpr_20 = aot_fpr_12 + ctx.fpr[26];
    aot_fpr_22 = aot_fpr_13 + ctx.fpr[24];
    ctx.fpr[14] = std::bit_cast<float>(aot_gpr_4);
    { const float fs = aot_fpr_20; const float ft = ctx.fpr[14]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_20 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_20 = fs * ft; }
    { const float fs = aot_fpr_22; const float ft = ctx.fpr[14]; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_22 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_22 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x08808914u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 97u, 0x08808914u, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808914u) goto L_08808914;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808914:
    aot_fpr_13 = aot_fpr_20 - ctx.fpr[26];
    ctx.fpr[14] = aot_fpr_22 - ctx.fpr[24];
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((ctx.fpr[14] < aot_fpr_13)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
      if (branch_taken) {
          goto L_08808934;
      }
      goto L_0880892C;
    }
L_0880892C:
    { const bool branch_taken = 0u == 0u;
    ctx.fpr[14] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_08808934;
      }
      goto L_08808934;
    }
L_08808934:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(aot_fpr_13)};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 24u);
    aot_gpr_31 = (0x08808968u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 100u, 0x08808968u, 0x08ACE418u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 736u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 736u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 736u, 0x08ACE418u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808968u) goto L_08808968;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808968:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      ctx.fpr[24] = std::bit_cast<float>(aot_run_words[2]);
      ctx.fpr[26] = std::bit_cast<float>(aot_run_words[3]);
      aot_gpr_16 = aot_run_words[4];
      ctx.gpr[17] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808990:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 3u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_31);
    aot_gpr_31 = (0x088089C0u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 102u, 0x088089C0u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088089C0u) goto L_088089C0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088089C0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088089CCu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088089CCu) goto L_088089CC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088089CC:
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_fpr_22 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x088089E4u);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 104u, 0x088089E4u, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088089E4u) goto L_088089E4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088089E4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(ctx.fpr[0])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 24u);
    aot_gpr_31 = (0x08808A10u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 105u, 0x08808A10u, 0x08ACDF58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 661u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 661u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 661u, 0x08ACDF58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808A10u) goto L_08808A10;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808A10:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808A30:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 3u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808A58u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 107u, 0x08808A58u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808A58u) goto L_08808A58;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808A58:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808A64u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808A64u) goto L_08808A64;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808A64:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808A74u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808A74u) goto L_08808A74;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808A74:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 15u);
    aot_gpr_31 = (0x08808A94u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 110u, 0x08808A94u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808A94u) goto L_08808A94;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808A94:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_31 = (0x08808AA0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0176.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 111u, 0x08808AA0u, 0x08AC5410u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0176_entry(rt, ctx, 391u, aot_mem);
#else
        recomp_unit_0176_entry(rt, ctx, 391u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0176_entry, 176u, 391u, 0x08AC5410u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808AA0u) goto L_08808AA0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808AA0:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808AB8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08808AD8u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 113u, 0x08808AD8u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808AD8u) goto L_08808AD8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808AD8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808AE4u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808AE4u) goto L_08808AE4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808AE4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808B00u);
    aot_gpr_5 = (0u | 35u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 115u, 0x08808B00u, 0x08ACD4BCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 442u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 442u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 442u, 0x08ACD4BCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808B00u) goto L_08808B00;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808B00:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808B14:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08808B34u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 117u, 0x08808B34u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808B34u) goto L_08808B34;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808B34:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808B40u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808B40u) goto L_08808B40;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808B40:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(1152)));
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808B60u);
    aot_gpr_5 = (0u | 16u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 119u, 0x08808B60u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808B60u) goto L_08808B60;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808B60:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808B74:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808B9Cu);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 121u, 0x08808B9Cu, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808B9Cu) goto L_08808B9C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808B9C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808BA8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808BA8u) goto L_08808BA8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808BA8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808BB8u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808BB8u) goto L_08808BB8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808BB8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 41u);
    aot_gpr_31 = (0x08808BD8u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 124u, 0x08808BD8u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808BD8u) goto L_08808BD8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808BD8:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808BF0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808C18u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 126u, 0x08808C18u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808C18u) goto L_08808C18;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808C18:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808C24u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808C24u) goto L_08808C24;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808C24:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808C34u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808C34u) goto L_08808C34;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808C34:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 13u);
    aot_gpr_31 = (0x08808C54u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 129u, 0x08808C54u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808C54u) goto L_08808C54;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808C54:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808C6C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808C94u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 131u, 0x08808C94u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808C94u) goto L_08808C94;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808C94:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808CA0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808CA0u) goto L_08808CA0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808CA0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808CB0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808CB0u) goto L_08808CB0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808CB0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 46u);
    aot_gpr_31 = (0x08808CD0u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 134u, 0x08808CD0u, 0x08ACD698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 482u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 482u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 482u, 0x08ACD698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808CD0u) goto L_08808CD0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808CD0:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808CE8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 3u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_31);
    aot_gpr_31 = (0x08808D18u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 136u, 0x08808D18u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808D18u) goto L_08808D18;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808D18:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808D24u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808D24u) goto L_08808D24;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808D24:
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_fpr_22 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x08808D3Cu);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 138u, 0x08808D3Cu, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808D3Cu) goto L_08808D3C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808D3C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(ctx.fpr[0])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 49u);
    aot_gpr_31 = (0x08808D68u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 139u, 0x08808D68u, 0x08ACDF58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 661u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 661u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 661u, 0x08ACDF58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808D68u) goto L_08808D68;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808D68:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808D88:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 3u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808DB0u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 141u, 0x08808DB0u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808DB0u) goto L_08808DB0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808DB0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808DBCu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808DBCu) goto L_08808DBC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808DBC:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_6 = (aot_gpr_4 << 16u);
    ctx.gpr[7] = (aot_gpr_5 << 16u);
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(aot_gpr_6) >> 16u));
    ctx.gpr[7] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[7]) >> 16u));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808DF0u);
    aot_gpr_5 = (0u | 30u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0176.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 143u, 0x08808DF0u, 0x08AC7A58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0176_entry(rt, ctx, 983u, aot_mem);
#else
        recomp_unit_0176_entry(rt, ctx, 983u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0176_entry, 176u, 983u, 0x08AC7A58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808DF0u) goto L_08808DF0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808DF0:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808E08:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 3u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_31);
    aot_gpr_31 = (0x08808E38u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 145u, 0x08808E38u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808E38u) goto L_08808E38;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808E38:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808E44u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808E44u) goto L_08808E44;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808E44:
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    aot_fpr_22 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_gpr_31 = (0x08808E5Cu);
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_22));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0035.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 147u, 0x08808E5Cu, 0x088933E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0035_entry(rt, ctx, 514u, aot_mem);
#else
        recomp_unit_0035_entry(rt, ctx, 514u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0035_entry, 35u, 514u, 0x088933E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808E5Cu) goto L_08808E5C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808E5C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(460), aot_gpr_4);
    { const std::uint32_t aot_run_words[3]{std::bit_cast<std::uint32_t>(aot_fpr_20), std::bit_cast<std::uint32_t>(aot_fpr_22), std::bit_cast<std::uint32_t>(ctx.fpr[0])};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 25u);
    aot_gpr_31 = (0x08808E88u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 148u, 0x08808E88u, 0x08ACDF58u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 661u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 661u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 661u, 0x08ACDF58u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808E88u) goto L_08808E88;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808E88:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808EA8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08808EC8u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 150u, 0x08808EC8u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808EC8u) goto L_08808EC8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808EC8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808ED4u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808ED4u) goto L_08808ED4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808ED4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[2] + static_cast<std::uint32_t>(460)));
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-257));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[2] + static_cast<std::uint32_t>(460), aot_gpr_4);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08808EF0u);
    aot_gpr_5 = (0u | 32u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0178.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 152u, 0x08808EF0u, 0x08ACD4BCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0178_entry(rt, ctx, 442u, aot_mem);
#else
        recomp_unit_0178_entry(rt, ctx, 442u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0178_entry, 178u, 442u, 0x08ACD4BCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808EF0u) goto L_08808EF0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808EF0:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808F04:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    ctx.gpr[17] = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x08808F2Cu);
    ctx.gpr[7] = (ctx.gpr[17] + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 154u, 0x08808F2Cu, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808F2Cu) goto L_08808F2C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08808F2C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08808F38u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08808F38u) goto L_08808F38;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08808F38:
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(640)));
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08808F58;
      }
      goto L_08808F48;
    }
L_08808F48:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(2196)));
    { const bool branch_taken = aot_gpr_5 != 0u;
      if (branch_taken) {
          goto L_08808F58;
      }
      goto L_08808F54;
    }
L_08808F54:
    aot_gpr_4 = (0u | 1u);
    goto L_08808F58;
L_08808F58:
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(524)));
    aot_gpr_4 = (aot_gpr_4 & 255u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 ^ aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(518)));
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
      if (branch_taken) {
          goto L_08808F84;
      }
      goto L_08808F7C;
    }
L_08808F7C:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_08808FCC;
      }
      goto L_08808F84;
    }
L_08808F84:
    aot_gpr_5 = (aot_gpr_16 + static_cast<std::uint32_t>(518));
    aot_gpr_6 = (aot_mem.aot_direct_load16(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (aot_gpr_6 + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store16(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint16_t>(ctx.gpr[7]));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(521)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_6) < 9 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
      if (branch_taken) {
          goto L_08808FB0;
      }
      goto L_08808FA4;
    }
L_08808FA4:
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
      if (branch_taken) {
          goto L_08808FCC;
      }
      goto L_08808FB0;
    }
L_08808FB0:
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load16(aot_gpr_16 + static_cast<std::uint32_t>(518)));
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(521), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_5) < 21 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08808FCC;
      }
      goto L_08808FC8;
    }
L_08808FC8:
    aot_mem.aot_direct_store16(aot_gpr_16 + static_cast<std::uint32_t>(518), static_cast<std::uint16_t>(0u));
    goto L_08808FCC;
L_08808FCC:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08808FE4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_4 + static_cast<std::uint32_t>(16));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08809004u);
    ctx.gpr[7] = (aot_gpr_16 + static_cast<std::uint32_t>(29704));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 166u, 0x08809004u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809004u) goto L_08809004;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809004:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x08809010u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809010u) goto L_08809010;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809010:
    aot_gpr_16 = (ctx.gpr[2] | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(2228)));
    aot_gpr_5 = (0u | 11u);
    { const bool branch_taken = aot_gpr_4 != aot_gpr_5;
      if (branch_taken) {
          goto L_08809034;
      }
      goto L_08809024;
    }
L_08809024:
    aot_gpr_31 = (0x0880902Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0129.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 169u, 0x0880902Cu, 0x08A08AE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0129_entry(rt, ctx, 78u, aot_mem);
#else
        recomp_unit_0129_entry(rt, ctx, 78u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0129_entry, 129u, 78u, 0x08A08AE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880902Cu) goto L_0880902C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880902C:
    aot_gpr_31 = (0x08809034u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0065.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 170u, 0x08809034u, 0x0890B488u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0065_entry(rt, ctx, 829u, aot_mem);
#else
        recomp_unit_0065_entry(rt, ctx, 829u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0065_entry, 65u, 829u, 0x0890B488u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809034u) goto L_08809034;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809034:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809048:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-96));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), ctx.gpr[21]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), ctx.gpr[22]);
    ctx.gpr[21] = (2236u << 16u);
    ctx.gpr[22] = (aot_gpr_16 + static_cast<std::uint32_t>(16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), ctx.gpr[17]);
    ctx.gpr[17] = (ctx.gpr[21] + static_cast<std::uint32_t>(29704));
    aot_gpr_5 = (ctx.gpr[22] | 0u);
    aot_gpr_6 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), std::bit_cast<std::uint32_t>(aot_fpr_20));
    { const std::uint32_t aot_run_words[3]{ctx.gpr[18], ctx.gpr[19], ctx.gpr[20]};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(64), aot_run_words); }
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), ctx.gpr[23]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(88), aot_gpr_31);
    aot_gpr_31 = (0x08809094u);
    ctx.gpr[7] = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0024.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 172u, 0x08809094u, 0x0886589Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0024_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0024_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0024_entry, 24u, 371u, 0x0886589Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809094u) goto L_08809094;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809094:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15976)));
    aot_gpr_31 = (0x088090A0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[21] + static_cast<std::uint32_t>(29704)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 431u, 0x08B65C68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088090A0u) goto L_088090A0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088090A0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-15972)));
    ctx.gpr[20] = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x088090B0u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 438u, 0x08B65CC4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088090B0u) goto L_088090B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088090B0:
    aot_mem.aot_direct_store32(ctx.gpr[21] + static_cast<std::uint32_t>(29704), 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(2196)));
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_088090C0;
    }
L_088090C0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[20] + static_cast<std::uint32_t>(464)));
    aot_gpr_5 = (1u << 16u);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_5);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_088090D4;
    }
L_088090D4:
    aot_gpr_4 = (17352u << 16u);
    aot_gpr_31 = (0x088090E0u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 177u, 0x088090E0u, 0x08A01528u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 371u, 0x08A01528u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088090E0u) goto L_088090E0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088090E0:
    ctx.gpr[18] = (ctx.gpr[20] + static_cast<std::uint32_t>(48));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x088090F4u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 178u, 0x088090F4u, 0x08A03B88u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 974u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 974u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 974u, 0x08A03B88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088090F4u) goto L_088090F4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088090F4:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_08809100;
    }
L_08809100:
    ctx.gpr[23] = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x08809114u);
    aot_gpr_6 = (ctx.gpr[23] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 180u, 0x08809114u, 0x08A036C4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 914u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 914u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 914u, 0x08A036C4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809114u) goto L_08809114;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809114:
    { const std::uint32_t vfpu_address = ctx.gpr[18] + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    { const std::uint32_t vfpu_address = ctx.gpr[23] + static_cast<std::uint32_t>(0);
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
      const std::uint32_t vfpu_address = aot_gpr_29 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    { const std::uint32_t vfpu_address = aot_gpr_29 + static_cast<std::uint32_t>(0);
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
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_08809144;
    }
L_08809144:
    aot_gpr_31 = (0x0880914Cu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 182u, 0x0880914Cu, 0x08A01528u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 371u, 0x08A01528u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880914Cu) goto L_0880914C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880914C:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x08809158u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 183u, 0x08809158u, 0x08A03590u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 886u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 886u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 886u, 0x08A03590u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809158u) goto L_08809158;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809158:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_08809160;
    }
L_08809160:
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_6 = (0u | 0u);
    aot_gpr_31 = (0x08809174u);
    ctx.gpr[7] = (ctx.gpr[20] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 185u, 0x08809174u, 0x08A03794u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 918u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 918u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 918u, 0x08A03794u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809174u) goto L_08809174;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809174:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_0880917C;
    }
L_0880917C:
    aot_gpr_31 = (0x08809184u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 187u, 0x08809184u, 0x08A01528u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 371u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 371u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 371u, 0x08A01528u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809184u) goto L_08809184;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809184:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08809198u);
    ctx.gpr[7] = (ctx.gpr[19] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0127.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 188u, 0x08809198u, 0x08A02D68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0127_entry(rt, ctx, 720u, aot_mem);
#else
        recomp_unit_0127_entry(rt, ctx, 720u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0127_entry, 127u, 720u, 0x08A02D68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809198u) goto L_08809198;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809198:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_088091A8;
      }
      goto L_088091A0;
    }
L_088091A0:
    aot_gpr_4 = (0u | 1u);
    aot_mem.aot_direct_store32(ctx.gpr[21] + static_cast<std::uint32_t>(29704), aot_gpr_4);
    goto L_088091A8;
L_088091A8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[22] | 0u);
    aot_gpr_31 = (0x088091B8u);
    aot_gpr_6 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08862890, 23u, 431u, 0x08862890u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], ctx.gpr[11], aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[28], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0023_entry, 23u, 431u, 0x08862890u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088091B8u) goto L_088091B8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_088091B8:
    ctx.gpr[2] = (0u | 0u);
    { std::uint32_t aot_run_words[10]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(52), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_gpr_16 = aot_run_words[1];
      ctx.gpr[17] = aot_run_words[2];
      ctx.gpr[18] = aot_run_words[3];
      ctx.gpr[19] = aot_run_words[4];
      ctx.gpr[20] = aot_run_words[5];
      ctx.gpr[21] = aot_run_words[6];
      ctx.gpr[22] = aot_run_words[7];
      ctx.gpr[23] = aot_run_words[8];
      aot_gpr_31 = aot_run_words[9];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(96));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088091EC:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(5920), static_cast<std::uint8_t>(0u));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088091F4:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_08809210;
      }
      goto L_088091FC;
    }
L_088091FC:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    if (aot_gpr_6 != 0u) {
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
        goto L_08809218;
    }
    goto L_08809210;
L_08809210:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_0880924C;
      }
      goto L_08809218;
    }
L_08809218:
    aot_gpr_6 = (aot_gpr_6 + aot_gpr_4);
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (aot_gpr_6 & 128u);
    if (aot_gpr_6 == 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
        goto L_08809234;
    }
    goto L_0880922C;
L_0880922C:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_0880924C;
      }
      goto L_08809234;
    }
L_08809234:
    aot_gpr_6 = (aot_gpr_4 << 6u);
    ctx.gpr[7] = (aot_gpr_4 << 3u);
    aot_gpr_6 = (aot_gpr_6 + ctx.gpr[7]);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    goto L_0880924C;
L_0880924C:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880927C;
      }
      goto L_08809254;
    }
L_08809254:
    aot_gpr_5 = (static_cast<std::int32_t>(aot_gpr_4) < 24 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08809268;
      }
      goto L_08809260;
    }
L_08809260:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_08809280;
      }
      goto L_08809268;
    }
L_08809268:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7652)));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23));
    ctx.gpr[2] = (aot_gpr_4 ^ aot_gpr_5);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
      if (branch_taken) {
          goto L_08809280;
      }
      goto L_0880927C;
    }
L_0880927C:
    ctx.gpr[2] = (0u | 0u);
    goto L_08809280;
L_08809280:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809288:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), ctx.gpr[19]);
    ctx.gpr[19] = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (aot_gpr_5 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), ctx.gpr[18]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), ctx.gpr[20]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_gpr_31);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
    ctx.gpr[18] = (aot_gpr_6 | 0u);
      if (branch_taken) {
          goto L_088092CC;
      }
      goto L_088092B8;
    }
L_088092B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(aot_gpr_16) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
        goto L_088092D4;
    }
    goto L_088092CC;
L_088092CC:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809300;
      }
      goto L_088092D4;
    }
L_088092D4:
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_16);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_gpr_5 & 128u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
        goto L_088092F0;
    }
    goto L_088092E8;
L_088092E8:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809300;
      }
      goto L_088092F0;
    }
L_088092F0:
    aot_gpr_5 = (aot_gpr_16 << 6u);
    aot_gpr_6 = (aot_gpr_16 << 3u);
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    goto L_08809300;
L_08809300:
    ctx.gpr[20] = (ctx.gpr[17] | 0u);
    ctx.gpr[17] = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(68), ctx.gpr[18]);
    aot_gpr_4 = (ctx.gpr[17] + static_cast<std::uint32_t>(68));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880931Cu);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0030.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 212u, 0x0880931Cu, 0x0887F5E4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0030_entry(rt, ctx, 728u, aot_mem);
#else
        recomp_unit_0030_entry(rt, ctx, 728u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0030_entry, 30u, 728u, 0x0887F5E4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880931Cu) goto L_0880931C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880931C:
    ctx.gpr[18] = (ctx.gpr[20] | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[19];
    aot_gpr_16 = (aot_gpr_16 & 255u);
      if (branch_taken) {
          goto L_08809384;
      }
      goto L_0880932C;
    }
L_0880932C:
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7656)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_0880934C;
      }
      goto L_0880933C;
    }
L_0880933C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    goto L_0880934C;
L_0880934C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (0u | 1u);
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(55), static_cast<std::uint8_t>(aot_gpr_16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(4)));
    aot_mem.aot_direct_store8(aot_gpr_5 + static_cast<std::uint32_t>(18), static_cast<std::uint8_t>(aot_gpr_6));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(20), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 + static_cast<std::uint32_t>(20));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x08809374u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0030.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 216u, 0x08809374u, 0x0887F5E4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0030_entry(rt, ctx, 728u, aot_mem);
#else
        recomp_unit_0030_entry(rt, ctx, 728u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0030_entry, 30u, 728u, 0x0887F5E4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809374u) goto L_08809374;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809374:
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(8));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[18] + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[19];
      if (branch_taken) {
          goto L_0880932C;
      }
      goto L_08809384;
    }
L_08809384:
    aot_gpr_4 = (0u | 1u);
    aot_mem.aot_direct_store8(ctx.gpr[17] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_4));
    { std::uint32_t aot_run_words[6]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      aot_gpr_31 = aot_run_words[5];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088093AC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    { const std::uint32_t aot_run_words[6]{aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
    aot_gpr_16 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_088093E4;
      }
      goto L_088093D0;
    }
L_088093D0:
    ctx.gpr[17] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_16) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    if (aot_gpr_4 != 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(4)));
        goto L_088093EC;
    }
    goto L_088093E4;
L_088093E4:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[17] = (0u | 0u);
      if (branch_taken) {
          goto L_08809418;
      }
      goto L_088093EC;
    }
L_088093EC:
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_16);
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_gpr_4 & 128u);
    if (aot_gpr_4 == 0u) {
    ctx.gpr[17] = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(0)));
        goto L_08809408;
    }
    goto L_08809400;
L_08809400:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[17] = (0u | 0u);
      if (branch_taken) {
          goto L_08809418;
      }
      goto L_08809408;
    }
L_08809408:
    aot_gpr_4 = (aot_gpr_16 << 6u);
    aot_gpr_5 = (aot_gpr_16 << 3u);
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    ctx.gpr[17] = (ctx.gpr[17] + aot_gpr_4);
    goto L_08809418;
L_08809418:
    aot_mem.aot_direct_store8(ctx.gpr[17] + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(0u));
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[19] = (0u | 0u);
    goto L_08809424;
L_08809424:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7656)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809444;
      }
      goto L_08809434;
    }
L_08809434:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 + ctx.gpr[19]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    goto L_08809444;
L_08809444:
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08809498;
      }
      goto L_0880944C;
    }
L_0880944C:
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809460;
      }
      goto L_08809454;
    }
L_08809454:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 + ctx.gpr[19]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    goto L_08809460;
L_08809460:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08809498;
      }
      goto L_08809474;
    }
L_08809474:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_4 = (aot_gpr_4 + ctx.gpr[19]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(55)));
    { const bool branch_taken = aot_gpr_5 != aot_gpr_16;
      if (branch_taken) {
          goto L_08809498;
      }
      goto L_08809490;
    }
L_08809490:
    aot_gpr_31 = (0x08809498u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0099.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 233u, 0x08809498u, 0x089904B0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0099_entry(rt, ctx, 75u, aot_mem);
#else
        recomp_unit_0099_entry(rt, ctx, 75u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0099_entry, 99u, 75u, 0x089904B0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809498u) goto L_08809498;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809498:
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < 9000 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08809424;
      }
      goto L_088094A8;
    }
L_088094A8:
    ctx.gpr[20] = (2237u << 16u);
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[19] = (0u | 0u);
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(-28816));
    goto L_088094B8;
L_088094B8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7656)));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_4) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_088094D8;
      }
      goto L_088094C8;
    }
L_088094C8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[19]);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    goto L_088094D8;
L_088094D8:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880954C;
      }
      goto L_088094E0;
    }
L_088094E0:
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_088094F4;
      }
      goto L_088094E8;
    }
L_088094E8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[19]);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    goto L_088094F4;
L_088094F4:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(20)));
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880954C;
      }
      goto L_08809508;
    }
L_08809508:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(24)));
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[19]);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(20)));
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(55)));
    { const bool branch_taken = aot_gpr_6 != aot_gpr_16;
      if (branch_taken) {
          goto L_0880954C;
      }
      goto L_08809524;
    }
L_08809524:
    aot_gpr_6 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (0u | 0u);
    if (aot_gpr_6 != 0u) {
    aot_gpr_4 = (aot_gpr_5 | 0u);
        goto L_08809534;
    }
    goto L_08809534;
L_08809534:
    aot_gpr_5 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(20), ctx.gpr[20]);
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(18), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(20));
    aot_gpr_31 = (0x0880954Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0030.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 244u, 0x0880954Cu, 0x0887F6B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0030_entry(rt, ctx, 736u, aot_mem);
#else
        recomp_unit_0030_entry(rt, ctx, 736u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0030_entry, 30u, 736u, 0x0887F6B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880954Cu) goto L_0880954C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880954C:
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < 9000 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_088094B8;
      }
      goto L_0880955C;
    }
L_0880955C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(68)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08809588;
      }
      goto L_08809568;
    }
L_08809568:
    aot_gpr_4 = (ctx.gpr[17] + static_cast<std::uint32_t>(68));
    aot_gpr_31 = (0x08809574u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0030.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 247u, 0x08809574u, 0x0887F6B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0030_entry(rt, ctx, 736u, aot_mem);
#else
        recomp_unit_0030_entry(rt, ctx, 736u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0030_entry, 30u, 736u, 0x0887F6B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809574u) goto L_08809574;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809574:
    aot_gpr_4 = (2236u << 16u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(68)));
    aot_gpr_31 = (0x08809584u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(31984));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08ABE8F4, 174u, 556u, 0x08ABE8F4u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0174_entry, 174u, 556u, 0x08ABE8F4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809584u) goto L_08809584;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809584:
    aot_mem.aot_direct_store32(ctx.gpr[17] + static_cast<std::uint32_t>(68), 0u);
    goto L_08809588;
L_08809588:
    { std::uint32_t aot_run_words[6]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      aot_gpr_31 = aot_run_words[5];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088095A8:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) < 0;
      if (branch_taken) {
          goto L_088095C4;
      }
      goto L_088095B0;
    }
L_088095B0:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    aot_gpr_6 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    if (aot_gpr_6 != 0u) {
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(4)));
        goto L_088095CC;
    }
    goto L_088095C4;
L_088095C4:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_088095F8;
      }
      goto L_088095CC;
    }
L_088095CC:
    aot_gpr_6 = (aot_gpr_6 + aot_gpr_4);
    aot_gpr_6 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (aot_gpr_6 & 128u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    aot_gpr_6 = (aot_gpr_4 << 6u);
      if (branch_taken) {
          goto L_088095E8;
      }
      goto L_088095E0;
    }
L_088095E0:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_088095F8;
      }
      goto L_088095E8;
    }
L_088095E8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_gpr_4 << 3u);
    aot_gpr_4 = (aot_gpr_6 + aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    goto L_088095F8;
L_088095F8:
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (aot_gpr_4 + static_cast<std::uint32_t>(8));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809600:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), std::bit_cast<std::uint32_t>(aot_fpr_22));
    { const std::uint32_t aot_run_words[4]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    aot_gpr_31 = (0x0880962Cu);
    ctx.gpr[17] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 258u, 0x0880962Cu, 0x08AD3220u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 542u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 542u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 542u, 0x08AD3220u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880962Cu) goto L_0880962C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880962C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809648;
      }
      goto L_08809634;
    }
L_08809634:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-7080)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08809648;
      }
      goto L_08809640;
    }
L_08809640:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880984C;
      }
      goto L_08809648;
    }
L_08809648:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    ctx.gpr[18] = (0u | 1u);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_6 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 == 0u;
    ctx.gpr[19] = (0u | 72u);
      if (branch_taken) {
          goto L_088096EC;
      }
      goto L_08809660;
    }
L_08809660:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[18]) < 0;
      if (branch_taken) {
          goto L_08809678;
      }
      goto L_08809668;
    }
L_08809668:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
        goto L_08809680;
    }
    goto L_08809678;
L_08809678:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_088096A8;
      }
      goto L_08809680;
    }
L_08809680:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[18]);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_gpr_5 & 128u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
        goto L_0880969C;
    }
    goto L_08809694;
L_08809694:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_088096A8;
      }
      goto L_0880969C;
    }
L_0880969C:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[19]);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    goto L_088096A8;
L_088096A8:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_088096D8;
      }
      goto L_088096B0;
    }
L_088096B0:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x088096BCu);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    goto L_088091F4;
L_088096BC:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_088096D4;
      }
      goto L_088096C4;
    }
L_088096C4:
    aot_gpr_31 = (0x088096CCu);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 272u, 0x088096CCu, 0x08AD34D8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 577u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 577u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 577u, 0x08AD34D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088096CCu) goto L_088096CC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088096CC:
    aot_gpr_31 = (0x088096D4u);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 273u, 0x088096D4u, 0x08AD3780u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 621u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 621u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 621u, 0x08AD3780u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x088096D4u) goto L_088096D4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_088096D4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    goto L_088096D8;
L_088096D8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_6 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(72));
      if (branch_taken) {
          goto L_08809660;
      }
      goto L_088096EC;
    }
L_088096EC:
    ctx.gpr[18] = (0u | 1u);
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08809824;
      }
      goto L_088096FC;
    }
L_088096FC:
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_fpr_22 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    ctx.gpr[19] = (0u | 72u);
    goto L_08809708;
L_08809708:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[18]) < 0;
      if (branch_taken) {
          goto L_08809720;
      }
      goto L_08809710;
    }
L_08809710:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
        goto L_08809728;
    }
    goto L_08809720;
L_08809720:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_08809750;
      }
      goto L_08809728;
    }
L_08809728:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[18]);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_gpr_5 & 128u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
        goto L_08809744;
    }
    goto L_0880973C;
L_0880973C:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (0u | 0u);
      if (branch_taken) {
          goto L_08809750;
      }
      goto L_08809744;
    }
L_08809744:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[19]);
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    goto L_08809750;
L_08809750:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08809808;
      }
      goto L_08809758;
    }
L_08809758:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08809764u);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    goto L_088091F4;
L_08809764:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809804;
      }
      goto L_0880976C;
    }
L_0880976C:
    aot_gpr_31 = (0x08809774u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 287u, 0x08809774u, 0x08AD34D8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 577u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 577u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 577u, 0x08AD34D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809774u) goto L_08809774;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809774:
    aot_gpr_31 = (0x0880977Cu);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 288u, 0x0880977Cu, 0x08AD3698u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 607u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 607u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 607u, 0x08AD3698u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880977Cu) goto L_0880977C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880977C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_088097F8;
      }
      goto L_08809784;
    }
L_08809784:
    aot_gpr_31 = (0x0880978Cu);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    goto L_088095A8;
L_0880978C:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_fpr_20 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(0)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_fpr_22 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(4)));
      if (branch_taken) {
          goto L_088097E4;
      }
      goto L_088097A8;
    }
L_088097A8:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_20 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_088097E4;
      }
      goto L_088097BC;
    }
L_088097BC:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(12)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_22 < aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_088097E4;
      }
      goto L_088097D0;
    }
L_088097D0:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_22 <= aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = ((ctx.fcr31 & 0x00800000u) != 0u);
    aot_gpr_4 = (0u | 1u);
      if (branch_taken) {
          goto L_088097EC;
      }
      goto L_088097E4;
    }
L_088097E4:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_088097EC;
      }
      goto L_088097EC;
    }
L_088097EC:
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_088097F8;
      }
      goto L_088097F4;
    }
L_088097F4:
    ctx.gpr[17] = (0u | 1u);
    goto L_088097F8;
L_088097F8:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08809804u);
    aot_gpr_5 = (0u | 33u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 298u, 0x08809804u, 0x08AD33C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 562u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 562u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 562u, 0x08AD33C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809804u) goto L_08809804;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809804:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    goto L_08809808;
L_08809808:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(72));
      if (branch_taken) {
          goto L_08809708;
      }
      goto L_0880981C;
    }
L_0880981C:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_22));
    goto L_08809824;
L_08809824:
    { const bool branch_taken = ctx.gpr[17] == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(5920), static_cast<std::uint8_t>(0u));
      if (branch_taken) {
          goto L_0880984C;
      }
      goto L_0880982C;
    }
L_0880982C:
    aot_gpr_31 = (0x08809834u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0131.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 303u, 0x08809834u, 0x08A114E4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0131_entry(rt, ctx, 163u, aot_mem);
#else
        recomp_unit_0131_entry(rt, ctx, 163u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0131_entry, 131u, 163u, 0x08A114E4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809834u) goto L_08809834;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809834:
    aot_gpr_31 = (0x0880983Cu);
    aot_gpr_4 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 304u, 0x0880983Cu, 0x08AD3610u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 598u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 598u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 598u, 0x08AD3610u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880983Cu) goto L_0880983C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880983C:
    aot_gpr_31 = (0x08809844u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0090.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 305u, 0x08809844u, 0x0896D084u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0090_entry(rt, ctx, 286u, aot_mem);
#else
        recomp_unit_0090_entry(rt, ctx, 286u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0090_entry, 90u, 286u, 0x0896D084u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809844u) goto L_08809844;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809844:
    aot_gpr_31 = (0x0880984Cu);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0131.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 306u, 0x0880984Cu, 0x08A11504u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0131_entry(rt, ctx, 165u, aot_mem);
#else
        recomp_unit_0131_entry(rt, ctx, 165u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0131_entry, 131u, 165u, 0x08A11504u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880984Cu) goto L_0880984C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880984C:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
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
L_08809870:
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(5920), static_cast<std::uint8_t>(aot_gpr_5));
    { const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      std::uint32_t vfpu_words[4]{};
      aot_mem.aot_direct_load32_block(vfpu_address, vfpu_words);
      float vfpu_value[4]{
        std::bit_cast<float>(vfpu_words[0]),
        std::bit_cast<float>(vfpu_words[1]),
        std::bit_cast<float>(vfpu_words[2]),
        std::bit_cast<float>(vfpu_words[3])};
      ctx.write_vfpu_vector_ct<0u, 4u>(vfpu_value); }
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(5936));
    { float vfpu_value[4]{}; ctx.read_vfpu_vector_ct<0u, 4u>(vfpu_value);
      const std::uint32_t vfpu_address = aot_gpr_4 + static_cast<std::uint32_t>(0);
      const std::uint32_t vfpu_words[4]{std::bit_cast<std::uint32_t>(vfpu_value[0]), std::bit_cast<std::uint32_t>(vfpu_value[1]), std::bit_cast<std::uint32_t>(vfpu_value[2]), std::bit_cast<std::uint32_t>(vfpu_value[3])};
      aot_mem.aot_direct_store32_block(vfpu_address, vfpu_words); }
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880988C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    { const std::uint32_t aot_run_words[3]{ctx.gpr[17], ctx.gpr[18], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.gpr[18] = (0u | 1u);
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
    ctx.gpr[17] = (0u | 72u);
      if (branch_taken) {
          goto L_08809940;
      }
      goto L_088098BC;
    }
L_088098BC:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[18]) < 0;
      if (branch_taken) {
          goto L_088098D4;
      }
      goto L_088098C4;
    }
L_088098C4:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
        goto L_088098DC;
    }
    goto L_088098D4;
L_088098D4:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809904;
      }
      goto L_088098DC;
    }
L_088098DC:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[18]);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_gpr_5 & 128u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
        goto L_088098F8;
    }
    goto L_088098F0;
L_088098F0:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809904;
      }
      goto L_088098F8;
    }
L_088098F8:
    aot_gpr_4 = (aot_gpr_4 + ctx.gpr[17]);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    goto L_08809904;
L_08809904:
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (ctx.gpr[18] | 0u);
      if (branch_taken) {
          goto L_08809928;
      }
      goto L_0880990C;
    }
L_0880990C:
    aot_gpr_31 = (0x08809914u);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    goto L_088091F4;
L_08809914:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809928;
      }
      goto L_0880991C;
    }
L_0880991C:
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x08809928u);
    aot_gpr_5 = (0u | 9u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 319u, 0x08809928u, 0x08AD33C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 562u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 562u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 562u, 0x08AD33C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809928u) goto L_08809928;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809928:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(72));
      if (branch_taken) {
          goto L_088098BC;
      }
      goto L_08809940;
    }
L_08809940:
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809958:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x0880996Cu);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-5336)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0162.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 322u, 0x0880996Cu, 0x08A8E094u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0162_entry(rt, ctx, 404u, aot_mem);
#else
        recomp_unit_0162_entry(rt, ctx, 404u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0162_entry, 162u, 404u, 0x08A8E094u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880996Cu) goto L_0880996C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880996C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7652)));
    ctx.gpr[2] = (ctx.gpr[2] ^ aot_gpr_4);
    ctx.gpr[2] = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809984:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08809994u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(48));
    goto L_08809958;
L_08809994:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088099A0:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_088099A8;
      }
      goto L_088099A8;
    }
L_088099A8:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_088099B0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    { const std::uint32_t aot_run_words[3]{aot_gpr_16, ctx.gpr[17], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.gpr[17] = (0u | 1u);
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[17]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_gpr_16 = (0u | 72u);
      if (branch_taken) {
          goto L_08809A64;
      }
      goto L_088099D8;
    }
L_088099D8:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[17]) < 0;
      if (branch_taken) {
          goto L_088099F0;
      }
      goto L_088099E0;
    }
L_088099E0:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[17]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    if (aot_gpr_5 != 0u) {
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
        goto L_088099F8;
    }
    goto L_088099F0;
L_088099F0:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809A20;
      }
      goto L_088099F8;
    }
L_088099F8:
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[17]);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_5 = (aot_gpr_5 & 128u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
        goto L_08809A14;
    }
    goto L_08809A0C;
L_08809A0C:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (0u | 0u);
      if (branch_taken) {
          goto L_08809A20;
      }
      goto L_08809A14;
    }
L_08809A14:
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_16);
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    goto L_08809A20;
L_08809A20:
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08809A4C;
      }
      goto L_08809A28;
    }
L_08809A28:
    aot_gpr_31 = (0x08809A30u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 336u, 0x08809A30u, 0x08AD375Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 618u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 618u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 618u, 0x08AD375Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809A30u) goto L_08809A30;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809A30:
    aot_gpr_4 = (ctx.gpr[2] & 130u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_08809A4C;
      }
      goto L_08809A3C;
    }
L_08809A3C:
    aot_gpr_31 = (0x08809A44u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 338u, 0x08809A44u, 0x08AD34D8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 577u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 577u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 577u, 0x08AD34D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809A44u) goto L_08809A44;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809A44:
    aot_gpr_31 = (0x08809A4Cu);
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0179.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 339u, 0x08809A4Cu, 0x08AD3780u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0179_entry(rt, ctx, 621u, aot_mem);
#else
        recomp_unit_0179_entry(rt, ctx, 621u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0179_entry, 179u, 621u, 0x08AD3780u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809A4Cu) goto L_08809A4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809A4C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528)));
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1));
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[17]) < static_cast<std::int32_t>(aot_gpr_5) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_16 = (aot_gpr_16 + static_cast<std::uint32_t>(72));
      if (branch_taken) {
          goto L_088099D8;
      }
      goto L_08809A64;
    }
L_08809A64:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809A78:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32528), aot_gpr_5);
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-32524), static_cast<std::uint8_t>(aot_gpr_4));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809A84:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (2237u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08809A98u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-28816));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0099.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 343u, 0x08809A98u, 0x089903B0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0099_entry(rt, ctx, 67u, aot_mem);
#else
        recomp_unit_0099_entry(rt, ctx, 67u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0099_entry, 99u, 67u, 0x089903B0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809A98u) goto L_08809A98;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809A98:
    aot_gpr_31 = (0x08809AA0u);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32504));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60680, 215u, 68u, 0x08B60680u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_16, ctx.gpr[17], ctx.gpr[18], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 68u, 0x08B60680u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809AA0u) goto L_08809AA0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809AA0:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809AAC:
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(0), aot_gpr_5);
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (aot_gpr_4 | 0u);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809ABC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08809AD0u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 347u, 0x08809AD0u, 0x08B59EE4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 542u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 542u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 542u, 0x08B59EE4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809AD0u) goto L_08809AD0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809AD0:
    aot_gpr_31 = (0x08809AD8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B58974, 213u, 150u, 0x08B58974u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0213_entry, 213u, 150u, 0x08B58974u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809AD8u) goto L_08809AD8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809AD8:
    aot_gpr_5 = (aot_gpr_16 + ctx.gpr[2]);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0))))));
    aot_gpr_4 = (0u | 32u);
    { const bool branch_taken = aot_gpr_6 != aot_gpr_4;
      if (branch_taken) {
          goto L_08809B04;
      }
      goto L_08809AF0;
    }
L_08809AF0:
    aot_mem.aot_direct_store8(aot_gpr_5 + static_cast<std::uint32_t>(0), static_cast<std::uint8_t>(0u));
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-1));
    aot_gpr_6 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(0))))));
    { const bool branch_taken = aot_gpr_6 == aot_gpr_4;
      if (branch_taken) {
          goto L_08809AF0;
      }
      goto L_08809B04;
    }
L_08809B04:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_16 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    { const bool branch_taken = aot_gpr_16 == 0u;
    aot_gpr_6 = (0u | 92u);
      if (branch_taken) {
          goto L_08809B34;
      }
      goto L_08809B14;
    }
L_08809B14:
    aot_gpr_5 = (0u | 47u);
    goto L_08809B18;
L_08809B18:
    { const bool branch_taken = aot_gpr_16 != aot_gpr_6;
      if (branch_taken) {
          goto L_08809B24;
      }
      goto L_08809B20;
    }
L_08809B20:
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(0), static_cast<std::uint8_t>(aot_gpr_5));
    goto L_08809B24;
L_08809B24:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_16 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(0))))));
    { const bool branch_taken = aot_gpr_16 != 0u;
      if (branch_taken) {
          goto L_08809B18;
      }
      goto L_08809B34;
    }
L_08809B34:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809B44:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-288));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(256), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_5 | 0u);
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(260), ctx.gpr[17]);
    ctx.gpr[17] = (aot_gpr_6 & 255u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    { const std::uint32_t aot_run_words[5]{ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(264), aot_run_words); }
    aot_gpr_31 = (0x08809B7Cu);
    aot_gpr_6 = (0u | 256u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 357u, 0x08809B7Cu, 0x08B58BA0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 200u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 200u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 200u, 0x08B58BA0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809B7Cu) goto L_08809B7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809B7C:
    aot_gpr_31 = (0x08809B84u);
    aot_gpr_4 = (aot_gpr_29 | 0u);
    goto L_08809ABC;
L_08809B84:
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[19] = (0u | 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08809B98u);
    aot_gpr_5 = (0u | 115u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 359u, 0x08809B98u, 0x08B58294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 50u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 50u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 50u, 0x08B58294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809B98u) goto L_08809B98;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809B98:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809BA4;
      }
      goto L_08809BA0;
    }
L_08809BA0:
    ctx.gpr[18] = (0u | 1u);
    goto L_08809BA4;
L_08809BA4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08809BB0u);
    aot_gpr_5 = (0u | 114u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 362u, 0x08809BB0u, 0x08B58294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 50u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 50u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 50u, 0x08B58294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809BB0u) goto L_08809BB0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809BB0:
    { const bool branch_taken = ctx.gpr[2] != 0u;
    aot_gpr_4 = (aot_gpr_16 | 0u);
      if (branch_taken) {
          goto L_08809BC8;
      }
      goto L_08809BB8;
    }
L_08809BB8:
    aot_gpr_31 = (0x08809BC0u);
    aot_gpr_5 = (0u | 43u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 364u, 0x08809BC0u, 0x08B58294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 50u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 50u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 50u, 0x08B58294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809BC0u) goto L_08809BC0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809BC0:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809BCC;
      }
      goto L_08809BC8;
    }
L_08809BC8:
    ctx.gpr[19] = (0u | 1u);
    goto L_08809BCC;
L_08809BCC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x08809BD8u);
    aot_gpr_5 = (0u | 119u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 367u, 0x08809BD8u, 0x08B58294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 50u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 50u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 50u, 0x08B58294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809BD8u) goto L_08809BD8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809BD8:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809BE4;
      }
      goto L_08809BE0;
    }
L_08809BE0:
    ctx.gpr[19] = (ctx.gpr[19] | 1538u);
    goto L_08809BE4;
L_08809BE4:
    { const bool branch_taken = ctx.gpr[18] == 0u;
      if (branch_taken) {
          goto L_08809C0C;
      }
      goto L_08809BEC;
    }
L_08809BEC:
    { const bool branch_taken = ctx.gpr[17] == 0u;
      if (branch_taken) {
          goto L_08809C0C;
      }
      goto L_08809BF4;
    }
L_08809BF4:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x08809C04u);
    aot_gpr_6 = (0u | 511u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0044_entry, 44u, 220u, 0x088B5564u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809C04u) goto L_08809C04;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809C04:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
      if (branch_taken) {
          goto L_08809C20;
      }
      goto L_08809C0C;
    }
L_08809C0C:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    aot_gpr_31 = (0x08809C1Cu);
    aot_gpr_6 = (0u | 511u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0044_entry, 44u, 214u, 0x088B54E4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809C1Cu) goto L_08809C1C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809C1C:
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
    goto L_08809C20;
L_08809C20:
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[19]) < 0;
      if (branch_taken) {
          goto L_08809C40;
      }
      goto L_08809C28;
    }
L_08809C28:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(5952)));
    aot_gpr_16 = (2235u << 16u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_16 = (aot_gpr_16 + static_cast<std::uint32_t>(18176));
      if (branch_taken) {
          goto L_08809C48;
      }
      goto L_08809C38;
    }
L_08809C38:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08809C68;
      }
      goto L_08809C40;
    }
L_08809C40:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08809D28;
      }
      goto L_08809C48;
    }
L_08809C48:
    aot_gpr_4 = (0u | 1u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(5952), aot_gpr_4);
    ctx.gpr[7] = (2177u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 16u);
    aot_gpr_6 = (0u | 8u);
    aot_gpr_31 = (0x08809C68u);
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-25940));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 380u, 0x08809C68u, 0x08B60C7Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 158u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 158u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 158u, 0x08B60C7Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809C68u) goto L_08809C68;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809C68:
    aot_gpr_5 = (aot_gpr_16 | 0u);
    aot_gpr_16 = (0u | 0u);
    aot_gpr_4 = (0u | 0u);
    goto L_08809C74;
L_08809C74:
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_6) >= 0;
      if (branch_taken) {
          goto L_08809C84;
      }
      goto L_08809C80;
    }
L_08809C80:
    aot_gpr_16 = (aot_gpr_5 | 0u);
    goto L_08809C84;
L_08809C84:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_gpr_6 = (aot_gpr_4 < static_cast<std::uint32_t>(16) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(8));
      if (branch_taken) {
          goto L_08809C74;
      }
      goto L_08809C94;
    }
L_08809C94:
    { const bool branch_taken = aot_gpr_16 == 0u;
      if (branch_taken) {
          goto L_08809CCC;
      }
      goto L_08809C9C;
    }
L_08809C9C:
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(0), ctx.gpr[19]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(4)));
    ctx.gpr[19] = (32768u << 16u);
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(-1));
    aot_gpr_5 = (ctx.gpr[18] & 1u);
    aot_gpr_4 = (aot_gpr_4 & ctx.gpr[19]);
    aot_gpr_5 = (aot_gpr_5 << 31u);
    aot_gpr_4 = (aot_gpr_4 | aot_gpr_5);
    { const bool branch_taken = ctx.gpr[18] == 0u;
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(4), aot_gpr_4);
      if (branch_taken) {
          goto L_08809CD4;
      }
      goto L_08809CC4;
    }
L_08809CC4:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_08809D24;
      }
      goto L_08809CCC;
    }
L_08809CCC:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (aot_gpr_16 | 0u);
      if (branch_taken) {
          goto L_08809D28;
      }
      goto L_08809CD4;
    }
L_08809CD4:
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32484)));
    ctx.gpr[20] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32488)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (ctx.gpr[21] | 0u);
    aot_gpr_6 = (ctx.gpr[20] | 0u);
    aot_gpr_31 = (0x08809CF0u);
    ctx.gpr[8] = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0205.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 389u, 0x08809CF0u, 0x08B3B264u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0205_entry(rt, ctx, 898u, aot_mem);
#else
        recomp_unit_0205_entry(rt, ctx, 898u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0205_entry, 205u, 898u, 0x08B3B264u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809CF0u) goto L_08809CF0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809CF0:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_6 = (32768u << 16u);
    aot_gpr_4 = (aot_gpr_4 & ctx.gpr[19]);
    aot_gpr_5 = (aot_gpr_5 & aot_gpr_6);
    aot_gpr_4 = (aot_gpr_5 | aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_16 + static_cast<std::uint32_t>(0)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(4), aot_gpr_4);
    ctx.gpr[7] = (ctx.gpr[21] | 0u);
    aot_gpr_6 = (ctx.gpr[20] | 0u);
    aot_gpr_4 = (aot_gpr_5 | 0u);
    aot_gpr_31 = (0x08809D24u);
    ctx.gpr[8] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0205.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 390u, 0x08809D24u, 0x08B3B264u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0205_entry(rt, ctx, 898u, aot_mem);
#else
        recomp_unit_0205_entry(rt, ctx, 898u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0205_entry, 205u, 898u, 0x08B3B264u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809D24u) goto L_08809D24;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809D24:
    ctx.gpr[2] = (aot_gpr_16 | 0u);
    goto L_08809D28;
L_08809D28:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(256), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(288));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809D4C:
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_4) < 28 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] == 0u;
      if (branch_taken) {
          goto L_08809D88;
      }
      goto L_08809D58;
    }
L_08809D58:
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_4) < 24 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
      if (branch_taken) {
          goto L_08809D78;
      }
      goto L_08809D64;
    }
L_08809D64:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-24));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), aot_gpr_4);
    aot_gpr_4 = (0u | 1u);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(0), aot_gpr_4);
      if (branch_taken) {
          goto L_08809D80;
      }
      goto L_08809D78;
    }
L_08809D78:
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_6 + static_cast<std::uint32_t>(0), 0u);
    goto L_08809D80;
L_08809D80:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_08809D8C;
      }
      goto L_08809D88;
    }
L_08809D88:
    ctx.gpr[2] = (0u | 0u);
    goto L_08809D8C;
L_08809D8C:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809D94:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    { const std::uint32_t aot_run_words[7]{aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], ctx.gpr[11]};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809DBC:
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(0), 0u);
    aot_gpr_5 = (0u | 96u);
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_5 = (0u | 112u);
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_5 = (0u | 127u);
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint8_t>(aot_gpr_5));
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(12), 0u);
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (aot_gpr_4 | 0u);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809DEC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_5 = (aot_gpr_5 & 1u);
      if (branch_taken) {
          goto L_08809E0C;
      }
      goto L_08809DFC;
    }
L_08809DFC:
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_08809E0C;
      }
      goto L_08809E04;
    }
L_08809E04:
    aot_gpr_31 = (0x08809E0Cu);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08ABE2C0, 174u, 470u, 0x08ABE2C0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0174_entry, 174u, 470u, 0x08ABE2C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809E0Cu) goto L_08809E0C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809E0C:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809E18:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x08809E2Cu);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    goto L_08809E44;
L_08809E2C:
    aot_gpr_31 = (0x08809E34u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_08809E80;
L_08809E34:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809E44:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x08809E54u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 409u, 0x08809E54u, 0x08B62FE8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 651u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 651u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 651u, 0x08B62FE8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809E54u) goto L_08809E54;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809E54:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_08809E64;
      }
      goto L_08809E5C;
    }
L_08809E5C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08809E74;
      }
      goto L_08809E64;
    }
L_08809E64:
    aot_gpr_4 = (0u | 0u);
    aot_gpr_31 = (0x08809E70u);
    aot_gpr_5 = (0u | 768u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0215.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 412u, 0x08809E70u, 0x08B63158u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0215_entry(rt, ctx, 674u, aot_mem);
#else
        recomp_unit_0215_entry(rt, ctx, 674u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0215_entry, 215u, 674u, 0x08B63158u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809E70u) goto L_08809E70;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809E70:
    ctx.gpr[2] = (0u | 1u);
    goto L_08809E74;
L_08809E74:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809E80:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32480)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_08809E94;
      }
      goto L_08809E8C;
    }
L_08809E8C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08809E98;
      }
      goto L_08809E94;
    }
L_08809E94:
    ctx.gpr[2] = (0u | 1u);
    goto L_08809E98;
L_08809E98:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809EA0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32480)));
    { const std::uint32_t aot_run_words[3]{aot_gpr_16, ctx.gpr[17], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_16 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_08809F54;
      }
      goto L_08809EBC;
    }
L_08809EBC:
    ctx.gpr[17] = (ctx.gpr[28] + static_cast<std::uint32_t>(7712));
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08809ED0u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16240));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 420u, 0x08809ED0u, 0x08986B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 695u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 695u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 695u, 0x08986B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809ED0u) goto L_08809ED0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809ED0:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08809EE0u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16268));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 421u, 0x08809EE0u, 0x08986B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 695u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 695u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 695u, 0x08986B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809EE0u) goto L_08809EE0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809EE0:
    aot_gpr_31 = (0x08809EE8u);
    aot_gpr_4 = (0u | 512u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 26u, 0x08B64324u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809EE8u) goto L_08809EE8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809EE8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08809F3C;
      }
      goto L_08809EF0;
    }
L_08809EF0:
    aot_gpr_31 = (0x08809EF8u);
    aot_gpr_4 = (0u | 4u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 77u, 0x08B646ECu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809EF8u) goto L_08809EF8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809EF8:
    aot_gpr_4 = (0u | 1u);
    aot_gpr_31 = (0x08809F04u);
    aot_gpr_5 = (0u | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 69u, 0x08B64654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809F04u) goto L_08809F04;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809F04:
    aot_gpr_4 = (0u | 4096u);
    aot_gpr_31 = (0x08809F10u);
    aot_gpr_5 = (0u | 4096u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 73u, 0x08B646A0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809F10u) goto L_08809F10;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809F10:
    aot_gpr_31 = (0x08809F18u);
    ctx.pc = 0x08B733FCu;
    AOT_REGCACHE_SYNC_OUT(); return;
L_08809F18:
    aot_gpr_4 = (0u | 127u);
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_4));
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_4));
    aot_gpr_31 = (0x08809F2Cu);
    aot_mem.aot_direct_store8(aot_gpr_16 + static_cast<std::uint32_t>(8), static_cast<std::uint8_t>(aot_gpr_4));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 428u, 0x08809F2Cu, 0x089867DCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 631u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 631u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 631u, 0x089867DCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809F2Cu) goto L_08809F2C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809F2C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_08809F5C;
      }
      goto L_08809F34;
    }
L_08809F34:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08809FBC;
      }
      goto L_08809F3C;
    }
L_08809F3C:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x08809F4Cu);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16292));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 431u, 0x08809F4Cu, 0x08986B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 695u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 695u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 695u, 0x08986B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x08809F4Cu) goto L_08809F4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_08809F4C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_08809FBC;
      }
      goto L_08809F54;
    }
L_08809F54:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_08809FBC;
      }
      goto L_08809F5C;
    }
L_08809F5C:
    aot_gpr_4 = (2235u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(18304));
    aot_gpr_6 = (0u | 0u);
    aot_gpr_5 = (aot_gpr_4 | 0u);
    goto L_08809F6C;
L_08809F6C:
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(8), 0u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_6) < 2 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08809F6C;
      }
      goto L_08809F84;
    }
L_08809F84:
    aot_gpr_6 = (0u | 0u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    goto L_08809F8C;
L_08809F8C:
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(16), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(352), 0u);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(128), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(464), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(576), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(688), aot_gpr_5);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_6) < 28 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_08809F8C;
      }
      goto L_08809FB4;
    }
L_08809FB4:
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479), static_cast<std::uint8_t>(0u));
    ctx.gpr[2] = (0u | 1u);
    goto L_08809FBC;
L_08809FBC:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809FD0:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_08809FD8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-48));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32476)));
    { const std::uint32_t aot_run_words[6]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_run_words); }
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A010;
      }
      goto L_0880A008;
    }
L_0880A008:
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-1));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32476), aot_gpr_4);
    goto L_0880A010;
L_0880A010:
    ctx.gpr[19] = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[19]) < 127 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A034;
      }
      goto L_0880A020;
    }
L_0880A020:
    ctx.gpr[19] = (ctx.gpr[19] + static_cast<std::uint32_t>(4));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[19]) < 128 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880A034;
      }
      goto L_0880A030;
    }
L_0880A030:
    ctx.gpr[19] = (0u | 127u);
    goto L_0880A034;
L_0880A034:
    aot_gpr_5 = (ctx.gpr[19] & 255u);
    aot_gpr_31 = (0x0880A040u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_0880A6FC;
L_0880A040:
    ctx.gpr[17] = (2237u << 16u);
    ctx.gpr[19] = (0u | 0u);
    ctx.gpr[18] = (0u | 0u);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(-28320));
    goto L_0880A050;
L_0880A050:
    ctx.gpr[20] = (ctx.gpr[18] & 255u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880A060u);
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0049.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 448u, 0x0880A060u, 0x088CA2F4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0049_entry(rt, ctx, 425u, aot_mem);
#else
        recomp_unit_0049_entry(rt, ctx, 425u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0049_entry, 49u, 425u, 0x088CA2F4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A060u) goto L_0880A060;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A060:
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_gpr_4 = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880A080;
      }
      goto L_0880A068;
    }
L_0880A068:
    aot_gpr_31 = (0x0880A070u);
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0049.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 450u, 0x0880A070u, 0x088CA494u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0049_entry(rt, ctx, 452u, aot_mem);
#else
        recomp_unit_0049_entry(rt, ctx, 452u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0049_entry, 49u, 452u, 0x088CA494u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A070u) goto L_0880A070;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A070:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880A080;
      }
      goto L_0880A078;
    }
L_0880A078:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[19] = (0u | 1u);
      if (branch_taken) {
          goto L_0880A090;
      }
      goto L_0880A080;
    }
L_0880A080:
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < 5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880A050;
      }
      goto L_0880A090;
    }
L_0880A090:
    { const bool branch_taken = ctx.gpr[19] == 0u;
      if (branch_taken) {
          goto L_0880A0CC;
      }
      goto L_0880A098;
    }
L_0880A098:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7660)));
    aot_mem.aot_direct_store32(aot_gpr_16 + static_cast<std::uint32_t>(12), aot_gpr_4);
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32460)));
    ctx.fpr[14] = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32468)));
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32472)));
    aot_fpr_13 = aot_fpr_13 - ctx.fpr[14];
    aot_gpr_16 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32480)));
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((aot_fpr_12 < aot_fpr_13)) ? 0x00800000u : 0u);
    if (!((ctx.fcr31 & 0x00800000u) != 0u)) {
    aot_fpr_13 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_12));
        goto L_0880A0C4;
    }
    goto L_0880A0C4;
L_0880A0C4:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32460), std::bit_cast<std::uint32_t>(aot_fpr_13));
      if (branch_taken) {
          goto L_0880A124;
      }
      goto L_0880A0CC;
    }
L_0880A0CC:
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32464)));
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32460)));
    ctx.gpr[21] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32428)));
    ctx.gpr[20] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32432)));
    aot_gpr_31 = (0x0880A0E4u);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B61B60, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 341u, 0x08B61B60u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A0E4u) goto L_0880A0E4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A0E4:
    ctx.gpr[19] = (ctx.gpr[3] | 0u);
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32480)));
    aot_gpr_5 = (ctx.gpr[19] | 0u);
    ctx.gpr[7] = (ctx.gpr[21] | 0u);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880A104u);
    aot_gpr_6 = (ctx.gpr[20] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B6256C, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 478u, 0x08B6256Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A104u) goto L_0880A104;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A104:
    if (static_cast<std::int32_t>(ctx.gpr[2]) >= 0) {
    aot_gpr_5 = (ctx.gpr[21] | 0u);
        goto L_0880A118;
    }
    goto L_0880A10C;
L_0880A10C:
    ctx.gpr[21] = (ctx.gpr[19] | 0u);
    ctx.gpr[20] = (ctx.gpr[18] | 0u);
    aot_gpr_5 = (ctx.gpr[21] | 0u);
    goto L_0880A118;
L_0880A118:
    aot_gpr_31 = (0x0880A120u);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B627F0, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], ctx.gpr[3], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], ctx.gpr[9], ctx.gpr[10], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 515u, 0x08B627F0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A120u) goto L_0880A120;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A120:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32460), std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    goto L_0880A124;
L_0880A124:
    { const bool branch_taken = aot_gpr_16 != 0u;
      if (branch_taken) {
          goto L_0880A16C;
      }
      goto L_0880A12C;
    }
L_0880A12C:
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880A154;
      }
      goto L_0880A138;
    }
L_0880A138:
    aot_gpr_4 = (2235u << 16u);
    aot_gpr_5 = (2235u << 16u);
    ctx.gpr[7] = (0u | 0u);
    aot_gpr_6 = (0u | 100u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(18304));
    { const bool branch_taken = 0u == 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(19104));
      if (branch_taken) {
          goto L_0880A174;
      }
      goto L_0880A154;
    }
L_0880A154:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(7712));
    aot_gpr_31 = (0x0880A164u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16316));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 466u, 0x0880A164u, 0x08986B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 695u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 695u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 695u, 0x08986B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A164u) goto L_0880A164;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A164:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880A1F0;
      }
      goto L_0880A16C;
    }
L_0880A16C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880A1F0;
      }
      goto L_0880A174;
    }
L_0880A174:
    ctx.gpr[8] = (ctx.gpr[7] + aot_gpr_4);
    ctx.gpr[9] = (aot_mem.aot_direct_load32(ctx.gpr[8] + static_cast<std::uint32_t>(0)));
    ctx.gpr[8] = (aot_mem.aot_direct_load32(ctx.gpr[8] + static_cast<std::uint32_t>(4)));
    ctx.gpr[10] = (ctx.gpr[7] + aot_gpr_5);
    aot_mem.aot_direct_store32(ctx.gpr[10] + static_cast<std::uint32_t>(0), ctx.gpr[9]);
    aot_mem.aot_direct_store32(ctx.gpr[10] + static_cast<std::uint32_t>(4), ctx.gpr[8]);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-1));
    { const bool branch_taken = aot_gpr_6 != 0u;
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(8));
      if (branch_taken) {
          goto L_0880A174;
      }
      goto L_0880A198;
    }
L_0880A198:
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479), static_cast<std::uint8_t>(aot_gpr_5));
    aot_gpr_6 = (0u | 0u);
    aot_gpr_5 = (aot_gpr_4 | 0u);
    goto L_0880A1A8;
L_0880A1A8:
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), 0u);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(8), 0u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_6) < 2 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_0880A1A8;
      }
      goto L_0880A1C0;
    }
L_0880A1C0:
    aot_gpr_6 = (0u | 0u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    goto L_0880A1C8;
L_0880A1C8:
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(16), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(352), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(128), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(464), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(576), aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(688), aot_gpr_5);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_6) < 28 ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(4));
      if (branch_taken) {
          goto L_0880A1C8;
      }
      goto L_0880A1F0;
    }
L_0880A1F0:
    { std::uint32_t aot_run_words[7]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      ctx.gpr[19] = aot_run_words[3];
      ctx.gpr[20] = aot_run_words[4];
      ctx.gpr[21] = aot_run_words[5];
      aot_gpr_31 = aot_run_words[6];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(48));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A214:
    aot_gpr_4 = (ctx.gpr[7] & 255u);
    ctx.gpr[7] = (ctx.gpr[8] & 255u);
    ctx.gpr[8] = (ctx.gpr[9] & 255u);
    ctx.gpr[9] = (2235u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    ctx.gpr[9] = (ctx.gpr[9] + static_cast<std::uint32_t>(18304));
    aot_gpr_5 = (aot_gpr_5 + ctx.gpr[9]);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(16), aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(128), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(240), ctx.gpr[8]);
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(352), ctx.gpr[7]);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A244:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_gpr_5 & 255u);
    aot_gpr_5 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(7713)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880A264;
      }
      goto L_0880A25C;
    }
L_0880A25C:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_0880A274;
      }
      goto L_0880A264;
    }
L_0880A264:
    aot_gpr_31 = (0x0880A26Cu);
    aot_gpr_5 = (0u | 255u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 478u, 0x0880A26Cu, 0x08986968u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 661u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 661u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 661u, 0x08986968u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A26Cu) goto L_0880A26C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A26C:
    ctx.gpr[2] = (ctx.gpr[2] << 24u);
    ctx.gpr[2] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[2]) >> 24u));
    goto L_0880A274;
L_0880A274:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A280:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A288:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_gpr_5 & 255u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x0880A29Cu);
    aot_gpr_5 = (0u | 255u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 482u, 0x0880A29Cu, 0x089867F8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 633u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 633u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 633u, 0x089867F8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A29Cu) goto L_0880A29C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A29C:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A2A8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_6 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(7713)));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = aot_gpr_6 != 0u;
    aot_gpr_4 = (aot_gpr_5 | 0u);
      if (branch_taken) {
          goto L_0880A308;
      }
      goto L_0880A2BC;
    }
L_0880A2BC:
    aot_gpr_6 = (2237u << 16u);
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(-1008)));
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880A2F4;
      }
      goto L_0880A2CC;
    }
L_0880A2CC:
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-1008));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(1400)));
    if (static_cast<std::int32_t>(aot_gpr_5) > 0) {
    aot_gpr_6 = (0u | 2u);
        goto L_0880A310;
    }
    goto L_0880A2DC;
L_0880A2DC:
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_5) < 0;
      if (branch_taken) {
          goto L_0880A2F4;
      }
      goto L_0880A2E4;
    }
L_0880A2E4:
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_6 + static_cast<std::uint32_t>(1398)));
    aot_gpr_6 = (0u | 109u);
    { const bool branch_taken = aot_gpr_5 == aot_gpr_6;
      if (branch_taken) {
          goto L_0880A320;
      }
      goto L_0880A2F4;
    }
L_0880A2F4:
    aot_gpr_31 = (0x0880A2FCu);
    aot_gpr_5 = (0u | 255u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 489u, 0x0880A2FCu, 0x08986890u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 648u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 648u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 648u, 0x08986890u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A2FCu) goto L_0880A2FC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A2FC:
    ctx.gpr[2] = (ctx.gpr[2] << 24u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[2]) >> 24u));
      if (branch_taken) {
          goto L_0880A324;
      }
      goto L_0880A308;
    }
L_0880A308:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_0880A324;
      }
      goto L_0880A310;
    }
L_0880A310:
    { const bool branch_taken = aot_gpr_5 != aot_gpr_6;
      if (branch_taken) {
          goto L_0880A2F4;
      }
      goto L_0880A318;
    }
L_0880A318:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 0u);
      if (branch_taken) {
          goto L_0880A324;
      }
      goto L_0880A320;
    }
L_0880A320:
    ctx.gpr[2] = (0u | 0u);
    goto L_0880A324;
L_0880A324:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A330:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_gpr_5 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x0880A344u);
    aot_gpr_5 = (0u | 255u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 496u, 0x0880A344u, 0x08987958u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 872u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 872u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 872u, 0x08987958u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A344u) goto L_0880A344;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A344:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A350:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (aot_gpr_5 & 255u);
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_5) < 8 ? 1u : 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = ctx.gpr[7] == 0u;
    aot_gpr_4 = (aot_gpr_6 | 0u);
      if (branch_taken) {
          goto L_0880A380;
      }
      goto L_0880A368;
    }
L_0880A368:
    aot_gpr_6 = (0u | 300u);
    aot_gpr_31 = (0x0880A374u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32476), aot_gpr_6);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 499u, 0x0880A374u, 0x08986890u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 648u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 648u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 648u, 0x08986890u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A374u) goto L_0880A374;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A374:
    ctx.gpr[2] = (ctx.gpr[2] << 24u);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(ctx.gpr[2]) >> 24u));
      if (branch_taken) {
          goto L_0880A384;
      }
      goto L_0880A380;
    }
L_0880A380:
    ctx.gpr[2] = (0u | 0u);
    goto L_0880A384;
L_0880A384:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A390:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_5 = (aot_gpr_5 & 255u);
    ctx.gpr[7] = (static_cast<std::int32_t>(aot_gpr_5) < 8 ? 1u : 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    { const bool branch_taken = ctx.gpr[7] == 0u;
    aot_gpr_4 = (aot_gpr_6 | 0u);
      if (branch_taken) {
          goto L_0880A3B8;
      }
      goto L_0880A3A8;
    }
L_0880A3A8:
    aot_gpr_31 = (0x0880A3B0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 504u, 0x0880A3B0u, 0x08987958u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 872u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 872u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 872u, 0x08987958u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A3B0u) goto L_0880A3B0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A3B0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880A3BC;
      }
      goto L_0880A3B8;
    }
L_0880A3B8:
    ctx.gpr[2] = (0u | 0u);
    goto L_0880A3BC;
L_0880A3BC:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A3C8:
    aot_gpr_4 = (0u | 286u);
    { const bool branch_taken = aot_gpr_5 == aot_gpr_4;
    aot_gpr_4 = (0u | 287u);
      if (branch_taken) {
          goto L_0880A3EC;
      }
      goto L_0880A3D4;
    }
L_0880A3D4:
    { const bool branch_taken = aot_gpr_5 == aot_gpr_4;
    aot_gpr_4 = (0u | 288u);
      if (branch_taken) {
          goto L_0880A3EC;
      }
      goto L_0880A3DC;
    }
L_0880A3DC:
    { const bool branch_taken = aot_gpr_5 == aot_gpr_4;
    aot_gpr_4 = (0u | 289u);
      if (branch_taken) {
          goto L_0880A3EC;
      }
      goto L_0880A3E4;
    }
L_0880A3E4:
    { const bool branch_taken = aot_gpr_5 != aot_gpr_4;
      if (branch_taken) {
          goto L_0880A3F4;
      }
      goto L_0880A3EC;
    }
L_0880A3EC:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_0880A3F8;
      }
      goto L_0880A3F4;
    }
L_0880A3F4:
    ctx.gpr[2] = (0u | 0u);
    goto L_0880A3F8;
L_0880A3F8:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A400:
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_5);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16328)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A41C:
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_5);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16328)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    jump_target = aot_gpr_31;
    ctx.gpr[2] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A438:
    ctx.gpr[9] = (ctx.gpr[7] & 255u);
    ctx.gpr[10] = (2235u << 16u);
    ctx.gpr[7] = (aot_gpr_5 << 2u);
    ctx.gpr[10] = (ctx.gpr[10] + static_cast<std::uint32_t>(18304));
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    { const bool branch_taken = ctx.gpr[9] == 0u;
    ctx.gpr[7] = (ctx.gpr[7] + ctx.gpr[10]);
      if (branch_taken) {
          goto L_0880A458;
      }
      goto L_0880A454;
    }
L_0880A454:
    ctx.gpr[8] = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(5)));
    goto L_0880A458;
L_0880A458:
    ctx.gpr[9] = (aot_gpr_6 < static_cast<std::uint32_t>(128) ? 1u : 0u);
    { const bool branch_taken = ctx.gpr[9] != 0u;
      if (branch_taken) {
          goto L_0880A468;
      }
      goto L_0880A464;
    }
L_0880A464:
    aot_gpr_6 = (0u | 127u);
    goto L_0880A468;
L_0880A468:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_6)) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[8])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[8] = (0u | 22u);
    aot_gpr_6 = (ctx.lo);
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[8];
    ctx.gpr[8] = (0u | 23u);
      if (branch_taken) {
          goto L_0880A49C;
      }
      goto L_0880A47C;
    }
L_0880A47C:
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[8];
    ctx.gpr[8] = (0u | 24u);
      if (branch_taken) {
          goto L_0880A49C;
      }
      goto L_0880A484;
    }
L_0880A484:
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[8];
    ctx.gpr[8] = (0u | 25u);
      if (branch_taken) {
          goto L_0880A49C;
      }
      goto L_0880A48C;
    }
L_0880A48C:
    { const bool branch_taken = aot_gpr_5 == ctx.gpr[8];
    ctx.gpr[8] = (0u | 26u);
      if (branch_taken) {
          goto L_0880A49C;
      }
      goto L_0880A494;
    }
L_0880A494:
    { const bool branch_taken = aot_gpr_5 != ctx.gpr[8];
      if (branch_taken) {
          goto L_0880A4F8;
      }
      goto L_0880A49C;
    }
L_0880A49C:
    aot_gpr_5 = (2236u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(32304));
    aot_gpr_5 = (aot_mem.aot_direct_load8(aot_gpr_5 + static_cast<std::uint32_t>(2651)));
    aot_gpr_5 = (0u < aot_gpr_5 ? 1u : 0u);
    aot_gpr_5 = (aot_gpr_5 & 255u);
    if (aot_gpr_5 == 0u) {
    aot_gpr_4 = (0u | 127u);
        goto L_0880A4E8;
    }
    goto L_0880A4B8;
L_0880A4B8:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(6)));
    aot_gpr_5 = (0u | 21336u);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_6)) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (ctx.lo);
    { const std::uint32_t dividend = aot_gpr_4; const std::uint32_t divisor = aot_gpr_5; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_gpr_4 = (0u | 64u);
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(576), aot_gpr_4);
    aot_gpr_4 = (ctx.lo);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(688), aot_gpr_4);
      if (branch_taken) {
          goto L_0880A57C;
      }
      goto L_0880A4E8;
    }
L_0880A4E8:
    { const std::uint32_t dividend = aot_gpr_6; const std::uint32_t divisor = aot_gpr_4; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_gpr_4 = (ctx.lo);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(688), aot_gpr_4);
      if (branch_taken) {
          goto L_0880A57C;
      }
      goto L_0880A4F8;
    }
L_0880A4F8:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[7] + static_cast<std::uint32_t>(128)));
    ctx.gpr[8] = (0u | 3u);
    if (aot_gpr_5 != ctx.gpr[8]) {
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(6)));
        goto L_0880A51C;
    }
    goto L_0880A508;
L_0880A508:
    aot_gpr_4 = (0u | 168u);
    { const std::uint32_t dividend = aot_gpr_6; const std::uint32_t divisor = aot_gpr_4; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_gpr_4 = (ctx.lo);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(688), aot_gpr_4);
      if (branch_taken) {
          goto L_0880A57C;
      }
      goto L_0880A51C;
    }
L_0880A51C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[7] + static_cast<std::uint32_t>(240)));
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_6)) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_6 = (ctx.lo);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_4 = (0u | 21336u);
      if (branch_taken) {
          goto L_0880A570;
      }
      goto L_0880A530;
    }
L_0880A530:
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_6) >= 0;
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
      if (branch_taken) {
          goto L_0880A548;
      }
      goto L_0880A53C;
    }
L_0880A53C:
    aot_gpr_4 = (20352u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    goto L_0880A548;
L_0880A548:
    aot_fpr_13 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32460)));
    aot_gpr_4 = (18086u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 45056u);
    { const float fs = aot_fpr_12; const float ft = aot_fpr_13; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    ctx.fpr[14] = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 / ctx.fpr[14];
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_gpr_4 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(688), aot_gpr_4);
      if (branch_taken) {
          goto L_0880A57C;
      }
      goto L_0880A570;
    }
L_0880A570:
    { const std::uint32_t dividend = aot_gpr_6; const std::uint32_t divisor = aot_gpr_4; if (divisor == 0u) { ctx.lo = 0xFFFFFFFFu; ctx.hi = dividend; } else { ctx.lo = dividend / divisor; ctx.hi = dividend % divisor; } }
    aot_gpr_4 = (ctx.lo);
    aot_mem.aot_direct_store32(ctx.gpr[7] + static_cast<std::uint32_t>(688), aot_gpr_4);
    goto L_0880A57C;
L_0880A57C:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A584:
    aot_gpr_4 = (aot_gpr_6 | 0u);
    aot_gpr_6 = (aot_gpr_4 < static_cast<std::uint32_t>(127) ? 1u : 0u);
    if (aot_gpr_6 == 0u) {
    aot_gpr_4 = (0u | 127u);
        goto L_0880A594;
    }
    goto L_0880A594;
L_0880A594:
    aot_gpr_6 = (2235u << 16u);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(18304));
    aot_gpr_5 = (aot_gpr_5 + aot_gpr_6);
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(576), aot_gpr_4);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A5AC:
    jump_target = aot_gpr_31;
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A5B4:
    aot_gpr_4 = (0u | 44000u);
    aot_gpr_4 = (aot_gpr_4 < aot_gpr_6 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A5C8;
      }
      goto L_0880A5C4;
    }
L_0880A5C4:
    aot_gpr_6 = (0u | 44000u);
    goto L_0880A5C8;
L_0880A5C8:
    aot_gpr_4 = (aot_gpr_5 << 2u);
    aot_gpr_5 = (2235u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(18304));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(464), aot_gpr_6);
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A5E0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880A5F4u);
    aot_gpr_16 = (aot_gpr_5 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0169.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 543u, 0x0880A5F4u, 0x08AABBE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0169_entry(rt, ctx, 416u, aot_mem);
#else
        recomp_unit_0169_entry(rt, ctx, 416u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0169_entry, 169u, 416u, 0x08AABBE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A5F4u) goto L_0880A5F4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A5F4:
    aot_gpr_31 = (0x0880A5FCu);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 63u, 0x08B64618u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A5FCu) goto L_0880A5FC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A5FC:
    // SAS cannot report a voice that is still in VCS's command queue. Keep
    // its owner alive until the mixer actually consumes the queued KeyOn.
    ctx.gpr[2] = vcs::audio_end_flags_with_pending_starts(ctx.gpr[2],
        aot_mem.aot_direct_load32(0x08BB4780u), aot_mem.aot_direct_load32(0x08BB4784u),
        aot_mem.aot_direct_load32(0x08BB4AA0u), aot_mem.aot_direct_load32(0x08BB4AA4u),
        aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479)) != 0u);
    aot_gpr_4 = (0u | 1u);
    aot_gpr_4 = (aot_gpr_4 << (aot_gpr_16 & 31u));
    aot_gpr_4 = (ctx.gpr[2] & aot_gpr_4);
    aot_gpr_31 = (0x0880A610u);
    aot_gpr_16 = (aot_gpr_4 < static_cast<std::uint32_t>(1) ? 1u : 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0169.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 545u, 0x0880A610u, 0x08AABC04u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0169_entry(rt, ctx, 418u, aot_mem);
#else
        recomp_unit_0169_entry(rt, ctx, 418u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0169_entry, 169u, 418u, 0x08AABC04u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A610u) goto L_0880A610;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A610:
    ctx.gpr[2] = (aot_gpr_16 | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A624:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_gpr_5 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(4));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880A640u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    goto L_08809D4C;
L_0880A640:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880A674;
      }
      goto L_0880A648;
    }
L_0880A648:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (2235u << 16u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(18304));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (0u | 1u);
    aot_gpr_6 = (ctx.gpr[7] << (aot_gpr_6 & 31u));
    aot_gpr_5 = (aot_gpr_5 | aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(0), aot_gpr_5);
    goto L_0880A674;
L_0880A674:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A680:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_4 = (aot_gpr_5 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_6 = (aot_gpr_29 + static_cast<std::uint32_t>(4));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880A69Cu);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    goto L_08809D4C;
L_0880A69C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880A6D0;
      }
      goto L_0880A6A4;
    }
L_0880A6A4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    aot_gpr_5 = (2235u << 16u);
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(18304));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (0u | 1u);
    aot_gpr_6 = (ctx.gpr[7] << (aot_gpr_6 & 31u));
    aot_gpr_5 = (aot_gpr_5 | aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(8), aot_gpr_5);
    // A stop newer than an unconsumed start cancels that start. The consumer
    // executes OFF then ON, so retaining both would restart a stopped loop.
    aot_mem.aot_direct_store32(aot_gpr_4,
        vcs::stop_queued_audio_start(aot_mem.aot_direct_load32(aot_gpr_4), aot_gpr_6));
    goto L_0880A6D0;
L_0880A6D0:
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(8)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A6DC:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(4), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A6E4:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(5), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A6EC:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(6), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A6F4:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(7), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A6FC:
    jump_target = aot_gpr_31;
    aot_mem.aot_direct_store8(aot_gpr_4 + static_cast<std::uint32_t>(8), static_cast<std::uint8_t>(aot_gpr_5));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A704:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7688)));
    ctx.gpr[17] = (2237u << 16u);
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(-28416));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_16 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_0880A740;
      }
      goto L_0880A728;
    }
L_0880A728:
    aot_gpr_4 = (0u | 1u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(7688), aot_gpr_4);
    aot_gpr_31 = (0x0880A738u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0010.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 561u, 0x0880A738u, 0x0882D794u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0010_entry(rt, ctx, 286u, aot_mem);
#else
        recomp_unit_0010_entry(rt, ctx, 286u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0010_entry, 10u, 286u, 0x0882D794u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A738u) goto L_0880A738;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A738:
    aot_gpr_31 = (0x0880A740u);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32448));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60680, 215u, 68u, 0x08B60680u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_16, ctx.gpr[17], ctx.gpr[18], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 68u, 0x08B60680u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A740u) goto L_0880A740;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A740:
    aot_gpr_31 = (0x0880A748u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0010.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 563u, 0x0880A748u, 0x0882E9B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0010_entry(rt, ctx, 611u, aot_mem);
#else
        recomp_unit_0010_entry(rt, ctx, 611u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0010_entry, 10u, 611u, 0x0882E9B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A748u) goto L_0880A748;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A748:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880A768;
      }
      goto L_0880A750;
    }
L_0880A750:
    ctx.gpr[2] = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(8)));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[2]) < 127 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A770;
      }
      goto L_0880A760;
    }
L_0880A760:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880A774;
      }
      goto L_0880A768;
    }
L_0880A768:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 127u);
      if (branch_taken) {
          goto L_0880A774;
      }
      goto L_0880A770;
    }
L_0880A770:
    ctx.gpr[2] = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(7)));
    goto L_0880A774;
L_0880A774:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880A788:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-144));
    aot_gpr_4 = (aot_mem.aot_direct_load8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479)));
    { const std::uint32_t aot_run_words[11]{std::bit_cast<std::uint32_t>(aot_fpr_20), aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], ctx.gpr[22], ctx.gpr[23], ctx.gpr[30], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(92), aot_run_words); }
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AEB4;
      }
      goto L_0880A7C4;
    }
L_0880A7C4:
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32452), 0u);
    aot_gpr_4 = (2235u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), 0u);
    ctx.gpr[22] = (aot_gpr_4 + static_cast<std::uint32_t>(19104));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), 0u);
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(-1008));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(40), ctx.gpr[22]);
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(-28320));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(88), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 + static_cast<std::uint32_t>(26304));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(20), aot_gpr_4);
    aot_gpr_4 = (2237u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(560));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(640));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(84), aot_gpr_4);
    aot_gpr_4 = (2237u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(80), aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(23168));
    aot_gpr_5 = (2237u << 16u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(23192));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(76), aot_gpr_4);
    aot_gpr_4 = (2231u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(72), aot_gpr_5);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16396));
    aot_gpr_5 = (2231u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(68), aot_gpr_4);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16452));
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32404));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(64), aot_gpr_5);
    aot_gpr_6 = (2231u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(16500));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(60), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(56), aot_gpr_6);
    aot_gpr_6 = (2237u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-28416));
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32448));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(36), aot_gpr_6);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(52), aot_gpr_4);
    aot_gpr_4 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(16580));
    ctx.gpr[7] = (2231u << 16u);
    aot_gpr_5 = (ctx.gpr[22] + static_cast<std::uint32_t>(688));
    aot_gpr_6 = (0u | 0u);
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(16616));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(48), aot_gpr_4);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(44), ctx.gpr[7]);
    aot_gpr_4 = (aot_gpr_6 + aot_gpr_5);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_4);
    aot_gpr_4 = (16204u << 16u);
    aot_gpr_4 = (aot_gpr_4 | 52429u);
    ctx.gpr[23] = (2237u << 16u);
    aot_fpr_20 = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[30] = (0u | 0u);
    ctx.gpr[20] = (0u | 0u);
    ctx.gpr[23] = (ctx.gpr[23] + static_cast<std::uint32_t>(23216));
    goto L_0880A8AC;
L_0880A8AC:
    aot_gpr_6 = (ctx.gpr[20] | 0u);
    ctx.gpr[7] = (0u | 0u);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_6) < 24 ? 1u : 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(20)));
    ctx.gpr[17] = (0u + static_cast<std::uint32_t>(-1));
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[21] = (0u | 1u);
      if (branch_taken) {
          goto L_0880A8D0;
      }
      goto L_0880A8C8;
    }
L_0880A8C8:
    aot_gpr_6 = (ctx.gpr[20] + static_cast<std::uint32_t>(-24));
    ctx.gpr[7] = (ctx.gpr[21] | 0u);
    goto L_0880A8D0;
L_0880A8D0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(40)));
    ctx.gpr[18] = (ctx.gpr[7] << 2u);
    aot_gpr_4 = (ctx.gpr[18] + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    ctx.gpr[19] = (ctx.gpr[21] << (aot_gpr_6 & 31u));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_5 & ctx.gpr[19]);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A8FC;
      }
      goto L_0880A8F4;
    }
L_0880A8F4:
    aot_gpr_31 = (0x0880A8FCu);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 44u, 0x08B64460u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A8FCu) goto L_0880A8FC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880A8FC:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[19]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), ctx.gpr[18]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(16)));
    { const bool branch_taken = aot_gpr_4 == ctx.gpr[17];
      if (branch_taken) {
          goto L_0880AC1C;
      }
      goto L_0880A910;
    }
L_0880A910:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[19]);
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(128)));
    { const bool branch_taken = aot_gpr_6 == ctx.gpr[17];
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), ctx.gpr[18]);
      if (branch_taken) {
          goto L_0880AC1C;
      }
      goto L_0880A920;
    }
L_0880A920:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(28), ctx.gpr[19]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(24), ctx.gpr[18]);
    aot_gpr_31 = (0x0880A930u);
    aot_gpr_4 = (aot_gpr_6 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 578u, 0x0880A930u, 0x0898694Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 660u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 660u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 660u, 0x0898694Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880A930u) goto L_0880A930;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880A930:
    ctx.gpr[7] = (0u | 3u);
    { const bool branch_taken = ctx.gpr[2] == ctx.gpr[21];
    ctx.gpr[9] = (0u | 4u);
      if (branch_taken) {
          goto L_0880A954;
      }
      goto L_0880A93C;
    }
L_0880A93C:
    aot_gpr_4 = (aot_gpr_29 + ctx.gpr[18]);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    ctx.gpr[21] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
    aot_gpr_5 = (aot_gpr_5 | ctx.gpr[19]);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store32(aot_gpr_4 + static_cast<std::uint32_t>(0), aot_gpr_5);
      if (branch_taken) {
          goto L_0880AE90;
      }
      goto L_0880A954;
    }
L_0880A954:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(88)));
    aot_gpr_6 = (0u | 2u);
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_4 + static_cast<std::uint32_t>(1400)));
    { const bool branch_taken = aot_gpr_4 != aot_gpr_6;
      if (branch_taken) {
          goto L_0880A970;
      }
      goto L_0880A968;
    }
L_0880A968:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[21] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
      if (branch_taken) {
          goto L_0880AE90;
      }
      goto L_0880A970;
    }
L_0880A970:
    ctx.gpr[11] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(128)));
    { const bool branch_taken = ctx.gpr[11] == aot_gpr_6;
    ctx.gpr[10] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(16)));
      if (branch_taken) {
          goto L_0880A988;
      }
      goto L_0880A97C;
    }
L_0880A97C:
    aot_gpr_4 = (0u | 67u);
    { const bool branch_taken = ctx.gpr[11] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880AB78;
      }
      goto L_0880A988;
    }
L_0880A988:
    aot_gpr_4 = (0u | 22u);
    { const bool branch_taken = ctx.gpr[20] != aot_gpr_4;
    ctx.gpr[8] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880A9AC;
      }
      goto L_0880A994;
    }
L_0880A994:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A9AC;
      }
      goto L_0880A9A8;
    }
L_0880A9A8:
    ctx.gpr[8] = (0u | 0u);
    goto L_0880A9AC;
L_0880A9AC:
    aot_gpr_4 = (0u | 23u);
    { const bool branch_taken = ctx.gpr[20] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880A9D0;
      }
      goto L_0880A9B8;
    }
L_0880A9B8:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(1)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A9D0;
      }
      goto L_0880A9CC;
    }
L_0880A9CC:
    ctx.gpr[8] = (ctx.gpr[21] | 0u);
    goto L_0880A9D0;
L_0880A9D0:
    aot_gpr_4 = (0u | 24u);
    { const bool branch_taken = ctx.gpr[20] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880A9F4;
      }
      goto L_0880A9DC;
    }
L_0880A9DC:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(2)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880A9F4;
      }
      goto L_0880A9F0;
    }
L_0880A9F0:
    ctx.gpr[8] = (aot_gpr_6 | 0u);
    goto L_0880A9F4;
L_0880A9F4:
    aot_gpr_4 = (0u | 25u);
    { const bool branch_taken = ctx.gpr[20] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880AA18;
      }
      goto L_0880AA00;
    }
L_0880AA00:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(3)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AA18;
      }
      goto L_0880AA14;
    }
L_0880AA14:
    ctx.gpr[8] = (ctx.gpr[7] | 0u);
    goto L_0880AA18;
L_0880AA18:
    aot_gpr_4 = (0u | 26u);
    { const bool branch_taken = ctx.gpr[20] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880AA3C;
      }
      goto L_0880AA24;
    }
L_0880AA24:
    aot_gpr_4 = (aot_mem.aot_direct_load8(aot_gpr_16 + static_cast<std::uint32_t>(4)));
    aot_gpr_4 = (0u < aot_gpr_4 ? 1u : 0u);
    aot_gpr_4 = (aot_gpr_4 & 255u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AA3C;
      }
      goto L_0880AA38;
    }
L_0880AA38:
    ctx.gpr[8] = (ctx.gpr[9] | 0u);
    goto L_0880AA3C;
L_0880AA3C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(352)));
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AA70;
      }
      goto L_0880AA48;
    }
L_0880AA48:
    if (ctx.gpr[8] == 0u) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(76)));
        goto L_0880AB0C;
    }
    goto L_0880AA50;
L_0880AA50:
    if (ctx.gpr[8] == ctx.gpr[21]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(76)));
        goto L_0880AB0C;
    }
    goto L_0880AA58;
L_0880AA58:
    if (ctx.gpr[8] == aot_gpr_6) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(76)));
        goto L_0880AB0C;
    }
    goto L_0880AA60;
L_0880AA60:
    if (ctx.gpr[8] == ctx.gpr[7]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(76)));
        goto L_0880AB0C;
    }
    goto L_0880AA68;
L_0880AA68:
    if (ctx.gpr[8] == ctx.gpr[9]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(76)));
        goto L_0880AB0C;
    }
    goto L_0880AA70;
L_0880AA70:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16324)));
    ctx.gpr[8] = (0u | 0u);
    ctx.gpr[9] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16300)));
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-1));
    goto L_0880AA80;
L_0880AA80:
    aot_gpr_6 = (ctx.gpr[7] | 0u);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_6) >= 0;
      if (branch_taken) {
          goto L_0880AA90;
      }
      goto L_0880AA8C;
    }
L_0880AA8C:
    aot_gpr_6 = (ctx.gpr[7] + static_cast<std::uint32_t>(7));
    goto L_0880AA90;
L_0880AA90:
    aot_gpr_6 = (aot_gpr_6 << 2u);
    aot_gpr_4 = (aot_gpr_6 + ctx.gpr[23]);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = aot_gpr_4 != ctx.gpr[10];
      if (branch_taken) {
          goto L_0880AAB4;
      }
      goto L_0880AAA4;
    }
L_0880AAA4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(84)));
    aot_gpr_4 = (aot_gpr_6 + aot_gpr_4);
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[30] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
      if (branch_taken) {
          goto L_0880AAD4;
      }
      goto L_0880AAB4;
    }
L_0880AAB4:
    { const bool branch_taken = ctx.gpr[9] != ctx.gpr[10];
      if (branch_taken) {
          goto L_0880AAC4;
      }
      goto L_0880AABC;
    }
L_0880AABC:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[30] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(80)));
      if (branch_taken) {
          goto L_0880AAD4;
      }
      goto L_0880AAC4;
    }
L_0880AAC4:
    ctx.gpr[8] = (ctx.gpr[8] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[8]) < 4 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-1));
      if (branch_taken) {
          goto L_0880AA80;
      }
      goto L_0880AAD4;
    }
L_0880AAD4:
    aot_gpr_4 = (0u | 4u);
    { const bool branch_taken = ctx.gpr[8] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880AAE8;
      }
      goto L_0880AAE0;
    }
L_0880AAE0:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[21] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
      if (branch_taken) {
          goto L_0880AE90;
      }
      goto L_0880AAE8;
    }
L_0880AAE8:
    aot_gpr_4 = (ctx.gpr[10] + ctx.gpr[10]);
    aot_gpr_4 = (ctx.gpr[10] + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16328)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (ctx.gpr[30] + static_cast<std::uint32_t>(64));
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-64));
      if (branch_taken) {
          goto L_0880AB70;
      }
      goto L_0880AB0C;
    }
L_0880AB0C:
    aot_gpr_5 = (ctx.gpr[8] << 2u);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(72)));
    ctx.gpr[9] = (aot_gpr_5 + aot_gpr_4);
    ctx.gpr[9] = (aot_mem.aot_direct_load32(ctx.gpr[9] + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (aot_gpr_5 + aot_gpr_6);
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16352)));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(6028));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[10]) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[7] + static_cast<std::uint32_t>(0)));
      if (branch_taken) {
          goto L_0880AB48;
      }
      goto L_0880AB34;
    }
L_0880AB34:
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[8]) < 5 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[7]) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
      if (branch_taken) {
          goto L_0880AB48;
      }
      goto L_0880AB40;
    }
L_0880AB40:
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880AB50;
      }
      goto L_0880AB48;
    }
L_0880AB48:
    // A sample-cache miss aborts the batch below. It must not discard stop
    // commands for the channels that this loop has not visited yet.
    vcs::finish_aborted_audio_stops(
        aot_mem.aot_direct_load32(0x08BB4AA8u),
        aot_mem.aot_direct_load32(0x08BB4AACu), ctx.gpr[20]);
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479), static_cast<std::uint8_t>(0u));
      if (branch_taken) {
          goto L_0880AEC0;
      }
      goto L_0880AB50;
    }
L_0880AB50:
    aot_gpr_4 = (ctx.gpr[10] + ctx.gpr[10]);
    aot_gpr_4 = (ctx.gpr[10] + aot_gpr_4);
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16328)));
    aot_gpr_4 = (aot_gpr_4 << 2u);
    aot_gpr_4 = (aot_gpr_5 + aot_gpr_4);
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    aot_gpr_6 = (ctx.gpr[9] + static_cast<std::uint32_t>(64));
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-64));
    goto L_0880AB70;
L_0880AB70:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[30] = (aot_gpr_6 | 0u);
      if (branch_taken) {
          goto L_0880ABAC;
      }
      goto L_0880AB78;
    }
L_0880AB78:
    aot_gpr_4 = (ctx.gpr[10] | 0u);
    aot_gpr_31 = (0x0880AB84u);
    aot_gpr_5 = (ctx.gpr[11] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0096.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 623u, 0x0880AB84u, 0x08986AA4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0096_entry(rt, ctx, 691u, aot_mem);
#else
        recomp_unit_0096_entry(rt, ctx, 691u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0096_entry, 96u, 691u, 0x08986AA4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AB84u) goto L_0880AB84;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AB84:
    ctx.gpr[10] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16328)));
    aot_gpr_5 = (ctx.gpr[10] + ctx.gpr[10]);
    aot_gpr_5 = (ctx.gpr[10] + aot_gpr_5);
    aot_gpr_5 = (aot_gpr_5 << 2u);
    ctx.gpr[11] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(128)));
    aot_gpr_4 = (aot_gpr_4 + aot_gpr_5);
    ctx.gpr[7] = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(4)));
    ctx.gpr[30] = (ctx.gpr[2] | 0u);
    ctx.gpr[7] = (ctx.gpr[7] + static_cast<std::uint32_t>(-64));
    goto L_0880ABAC;
L_0880ABAC:
    aot_gpr_16 = (ctx.gpr[7] | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(68)));
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    aot_gpr_6 = (ctx.gpr[10] | 0u);
    ctx.gpr[7] = (ctx.gpr[11] | 0u);
    ctx.gpr[8] = (ctx.gpr[30] | 0u);
    aot_gpr_31 = (0x0880ABCCu);
    ctx.gpr[9] = (aot_gpr_16 | 0u);
    goto L_08809D94;
L_0880ABCC:
    ctx.gpr[10] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (0u | 27u);
    { const bool branch_taken = ctx.gpr[10] == aot_gpr_4;
    ctx.gpr[7] = (0u | 1u);
      if (branch_taken) {
          goto L_0880AC08;
      }
      goto L_0880ABDC;
    }
L_0880ABDC:
    aot_gpr_4 = (0u | 26u);
    { const bool branch_taken = ctx.gpr[10] == aot_gpr_4;
    aot_gpr_4 = (0u | 265u);
      if (branch_taken) {
          goto L_0880AC08;
      }
      goto L_0880ABE8;
    }
L_0880ABE8:
    { const bool branch_taken = ctx.gpr[10] == aot_gpr_4;
    aot_gpr_4 = (0u | 249u);
      if (branch_taken) {
          goto L_0880AC08;
      }
      goto L_0880ABF0;
    }
L_0880ABF0:
    { const bool branch_taken = ctx.gpr[10] == aot_gpr_4;
    aot_gpr_4 = (0u | 250u);
      if (branch_taken) {
          goto L_0880AC08;
      }
      goto L_0880ABF8;
    }
L_0880ABF8:
    { const bool branch_taken = ctx.gpr[10] == aot_gpr_4;
    aot_gpr_4 = (0u | 16u);
      if (branch_taken) {
          goto L_0880AC08;
      }
      goto L_0880AC00;
    }
L_0880AC00:
    { const bool branch_taken = ctx.gpr[10] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880AC0C;
      }
      goto L_0880AC08;
    }
L_0880AC08:
    ctx.gpr[7] = (0u | 0u);
    goto L_0880AC0C;
L_0880AC0C:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (ctx.gpr[30] | 0u);
    aot_gpr_31 = (0x0880AC1Cu);
    aot_gpr_6 = (aot_gpr_16 | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 55u, 0x08B64550u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AC1Cu) goto L_0880AC1C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AC1C:
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(464)));
    ctx.gpr[21] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
    ctx.gpr[17] = (0u + static_cast<std::uint32_t>(-1));
    { const bool branch_taken = aot_gpr_6 == ctx.gpr[17];
    ctx.gpr[18] = (0u | 127u);
      if (branch_taken) {
          goto L_0880AC78;
      }
      goto L_0880AC30;
    }
L_0880AC30:
    aot_gpr_5 = (aot_gpr_6 << 12u);
    aot_gpr_4 = (0u | 44000u);
    aot_gpr_5 = (0u + aot_gpr_5);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_5); const std::int32_t divisor = static_cast<std::int32_t>(aot_gpr_4); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(64)));
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    aot_gpr_16 = (ctx.lo);
    aot_gpr_31 = (0x0880AC54u);
    ctx.gpr[7] = (aot_gpr_16 | 0u);
    goto L_08809D94;
L_0880AC54:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_31 = (0x0880AC60u);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 51u, 0x08B64500u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AC60u) goto L_0880AC60;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AC60:
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_16) < 4097 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880AC78;
      }
      goto L_0880AC6C;
    }
L_0880AC6C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(60)));
    aot_gpr_31 = (0x0880AC78u);
    aot_gpr_5 = (aot_gpr_16 | 0u);
    goto L_08809D94;
L_0880AC78:
    ctx.gpr[8] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(688)));
    if (ctx.gpr[8] == ctx.gpr[17]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
        goto L_0880AE40;
    }
    goto L_0880AC84;
L_0880AC84:
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[22] + static_cast<std::uint32_t>(576)));
    if (ctx.gpr[7] == ctx.gpr[17]) {
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
        goto L_0880AE40;
    }
    goto L_0880AC90;
L_0880AC90:
    aot_gpr_4 = (0u | 63u);
    { const bool branch_taken = ctx.gpr[7] != aot_gpr_4;
    ctx.gpr[17] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16288)));
      if (branch_taken) {
          goto L_0880ACA8;
      }
      goto L_0880AC9C;
    }
L_0880AC9C:
    ctx.gpr[9] = (ctx.gpr[8] << 6u);
    { const bool branch_taken = 0u == 0u;
    aot_gpr_16 = (ctx.gpr[8] << 6u);
      if (branch_taken) {
          goto L_0880ACC4;
      }
      goto L_0880ACA8;
    }
L_0880ACA8:
    aot_gpr_4 = (ctx.gpr[18] - ctx.gpr[7]);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[8])) * static_cast<std::int64_t>(static_cast<std::int32_t>(aot_gpr_4)); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    ctx.gpr[9] = (ctx.lo);
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[8])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[7])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_16 = (ctx.lo);
    goto L_0880ACC4;
L_0880ACC4:
    { const std::int32_t dividend = static_cast<std::int32_t>(ctx.gpr[9]); const std::int32_t divisor = static_cast<std::int32_t>(ctx.gpr[18]); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_6 = (ctx.gpr[7] | 0u);
    ctx.gpr[7] = (ctx.gpr[8] | 0u);
    ctx.gpr[8] = (ctx.gpr[17] | 0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(56)));
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    ctx.gpr[17] = (ctx.lo);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_16); const std::int32_t divisor = static_cast<std::int32_t>(ctx.gpr[18]); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    ctx.gpr[9] = (ctx.gpr[17] | 0u);
    aot_gpr_16 = (ctx.lo);
    aot_gpr_31 = (0x0880AD04u);
    ctx.gpr[10] = (aot_gpr_16 | 0u);
    goto L_08809D94;
L_0880AD04:
    ctx.gpr[10] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    ctx.gpr[8] = (ctx.gpr[17] << 1u);
    ctx.gpr[7] = (aot_mem.aot_direct_load32(ctx.gpr[10] + static_cast<std::uint32_t>(0)));
    ctx.gpr[17] = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-16288)));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[8]) < static_cast<std::int32_t>(ctx.gpr[7]) ? 1u : 0u);
    if (aot_gpr_4 == 0u) {
    ctx.gpr[8] = (ctx.gpr[7] | 0u);
        goto L_0880AD20;
    }
    goto L_0880AD20;
L_0880AD20:
    aot_gpr_6 = (aot_mem.aot_direct_load32(ctx.gpr[10] + static_cast<std::uint32_t>(0)));
    ctx.gpr[7] = (aot_gpr_16 << 1u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[7]) < static_cast<std::int32_t>(aot_gpr_6) ? 1u : 0u);
    if (aot_gpr_4 == 0u) {
    ctx.gpr[7] = (aot_gpr_6 | 0u);
        goto L_0880AD34;
    }
    goto L_0880AD34;
L_0880AD34:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[8])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_6 = (0u | 3u);
    aot_gpr_4 = (ctx.lo);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_4); const std::int32_t divisor = static_cast<std::int32_t>(aot_gpr_6); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_4 = (0u | 4096u);
    ctx.gpr[19] = (ctx.lo);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(ctx.gpr[19]) ? 1u : 0u);
    if (aot_gpr_4 != 0u) {
    ctx.gpr[19] = (0u | 4096u);
        goto L_0880AD60;
    }
    goto L_0880AD60;
L_0880AD60:
    { const std::int64_t product = static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[17])) * static_cast<std::int64_t>(static_cast<std::int32_t>(ctx.gpr[7])); ctx.lo = static_cast<std::uint32_t>(product); ctx.hi = static_cast<std::uint32_t>(static_cast<std::uint64_t>(product) >> 32u); }
    aot_gpr_4 = (ctx.lo);
    { const std::int32_t dividend = static_cast<std::int32_t>(aot_gpr_4); const std::int32_t divisor = static_cast<std::int32_t>(aot_gpr_6); if (divisor == 0) { ctx.lo = dividend >= 0 ? 0xFFFFFFFFu : 1u; ctx.hi = static_cast<std::uint32_t>(dividend); } else if (dividend == static_cast<std::int32_t>(0x80000000u) && divisor == -1) { ctx.lo = 0x80000000u; ctx.hi = 0u; } else { ctx.lo = static_cast<std::uint32_t>(dividend / divisor); ctx.hi = static_cast<std::uint32_t>(dividend % divisor); } }
    aot_gpr_4 = (0u | 4096u);
    ctx.gpr[18] = (ctx.gpr[19] | 0u);
    ctx.gpr[17] = (ctx.lo);
    aot_gpr_4 = (static_cast<std::int32_t>(aot_gpr_4) < static_cast<std::int32_t>(ctx.gpr[17]) ? 1u : 0u);
    if (aot_gpr_4 != 0u) {
    ctx.gpr[17] = (0u | 4096u);
        goto L_0880AD8C;
    }
    goto L_0880AD8C;
L_0880AD8C:
    aot_gpr_31 = (0x0880AD94u);
    aot_gpr_16 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0033.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 649u, 0x0880AD94u, 0x0888A208u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0033_entry(rt, ctx, 600u, aot_mem);
#else
        recomp_unit_0033_entry(rt, ctx, 600u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0033_entry, 33u, 600u, 0x0888A208u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AD94u) goto L_0880AD94;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AD94:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880ADF8;
      }
      goto L_0880AD9C;
    }
L_0880AD9C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(7688)));
    { const bool branch_taken = aot_gpr_4 != 0u;
      if (branch_taken) {
          goto L_0880ADC0;
      }
      goto L_0880ADA8;
    }
L_0880ADA8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880ADB8u);
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(7688), aot_gpr_5);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0010.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 652u, 0x0880ADB8u, 0x0882D794u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0010_entry(rt, ctx, 286u, aot_mem);
#else
        recomp_unit_0010_entry(rt, ctx, 286u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0010_entry, 10u, 286u, 0x0882D794u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880ADB8u) goto L_0880ADB8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880ADB8:
    aot_gpr_31 = (0x0880ADC0u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(52)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60680, 215u, 68u, 0x08B60680u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_16, ctx.gpr[17], ctx.gpr[18], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 68u, 0x08B60680u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880ADC0u) goto L_0880ADC0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880ADC0:
    aot_gpr_31 = (0x0880ADC8u);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(36)));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0010.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 654u, 0x0880ADC8u, 0x0882E9B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0010_entry(rt, ctx, 611u, aot_mem);
#else
        recomp_unit_0010_entry(rt, ctx, 611u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0010_entry, 10u, 611u, 0x0882E9B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880ADC8u) goto L_0880ADC8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880ADC8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880ADF8;
      }
      goto L_0880ADD0;
    }
L_0880ADD0:
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[19]);
    aot_fpr_13 = std::bit_cast<float>(ctx.gpr[17]);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    aot_fpr_13 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_13)));
    { const float fs = aot_fpr_12; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_12 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_12 = fs * ft; }
    { const float fs = aot_fpr_13; const float ft = aot_fpr_20; if ((std::isinf(fs) && ft == 0.0f) || (std::isinf(ft) && fs == 0.0f)) aot_fpr_13 = std::bit_cast<float>(0x7FC00000u); else aot_fpr_13 = fs * ft; }
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_12));
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_13));
    ctx.gpr[18] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_16 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    goto L_0880ADF8;
L_0880ADF8:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(48)));
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880AE08u);
    aot_gpr_6 = (aot_gpr_16 | 0u);
    goto L_08809D94;
L_0880AE08:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_6 = (aot_gpr_16 | 0u);
    ctx.gpr[7] = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880AE20u);
    ctx.gpr[8] = (aot_gpr_16 | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 47u, 0x08B6449Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AE20u) goto L_0880AE20;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AE20:
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    aot_gpr_5 = (0u | 15u);
    ctx.gpr[7] = (0u | 0u);
    ctx.gpr[8] = (0u | 0u);
    aot_gpr_6 = (16384u << 16u);
    aot_gpr_31 = (0x0880AE3Cu);
    ctx.gpr[9] = (4096u << 16u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 59u, 0x08B645ACu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AE3Cu) goto L_0880AE3C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AE3C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(24)));
    goto L_0880AE40;
L_0880AE40:
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(16)));
    aot_gpr_4 = (aot_gpr_29 + aot_gpr_4);
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(0)));
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (~(aot_gpr_4 | 0u));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_6);
    aot_gpr_6 = (256u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), aot_gpr_4);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_6);
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(28)));
    aot_mem.aot_direct_store32(aot_gpr_5 + static_cast<std::uint32_t>(0), aot_gpr_4);
    aot_gpr_4 = (aot_gpr_4 & aot_gpr_6);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AE90;
      }
      goto L_0880AE7C;
    }
L_0880AE7C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(44)));
    aot_gpr_31 = (0x0880AE88u);
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    goto L_08809D94;
L_0880AE88:
    aot_gpr_31 = (0x0880AE90u);
    aot_gpr_4 = (ctx.gpr[20] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0216_entry, 216u, 41u, 0x08B64424u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AE90u) goto L_0880AE90;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AE90:
    // This channel has been visited. Do not report its old start request as
    // still pending while the audio thread works on the rest of the batch.
    {
        const auto word = ctx.gpr[20] < 24u ? 0x08BB4AA0u : 0x08BB4AA4u;
        const auto bit = 1u << (ctx.gpr[20] < 24u ? ctx.gpr[20] : ctx.gpr[20] - 24u);
        aot_mem.aot_direct_store32(word, vcs::cancel_pending_audio_start(
            aot_mem.aot_direct_load32(word), bit));
    }
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(32)));
    ctx.gpr[20] = (ctx.gpr[21] | 0u);
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(4));
    ctx.gpr[22] = (ctx.gpr[22] + static_cast<std::uint32_t>(4));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[20]) < 28 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(32), aot_gpr_4);
      if (branch_taken) {
          goto L_0880A8AC;
      }
      goto L_0880AEAC;
    }
L_0880AEAC:
    { const bool branch_taken = 0u == 0u;
    aot_mem.aot_direct_store8(ctx.gpr[28] + static_cast<std::uint32_t>(-32479), static_cast<std::uint8_t>(0u));
      if (branch_taken) {
          goto L_0880AEC0;
      }
      goto L_0880AEB4;
    }
L_0880AEB4:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32452)));
    aot_gpr_4 = (aot_gpr_4 + static_cast<std::uint32_t>(1));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32452), aot_gpr_4);
    goto L_0880AEC0;
L_0880AEC0:
    { std::uint32_t aot_run_words[11]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(92), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_gpr_16 = aot_run_words[1];
      ctx.gpr[17] = aot_run_words[2];
      ctx.gpr[18] = aot_run_words[3];
      ctx.gpr[19] = aot_run_words[4];
      ctx.gpr[20] = aot_run_words[5];
      ctx.gpr[21] = aot_run_words[6];
      ctx.gpr[22] = aot_run_words[7];
      ctx.gpr[23] = aot_run_words[8];
      ctx.gpr[30] = aot_run_words[9];
      aot_gpr_31 = aot_run_words[10];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(144));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880AEF4:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_31);
    aot_gpr_31 = (0x0880AF04u);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(7696));
    goto L_08809DBC;
L_0880AF04:
    aot_gpr_31 = (0x0880AF0Cu);
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32388));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08B60680, 215u, 68u, 0x08B60680u>(ctx, &aot_mem, ctx.pc, ctx.gpr[1], ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_16, ctx.gpr[17], ctx.gpr[18], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0215_entry, 215u, 68u, 0x08B60680u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AF0Cu) goto L_0880AF0C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AF0C:
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_mem.aot_direct_load32(ctx.gpr[28] + static_cast<std::uint32_t>(-32472)));
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_gpr_4 = (16448u << 16u);
    aot_fpr_12 = aot_fpr_13 - aot_fpr_12;
    ctx.fpr[14] = std::bit_cast<float>(aot_gpr_4);
    ctx.fpr[14] = aot_fpr_12 / ctx.fpr[14];
    aot_gpr_5 = (16704u << 16u);
    ctx.fpr[15] = std::bit_cast<float>(aot_gpr_5);
    aot_fpr_12 = aot_fpr_12 / ctx.fpr[15];
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32468), std::bit_cast<std::uint32_t>(ctx.fpr[14]));
    aot_mem.aot_direct_store32(ctx.gpr[28] + static_cast<std::uint32_t>(-32464), std::bit_cast<std::uint32_t>(aot_fpr_12));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880AF48:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    { const std::uint32_t aot_run_words[7]{aot_gpr_16, ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], ctx.gpr[20], ctx.gpr[21], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words); }
    aot_gpr_31 = (0x0880AF70u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AF70u) goto L_0880AF70;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AF70:
    ctx.gpr[21] = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32276));
    aot_gpr_31 = (0x0880AF80u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 672u, 0x0880AF80u, 0x08806E68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 430u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 430u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 430u, 0x08806E68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AF80u) goto L_0880AF80;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AF80:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880AF8Cu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10001));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 673u, 0x0880AF8Cu, 0x088070DCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 453u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 453u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 453u, 0x088070DCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AF8Cu) goto L_0880AF8C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AF8C:
    ctx.gpr[20] = (0u | 1u);
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[21]) < 1 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[17] = (2234u << 16u);
      if (branch_taken) {
          goto L_0880B04C;
      }
      goto L_0880AF9C;
    }
L_0880AF9C:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(-32092));
    goto L_0880AFA0;
L_0880AFA0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880AFACu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AFACu) goto L_0880AFAC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AFAC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880AFB8u);
    aot_gpr_5 = (ctx.gpr[20] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AFB8u) goto L_0880AFB8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880AFB8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880AFC8u);
    aot_gpr_6 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 678u, 0x0880AFC8u, 0x08807644u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 508u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 508u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 508u, 0x08807644u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AFC8u) goto L_0880AFC8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AFC8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880AFD4u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 679u, 0x0880AFD4u, 0x08806B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 375u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 375u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 375u, 0x08806B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880AFD4u) goto L_0880AFD4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880AFD4:
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
    { const bool branch_taken = ctx.gpr[19] == 0u;
      if (branch_taken) {
          goto L_0880AFF8;
      }
      goto L_0880AFE0;
    }
L_0880AFE0:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(15532)));
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[20]) < 2 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
      if (branch_taken) {
          goto L_0880B010;
      }
      goto L_0880AFF0;
    }
L_0880AFF0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B024;
      }
      goto L_0880AFF8;
    }
L_0880AFF8:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B008u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16896));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 683u, 0x0880B008u, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B008u) goto L_0880B008;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B008:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B060;
      }
      goto L_0880B010;
    }
L_0880B010:
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_31 = (0x0880B01Cu);
    aot_gpr_4 = (ctx.gpr[18] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 427u, 0x08B55960u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B01Cu) goto L_0880B01C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B01C:
    aot_gpr_4 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(15532)));
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_4 + static_cast<std::uint32_t>(8)));
    goto L_0880B024;
L_0880B024:
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_31 = (0x0880B030u);
    aot_gpr_4 = (ctx.gpr[19] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 427u, 0x08B55960u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B030u) goto L_0880B030;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B030:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B03Cu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B03Cu) goto L_0880B03C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B03C:
    ctx.gpr[20] = (ctx.gpr[20] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[21]) < static_cast<std::int32_t>(ctx.gpr[20]) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880AFA0;
      }
      goto L_0880B04C;
    }
L_0880B04C:
    aot_gpr_5 = (aot_mem.aot_direct_load32(ctx.gpr[17] + static_cast<std::uint32_t>(15532)));
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32088));
    aot_gpr_31 = (0x0880B05Cu);
    aot_gpr_5 = (aot_mem.aot_direct_load32(aot_gpr_5 + static_cast<std::uint32_t>(8)));
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 427u, 0x08B55960u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B05Cu) goto L_0880B05C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B05C:
    ctx.gpr[2] = (0u | 0u);
    goto L_0880B060;
L_0880B060:
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
L_0880B084:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_gpr_6 = (16672u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_16);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    { const std::uint32_t aot_run_words[4]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_run_words); }
    aot_gpr_31 = (0x0880B0B0u);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 693u, 0x0880B0B0u, 0x0889D684u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 300u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 300u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 300u, 0x0889D684u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B0B0u) goto L_0880B0B0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B0B0:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_4 = (0u | 10u);
    ctx.gpr[18] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const bool branch_taken = ctx.gpr[18] != aot_gpr_4;
    ctx.gpr[17] = (0u | 1u);
      if (branch_taken) {
          goto L_0880B104;
      }
      goto L_0880B0C4;
    }
L_0880B0C4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B0D0u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 695u, 0x0880B0D0u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B0D0u) goto L_0880B0D0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B0D0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B0DCu);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880692C, 0u, 337u, 0x0880692Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 337u, 0x0880692Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B0DCu) goto L_0880B0DC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B0DC:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880B1E0;
      }
      goto L_0880B0E4;
    }
L_0880B0E4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B0F0u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806A88, 0u, 360u, 0x08806A88u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 360u, 0x08806A88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B0F0u) goto L_0880B0F0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B0F0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B0FCu);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B0FCu) goto L_0880B0FC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B0FC:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880B1EC;
      }
      goto L_0880B104;
    }
L_0880B104:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B114u);
    aot_gpr_6 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 701u, 0x0880B114u, 0x0889D50Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 277u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 277u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 277u, 0x0889D50Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B114u) goto L_0880B114;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B114:
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < 2 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_4 = (ctx.gpr[2] | 0u);
      if (branch_taken) {
          goto L_0880B12C;
      }
      goto L_0880B120;
    }
L_0880B120:
    aot_gpr_5 = (static_cast<std::int32_t>(ctx.gpr[18]) < 37 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 != 0u;
      if (branch_taken) {
          goto L_0880B148;
      }
      goto L_0880B12C;
    }
L_0880B12C:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_4);
    aot_gpr_6 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 2u);
    aot_gpr_31 = (0x0880B144u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(16940));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 704u, 0x0880B144u, 0x0889CFFCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 201u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 201u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 201u, 0x0889CFFCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B144u) goto L_0880B144;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B144:
    aot_gpr_4 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    goto L_0880B148;
L_0880B148:
    ctx.gpr[19] = (aot_gpr_4 | 0u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    aot_gpr_31 = (0x0880B158u);
    aot_gpr_6 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0213.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 706u, 0x0880B158u, 0x08B58090u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0213_entry(rt, ctx, 16u, aot_mem);
#else
        recomp_unit_0213_entry(rt, ctx, 16u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0213_entry, 213u, 16u, 0x08B58090u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B158u) goto L_0880B158;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B158:
    ctx.gpr[18] = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    { const bool branch_taken = ctx.gpr[19] == ctx.gpr[18];
    aot_gpr_4 = (ctx.gpr[2] | 0u);
      if (branch_taken) {
          goto L_0880B1E0;
      }
      goto L_0880B164;
    }
L_0880B164:
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[18] + static_cast<std::uint32_t>(0))))));
    aot_gpr_6 = (2234u << 16u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(14520));
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(1));
    ctx.gpr[7] = (aot_gpr_5 & 255u);
    ctx.gpr[7] = (aot_gpr_6 + ctx.gpr[7]);
    ctx.gpr[7] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[7] + static_cast<std::uint32_t>(0))))));
    ctx.gpr[7] = (ctx.gpr[7] & 8u);
    { const bool branch_taken = ctx.gpr[7] == 0u;
      if (branch_taken) {
          goto L_0880B1B0;
      }
      goto L_0880B18C;
    }
L_0880B18C:
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
    goto L_0880B190;
L_0880B190:
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), ctx.gpr[18]);
    aot_gpr_5 = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[18] + static_cast<std::uint32_t>(0))))));
    ctx.gpr[7] = (aot_gpr_5 & 255u);
    ctx.gpr[7] = (aot_gpr_6 + ctx.gpr[7]);
    ctx.gpr[7] = (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(aot_mem.aot_direct_load8(ctx.gpr[7] + static_cast<std::uint32_t>(0))))));
    ctx.gpr[7] = (ctx.gpr[7] & 8u);
    { const bool branch_taken = ctx.gpr[7] != 0u;
    ctx.gpr[18] = (ctx.gpr[18] + static_cast<std::uint32_t>(1));
      if (branch_taken) {
          goto L_0880B190;
      }
      goto L_0880B1B0;
    }
L_0880B1B0:
    { const bool branch_taken = aot_gpr_5 != 0u;
      if (branch_taken) {
          goto L_0880B1E0;
      }
      goto L_0880B1B8;
    }
L_0880B1B8:
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    { const bool branch_taken = static_cast<std::int32_t>(aot_gpr_4) >= 0;
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
      if (branch_taken) {
          goto L_0880B1D0;
      }
      goto L_0880B1C4;
    }
L_0880B1C4:
    aot_gpr_4 = (20352u << 16u);
    aot_fpr_13 = std::bit_cast<float>(aot_gpr_4);
    aot_fpr_12 = aot_fpr_12 + aot_fpr_13;
    goto L_0880B1D0;
L_0880B1D0:
    aot_gpr_31 = (0x0880B1D8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B1D8u) goto L_0880B1D8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B1D8:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880B1EC;
      }
      goto L_0880B1E0;
    }
L_0880B1E0:
    aot_gpr_31 = (0x0880B1E8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 716u, 0x0880B1E8u, 0x08806DA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 424u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 424u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 424u, 0x08806DA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B1E8u) goto L_0880B1E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B1E8:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    goto L_0880B1EC;
L_0880B1EC:
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
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
L_0880B208:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_gpr_6 = (16256u << 16u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B22Cu);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 719u, 0x0880B22Cu, 0x0889D684u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 300u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 300u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 300u, 0x0889D684u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B22Cu) goto L_0880B22C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B22C:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B240u);
    ctx.gpr[17] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 720u, 0x0880B240u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B240u) goto L_0880B240;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B240:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B24Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 721u, 0x0880B24Cu, 0x08806984u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 344u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 344u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 344u, 0x08806984u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B24Cu) goto L_0880B24C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B24C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880B25C;
      }
      goto L_0880B254;
    }
L_0880B254:
    { const bool branch_taken = ctx.gpr[17] != 0u;
      if (branch_taken) {
          goto L_0880B270;
      }
      goto L_0880B25C;
    }
L_0880B25C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B268u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B268u) goto L_0880B268;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B268:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B294;
      }
      goto L_0880B270;
    }
L_0880B270:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B27Cu);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 726u, 0x0880B27Cu, 0x0889D184u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 221u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 221u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 221u, 0x0889D184u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B27Cu) goto L_0880B27C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B27C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B288u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B288u) goto L_0880B288;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B288:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B294u);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 728u, 0x0880B294u, 0x08807904u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 546u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 546u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 546u, 0x08807904u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B294u) goto L_0880B294;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B294:
    aot_gpr_31 = (0x0880B29Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 729u, 0x0880B29Cu, 0x08807890u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 538u, 0x08807890u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B29Cu) goto L_0880B29C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B29C:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B2B0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B2D0u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 731u, 0x0880B2D0u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B2D0u) goto L_0880B2D0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B2D0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B2DCu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 732u, 0x0880B2DCu, 0x08807294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 466u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 466u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 466u, 0x08807294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B2DCu) goto L_0880B2DC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B2DC:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880B2F4;
      }
      goto L_0880B2E4;
    }
L_0880B2E4:
    aot_gpr_31 = (0x0880B2ECu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 734u, 0x0880B2ECu, 0x08806DA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 424u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 424u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 424u, 0x08806DA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B2ECu) goto L_0880B2EC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B2EC:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880B308;
      }
      goto L_0880B2F4;
    }
L_0880B2F4:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32084));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B304u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 736u, 0x0880B304u, 0x0889D6DCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 305u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 305u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 305u, 0x0889D6DCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B304u) goto L_0880B304;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B304:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    goto L_0880B308;
L_0880B308:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B31C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    { const std::uint32_t aot_run_words[4]{ctx.gpr[17], ctx.gpr[18], ctx.gpr[19], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_gpr_31 = (0x0880B340u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B340u) goto L_0880B340;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B340:
    ctx.gpr[19] = (ctx.gpr[2] | 0u);
    ctx.gpr[18] = (0u | 1u);
    ctx.gpr[17] = (0u | 5u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B35Cu);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 740u, 0x0880B35Cu, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B35Cu) goto L_0880B35C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B35C:
    { const bool branch_taken = ctx.gpr[19] == 0u;
      if (branch_taken) {
          goto L_0880B37C;
      }
      goto L_0880B364;
    }
L_0880B364:
    { const bool branch_taken = ctx.gpr[19] == ctx.gpr[17];
    aot_gpr_6 = (2231u << 16u);
      if (branch_taken) {
          goto L_0880B37C;
      }
      goto L_0880B36C;
    }
L_0880B36C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 2u);
    aot_gpr_31 = (0x0880B37Cu);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(16960));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 743u, 0x0880B37Cu, 0x0889CFFCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 201u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 201u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 201u, 0x0889CFFCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B37Cu) goto L_0880B37C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B37C:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32084));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B38Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 744u, 0x0880B38Cu, 0x0889D6DCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 305u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 305u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 305u, 0x0889D6DCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B38Cu) goto L_0880B38C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B38C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880B3A4;
      }
      goto L_0880B394;
    }
L_0880B394:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B3A4u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(16984));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 746u, 0x0880B3A4u, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B3A4u) goto L_0880B3A4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B3A4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B3B0u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B3B0u) goto L_0880B3B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B3B0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B3BCu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 748u, 0x0880B3BCu, 0x08807510u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 492u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 492u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 492u, 0x08807510u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B3BCu) goto L_0880B3BC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B3BC:
    ctx.gpr[2] = (ctx.gpr[18] | 0u);
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
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
L_0880B3DC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-112));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(96), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(100), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(104), aot_gpr_31);
    aot_gpr_31 = (0x0880B3F8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B3F8u) goto L_0880B3F8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B3F8:
    aot_gpr_4 = (0u | 6u);
    { const bool branch_taken = ctx.gpr[2] != aot_gpr_4;
      if (branch_taken) {
          goto L_0880B418;
      }
      goto L_0880B404;
    }
L_0880B404:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B410u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B410u) goto L_0880B410;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B410:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B4AC;
      }
      goto L_0880B418;
    }
L_0880B418:
    aot_gpr_6 = (16256u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_6);
    aot_gpr_31 = (0x0880B42Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 754u, 0x0880B42Cu, 0x0889D684u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 300u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 300u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 300u, 0x0889D684u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B42Cu) goto L_0880B42C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B42C:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    ctx.gpr[17] = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    { const bool branch_taken = static_cast<std::int32_t>(ctx.gpr[17]) >= 0;
    aot_gpr_6 = (2231u << 16u);
      if (branch_taken) {
          goto L_0880B44C;
      }
      goto L_0880B43C;
    }
L_0880B43C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B44Cu);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(17020));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 756u, 0x0880B44Cu, 0x0889CFFCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 201u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 201u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 201u, 0x0889CFFCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B44Cu) goto L_0880B44C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B44C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880B45Cu);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0120.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 757u, 0x0880B45Cu, 0x089E69E8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0120_entry(rt, ctx, 547u, aot_mem);
#else
        recomp_unit_0120_entry(rt, ctx, 547u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0120_entry, 120u, 547u, 0x089E69E8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B45Cu) goto L_0880B45C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B45C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880B474;
      }
      goto L_0880B464;
    }
L_0880B464:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32072));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B474u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 759u, 0x0880B474u, 0x0889CFFCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 201u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 201u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 201u, 0x0889CFFCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B474u) goto L_0880B474;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B474:
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32056));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B484u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0120.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 760u, 0x0880B484u, 0x089E6F54u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0120_entry(rt, ctx, 622u, aot_mem);
#else
        recomp_unit_0120_entry(rt, ctx, 622u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0120_entry, 120u, 622u, 0x089E6F54u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B484u) goto L_0880B484;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B484:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B490u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B490u) goto L_0880B490;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B490:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880B4AC;
      }
      goto L_0880B498;
    }
L_0880B498:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880B4ACu);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(17048));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 763u, 0x0880B4ACu, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B4ACu) goto L_0880B4AC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B4AC:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(96), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(112));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B4C0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B4D8u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 765u, 0x0880B4D8u, 0x08807344u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 478u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 478u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 478u, 0x08807344u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B4D8u) goto L_0880B4D8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B4D8:
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32052));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B4E8u);
    aot_gpr_6 = (0u | 6u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 766u, 0x0880B4E8u, 0x08806DE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 426u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 426u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 426u, 0x08806DE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B4E8u) goto L_0880B4E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B4E8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B4F4u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 767u, 0x0880B4F4u, 0x08807148u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 456u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 456u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 456u, 0x08807148u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B4F4u) goto L_0880B4F4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B4F4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B500u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B500u) goto L_0880B500;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B500:
    ctx.gpr[2] = (0u < ctx.gpr[2] ? 1u : 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B514:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B528u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    goto L_0880B3DC;
L_0880B528:
    aot_gpr_31 = (0x0880B530u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_0880B4C0;
L_0880B530:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880B544;
      }
      goto L_0880B538;
    }
L_0880B538:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B544u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B544u) goto L_0880B544;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B544:
    ctx.gpr[2] = (0u | 1u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B558:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 2u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B574u);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 775u, 0x0880B574u, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B574u) goto L_0880B574;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B574:
    aot_gpr_31 = (0x0880B57Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_0880B3DC;
L_0880B57C:
    aot_gpr_31 = (0x0880B584u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    goto L_0880B4C0;
L_0880B584:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880B5A4;
      }
      goto L_0880B58C;
    }
L_0880B58C:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B59Cu);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(17100));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 779u, 0x0880B59Cu, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B59Cu) goto L_0880B59C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B59C:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B5B0;
      }
      goto L_0880B5A4;
    }
L_0880B5A4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B5B0u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-3));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B5B0u) goto L_0880B5B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B5B0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B5BCu);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B5BCu) goto L_0880B5BC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B5BC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B5C8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880692C, 0u, 337u, 0x0880692Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 337u, 0x0880692Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B5C8u) goto L_0880B5C8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B5C8:
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_gpr_4 = (aot_gpr_16 | 0u);
      if (branch_taken) {
          goto L_0880B600;
      }
      goto L_0880B5D0;
    }
L_0880B5D0:
    aot_gpr_31 = (0x0880B5D8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806A88, 0u, 360u, 0x08806A88u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 360u, 0x08806A88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B5D8u) goto L_0880B5D8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B5D8:
    aot_fpr_12 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(ctx.fpr[0]) || std::isnan(aot_fpr_12)) && ctx.fpr[0] == aot_fpr_12)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
      if (branch_taken) {
          goto L_0880B600;
      }
      goto L_0880B5EC;
    }
L_0880B5EC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B5F8u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10001));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 787u, 0x0880B5F8u, 0x088067C4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 320u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 320u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 320u, 0x088067C4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B5F8u) goto L_0880B5F8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B5F8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B624;
      }
      goto L_0880B600;
    }
L_0880B600:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B60Cu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 789u, 0x0880B60Cu, 0x088075C4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 503u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 503u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 503u, 0x088075C4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B60Cu) goto L_0880B60C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B60C:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880B624;
      }
      goto L_0880B614;
    }
L_0880B614:
    aot_gpr_5 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B624u);
    aot_gpr_5 = (aot_gpr_5 + static_cast<std::uint32_t>(17148));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 791u, 0x0880B624u, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B624u) goto L_0880B624;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B624:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B638:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B658u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 793u, 0x0880B658u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B658u) goto L_0880B658;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B658:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B664u);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 794u, 0x0880B664u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B664u) goto L_0880B664;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B664:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B674u);
    aot_gpr_6 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 795u, 0x0880B674u, 0x088069B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 346u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 346u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 346u, 0x088069B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B674u) goto L_0880B674;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B674:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B680u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08807098, 0u, 451u, 0x08807098u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 451u, 0x08807098u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B680u) goto L_0880B680;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B680:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B698:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B6BCu);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 798u, 0x0880B6BCu, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B6BCu) goto L_0880B6BC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B6BC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B6C8u);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 799u, 0x0880B6C8u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B6C8u) goto L_0880B6C8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B6C8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B6D4u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 800u, 0x0880B6D4u, 0x08807148u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 456u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 456u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 456u, 0x08807148u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B6D4u) goto L_0880B6D4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B6D4:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B6EC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B710u);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 802u, 0x0880B710u, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B710u) goto L_0880B710;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B710:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B71Cu);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 803u, 0x0880B71Cu, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B71Cu) goto L_0880B71C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B71C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B728u);
    aot_gpr_5 = (0u | 3u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 804u, 0x0880B728u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B728u) goto L_0880B728;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B728:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B734u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 805u, 0x0880B734u, 0x08807414u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 486u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 486u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 486u, 0x08807414u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B734u) goto L_0880B734;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B734:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B74C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B760u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 807u, 0x0880B760u, 0x08807814u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 531u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 531u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 531u, 0x08807814u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B760u) goto L_0880B760;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B760:
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[2]);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B770u);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B770u) goto L_0880B770;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B770:
    aot_gpr_31 = (0x0880B778u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 809u, 0x0880B778u, 0x08807804u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 530u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 530u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 530u, 0x08807804u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B778u) goto L_0880B778;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B778:
    aot_fpr_12 = std::bit_cast<float>(ctx.gpr[2]);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B788u);
    aot_fpr_12 = static_cast<float>(static_cast<std::int32_t>(std::bit_cast<std::uint32_t>(aot_fpr_12)));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B788u) goto L_0880B788;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B788:
    ctx.gpr[2] = (0u | 2u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B79C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_fpr_12 = std::bit_cast<float>(0u);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B7B8u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 812u, 0x0880B7B8u, 0x0889D684u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 300u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 300u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 300u, 0x0889D684u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B7B8u) goto L_0880B7B8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B7B8:
    aot_fpr_12 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(ctx.fpr[0]));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B7C8u);
    aot_gpr_5 = (std::bit_cast<std::uint32_t>(aot_fpr_12));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 813u, 0x0880B7C8u, 0x08807824u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 532u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 532u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 532u, 0x08807824u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B7C8u) goto L_0880B7C8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B7C8:
    ctx.gpr[2] = (0u | 0u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B7DC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880B7FCu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 815u, 0x0880B7FCu, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B7FCu) goto L_0880B7FC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B7FC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B808u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B808u) goto L_0880B808;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B808:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B814u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 817u, 0x0880B814u, 0x088068B4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 327u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 327u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 327u, 0x088068B4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B814u) goto L_0880B814;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B814:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B820u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 818u, 0x0880B820u, 0x08806E68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 430u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 430u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 430u, 0x08806E68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B820u) goto L_0880B820;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B820:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B838:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), ctx.gpr[18]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[18] = (0u | 1u);
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_31);
    aot_gpr_31 = (0x0880B860u);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 820u, 0x0880B860u, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B860u) goto L_0880B860;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B860:
    ctx.gpr[17] = (0u | 2u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B870u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B870u) goto L_0880B870;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B870:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B87Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 822u, 0x0880B87Cu, 0x088078B0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 540u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 540u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 540u, 0x088078B0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B87Cu) goto L_0880B87C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B87C:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880B88C;
      }
      goto L_0880B884;
    }
L_0880B884:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880B898;
      }
      goto L_0880B88C;
    }
L_0880B88C:
    aot_gpr_31 = (0x0880B894u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 825u, 0x0880B894u, 0x08806DA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 424u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 424u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 424u, 0x08806DA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B894u) goto L_0880B894;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B894:
    ctx.gpr[2] = (ctx.gpr[18] | 0u);
    goto L_0880B898;
L_0880B898:
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B8B0:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    goto L_0880B8B8;
L_0880B8B8:
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880B8CCu);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 829u, 0x0880B8CCu, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B8CCu) goto L_0880B8CC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B8CC:
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32320));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B8DCu);
    aot_gpr_6 = (0u | 4u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 830u, 0x0880B8DCu, 0x08806DE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 426u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 426u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 426u, 0x08806DE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B8DCu) goto L_0880B8DC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B8DC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B8E8u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10001));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 831u, 0x0880B8E8u, 0x08807148u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 456u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 456u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 456u, 0x08807148u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B8E8u) goto L_0880B8E8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B8E8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B8F4u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B8F4u) goto L_0880B8F4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B8F4:
    aot_gpr_31 = (0x0880B8FCu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 833u, 0x0880B8FCu, 0x08806DA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 424u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 424u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 424u, 0x08806DA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B8FCu) goto L_0880B8FC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B8FC:
    ctx.gpr[2] = (0u | 3u);
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880B910:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-32));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), std::bit_cast<std::uint32_t>(aot_fpr_20));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), std::bit_cast<std::uint32_t>(aot_fpr_22));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(16), aot_gpr_31);
    aot_gpr_31 = (0x0880B934u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806A88, 0u, 360u, 0x08806A88u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 360u, 0x08806A88u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B934u) goto L_0880B934;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B934:
    aot_fpr_22 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(ctx.fpr[0]));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B948u);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 836u, 0x0880B948u, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B948u) goto L_0880B948;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B948:
    aot_fpr_20 = std::bit_cast<float>(0u);
    ctx.fcr31 = (ctx.fcr31 & ~0x00800000u) | (((!(std::isnan(aot_fpr_22) || std::isnan(aot_fpr_20)) && aot_fpr_22 == aot_fpr_20)) ? 0x00800000u : 0u);
    { const bool branch_taken = !((ctx.fcr31 & 0x00800000u) != 0u);
    ctx.gpr[17] = (0u + static_cast<std::uint32_t>(-1));
      if (branch_taken) {
          goto L_0880B970;
      }
      goto L_0880B95C;
    }
L_0880B95C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B968u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B968u) goto L_0880B968;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B968:
    { const bool branch_taken = ctx.gpr[2] == ctx.gpr[17];
      if (branch_taken) {
          goto L_0880B9C0;
      }
      goto L_0880B970;
    }
L_0880B970:
    aot_gpr_4 = (16256u << 16u);
    aot_fpr_12 = std::bit_cast<float>(aot_gpr_4);
    ctx.gpr[17] = (aot_gpr_16 | 0u);
    aot_fpr_20 = aot_fpr_22 + aot_fpr_12;
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880B98Cu);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B98Cu) goto L_0880B98C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B98C:
    aot_fpr_13 = std::bit_cast<float>(ctx.fpu_float_to_word_ct<1u>(aot_fpr_20));
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880B9A0u);
    aot_gpr_6 = (std::bit_cast<std::uint32_t>(aot_fpr_13));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 841u, 0x0880B9A0u, 0x088071ACu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 459u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 459u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 459u, 0x088071ACu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9A0u) goto L_0880B9A0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B9A0:
    aot_gpr_16 = (0u | 2u);
    aot_gpr_4 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880B9B0u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9B0u) goto L_0880B9B0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B9B0:
    if (ctx.gpr[2] == 0u) {
    aot_gpr_16 = (0u | 0u);
        goto L_0880B9FC;
    }
    goto L_0880B9B8;
L_0880B9B8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880B9FC;
      }
      goto L_0880B9C0;
    }
L_0880B9C0:
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32312));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B9D0u);
    aot_gpr_6 = (0u | 6u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 845u, 0x0880B9D0u, 0x08806DE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 426u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 426u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 426u, 0x08806DE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9D0u) goto L_0880B9D0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B9D0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B9DCu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10001));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 846u, 0x0880B9DCu, 0x08807148u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 456u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 456u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 456u, 0x08807148u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9DCu) goto L_0880B9DC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880B9DC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B9E8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9E8u) goto L_0880B9E8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B9E8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880B9F4u);
    aot_fpr_12 = std::bit_cast<float>(std::bit_cast<std::uint32_t>(aot_fpr_20));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806DC0, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, aot_gpr_31, aot_fpr_12); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 425u, 0x08806DC0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880B9F4u) goto L_0880B9F4;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880B9F4:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 3u);
      if (branch_taken) {
          goto L_0880BA00;
      }
      goto L_0880B9FC;
    }
L_0880B9FC:
    ctx.gpr[2] = (aot_gpr_16 | 0u);
    goto L_0880BA00;
L_0880BA00:
    { std::uint32_t aot_run_words[5]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_fpr_20 = std::bit_cast<float>(aot_run_words[0]);
      aot_fpr_22 = std::bit_cast<float>(aot_run_words[1]);
      aot_gpr_16 = aot_run_words[2];
      ctx.gpr[17] = aot_run_words[3];
      aot_gpr_31 = aot_run_words[4];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(32));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BA1C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    { const bool branch_taken = aot_gpr_5 != 0u;
    aot_gpr_16 = (aot_gpr_4 | 0u);
      if (branch_taken) {
          goto L_0880BA38;
      }
      goto L_0880BA30;
    }
L_0880BA30:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (0u | 1u);
      if (branch_taken) {
          goto L_0880BA50;
      }
      goto L_0880BA38;
    }
L_0880BA38:
    aot_gpr_31 = (0x0880BA40u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 854u, 0x0880BA40u, 0x08806DA8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 424u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 424u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 424u, 0x08806DA8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BA40u) goto L_0880BA40;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BA40:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BA4Cu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-2));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 855u, 0x0880BA4Cu, 0x0880673Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 315u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 315u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 315u, 0x0880673Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BA4Cu) goto L_0880BA4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BA4C:
    ctx.gpr[2] = (0u | 2u);
    goto L_0880BA50;
L_0880BA50:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BA60:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_31);
    aot_gpr_31 = (0x0880BA80u);
    aot_gpr_6 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 858u, 0x0880BA80u, 0x0889D50Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 277u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 277u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 277u, 0x0889D50Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BA80u) goto L_0880BA80;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BA80:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 2u);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880BA98u);
    ctx.gpr[7] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 859u, 0x0880BA98u, 0x0889D588u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 284u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 284u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 284u, 0x0889D588u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BA98u) goto L_0880BA98;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BA98:
    aot_gpr_6 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880BAACu);
    ctx.gpr[7] = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 860u, 0x0880BAACu, 0x0889E390u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 513u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 513u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 513u, 0x0889E390u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BAACu) goto L_0880BAAC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BAAC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BAB8u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    goto L_0880BA1C;
L_0880BAB8:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BACC:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_6 = (0u | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880BAECu);
    ctx.gpr[7] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 863u, 0x0880BAECu, 0x0889D588u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 284u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 284u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 284u, 0x0889D588u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BAECu) goto L_0880BAEC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BAEC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BAF8u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 864u, 0x0880BAF8u, 0x0889E124u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 470u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 470u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 470u, 0x0889E124u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BAF8u) goto L_0880BAF8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BAF8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BB04u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    goto L_0880BA1C;
L_0880BB04:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BB14:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_6 = (0u | 0u);
    { const std::uint32_t aot_run_words[3]{ctx.gpr[17], ctx.gpr[18], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_gpr_31 = (0x0880BB3Cu);
    ctx.gpr[7] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 867u, 0x0880BB3Cu, 0x0889D588u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 284u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 284u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 284u, 0x0889D588u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB3Cu) goto L_0880BB3C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BB3C:
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    aot_gpr_31 = (0x0880BB48u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB48u) goto L_0880BB48;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BB48:
    ctx.gpr[17] = (ctx.gpr[2] | 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BB58u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 869u, 0x0880BB58u, 0x0889E124u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 470u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 470u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 470u, 0x0889E124u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB58u) goto L_0880BB58;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BB58:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880BB68;
      }
      goto L_0880BB60;
    }
L_0880BB60:
    aot_gpr_31 = (0x0880BB68u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 871u, 0x0880BB68u, 0x08807890u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 538u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 538u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 538u, 0x08807890u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB68u) goto L_0880BB68;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BB68:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 0u);
    aot_gpr_31 = (0x0880BB78u);
    aot_gpr_6 = (0u + static_cast<std::uint32_t>(-1));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 872u, 0x0880BB78u, 0x08807644u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 508u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 508u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 508u, 0x08807644u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB78u) goto L_0880BB78;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BB78:
    aot_gpr_31 = (0x0880BB80u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BB80u) goto L_0880BB80;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BB80:
    ctx.gpr[2] = (ctx.gpr[2] - ctx.gpr[17]);
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BB9C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), aot_gpr_31);
    aot_gpr_31 = (0x0880BBBCu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 875u, 0x0880BBBCu, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BBBCu) goto L_0880BBBC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BBBC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BBC8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806AE0, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BBC8u) goto L_0880BBC8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BBC8:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880BC04;
      }
      goto L_0880BBD0;
    }
L_0880BBD0:
    aot_gpr_6 = (2231u << 16u);
    ctx.gpr[17] = (ctx.gpr[28] + static_cast<std::uint32_t>(-32044));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 2u);
    ctx.gpr[7] = (0u | 0u);
    aot_gpr_31 = (0x0880BBECu);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(17204));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 878u, 0x0880BBECu, 0x0889D588u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 284u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 284u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 284u, 0x0889D588u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BBECu) goto L_0880BBEC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BBEC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[17] | 0u);
    aot_gpr_31 = (0x0880BBFCu);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 879u, 0x0880BBFCu, 0x0889D1FCu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 229u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 229u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 229u, 0x0889D1FCu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BBFCu) goto L_0880BBFC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BBFC:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880BC14;
      }
      goto L_0880BC04;
    }
L_0880BC04:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BC10u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BC10u) goto L_0880BC10;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BC10:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    goto L_0880BC14;
L_0880BC14:
    { std::uint32_t aot_run_words[3]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      aot_gpr_31 = aot_run_words[2];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BC28:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_gpr_5 = (0u | 1u);
    { const std::uint32_t aot_run_words[3]{ctx.gpr[17], ctx.gpr[18], aot_gpr_31};
      aot_mem.aot_direct_store32_block(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_run_words); }
    aot_gpr_31 = (0x0880BC4Cu);
    aot_gpr_6 = (0u | 5u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 884u, 0x0880BC4Cu, 0x0889D46Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 269u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 269u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 269u, 0x0889D46Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BC4Cu) goto L_0880BC4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BC4C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BC58u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 885u, 0x0880BC58u, 0x0889DB90u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 388u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 388u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 388u, 0x0889DB90u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BC58u) goto L_0880BC58;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BC58:
    ctx.gpr[18] = (ctx.gpr[2] | 0u);
    aot_gpr_6 = (2231u << 16u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880BC70u);
    aot_gpr_6 = (aot_gpr_6 + static_cast<std::uint32_t>(17224));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 886u, 0x0880BC70u, 0x0889D420u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 265u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 265u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 265u, 0x0889D420u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BC70u) goto L_0880BC70;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BC70:
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < 1 ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 != 0u;
    ctx.gpr[17] = (0u | 1u);
      if (branch_taken) {
          goto L_0880BC9C;
      }
      goto L_0880BC7C;
    }
L_0880BC7C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 1u);
    aot_gpr_31 = (0x0880BC8Cu);
    aot_gpr_6 = (ctx.gpr[17] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 888u, 0x0880BC8Cu, 0x088071ACu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 459u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 459u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 459u, 0x088071ACu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BC8Cu) goto L_0880BC8C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BC8C:
    ctx.gpr[17] = (ctx.gpr[17] + static_cast<std::uint32_t>(1));
    aot_gpr_4 = (static_cast<std::int32_t>(ctx.gpr[18]) < static_cast<std::int32_t>(ctx.gpr[17]) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_4 == 0u;
      if (branch_taken) {
          goto L_0880BC7C;
      }
      goto L_0880BC9C;
    }
L_0880BC9C:
    ctx.gpr[2] = (ctx.gpr[18] | 0u);
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BCB8:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880BCD0u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 891u, 0x0880BCD0u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BCD0u) goto L_0880BCD0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BCD0:
    aot_gpr_31 = (0x0880BCD8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BCD8u) goto L_0880BCD8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BCD8:
    aot_gpr_5 = (ctx.gpr[2] + static_cast<std::uint32_t>(-1));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_6 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_31 = (0x0880BCECu);
    ctx.gpr[7] = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 893u, 0x0880BCECu, 0x0880768Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 512u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 512u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 512u, 0x0880768Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BCECu) goto L_0880BCEC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BCEC:
    aot_gpr_5 = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_31 = (0x0880BCF8u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08807098, 0u, 451u, 0x08807098u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 451u, 0x08807098u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BCF8u) goto L_0880BCF8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BCF8:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BD04u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 895u, 0x0880BD04u, 0x0880673Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 315u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 315u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 315u, 0x0880673Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD04u) goto L_0880BD04;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BD04:
    aot_gpr_31 = (0x0880BD0Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD0Cu) goto L_0880BD0C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BD0C:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BD1C:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), aot_gpr_31);
    aot_gpr_31 = (0x0880BD34u);
    aot_gpr_5 = (0u | 2u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 898u, 0x0880BD34u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD34u) goto L_0880BD34;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BD34:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BD40u);
    aot_gpr_5 = (0u | 2u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD40u) goto L_0880BD40;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BD40:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BD4Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 900u, 0x0880BD4Cu, 0x0880673Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 315u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 315u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 315u, 0x0880673Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD4Cu) goto L_0880BD4C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BD4C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_5 = (0u | 0u);
    aot_gpr_6 = (0u + static_cast<std::uint32_t>(-1));
    aot_gpr_31 = (0x0880BD60u);
    ctx.gpr[7] = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 901u, 0x0880BD60u, 0x0880768Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 512u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 512u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 512u, 0x0880768Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD60u) goto L_0880BD60;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BD60:
    aot_gpr_5 = (ctx.gpr[2] < static_cast<std::uint32_t>(1) ? 1u : 0u);
    aot_gpr_31 = (0x0880BD6Cu);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08807098, 0u, 451u, 0x08807098u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 451u, 0x08807098u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD6Cu) goto L_0880BD6C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BD6C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BD78u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 903u, 0x0880BD78u, 0x088067C4u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 320u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 320u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 320u, 0x088067C4u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD78u) goto L_0880BD78;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BD78:
    aot_gpr_31 = (0x0880BD80u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806634, 0u, 302u, 0x08806634u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 302u, 0x08806634u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BD80u) goto L_0880BD80;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BD80:
    aot_gpr_16 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(0)));
    aot_gpr_31 = (aot_mem.aot_direct_load32(aot_gpr_29 + static_cast<std::uint32_t>(4)));
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(16));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BD90:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-144));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(128), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(132), ctx.gpr[17]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[17] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(136), ctx.gpr[18]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(140), aot_gpr_31);
    aot_gpr_31 = (0x0880BDB4u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 906u, 0x0880BDB4u, 0x0889D4C0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 273u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 273u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 273u, 0x0889D4C0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BDB4u) goto L_0880BDB4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BDB4:
    aot_gpr_6 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32040));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BDC4u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0038.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 907u, 0x0880BDC4u, 0x0889D76Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0038_entry(rt, ctx, 317u, aot_mem);
#else
        recomp_unit_0038_entry(rt, ctx, 317u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0038_entry, 38u, 317u, 0x0889D76Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BDC4u) goto L_0880BDC4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BDC4:
    { const bool branch_taken = ctx.gpr[2] != 0u;
      if (branch_taken) {
          goto L_0880BE00;
      }
      goto L_0880BDCC;
    }
L_0880BDCC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BDD8u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BDD8u) goto L_0880BDD8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BDD8:
    aot_gpr_4 = (ctx.gpr[2] | 0u);
    aot_gpr_5 = (aot_gpr_4 < static_cast<std::uint32_t>(9) ? 1u : 0u);
    { const bool branch_taken = aot_gpr_5 == 0u;
      if (branch_taken) {
          goto L_0880BE8C;
      }
      goto L_0880BDE8;
    }
L_0880BDE8:
    aot_gpr_4 = (aot_gpr_4 << 2u);
    ctx.gpr[1] = (2231u << 16u);
    ctx.gpr[1] = (ctx.gpr[1] + aot_gpr_4);
    ctx.gpr[1] = (aot_mem.aot_direct_load32(ctx.gpr[1] + static_cast<std::uint32_t>(17520)));
    jump_target = ctx.gpr[1];
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BE00:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880BF2C;
      }
      goto L_0880BE08;
    }
L_0880BE08:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE14u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 913u, 0x0880BE14u, 0x08806B2Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 375u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 375u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 375u, 0x08806B2Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE14u) goto L_0880BE14;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BE14:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE20u);
    aot_gpr_5 = (ctx.gpr[2] | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 914u, 0x0880BE20u, 0x08806E68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 430u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 430u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 430u, 0x08806E68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE20u) goto L_0880BE20;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BE20:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880BF2C;
      }
      goto L_0880BE28;
    }
L_0880BE28:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE34u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE34u) goto L_0880BE34;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BE34:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880BF2C;
      }
      goto L_0880BE3C;
    }
L_0880BE3C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE48u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806AE0, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE48u) goto L_0880BE48;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BE48:
    { const bool branch_taken = ctx.gpr[2] == 0u;
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32020));
      if (branch_taken) {
          goto L_0880BE58;
      }
      goto L_0880BE50;
    }
L_0880BE50:
    { const bool branch_taken = 0u == 0u;
    aot_gpr_4 = (ctx.gpr[28] + static_cast<std::uint32_t>(-32028));
      if (branch_taken) {
          goto L_0880BE58;
      }
      goto L_0880BE58;
    }
L_0880BE58:
    aot_gpr_5 = (aot_gpr_4 | 0u);
    aot_gpr_31 = (0x0880BE64u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 921u, 0x0880BE64u, 0x08806E68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 430u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 430u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 430u, 0x08806E68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE64u) goto L_0880BE64;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BE64:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880BF2C;
      }
      goto L_0880BE6C;
    }
L_0880BE6C:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(-32012));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE7Cu);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 923u, 0x0880BE7Cu, 0x08806D08u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 412u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 412u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 412u, 0x08806D08u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE7Cu) goto L_0880BE7C;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BE7C:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880BE8Cu);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 576u, 0x08B562D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE8Cu) goto L_0880BE8C;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BE8C:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BE98u);
    aot_gpr_5 = (aot_gpr_29 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 925u, 0x0880BE98u, 0x08806E68u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 430u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 430u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 430u, 0x08806E68u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BE98u) goto L_0880BE98;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BE98:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
      if (branch_taken) {
          goto L_0880BF2C;
      }
      goto L_0880BEA0;
    }
L_0880BEA0:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(-32000));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BEB0u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 927u, 0x0880BEB0u, 0x08806D08u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 412u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 412u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 412u, 0x08806D08u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BEB0u) goto L_0880BEB0;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BEB0:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880BEC0u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 576u, 0x08B562D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BEC0u) goto L_0880BEC0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BEC0:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880BE8C;
      }
      goto L_0880BEC8;
    }
L_0880BEC8:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(-31984));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BED8u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 930u, 0x0880BED8u, 0x08806C5Cu)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 397u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 397u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 397u, 0x08806C5Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BED8u) goto L_0880BED8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BED8:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880BEE8u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 576u, 0x08B562D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BEE8u) goto L_0880BEE8;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BEE8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880BE8C;
      }
      goto L_0880BEF0;
    }
L_0880BEF0:
    ctx.gpr[18] = (ctx.gpr[28] + static_cast<std::uint32_t>(-31968));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BF00u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 933u, 0x0880BF00u, 0x08806CC8u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 407u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 407u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 407u, 0x08806CC8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF00u) goto L_0880BF00;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BF00:
    aot_gpr_4 = (aot_gpr_29 | 0u);
    aot_gpr_5 = (ctx.gpr[18] | 0u);
    aot_gpr_31 = (0x0880BF10u);
    aot_gpr_6 = (ctx.gpr[2] | 0u);
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_direct<&recomp_unit_0212_entry, 212u, 576u, 0x08B562D8u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF10u) goto L_0880BF10;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BF10:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          goto L_0880BE8C;
      }
      goto L_0880BF18;
    }
L_0880BF18:
    aot_gpr_5 = (ctx.gpr[28] + static_cast<std::uint32_t>(-31956));
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BF28u);
    aot_gpr_6 = (0u | 3u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 936u, 0x0880BF28u, 0x08806DE0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 426u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 426u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 426u, 0x08806DE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF28u) goto L_0880BF28;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BF28:
    ctx.gpr[2] = (ctx.gpr[17] | 0u);
    goto L_0880BF2C;
L_0880BF2C:
    { std::uint32_t aot_run_words[4]{};
      aot_mem.aot_direct_load32_block(aot_gpr_29 + static_cast<std::uint32_t>(128), aot_run_words);
      aot_gpr_16 = aot_run_words[0];
      ctx.gpr[17] = aot_run_words[1];
      ctx.gpr[18] = aot_run_words[2];
      aot_gpr_31 = aot_run_words[3];
    }
    jump_target = aot_gpr_31;
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(144));
    // PSPRECOMP_V813_JR_FAST_EXIT: preserve the exact 256-transfer boundary; common JR path is shared.
    if (++local_transfers < 256u) goto LOCAL_JR_DISPATCH;
    // PSPRECOMP_V812_SHARED_SCHED_JUMP
    goto LOCAL_SCHED_BOUNDARY;
L_0880BF44:
    aot_gpr_29 = (aot_gpr_29 + static_cast<std::uint32_t>(-16));
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(0), aot_gpr_16);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(8), ctx.gpr[18]);
    aot_gpr_16 = (aot_gpr_4 | 0u);
    ctx.gpr[18] = (0u | 1u);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(4), ctx.gpr[17]);
    aot_mem.aot_direct_store32(aot_gpr_29 + static_cast<std::uint32_t>(12), aot_gpr_31);
    aot_gpr_31 = (0x0880BF68u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806654, 0u, 303u, 0x08806654u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], ctx.gpr[8], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 303u, 0x08806654u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF68u) goto L_0880BF68;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BF68:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BF74u);
    aot_gpr_5 = (0u | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 940u, 0x0880BF74u, 0x088079D0u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 555u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 555u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 555u, 0x088079D0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF74u) goto L_0880BF74;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BF74:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BF80u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806AE0, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 368u, 0x08806AE0u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF80u) goto L_0880BF80;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BF80:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          goto L_0880BFD0;
      }
      goto L_0880BF88;
    }
L_0880BF88:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BF94u);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08806888, 0u, 324u, 0x08806888u>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 324u, 0x08806888u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BF94u) goto L_0880BF94;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BF94:
    { const bool branch_taken = ctx.gpr[2] != ctx.gpr[18];
      if (branch_taken) {
          goto L_0880BFD8;
      }
      goto L_0880BF9C;
    }
L_0880BF9C:
    aot_gpr_31 = (0x0880BFA4u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 945u, 0x0880BFA4u, 0x08807220u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 462u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 462u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 462u, 0x08807220u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFA4u) goto L_0880BFA4;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BFA4:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BFB0u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-1));
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_0880682C, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem, ctx.pc, ctx.gpr[2], aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_16, aot_gpr_29, aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 322u, 0x0880682Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFB0u) goto L_0880BFB0;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BFB0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BFBCu);
    aot_gpr_5 = (0u | 1u);
    if (([&]() { /*PSPRECOMP_RESIDENT_SCHED_SAFE*/ if (rt.resident_generated_leaf_fast_allowed()) return rt.invoke_resident_generated_leaf<&vcs_resident_region_08807098, 0u, 451u, 0x08807098u>(ctx, &aot_mem, ctx.pc, aot_gpr_4, aot_gpr_5, aot_gpr_6, ctx.gpr[7], aot_gpr_31); AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_resident_scheduler_fallback<&recomp_unit_0000_entry, 0u, 451u, 0x08807098u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFBCu) goto L_0880BFBC;
    AOT_REGCACHE_SYNC_OUT(); return;
L_0880BFBC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BFC8u);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10002));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 948u, 0x0880BFC8u, 0x08807414u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 486u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 486u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 486u, 0x08807414u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFC8u) goto L_0880BFC8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BFC8:
    { const bool branch_taken = 0u == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0002_entry, 2u, 6u, 0x0880C03Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0880BFD0;
    }
L_0880BFD0:
    { const bool branch_taken = 0u == 0u;
    ctx.gpr[2] = (ctx.gpr[18] | 0u);
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0002_entry, 2u, 8u, 0x0880C04Cu>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0880BFD8;
    }
L_0880BFD8:
    ctx.gpr[17] = (0u | 0u);
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BFE8u);
    aot_gpr_5 = (0u | 1u);
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 951u, 0x0880BFE8u, 0x08807294u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 466u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 466u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 466u, 0x08807294u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFE8u) goto L_0880BFE8;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BFE8:
    { const bool branch_taken = ctx.gpr[2] == 0u;
      if (branch_taken) {
          (void)([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0002_entry, 2u, 3u, 0x0880C018u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()); AOT_REGCACHE_SYNC_OUT(); return;
      }
      goto L_0880BFF0;
    }
L_0880BFF0:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    aot_gpr_31 = (0x0880BFFCu);
    aot_gpr_5 = (0u + static_cast<std::uint32_t>(-10002));
    // PSPRECOMP_V814_LINKED_CALL: non-recursive generated call 0001->0000.
#if defined(PSPRECOMP_AOT_PRODUCTION_FASTPATHS)
    AOT_REGCACHE_SYNC_OUT();
    if (rt.prepare_aot_linked_call(ctx, &recomp_unit_0001_entry, 953u, 0x0880BFFCu, 0x08807148u)) {
#if defined(__clang__)
        [[clang::musttail]] return recomp_unit_0000_entry(rt, ctx, 456u, aot_mem);
#else
        recomp_unit_0000_entry(rt, ctx, 456u, aot_mem); return;
#endif
    }
    return;
#else
    if (([&]() { AOT_REGCACHE_SYNC_OUT(); const bool aot_regcache_same_ = (rt.invoke_chained_trusted_direct<&recomp_unit_0000_entry, 0u, 456u, 0x08807148u>(ctx, &aot_mem)); if (aot_regcache_same_) AOT_REGCACHE_SYNC_IN(); else aot_regcache_valid = false; return aot_regcache_same_; }()) && ctx.pc == 0x0880BFFCu) goto L_0880BFFC;
    AOT_REGCACHE_SYNC_OUT(); return;
#endif
L_0880BFFC:
    aot_gpr_4 = (aot_gpr_16 | 0u);
    ctx.pc = 0x0880C000u; AOT_REGCACHE_SYNC_OUT(); return;
}

#undef AOT_REGCACHE_SYNC_IN
#undef AOT_REGCACHE_SYNC_OUT

void recomp_unit_0001(Runtime &rt, AllegrexContext &ctx) {
    auto aot_mem = rt.memory().aot_fast_view();
    recomp_unit_0001_entry(rt, ctx, 0u, aot_mem);
}

void register_generated_unit_1(Runtime &runtime) {
    runtime.register_generated_unit(1u, 0x08808000u, 16384u, &recomp_unit_0001, &recomp_unit_0001_entry);
    // PSPRECOMP_V812_COMPACT_REGISTRATION: same entry set, looped once in Runtime instead of source-expanded calls.
    runtime.register_generated_entry_mask(0x08808000u, &recomp_unit_0001, "recomp_unit_0001",
                                          kEntryMasks_recomp_unit_0001, 64u);
}
} // namespace psprecomp
