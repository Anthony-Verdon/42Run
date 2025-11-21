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
    UI::ACanvas::BeginCanvas(this);
    ml::vec2 sizeStarIcon = ml::vec2(50, 50);
    Sprite sprite;
    sprite.textureName = "star";
    sprite.textureSize = ml::vec2(1, 1);
    sprite.spriteCoords = ml::vec2(0, 0);
    sprite.size = sizeStarIcon;
    UI::ACanvas::AddComponent(std::make_unique<UI::UISprite>(sprite, sizeStarIcon / 2));
    Json::Node file = Json::ParseFile(SCORE_FILE);
    int score = file[STARS_KEY];
    UI::ACanvas::AddComponent(std::make_unique<UI::UIText>(std::to_string(score), "arial", sizeStarIcon));
    ml::vec2 sizeButton = ml::vec2(100, 100);
    playButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("Play", "arial", (WindowManager::GetWindowSize() - sizeButton) / 2, sizeButton));

    ml::vec2 buttonPos = WindowManager::GetWindowSize();
    float offset = buttonPos.x * 0.25;
    buttonPos.x = offset - sizeButton.x / 2;
    buttonPos.y = buttonPos.y * 0.75;
    duckButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("duck", "arial", buttonPos, sizeButton));
    buttonPos.x += offset;
    bearButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("bear", "arial", buttonPos, sizeButton));
    buttonPos.x += offset;
    dogButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("dog", "arial", buttonPos, sizeButton));
    UI::ACanvas::EndCanvas();

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
    else if (data.componentID == bearButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF:
            modelChosen = "assets/characters/bear.glb";
            std::cout << "bear chosen" << std::endl;
            break;
        default:
            break;
        }
    }
    else if (data.componentID == dogButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF:
            modelChosen = "assets/characters/dog.glb";
            std::cout << "dog chosen" << std::endl;
            break;
        default:
            break;
        }
    }
}