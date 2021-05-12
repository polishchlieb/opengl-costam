#include "Font.hpp"
#include <glad/glad.h>
#include <freetype/freetype.h>
#include <stdexcept>

Font::Font(const std::string& filePath, size_t size) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		throw std::runtime_error("couldn't init freetype");

	FT_Face face;
	if (FT_New_Face(ft, filePath.c_str(), 0, &face))
		throw std::runtime_error("couldn't load the font: " + filePath);

	FT_Set_Pixel_Sizes(face, 0, size);

	for (uint8_t c = 0; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			throw std::runtime_error("couldn't load glyphs: " + filePath);

		auto size = face->glyph->bitmap.width * face->glyph->bitmap.rows;
		auto* buffer = new unsigned char[static_cast<uint64_t>(size * 4)];

		auto* bufferCursor = buffer - 1;
		for (unsigned int i = 0; i < size; ++i) {
			*(++bufferCursor) = 255;
			*(++bufferCursor) = 255;
			*(++bufferCursor) = 255;
			*(++bufferCursor) = face->glyph->bitmap.buffer[i];
		}

		Character character{
			std::make_unique<Texture>(
				buffer, glm::uvec2{face->glyph->bitmap.width, face->glyph->bitmap.rows}
			),
			{face->glyph->bitmap_left, face->glyph->bitmap_top},
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		delete[] buffer;

		characters.insert({c, std::move(character)});
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

float Font::measureText(const std::string& text, float scale) const {
	float result = 0.f;

	for (char c : text) {
		const auto& ch = characters.find(c)->second;
		result += (ch.advance >> 6) * scale;
	}

	return result;
}
