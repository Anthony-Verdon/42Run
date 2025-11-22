#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"

#define BEAR_PRIZE 25
#define DOG_PRIZE 50

class MenuCanvas : public UI::ACanvas
{
  private:
    UI::ComponentID playButton;
    UI::ComponentID leftArrow, rightArrow;
    Sprite playButtonSprite;

    void HandleEvents(UI::EventData &data);

  public:
    MenuCanvas();
    ~MenuCanvas();

    void Init();
};