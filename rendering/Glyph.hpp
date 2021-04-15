#pragma once
#include <glad/glad.h>
#include <cstdint>

struct Glyph {
	GLuint textureID;
	uint16_t index;
	uint16_t width;
	uint16_t height;
	uint16_t spriteWidth;
	uint16_t spriteHeight;
	uint16_t baseWidth;
};