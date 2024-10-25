#version 450

layout(push_constant) uniform PushConstants {
    mat4 u_model_view;
    mat4 u_projection;
} push_constants;

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_tex_coords;

layout (location = 0) out vec2 tex_coord_out;

void main() {
    gl_Position = push_constants.u_projection * push_constants.u_model_view * vec4(in_position, 0.0, 1.0);
    tex_coord_out = in_tex_coords;
}
