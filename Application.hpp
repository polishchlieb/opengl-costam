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
	~Application();

	void init();
	void loop();
private:
	glm::mat4 mvp{};

	std::unique_ptr<Texture> chungusTexture;
	std::unique_ptr<Entity> chungus;

	Shader shader;
	Shader lightShader;

	float deltaTime = 0.f;
	float lastFrame = 0.f;

	float scrollX = 0.f;

	void draw();
	void drawGUI();
	void processEvents();

	AudioContext context;

	std::array<float, 180> vertices;
};