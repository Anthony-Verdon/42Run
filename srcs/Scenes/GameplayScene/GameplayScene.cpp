#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Game/Layers.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "MapManager/MapManager.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Engine/3D/WorldPhysic3D/ObjectAndBroadPhaseLayer/ObjectAndBroadPhaseLayer.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/2D/Renderers/TextRenderer/TextRenderer.hpp"
#if DRAW_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

void SetUpLayers();

GameplayScene::GameplayScene(const std::string &playerModelPath) : playerModelPath(playerModelPath)
{
}

GameplayScene::~GameplayScene()
{
}

void GameplayScene::Load()
{
    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ChunkGenerator::Init();
    WorldPhysic3D::Init(SetUpLayers);
    MapManager::Init();

    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));
    player.Init(playerModelPath);
    accumulatedTime = 0;
#if DRAW_IMGUI
    isWindowFocused = false;
#endif

    AudioManager::InitSound(backgroundAudio, "assets/sounds/3. Road Trip through Nevada.wav");
    backgroundAudio.SetLooping(true);
    backgroundAudio.Play();
}

void GameplayScene::Run()
{
    accumulatedTime += Time::getDeltaTime();
    while (accumulatedTime >= WorldPhysic3D::GetDeltaTime())
    {
        WorldPhysic3D::Update();
        accumulatedTime -= WorldPhysic3D::GetDeltaTime();
    }

    MapManager::UpdateTerrain(player.GetPosition(), player.GetDirection());
    ProcessInput();

    UpdateCamera();
    Draw();
}

void GameplayScene::Quit()
{
    player.Destroy();
    WorldPhysic3D::Destroy();

    backgroundAudio.SetLooping(false);
    backgroundAudio.Stop();
}

void GameplayScene::ProcessInput()
{
#if DRAW_IMGUI
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE) && WindowManager::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else if (WindowManager::IsInputPressed(GLFW_MOUSE_BUTTON_1) && WindowManager::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_NORMAL && !isWindowFocused)
        WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

    player.ProcessInput();
    player.Update();
}

void GameplayScene::UpdateCamera()
{
#if CAMERA_MODE == 0 // FOLLOWING PLAYER
    ml::vec3 cameraPosition;
    if (player.IsDefeated())
        cameraPosition = (ml::vec3(0, 1, 0) * 3 - ml::normalize(player.GetDirection()) * 5);
    else
        cameraPosition = (ml::vec3(0, 1, 0) - ml::normalize(player.GetDirection())) * 3;
    camera.setPosition(player.GetPosition() + cameraPosition);

    ml::vec3 cameraOrientation = ml::vec3(0, -0.25, 0);
    camera.setFrontDirection(ml::normalize(player.GetDirection() + cameraOrientation));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
#elif CAMERA_MODE == 1 // DETACH
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
#elif CAMERA_MODE == 2 // SIDE
    ml::vec3 playerDirection = player.GetDirection();
    std::swap(playerDirection.x, playerDirection.z);
    ml::vec3 cameraPosition = ml::vec3(0, 1, 0) * 3 - ml::normalize(playerDirection) * 10;
    camera.setPosition(player.GetPosition() + cameraPosition);

    ml::vec3 cameraOrientation = ml::vec3(0, -0.25, 0);
    camera.setFrontDirection(ml::normalize(playerDirection + cameraOrientation));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
#endif
}

void GameplayScene::Draw()
{
#if DRAW_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif

    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    player.Draw(camera.getPosition(), lights, projection, view);

    auto chunks = MapManager::GetChunks();
    float translationValue = sin(Time::getTime()) / 2 + 0.5;
    float rotationValue = (int)ml::degrees(Time::getTime() * 2) % 360;
    for (; !chunks.empty(); chunks.pop())
    {
        Chunk &chunk = chunks.front();
        for (int i = 0; i < LaneType::COUNT; i++)
        {
            Lane &lane = chunk.lanes[i];
            for (auto it = lane.tiles.begin(); it != lane.tiles.end(); it++)
            {
                const std::shared_ptr<Tile42Run> &tile = *it;
                if (tile->flag & TileFlag::DONT_DRAW)
                    continue;
                ml::mat4 translation = ml::mat4(1.0f);
                ml::mat4 rotation = ml::mat4(1.0f);
                if (tile->flag & TileFlag::UP_AND_DOWN)
                    translation = ml::translate(translation, ml::vec3(0, translationValue, 0));
                if (tile->flag & TileFlag::ROTATE_OVER_TIME)
                    rotation = ml::rotate(rotation, rotationValue, ml::vec3(0, 1, 0));
                ModelManager::Draw(tile->modelIndex, camera.getPosition(), lights, projection, view, tile->transform * translation * rotation);
            }
        }
    }

#if DRAW_IMGUI
    isWindowFocused = player.IsWindowFocused();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
#if DEBUG_DRAW_PHYSIC_3D
    WorldPhysic3D::DebugDraw();
#endif
    canvas.Update();
    LineRenderer3D::Draw(projection, view);
}

void SetUpLayers()
{
    WorldPhysic3D::broadPhaseLayerInterface->AddObjectToBroadPhaseLayerRelation(ObjectLayers::NON_MOVING, BroadPhaseLayers::NON_MOVING, "NON_MOVING");
    WorldPhysic3D::broadPhaseLayerInterface->AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::MOVING, "MOVING");
    WorldPhysic3D::objectVsBroadPhaseLayerFilter->AddObjectToBroadPhaseLayerRelation(ObjectLayers::NON_MOVING, BroadPhaseLayers::MOVING);
    WorldPhysic3D::objectVsBroadPhaseLayerFilter->AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::MOVING);
    WorldPhysic3D::objectVsBroadPhaseLayerFilter->AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::NON_MOVING);
    WorldPhysic3D::objectLayerPairFilter->AddObjectToObjectRelation(ObjectLayers::NON_MOVING, ObjectLayers::MOVING);
    WorldPhysic3D::objectLayerPairFilter->AddObjectToObjectRelation(ObjectLayers::MOVING, ObjectLayers::MOVING);
    WorldPhysic3D::objectLayerPairFilter->AddObjectToObjectRelation(ObjectLayers::MOVING, ObjectLayers::NON_MOVING);
}