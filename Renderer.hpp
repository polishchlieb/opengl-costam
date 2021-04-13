#ifndef UNTITLED_RENDERER_HPP
#define UNTITLED_RENDERER_HPP

#include "Buffer.hpp"
#include <unordered_set>
#include <glm/glm.hpp>

class Renderer {
public:
	static void init();
	static void shutdown();

	static void beginBatch();
	static void endBatch();
	static void render();

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID);
	static void drawScrollingQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, float& scrollX);

	static void clear();
};

#endif //UNTITLED_RENDERER_HPP
