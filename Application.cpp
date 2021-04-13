#include "Application.hpp"
#include "Renderer.hpp"
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Application::Application(GLFWwindow* window)
	: window(window) {}

void Application::init() {
	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mvp = proj * camera.calculateViewMatrix() * model;

	Renderer::init();

	background = std::make_unique<Texture>("res/textures/background.png", GL_REPEAT);
	sun = std::make_unique<Texture>("res/textures/sloneczko.png");
	breadoggo = std::make_unique<Texture>("res/textures/breadoggo.png");

	shader.attach(ShaderComponent::fromFile(
		GL_VERTEX_SHADER,
		"shader/vertex.shader"
	));
	shader.attach(ShaderComponent::fromFile(
		GL_FRAGMENT_SHADER,
		"shader/fragment.shader"
	));

	shader.link();
	shader.validate();
	shader.bind();

	shader.setUniformMat4f("u_MVP", mvp);

	std::array<int, 32> samplers;
	for (uint16_t i = 0; i < 32; ++i)
		samplers[i] = i;
	shader.setUniform1iv("u_Textures", samplers);

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	entity = std::make_unique<Entity>(
		glm::vec2{430.f, 120.f}, glm::vec2{100.f, 100.f}, breadoggo->getId()
	);
}

void Application::loop() {
	Renderer::clear();

	mvp = proj * camera.calculateViewMatrix() * model;
	shader.bind();
	shader.setUniformMat4f("u_MVP", mvp);

	draw();
	drawGUI();

	glfwSwapBuffers(window);

	processEvents();

	float currentFrame = glfwGetTime();
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
	Renderer::beginBatch();

	Renderer::drawScrollingQuad(
		{ 0.f, 0.f },
		{ 960.f, 540.f },
		background->getId(),
		scrollX
	);

	Renderer::drawQuad(
		{ 0.f, 440.f },
		{ 100.f, 100.f },
		sun->getId()
	);

	entity->draw();

	Renderer::endBatch();
	Renderer::render();
}

void Application::drawGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static int fun = 0;
	static float x = 0, y = 0;

	ImGui::Begin("stuff");

	ImGui::BeginGroup();
	ImGui::Text("camera");
	ImGui::SliderFloat("camera x", &camera.position.x, -960.0f, 960.0f);
	ImGui::SliderFloat("camera y", &camera.position.y, -540.0f, 540.0f);
	ImGui::EndGroup();

	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::processEvents() {
	glfwPollEvents();
	if (glfwGetKey(window, GLFW_KEY_W)) {
		entity->move({ 0.005f, 0.f });
		scrollX += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		entity->move({ -0.005f, 0.f });
		scrollX -= 0.005f;
	}
}
