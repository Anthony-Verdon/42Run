#pragma once

#include "Engine/AScene/AScene.hpp"
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
    std::unique_ptr<AScene> Run();
    void Quit();

    int GetID() { return SceneType::MENU; }
};