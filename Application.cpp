#include "Application.hpp"
#include "rendering/Renderer2D.hpp"
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


#include "rendering/ColorTexture.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/GL.hpp"

Application::~Application() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Renderer2D::shutdown();

	glfwTerminate();
}

void Application::init() {
	Window::init();
	Window::create("hell world", 960, 540);

	Window::setCursorMode(Window::CursorMode::Disabled);

	GL::enableDepthTesting();
	GL::enableBlending();

	Shader shader;
	shader.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::VertexShader,
		"vertex.shader"
	));
	shader.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::FragmentShader,
		"fragment.shader"
	));
	shader.link();
	shader.validate();

	Shader lightShader;
	lightShader.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::VertexShader,
		"light.vertex.shader"
	));
	lightShader.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::FragmentShader,
		"light.fragment.shader"
	));
	lightShader.link();
	lightShader.validate();

	Shader shader2D;
	shader2D.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::VertexShader,
		"2d.vertex.shader"
	));
	shader2D.attach(ShaderComponent::fromFile(
		ShaderComponent::ShaderType::FragmentShader,
		"2d.fragment.shader"
	));
	shader2D.link();
	shader2D.validate();

	Renderer2D::init();

	ColorTexture whiteTexture{ {1.f, 1.f, 1.f, 1.f} };
	Texture chungusTexture{ "res/meshes/chungus-1/chungus_TM_u0_v0.png" };

	Mesh chungus{ "res/meshes/chungus-1/chungus.obj" };
	Mesh lightCube{ "res/meshes/cube/cube.obj" };
	Mesh peter{ "res/meshes/piotr-g-1/peter.obj" };

	AudioListener::setPosition({ 0.f, 0.f, 0.f });
	AudioListener::setVelocity({ 0.f, 0.f, 0.f });
	AudioListener::setOrientation({
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f
	});

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Window::getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void Application::loop() {
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/);

	draw();
	drawGUI();

	Window::swapBuffers();

	processEvents();

	auto currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Application::draw() {
}

void Application::drawGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("camera");

	ImGui::BeginGroup();
	ImGui::Text("camera");

	float cameraX, cameraY;
	ImGui::SliderFloat("camera x", &cameraX, -960.0f, 960.0f);
	ImGui::SliderFloat("camera y", &cameraY, -540.0f, 540.0f);
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
}
