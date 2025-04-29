#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Game/Game.hpp"
#include "MapManager/MapManager.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "WorldPhysic/DebugRenderer.hpp"
#include "globals.hpp"
#include "Engine/Time/Time.hpp"
#include <iostream>

Game::Game()
{
    srand(time(NULL));

    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    LineRenderer3D::Init();
    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));

    LoadAssets();
    
    WorldPhysic3D::Init(BPLayerInterface, ObjectVsBPLayerFilter, OBjectLPFilter);
    WorldPhysic3D::SetContactListener(&contactListener);
    JPH::DebugRenderer::sInstance = new DebugRendererImpl();
    player.SetModelIndex(0);
    ModelManager::GetModel(player.GetModelIndex()).Play("Run");
    player.SetPosition(ml::vec3(0, 1, 0));
    player.Init();
    MapManager::Init();

    accumulatedTime = 0;
}

Game::~Game()
{
    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Destroy();

    player.Destroy();
    LineRenderer3D::Destroy();
    WorldPhysic3D::Destroy();
    delete JPH::DebugRenderer::sInstance;
    JPH::DebugRenderer::sInstance = nullptr;

}

void Game::LoadAssets()
{
    ModelManager::AddModels(ModelLoader::LoadModel("assets/duck.glb"));
    ModelManager::AddModels(ModelLoader::LoadModel("assets/tiles/large/tileLarge_teamBlue.gltf.glb"));
    ModelManager::AddModels(ModelLoader::LoadModel("assets/tiles/high/tileHigh_teamBlue.gltf.glb"));
    ModelManager::AddModels(ModelLoader::LoadModel("assets/tiles/low/tileLow_teamBlue.gltf.glb"));
    ModelManager::AddModels(ModelLoader::LoadModel("assets/slopes/lowMedium/tileSlopeLowMedium_teamBlue.gltf.glb"));
    ModelManager::AddModels(ModelLoader::LoadModel("assets/slopes/mediumHigh/tileSlopeMediumHigh_teamBlue.gltf.glb"));

    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Init();
}

void Game::Run()
{
    accumulatedTime += Time::getDeltaTime();
    MapManager::UpdateTerrain(player.GetPosition(), player.GetDirection());
    ProcessInput();
    
    while (accumulatedTime >= WorldPhysic3D::GetDeltaTime()) 
    {
        WorldPhysic3D::Update();
        accumulatedTime -= WorldPhysic3D::GetDeltaTime();
    }
    
    UpdateCamera();
    Draw();
    WorldPhysic3D::DebugDraw({}, JPH::DebugRenderer::sInstance);

    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    LineRenderer3D::Draw(projection, view);
}

void Game::ProcessInput()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    player.ProcessInput();
    player.Update();
}

//#define CAMERA_DETACH

void Game::UpdateCamera()
{
    #ifdef CAMERA_DETACH
    // position
    camera.setSpeed(20);
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
    #else
    ml::vec3 cameraPosition;
    if (player.IsDefeated())
        cameraPosition = (ml::vec3(0, 1, 0) * 3 - ml::normalize(player.GetDirection()) * 5);
    else
        cameraPosition = (ml::vec3(0, 1, 0) - ml::normalize(player.GetDirection())) * 3;
    camera.setPosition(player.GetPosition() + cameraPosition);

    ml::vec3 cameraOrientation = ml::vec3(0, -0.25, 0);
    camera.setFrontDirection(ml::normalize(player.GetDirection() + cameraOrientation));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
    #endif
}

void Game::Draw()
{
    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    player.Draw(camera.getPosition(), lights, projection, view);

    auto chunks = MapManager::GetChunks();
    for (; !chunks.empty(); chunks.pop())
    {
        for (auto it = chunks.front().tiles.begin(); it != chunks.front().tiles.end(); it++)
            ModelManager::GetModel(it->modelIndex).Draw(camera.getPosition(), lights, projection, view, it->transform); 
    }
}