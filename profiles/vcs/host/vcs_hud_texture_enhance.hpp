#pragma once

// HUD texture enhancement for the DirectX 12 backend.
//
// The interface art is authored for a 480x272 screen. At a 3840x2160 internal
// target every PSP texel covers about 8x8 pixels, so plain bilinear magnification
// turns glyph and icon edges into soft, diamond-shaped ramps, and point sampling
// turns them into blocks. Both are cheaper to fix once, when the texture is
// decoded, than per pixel in the shader:
//
//  * transparent texels inherit the colour of their opaque neighbours (alpha
//    bleeding), so filtering across an edge no longer pulls in the black that
//    PSP tools left behind transparent pixels -- the dark fringe around text;
//  * the image is enlarged by an integer factor with a separable Catmull-Rom
//    cubic, clamped to the 2x2 source neighbourhood so it cannot ring;
//  * optionally ("sharp") hard transitions -- and only hard ones, judged by how
//    far apart the neighbouring source texels are -- get their ramp narrowed,
//    which is what makes glyph outlines read as HD instead of as a blur. Soft
//    gradients (shadows, glows, bar fills) fall below the threshold and are
//    left exactly as the cubic produced them;
//  * a box-filtered mip chain is appended so the larger image never aliases
//    when the game draws it smaller (pause-menu map, scaled icons).
//
// Everything here was written for this project from the textbook definitions
// (Catmull-Rom spline, DirectDraw Surface layout, FNV-1a). No third-party
// scaler source is used.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace vcs::hud_texture {

// Tightly packed R8G8B8A8, row-major -- the layout the backend uploads.
struct Image {
    std::uint32_t width{};
    std::uint32_t height{};
    std::vector<std::byte> rgba;
};

// Stable identity for texture dumping/replacement: FNV-1a over the dimensions
// and the decoded level-0 pixels. Independent of guest addresses, cache keys
// and sampler state, so the same artwork gets the same name on every run.
inline std::uint64_t content_hash(const std::byte *rgba, std::uint32_t width,
                                  std::uint32_t height) noexcept {
    std::uint64_t hash = 1469598103934665603ull;
    const auto feed = [&hash](std::uint8_t b) noexcept {
        hash ^= b;
        hash *= 1099511628211ull;
    };
    for (unsigned shift = 0u; shift < 32u; shift += 8u) feed(static_cast<std::uint8_t>(width >> shift));
    for (unsigned shift = 0u; shift < 32u; shift += 8u) feed(static_cast<std::uint8_t>(height >> shift));
    const std::size_t bytes = static_cast<std::size_t>(width) * height * 4u;
    for (std::size_t i = 0u; i < bytes; ++i) feed(static_cast<std::uint8_t>(rgba[i]));
    return hash;
}

inline std::string hash_name(std::uint64_t hash) {
    static constexpr char kDigits[] = "0123456789abcdef";
    std::string text(16u, '0');
    for (int i = 15; i >= 0; --i, hash >>= 4u) text[static_cast<std::size_t>(i)] = kDigits[hash & 0xFu];
    return text;
}

