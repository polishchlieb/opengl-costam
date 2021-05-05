#pragma once
#include <glm/glm.hpp>

class Camera3D {
public:
	glm::vec3 position, front, up, right, worldUp;
	float yaw, pitch;
	float movementSpeed, mouseSensitivity, zoom;
};