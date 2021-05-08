#ifndef UNTITLED_SHADER_HPP
#define UNTITLED_SHADER_HPP

#include "ShaderComponent.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>

class Shader {
public:
    Shader();
    ~Shader();

    void attach(const ShaderComponent& component);
    void bind() const;
    void unbind() const;
    void link() const;
    void validate() const;
    void createSamplers();

    void setUniform1i(const std::string& name, int value);
    void setUniformSampler2D(const std::string& name, int value);
    template<size_t size>
    void setUniform1iv(const std::string& name, std::array<int, size>& array) {
        glUniform1iv(
            getUniformLocation(name),
            static_cast<GLsizei>(array.size()),
            array.data()
        );
    }
    void setUniformVec4(const std::string& name, float f0, float f1, float f2, float f3);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void setUniformVec3(const std::string& name, const glm::vec3& value);
private:
    unsigned int program;
    bool bound = false;

    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
    GLint getUniformLocation(const std::string& name) const;
};

#endif //UNTITLED_SHADER_HPP