// Gives every fully transparent texel the average colour of its non-transparent
// neighbours, `passes` rings deep (two covers the Catmull-Rom footprint). Alpha
// is untouched, so the image composites exactly as before; only what a filter
// sees when it reaches across an edge changes. Textures that are entirely
// opaque or entirely transparent (alpha unused) are left alone.
inline void bleed_transparent_rgb(std::byte *rgba, std::uint32_t width, std::uint32_t height,
                                  std::uint32_t passes = 2u) {
    const std::size_t count = static_cast<std::size_t>(width) * height;
    std::vector<std::uint8_t> known(count);
    bool any_transparent = false;
    bool any_visible = false;
    for (std::size_t i = 0u; i < count; ++i) {
        known[i] = rgba[i * 4u + 3u] != std::byte{0} ? 1u : 0u;
        any_transparent |= known[i] == 0u;
        any_visible |= known[i] != 0u;
    }
    if (!any_transparent || !any_visible) return;
    std::vector<std::uint8_t> next;
    for (std::uint32_t pass = 0u; pass < passes; ++pass) {
        next = known;
        bool changed = false;
        for (std::uint32_t y = 0u; y < height; ++y) {
            for (std::uint32_t x = 0u; x < width; ++x) {
                const std::size_t index = static_cast<std::size_t>(y) * width + x;
                if (known[index] != 0u) continue;
                std::uint32_t sum[3]{};
                std::uint32_t samples = 0u;
                for (int dy = -1; dy <= 1; ++dy) {
                    const int ny = static_cast<int>(y) + dy;
                    if (ny < 0 || ny >= static_cast<int>(height)) continue;
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int nx = static_cast<int>(x) + dx;
                        if (nx < 0 || nx >= static_cast<int>(width)) continue;
                        const std::size_t n = static_cast<std::size_t>(ny) * width +
                                              static_cast<std::size_t>(nx);
                        if (known[n] == 0u) continue;
                        for (int c = 0; c < 3; ++c)
                            sum[c] += static_cast<std::uint8_t>(rgba[n * 4u + static_cast<std::size_t>(c)]);
                        ++samples;
                    }
                }
                if (samples == 0u) continue;
                for (int c = 0; c < 3; ++c)
                    rgba[index * 4u + static_cast<std::size_t>(c)] =
                        static_cast<std::byte>((sum[c] + samples / 2u) / samples);
                next[index] = 1u;
                changed = true;
            }
        }
        known.swap(next);
        if (!changed) break;
    }
}

namespace detail {

struct CubicPhase {
    int offset{};                  // first central source texel relative to the output's own texel
    std::array<float, 4> weight{}; // taps at offset-1 .. offset+2
};

// Output pixel o of an integer upscale by `factor` samples source coordinate
// (o + 0.5) / factor - 0.5 (texel centres at integers). Only `factor` distinct
// fractional positions exist, so the weights are computed once per phase.
inline std::vector<CubicPhase> catmull_rom_phases(std::uint32_t factor) {
    std::vector<CubicPhase> phases(factor);
    for (std::uint32_t p = 0u; p < factor; ++p) {
        const float x = (static_cast<float>(p) + 0.5f) / static_cast<float>(factor) - 0.5f;
        const float base = std::floor(x);
        const float t = x - base;
        const float t2 = t * t;
        const float t3 = t2 * t;
        phases[p].offset = static_cast<int>(base);
        phases[p].weight = {
            0.5f * (-t3 + 2.0f * t2 - t),
            0.5f * (3.0f * t3 - 5.0f * t2 + 2.0f),
            0.5f * (-3.0f * t3 + 4.0f * t2 + t),
            0.5f * (t3 - t2),
        };
    }
    return phases;
}

inline std::uint32_t address(int index, std::uint32_t size, bool clamp) noexcept {
    const int n = static_cast<int>(size);
    if (clamp) return static_cast<std::uint32_t>(std::clamp(index, 0, n - 1));
    const int wrapped = index % n;
    return static_cast<std::uint32_t>(wrapped < 0 ? wrapped + n : wrapped);
}

} // namespace detail

