#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

static constexpr auto worldUp = glm::vec3(0.f, 1.f, 0.f);

class Camera3D {
public:
	explicit Camera3D(const glm::vec3& position) : position(position) {}

	glm::mat4 calculateViewMatrix() const;

	void lookAt(float yaw, float pitch);
	void lookAt(const glm::vec3& target);

	void move(const glm::vec3& diff) { position += diff; }
	void setPosition(const glm::vec3& value) { position = value; }

	const glm::vec3& getFront() { return front; }
	const glm::vec3& getUp() { return up; }
	const glm::vec3& getPosition() { return position; }
private:
	glm::vec3 position;
	glm::vec3 front, up;
	float yaw, pitch;
};