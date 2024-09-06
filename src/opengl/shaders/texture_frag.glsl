#ifdef CPP_GLSL_INCLUDE
std::string texture_frag = R"(
#version 330 core

layout (location = 0) out vec4 fragmentColor;

in vec2 tex_coord_out;
uniform sampler2D tex1;

void main() {
    fragmentColor = texture(tex1, tex_coord_out);
}

)";
#endif