// Integer upscale of an R8G8B8A8 image. `clamp_u`/`clamp_v` mirror the GE
// addressing so tiled textures stay seamless. `sharp` enables the edge-only
// ramp narrowing described at the top of this file.
inline std::vector<std::byte> upscale(const std::byte *source, std::uint32_t width,
                                      std::uint32_t height, std::uint32_t factor,
                                      bool clamp_u, bool clamp_v, bool sharp) {
    const std::uint32_t out_width = width * factor;
    const std::uint32_t out_height = height * factor;
    const std::vector<detail::CubicPhase> phases = detail::catmull_rom_phases(factor);

    // Every output column/row has four taps and one bracketing "cell" -- the
    // two central source texels, whose 2x2 block bounds the result. All of it
    // depends only on the coordinate, so it is tabulated once per axis.
    // Cell indices are the pre-addressing position + 1, i.e. 0 .. size.
    struct Axis {
        std::vector<std::uint32_t> taps;  // 4 per output coordinate
        std::vector<std::uint32_t> cell;  // 1 per output coordinate
        std::vector<std::uint32_t> first; // cell -> addressed first texel
        std::vector<std::uint32_t> second;
    };
    const auto make_axis = [&phases, factor](std::uint32_t size, bool clamp) {
        Axis axis;
        const std::uint32_t out = size * factor;
        axis.taps.resize(static_cast<std::size_t>(out) * 4u);
        axis.cell.resize(out);
        for (std::uint32_t o = 0u; o < out; ++o) {
            const int base = static_cast<int>(o / factor) + phases[o % factor].offset;
            for (int k = 0; k < 4; ++k)
                axis.taps[static_cast<std::size_t>(o) * 4u + static_cast<std::size_t>(k)] =
                    detail::address(base - 1 + k, size, clamp);
            axis.cell[o] = static_cast<std::uint32_t>(base + 1);
        }
        axis.first.resize(size + 1u);
        axis.second.resize(size + 1u);
        for (std::uint32_t c = 0u; c <= size; ++c) {
            axis.first[c] = detail::address(static_cast<int>(c) - 1, size, clamp);
            axis.second[c] = detail::address(static_cast<int>(c), size, clamp);
        }
        return axis;
    };
    const Axis ax = make_axis(width, clamp_u);
    const Axis ay = make_axis(height, clamp_v);

    std::vector<float> texels(static_cast<std::size_t>(width) * height * 4u);
    for (std::size_t i = 0u; i < texels.size(); ++i)
        texels[i] = static_cast<float>(static_cast<std::uint8_t>(source[i]));

    // Per-cell channel bounds, (width + 1) x (height + 1) cells.
    const std::size_t cells_x = static_cast<std::size_t>(width) + 1u;
    std::vector<float> cell_lo(cells_x * (height + 1u) * 4u);
    std::vector<float> cell_hi(cell_lo.size());
    for (std::uint32_t cy = 0u; cy <= height; ++cy) {
        for (std::uint32_t cx = 0u; cx <= width; ++cx) {
            const std::size_t t00 = (static_cast<std::size_t>(ay.first[cy]) * width + ax.first[cx]) * 4u;
            const std::size_t t01 = (static_cast<std::size_t>(ay.first[cy]) * width + ax.second[cx]) * 4u;
            const std::size_t t10 = (static_cast<std::size_t>(ay.second[cy]) * width + ax.first[cx]) * 4u;
            const std::size_t t11 = (static_cast<std::size_t>(ay.second[cy]) * width + ax.second[cx]) * 4u;
            const std::size_t cell = (cy * cells_x + cx) * 4u;
            for (std::size_t c = 0u; c < 4u; ++c) {
                const float a = texels[t00 + c], b = texels[t01 + c];
                const float d = texels[t10 + c], e = texels[t11 + c];
                cell_lo[cell + c] = std::min(std::min(a, b), std::min(d, e));
                cell_hi[cell + c] = std::max(std::max(a, b), std::max(d, e));
            }
        }
    }

    // Horizontal pass into a float intermediate: out_width x height.
    std::vector<float> wide(static_cast<std::size_t>(out_width) * height * 4u);
    for (std::uint32_t y = 0u; y < height; ++y) {
        const float *src = texels.data() + static_cast<std::size_t>(y) * width * 4u;
        float *row = wide.data() + static_cast<std::size_t>(y) * out_width * 4u;
        for (std::uint32_t ox = 0u; ox < out_width; ++ox) {
            const std::array<float, 4> &w = phases[ox % factor].weight;
            const std::uint32_t *tap = ax.taps.data() + static_cast<std::size_t>(ox) * 4u;
            const float *s0 = src + tap[0] * 4u, *s1 = src + tap[1] * 4u;
            const float *s2 = src + tap[2] * 4u, *s3 = src + tap[3] * 4u;
            float *out = row + static_cast<std::size_t>(ox) * 4u;
            for (std::size_t c = 0u; c < 4u; ++c)
                out[c] = w[0] * s0[c] + w[1] * s1[c] + w[2] * s2[c] + w[3] * s3[c];
        }
    }

    // Edge narrowing: blend in only where the texels bracketing a pixel differ
    // by more than kRangeStart (fully from kRangeFull), so smooth art is left
    // alone. kContrast 2.5 shrinks a one-texel ramp to roughly 0.4 texel.
    constexpr float kRangeStart = 0.25f * 255.0f;
    constexpr float kRangeFull = 0.60f * 255.0f;
    constexpr float kContrast = 2.5f;

    std::vector<std::byte> result(static_cast<std::size_t>(out_width) * out_height * 4u);
    for (std::uint32_t oy = 0u; oy < out_height; ++oy) {
        const std::array<float, 4> &w = phases[oy % factor].weight;
        const std::uint32_t *tap = ay.taps.data() + static_cast<std::size_t>(oy) * 4u;
        const std::size_t stride = static_cast<std::size_t>(out_width) * 4u;
        const float *r0 = wide.data() + tap[0] * stride, *r1 = wide.data() + tap[1] * stride;
        const float *r2 = wide.data() + tap[2] * stride, *r3 = wide.data() + tap[3] * stride;
        const std::size_t cell_row = static_cast<std::size_t>(ay.cell[oy]) * cells_x;
        std::byte *out = result.data() + static_cast<std::size_t>(oy) * stride;
        for (std::uint32_t ox = 0u; ox < out_width; ++ox) {
            const std::size_t column = static_cast<std::size_t>(ox) * 4u;
            const std::size_t cell = (cell_row + ax.cell[ox]) * 4u;
            for (std::size_t c = 0u; c < 4u; ++c) {
                const std::size_t i = column + c;
                const float lo = cell_lo[cell + c];
                const float hi = cell_hi[cell + c];
                float value = std::clamp(w[0] * r0[i] + w[1] * r1[i] + w[2] * r2[i] + w[3] * r3[i], lo, hi);
                const float range = hi - lo;
                if (sharp && range > kRangeStart) {
                    const float blend = std::min(1.0f, (range - kRangeStart) / (kRangeFull - kRangeStart));
                    float s = std::clamp(((value - lo) / range - 0.5f) * kContrast + 0.5f, 0.0f, 1.0f);
                    s = s * s * (3.0f - 2.0f * s);
                    value += blend * (lo + s * range - value);
                }
                out[i] = static_cast<std::byte>(static_cast<std::uint8_t>(value + 0.5f));
            }
        }
    }
    return result;
}

