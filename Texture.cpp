#include "Texture.hpp"
#include <iostream>
#include <stb_image.h>

Texture::Texture(const std::string& path)
    : width(0), height(0), bpp(0)
{
    int width, height, bpp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);

    glCreateTextures(GL_TEXTURE_2D, 1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    if (buffer)
        stbi_image_free(buffer);
}

Texture::Texture()
    : id(0), width(0), height(0), bpp(0) {}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot) const {
    glBindTextureUnit(slot, id);
}
