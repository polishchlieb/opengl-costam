#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "rendering/Window.hpp"
#include "scene/Camera3D.hpp"

#include <sstream>
#include "rendering/ModelLoader.hpp"

#include "rendering/ColorTexture.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/GL.hpp"
#include "rendering/Renderer2D.hpp"

#include "shader/Shader.hpp"
#include "shader/ShaderComponent.hpp"

#include <iomanip>

int main() {
    Window::init();
    Window::create("hell world", 960, 540);

    Window::setCursorMode(Window::CursorMode::Disabled);

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

    ColorTexture whiteTexture{{1.f, 1.f, 1.f, 1.f}};
    ColorTexture pinkTexture({{1.f, .75f, .6f, 1.f}});
    Texture chungusTexture{"res/meshes/chungus-1/chungus_TM_u0_v0.png"};

    Mesh chungus{"res/meshes/chungus-1/chungus.obj"};
    Mesh lightCube{"res/meshes/cube/cube.obj"};
    Mesh peter{"res/meshes/piotr-g-1/peter.obj"};

    Font comicSans{"res/fonts/comic.ttf", 48};

    static Camera3D camera{{0.f, 5.f, 20.f}};
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

    glm::vec3 lightPos{0.f, 5.f, 10.f};

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    auto chungusModel = glm::scale(glm::mat4(1.f), glm::vec3(.1f));
    auto peterModel = glm::translate(glm::mat4(1.f), glm::vec3(-20.f, 0.f, 0.f));
    peterModel = glm::scale(peterModel, glm::vec3(3.f));

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
        // lightPos.y = cos(Window::getTime()) * 4.5f + 5.5f;

        // calculate matrices
        auto projection = glm::perspective(glm::radians(fov), Window::getAspectRatio(), .1f, 100.f);
        auto view = camera.calculateViewMatrix();

        // render
        Window::clearBuffers(
            Window::BufferType::ColorBuffer | Window::BufferType::DepthBuffer
        );

        { // render 3d shit
            GL::enableDepthTesting();

            // draw chungus
            shader.bind();
            // shader.setUniformVec3("lightColor", { 1.f, 1.f, 1.f });
            shader.setUniformVec3("lightPos", lightPos);
            shader.setUniformVec3("viewPos", camera.getPosition());

            glm::vec3 lightColor{
                1.f, 1.f, 1.f
                // sin(Window::getTime() * 2.f),
                // sin(Window::getTime() * .7f),
                // sin(Window::getTime() * 1.3f)
            };

            auto diffuseColor = lightColor * glm::vec3{.5f};
            auto ambientColor = lightColor * glm::vec3{.2f};

            chungusTexture.bind(0);

            shader.setUniformSampler2D("material.diffuse", 0);
            shader.setUniformVec3("material.specular", {.5f, .5f, .5f});
            shader.setUniformFloat("material.shininess", 32.0f);

            // https://learnopengl.com/Lighting/Light-casters
            // https://learnopengl.com/Lighting/Multiple-lights

            shader.setUniformVec3("light.ambient", ambientColor);
            shader.setUniformVec3("light.diffuse", diffuseColor);
            shader.setUniformVec3("light.specular", {1.f, 1.f, 1.f});

            shader.setUniformSampler2D("textureID", 0);
            shader.setUniformMat4f("model", chungusModel);
            shader.setUniformMat4f("u_MVP", projection * view * chungusModel);
            chungus.draw();

            // draw peter griffin
            shader.bind();
            // shader.setUniformVec3("lightColor", { 1.f, 1.f, 1.f });
            shader.setUniformVec3("lightPos", lightPos);
            shader.setUniformVec3("viewPos", camera.getPosition());
            pinkTexture.bind(1);
            shader.setUniformSampler2D("material.diffuse", 1);
            shader.setUniformSampler2D("textureID", 1);
            shader.setUniformMat4f("model", peterModel);
            shader.setUniformMat4f("u_MVP", projection * view * peterModel);
            peter.draw();
            //peter

            // draw light
            auto lightModel = glm::translate(glm::mat4(1.f), lightPos);
            lightModel = glm::scale(lightModel, glm::vec3(.2f));
            lightShader.bind();
            lightShader.setUniformMat4f("u_MVP", projection * view * lightModel);
            lightShader.setUniformVec4("u_Color", lightColor[0], lightColor[1], lightColor[2], 1.f);
            lightCube.draw();
        }
        { // draw 2d shit
            GL::disableDepthTesting();

            auto projection2D = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
            shader2D.bind();
            shader2D.createSamplers();
            shader2D.setUniformMat4f("mvp", projection2D);
            
            float hp = (sinf(Window::getTime() / 3) + 1.f) / 2.f;
            std::ostringstream percent;
            percent << std::fixed << std::setprecision(1) << hp * 100 << "%";
            const auto str = percent.str();
            float size = comicSans.measureText(str, 1.f);

            Renderer2D::beginBatch();

            Renderer2D::drawQuad({20.f, 20.f }, {350.f, 50.f}, {1.f, 0.f, 0.f, 1.f});
            Renderer2D::drawQuad({20.f, 20.f }, {hp * 350.f, 50.f}, {1.f, 1.f, 0.f, 1.f});
            Renderer2D::drawText({20.f + (350.f - size) / 2, 28.5f}, comicSans, str, 1.f, {0.f, 0.f, 0.f, 1.f});

            Renderer2D::endBatch();
            Renderer2D::render();
        }

        Window::swapBuffers();
        Window::pollEvents();
    }

    Window::terminate();
}