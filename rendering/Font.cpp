#include "Font.hpp"

uint16_t Font::getGlyphWidth(uint16_t index) {
	return glyphWidths[index];
}

Glyph Font::getGlyph(uint16_t index) {
	return Glyph{
		textureID,
		index,
		glyphWidth, glyphHeight,
		spriteWidth, spriteHeight,
		glyphWidths[index]
	};
}
