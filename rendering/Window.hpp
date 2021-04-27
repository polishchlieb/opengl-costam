#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Window {
public:
	static void init();
	static void create(const std::string& title, size_t width, size_t height);
	static void setTitle(const std::string& title);

	static bool isKeyPressed(int code);
	static void swapBuffers();
	static bool shouldClose();

	static const glm::vec2& getSize() { return size; }
	static GLFWwindow* getWindow() { return window; }

	static glm::vec2 getMousePosition();
	static bool getMouseButton(int button);
private:
	static GLFWwindow* window;
	static glm::vec2 size;
	static std::unordered_map<int, bool> previousMouseState;
};
