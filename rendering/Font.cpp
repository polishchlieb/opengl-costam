#include "Font.hpp"

uint16_t Font::getGlyphWidth(uint16_t index) {
	return glyphWidths[index];
}

Font::Font(GLuint textureID, std::vector<uint16_t> glyphWidths, uint16_t spriteWidth, uint16_t spriteHeight, uint16_t glyphWidth, uint16_t glyphHeight)
	: textureID(textureID), glyphWidths(std::move(glyphWidths)), spriteWidth(spriteWidth), spriteHeight(spriteHeight), glyphWidth(glyphWidth), glyphHeight(glyphHeight) {}

Glyph Font::getGlyph(uint16_t index, glm::vec4 color) {
	return Glyph{
		textureID,
		index,
		glyphWidth, glyphHeight,
		spriteWidth, spriteHeight,
		glyphWidths[index],
		color
	};
}
