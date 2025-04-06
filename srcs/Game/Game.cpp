#include "Game/Game.hpp"
#include "MapManager/MapManager.hpp"
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
    player.SetModelIndex(0);
    ModelManager::GetModel(0).Play("Run");
    player.SetPosition(ml::vec3(0, 1, 0));
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

    /*
    int frontBack = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_W) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_S);
    int leftRight = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A);
    ml::vec3 rightDirection =  ml::normalize(ml::crossProduct(player.GetDirection(), ml::vec3(0, 1, 0)));
    ml::vec3 position = frontBack * player.GetDirection() + leftRight * rightDirection;
    if (position != ml::vec3(0, 0, 0))
        position = ml::normalize(position);
    player.AddToPosition(position * Time::getDeltaTime() * 5);

    int turn = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_RIGHT) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT);
    player.AddToAngle((float)turn * Time::getDeltaTime() * 5);
    player.SetDirection(ml::normalize(ml::vec3(-sinf(player.GetAngle()), 0, cosf(player.GetAngle()))));
    */
    player.SetDirection(ml::normalize(ml::vec3(-sinf(player.GetAngle()), 0, cosf(player.GetAngle()))));
    player.AddToPosition(player.GetDirection() * Time::getDeltaTime() * 5);

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

    auto tiles = MapManager::UpdateTerrain(player.GetPosition() / 2.0f);
    for (auto it = tiles.begin(); it != tiles.end(); it++)
        ModelManager::GetModel(it->second).Draw(camera.getPosition(), lights, projection, view, ml::translate(ml::mat4(1.0f), it->first * 2.0f)); 
}