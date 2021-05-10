#ifndef UNTITLED_RENDERER_HPP
#define UNTITLED_RENDERER_HPP

#include <unordered_set>
#include <glm/glm.hpp>
#include <array>
#include "Vertex.hpp"
#include "Glyph.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	unsigned int textureID;
	glm::vec2 size;
	glm::vec2 bearing;
	unsigned int advance;
};

class Renderer2D {
public:
	static void init();
	static void shutdown();

	static void beginBatch();
	static void endBatch();
	static void render();

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID);
	static void drawScrollingQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, float scrollX);
	static void drawGlyph(const glm::vec2& position, const glm::vec2& size, const Glyph& glyph);
	static float drawText(glm::vec2 position, const std::string& value, float scale, const glm::vec4& color);
	static float measureText(const std::string& value, float scale = 1.f);
	static void drawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color);

	static void clear();
private:
	static void drawGlyph(glm::vec2 position, const Character& ch, float scale, const glm::vec4& color);
};

#endif //UNTITLED_RENDERER_HPP
