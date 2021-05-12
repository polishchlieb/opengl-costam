#include "Entity.hpp"
#include "../rendering/Renderer2D.hpp"

Entity::Entity(glm::vec2 position, glm::vec2 size, glm::vec4 color)
	: position(position), size(size), color(color), textured(false) {}

Entity::Entity(glm::vec2 position, glm::vec2 size, Texture& texture)
	: position(position), size(size), texture(texture), textured(true) {}

void Entity::move(glm::vec2 difference) {
	position += difference;
}

void Entity::draw() {
	if (textured)
		Renderer2D::drawQuad(position, size, texture->get());
	else
		Renderer2D::drawQuad(position, size, color);
}