#include "Window.hpp"
#include <stdexcept>
#include <iostream>

struct WindowState {
	GLFWwindow* window;
	glm::vec2 size;
	std::unordered_map<int, bool> previousMouseState;
	float lastMouseX, lastMouseY;
	std::function<void(glm::vec2, glm::vec2)> mousePosCallback;
	std::function<void(float)> scrollCallback;
};

static WindowState windowState;

void Window::init() {
	if (!glfwInit())
		throw std::runtime_error("Couldn't initialize the window");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

#ifndef RELEASE
static void dumpOpenGLMessage(
	unsigned int source, unsigned int type, unsigned int id,
	unsigned int severity, int length, const char* message,
	const void* userParam
) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "[high severity] " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "[medium severity] " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "[low severity] " << message << std::endl;
		break;
	default:
		break;
	}
}
#endif

void Window::create(const std::string& title, size_t width, size_t height) {
	windowState.window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!windowState.window) {
		glfwTerminate();
		throw std::runtime_error("Couldn't create the window!");
	}

	glfwMakeContextCurrent(windowState.window);
	glfwSwapInterval(1);

	gladLoadGL();

	windowState.size = {width, height};
	windowState.lastMouseX = width / 2.f;
	windowState.lastMouseY = height / 2.f;

	glfwSetFramebufferSizeCallback(Window::getWindow(), [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		windowState.size = {width, height};
	});

#ifndef RELEASE
	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
}

void Window::setTitle(const std::string& title) {
	glfwSetWindowTitle(windowState.window, title.c_str());
}

void Window::clearBuffers(BufferType type) {
	glClear(static_cast<GLbitfield>(type));
}

bool Window::isKeyPressed(Key key) {
	return glfwGetKey(windowState.window, static_cast<int>(key)) == GLFW_PRESS;
}

void Window::swapBuffers() {
	glfwSwapBuffers(windowState.window);
}

void Window::pollEvents() {
	glfwPollEvents();
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(windowState.window);
}

void Window::terminate() {
	glfwTerminate();
}

// this will close the window at the next frame
void Window::close() {
	glfwSetWindowShouldClose(windowState.window, true);
}

const glm::vec2& Window::getSize() {
	return windowState.size;
}

float Window::getAspectRatio() {
	return windowState.size.x / windowState.size.y;
}

GLFWwindow* Window::getWindow() {
	return windowState.window;
}

glm::vec2 Window::getMousePosition() {
	double x, y;
	glfwGetCursorPos(windowState.window, &x, &y);
	return {static_cast<float>(x), static_cast<float>(y)};
}

// checks if given button was released
bool Window::getMouseButton(int button) {
	auto state = glfwGetMouseButton(windowState.window, button);
	auto result =
		state == GLFW_RELEASE && windowState.previousMouseState[button] == GLFW_PRESS;
	windowState.previousMouseState[button] = state;
	return result;
}

float Window::getTime() {
	return static_cast<float>(glfwGetTime());
}

void Window::setCursorMode(CursorMode value) {
	glfwSetInputMode(windowState.window, GLFW_CURSOR, static_cast<int>(value));
}

void Window::onCursorMove(std::function<void(glm::vec2, glm::vec2)> callback) {
	windowState.mousePosCallback = std::move(callback);
	glfwSetCursorPosCallback(windowState.window, [](GLFWwindow* window, double x, double y) {
		// invert y coordinate here (glfw uses top as 0, we want bottom to be 0)
		auto xoffset = static_cast<float>(x) - windowState.lastMouseX;
		auto yoffset = windowState.lastMouseY - static_cast<float>(y);
		windowState.lastMouseX = static_cast<float>(x);
		windowState.lastMouseY = static_cast<float>(y);
		windowState.mousePosCallback({x, Window::getSize().y - y}, {xoffset, yoffset});
	});
}

void Window::onScroll(std::function<void(float offset)> callback) {
	windowState.scrollCallback = std::move(callback);
	glfwSetScrollCallback(windowState.window, [](GLFWwindow* window, double, double yoffset) {
		windowState.scrollCallback(yoffset);
	});
}

