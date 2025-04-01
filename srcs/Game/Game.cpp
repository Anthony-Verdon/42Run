#include "Game/Game.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "globals.hpp"
#include "Engine/Time/Time.hpp"
#include <iostream>

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

Game::Game()
{
    WindowManager::SetUserPointer(&camera);
    WindowManager::SetScrollCallback(scroll_callback);
    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    lights.push_back(std::make_unique<PointLight>(ml::vec3(1, 0, 0), 15, ml::vec3(0, 0, -3)));
    lights.push_back(std::make_unique<PointLight>(ml::vec3(0, 1, 0), 15, ml::vec3(0, 0, 3)));
    lights.push_back(std::make_unique<PointLight>(ml::vec3(0, 0, 1), 15, ml::vec3(-3, 0, 0)));
    lights.push_back(std::make_unique<PointLight>(ml::vec3(1, 1, 1), 15, ml::vec3(3, 0, 0)));
    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 0), 3, ml::vec3(0, -1, 0)));
    lights.push_back(std::make_unique<SpotLight>(ml::vec3(0, 1, 1), 15, camera.getPosition(), camera.getFrontDirection(), 12.5f, 17.5f));

    LoadAssets();
}

Game::~Game()
{
    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Destroy();
}

void Game::LoadAssets()
{
    ModelManager::AddModels(ModelLoader::LoadModel("assets/duck.glb"));

    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Init();
}

void Game::Run()
{
    ProcessInput();

    // models
    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    ml::mat4 transform(1.0f);

    auto light = dynamic_cast<SpotLight*>(lights[5].get());
    light->position = camera.getPosition();
    light->direction = camera.getFrontDirection();
    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Draw(camera.getPosition(), lights, projection, view, transform);
}

void Game::ProcessInput()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    UpdateCamera();
}

void Game::UpdateCamera()
{
    // position
    const float speed = camera.getSpeed() * Time::getDeltaTime();

    int front = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_W) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_S);
    camera.addToPosition((float)front * camera.getFrontDirection() * speed);

    int right = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A);
    camera.addToPosition((float)right * camera.getRightDirection() * speed);

    int up = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_SPACE) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT_SHIFT);
    camera.addToPosition((float)up * camera.getUpDirection() * speed);

    // orientation
    const float sensitivity = 0.1f;

    ml::vec2 mousePos = WindowManager::GetMousePosition();
    static float lastX = mousePos.x;
    static float lastY = mousePos.y;

    float xOffset;
    float yOffset;
    xOffset = (mousePos.x - lastX) * sensitivity;
    yOffset = (lastY - mousePos.y) * sensitivity;
    lastX = mousePos.x;
    lastY = mousePos.y;
    camera.addToYaw(xOffset);
    camera.addToPitch(yOffset);
    if (camera.getPitch() > 89.0f)
        camera.setPitch(89.0f);
    else if (camera.getPitch() < -89.0f)
        camera.setPitch(-89.0f);

    ml::vec3 direction(cosf(ml::radians(camera.getYaw())) * cosf(ml::radians(camera.getPitch())),
                sinf(ml::radians(camera.getPitch())),
                sinf(ml::radians(camera.getYaw())) * cosf(ml::radians(camera.getPitch())));
    camera.setFrontDirection(ml::normalize(direction));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    (void)xOffset;

    Camera3D *camera = reinterpret_cast<Camera3D *>(glfwGetWindowUserPointer(window));
    camera->addToFov((float)-yOffset);
    if (camera->getFov() < 1.0f)
        camera->setFov(1.0f);
    else if (camera->getFov() > 45.0f)
        camera->setFov(45.0f);
}