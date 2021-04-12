#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float textureIndex;

uniform mat4 u_MVP;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TextureIndex;

void main() {
    v_Color = color;
    v_TexCoord = texCoord;
    v_TextureIndex = textureIndex;
    gl_Position = u_MVP * position;
}