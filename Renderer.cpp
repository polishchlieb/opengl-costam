#include "Renderer.hpp"

#include <cstring>
#include <glad/glad.h>
#include <set>

static constexpr size_t maxBoundTextures = 32;

void Renderer::render(const std::vector<Quad>& quads, const Buffer& vb) {
	std::set<const Texture*> textures;
	for (const auto& quad : quads)
		textures.insert(&quad.texture);
    {
        size_t i = 0;
        for (const auto texture : textures) {
            texture->bind(i);
            ++i;
        }
    }

    auto* vertices = new Vertex[quads.size() * 4];
    for (size_t i = 0; i < quads.size(); ++i)
        memcpy(
            vertices + i * 4,
            const_cast<Quad&>(quads[i]).data(),
            quads[i].size() * sizeof(Vertex)
        );

    glBindBuffer(GL_ARRAY_BUFFER, vb.getId());
    glBufferSubData(GL_ARRAY_BUFFER, 0, quads.size() * 4 * sizeof(Vertex), vertices);
    delete[] vertices;

    // 6 is the element count of one quad
    glDrawElements(GL_TRIANGLES, 6 * quads.size(), GL_UNSIGNED_INT, nullptr);
}

