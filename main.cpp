#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader/Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Quad.hpp"
#include "Renderer.hpp"
#include "SpriteSheet.hpp"

#include "game/Chessboard.hpp"

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
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "nigga world", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync

	if (!gladLoadGL()) {
		std::cout << "kaplica!" << std::endl;
		return 1;
	}

	std::cout << "wersyja: " << GLVersion.major << "." << GLVersion.minor << std::endl;

	glDebugMessageCallback(dumpOpenGLMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	constexpr size_t quadCount = 64 + 16 * 2;

	std::array<unsigned int, 6 * quadCount> indices;
	for (size_t i = 0; i < quadCount; ++i) {
		indices[i * 6] = i * 4;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4;
	}

	std::array<Texture, 1> textures = {
		Texture{"res/textures/spritesheet.png"}
	};
	const Texture& spriteSheetTexture = textures[0];

	SpriteSheet spriteSheet{
		{64, 64},
		spriteSheetTexture
	};

	VertexArray va;

	Buffer vb{ GL_ARRAY_BUFFER, nullptr, sizeof(Vertex) * 1000, GL_DYNAMIC_DRAW };
	VertexBufferLayout layout;
	layout.push(GL_FLOAT, 2); // position
	layout.push(GL_FLOAT, 4); // rgba color
	layout.push(GL_FLOAT, 2); // texcoords
	layout.push(GL_FLOAT, 1); // texture id
	va.addBuffer(vb, layout);

	Buffer ib{ GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size() * sizeof(unsigned int), GL_STATIC_DRAW };

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

	glm::mat4 mvp = proj * view * model;

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
	shader.setUniform1iv("u_Textures", samplers.data(), samplers.size());

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	Renderer renderer;

	Chessboard chessboard;
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		mvp = proj * view * model;
		shader.bind();
		shader.setUniformMat4f("u_MVP", mvp);

		// Rendering the chessboard
		{ // batch renderer
			std::vector<SpriteSheetQuad> quads;
			quads.reserve(quadCount);

			for (uint8_t i = 0; i < 8; ++i) {
				for (uint8_t j = 0; j < 8; ++j) {
					bool isEven = j % 2 == 0;
					bool isKacperek = isEven
						? i % 2 == 0
						: i % 2 == 1;

					quads.push_back({
						static_cast<unsigned int>(isKacperek ? 3 : 5),
						{i * 64.0f, j * 64.0f},
						{i * 64.0f + 64.0f, j * 64.0f},
						{i * 64.0f + 64.0f, j * 64.0f + 64.0f},
						{i * 64.0f, j * 64.0f + 64.0f}
					});
				}
			}

			for (const auto& figure : chessboard.figures) {
				unsigned int index;
				switch (figure.type) {
					case FigureType::rook: index = 2; break;
					case FigureType::knight: index = 10; break;
					case FigureType::bishop: index = 6; break;
					case FigureType::queen: index = 1; break;
					case FigureType::king: index = 9; break;
					case FigureType::pawn: index = 7; break;
				}

				const auto [x, y] = figure.position;

				quads.push_back({
					index,
					{x * 64.0f + 11.0f, y * 64.0f + 11.0f},
					{x * 64.0f + 11.0f + 42.0f, y * 64.0f + 11.0f},
					{x * 64.0f + 11.0f + 42.0f, y * 64.0f + 11.0f + 42.0f},
					{x * 64.0f + 11.0f, y * 64.0f + 11.0f + 42.0f}
				});
			}

			spriteSheet.draw(quads, vb);
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
			ImGui::SliderFloat("camera x", &view[3][0], 0.0f, 960.0f);
			ImGui::SliderFloat("camera y", &view[3][1], 0.0f, 540.0f);
			ImGui::EndGroup();

			ImGui::Text("fun");
			ImGui::SliderInt("fun", &fun, 0, 255);
			ImGui::SliderFloat("x", &x, 0, 960);
			ImGui::SliderFloat("y", &y, 0, 540);

			ImGui::Text("pionek");
			ImGui::SliderInt("pionek x",
				&chessboard.figures[0].position[0], 0, 7);
			ImGui::SliderInt("pionek y",
				&chessboard.figures[0].position[1], 0, 7);

			ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_UP))
			++fun;
		if (glfwGetKey(window, GLFW_KEY_DOWN))
			--fun;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);
			x = static_cast<float>(xPos);
			y = static_cast<float>(yPos);

			for (auto& figure : chessboard.figures) {
				if (figure.contains(x, 540.0f - y)) {
					++figure.position[1];
				}
			}
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}