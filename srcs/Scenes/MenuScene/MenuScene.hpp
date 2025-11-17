#pragma once

#include "Engine/Scenes/AScene/AScene.hpp"
#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/Scenes.hpp"

class MenuScene : public AScene
{
  private:
    MenuCanvas canvas;

  public:
    MenuScene();
    ~MenuScene();

    void Load();
    void Run();
    void Quit();

    int GetID() { return SceneType::MENU; }
};