#include "Light3D.hpp"

Light3D::Light3D(const glm::vec3& position, const glm::vec3& color, std::reference_wrapper<Mesh> mesh)
	: position(position), color(color), mesh(mesh)
{

}

void Light3D::draw() const {

}
