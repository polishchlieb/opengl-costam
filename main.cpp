#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include <iostream>

int main() {
	/* Initialize the library */
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(960, 540, "nigga world", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync

	if (!gladLoadGL()) {
		std::cout << "kaplica!" << std::endl;
		return EXIT_FAILURE;
	}

	Application app{window};
	app.init();

	while (!glfwWindowShouldClose(window)) {
		app.loop();
	}

	app.shutdown();
	return 0;
}