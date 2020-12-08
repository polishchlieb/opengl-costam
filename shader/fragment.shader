#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TextureIndex;
in float v_Invert;

uniform sampler2D u_Textures[2];

void main() {
    int index = int(v_TextureIndex);
    vec4 textureColor = texture(u_Textures[index], v_TexCoord);
    if (v_Invert == 1.0)
        color = vec4(1.0 - textureColor.r, 1.0 - textureColor.g, 1.0 - textureColor.b, textureColor.a);
    else
        color = textureColor;
}
