#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Application.hpp"
#include "rendering/Window.hpp"
#include "scene/Camera3D.hpp"

#include <sstream>
#include "rendering/ModelLoader.hpp"

#include "rendering/ColorTexture.hpp"

int main() {
	Window::init();
	Window::create("hell world", 960, 540);

    glfwSetInputMode(Window::getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader;
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
    
    Shader lightShader;
    lightShader.attach(ShaderComponent::fromFile(
        GL_VERTEX_SHADER,
        "light.vertex.shader"
    ));
    lightShader.attach(ShaderComponent::fromFile(
        GL_FRAGMENT_SHADER,
        "light.fragment.shader"
    ));
    lightShader.link();
    lightShader.validate();

    ColorTexture whiteTexture{{1.f, 1.f, 1.f, 1.f}};
    whiteTexture.bind(1);
    Texture chungusTexture{ "res/meshes/chungus-1/chungus_TM_u0_v0.png" };
    chungusTexture.bind(2);

    auto chungusVertices = loadObj("res/meshes/chungus-1/chungus.obj");
    /* float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, 0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.f, 0.f, 0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, 0.f, 0.f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.f, 0.f, 1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.f, 0.f, 0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.f, 0.f, 0.0f,  1.0f,  0.0f
    }; */
    auto vertices = loadObj("res/meshes/cube/cube.obj");

    unsigned int chungusVBO, chungusVAO;
    glGenVertexArrays(1, &chungusVAO);
    glGenBuffers(1, &chungusVBO);
    glBindBuffer(GL_ARRAY_BUFFER, chungusVBO);
    glBufferData(GL_ARRAY_BUFFER, chungusVertices.size() * sizeof(Vertex), chungusVertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(chungusVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(lightCubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    static Camera3D camera{{0.f, 60.f, 70.f}};
    camera.lookAt({0.f, 0.f, 0.f});

    static float lastX = Window::getSize().x / 2, lastY = Window::getSize().y / 2;
    static float yaw = -90.f, pitch = 0.f;
    glfwSetCursorPosCallback(Window::getWindow(), [](GLFWwindow*, double xpos, double ypos) {
        auto xoffset = static_cast<float>(xpos) - lastX;
        auto yoffset = lastY - static_cast<float>(ypos);
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        
        constexpr float sensitivity = 0.08f;
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch > 89.f)
            pitch = 89.f;
        if (pitch < -89.f)
            pitch = -89.f;

        camera.lookAt(yaw, pitch);
    });

    static float fov = 45.f;
    glfwSetScrollCallback(Window::getWindow(), [](GLFWwindow* window, double xoffset, double yoffset) {
        fov -= static_cast<float>(yoffset);
        if (fov < 1.f)
            fov = 1.f;
        if (fov > 90.f)
            fov = 90.f;
    });

    glm::vec3 lightPos{0.f, 120.f, 100.f};

    float deltaTime = 0.f;
    float lastFrame = 0.f;
	while (!Window::shouldClose()) {
        // calculate deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // handle input
        if (Window::isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window::getWindow(), true);
        
        float cameraSpeed = 10.f * deltaTime;
        if (Window::isKeyPressed(GLFW_KEY_W))
            camera.move(cameraSpeed * camera.getFront());
        if (Window::isKeyPressed(GLFW_KEY_S))
            camera.move(-(cameraSpeed * camera.getFront()));
        if (Window::isKeyPressed(GLFW_KEY_A))
            camera.move(-(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed));
        if (Window::isKeyPressed(GLFW_KEY_D))
            camera.move(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed);

        // update light position
        /* lightPos.x = cos(glfwGetTime()) * 2.f;
        lightPos.z = sin(glfwGetTime()) * 2.f;
        lightPos.y = pow(cos(glfwGetTime()), 2) * 2.f; */

        // calculate matrices
        auto projection = glm::perspective(glm::radians(fov), Window::getAspectRatio(), .1f, 100.f);
        auto view = camera.calculateViewMatrix();

        // render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw chungus
        auto chungusModel = glm::mat4(1.f);
        shader.bind();
        shader.setUniformVec3("lightColor", {1.f, 1.f, 1.f});
        shader.setUniformVec3("lightPos", lightPos);
        shader.setUniformVec3("viewPos", camera.getPosition());
        shader.setUniformSampler2D("textureID", 2);
        shader.setUniformMat4f("model", chungusModel);
        shader.setUniformMat4f("u_MVP", projection * view * chungusModel);
        glBindVertexArray(chungusVAO);
        glDrawArrays(GL_TRIANGLES, 0, chungusVertices.size());

        // draw light
        auto lightModel = glm::translate(glm::mat4(1.f), lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(.2f));
        lightShader.bind();
        lightShader.setUniformMat4f("u_MVP", projection * view * lightModel);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        Window::swapBuffers();
        glfwPollEvents();
	}

    glDeleteBuffers(1, &chungusVBO);
    glDeleteBuffers(1, &lightCubeVBO);
    glDeleteVertexArrays(1, &chungusVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);

    glfwTerminate();

	return 0;
}