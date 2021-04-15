#include "Camera.hpp"

Camera2D::Camera2D(glm::vec2 position)
	: position(std::move(position)) {}

glm::mat4 Camera2D::calculateViewMatrix() {
	return glm::translate(
		glm::mat4(1.0f),
		glm::vec3(-960.f / 2.f - position.x, -540.f / 2.f - position.y, 0)
	);
}

void Camera2D::move(glm::vec2 difference) {
	position += difference;
}
