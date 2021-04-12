#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader/Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.hpp"
#include <map>

#include "Camera.hpp"
#include <array>

#include "Entity.hpp"

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

int main() {
	/* Initialize the library */
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(960, 540, "nigga world", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync

	if (!gladLoadGL()) {
		std::cout << "kaplica!" << std::endl;
		return EXIT_FAILURE;
	}

	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Camera2D camera{glm::vec2{-480.f, -270.f}};

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0, 0));
	glm::mat4 mvp = proj * camera.calculateViewMatrix() * model;

	Renderer::init();

	std::array<Texture, 2> textures = {
		Texture{"res/textures/axajser.png"},
		Texture{"res/textures/breadoggo.png"}
	};

	Shader shader;
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

	std::array<int, textures.size()> samplers;
	for (uint16_t i = 0; i < textures.size(); ++i)
		samplers[i] = i;
	shader.setUniform1iv("u_Textures", samplers);

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	Entity entity{{430.f, 220.f}, {100.f, 100.f}, textures[1].getId()};

	float deltaTime = 0.f;
	float lastFrame = 0.f;
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		Renderer::clear();

		mvp = proj * camera.calculateViewMatrix() * model;
		shader.bind();
		shader.setUniformMat4f("u_MVP", mvp);

		Renderer::beginBatch();
		Renderer::drawQuad(
			{0.f, 0.f},
			{3338.f, 1668.f},
			textures[0].getId()
		);
		entity.draw();
		Renderer::drawQuad(
			{0.f, 0.f},
			{100.f, 100.f},
			textures[1].getId()
		);
		Renderer::endBatch();
		Renderer::render();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static int fun = 0;
		static float x = 0, y = 0;
		{
			ImGui::Begin("hell world");

			ImGui::BeginGroup();
			ImGui::Text("camera");
			ImGui::SliderFloat("camera x", &camera.position.x, -960.0f, 960.0f);
			ImGui::SliderFloat("camera y", &camera.position.y, -540.0f, 540.0f);
			ImGui::EndGroup();

			ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_W)) {
			camera.move({0.f, 5.f});
			entity.move({0.f, 5.f});
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			camera.move({-5.f, 0.f});
			entity.move({-5.f, 0.f});
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera.move({0.f, -5.f});
			entity.move({0.f, -5.f});
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			camera.move({5.f, 0.f});
			entity.move({5.f, 0.f});
		}

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Renderer::shutdown();

	glfwTerminate();
	return 0;
}