#include "Renderer.hpp"

#include <cstring>
#include <glad/glad.h>
#include <set>
#include <array>
#include <stdexcept>
#include "Vertex.hpp"

static const size_t maxQuads = 1000;
static const size_t maxVertices = maxQuads * 4;
static const size_t maxIndices = maxQuads * 6;
static GLint maxTextures;

struct RendererData {
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;

	GLuint whiteTexture = 0;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer = nullptr;
	Vertex* quadBufferPtr = nullptr;

	std::set<uint32_t> textureSlots;

	uint32_t textureSlotIndex = 1;
};

static RendererData data;

void Renderer::init() {
	if (data.quadBuffer != nullptr)
		throw std::runtime_error("Renderer::init was called twice");

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	--maxTextures; // - white texture slot

	data.quadBuffer = new Vertex[maxVertices];

	glCreateVertexArrays(1, &data.quadVA);
	glBindVertexArray(data.quadVA);

	glCreateBuffers(1, &data.quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(data.quadVA, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, position));

	glEnableVertexArrayAttrib(data.quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, color));

	glEnableVertexArrayAttrib(data.quadVA, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, texCoords));

	glEnableVertexArrayAttrib(data.quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, texIndex));

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

	glCreateTextures(GL_TEXTURE_2D, 1, &data.whiteTexture);
	glBindTexture(GL_TEXTURE_2D, data.whiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	// data.textureSlots.reserve(maxTextures);
	// data.textureSlots[0] = data.whiteTexture;
	data.textureSlots.insert(data.whiteTexture);
	// for (size_t i = 1; i < maxTextures; ++i)
		// data.textureSlots[i] = 0;
}

void Renderer::shutdown() {
	glDeleteVertexArrays(1, &data.quadVA);
	glDeleteBuffers(1, &data.quadVB);
	glDeleteBuffers(1, &data.quadIB);

	glDeleteTextures(1, &data.whiteTexture);

	delete[] data.quadBuffer;
}

void Renderer::beginBatch() {
	data.quadBufferPtr = data.quadBuffer;
}

void Renderer::endBatch() {
	GLsizeiptr size = (uint8_t*)data.quadBufferPtr - (uint8_t*)data.quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.quadBuffer);
}

void Renderer::render() {
	/* for (uint32_t i = 0; i < data.textureSlotIndex; ++i)
		glBindTextureUnit(i, data.textureSlots[i]); */
	uint8_t i = 0;
	for (const auto textureID : data.textureSlots) {
		glBindTextureUnit(i, textureID);
		++i;
	}

	glBindVertexArray(data.quadVA);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);
	
	data.indexCount = 0;
	data.textureSlotIndex = 1;
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if (data.indexCount >= maxIndices) {
		endBatch();
		render();
		beginBatch();
	}

	float textureIndex = 0.f;

	data.quadBufferPtr->position = {position.x, position.y};
	data.quadBufferPtr->color = {1.f, 0.f, 0.f, 1.f};
	data.quadBufferPtr->texCoords = {0.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = {0.f, 1.f, 0.f, 1.f};
	data.quadBufferPtr->texCoords = {1.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = {0.f, 1.f, 0.f, 1.f};
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = {1.f, 0.f, 0.f, 1.f};
	data.quadBufferPtr->texCoords = {0.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer::drawScrollingQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, float scrollX) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	// todo: some tint maybe?
	const glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

	float textureIndex = 0.f;
	/* for (uint32_t i = 1; i < data.textureSlotIndex; ++i) {
		if (data.textureSlots[i] == textureID) {
			textureIndex = static_cast<float>(i);
			break;
		}
	} */
	if (data.textureSlots.contains(textureID))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(textureID)
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		/* data.textureSlots[data.textureSlotIndex] = textureID;
		data.textureSlotIndex++; */
		data.textureSlots.insert(textureID);
	}

	data.quadBufferPtr->position = { position.x, position.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.f + scrollX, 0.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x, position.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.f + scrollX, 0.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x, position.y + size.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.f + scrollX, 1.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x, position.y + size.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.f + scrollX, 1.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

#include <iostream>

void Renderer::drawGlyph(const glm::vec2& position, const glm::vec2& size, const Glyph& glyph) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	// todo: some tint maybe?
	const glm::vec4 color = { 1.f, 0.f, 0.f, 1.f };

	float textureIndex = 0.f;
	/* for (uint32_t i = 1; i < data.textureSlotIndex; ++i) {
		if (data.textureSlots[i] == glyph.textureID) {
			textureIndex = static_cast<float>(i);
			break;
		}
	} */
	if (data.textureSlots.contains(glyph.textureID))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(glyph.textureID)
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		// data.textureSlots[data.textureSlotIndex] = glyph.textureID;
		data.textureSlots.insert(glyph.textureID);
		data.textureSlotIndex++;
	}

	const auto glyphsPerX = glyph.spriteWidth / glyph.width;
	const auto glyphsPerY = glyph.spriteHeight / glyph.height;

	const auto x = (glyph.index % glyphsPerX) * glyph.width;
	const auto y = glyph.spriteHeight - (glyph.index / glyphsPerY + 1) * glyph.height;

	const auto toSpriteSize = [&glyph](int value) {
		return static_cast<float>(value) / static_cast<float>(glyph.spriteWidth);
	};

	data.quadBufferPtr->position = {position.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {
		toSpriteSize(x),
		toSpriteSize(y)
	};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {
		toSpriteSize(x + glyph.baseWidth),
		toSpriteSize(y)
	};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {
		toSpriteSize(x + glyph.baseWidth),
		toSpriteSize(y + glyph.height)
	};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {
		toSpriteSize(x),
		toSpriteSize(y + glyph.height)
	};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	// todo: some tint maybe?
	const glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

	float textureIndex = 0.f;
	if (data.textureSlots.contains(textureID))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(textureID)
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		// data.textureSlots[data.textureSlotIndex] = textureID;
		data.textureSlots.insert(textureID);
		data.textureSlotIndex++;
	}

	data.quadBufferPtr->position = { position.x, position.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x, position.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 0.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x + size.x, position.y + size.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {1.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = { position.x, position.y + size.y };
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = {0.f, 1.f};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}
