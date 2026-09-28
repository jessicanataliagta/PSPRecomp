#include "framebuffer_capture.hpp"
#include "ge_renderer.hpp"
#include "vcs_profile.hpp"
#include "vcs_tier2_superblocks.hpp"

#include "psprecomp/guest_memory.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

void require_rgb(const std::vector<std::uint8_t> &rgb, std::uint8_t r, std::uint8_t g, std::uint8_t b,
                 const char *message) {
    require(rgb.size() == 3u, message);
    require(rgb[0] == r && rgb[1] == g && rgb[2] == b, message);
}



std::uint32_t ge_command(std::uint32_t command, std::uint32_t data) {
    return (command << 24u) | (data & 0x00FFFFFFu);
}

void put_sprite_vertex(psprecomp::GuestMemory &memory, std::uint32_t address,
                       std::uint16_t u, std::uint16_t v,
                       std::int16_t x, std::int16_t y, std::uint16_t z) {
    memory.store16(address + 0u, u);
    memory.store16(address + 2u, v);
    memory.store16(address + 4u, static_cast<std::uint16_t>(x));
    memory.store16(address + 6u, static_cast<std::uint16_t>(y));
    memory.store16(address + 8u, z);
}

std::array<std::uint32_t, 256> make_sprite_commands(std::uint32_t texture_address) {
    std::array<std::uint32_t, 256> commands{};
    commands[0x12] = ge_command(0x12u, 0x00800102u);  // through, tc16, pos16
    commands[0x1E] = ge_command(0x1Eu, 1u);           // texture enabled
    commands[0x9C] = ge_command(0x9Cu, 0u);           // framebuffer at VRAM base
    commands[0x9D] = ge_command(0x9Du, 4u);           // stride 4
    commands[0xA0] = ge_command(0xA0u, texture_address & 0x00FFFFF0u);
    commands[0xA8] = ge_command(0xA8u, ((texture_address >> 8u) & 0x000F0000u) | 2u);
    commands[0xB8] = ge_command(0xB8u, 0x0101u);      // 2x2
    commands[0xC2] = ge_command(0xC2u, 0u);           // linear layout
    commands[0xC3] = ge_command(0xC3u, 3u);           // RGBA8888
    commands[0xC6] = ge_command(0xC6u, 0u);           // nearest
    commands[0xC7] = ge_command(0xC7u, 0x0101u);      // clamp U/V
    commands[0xC9] = ge_command(0xC9u, 0x0103u);      // replace, use texture alpha
    commands[0xD2] = ge_command(0xD2u, 3u);           // framebuffer RGBA8888
    commands[0xD4] = ge_command(0xD4u, 0u);
    commands[0xD5] = ge_command(0xD5u, 3u | (3u << 10u));
    commands[0xE8] = ge_command(0xE8u, 0u);
    commands[0xE9] = ge_command(0xE9u, 0u);
    return commands;
}

void test_ge_sprite_renderer() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t texture = 0x08010000u;
    constexpr std::uint32_t vertices = 0x08020000u;
    memory.store32(texture + 0u, 0xFF0000FFu);  // red
    memory.store32(texture + 4u, 0xFF00FF00u);  // green
    memory.store32(texture + 8u, 0xFFFF0000u);  // blue
    memory.store32(texture + 12u, 0xFFFFFFFFu); // white
    put_sprite_vertex(memory, vertices + 0u, 0u, 0u, 0, 0, 0u);
    put_sprite_vertex(memory, vertices + 10u, 2u, 2u, 2, 2, 0u);

    auto commands = make_sprite_commands(texture);
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00060002u, stats, error),
            error.empty() ? "GE sprite renderer failed" : error.c_str());
    require(stats.rectangles == 1u && stats.pixels_written == 4u, "GE sprite did not rasterize four pixels");
    require(memory.load32(0x04000000u + 0u) == 0xFF0000FFu, "GE sprite top-left texel mismatch");
    require(memory.load32(0x04000000u + 4u) == 0xFF00FF00u, "GE sprite top-right texel mismatch");
    require(memory.load32(0x04000000u + 16u) == 0xFFFF0000u, "GE sprite bottom-left texel mismatch");
    require(memory.load32(0x04000000u + 20u) == 0xFFFFFFFFu, "GE sprite bottom-right texel mismatch");
    require(stats.next_vertex_address == vertices + 20u, "GE vertex address did not advance by the packed stride");
}

