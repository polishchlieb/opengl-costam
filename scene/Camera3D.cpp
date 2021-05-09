#include "Camera3D.hpp"

glm::mat4 Camera3D::calculateViewMatrix() const {
	return glm::lookAt(position, position + front, up);
}

void Camera3D::lookAt(float yaw, float pitch) {
	glm::vec3 front;

	float cosPitch = cos(glm::radians(pitch));
	front.x = cos(glm::radians(yaw)) * cosPitch;
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cosPitch;
	this->front = glm::normalize(front);

	auto right = glm::normalize(glm::cross(front, worldUp));
	this->up = glm::normalize(glm::cross(right, front));
}

void Camera3D::lookAt(const glm::vec3& target) {
	auto direction = glm::normalize(position - target);
	auto right = glm::normalize(glm::cross(worldUp, direction));
	this->up = glm::cross(direction, right);
	this->front = glm::normalize(glm::cross(up, right));
}
