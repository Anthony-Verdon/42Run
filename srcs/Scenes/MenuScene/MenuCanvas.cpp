#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/UI/Events.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"
#include "Engine/UI/UISprite/UISprite.hpp"
#include "Engine/UI/UIText/UIText.hpp"
#include "SaveDefines.hpp"

MenuCanvas::MenuCanvas()
{
    ml::vec2 sizeStarIcon = ml::vec2(50, 50);
    Sprite sprite;
    sprite.textureName = "star";
    sprite.textureSize = ml::vec2(1, 1);
    sprite.spriteCoords = ml::vec2(0, 0);
    sprite.size = sizeStarIcon;
    AddComponent(std::make_unique<UI::UISprite>(sprite, sizeStarIcon / 2));
    Json::Node file = Json::ParseFile(SCORE_FILE);
    int score = file[NB_STARS];
    AddComponent(std::make_unique<UI::UIText>(std::to_string(score), "arial", sizeStarIcon));
    ml::vec2 sizeButton = ml::vec2(100, 100);
    playButton = AddComponent(std::make_unique<UI::Button>("Play", "arial", (WindowManager::GetWindowSize() - sizeButton) / 2, sizeButton));

    ml::vec2 buttonPos = WindowManager::GetWindowSize();
    float offset = buttonPos.x * 0.25;
    buttonPos.x = offset - sizeButton.x / 2;
    buttonPos.y = buttonPos.y * 0.75;
    duckButton = AddComponent(std::make_unique<UI::Button>("duck", "arial", buttonPos, sizeButton));
    buttonPos.x += offset;
    if (file[BEAR_UNLOCK])
        bearButton = AddComponent(std::make_unique<UI::Button>("bear", "arial", buttonPos, sizeButton));
    else
        bearButton = AddComponent(std::make_unique<UI::Button>("unlock bear (" + std::to_string(BEAR_PRIZE) + ")", "arial", buttonPos, sizeButton));
    buttonPos.x += offset;
    if (file[DOG_UNLOCK])
        dogButton = AddComponent(std::make_unique<UI::Button>("dog", "arial", buttonPos, sizeButton));
    else
        dogButton = AddComponent(std::make_unique<UI::Button>("unlock dog (" + std::to_string(DOG_PRIZE) + ")", "arial", buttonPos, sizeButton));

    modelChosen = "assets/characters/duck.glb"; // default
}

MenuCanvas::~MenuCanvas()
{
}

void MenuCanvas::HandleEvents(UI::EventData &data)
{
    if (data.componentID == playButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF:
            SceneManager::SwitchScene(std::make_unique<GameplayScene>(modelChosen));
            break;
        default:
            break;
        }
    }
    else if (data.componentID == duckButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF:
            modelChosen = "assets/characters/duck.glb";
            std::cout << "duck chosen" << std::endl;
            break;
        default:
            break;
        }
    }
    else if (data.componentID == bearButton || data.componentID == dogButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF: {
            std::string unlock_tag, character_name;
            int prize;
            if (data.componentID == bearButton)
            {
                character_name = "bear";
                unlock_tag = BEAR_UNLOCK;
                prize = BEAR_PRIZE;
            }
            else
            {
                character_name = "dog";
                unlock_tag = DOG_UNLOCK;
                prize = DOG_PRIZE;
            }
            Json::Node file = Json::ParseFile(SCORE_FILE);
            if (!file[unlock_tag.c_str()])
            {
                int nbStars = file[NB_STARS];
                if (nbStars >= prize)
                {
                    file[NB_STARS] = nbStars - prize;
                    file[unlock_tag.c_str()] = true;
                    auto &component = GetComponent(data.componentID);
                    auto button = dynamic_cast<UI::Button *>(component.get());
                    button->UpdateText(character_name);
                    Json::WriteFile(SCORE_FILE, file);
                }
                else
                {
                    std::cout << "not enough stars to unlock " << character_name << std::endl;
                }
            }
            if (file[unlock_tag.c_str()])
            {
                modelChosen = "assets/characters/" + character_name + ".glb";
                std::cout << character_name << " chosen" << std::endl;
            }
            break;
        }
        default:
            break;
        }
    }
}