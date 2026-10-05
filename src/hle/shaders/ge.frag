#version 450
#include "ge_common.glsl"

layout(set = 0, binding = 0) uniform sampler2D source_texture;

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in float in_q;
layout(location = 3) in float in_fog;

layout(location = 0) out vec4 out_color;

uvec4 unpack_bytes(uint value) {
    return uvec4(value & 0xFFu, (value >> 8u) & 0xFFu, (value >> 16u) & 0xFFu, (value >> 24u) & 0xFFu);
}

bool alpha_pass(uint fn, uint lhs, uint rhs) {
    switch (fn & 7u) {
    case 0u: return false;
    case 1u: return true;
    case 2u: return lhs == rhs;
    case 3u: return lhs != rhs;
    case 4u: return lhs < rhs;
    case 5u: return lhs <= rhs;
    case 6u: return lhs > rhs;
    default: return lhs >= rhs;
    }
}

vec4 apply_texture_function(vec4 vertex_color, vec4 texel, uvec4 control, uvec4 env_bytes) {
    uint fn = control.x & 7u;
    bool use_alpha = control.y != 0u;
    vec4 result = vertex_color;
    vec3 env = vec3(env_bytes.xyz) / 255.0;
    if (fn == 0u) {         // MODULATE
        result.rgb = vertex_color.rgb * texel.rgb;
        result.a = use_alpha ? vertex_color.a * texel.a : vertex_color.a;
    } else if (fn == 1u) {  // DECAL
        result.rgb = mix(vertex_color.rgb, texel.rgb, use_alpha ? texel.a : 1.0);
        result.a = vertex_color.a;
    } else if (fn == 2u) {  // BLEND
        result.rgb = mix(vertex_color.rgb, env, texel.rgb);
        result.a = use_alpha ? vertex_color.a * texel.a : vertex_color.a;
    } else if (fn == 3u) {  // REPLACE
        result = texel;
        if (!use_alpha) result.a = vertex_color.a;
    } else if (fn == 4u) {  // ADD
        result.rgb = clamp(vertex_color.rgb + texel.rgb, 0.0, 1.0);
        result.a = use_alpha ? vertex_color.a * texel.a : vertex_color.a;
    }
    if (control.z != 0u) result.rgb = clamp(result.rgb * 2.0, 0.0, 1.0);
    return result;
}

float quantize(float value, float levels) {
    return floor(clamp(value, 0.0, 1.0) * levels + 0.5) / levels;
}

vec4 quantize_framebuffer(vec4 color, uint format) {
    color = clamp(color, 0.0, 1.0);
    if ((format & 3u) == 0u) {         // 5650
        color = vec4(quantize(color.r, 31.0), quantize(color.g, 63.0), quantize(color.b, 31.0), 1.0);
    } else if ((format & 3u) == 1u) {  // 5551
        color = vec4(quantize(color.r, 31.0), quantize(color.g, 31.0), quantize(color.b, 31.0),
                     color.a >= 0.5 ? 1.0 : 0.0);
    } else if ((format & 3u) == 2u) {  // 4444
        color = vec4(quantize(color.r, 15.0), quantize(color.g, 15.0), quantize(color.b, 15.0),
                     quantize(color.a, 15.0));
    }
    return color;
}

void main() {
    uvec4 alpha_control = unpack_bytes(pc.pixel.x);
    uvec4 texture_control = unpack_bytes(pc.pixel.y);
    uvec4 texture_env = unpack_bytes(pc.pixel.z);
    uvec4 fog_control = unpack_bytes(pc.pixel.w);
    vec4 color = clamp(in_color, 0.0, 1.0);
    if (texture_control.w != 0u) {
        float q = abs(in_q) < 1.0e-20 ? 1.0 : in_q;
        color = apply_texture_function(color, texture(source_texture, in_uv / q * pc.sample_scale.xy), texture_control,
                                       texture_env);
    }
    if (fog_control.w != 0u)
        color.rgb = mix(vec3(fog_control.xyz) / 255.0, color.rgb, clamp(in_fog, 0.0, 1.0));
    if (alpha_control.x != 0u) {
        uint a = uint(floor(clamp(color.a, 0.0, 1.0) * 255.0 + 0.5));
        uint mask = alpha_control.w;
        if (!alpha_pass(alpha_control.y, a & mask, alpha_control.z & mask)) discard;
    }
    // A fixed blend source colour is applied here (blend factor ONE), since
    // Vulkan's single blend constant is reserved for the destination's.
    if (pc.pixel2.z != 0u) color.rgb *= vec3(unpack_bytes(pc.pixel2.y).xyz) / 255.0;
    out_color = quantize_framebuffer(color, pc.pixel2.x);
}
