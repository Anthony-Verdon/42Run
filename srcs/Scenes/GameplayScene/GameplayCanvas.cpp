#include "Scenes/GameplayScene/GameplayCanvas.hpp"
#include "Engine/UI/UISprite/UISprite.hpp"
#include "Engine/UI/UIText/UIText.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"
#include "Scenes/MenuScene/MenuScene.hpp"

GameplayCanvas::GameplayCanvas()
{
    UI::ACanvas::BeginCanvas(this);
    ml::vec2 size = ml::vec2(50, 50);
    Sprite sprite;
    sprite.textureName = "star";
    sprite.textureSize = ml::vec2(1, 1);
    sprite.spriteCoords = ml::vec2(0, 0);
    sprite.size = size;
    UI::ACanvas::AddComponent(std::make_unique<UI::UISprite>(sprite, size / 2));
    score = "0";
    scoreText = UI::ACanvas::AddComponent(std::make_unique<UI::UIText>(score, "arial", ml::vec2(size.x, WindowManager::GetWindowHeight() - size.y)));
    UI::ACanvas::EndCanvas();
}

GameplayCanvas::~GameplayCanvas()
{
}

void GameplayCanvas::GameFinished()
{
    UI::ACanvas::BeginCanvas(this);
    ml::vec2 size = ml::vec2(100, 100);
    menuButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("return to menu", "arial", (WindowManager::GetWindowSize() - size) / 2, size));
    UI::ACanvas::EndCanvas();
}

void GameplayCanvas::HandleEvents(UI::EventData &data)
{
    switch (data.event)
    {
    case UPDATE_SCORE: {
        UpdateScoreEventData dataCast = dynamic_cast<UpdateScoreEventData &>(data);
        auto &component = GetComponent(scoreText);
        auto textComponent = dynamic_cast<UI::UIText *>(component.get());
        textComponent->UpdateText(std::to_string(dataCast.score));
        break;
    }
    case UI::CLICK_OFF: {
        SceneManager::SwitchScene(std::make_unique<MenuScene>());

        break;
    }
    }
}
