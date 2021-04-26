#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Entity {
public:
	Entity(glm::vec2 position, glm::vec2 size, glm::vec4 color);
	Entity(glm::vec2 position, glm::vec2 size, GLuint textureID);

	void move(glm::vec2 difference);
	virtual void draw();

	const glm::vec2& getPosition() { return position; }
protected:
	glm::vec2 position;
private:
	glm::vec2 size;
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLuint textureID = 0;
	bool textured;
};