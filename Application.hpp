#pragma once
#include <memory>
#include "rendering/Texture.hpp"
#include <GLFW/glfw3.h>
#include "scene/Camera2D.hpp"
#include <glm/glm.hpp>
#include "shader/Shader.hpp"
#include "rendering/Font.hpp"
#include "scene/Entity.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include "audio/AudioContext.hpp"
#include "audio/AudioSource.hpp"

#include <thread>

class Application {
public:
	void init();
	void loop();
	void shutdown();
private:
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 960.f / 540.0f, 0.1f, 100.0f);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mvp{};

	std::unique_ptr<Texture> chungusTexture;
	std::unique_ptr<Entity> chungus;

	Shader shader;
	Shader lightShader;

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
		default:
			break;
		}
	}

	void draw();
	void drawGUI();
	void processEvents();

	void initAudio();
	void initOpenGL();
	void initRendering();
	void initImGui();

	AudioContext context;

	std::array<float, 180> vertices;
};