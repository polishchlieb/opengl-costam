#include <iostream>
#include "Quad.hpp"

Quad::Quad(
    glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4,
    glm::vec4 color, const Texture& texture, int offset, bool invert
)
    : texture(texture), offset(offset)
{
    const auto id = texture.getSlot();

    Vertex v0{};
    v0.position = { pos1[0], pos1[1] };
    v0.color = color;
    v0.texCoords = { 0.0f, 0.0f };
    v0.textureId = static_cast<float>(id);
    v0.invert = invert;

    Vertex v1{};
    v1.position = { pos2[0], pos2[1] };
    v1.color = color;
    v1.texCoords = { 1.0f, 0.0f };
    v1.textureId = static_cast<float>(id);
    v1.invert = invert;

    Vertex v2{};
    v2.position = { pos3[0], pos3[1] };
    v2.color = color;
    v2.texCoords = { 1.0f, 1.0f };
    v2.textureId = static_cast<float>(id);
    v2.invert = invert;

    Vertex v3{};
    v3.position = { pos4[0], pos4[1] };
    v3.color = color;
    v3.texCoords = { 0.0f, 1.0f };
    v3.textureId = static_cast<float>(id);
    v3.invert = invert;

    vertices = { v0, v1, v2, v3 };
}

Vertex* Quad::data() {
    return vertices.data();
}

size_t Quad::size() const {
    return vertices.size();
}
