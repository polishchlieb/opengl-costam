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

static struct Edge {
	float startX, startY;
	float endX, endY;
};
static struct Cell {
	int edgeId[4];
	bool edgeExists[4];
	bool exists = false;
};

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

static std::vector<Cell> cells;
static std::vector<Edge> edges;

void getPolyMap(size_t startX, size_t startY, size_t width, size_t height, float blockWidth, size_t pitch) {
	edges.clear();

	for (size_t x = 0; x < width; ++x)
		for (size_t y = 0; y < height; ++y)
			for (uint8_t j = 0; j < 4; ++j) {
				cells[(y + startY) * pitch + (x + startX)].edgeExists[j] = false;
				cells[(y + startY) * pitch + (x + startX)].edgeId[j] = 0;
			}

	for (size_t x = 1; x < width - 1; ++x) {
		for (size_t y = 1; y < height - 1; ++y) {
			auto i = (y + startY) * pitch + (x + startX);
			auto n = (y + startY - 1) * pitch + (x + startX);
			auto s = (y + startY + 1) * pitch + (x + startX);
			auto w = (y + startY) * pitch + (x + startX - 1);
			auto e = (y + startY) * pitch + (x + startX + 1);

			if (cells[i].exists) {
				if (!cells[w].exists) {
					if (cells[n].edgeExists[WEST]) {
						edges[cells[n].edgeId[WEST]].endY += blockWidth;
						cells[i].edgeId[WEST] = cells[n].edgeId[WEST];
						cells[i].edgeExists[WEST] = true;
					}
					else {
						Edge edge;
						edge.startX = (startX + x) * blockWidth;
						edge.startY = (startY + y) * blockWidth;
						edge.endX = edge.startX;
						edge.endY = edge.startY + blockWidth;

						auto edgeId = edges.size();
						edges.push_back(edge);

						cells[i].edgeId[WEST] = edgeId;
						cells[i].edgeExists[WEST] = true;
					}
				}

				if (!cells[e].exists) {
					if (cells[n].edgeExists[EAST]) {
						edges[cells[n].edgeId[EAST]].endY += blockWidth;
						cells[i].edgeId[EAST] = cells[n].edgeId[EAST];
						cells[i].edgeExists[EAST] = true;
					}
					else {
						Edge edge;
						edge.startX = (startX + x + 1) * blockWidth;
						edge.startY = (startY + y) * blockWidth;
						edge.endX = edge.startX;
						edge.endY = edge.startY + blockWidth;

						auto edgeId = edges.size();
						edges.push_back(edge);

						cells[i].edgeId[EAST] = edgeId;
						cells[i].edgeExists[EAST] = true;
					}
				}

				if (!cells[n].exists) {
					if (cells[w].edgeExists[NORTH]) {
						edges[cells[w].edgeId[NORTH]].endX += blockWidth;
						cells[i].edgeId[NORTH] = cells[w].edgeId[NORTH];
						cells[i].edgeExists[NORTH] = true;
					}
					else {
						Edge edge;
						edge.startX = (startX + x) * blockWidth;
						edge.startY = (startY + y) * blockWidth;
						edge.endX = edge.startX + blockWidth;
						edge.endY = edge.startY;

						auto edgeId = edges.size();
						edges.push_back(edge);

						cells[i].edgeId[NORTH] = edgeId;
						cells[i].edgeExists[NORTH] = true;
					}
				}

				if (!cells[s].exists) {
					if (cells[w].edgeExists[SOUTH]) {
						edges[cells[w].edgeId[SOUTH]].endX += blockWidth;
						cells[i].edgeId[SOUTH] = cells[w].edgeId[SOUTH];
						cells[i].edgeExists[SOUTH] = true;
					}
					else {
						Edge edge;
						edge.startX = (startX + x) * blockWidth;
						edge.startY = (startY + y + 1) * blockWidth;
						edge.endX = edge.startX + blockWidth;
						edge.endY = edge.startY;

						auto edgeId = edges.size();
						edges.push_back(edge);

						cells[i].edgeId[SOUTH] = edgeId;
						cells[i].edgeExists[SOUTH] = true;
					}
				}
			}
		}
	}
}

