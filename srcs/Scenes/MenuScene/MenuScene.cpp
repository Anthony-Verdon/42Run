#include "Scenes/MenuScene/MenuScene.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/Time/Time.hpp"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Load()
{
    camera.setPosition(ml::vec3(0, 0, 0));
    camera.setFrontDirection(ml::vec3(1, 0, 0));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));

    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));

    paths = {
        "assets/models/characters/duck.glb",
        "assets/models/characters/bear.glb",
        "assets/models/characters/dog.glb",
    };

    for (size_t i = 0; i < paths.size(); i++)
    {
        ModelManager::AddModels(ModelLoader::LoadModel(paths[i]));
        int modelIndex = ModelManager::GetNbModel() - 1;
        ModelManager::GetModel(modelIndex).Init();
        ModelManager::GetModel(modelIndex).Play("Idle");
        charactersModelIndex.push_back(modelIndex);
    }

    characterSelect = 0;
    angle = 180;
    angleOffset = 360.0f / charactersModelIndex.size();
}

void MenuScene::Run()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    ml::mat4 characterRotation = ml::rotate(ml::mat4(1.0f), 270, ml::vec3(0, 1, 0)); // rotation on himself
    ml::mat4 pointPosition = ml::translate(ml::mat4(1.0f), ml::vec3(4, -0.5, 0));    // point around characters rotate
    for (size_t i = 0; i < charactersModelIndex.size(); i++)
    {
        ml::mat4 characterRotationOverPoint = ml::rotate(ml::mat4(1.0f), angle - i * angleOffset, ml::vec3(0, 1, 0)) * ml::translate(ml::mat4(1.0f), ml::vec3(1, 0, 0)); // rotation around the point, rotate is the angle, translate is the distance to the point
        ml::mat4 transform = pointPosition * characterRotationOverPoint * characterRotation;                                                                             // final transform
        ModelManager::Draw(charactersModelIndex[i], camera.getPosition(), lights, projection, view, transform);
    }
    canvas.Update();
}

void MenuScene::Quit()
{
}

void MenuScene::UpdateCharacterSelect(int nbOffset)
{
    angle += angleOffset * nbOffset;
    characterSelect = characterSelect + nbOffset;
    if (characterSelect < 0)
        characterSelect = charactersModelIndex.size() - 1;
    characterSelect = characterSelect % charactersModelIndex.size();
}