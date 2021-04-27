#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Application.hpp"
#include "rendering/Window.hpp"

int main() {
	Window::init();
	Window::create("hell world", 960, 540);

	Application app;
	app.init();

	while (!Window::shouldClose()) {
		app.loop();
	}

	app.shutdown();
	return 0;
}