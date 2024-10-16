#version 450

layout (location = 0) out vec4 fragmentColor;

layout (location = 0) in vec4 out_color;

void main() {
    // simple pass through
    fragmentColor = out_color;
}
