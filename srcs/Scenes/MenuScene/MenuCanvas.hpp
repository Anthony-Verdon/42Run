#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"

#define BEAR_PRIZE 25
#define DOG_PRIZE 50

class MenuCanvas : public UI::ACanvas
{
  private:
    UI::ComponentID playButton;
    UI::ComponentID duckButton;
    UI::ComponentID bearButton;
    UI::ComponentID dogButton;
    std::string modelChosen;

    void HandleEvents(UI::EventData &data);

  public:
    MenuCanvas();
    ~MenuCanvas();
};