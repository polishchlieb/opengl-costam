#include "Shader.hpp"
#include <glad/glad.h>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <array>

Shader::Shader() {
    program = glCreateProgram();
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::attach(const ShaderComponent& component) {
    glAttachShader(program, component.id);
}

void Shader::link() const {
    glLinkProgram(program);
}

void Shader::validate() const {
    glValidateProgram(program);
}

void Shader::createSamplers() {
    std::array<int, 32> samplers;
    for (uint16_t i = 0; i < 32; ++i)
        samplers[i] = i;
    setUniform1iv("textures", samplers);
}

void Shader::bind() const {
    glUseProgram(program);
}

void Shader::setUniformFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformVec4(const std::string& name, float f0, float f1, float f2, float f3) {
    glUniform4f(getUniformLocation(name), f0, f1, f2, f3);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformSampler2D(const std::string& name, int value) {
    setUniform1i(name, value);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

int Shader::getUniformLocation(const std::string& name) const {
    if (uniformLocationCache.contains(name))
        return uniformLocationCache[name];

    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1)
        throw std::runtime_error("couldn't find the uniform location");

    uniformLocationCache[name] = location;

    return location;
}