void test_ge_scissor_and_blend() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t texture = 0x08010000u;
    constexpr std::uint32_t vertices = 0x08020000u;
    for (std::uint32_t i = 0; i < 4u; ++i) memory.store32(texture + i * 4u, 0x800000FFu);
    put_sprite_vertex(memory, vertices + 0u, 0u, 0u, 0, 0, 0u);
    put_sprite_vertex(memory, vertices + 10u, 2u, 2u, 2, 2, 0u);
    for (std::uint32_t y = 0; y < 2u; ++y)
        for (std::uint32_t x = 0; x < 2u; ++x)
            memory.store32(0x04000000u + (y * 4u + x) * 4u, 0xFFFF0000u); // blue

    auto commands = make_sprite_commands(texture);
    commands[0x21] = ge_command(0x21u, 1u);
    commands[0xDF] = ge_command(0xDFu, 2u | (3u << 4u)); // src alpha, inverse src alpha
    commands[0xD4] = ge_command(0xD4u, 1u);
    commands[0xD5] = ge_command(0xD5u, 1u | (1u << 10u));
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00060002u, stats, error),
            error.empty() ? "GE blend renderer failed" : error.c_str());
    require(stats.pixels_written == 2u, "GE scissor did not restrict the sprite to one column");
    require(memory.load32(0x04000000u) == 0xFFFF0000u, "GE scissor modified the excluded column");
    const std::uint32_t blended = memory.load32(0x04000004u);
    const std::uint8_t red = static_cast<std::uint8_t>(blended);
    const std::uint8_t blue = static_cast<std::uint8_t>(blended >> 16u);
    require(red >= 126u && red <= 129u && blue >= 126u && blue <= 129u,
            "GE alpha blend did not produce the expected red/blue mix");
}


std::uint32_t ge_float24(float value) {
    return (std::bit_cast<std::uint32_t>(value) >> 8u) & 0x00FFFFFFu;
}

void put_3d_vertex(psprecomp::GuestMemory &memory, std::uint32_t address,
                   std::uint32_t color, float x, float y, float z) {
    memory.store32(address + 0u, color);
    memory.store32(address + 4u, std::bit_cast<std::uint32_t>(x));
    memory.store32(address + 8u, std::bit_cast<std::uint32_t>(y));
    memory.store32(address + 12u, std::bit_cast<std::uint32_t>(z));
}

void test_ge_matrix_persistence() {
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::update_ge_transform_state(transform, 0x3Au, 0u);
    vcs::update_ge_transform_state(transform, 0x3Bu, ge_float24(2.0f));
    vcs::update_ge_transform_state(transform, 0x3Bu, ge_float24(3.0f));
    vcs::update_ge_transform_state(transform, 0x3Bu, ge_float24(4.0f));
    require(transform.world[0] == 2.0f && transform.world[1] == 3.0f && transform.world[2] == 4.0f,
            "GE matrix DATA words were not persisted");
    require(transform.world_cursor == 3u, "GE matrix cursor did not auto-increment");
}

std::array<std::uint32_t, 256> make_3d_point_commands(std::uint32_t vertex_type);
std::uint32_t point_pixel(const psprecomp::GuestMemory &memory, std::uint32_t x, std::uint32_t y);

void test_ge_matrix_reserved_cursor_ranges() {
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);

    transform.bones[0] = 7.0f;
    vcs::update_ge_transform_state(transform, 0x2Au, 96u);
    vcs::update_ge_transform_state(transform, 0x2Bu, ge_float24(9.0f));
    require(transform.bones[0] == 7.0f && transform.bone_cursor == 97u,
            "reserved GE bone cursor wrapped into bone zero");
    vcs::update_ge_transform_state(transform, 0x2Au, 127u);
    vcs::update_ge_transform_state(transform, 0x2Bu, ge_float24(11.0f));
    require(transform.bones[0] == 7.0f && transform.bone_cursor == 0u,
            "GE bone cursor did not wrap at its seven-bit boundary");
    vcs::update_ge_transform_state(transform, 0x2Bu, ge_float24(13.0f));
    require(transform.bones[0] == 13.0f && transform.bone_cursor == 1u,
            "GE bone cursor did not resume at zero after the hardware wrap");

    transform.world[0] = 17.0f;
    vcs::update_ge_transform_state(transform, 0x3Au, 12u);
    for (std::uint32_t i = 0u; i < 4u; ++i)
        vcs::update_ge_transform_state(transform, 0x3Bu, ge_float24(20.0f + static_cast<float>(i)));
    require(transform.world[0] == 17.0f && transform.world_cursor == 0u,
            "reserved GE world cursor wrapped through modulo 12");
    vcs::update_ge_transform_state(transform, 0x3Bu, ge_float24(29.0f));
    require(transform.world[0] == 29.0f && transform.world_cursor == 1u,
            "GE world cursor did not resume after the four-bit wrap");
}

