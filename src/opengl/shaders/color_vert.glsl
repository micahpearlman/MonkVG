#ifdef CPP_GLSL_INCLUDE
std::string color_vert = R"(
#version 330 core

uniform mat4 u_model_view;
uniform mat4 u_projection;
uniform vec4 u_color;

layout (location = 0) in vec2 coords2d;
// layout (location = 0) in vec3 vertexPosition;
// layout (location = 1) in vec3 vertexColor;

out vec4 out_color;
const vec3 vertices[3] = vec3[3](
    vec3(-0.5, -0.5, 0.0),  // Vertex 1
    vec3(0.5, -0.5, 0.0),   // Vertex 2
    vec3(0.0, 0.5, 0.0)     // Vertex 3
);
void main() {
    // gl_Position = vec4(vertices[gl_VertexID], 1.0);
    gl_Position = u_projection * u_model_view * vec4(coords2d, 1.0, 1.0);
    // gl_Position = u_projection * vec4(coords2d, 1.0, 1.0);
    // gl_Position = vec4(coords2d.x, coords2d.y, 0.0, 1.0);
    // gl_Position = u_projection * u_model_view * vec4(vertexPosition, 1.0);
    out_color = u_color;
}

)";
#endif