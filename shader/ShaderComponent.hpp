#ifndef UNTITLED_SHADERCOMPONENT_HPP
#define UNTITLED_SHADERCOMPONENT_HPP

#include <string>

class Shader;

class ShaderComponent {
public:
    static ShaderComponent fromString(unsigned int type, const std::string& source);
    static ShaderComponent fromFile(unsigned int type, const std::string& fileName);

    ~ShaderComponent();
private:
    friend Shader;
    unsigned int id;

    explicit ShaderComponent(unsigned int type);

    void load(const std::string& source);
    void compile();
};

#endif //UNTITLED_SHADERCOMPONENT_HPP