void test_ge_bounding_box_visibility_and_streams() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x0802E000u;
    put_3d_vertex(memory, vertices + 0u, 0xFFFFFFFFu, -0.5f, -0.5f, 0.0f);
    put_3d_vertex(memory, vertices + 16u, 0xFFFFFFFFu,  0.5f, -0.5f, 0.0f);
    put_3d_vertex(memory, vertices + 32u, 0xFFFFFFFFu,  0.0f,  0.5f, 0.0f);

    std::array<std::uint32_t, 256> commands{};
    commands[0x12] = ge_command(0x12u, (7u << 2u) | (3u << 7u));
    commands[0x1C] = ge_command(0x1Cu, 1u);
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);

    vcs::GeBoundingBoxResult result{};
    std::string error;
    require(vcs::test_ge_bounding_box(memory, commands, transform, vertices, 0u, 3u, result, error),
            error.empty() ? "GE BBOX inside test failed" : error.c_str());
    require(result.visible && result.next_vertex_address == vertices + 48u && result.next_index_address == 0u,
            "GE BBOX rejected visible geometry or advanced the wrong stream");

    transform.world[9] = 4.0f;
    require(vcs::test_ge_bounding_box(memory, commands, transform, vertices, 0u, 3u, result, error),
            error.empty() ? "GE BBOX outside test failed" : error.c_str());
    require(!result.visible && result.next_vertex_address == vertices + 48u,
            "GE BBOX failed to reject a box fully outside one clip plane");

    result = {};
    require(vcs::test_ge_bounding_box(memory, commands, transform, vertices, 0u, 0u, result, error),
            "GE BBOX count-zero reset failed");
    require(!result.visible && result.next_vertex_address == vertices && result.next_index_address == 0u,
            "GE BBOX count zero did not reset without consuming vertices");

    constexpr std::uint32_t indices = 0x0802F000u;
    memory.store16(indices + 0u, 0u);
    memory.store16(indices + 2u, 1u);
    memory.store16(indices + 4u, 2u);
    commands[0x12] = ge_command(0x12u, (7u << 2u) | (3u << 7u) | (2u << 11u));
    vcs::reset_ge_transform_state(transform);
    require(vcs::test_ge_bounding_box(memory, commands, transform, vertices, indices, 3u, result, error),
            error.empty() ? "GE indexed BBOX test failed" : error.c_str());
    require(result.visible && result.next_vertex_address == vertices && result.next_index_address == indices + 6u,
            "GE indexed BBOX did not advance only the index stream");
}

void test_ge_depth_clip_enable_state() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x0802F800u;
    put_3d_vertex(memory, vertices, 0xFF3366CCu, 0.0f, 0.0f, 2.0f);
    auto commands = make_3d_point_commands((7u << 2u) | (3u << 7u));
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);

    vcs::GeRenderStats stats{};
    std::string error;
    commands[0x1C] = ge_command(0x1Cu, 1u);
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE depth-clipped point failed" : error.c_str());
    require(stats.pixels_written == 0u,
            "GE rendered a point beyond +W while DEPTHCLIPENABLE was set");

    memory.zero(0x04000000u, 8u * 8u * 4u);
    stats = {};
    commands[0x1C] = ge_command(0x1Cu, 0u);
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE depth-clamp point failed" : error.c_str());
    require(stats.pixels_written == 1u && point_pixel(memory, 4u, 4u) == 0xFF3366CCu,
            "GE incorrectly applied Z clip planes while DEPTHCLIPENABLE was clear");
}

void test_ge_unsigned_raster_offset() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x0802FC00u;
    put_3d_vertex(memory, vertices, 0xFF55AA11u, 0.0f, 0.0f, 0.0f);
    auto commands = make_3d_point_commands((7u << 2u) | (3u << 7u));
    commands[0x42] = ge_command(0x42u, ge_float24(0.0f));
    commands[0x45] = ge_command(0x45u, ge_float24(2048.0f));
    commands[0x4C] = ge_command(0x4Cu, 0x8000u);  // unsigned 12.4 == 2048.0
    commands[0x46] = ge_command(0x46u, ge_float24(1.0f));

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE raster offset point failed" : error.c_str());
    require(stats.pixels_written == 1u && point_pixel(memory, 0u, 1u) == 0xFF55AA11u,
            "GE OFFSETX bit 15 was interpreted as a sign bit instead of unsigned 12.4");
}

void test_ge_non_through_triangle() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08030000u;
    put_3d_vertex(memory, vertices + 0u, 0xFF0000FFu, -0.75f, -0.75f, 0.0f);
    put_3d_vertex(memory, vertices + 16u, 0xFF00FF00u, 0.75f, -0.75f, 0.0f);
    put_3d_vertex(memory, vertices + 32u, 0xFFFF0000u, 0.0f, 0.75f, 0.0f);

    std::array<std::uint32_t, 256> commands{};
    commands[0x12] = ge_command(0x12u, (7u << 2u) | (3u << 7u)); // color8888, pos float, transform 3D
    commands[0x42] = ge_command(0x42u, ge_float24(2.0f));
    commands[0x43] = ge_command(0x43u, ge_float24(2.0f));
    commands[0x44] = ge_command(0x44u, ge_float24(32767.5f));
    commands[0x45] = ge_command(0x45u, ge_float24(2.0f));
    commands[0x46] = ge_command(0x46u, ge_float24(2.0f));
    commands[0x47] = ge_command(0x47u, ge_float24(32767.5f));
    commands[0x9C] = ge_command(0x9Cu, 0u);
    commands[0x9D] = ge_command(0x9Du, 4u);
    commands[0xD2] = ge_command(0xD2u, 3u);
    commands[0xD4] = ge_command(0xD4u, 0u);
    commands[0xD5] = ge_command(0xD5u, 3u | (3u << 10u));
    commands[0xE8] = ge_command(0xE8u, 0u);
    commands[0xE9] = ge_command(0xE9u, 0u);

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u,
                                     0x00030003u, stats, error),
            error.empty() ? "GE transformed triangle failed" : error.c_str());
    require(stats.triangles == 1u, "GE triangle topology was not emitted");
    require(stats.pixels_written > 0u, "GE transformed triangle produced no framebuffer pixels");
    require(stats.next_vertex_address == vertices + 48u, "GE transformed vertex stream did not advance");
}


