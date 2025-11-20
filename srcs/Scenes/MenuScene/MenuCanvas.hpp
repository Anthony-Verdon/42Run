#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"

class MenuCanvas : public UI::ACanvas
{
  private:
    unsigned int playButton;

    void HandleEvents(UI::EventData &data);

  public:
    MenuCanvas();
    ~MenuCanvas();
};