#include "ShaderComponent.hpp"
#include <glad/glad.h>
#include <fstream>

ShaderComponent::ShaderComponent(unsigned int type)
    : id(glCreateShader(type)) {}

ShaderComponent::~ShaderComponent() {
    glDeleteShader(id);
}

void ShaderComponent::load(const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
}

void ShaderComponent::compile() {
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        throw std::runtime_error("Couldn't compile the shader" + std::string(message));
    }
}

ShaderComponent ShaderComponent::fromString(unsigned int type, const std::string &source) {
    ShaderComponent component{type};
    component.load(source);
    component.compile();
    return component;
}

ShaderComponent ShaderComponent::fromFile(unsigned int type, const std::string &fileName) {
    std::ifstream file{fileName};
    if (!file.is_open())
        throw std::runtime_error("Couldn't load the file");

    std::string line, source;
    while (std::getline(file, line))
        source += line + '\n';

    ShaderComponent component{type};
    component.load(source);
    component.compile();
    return component;
}
