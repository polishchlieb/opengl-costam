#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

static constexpr auto worldUp = glm::vec3(0.f, 1.f, 0.f);

class Camera3D {
public:
	explicit Camera3D(const glm::vec3& position) : position(position) {}

	glm::mat4 calculateViewMatrix() const {
		return glm::lookAt(position, position + front, up);
	}

	void lookAt(float yaw, float pitch) {
		glm::vec3 front;

		float cosPitch = cos(glm::radians(pitch));
		front.x = cos(glm::radians(yaw)) * cosPitch;
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cosPitch;
		this->front = glm::normalize(front);

		auto right = glm::normalize(glm::cross(front, worldUp));
		this->up = glm::normalize(glm::cross(right, front));
	}

	void lookAt(const glm::vec3& target) {
		auto direction = glm::normalize(position - target);
		auto right = glm::normalize(glm::cross(worldUp, direction));
		this->up = glm::cross(direction, right);
		this->front = glm::normalize(glm::cross(up, right));
	}

	void move(const glm::vec3& diff) {
		position += diff;
	}

	const glm::vec3& getFront() { return front; }
	const glm::vec3& getUp() { return up; }
	const glm::vec3& getPosition() { return position; }
private:
	glm::vec3 position;
	glm::vec3 front, up;
	float yaw, pitch;
};