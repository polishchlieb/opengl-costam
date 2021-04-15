#include "Application.hpp"
#include "rendering/Renderer.hpp"
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stb_image.h>

Application::Application(GLFWwindow* window)
	: window(window) {}

void Application::init() {
	serverIP = new char[50];

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
	Breadoggo::init(breadoggo->getId());

	comicSans = std::make_unique<Texture>("res/textures/komiczny-sans.png");
	std::vector<uint16_t> glyphWidths = {
		92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 55, 44, 78, 154, 127, 150, 120, 71, 67, 67, 97, 88, 51, 76, 46, 94, 112, 82, 112, 112, 112, 112, 112, 112, 112, 112, 55, 55, 70, 93, 70, 96, 170, 134, 115, 110, 132, 114, 111, 124, 141, 100, 122, 112, 101, 162, 146, 146, 95, 160, 115, 127, 124, 135, 119, 190, 132, 116, 127, 69, 101, 69, 106, 115, 102, 94, 109, 94, 107, 100, 93, 97, 106, 51, 74, 99, 50, 142, 96, 96, 98, 95, 88, 89, 86, 95, 89, 125, 108, 95, 98, 67, 77, 67, 109, 92, 112, 92, 55, 92, 75, 124, 109, 109, 92, 227, 127, 70, 127, 124, 127, 127, 92, 33, 33, 72, 72, 71, 81, 162, 92, 162, 89, 70, 89, 117, 98, 98, 55, 102, 102, 101, 112, 134, 74, 116, 102, 146, 127, 106, 88, 76, 146, 127, 75, 88, 102, 42, 102, 95, 127, 46, 102, 94, 89, 106, 101, 102, 85, 98, 115, 134, 134, 134, 134, 101, 110, 110, 110, 114, 114, 114, 114, 100, 100, 132, 132, 146, 146, 146, 146, 146, 146, 88, 115, 135, 135, 135, 135, 116, 124, 81, 88, 94, 94, 94, 94, 50, 94, 94, 110, 100, 100, 100, 100, 51, 51, 152, 110, 96, 96, 96, 96, 96, 96, 88, 88, 95, 95, 95, 95, 95, 86, 102
	};
	font = std::make_unique<Font>(comicSans->getId(), glyphWidths, 4096, 4096, 256, 256);

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

	entity = std::make_unique<Breadoggo>(glm::vec2{430.f, 120.f});
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

	delete[] serverIP;
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

	const std::string str = "NIGGERS";
	for (uint32_t i = 0, j = 0; i < str.size(); ++i) {
		const auto width = static_cast<float>(font->getGlyphWidth(str[i])) / 2.f;
		const auto height = 256.f / 2.f;

		Renderer::drawGlyph(
			{200.f + j, 430.f},
			{width, height},
			font->getGlyph(str[i])
		);
		j += width;
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

	ImGui::Begin("stuff");

	ImGui::BeginGroup();
	ImGui::Text("camera");
	ImGui::SliderFloat("camera x", &camera.position.x, -960.0f, 960.0f);
	ImGui::SliderFloat("camera y", &camera.position.y, -540.0f, 540.0f);
	ImGui::EndGroup();

	ImGui::InputText("server ip", serverIP, 50);

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
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		entity->jump();
	}
}
