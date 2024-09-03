#version 330 core

layout (location = 0) out vec4 fragmentColor;

smooth in vec4 out_color;

void main() {
    fragmentColor = out_color;
}
