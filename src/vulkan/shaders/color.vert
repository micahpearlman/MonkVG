#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 u_model_view;
    mat4 u_projection;
    vec4 u_color;
};

layout(location = 0) in vec2 coords2d;

layout(location = 0) out vec4 out_color;


void main() {
    gl_Position = u_projection * u_model_view * vec4(coords2d, 0.0, 1.0);
    out_color = u_color;
}
