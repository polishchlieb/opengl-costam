#include "Renderer2D.hpp"

#include <cstring>
#include <glad/glad.h>
#include <set>
#include <stdexcept>
#include "ColorTexture.hpp"

static const size_t maxQuads = 1000;
static const size_t maxVertices = maxQuads * 4;
static const size_t maxIndices = maxQuads * 6;
static GLint maxTextures;

struct RendererData {
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;
	GLuint dynamicIB = 0;

	Texture whiteTexture;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex2D* quadBuffer = nullptr;
	Vertex2D* quadBufferPtr = nullptr;

	std::set<uint32_t> textureSlots;

	uint32_t textureSlotIndex = 1;
};

static RendererData data;

void Renderer2D::init() {
	if (data.quadBuffer != nullptr)
		throw std::runtime_error("Renderer::init was called twice");

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	--maxTextures; // - white texture slot

	data.quadBuffer = new Vertex2D[maxVertices];

	glCreateVertexArrays(1, &data.quadVA);
	glBindVertexArray(data.quadVA);

	glCreateBuffers(1, &data.quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(data.quadVA, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*) offsetof(Vertex2D, position));

	glEnableVertexArrayAttrib(data.quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*) offsetof(Vertex2D, color));

	glEnableVertexArrayAttrib(data.quadVA, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*) offsetof(Vertex2D, texCoords));

	glEnableVertexArrayAttrib(data.quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*) offsetof(Vertex2D, texIndex));

	// move this to heap if there is a stack overflo 
	uint32_t indices[maxIndices];
	uint32_t offset = 0;
	for (uint32_t i = 0, offset = 0; i < maxIndices; i += 6, offset += 4) {
		indices[i] = offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;
		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = offset;
	}

	glCreateBuffers(1, &data.quadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.quadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	data.whiteTexture = ColorTexture{glm::vec4{1.f}};
	data.textureSlots.insert(data.whiteTexture.getId());
}

void Renderer2D::shutdown() {
	glDeleteVertexArrays(1, &data.quadVA);
	glDeleteBuffers(1, &data.quadVB);
	glDeleteBuffers(1, &data.quadIB);

	delete[] data.quadBuffer;
}

void Renderer2D::beginBatch() {
	data.quadBufferPtr = data.quadBuffer;
}

void Renderer2D::endBatch() {
	GLsizeiptr size = (uint8_t*)data.quadBufferPtr - (uint8_t*)data.quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.quadBuffer);
}

void Renderer2D::render() {
	uint8_t i = 0;
	for (const auto textureID : data.textureSlots) {
		glBindTextureUnit(i, textureID);
		++i;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.quadIB);
	glBindVertexArray(data.quadVA);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);
	
	data.indexCount = 0;
	data.textureSlotIndex = 1;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if (data.indexCount >= maxIndices) {
		endBatch();
		render();
		beginBatch();
	}

	float textureIndex = 0.f;

	data.quadBufferPtr->position = {position.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {-1.f, -1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, -1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {-1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer2D::drawScrollingQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture, float scrollX) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	// todo: some tint maybe?
	const glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

	float textureIndex = 0.f;
	if (data.textureSlots.contains(texture.getId()))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(texture.getId())
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		data.textureSlots.insert(texture.getId());
	}

	data.quadBufferPtr->position = {position.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.f + scrollX, 0.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f + scrollX, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f + scrollX, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f + scrollX, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

float Renderer2D::drawText(glm::vec2 position, const Font& font, const std::string& value, float scale, const glm::vec4& color) {
	float startX = position.x;
	for (char c : value) {
		const auto& ch = font.getChar(c);
		drawGlyph(position, ch, scale, color);
		position.x += (ch.advance >> 6) * scale;
	}
	return position.x - startX;
}

void Renderer2D::drawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color) {
	if (data.indexCount >= maxIndices) {
		endBatch();
		render();
		beginBatch();
	}

	float textureIndex = 0.f;

	data.quadBufferPtr->position = {p1.x, p1.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {p2.x, p2.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {p3.x, p3.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {p1.x, p1.y};
	data.quadBufferPtr->color = {0.f, 0.f, 0.f, 0.f};
	data.quadBufferPtr->texCoords = { 0.f, 1.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	// todo: some tint maybe?
	const glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

	float textureIndex = 0.f;
	if (data.textureSlots.contains(texture.getId()))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(texture.getId())
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		data.textureSlots.insert(texture.getId());
		data.textureSlotIndex++;
	}

	data.quadBufferPtr->position = {position.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 1.f}; 
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer2D::drawGlyph(glm::vec2 position, const Character& ch, float scale, const glm::vec4& color) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	float xPos = position.x + ch.bearing.x * scale;
	float yPos = position.y - (ch.texture->getHeight() - ch.bearing.y) * scale;

	float width = ch.texture->getWidth() * scale;
	float height = ch.texture->getHeight() * scale;

	float textureIndex = 0.f;
	if (data.textureSlots.contains(ch.texture->getId()))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(ch.texture->getId())
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		data.textureSlots.insert(ch.texture->getId());
		data.textureSlotIndex++;
	}

	data.quadBufferPtr->position = {xPos, yPos};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {xPos + width, yPos};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {xPos + width, yPos + height};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {xPos, yPos + height};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}
