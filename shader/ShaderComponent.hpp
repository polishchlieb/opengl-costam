#ifndef UNTITLED_SHADERCOMPONENT_HPP
#define UNTITLED_SHADERCOMPONENT_HPP

#include <string>
#include <glad/glad.h>

class Shader;

class ShaderComponent {
public:
    static enum class ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER
    };

    static ShaderComponent fromString(ShaderType type, const std::string& source);
    static ShaderComponent fromFile(ShaderType type, const std::string& fileName);

    ~ShaderComponent();
private:
    friend Shader;
    unsigned int id;

    explicit ShaderComponent(ShaderType type);

    void load(const std::string& source);
    void compile();
};

#endif //UNTITLED_SHADERCOMPONENT_HPP
