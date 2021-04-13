#include "Breadoggo.hpp"
#include "event/KeyPressedEvent.hpp"
#include <GLFW/glfw3.h>

static GLuint _textureID;

void Breadoggo::init(GLuint texID) {
	_textureID = texID;
}

void Breadoggo::jump() {
	if (!isJumping) {
		isJumping = true;
		jumpDirection = UP;
	}
}

void Breadoggo::draw() {
	if (isJumping) {
		if (position.y >= 170.f && jumpDirection == UP)
			jumpDirection = DOWN;
		if (position.y <= 120.f && jumpDirection == DOWN)
			isJumping = false;
	}

	if (isJumping)
		move((jumpDirection == UP)
			? glm::vec2{0.f, 5.f}
			: glm::vec2{0.f, -5.f});

	Entity::draw();
}

Breadoggo::Breadoggo(glm::vec2 position)
	: Entity{position, {100.f, 100.f}, _textureID} {}