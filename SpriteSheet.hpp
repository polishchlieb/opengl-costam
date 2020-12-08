#pragma once
#include <array>
#include "Texture.hpp"
#include "Quad.hpp"
#include <vector>
#include "Buffer.hpp"

struct SpriteSheetQuad {
	unsigned int index;
	glm::vec2 pos1, pos2, pos3, pos4;
	bool invert = false;
};

class SpriteSheet {
public:
	SpriteSheet(
		std::array<size_t, 2> spriteResolution, const Texture& texture
	);

	void draw(const std::vector<SpriteSheetQuad>& quads, const Buffer& vb);
private:
	std::array<size_t, 2> spriteResolution;
	const Texture& texture;
};