#ifdef CPP_GLSL_INCLUDE
std::string color_frag = R"(
#version 330 core

layout (location = 0) out vec4 fragmentColor;

in vec4 out_color;

void main() {
    // simple pass through
    fragmentColor = out_color;
}

)";
#endif
