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
    ModelManager::GetModel(player.GetModelIndex()).Play("Run");
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
    ModelManager::AddModels(ModelLoader::LoadModel("assets/swiper/classic/swiper_teamBlue.gltf.glb"));

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

    player.ProcessInput();
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
    player.Draw(camera.getPosition(), lights, projection, view);

    auto tiles = MapManager::UpdateTerrain(player.GetPosition() / 2.0f);
    for (auto it = tiles.begin(); it != tiles.end(); it++)
        ModelManager::GetModel(it->modelIndex).Draw(camera.getPosition(), lights, projection, view, it->transform); 
}