#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Direction {
	UP,
	DOWN,
	RIGHT,
	LEFT
};

class Camera2D {
public:
	glm::vec2 position;
	float zoom;

	Camera2D(glm::vec2 position, float zoom)
		: position(std::move(position)), zoom(zoom) {}

	glm::mat4 calculateViewMatrix() {
		return glm::translate(
			glm::mat4(1.0f),
			glm::vec3(-960.f / 2.f -position.x, -540.f / 2.f -position.y, 0)
		);
	}

	void move(Direction direction, float distance) {
		switch (direction) {
			case UP: position.y += distance; break;
			case DOWN: position.y -= distance; break;
			case RIGHT: position.x += distance; break;
			case LEFT: position.x -= distance; break;
		}
	}
};