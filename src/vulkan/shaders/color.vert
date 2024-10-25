#version 450

layout(push_constant) uniform PushConstants {
    mat4 u_model_view;
    mat4 u_projection;
    vec4 u_color;
} push_constants;

layout(location = 0) in vec2 coords2d;

layout(location = 0) out vec4 out_color;


void main() {
    gl_Position = push_constants.u_projection * push_constants.u_model_view * vec4(coords2d, 0.0, 1.0);
    out_color = push_constants.u_color;
}
