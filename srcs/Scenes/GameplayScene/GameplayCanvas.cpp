#include "Scenes/GameplayScene/GameplayCanvas.hpp"
#include "Engine/UI/UISprite/UISprite.hpp"
#include "Engine/UI/UIText/UIText.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"
#include "Engine/WindowManager/WindowManager.hpp"

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

void GameplayCanvas::HandleEvents(UI::EventData &data)
{
    if (data.event == UPDATE_SCORE)
    {
        UpdateScoreEventData dataCast = dynamic_cast<UpdateScoreEventData &>(data);
        auto &component = GetComponent(scoreText);
        auto textComponent = dynamic_cast<UI::UIText *>(component.get());
        textComponent->UpdateText(std::to_string(dataCast.score));
    }
}