// Number of levels in a full chain for this size, capped at the backend's 8.
inline std::uint32_t mip_level_count(std::uint32_t width, std::uint32_t height) noexcept {
    std::uint32_t levels = 1u;
    while (levels < 8u && (width > 1u || height > 1u)) {
        width = std::max(1u, width >> 1u);
        height = std::max(1u, height >> 1u);
        ++levels;
    }
    return levels;
}

// Appends `levels - 1` box-filtered levels to `base` (sizes halve with
// max(1, n >> 1), the layout prepare_texture_upload expects). Colour is
// averaged with alpha weighting so transparent texels never darken an edge.
inline std::vector<std::byte> build_mip_chain(std::vector<std::byte> base, std::uint32_t width,
                                              std::uint32_t height, std::uint32_t levels) {
    std::size_t total = 0u;
    for (std::uint32_t level = 0u, w = width, h = height; level < levels; ++level) {
        total += static_cast<std::size_t>(w) * h * 4u;
        w = std::max(1u, w >> 1u);
        h = std::max(1u, h >> 1u);
    }
    base.resize(total);
    std::size_t source_offset = 0u;
    std::uint32_t w = width, h = height;
    for (std::uint32_t level = 1u; level < levels; ++level) {
        const std::uint32_t nw = std::max(1u, w >> 1u);
        const std::uint32_t nh = std::max(1u, h >> 1u);
        const std::size_t target_offset = source_offset + static_cast<std::size_t>(w) * h * 4u;
        for (std::uint32_t y = 0u; y < nh; ++y) {
            for (std::uint32_t x = 0u; x < nw; ++x) {
                float rgb[3]{};
                float plain[3]{};
                float alpha = 0.0f;
                for (std::uint32_t dy = 0u; dy < 2u; ++dy) {
                    for (std::uint32_t dx = 0u; dx < 2u; ++dx) {
                        const std::uint32_t sx = std::min(w - 1u, x * 2u + dx);
                        const std::uint32_t sy = std::min(h - 1u, y * 2u + dy);
                        const std::byte *p = base.data() + source_offset +
                                             (static_cast<std::size_t>(sy) * w + sx) * 4u;
                        const float a = static_cast<float>(static_cast<std::uint8_t>(p[3]));
                        for (int c = 0; c < 3; ++c) {
                            const float v = static_cast<float>(static_cast<std::uint8_t>(p[c]));
                            rgb[c] += v * a;
                            plain[c] += v;
                        }
                        alpha += a;
                    }
                }
                std::byte *out = base.data() + target_offset + (static_cast<std::size_t>(y) * nw + x) * 4u;
                for (int c = 0; c < 3; ++c) {
                    const float v = alpha > 0.0f ? rgb[c] / alpha : plain[c] * 0.25f;
                    out[c] = static_cast<std::byte>(static_cast<std::uint8_t>(std::clamp(v + 0.5f, 0.0f, 255.0f)));
                }
                out[3] = static_cast<std::byte>(static_cast<std::uint8_t>(
                    std::clamp(alpha * 0.25f + 0.5f, 0.0f, 255.0f)));
            }
        }
        source_offset = target_offset;
        w = nw;
        h = nh;
    }
    return base;
}