std::array<std::uint32_t, 256> make_3d_point_commands(std::uint32_t vertex_type) {
    std::array<std::uint32_t, 256> commands{};
    commands[0x12] = ge_command(0x12u, vertex_type);
    commands[0x42] = ge_command(0x42u, ge_float24(4.0f));
    commands[0x43] = ge_command(0x43u, ge_float24(4.0f));
    commands[0x44] = ge_command(0x44u, ge_float24(32767.5f));
    commands[0x45] = ge_command(0x45u, ge_float24(4.0f));
    commands[0x46] = ge_command(0x46u, ge_float24(4.0f));
    commands[0x47] = ge_command(0x47u, ge_float24(32767.5f));
    commands[0x9C] = ge_command(0x9Cu, 0u);
    commands[0x9D] = ge_command(0x9Du, 8u);
    commands[0xD2] = ge_command(0xD2u, 3u);
    commands[0xD4] = ge_command(0xD4u, 0u);
    commands[0xD5] = ge_command(0xD5u, 7u | (7u << 10u));
    commands[0xE8] = ge_command(0xE8u, 0u);
    commands[0xE9] = ge_command(0xE9u, 0u);
    return commands;
}

std::uint32_t point_pixel(const psprecomp::GuestMemory &memory, std::uint32_t x, std::uint32_t y) {
    return memory.load32(0x04000000u + (y * 8u + x) * 4u);
}

void test_ge_two_bone_skinning() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08031000u;
    // weight8 x2, then alignment padding, color8888, position float.
    memory.store8(vertices + 0u, 64u);
    memory.store8(vertices + 1u, 64u);
    memory.store32(vertices + 4u, 0xFF0000FFu);
    memory.store32(vertices + 8u, std::bit_cast<std::uint32_t>(-0.5f));
    memory.store32(vertices + 12u, std::bit_cast<std::uint32_t>(0.0f));
    memory.store32(vertices + 16u, std::bit_cast<std::uint32_t>(0.0f));

    const std::uint32_t type = (7u << 2u) | (3u << 7u) | (1u << 9u) | (1u << 14u);
    auto commands = make_3d_point_commands(type);
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    transform.bones[12u + 9u] = 1.0f;

    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE skinned point failed" : error.c_str());
    require(point_pixel(memory, 4u, 4u) == 0xFF0000FFu,
            "GE two-bone blend did not move the point to the weighted position");
    require(stats.skinned_vertices == 1u && stats.next_vertex_address == vertices + 20u,
            "GE skinning statistics or packed stride are incorrect");
}

void test_ge_morph_targets() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08032000u;
    put_3d_vertex(memory, vertices + 0u, 0xFF0000FFu, -0.5f, 0.0f, 0.0f);
    put_3d_vertex(memory, vertices + 16u, 0xFFFF0000u, 0.5f, 0.0f, 0.0f);

    const std::uint32_t type = (7u << 2u) | (3u << 7u) | (1u << 18u);
    auto commands = make_3d_point_commands(type);
    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    transform.morph_weights[0] = 0.25f;
    transform.morph_weights[1] = 0.75f;

    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE morphed point failed" : error.c_str());
    require(point_pixel(memory, 5u, 4u) == 0xFFBF003Fu,
            "GE morph weights did not blend point position and color");
    require(stats.morphed_vertices == 1u && stats.next_vertex_address == vertices + 32u,
            "GE morph statistics or repeated target stride are incorrect");
}

void test_ge_directional_lighting() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08033000u;
    // color8888, normal float3, position float3: 28-byte stride.
    memory.store32(vertices + 0u, 0xFF0000FFu);
    memory.store32(vertices + 4u, std::bit_cast<std::uint32_t>(0.0f));
    memory.store32(vertices + 8u, std::bit_cast<std::uint32_t>(0.0f));
    memory.store32(vertices + 12u, std::bit_cast<std::uint32_t>(1.0f));
    memory.store32(vertices + 16u, std::bit_cast<std::uint32_t>(0.0f));
    memory.store32(vertices + 20u, std::bit_cast<std::uint32_t>(0.0f));
    memory.store32(vertices + 24u, std::bit_cast<std::uint32_t>(0.0f));

    const std::uint32_t type = (7u << 2u) | (3u << 5u) | (3u << 7u);
    auto commands = make_3d_point_commands(type);
    commands[0x17] = ge_command(0x17u, 1u);
    commands[0x18] = ge_command(0x18u, 1u);
    commands[0x53] = ge_command(0x53u, 2u);       // vertex color supplies diffuse material
    commands[0x58] = ge_command(0x58u, 255u);
    commands[0x5D] = ge_command(0x5Du, 255u);
    commands[0x5F] = ge_command(0x5Fu, 0u);       // directional, diffuse
    commands[0x63] = ge_command(0x63u, ge_float24(0.0f));
    commands[0x64] = ge_command(0x64u, ge_float24(0.0f));
    commands[0x65] = ge_command(0x65u, ge_float24(1.0f));
    commands[0x90] = ge_command(0x90u, 0x00FFFFFFu);

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE lit point failed" : error.c_str());
    require(point_pixel(memory, 4u, 4u) == 0xFF0000FFu,
            "GE directional diffuse lighting did not preserve the red material color");
    require(stats.lit_vertices == 1u, "GE lighting statistics did not record the vertex");

    memory.store32(0x04000000u + (4u * 8u + 4u) * 4u, 0u);
    commands[0x51] = ge_command(0x51u, 1u);
    stats = {};
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            "GE reversed-normal lighting failed");
    require((point_pixel(memory, 4u, 4u) & 0x00FFFFFFu) == 0u,
            "GE reverse-normal state did not suppress the opposing directional diffuse light");
}


