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
    UI::ACanvas::AddComponent(std::make_unique<UI::UIText>(std::to_string(score), "arial", ml::vec2(sizeStarIcon.x, WindowManager::GetWindowHeight() - sizeStarIcon.y)));
    ml::vec2 sizeButton = ml::vec2(100, 100);
    playButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("Play", "arial", (WindowManager::GetWindowSize() - sizeButton) / 2, sizeButton));
    UI::ACanvas::EndCanvas();
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
        case UI::EngineEvents::CURSEUR_ON:
            std::cout << "curseur on" << std::endl;
            break;
        case UI::EngineEvents::CURSEUR_OFF:
            std::cout << "curseur off" << std::endl;
            break;
        case UI::EngineEvents::CLICK_ON:
            std::cout << "click on" << std::endl;
            break;
        case UI::EngineEvents::CLICK_OFF:
            std::cout << "click off" << std::endl;
            SceneManager::SwitchScene(std::make_unique<GameplayScene>());
            break;
        default:
            break;
        }
    }
}