// ---- DirectDraw Surface I/O (uncompressed 32-bit only) ----------------------
//
// Dumps are written as classic A8R8G8B8 (B,G,R,A bytes in memory), which GIMP,
// Paint.NET, Photoshop (with the Intel/NVIDIA plugins) and texconv all read.
// Replacements accept any 32-bit RGB(A) bit-mask layout, or a DX10 header with
// R8G8B8A8/B8G8R8A8 (UNORM or SRGB). Only the top level is read; the chain is
// rebuilt here. Block-compressed files are rejected with a clear message.

namespace detail {

inline void put_u32(std::vector<std::uint8_t> &out, std::uint32_t value) {
    for (unsigned shift = 0u; shift < 32u; shift += 8u)
        out.push_back(static_cast<std::uint8_t>(value >> shift));
}

inline std::uint32_t get_u32(const std::uint8_t *p) noexcept {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8u) |
           (static_cast<std::uint32_t>(p[2]) << 16u) | (static_cast<std::uint32_t>(p[3]) << 24u);
}

inline std::uint8_t extract_channel(std::uint32_t pixel, std::uint32_t mask) noexcept {
    if (mask == 0u) return 0u;
    unsigned shift = 0u;
    while (((mask >> shift) & 1u) == 0u) ++shift;
    const std::uint32_t max = mask >> shift;
    const std::uint32_t value = (pixel & mask) >> shift;
    return static_cast<std::uint8_t>((value * 255u + max / 2u) / max);
}

} // namespace detail

inline bool write_dds(const std::filesystem::path &path, std::uint32_t width, std::uint32_t height,
                      const std::byte *rgba) {
    std::vector<std::uint8_t> out;
    out.reserve(128u + static_cast<std::size_t>(width) * height * 4u);
    out.insert(out.end(), {'D', 'D', 'S', ' '});
    detail::put_u32(out, 124u);                         // dwSize
    detail::put_u32(out, 0x1u | 0x2u | 0x4u | 0x8u | 0x1000u); // CAPS|HEIGHT|WIDTH|PITCH|PIXELFORMAT
    detail::put_u32(out, height);
    detail::put_u32(out, width);
    detail::put_u32(out, width * 4u);                   // pitch
    detail::put_u32(out, 0u);                           // depth
    detail::put_u32(out, 0u);                           // mip count
    for (int i = 0; i < 11; ++i) detail::put_u32(out, 0u);
    detail::put_u32(out, 32u);                          // ddspf.dwSize
    detail::put_u32(out, 0x40u | 0x1u);                 // DDPF_RGB | DDPF_ALPHAPIXELS
    detail::put_u32(out, 0u);                           // fourCC
    detail::put_u32(out, 32u);
    detail::put_u32(out, 0x00FF0000u);
    detail::put_u32(out, 0x0000FF00u);
    detail::put_u32(out, 0x000000FFu);
    detail::put_u32(out, 0xFF000000u);
    detail::put_u32(out, 0x1000u);                      // DDSCAPS_TEXTURE
    for (int i = 0; i < 4; ++i) detail::put_u32(out, 0u);
    const std::size_t count = static_cast<std::size_t>(width) * height;
    for (std::size_t i = 0u; i < count; ++i) {
        const std::byte *p = rgba + i * 4u;
        out.push_back(static_cast<std::uint8_t>(p[2]));
        out.push_back(static_cast<std::uint8_t>(p[1]));
        out.push_back(static_cast<std::uint8_t>(p[0]));
        out.push_back(static_cast<std::uint8_t>(p[3]));
    }
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) return false;
    file.write(reinterpret_cast<const char *>(out.data()), static_cast<std::streamsize>(out.size()));
    return static_cast<bool>(file);
}

