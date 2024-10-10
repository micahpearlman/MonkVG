#ifdef CPP_GLSL_INCLUDE
std::string texture_vert = R"(
#version 330 core

uniform mat4 u_model_view;
uniform mat4 u_projection;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;

out vec2 tex_coord_out;
void main() {
    gl_Position = u_projection * u_model_view * vec4(position, 1.0, 1.0);
    tex_coord_out = tex_coords;
}

)";
#endif