#pragma once
/* DO NOT EDIT!  AUTOGENERATED BY CMAKE*/
const char* color_vert = R"(
#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

layout (location = 0) in vec2 coords2d;

smooth out vec4 out_color;

void main() {
    gl_Position = projection * view * model * vec4(coords2d, 0.0, 1.0);
    out_color = color;
}

)";