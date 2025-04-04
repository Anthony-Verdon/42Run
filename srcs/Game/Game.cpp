#include "Game/Game.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "globals.hpp"
#include "Engine/Time/Time.hpp"
#include <iostream>

Game::Game()
{
    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));

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
    ModelManager::AddModels(ModelLoader::LoadModel("assets/tiles/low/tileLow_teamBlue.gltf.glb"));
    player.SetModelIndex(0);

    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Init();
}

void Game::Run()
{
    ProcessInput();
    Draw();
}

void Game::ProcessInput()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    int frontBack = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_W) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_S);
    int leftRight = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A);
    ml::vec3 rightDirection =  ml::normalize(ml::crossProduct(player.GetDirection(), ml::vec3(0, 1, 0)));
    ml::vec3 position = frontBack * player.GetDirection() + leftRight * rightDirection;
    if (position != ml::vec3(0, 0, 0))
        position = ml::normalize(position);
    player.AddToPosition(position * Time::getDeltaTime());

    int turn = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_RIGHT) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT);
    player.AddToAngle((float)turn * Time::getDeltaTime());
    player.SetDirection(ml::normalize(ml::vec3(-sinf(player.GetAngle()), 0, cosf(player.GetAngle()))));

    UpdateCamera();
}

void Game::UpdateCamera()
{
    ml::vec3 cameraPosition = (ml::vec3(0, 1, 0) - ml::normalize(player.GetDirection())) * 3;
    camera.setPosition(player.GetPosition() + cameraPosition);

    ml::vec3 cameraOrientation = ml::vec3(0, -0.25, 0);
    camera.setFrontDirection(ml::normalize(player.GetDirection() + cameraOrientation));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
}

void Game::Draw()
{
    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    ml::mat4 transform = ml::translate(ml::mat4(1.0f), player.GetPosition()) * ml::rotate(ml::mat4(1.0f), ml::degrees(player.GetAngle()), ml::vec3(0, 1, 0));
    ModelManager::GetModel(player.GetModelIndex()).Draw(camera.getPosition(), lights, projection, view, transform);

    ml::vec2 path[] = {
        ml::vec2(0, 0),
        ml::vec2(0, 1),
        ml::vec2(0, 2),
        ml::vec2(0, 3)
    };

    size_t size = sizeof(path) / sizeof(ml::vec2);
    for (size_t i = 0; i < size; i++)
        ModelManager::GetModel(1).Draw(camera.getPosition(), lights, projection, view, ml::translate(ml::mat4(1.0f), ml::vec3(path[i].x, 0, path[i].y) * 2.0f));
        
}