#pragma once

#include <glm/glm.hpp>
#include "../rendering/Mesh.hpp"

class Light3D {
public:
	Light3D(const glm::vec3& position, const glm::vec3& color, std::reference_wrapper<Mesh> mesh);

	void move(const glm::vec3& diff) { position += diff; }
	void setPosition(const glm::vec3& value) { position = value; }

	void draw() const;

	const glm::vec3& getPosition() const { return position; }
	const glm::vec3& getColor() const { return color; }
private:
	glm::vec3 position, color;
	std::reference_wrapper<Mesh> mesh;
};