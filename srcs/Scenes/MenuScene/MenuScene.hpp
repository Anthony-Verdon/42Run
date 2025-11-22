#pragma once

#include "Engine/Scenes/AScene/AScene.hpp"
#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/Scenes.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Lights/Lights.hpp"

class MenuScene : public AScene
{
  private:
    MenuCanvas canvas;

    Camera3D camera;
    std::vector<std::unique_ptr<ALight>> lights;
    std::vector<std::string> paths;
    std::vector<int> charactersModelIndex;
    int characterSelect;
    float angle;
    float angleOffset;

  public:
    MenuScene();
    ~MenuScene();

    void Load();
    void Run();
    void Quit();

    int GetID() { return SceneType::MENU; }

    void UpdateCharacterSelect(int nbOffset);
    std::string GetCharacterSelectPath() { return (paths[characterSelect]); }
};