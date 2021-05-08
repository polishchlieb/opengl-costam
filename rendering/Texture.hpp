#ifndef UNTITLED_TEXTURE_HPP
#define UNTITLED_TEXTURE_HPP

#include <string>
#include <iostream>
#include <glad/glad.h>

class Texture {
public:
    explicit Texture(const std::string& path, GLint wrap = GL_CLAMP_TO_EDGE);
    Texture();
    ~Texture();

    void bind(unsigned int slot = 0) const;

    inline int getWidth() const { return width; }
    int getHeight() const { return height; }
    unsigned int getId() const { return id; }
protected:
    unsigned int id;
    int width, height, bpp;
};

#endif //UNTITLED_TEXTURE_HPP
