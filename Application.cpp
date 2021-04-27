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
}

void Application::draw() {
	mvp = proj * camera.calculateViewMatrix() * model;
	shader.bind();
	shader.setUniformMat4f("u_MVP", mvp);

	Renderer::beginBatch();

	// 144 (16 * 9) cells of size 60x60 pixels
	const auto mouse = Window::getMousePosition();
	if (Window::getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		int index = ((540 - (int)mouse.y) / 60) * 16 + ((int)mouse.x / 60);
		cells[index].exists = !cells[index].exists;
	}

	for (size_t x = 0; x < 16; ++x)
		for (size_t y = 0; y < 9; ++y) {
			size_t index = y * 16 + x;
			if (cells[index].exists)
				Renderer::drawQuad(
					{60.f * x, 60.f * y},
					{60.f, 60.f},
					{1.f, 0.f, 0.f, 1.f}
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

	ImGui::Begin("utils");
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::processEvents() {
	glfwPollEvents();
	
	if (Window::isKeyPressed(GLFW_KEY_W)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_W)
		);
	}
	if (Window::isKeyPressed(GLFW_KEY_S)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_S)
		);
	}
	if (Window::isKeyPressed(GLFW_KEY_A)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_A)
		);
	}
	if (Window::isKeyPressed(GLFW_KEY_D)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_A)
		);
	}
	if (Window::isKeyPressed(GLFW_KEY_SPACE)) {
		EventDispatcher::dispatch(
			std::make_shared<KeyPressedEvent>(GLFW_KEY_SPACE)
		);
	}
}

void Application::initAudio() {
	AudioListener::setPosition({0.f, 0.f, 0.f});
	AudioListener::setVelocity({0.f, 0.f, 0.f});
	AudioListener::setOrientation({
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f
	});
}

void Application::initOpenGL() {
	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void Application::initRendering() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Renderer::init();

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

	cells.reserve(144);
	for (uint8_t y = 0; y < 9; ++y)
		for (uint8_t x = 0; x < 16; ++x)
			cells.emplace_back();
}

void Application::initImGui() {
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Window::getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}
