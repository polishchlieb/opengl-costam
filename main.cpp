#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Application.hpp"
#include "rendering/Window.hpp"
#include "scene/Camera3D.hpp"

int main() {
	Window::init();
	Window::create("hell world", 960, 540);

    glfwSetInputMode(Window::getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(Window::getWindow(), [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

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

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int vbo, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    static Camera3D camera{{0.f, 0.f, 3.f}};
    camera.lookAt({0.f, 0.f, 0.f});

    static float lastX = 960.f / 2, lastY = 540.f / 2;
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

    glm::vec3 lightPos{2137.f, 0.f, 0.f};

    float deltaTime = 0.f;
    float lastFrame = 0.f;
	while (!Window::shouldClose()) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (Window::isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window::getWindow(), true);
        
        float cameraSpeed = 2.5f * deltaTime;
        if (Window::isKeyPressed(GLFW_KEY_W))
            camera.move(cameraSpeed * camera.getFront());
        if (Window::isKeyPressed(GLFW_KEY_S))
            camera.move(-(cameraSpeed * camera.getFront()));
        if (Window::isKeyPressed(GLFW_KEY_A))
            camera.move(-(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed));
        if (Window::isKeyPressed(GLFW_KEY_D))
            camera.move(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPos.x = cos(glfwGetTime()) * 2.f;
        lightPos.z = sin(glfwGetTime()) * 2.f;
        lightPos.y = pow(cos(glfwGetTime()), 2) * 2.f;

        shader.bind();
        shader.setUniformVec3("objectColor", {1.f, .5f, .31f});
        shader.setUniformVec3("lightColor", {1.f, 1.f, 1.f});
        shader.setUniformVec3("lightPos", lightPos);
        shader.setUniformVec3("viewPos", camera.getPosition());

        auto model = glm::mat4(1.f);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 960.f / 540.f, .1f, 100.f);

        auto view = camera.calculateViewMatrix();

        shader.setUniformMat4f("model", model);
        shader.setUniformMat4f("u_MVP", projection * view * model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightShader.bind();

        model = glm::translate(glm::mat4(1.f), lightPos);
        model = glm::scale(model, glm::vec3(.2f));
        lightShader.setUniformMat4f("u_MVP", projection * view * model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        Window::swapBuffers();
        glfwPollEvents();
	}

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

	return 0;
}