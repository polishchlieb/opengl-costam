#ifndef UNTITLED_QUAD_HPP
#define UNTITLED_QUAD_HPP

#include <glm/glm.hpp>
#include <array>
#include "Texture.hpp"

struct Vertex {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 texCoords;
    float textureId;
    float invert = 0.0f;
};

class Quad {
public:
    Quad(
        glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4,
        glm::vec4 color, const Texture& texture, int offset = 0,
        bool invert = false
    );
    Quad() = default;

    Vertex* data();
    [[nodiscard]] size_t size() const;

    [[nodiscard]] inline unsigned int getTextureId() const {
        return texture.getId();
    }

    const Texture& texture;
    int offset;
private:
    std::array<Vertex, 4> vertices{};
};

#endif //UNTITLED_QUAD_HPP
