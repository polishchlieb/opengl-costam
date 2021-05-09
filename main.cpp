#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Application.hpp"
#include "rendering/Window.hpp"
#include "scene/Camera3D.hpp"

#include <sstream>
#include "rendering/ModelLoader.hpp"

#include "rendering/ColorTexture.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/GL.hpp"

int main() {
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

    ColorTexture whiteTexture{{1.f, 1.f, 1.f, 1.f}};
    Texture chungusTexture{"res/meshes/chungus-1/chungus_TM_u0_v0.png"};

    Mesh chungus{"res/meshes/chungus-1/chungus.obj"};
    Mesh lightCube{"res/meshes/cube/cube.obj"};

    static Camera3D camera{{0.f, 60.f, 70.f}};
    camera.lookAt({0.f, 0.f, 0.f});

    static float lastX = Window::getSize().x / 2, lastY = Window::getSize().y / 2;
    static float yaw = -90.f, pitch = 0.f;
    Window::onCursorMove([](glm::vec2, glm::vec2 offset) {
        constexpr float sensitivity = 0.08f;
        yaw += offset.x * sensitivity;
        pitch += offset.y * sensitivity;

        if (pitch > 89.f)
            pitch = 89.f;
        if (pitch < -89.f)
            pitch = -89.f;

        camera.lookAt(yaw, pitch);
    });

    static float fov = 45.f;
    Window::onScroll([](float offset) {
        fov -= static_cast<float>(offset);
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
        float currentFrame = Window::getTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // handle input
        if (Window::isKeyPressed(Window::Key::Esc))
            Window::close();
        
        float cameraSpeed = 10.f * deltaTime;
        if (Window::isKeyPressed(Window::Key::W))
            camera.move(cameraSpeed * camera.getFront());
        if (Window::isKeyPressed(Window::Key::S))
            camera.move(-(cameraSpeed * camera.getFront()));
        if (Window::isKeyPressed(Window::Key::A))
            camera.move(-(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed));
        if (Window::isKeyPressed(Window::Key::D))
            camera.move(glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * cameraSpeed);

        // update light position
        lightPos.y = cos(Window::getTime()) * 50.f + 70.f;

        // calculate matrices
        auto projection = glm::perspective(glm::radians(fov), Window::getAspectRatio(), .1f, 100.f);
        auto view = camera.calculateViewMatrix();

        // render
        Window::clearBuffers(
            Window::BufferType::ColorBuffer | Window::BufferType::DepthBuffer
        );

        // draw chungus
        auto chungusModel = glm::mat4(1.f);
        shader.bind();
        shader.setUniformVec3("lightColor", {1.f, 1.f, 1.f});
        shader.setUniformVec3("lightPos", lightPos);
        shader.setUniformVec3("viewPos", camera.getPosition());
        chungusTexture.bind(0);
        shader.setUniformSampler2D("textureID", 0);
        shader.setUniformMat4f("model", chungusModel);
        shader.setUniformMat4f("u_MVP", projection * view * chungusModel);
        chungus.draw();

        // draw light
        auto lightModel = glm::translate(glm::mat4(1.f), lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(.2f));
        lightShader.bind();
        lightShader.setUniformMat4f("u_MVP", projection * view * lightModel);
        lightCube.draw();

        Window::swapBuffers();
        Window::pollEvents();
    }

    Window::terminate();
}