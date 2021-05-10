#version 330 core

layout(location = 0) out vec4 color;

in vec4 fColor;
in vec2 texCoords;
in float texIndex;

uniform sampler2D textures[32];

void main() {
  vec4 sampled = texture(textures[int(texIndex)], texCoords);
  color = sampled * fColor;
}