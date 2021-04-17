#pragma once
#include "scene/Entity.hpp"

class Breadoggo : public Entity {
public:
	Breadoggo(glm::vec2 position);
	static void init(GLuint textureID);

	void jump();
	void draw() override;
private:
	bool isJumping = false;

	enum JumpDirection {
		UP, DOWN
	};
	JumpDirection jumpDirection = UP;
};