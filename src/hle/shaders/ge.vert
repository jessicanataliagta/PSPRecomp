#version 450
#include "ge_common.glsl"

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in float in_fog;
layout(location = 4) in float in_q;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out float out_q;
layout(location = 3) out float out_fog;

void main() {
    if (pc.control.x == 1u) {
        vec4 p = in_position;
        float clip_w = dot(pc.row3, p);
        if (abs(clip_w) < 1.0e-12) clip_w = 1.0;
        float clip_z = dot(pc.row2, p);
        if (pc.control.y == 0u) clip_z = clamp(clip_z, 0.0, clip_w);
        gl_Position = vec4(dot(pc.row0, p), dot(pc.row1, p), clip_z, clip_w);
        out_uv = in_uv * pc.uv.xy + pc.uv.zw;
        out_fog = clamp((dot(pc.view_z, p) + pc.fog.x) * pc.fog.y, 0.0, 1.0);
    } else {
        // PSP screen coordinates (already viewport-transformed) in this
        // target's logical extent. Vulkan clip space has +Y down like the PSP.
        float clip_w = in_position.w;
        if (abs(clip_w) < 1.0e-12) clip_w = 1.0;
        gl_Position = vec4((in_position.x * pc.uv.x - 1.0) * clip_w,
                           (in_position.y * pc.uv.y - 1.0) * clip_w,
                           clamp(in_position.z * pc.uv.z, 0.0, 1.0) * clip_w,
                           clip_w);
        out_uv = in_uv;
        out_fog = in_fog;
    }
    out_color = in_color;
    if (pc.control.z != 0u) {
        vec4 lit = clamp(out_color * pc.color_mul + pc.color_add, 0.0, 1.0);
        out_color = floor(lit * 255.0) * (1.0 / 255.0);
    }
    out_q = in_q;
}
