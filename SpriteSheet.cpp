#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet(
	std::array<size_t, 2> spriteResolution, const Texture& texture
) : spriteResolution(spriteResolution), texture(texture) {}

void SpriteSheet::draw(const std::vector<SpriteSheetQuad>& quads, const Buffer& vb) {
	texture.bind(0);
	const auto id = texture.getSlot();

	size_t spritesPerLine = texture.getWidth() / spriteResolution[0];

	const float dx = static_cast<float>(spriteResolution[0]) / static_cast<float>(texture.getWidth());
	const float dy = static_cast<float>(spriteResolution[1]) / static_cast<float>(texture.getHeight());

	auto vertices = new Vertex[4 * quads.size()];
	for (size_t i = 0; i < quads.size(); ++i) {
		const auto& quad = quads[i];

		const float textureX = (quad.index % spritesPerLine) * dx;
		const float textureY = (1 - quad.index / spritesPerLine) * dy;

		Vertex v0{};
		v0.position = quad.pos1;
		v0.texCoords = { textureX, textureY };
		v0.color = {0.0f, 0.0f, 0.0f, 0.0f};
		v0.textureId = static_cast<float>(id);

		Vertex v1{};
		v1.position = quad.pos2;
		v1.texCoords = { textureX + dx, textureY };
		v1.color = { 0.0f, 0.0f, 0.0f, 0.0f };
		v1.textureId = static_cast<float>(id);

		Vertex v2{};
		v2.position = quad.pos3;
		v2.texCoords = { textureX + dx, textureY + dy };
		v2.color = { 0.0f, 0.0f, 0.0f, 0.0f };
		v2.textureId = static_cast<float>(id);

		Vertex v3{};
		v3.position = quad.pos4;
		v3.texCoords = { textureX, textureY + dy };
		v3.color = { 0.0f, 0.0f, 0.0f, 0.0f };
		v3.textureId = static_cast<float>(id);

		std::array<Vertex, 4> currentVertices = { v0, v1, v2, v3 };
		memcpy(
			vertices + i * 4,
			currentVertices.data(),
			currentVertices.size() * sizeof(Vertex)
		);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vb.getId());
	glBufferSubData(GL_ARRAY_BUFFER, 0, quads.size() * 4 * sizeof(Vertex), vertices);
	delete[] vertices;
	
	// 6 is the element count of one quad
	glDrawElements(GL_TRIANGLES, 6 * quads.size(), GL_UNSIGNED_INT, nullptr);
}