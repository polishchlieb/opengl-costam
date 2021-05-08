#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 u_MVP;

out vec3 fragmentPos;
out vec2 texCoord;
out vec3 normal;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    texCoord = vTexCoord;
    fragmentPos = vec3(model * vec4(position, 1.0));
    
    // TODO: change normal to uniform and calculate this on the CPU side
    normal = mat3(transpose(inverse(model))) * vNormal;
}