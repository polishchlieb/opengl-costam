#pragma once
#include <memory>
#include "rendering/Texture.hpp"
#include <GLFW/glfw3.h>
#include "scene/Camera.hpp"
#include <glm/glm.hpp>
#include "shader/Shader.hpp"
#include "Breadoggo.hpp"
#include "rendering/Font.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <thread>

class Application {
public:
	explicit Application(GLFWwindow* window);

	void init();
	void loop();
	void shutdown();
private:
	GLFWwindow* window;

	std::unique_ptr<Texture> background;
	std::unique_ptr<Texture> sun;
	std::unique_ptr<Texture> breadoggo;

	std::unique_ptr<Font> font;
	std::unique_ptr<Texture> comicSans;

	Camera2D camera{glm::vec2{-480.f, -270.f}};
	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0, 0));
	glm::mat4 mvp = glm::mat4{};

	Shader shader;

	std::unique_ptr<Breadoggo> entity;

	float deltaTime = 0.f;
	float lastFrame = 0.f;

	float scrollX = 0.f;

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
		// case GL_DEBUG_SEVERITY_NOTIFICATION:
		//	std::cout << "[notification] " << message << std::endl;
		//	break;
		default:
			break;
		}
	}

	void draw();
	void drawGUI();
	void processEvents();

	void initOpenAL();
	void initOpenGL();
	void initRendering();
	void initImGui();

	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
	ALuint monoSoundBuffer = 0;
	ALuint source = 0;
	bool amogusPlayed = false;
	std::thread amogus;

	char* serverIP = nullptr;
};