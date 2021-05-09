#include "ColorTexture.hpp"

ColorTexture::ColorTexture(const glm::vec4& color)
    : Texture{}
{
    glCreateTextures(GL_TEXTURE_2D, 1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    uint8_t rgba[4] = {
        static_cast<uint8_t>(color[0] * 255),
        static_cast<uint8_t>(color[1] * 255),
        static_cast<uint8_t>(color[2] * 255),
        static_cast<uint8_t>(color[3] * 255)
    };
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    glBindTexture(GL_TEXTURE_2D, 0);
}
