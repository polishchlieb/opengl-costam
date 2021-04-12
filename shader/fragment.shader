#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TextureIndex;

uniform sampler2D u_Textures[32];

void main() {
    int index = int(v_TextureIndex);
    color = texture(u_Textures[index], v_TexCoord) * v_Color;
}
