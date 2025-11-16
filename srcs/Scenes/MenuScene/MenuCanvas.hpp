#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"

class MenuCanvas : public UI::ACanvas
{
  private:
    unsigned int playButton;

    void HandleEvents(unsigned int componentID, unsigned int event);

  public:
    MenuCanvas();
    ~MenuCanvas();
};