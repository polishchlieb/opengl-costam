#include "Window.hpp"
#include <stdexcept>

void Window::init() {
	if (!glfwInit())
		throw std::runtime_error("Couldn't initialize the window");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* Window::window;
glm::vec2 Window::size;

void Window::create(const std::string& title, size_t width, size_t height) {
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Couldn't create the window!");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGL();

	size = {width, height};
}

void Window::setTitle(const std::string& title) {
	glfwSetWindowTitle(window, title.c_str());
}

bool Window::isKeyPressed(int code) {
	return glfwGetKey(window, code) == GLFW_PRESS;
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}
