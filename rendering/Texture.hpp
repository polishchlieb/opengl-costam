#ifndef UNTITLED_TEXTURE_HPP
#define UNTITLED_TEXTURE_HPP

#include <string>
#include <iostream>
#include <glad/glad.h>

#include "Image.hpp"
#include <glm/ext/vector_uint2.hpp>

class Texture {
public:
    Texture(const std::string& path, GLint wrap = GL_CLAMP_TO_EDGE);
    Texture(const Image& image, GLint wrap = GL_CLAMP_TO_EDGE);
    Texture(const unsigned char* buffer, glm::uvec2 size, GLint wrap = GL_CLAMP_TO_EDGE);
    Texture();
    ~Texture();

    void bind(unsigned int slot = 0) const;

    inline int getWidth() const { return width; }
    int getHeight() const { return height; }
    unsigned int getId() const { return id; }
protected:
    unsigned int id;
    int width, height;

    void init(const unsigned char* buffer, glm::uvec2 size, GLint wrap = GL_CLAMP_TO_EDGE);
};

#endif //UNTITLED_TEXTURE_HPP
