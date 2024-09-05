#ifdef CPP_GLSL_INCLUDE
std::string color_vert = R"(
#version 330 core

uniform mat4 u_model_view;
uniform mat4 u_projection;
uniform vec4 u_color;

layout (location = 0) in vec2 coords2d;

out vec4 out_color;
void main() {
    gl_Position = u_projection * u_model_view * vec4(coords2d, 1.0, 1.0);
    out_color = u_color;
}

)";
#endif