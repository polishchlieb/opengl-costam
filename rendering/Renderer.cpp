#include "Renderer.hpp"

#include <cstring>
#include <glad/glad.h>
#include <set>
#include <stdexcept>

static const size_t maxQuads = 1000;
static const size_t maxVertices = maxQuads * 4;
static const size_t maxIndices = maxQuads * 6;
static GLint maxTextures;

struct RendererData {
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;
	GLuint dynamicIB = 0;

	GLuint whiteTexture = 0;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer = nullptr;
	Vertex* quadBufferPtr = nullptr;

	std::set<uint32_t> textureSlots;

	uint32_t textureSlotIndex = 1;
};

#include <unordered_map>
static std::unordered_map<char, Character> characters;

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

	data.textureSlots.insert(data.whiteTexture);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		throw std::runtime_error("couldn't init freetype");

	FT_Face face;
	if (FT_New_Face(ft, "res/fonts/comic.ttf", 0, &face))
		throw std::runtime_error("couldn't load the font");

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			throw std::runtime_error("couldn't load glyph " + c);

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		auto size = face->glyph->bitmap.width * face->glyph->bitmap.rows;
		unsigned char* buffer = new unsigned char[static_cast<uint64_t>(size) * 4];

		for (unsigned int i = 0; i < size; ++i) {
			buffer[i * 4] = 255;
			buffer[i * 4 + 1] = 255;
			buffer[i * 4 + 2] = 255;
			buffer[i * 4 + 3] = face->glyph->bitmap.buffer[i];
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			buffer
		);

		delete[] buffer;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character{
			texture,
			glm::vec2{face->glyph->bitmap.width, face->glyph->bitmap.rows},
			glm::vec2{face->glyph->bitmap_left, face->glyph->bitmap_top},
			face->glyph->advance.x
		};
		characters.insert({c, character});
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
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

void Renderer::drawGlyph(const glm::vec2& position, const glm::vec2& size, const Glyph& glyph) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	float textureIndex = 0.f;
	if (data.textureSlots.contains(glyph.textureID))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(glyph.textureID)
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
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
	data.quadBufferPtr->color = glyph.color;
	data.quadBufferPtr->texCoords = {toSpriteSize(x), toSpriteSize(y)};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y};
	data.quadBufferPtr->color = glyph.color;
	data.quadBufferPtr->texCoords = {toSpriteSize(x + glyph.baseWidth), toSpriteSize(y)};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x + size.x, position.y + size.y};
	data.quadBufferPtr->color = glyph.color;
	data.quadBufferPtr->texCoords = {toSpriteSize(x + glyph.baseWidth), toSpriteSize(y + glyph.height)};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = {position.x, position.y + size.y};
	data.quadBufferPtr->color = glyph.color;
	data.quadBufferPtr->texCoords = {toSpriteSize(x), toSpriteSize(y + glyph.height)};
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.indexCount += 6;
}

void Renderer::drawText(glm::vec2 position, const std::string& value, float scale, const glm::vec4& color) {
	for (char c : value) {
		Character ch = characters[c];
		drawGlyph(position, ch, scale, color);
		position.x += (ch.advance >> 6) * scale;
	}
}

void Renderer::drawTriangle(const std::array<glm::vec2, 3>& points, const glm::vec4& color) {
	if (data.indexCount >= maxIndices) {
		endBatch();
		render();
		beginBatch();
	}

	float textureIndex = 0.f;

	data.quadBufferPtr->position = points[0];
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 0.f, 0.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = points[1];
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.f, 0.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = points[2];
	data.quadBufferPtr->color = color;
	data.quadBufferPtr->texCoords = { 1.f, 1.f };
	data.quadBufferPtr->texIndex = textureIndex;
	data.quadBufferPtr++;

	data.quadBufferPtr->position = points[0];
	data.quadBufferPtr->color = {0.f, 0.f, 0.f, 0.f};
	data.quadBufferPtr->texCoords = { 0.f, 1.f };
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

void Renderer::drawGlyph(glm::vec2 position, const Character& ch, float scale, const glm::vec4& color) {
	if (data.indexCount >= maxIndices || data.textureSlotIndex > static_cast<GLuint>(maxTextures)) {
		endBatch();
		render();
		beginBatch();
	}

	float xPos = position.x + ch.bearing.x * scale;
	float yPos = position.y - (ch.size.y - ch.bearing.y) * scale;

	float width = ch.size.x * scale;
	float height = ch.size.y * scale;

	float textureIndex = 0.f;
	if (data.textureSlots.contains(ch.textureID))
		textureIndex = static_cast<float>(std::distance(
			data.textureSlots.begin(),
			data.textureSlots.find(ch.textureID)
		));

	if (textureIndex == 0.f) {
		textureIndex = static_cast<float>(data.textureSlotIndex);
		// data.textureSlots[data.textureSlotIndex] = textureID;
		data.textureSlots.insert(ch.textureID);
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
