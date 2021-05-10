#pragma once
#include <glm/glm.hpp>

struct Vertex2D {
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 texCoords;
	float texIndex;
};