inline bool read_dds(const std::filesystem::path &path, Image &image, std::string &error) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { error = "cannot open"; return false; }
    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (data.size() < 128u || std::memcmp(data.data(), "DDS ", 4u) != 0 ||
        detail::get_u32(data.data() + 4u) != 124u) {
        error = "not a DDS file";
        return false;
    }
    const std::uint32_t height = detail::get_u32(data.data() + 12u);
    const std::uint32_t width = detail::get_u32(data.data() + 16u);
    const std::uint8_t *pf = data.data() + 76u;
    const std::uint32_t pf_flags = detail::get_u32(pf + 4u);
    const std::uint32_t fourcc = detail::get_u32(pf + 8u);
    if (width == 0u || height == 0u || width > 8192u || height > 8192u) {
        error = "unsupported size";
        return false;
    }
    std::size_t offset = 128u;
    std::uint32_t masks[4]{};
    if ((pf_flags & 0x4u) != 0u && fourcc == 0x30315844u) { // 'DX10'
        if (data.size() < 148u) { error = "truncated DX10 header"; return false; }
        const std::uint32_t format = detail::get_u32(data.data() + 128u);
        offset = 148u;
        if (format == 28u || format == 29u) {       // R8G8B8A8_UNORM(_SRGB)
            masks[0] = 0x000000FFu; masks[1] = 0x0000FF00u; masks[2] = 0x00FF0000u; masks[3] = 0xFF000000u;
        } else if (format == 87u || format == 91u) { // B8G8R8A8_UNORM(_SRGB)
            masks[0] = 0x00FF0000u; masks[1] = 0x0000FF00u; masks[2] = 0x000000FFu; masks[3] = 0xFF000000u;
        } else {
            error = "DXGI format " + std::to_string(format) + " is not supported; save as uncompressed A8R8G8B8";
            return false;
        }
    } else if ((pf_flags & 0x40u) != 0u && detail::get_u32(pf + 12u) == 32u) {
        masks[0] = detail::get_u32(pf + 16u);
        masks[1] = detail::get_u32(pf + 20u);
        masks[2] = detail::get_u32(pf + 24u);
        masks[3] = (pf_flags & 0x1u) != 0u ? detail::get_u32(pf + 28u) : 0u;
    } else {
        error = "compressed or non-32-bit DDS is not supported; save as uncompressed A8R8G8B8";
        return false;
    }
    const std::size_t count = static_cast<std::size_t>(width) * height;
    if (data.size() - offset < count * 4u) { error = "truncated pixel data"; return false; }
    image.width = width;
    image.height = height;
    image.rgba.resize(count * 4u);
    for (std::size_t i = 0u; i < count; ++i) {
        const std::uint32_t pixel = detail::get_u32(data.data() + offset + i * 4u);
        for (int c = 0; c < 3; ++c)
            image.rgba[i * 4u + static_cast<std::size_t>(c)] =
                static_cast<std::byte>(detail::extract_channel(pixel, masks[c]));
        image.rgba[i * 4u + 3u] = static_cast<std::byte>(
            masks[3] != 0u ? detail::extract_channel(pixel, masks[3]) : 255u);
    }
    return true;
}

} // namespace vcs::hud_texture
