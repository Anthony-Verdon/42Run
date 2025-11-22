#pragma once

#include "Engine/UI/Button/Button.hpp"

class UnlockButton : public UI::Button
{
  private:
    Sprite background, star;

  public:
    UnlockButton() = delete;
    UnlockButton(const std::string &text, const std::string &font, const ml::vec2 &pos, const ml::vec2 &size);
    ~UnlockButton();

    void Draw();
    void CurseurOn();
    void CurseurOff();
};