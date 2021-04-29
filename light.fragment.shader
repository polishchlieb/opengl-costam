#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TextureIndex;

void main() {
    float distance = length(v_TexCoord);
    color = vec4(v_Color.rgb, v_Color.a * (pow(0.01, distance) - 0.01));
}