void configure_test_texture(std::array<std::uint32_t, 256> &commands, std::uint32_t texture) {
    commands[0x1E] = ge_command(0x1Eu, 1u);
    commands[0xA0] = ge_command(0xA0u, texture & 0x00FFFFF0u);
    commands[0xA8] = ge_command(0xA8u, ((texture >> 8u) & 0x000F0000u) | 2u);
    commands[0xB8] = ge_command(0xB8u, 0x0101u);
    commands[0xC2] = ge_command(0xC2u, 0u);
    commands[0xC3] = ge_command(0xC3u, 3u);
    commands[0xC6] = ge_command(0xC6u, 0u);
    commands[0xC7] = ge_command(0xC7u, 0x0101u);
    commands[0xC9] = ge_command(0xC9u, 0x0103u);
}

void put_lit_3d_vertex(psprecomp::GuestMemory &memory, std::uint32_t address,
                       std::uint32_t color, float nx, float ny, float nz,
                       float x, float y, float z) {
    memory.store32(address + 0u, color);
    memory.store32(address + 4u, std::bit_cast<std::uint32_t>(nx));
    memory.store32(address + 8u, std::bit_cast<std::uint32_t>(ny));
    memory.store32(address + 12u, std::bit_cast<std::uint32_t>(nz));
    memory.store32(address + 16u, std::bit_cast<std::uint32_t>(x));
    memory.store32(address + 20u, std::bit_cast<std::uint32_t>(y));
    memory.store32(address + 24u, std::bit_cast<std::uint32_t>(z));
}

void test_ge_texture_matrix_uv_generation() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t texture = 0x08014000u;
    constexpr std::uint32_t vertices = 0x08034000u;
    memory.store32(texture + 0u, 0xFF0000FFu);
    memory.store32(texture + 4u, 0xFF00FF00u);
    memory.store32(texture + 8u, 0xFFFF0000u);
    memory.store32(texture + 12u, 0xFFFFFFFFu);
    put_3d_vertex(memory, vertices, 0xFFFFFFFFu, 0.0f, 0.0f, 0.0f);

    auto commands = make_3d_point_commands((7u << 2u) | (3u << 7u));
    configure_test_texture(commands, texture);
    commands[0xC0] = ge_command(0xC0u, 1u); // texture matrix, position source

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    transform.texture.fill(0.0f);
    transform.texture[9u] = 1.5f;
    transform.texture[10u] = 0.5f;
    transform.texture[11u] = 2.0f;

    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE texture-matrix point failed" : error.c_str());
    require(point_pixel(memory, 4u, 4u) == 0xFF00FF00u,
            "GE texture matrix or projective Q did not select the expected texel");
    require(stats.generated_uv_vertices == 1u, "GE texture-matrix UV generation was not recorded");
}

void test_ge_environment_map_uv_generation() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t texture = 0x08015000u;
    constexpr std::uint32_t vertices = 0x08035000u;
    memory.store32(texture + 0u, 0xFF0000FFu);
    memory.store32(texture + 4u, 0xFF00FF00u);
    memory.store32(texture + 8u, 0xFFFF0000u);
    memory.store32(texture + 12u, 0xFFFFFFFFu);
    put_lit_3d_vertex(memory, vertices, 0xFFFFFFFFu, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f);

    auto commands = make_3d_point_commands((7u << 2u) | (3u << 5u) | (3u << 7u));
    configure_test_texture(commands, texture);
    commands[0xC0] = ge_command(0xC0u, 2u);       // environment map
    commands[0xC1] = ge_command(0xC1u, 0u | (1u << 8u));
    commands[0x63] = ge_command(0x63u, ge_float24(1.0f));
    commands[0x64] = ge_command(0x64u, ge_float24(0.0f));
    commands[0x65] = ge_command(0x65u, ge_float24(0.0f));
    commands[0x66] = ge_command(0x66u, ge_float24(-1.0f));
    commands[0x67] = ge_command(0x67u, ge_float24(0.0f));
    commands[0x68] = ge_command(0x68u, ge_float24(0.0f));

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00000001u, stats, error),
            error.empty() ? "GE environment-map point failed" : error.c_str());
    require(point_pixel(memory, 4u, 4u) == 0xFF00FF00u,
            "GE environment-map light selection did not generate the expected ST coordinates");
    require(stats.generated_uv_vertices == 1u, "GE environment-map UV generation was not recorded");
}

