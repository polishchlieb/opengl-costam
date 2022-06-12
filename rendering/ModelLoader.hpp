#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadObj(const std::string& filePath);