#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../rendering/Texture.hpp"
#include <optional>

class Entity {
public:
	Entity(glm::vec2 position, glm::vec2 size, glm::vec4 color);
	Entity(glm::vec2 position, glm::vec2 size, Texture& texture);

	void move(glm::vec2 difference);
	virtual void draw();

	const glm::vec2& getPosition() const { return position; }
	const glm::vec2& getSize() const { return size; }
protected:
	glm::vec2 position;
private:
	glm::vec2 size;
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::optional<std::reference_wrapper<Texture>> texture;
	bool textured;
};