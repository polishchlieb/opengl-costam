#include "Texture.hpp"
#include <iostream>
#include <stb_image.h>

Texture::Texture(const std::string& path, GLint wrap) {
    int bpp;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
    init(buffer, {width, height}, wrap);
    stbi_image_free(buffer);
}

Texture::Texture(const Image& image, GLint wrap)
    : width(image.getWidth()), height(image.getHeight())
{
    init(image.getBuffer(), {width, height}, wrap);
}

Texture::Texture(const unsigned char* buffer, glm::uvec2 size, GLint wrap)
    : width(size.x), height(size.y)
{
    init(buffer, size, wrap);
}

Texture::Texture()
    : id(0), width(0), height(0) {}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot) const {
    glBindTextureUnit(slot, id);
}

void Texture::init(const unsigned char* buffer, glm::uvec2 size, GLint wrap) {
    glCreateTextures(GL_TEXTURE_2D, 1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}
