#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>

class Window {
public:
	static void init();
	static void create(const std::string& title, size_t width, size_t height);
	static void setTitle(const std::string& title);

	static enum class BufferType {
		ColorBuffer = GL_COLOR_BUFFER_BIT,
		DepthBuffer = GL_DEPTH_BUFFER_BIT
	};

	static void clearBuffers(BufferType type);

	static enum class Key {
		W = GLFW_KEY_W,
		A = GLFW_KEY_A,
		S = GLFW_KEY_S,
		D = GLFW_KEY_D,
		Space = GLFW_KEY_SPACE,
		Esc = GLFW_KEY_ESCAPE
	};
	static bool isKeyPressed(Key key);

	static void swapBuffers();
	static void pollEvents();
	static bool shouldClose();
	static void terminate();
	static void close();

	static const glm::vec2& getSize();
	static float getAspectRatio();
	static GLFWwindow* getWindow();

	static glm::vec2 getMousePosition();
	static bool getMouseButton(int button);
	static float getTime();

	static enum class CursorMode {
		Disabled = GLFW_CURSOR_DISABLED,
		Normal = GLFW_CURSOR_NORMAL,
		Hidden = GLFW_CURSOR_HIDDEN
	};
	static void setCursorMode(CursorMode value);

	static void onCursorMove(std::function<void(glm::vec2 currentPosition, glm::vec2 offset)> callback);
	static void onScroll(std::function<void(float offset)> callback);
};

static inline Window::BufferType operator|(Window::BufferType a, Window::BufferType b) {
	return static_cast<Window::BufferType>(
		static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b)
	);
}