void put_through_triangle_vertex(psprecomp::GuestMemory &memory, std::uint32_t address,
                                 std::uint32_t color, std::int16_t x, std::int16_t y,
                                 std::uint16_t z = 0u) {
    memory.store32(address + 0u, color);
    memory.store16(address + 4u, static_cast<std::uint16_t>(x));
    memory.store16(address + 6u, static_cast<std::uint16_t>(y));
    memory.store16(address + 8u, z);
}

std::array<std::uint32_t, 256> make_through_triangle_commands() {
    std::array<std::uint32_t, 256> commands{};
    commands[0x12] = ge_command(0x12u, (1u << 23u) | (7u << 2u) | (2u << 7u));
    commands[0x9C] = ge_command(0x9Cu, 0u);
    commands[0x9D] = ge_command(0x9Du, 8u);
    commands[0xD2] = ge_command(0xD2u, 3u);
    commands[0xD4] = ge_command(0xD4u, 0u);
    commands[0xD5] = ge_command(0xD5u, 7u | (7u << 10u));
    commands[0xE8] = ge_command(0xE8u, 0u);
    commands[0xE9] = ge_command(0xE9u, 0u);
    return commands;
}

void put_test_triangle(psprecomp::GuestMemory &memory, std::uint32_t vertices) {
    put_through_triangle_vertex(memory, vertices + 0u, 0xFF0000FFu, 1, 1);
    put_through_triangle_vertex(memory, vertices + 12u, 0xFF00FF00u, 6, 1);
    put_through_triangle_vertex(memory, vertices + 24u, 0xFFFF0000u, 1, 6);
}

void clear_test_framebuffer(psprecomp::GuestMemory &memory) {
    for (std::uint32_t pixel = 0u; pixel < 64u; ++pixel)
        memory.store32(0x04000000u + pixel * 4u, 0u);
}

void test_ge_cull_face_state() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08036000u;
    put_test_triangle(memory, vertices);
    auto commands = make_through_triangle_commands();
    commands[0x50] = ge_command(0x50u, 1u); // gouraud, isolate culling
    commands[0x1D] = ge_command(0x1Du, 1u);
    commands[0x9B] = ge_command(0x9Bu, 1u); // accept counter-clockwise

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00030003u, stats, error),
            error.empty() ? "GE CCW culling test failed" : error.c_str());
    require(stats.pixels_written > 0u && stats.culled_triangles == 0u,
            "GE counter-clockwise cull mode rejected the selected winding");

    clear_test_framebuffer(memory);
    commands[0x9B] = ge_command(0x9Bu, 0u); // accept clockwise
    stats = {};
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00030003u, stats, error),
            "GE clockwise culling test failed");
    require(stats.pixels_written == 0u && stats.culled_triangles == 1u,
            "GE clockwise cull mode failed to reject the opposing winding");
}

void test_ge_flat_shading_provoking_vertex() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t vertices = 0x08037000u;
    put_test_triangle(memory, vertices);
    auto commands = make_through_triangle_commands();
    commands[0x50] = ge_command(0x50u, 0u); // flat

    vcs::GeTransformState transform{};
    vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{};
    std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00030003u, stats, error),
            error.empty() ? "GE flat-shaded triangle failed" : error.c_str());
    const std::uint32_t flat = memory.load32(0x04000000u + (2u * 8u + 2u) * 4u);
    require(flat == 0xFFFF0000u,
            "GE flat shading did not use the third/provoking vertex color");
    require(stats.flat_shaded_primitives == 1u, "GE flat-shaded primitive was not recorded");

    clear_test_framebuffer(memory);
    commands[0x50] = ge_command(0x50u, 1u); // gouraud
    stats = {};
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u, 0x00030003u, stats, error),
            "GE Gouraud triangle failed");
    const std::uint32_t gouraud = memory.load32(0x04000000u + (2u * 8u + 2u) * 4u);
    require(gouraud != 0u && gouraud != 0xFFFF0000u,
            "GE Gouraud mode incorrectly retained the provoking vertex color");
}


void clear_test_framebuffer(psprecomp::GuestMemory &memory, std::uint32_t width, std::uint32_t height,
                            std::uint32_t stride = 8u) {
    for (std::uint32_t y = 0u; y < height; ++y)
        for (std::uint32_t x = 0u; x < width; ++x)
            memory.store32(0x04000000u + (y * stride + x) * 4u, 0u);
}

void configure_4x4_sprite(std::array<std::uint32_t, 256> &commands,
                          std::uint32_t texture, std::uint32_t format) {
    commands = make_sprite_commands(texture);
    commands[0x9D] = ge_command(0x9Du, 8u);
    commands[0xA8] = ge_command(0xA8u, ((texture >> 8u) & 0x000F0000u) | 4u);
    commands[0xB8] = ge_command(0xB8u, 0x0202u); // 4x4
    commands[0xC3] = ge_command(0xC3u, format);
    commands[0xD5] = ge_command(0xD5u, 3u | (3u << 10u));
}

