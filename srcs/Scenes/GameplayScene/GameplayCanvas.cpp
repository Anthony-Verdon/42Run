#include "Scenes/GameplayScene/GameplayCanvas.hpp"
#include "Engine/UI/UISprite/UISprite.hpp"
#include "Engine/UI/UIText/UIText.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"
#include "Scenes/MenuScene/MenuScene.hpp"
#include "Engine/AudioManager/AudioManager.hpp"
#include "Engine/UI/Callbacks.hpp"

GameplayCanvas::GameplayCanvas()
{
    ml::vec2 size = ml::vec2(50, 50);
    Sprite sprite;
    sprite.textureName = "star";
    sprite.textureSize = ml::vec2(1, 1);
    sprite.spriteCoords = ml::vec2(0, 0);
    sprite.size = size;
    AddComponent(std::make_unique<UI::UISprite>(sprite, size / 2));
    score = "0";
    scoreText = AddComponent(std::make_unique<UI::UIText>(score, "arial", size, TextRenderer::TextAlign::LEFT));
}

GameplayCanvas::~GameplayCanvas()
{
}

void GameplayCanvas::GameFinished()
{
    menuButtonSprite = {"rectangle_button_depth_flat", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(300, 100)};
    menuButton = AddComponent(std::make_unique<UI::SpriteButton>(menuButtonSprite, "Menu", "arial", WindowManager::GetWindowSize() / 2, [this](const UI::CallbackData &data) { menuButtonCallback(data); }));
}

void GameplayCanvas::UpdateScore(int score)
{
    auto textComponent = dynamic_cast<UI::UIText *>(GetComponent(scoreText).get());
    textComponent->UpdateText(std::to_string(score));
}

void GameplayCanvas::menuButtonCallback(const UI::CallbackData &data)
{
    switch (data.callback)
    {

    case UI::EngineCallbacks::CURSEUR_ON: {
        auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(menuButton).get());
        menuButtonSprite.textureName = "rectangle_button_flat";
        button->UpdateSprite(menuButtonSprite);
        break;
    }
    case UI::EngineCallbacks::CURSEUR_OFF: {
        auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(menuButton).get());
        menuButtonSprite.textureName = "rectangle_button_depth_flat";
        button->UpdateSprite(menuButtonSprite);
        break;
    }
    case UI::EngineCallbacks::CLICK_OFF: {
        SceneManager::SwitchScene(std::make_unique<MenuScene>());
        AudioManager::Play("assets/sounds/click-b.mp3");
        break;
    }
    }
}
