#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"
#include "Engine/2D/Sprite/Sprite.hpp"

#define BEAR_PRIZE 25
#define DOG_PRIZE 50

class MenuCanvas : public UI::ACanvas
{
  private:
    UI::ComponentID nbStarText;

    UI::ComponentID playButton;
    Sprite playButtonSprite;
    void PlayButtonCallback(const UI::CallbackData &data);

    UI::ComponentID unlockButton;
    void UnlockButtonCallback(const UI::CallbackData &data);

    UI::ComponentID leftArrow, rightArrow;
    void ArrowButtonCallback(const UI::CallbackData &data, bool leftArrow);

    UI::ComponentID soundSlider;
    UI::ComponentID soundSliderText;
    void SoundSliderCallback(const UI::CallbackData &data);

    UI::ComponentID settingsButton;
    UI::ComponentID settingsBackground;
    bool settingsOpen;
    void SettingsButtonCallback(const UI::CallbackData &data);

  public:
    MenuCanvas();
    ~MenuCanvas();

    void Init();
};