void write_psp_dxt_color_block(psprecomp::GuestMemory &memory, std::uint32_t address,
                               std::uint16_t c1, std::uint16_t c2) {
    // PSP order: four selector rows first, then the two 565 endpoints.
    for (std::uint32_t i = 0u; i < 4u; ++i) memory.store8(address + i, 0u);
    memory.store16(address + 4u, c1);
    memory.store16(address + 6u, c2);
}

void test_ge_psp_dxt_formats() {
    constexpr std::uint32_t vertices = 0x08026000u;
    constexpr std::uint32_t texture = 0x08027000u;
    const auto render = [&](std::uint32_t format) {
        psprecomp::GuestMemory memory;
        put_sprite_vertex(memory, vertices + 0u, 0u, 0u, 0, 0, 0u);
        put_sprite_vertex(memory, vertices + 10u, 4u, 4u, 4, 4, 0u);
        write_psp_dxt_color_block(memory, texture, 0xF800u, 0x001Fu); // selector 0 = red
        if (format == 9u) {
            for (std::uint32_t row = 0u; row < 4u; ++row)
                memory.store16(texture + 8u + row * 2u, 0xFFFFu);
        } else if (format == 10u) {
            memory.store32(texture + 8u, 0u);
            memory.store16(texture + 12u, 0u);
            memory.store8(texture + 14u, 200u);
            memory.store8(texture + 15u, 10u);
        }
        clear_test_framebuffer(memory, 4u, 4u);
        std::array<std::uint32_t,256> commands{};
        configure_4x4_sprite(commands, texture, format);
        vcs::GeTransformState transform{}; vcs::reset_ge_transform_state(transform);
        vcs::GeRenderStats stats{}; std::string error;
        require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u,
                                         0x00060002u, stats, error),
                error.empty() ? "GE PSP DXT sprite failed" : error.c_str());
        return memory.load32(0x04000000u);
    };
    require(render(8u) == 0xFF0000F8u, "PSP DXT1 reversed block decode mismatch");
    require(render(9u) == 0xFF0000F8u, "PSP DXT3 alpha/color decode mismatch");
    require(render(10u) == 0xC80000F8u, "PSP DXT5 alpha palette decode mismatch");
}

void test_ge_fixed_mip_level_selection() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t level0 = 0x08028000u;
    constexpr std::uint32_t level1 = 0x08029000u;
    constexpr std::uint32_t vertices = 0x0802A000u;
    for (std::uint32_t i = 0u; i < 16u; ++i) memory.store32(level0 + i * 4u, 0xFF0000FFu);
    for (std::uint32_t i = 0u; i < 4u; ++i) memory.store32(level1 + i * 4u, 0xFF00FF00u);
    put_sprite_vertex(memory, vertices + 0u, 0u, 0u, 0, 0, 0u);
    put_sprite_vertex(memory, vertices + 10u, 2u, 2u, 2, 2, 0u);
    clear_test_framebuffer(memory, 2u, 2u);
    std::array<std::uint32_t,256> commands{};
    configure_4x4_sprite(commands, level0, 3u);
    commands[0xA1] = ge_command(0xA1u, level1 & 0x00FFFFF0u);
    commands[0xA9] = ge_command(0xA9u, ((level1 >> 8u) & 0x000F0000u) | 2u);
    commands[0xB9] = ge_command(0xB9u, 0x0101u);
    commands[0xC2] = ge_command(0xC2u, 1u << 16u); // max mip level 1
    commands[0xC6] = ge_command(0xC6u, 4u);         // mipmapping enabled, nearest
    commands[0xC8] = ge_command(0xC8u, 1u | (16u << 16u)); // constant LOD = 1.0
    vcs::GeTransformState transform{}; vcs::reset_ge_transform_state(transform);
    vcs::GeRenderStats stats{}; std::string error;
    require(vcs::render_ge_primitive(memory, commands, transform, vertices, 0u,
                                     0x00060002u, stats, error),
            error.empty() ? "GE fixed mip selection failed" : error.c_str());
    require(memory.load32(0x04000000u) == 0xFF00FF00u,
            "constant PSP LOD did not select mip level 1");
}


