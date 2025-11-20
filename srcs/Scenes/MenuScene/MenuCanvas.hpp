#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"

class MenuCanvas : public UI::ACanvas
{
  private:
    UI::ComponentID playButton;

    void HandleEvents(UI::EventData &data);

  public:
    MenuCanvas();
    ~MenuCanvas();
};