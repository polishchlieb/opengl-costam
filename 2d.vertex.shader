#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexCoords;
layout(location = 3) in float vTexIndex;

uniform mat4 mvp;

out vec4 fColor;
out vec2 texCoords;
out float texIndex;

void main() {
  gl_Position = mvp * vec4(position, 0.0, 1.0);
  fColor = vColor;
  texCoords = vTexCoords;
  texIndex = vTexIndex;
}