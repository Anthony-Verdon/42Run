#include "Scenes/MenuScene/MenuScene.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/Time/Time.hpp"
#include "SaveDefines.hpp"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Load()
{
    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    camera.setPosition(ml::vec3(0, 0, 0));
    camera.setFrontDirection(ml::vec3(1, 0, 0));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));

    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));

    Json::Node file = Json::ParseFile(SCORE_FILE);
    characters.push_back({"assets/models/characters/duck.glb", 0, true, 0, DUCK});
    characters.push_back({"assets/models/characters/bear.glb", 0, file[BEAR_UNLOCK], BEAR_PRIZE, BEAR});
    characters.push_back({"assets/models/characters/dog.glb", 0, file[DOG_UNLOCK], DOG_PRIZE, DOG});
    nbStars = file[NB_STARS];

    for (size_t i = 0; i < characters.size(); i++)
    {
        ModelManager::AddModels(ModelLoader::LoadModel(characters[i].path));
        characters[i].modelIndex = ModelManager::GetNbModel() - 1;
        ModelManager::GetModel(characters[i].modelIndex).Init();
        ModelManager::GetModel(characters[i].modelIndex).Play("Idle");
    }

    characterSelect = 0;
    angle = 180;
    angleOffset = 360.0f / characters.size();
    UpdateCharacterSelect(file[CHARACTER_SELECTED]);

    canvas.Init();
}

void MenuScene::Run()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    ml::mat4 characterRotation = ml::rotate(ml::mat4(1.0f), 270, ml::vec3(0, 1, 0)); // rotation on himself
    ml::mat4 pointPosition = ml::translate(ml::mat4(1.0f), ml::vec3(4, -0.5, 0));    // point around characters rotate
    for (size_t i = 0; i < characters.size(); i++)
    {
        ml::mat4 characterRotationOverPoint = ml::rotate(ml::mat4(1.0f), angle - i * angleOffset, ml::vec3(0, 1, 0)) * ml::translate(ml::mat4(1.0f), ml::vec3(1, 0, 0)); // rotation around the point, rotate is the angle, translate is the distance to the point
        ml::mat4 transform = pointPosition * characterRotationOverPoint * characterRotation;                                                                             // final transform
        if (characters[i].unlock)
            ModelManager::Draw(characters[i].modelIndex, camera.getPosition(), lights, projection, view, transform);
        else
            ModelManager::Draw(characters[i].modelIndex, camera.getPosition(), lights, projection, view, transform, true, ml::vec3(0, 0, 0));
    }

    canvas.Update();
}

void MenuScene::Quit()
{
    Json::Node file = Json::ParseFile(SCORE_FILE);

    for (size_t i = 0; i < characters.size(); i++)
    {
        switch (characters[i].skin)
        {
        case BEAR:
            file[BEAR_UNLOCK] = characters[i].unlock;
            break;
        case DOG:
            file[DOG_UNLOCK] = characters[i].unlock;
            break;
        default:
            break;
        }
    }
    file[NB_STARS] = nbStars;
    file[CHARACTER_SELECTED] = characterSelect;

    Json::WriteFile(SCORE_FILE, file);
}

void MenuScene::UpdateCharacterSelect(int nbOffset)
{
    angle += angleOffset * nbOffset;
    characterSelect = characterSelect + nbOffset;
    if (characterSelect < 0)
        characterSelect = characters.size() - 1;
    characterSelect = characterSelect % characters.size();
}