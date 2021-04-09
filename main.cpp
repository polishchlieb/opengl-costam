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

#include "Quad.hpp"
#include "Renderer.hpp"
#include "SpriteSheet.hpp"
#include <map>

#include "Camera.hpp"

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
		/* case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cout << "[notification] " << message << std::endl;
			break; */
	default:
		break;
	}
}

int main() {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "nigga world", nullptr, nullptr);
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

	std::cout << "wersyja: " << GLVersion.major << "." << GLVersion.minor << std::endl;

	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::array<Texture, 2> textures = {
		Texture{"res/textures/axajser.png"},
		Texture{"res/textures/breadoggo.png"}
	};

	VertexArray va;
	Buffer vb{ GL_ARRAY_BUFFER, nullptr, sizeof(Vertex) * 1000, GL_DYNAMIC_DRAW };
	VertexBufferLayout layout;
	layout.push(GL_FLOAT, 2); // position
	layout.push(GL_FLOAT, 4); // rgba color
	layout.push(GL_FLOAT, 2); // texcoords
	layout.push(GL_FLOAT, 1); // texture id
	layout.push(GL_FLOAT, 1); // invert?
	va.addBuffer(vb, layout);

	std::array<unsigned int, 6000> indices;
	for (size_t i = 0; i < 990; ++i) {
		indices[i * 6] = i * 4;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4;
	}
	Buffer ib{ GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size() * sizeof(unsigned int), GL_STATIC_DRAW };

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	Camera2D camera{glm::vec2{-480.f, -270.f}, 2.f};
	// glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0, 0));

	glm::mat4 mvp = proj * camera.calculateViewMatrix() * model;

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

	Renderer renderer;

	float deltaTime = 0.f;
	float lastFrame = 0.f;

	glm::vec2 breadoggoPos{416.f, 206.f};

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		mvp = proj * camera.calculateViewMatrix() * model;
		shader.bind();
		shader.setUniformMat4f("u_MVP", mvp);

		{
			Quad background{
				{0.f, 0.f},
				{3338.f, 0.f},
				{3338.f, 1668.f},
				{0.f, 1668.f},
				{0.f, 0.f, 0.f, 0.f},
				textures[0],
			};
			Quad character{
				{breadoggoPos.x, breadoggoPos.y},
				{breadoggoPos.x + 128.f, breadoggoPos.y},
				{breadoggoPos.x + 128.f, breadoggoPos.y + 128.f},
				{breadoggoPos.x, breadoggoPos.y + 128.f},
				{0.f, 0.f, 0.f, 0.f},
				textures[1]
			};
			renderer.render({ background, character }, vb);
		}

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
			camera.move(UP, 5.f);
			breadoggoPos.y += 5.f;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			camera.move(LEFT, 5.f);
			breadoggoPos.x -= 5.f;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera.move(DOWN, 5.f);
			breadoggoPos.y -= 5.f;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			camera.move(RIGHT, 5.f);
			breadoggoPos.x += 5.f;
		}

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}