std::vector<std::tuple<float, float, float>> visibilityPolygonPoints;
void calculateVisibilityPolygon(float ox, float oy, float radius) {
	visibilityPolygonPoints.clear();

	for (auto& e1 : edges) {
		for (int i = 0; i < 2; i++) {
			float rdx, rdy;
			rdx = (i == 0 ? e1.startX : e1.endX) - ox;
			rdy = (i == 0 ? e1.startY : e1.endY) - oy;

			float base_ang = atan2f(rdy, rdx);

			float ang = 0;
			for (int j = 0; j < 3; j++) {
				if (j == 0)	ang = base_ang - 0.0001f;
				if (j == 1)	ang = base_ang;
				if (j == 2)	ang = base_ang + 0.0001f;

				rdx = radius * cosf(ang);
				rdy = radius * sinf(ang);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool bValid = false;

				for (auto& e2 : edges) {
					float sdx = e2.endX - e2.startX;
					float sdy = e2.endY - e2.startY;

					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f) {
						float t2 = (rdx * (e2.startY - oy) + (rdy * (ox - e2.startX))) / (sdx * rdy - sdy * rdx);
						float t1 = (e2.startX + sdx * t2 - ox) / rdx;

						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f) {
							if (t1 < min_t1) {
								min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								bValid = true;
							}
						}
					}
				}

				if (bValid)
					visibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
			}
		}
	}

	std::sort(
		visibilityPolygonPoints.begin(),
		visibilityPolygonPoints.end(),
		[&](const auto& t1, const auto& t2) {
			return std::get<0>(t1) < std::get<0>(t2);
		}
	);
}

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
	auto mouse = Window::getMousePosition();
	mouse.y = 540.f - mouse.y;

	if (Window::getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		int index = ((int)mouse.y / 60) * 16 + ((int)mouse.x / 60);
		cells[index].exists = !cells[index].exists;
	}

	getPolyMap(0, 0, 16, 9, 60.f, 16);

	if (Window::isKeyPressed(GLFW_KEY_E))
		calculateVisibilityPolygon(mouse.x, mouse.y, 1000.f);

	auto rayCast = visibilityPolygonPoints.size();
	auto it = std::unique(
		visibilityPolygonPoints.begin(),
		visibilityPolygonPoints.end(),
		[](const auto& t1, const auto& t2) {
			return fabs(std::get<1>(t1) - std::get<1>(t2)) < 0.1f && fabs(std::get<2>(t1) - std::get<2>(t2)) < 0.1f;
		}
	);
	visibilityPolygonPoints.resize(std::distance(visibilityPolygonPoints.begin(), it));

	if (Window::isKeyPressed(GLFW_KEY_E)) {
		for (size_t i = 0; i < visibilityPolygonPoints.size() - 1; ++i) {
			Renderer::drawTriangle({
				mouse,
				glm::vec2{
					std::get<1>(visibilityPolygonPoints[i]),
					std::get<2>(visibilityPolygonPoints[i])
				},
				glm::vec2{
					std::get<1>(visibilityPolygonPoints[i + 1]),
					std::get<2>(visibilityPolygonPoints[i + 1])
				}
			}, { 1.f, 1.f, 1.f, 1.f });
		}

		Renderer::drawTriangle({
			mouse,
			glm::vec2{
				std::get<1>(visibilityPolygonPoints[visibilityPolygonPoints.size() - 1]),
				std::get<2>(visibilityPolygonPoints[visibilityPolygonPoints.size() - 1])
			},
			glm::vec2{
				std::get<1>(visibilityPolygonPoints[0]),
				std::get<2>(visibilityPolygonPoints[0])
			}
		}, {1.f, 1.f, 1.f, 1.f});
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

	for (const auto& edge : edges) {
		if (edge.startX == edge.endX) {
			Renderer::drawQuad(
				{edge.startX, edge.startY},
				{2.f, edge.endY - edge.startY},
				{0.f, 1.f, 0.f, 1.f}
			);
		} else if (edge.startY == edge.endY) {
			Renderer::drawQuad(
				{edge.startX, edge.startY},
				{edge.endX - edge.startX, 2.f},
				{0.f, 1.f, 0.f, 1.f}
			);
		} else {
			std::cerr << edge.startX << " " << edge.startY << std::endl << edge.endX << " " << edge.endY << std::endl;
			throw std::runtime_error("no i chuj..");
		}
		
		Renderer::drawQuad(
			{edge.startX, edge.startY},
			{3.f, 3.f},
			{0.f, 0.f, 1.f, 1.f}
		);
		Renderer::drawQuad(
			{edge.endX, edge.endY},
			{4.f, 4.f},
			{0.f, 0.f, 1.f, 1.f}
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

	const auto worldWidth = 16;
	const auto worldHeight = 9;

	for (int x = 1; x < (worldWidth - 1); x++) {
		cells[1 * worldWidth + x].exists = true;
		cells[(worldHeight - 2) * worldWidth + x].exists = true;
	}

	for (int x = 1; x < (worldHeight - 1); x++) {
		cells[x * worldWidth + 1].exists = true;
		cells[x * worldWidth + (worldWidth - 2)].exists = true;
	}
}

void Application::initImGui() {
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Window::getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}
