#include "Shader.hpp"
#include <glad/glad.h>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

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

void Shader::bind() const {
    glUseProgram(program);
}

void Shader::setUniform4f(const std::string& name, float f0, float f1, float f2, float f3) {
    glUniform4f(getUniformLocation(name), f0, f1, f2, f3);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

GLint Shader::getUniformLocation(const std::string& name) const {
    if (uniformLocationCache.contains(name))
        return uniformLocationCache[name];

    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1)
        throw std::runtime_error("couldn't find the uniform location");

    uniformLocationCache[name] = location;

    return location;
}
