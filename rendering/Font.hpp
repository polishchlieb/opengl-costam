#pragma once
#include "Glyph.hpp"
#include <vector>

class Font {
public:
	Font(GLuint textureID, std::vector<uint16_t> glyphWidths, uint16_t spriteWidth, uint16_t spriteHeight, uint16_t glyphWidth, uint16_t glyphHeight);
	Glyph getGlyph(uint16_t index);
	uint16_t getGlyphWidth(uint16_t index);
private:
	GLuint textureID;
	std::vector<uint16_t> glyphWidths;
	uint16_t spriteWidth, spriteHeight, glyphWidth, glyphHeight;
};
