#pragma once
#include <glad/glad.h>

class GL {
public:
	static void enableDepthTesting() {
		glEnable(GL_DEPTH_TEST);
	}

	static void disableDepthTesting() {
		glDisable(GL_DEPTH_TEST);
	}

	static void enableBlending() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
};