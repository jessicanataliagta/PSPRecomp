// Shared push-constant block of the Vulkan GE backend (ge_gpu_vulkan.cpp,
// struct PushConstants). 208 bytes.
layout(push_constant, std430) uniform Push {
    vec4 row0;        // model -> Vulkan clip rows (hardware transform), see make_push_constants()
    vec4 row1;
    vec4 row2;
    vec4 row3;
    vec4 view_z;      // model -> view Z, for fog
    vec4 uv;          // hw: uv scale/offset; screen: 2/W, 2/H, 1/65535
    vec4 fog;         // fog end, fog slope
    uvec4 control;    // x: 1 hardware transform, 2 screen space; y: depth clip; z: affine vertex colour
    vec4 color_mul;
    vec4 color_add;
    uvec4 pixel;      // alpha control, texture control, texture env, fog control
    uvec4 pixel2;     // framebuffer format, fixed blend source colour, apply it
    vec4 sample_scale; // xy: scale from texture to render-target coordinates
} pc;
