#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

std::vector<Vertex> loadObj(const std::string& filePath);