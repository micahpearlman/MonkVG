#version 450

// layout(set = 0, binding = 0) uniform UBO {    
//     vec4 u_color;
// };

layout (set = 0, binding = 1) uniform sampler2D tex1;

layout(location = 0) out vec4 fragmentColor;
layout(location = 0) in vec2 out_tex_coords;


void main() {
    // fragmentColor = vec4(0.6353, 0.9725, 0.6118, 1.0);// texture(tex1, tex_coord_out);// * u_color;
    fragmentColor = texture(tex1, out_tex_coords);// * u_color;
}
