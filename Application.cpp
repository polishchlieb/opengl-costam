#include "Application.hpp"
#include "rendering/Renderer.hpp"
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stb_image.h>

#include <AudioFile.h>

#include "audio/AudioListener.hpp"
#include "event/EventDispatcher.hpp"
#include "event/KeyPressedEvent.hpp"

#include "rendering/Window.hpp"

void Application::init() {
	initOpenGL();
	initRendering();
	initImGui();
	initAudio();
}

void Application::loop() {
	Renderer::clear();

	draw();
	drawGUI();

	Window::swapBuffers();

	processEvents();

	auto currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Application::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Renderer::shutdown();

	glfwTerminate();

	delete[] serverIP;
	delete[] serverPort;

	if (socket)
		socket->disconnect();
	if (socketThread.joinable())
		socketThread.detach();
}

void Application::draw() {
	mvp = proj * camera.calculateViewMatrix() * model;
	shader.bind();
	shader.setUniformMat4f("u_MVP", mvp);

	Renderer::beginBatch();

	// 144 cells of size 60x60 pixels
	struct Cell {
		uint16_t x, y;
		bool exists;
	};
	std::vector<Cell> cells;
	cells.reserve(144);

	for (uint8_t x = 0; x < 16; ++x)
		for (uint8_t y = 0; y < 9; ++y)
			cells.emplace_back(x, y, (y * 15 + x) % 2 == 0);

	for (const auto& cell : cells) {
		if (cell.exists)
			Renderer::drawQuad(
				{60.f * cell.x, 60.f * cell.y},
				{60.f, 60.f},
				{1.f, 0.f, 0.f, 1.f}
			);
	}

	entity->draw();
	if (socket)
		for (const auto& [id, player] : socket->getPlayers()) {
			if (id != socket->getPlayerId())
				Renderer::drawQuad(
					player.position,
					{100.f, 100.f},
					breadoggo->getId()
				);
		}

	Renderer::endBatch();
	Renderer::render();
}

void Application::drawGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static int fun = 0;
	static float x = 0, y = 0;

	ImGui::Begin("camera");

	ImGui::BeginGroup();
	ImGui::Text("camera");
	ImGui::SliderFloat("camera x", &camera.position.x, -960.0f, 960.0f);
	ImGui::SliderFloat("camera y", &camera.position.y, -540.0f, 540.0f);
	ImGui::EndGroup();

	ImGui::End();

	ImGui::Begin("connection");

	ImGui::InputText("server ip", serverIP, 50);
	ImGui::InputText("server port", serverPort, 6);

	static bool amogusPlayed = false;
	if (serverIP[0] == 's' && serverIP[1] == 'u' && serverIP[2] == 's' && !amogusPlayed) {
		amogus.play();
		amogusPlayed = true;
	}

	if (ImGui::Button("join")) {
		socket = std::make_unique<SimpleClient>(std::string{serverIP}, serverPort);
		socketThread = std::thread{[this]() {
			try {
				socket->connect();
			} catch (const std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}};
	}

	if (socket)
		ImGui::Text("Player ID: %d", socket->getPlayerId());

	ImGui::End();

	ImGui::Begin("utils");
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::processEvents() {
	glfwPollEvents();
	
	static float lastBreadoggoX = entity->getPosition().x;
	static float lastBreadoggoY = entity->getPosition().y;

	if (Window::isKeyPressed(GLFW_KEY_W)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_W)
		);
		entity->move({0.f, 5.f});
	}
	if (Window::isKeyPressed(GLFW_KEY_S)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_S)
		);
		entity->move({0.f, -5.f});
	}
	if (Window::isKeyPressed(GLFW_KEY_A)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_A)
		);
		entity->move({-5.f, 0.f});
	}
	if (Window::isKeyPressed(GLFW_KEY_D)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_A)
		);
		entity->move({5.f, 0.f});
	}
	if (Window::isKeyPressed(GLFW_KEY_SPACE)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_SPACE)
		);
		entity->jump();
	}

	float diffX = abs(lastBreadoggoX - entity->getPosition().x);
	float diffY = abs(lastBreadoggoY - entity->getPosition().y);
	if (diffX >= std::numeric_limits<float>::epsilon()
	  || diffY >= std::numeric_limits<float>::epsilon()) {
		if (socket)
			socket->sendPositionUpdate(
				entity->getPosition().x,
				entity->getPosition().y
			);

		lastBreadoggoX = entity->getPosition().x;
		lastBreadoggoY = entity->getPosition().y;
	}
}

void Application::initAudio() {
	AudioListener::setPosition({0.f, 0.f, 0.f});
	AudioListener::setVelocity({0.f, 0.f, 0.f});
	AudioListener::setOrientation({
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f
	});
	
	amogus.load("res/audio/amogus.wav");
}

void Application::initOpenGL() {
	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::initRendering() {
	mvp = proj * camera.calculateViewMatrix() * model;

	Renderer::init();

	background = std::make_unique<Texture>("res/textures/background.png", GL_REPEAT);
	sun = std::make_unique<Texture>("res/textures/sloneczko.png");

	menuBackground = std::make_unique<Texture>("res/textures/chemia.png");

	breadoggo = std::make_unique<Texture>("res/textures/breadoggo.png");
	Breadoggo::init(breadoggo->getId());

	shader.attach(ShaderComponent::fromFile(
		GL_VERTEX_SHADER,
		"vertex.shader"
	));
	shader.attach(ShaderComponent::fromFile(
		GL_FRAGMENT_SHADER,
		"fragment.shader"
	));

	shader.link();
	shader.validate();
	shader.bind();

	shader.createSamplers();

	shader.setUniformMat4f("u_MVP", mvp);

	entity = std::make_unique<Breadoggo>(glm::vec2{0.f, 0.f});
}

void Application::initImGui() {
	serverIP = new char[50];
	*serverIP = '\0';

	serverPort = new char[7];
	*serverPort = '\0';

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Window::getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}
