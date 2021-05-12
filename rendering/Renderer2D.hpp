#ifndef UNTITLED_RENDERER_HPP
#define UNTITLED_RENDERER_HPP

#include <unordered_set>
#include <glm/glm.hpp>
#include <array>
#include "Vertex.hpp"
#include "Font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

class Renderer2D {
public:
	static void init();
	static void shutdown();

	static void beginBatch();
	static void endBatch();
	static void render();

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture);
	static void drawScrollingQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture, float scrollX);
	static float drawText(glm::vec2 position, const Font& font, const std::string& value, float scale, const glm::vec4& color);
	static void drawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color);
private:
	static void drawGlyph(glm::vec2 position, const Character& ch, float scale, const glm::vec4& color);
};

#endif //UNTITLED_RENDERER_HPP
