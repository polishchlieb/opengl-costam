#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D breadog;

void main() {
    // color = vec4(1.0, 1.0, 1.0, length(v_TexCoord));
    color = texture(breadog, v_TexCoord);
}