#pragma once

#include <string>
#include "Texture.hpp"

class Mesh {
public:
	Mesh(const std::string& filePath);
	~Mesh();

	void draw();
private:
	unsigned int vao, vbo;
	size_t vertexCount;
	Texture* texture;
};