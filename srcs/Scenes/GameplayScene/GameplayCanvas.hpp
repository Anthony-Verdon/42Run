#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"

class GameplayCanvas : public UI::ACanvas
{
  private:
    std::string score;
    UI::ComponentID scoreText;
    Sprite menuButtonSprite;
    UI::ComponentID menuButton;

    void menuButtonCallback(const UI::CallbackData &data);

  public:
    GameplayCanvas();
    ~GameplayCanvas();

    void UpdateScore(int score);
    void GameFinished();
};