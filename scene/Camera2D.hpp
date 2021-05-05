#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera2D {
public:
	explicit Camera2D(glm::vec2 position);
	glm::mat4 calculateViewMatrix();
	void move(glm::vec2 difference);

	glm::vec2 position;
};