void test_tier2_counter_snapshot_reset() {
    using vcs::Tier2ClusterId;
    auto &entity = vcs::tier2_detail::counters(Tier2ClusterId::Entity);
    auto &world = vcs::tier2_detail::counters(Tier2ClusterId::World);

    (void)vcs::consume_tier2_counters();

    entity.entries = 3;
    entity.fused_tail_edges = 5;
    entity.fused_calls = 7;
    entity.cold_exits = 11;
    entity.fallbacks = 13;
    entity.sampled_entries = 17;
    entity.sampled_ns = 19000;
    world.entries = 23;
    world.fused_calls = 29;

    const auto snapshot = vcs::consume_tier2_counters();
    const auto &entity_snapshot =
        snapshot.cluster[static_cast<std::size_t>(Tier2ClusterId::Entity)];
    const auto &world_snapshot =
        snapshot.cluster[static_cast<std::size_t>(Tier2ClusterId::World)];

    require(entity_snapshot.entries == 3, "Tier2 entity entry counter snapshot mismatch");
    require(entity_snapshot.fused_tail_edges == 5, "Tier2 fused-tail snapshot mismatch");
    require(entity_snapshot.fused_calls == 7, "Tier2 fused-call snapshot mismatch");
    require(entity_snapshot.cold_exits == 11, "Tier2 cold-exit snapshot mismatch");
    require(entity_snapshot.fallbacks == 13, "Tier2 fallback snapshot mismatch");
    require(entity_snapshot.sampled_entries == 17, "Tier2 sampled-entry snapshot mismatch");
    require(entity_snapshot.sampled_ns == 19000, "Tier2 sampled-time snapshot mismatch");
    require(world_snapshot.entries == 23 && world_snapshot.fused_calls == 29,
            "Tier2 multi-cluster snapshot mismatch");

    const auto reset_snapshot = vcs::consume_tier2_counters();
    for (const auto &cluster : reset_snapshot.cluster) {
        require(cluster.entries == 0 && cluster.fused_tail_edges == 0 &&
                    cluster.fused_calls == 0 && cluster.cold_exits == 0 &&
                    cluster.fallbacks == 0 && cluster.sampled_entries == 0 &&
                    cluster.sampled_ns == 0,
                "Tier2 counters were not reset after consume");
    }

    require(vcs::tier2_detail::g_active_depth == 0u,
            "Tier2 active-depth started nonzero");
    {
        vcs::tier2_detail::SampleScope scope(Tier2ClusterId::Entity);
        require(vcs::tier2_detail::g_active_depth == 1u,
                "Tier2 SampleScope did not arm reentry guard");
        require(!vcs::tier2_cluster_enabled(Tier2ClusterId::Geometry),
                "Tier2 allowed nested cluster reentry while a superblock was active");
    }
    require(vcs::tier2_detail::g_active_depth == 0u,
            "Tier2 SampleScope leaked active-depth on unwind");
    (void)vcs::consume_tier2_counters();
}

void test_framebuffer_formats() {
    psprecomp::GuestMemory memory;
    constexpr std::uint32_t address = 0x04000000u;
    const vcs::FramebufferDescription base{address, 1u, 1u, 1u, 0u};

    memory.store16(address, 0x001Fu);
    require_rgb(vcs::decode_framebuffer_rgb(memory, base), 255u, 0u, 0u, "RGB565 red conversion failed");
    memory.store16(address, 0x07E0u);
    require_rgb(vcs::decode_framebuffer_rgb(memory, base), 0u, 255u, 0u, "RGB565 green conversion failed");
    memory.store16(address, 0xF800u);
    require_rgb(vcs::decode_framebuffer_rgb(memory, base), 0u, 0u, 255u, "RGB565 blue conversion failed");

    auto format = base;
    format.pixel_format = 1u;
    memory.store16(address, 0x001Fu);
    require_rgb(vcs::decode_framebuffer_rgb(memory, format), 255u, 0u, 0u, "RGBA5551 conversion failed");

    format.pixel_format = 2u;
    memory.store16(address, 0x0F00u);
    require_rgb(vcs::decode_framebuffer_rgb(memory, format), 0u, 0u, 255u, "RGBA4444 conversion failed");

    format.pixel_format = 3u;
    memory.store32(address, 0x7F563412u);
    require_rgb(vcs::decode_framebuffer_rgb(memory, format), 0x12u, 0x34u, 0x56u, "RGBA8888 conversion failed");

    bool rejected = false;
    try {
        test_tier2_counter_snapshot_reset();
        auto invalid = format;
        invalid.stride = 0u;
        (void)vcs::decode_framebuffer_rgb(memory, invalid);
    } catch (...) {
        rejected = true;
    }
    require(rejected, "invalid framebuffer stride was accepted");

    const auto path = std::filesystem::temp_directory_path() / "psprecomp_framebuffer_test.ppm";
    vcs::write_framebuffer_ppm(path, format, vcs::decode_framebuffer_rgb(memory, format));
    require(std::filesystem::file_size(path) > 3u, "PPM frame dump was not written");
    std::filesystem::remove(path);
}
} // namespace

int main() {
    try {
        test_framebuffer_formats();
        test_ge_matrix_persistence();
        test_ge_matrix_reserved_cursor_ranges();
        test_ge_bounding_box_visibility_and_streams();
        test_ge_depth_clip_enable_state();
        test_ge_unsigned_raster_offset();
        test_ge_sprite_renderer();
        test_ge_scissor_and_blend();
        test_ge_psp_dxt_formats();
        test_ge_fixed_mip_level_selection();
        test_ge_non_through_triangle();
        test_ge_two_bone_skinning();
        test_ge_morph_targets();
        test_ge_directional_lighting();
        test_ge_texture_matrix_uv_generation();
        test_ge_environment_map_uv_generation();
        test_ge_cull_face_state();
        test_ge_flat_shading_provoking_vertex();
        std::string error;
        const bool profile_ok = vcs::run_profile_self_tests(error);
        require(profile_ok, error.empty() ? "VCS profile self-test failed" : error.c_str());
        std::cout << "All VCS scheduler/callback/framebuffer tests passed.\n";
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "VCS profile test failure: " << exception.what() << "\n";
        return 1;
    }
}
