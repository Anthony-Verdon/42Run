#pragma once

#include "Scenes/Scenes.hpp"
#include "Engine/Scenes/AScene/AScene.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Lights/Lights.hpp"
#include "Player/Player.hpp"
#include "Scenes/GameplayScene/GameplayCanvas.hpp"

class GameplayScene : public AScene
{
  private:
    Camera3D camera;
    std::vector<std::unique_ptr<ALight>> lights;
    Player player;
    GameplayCanvas canvas;

    float accumulatedTime;

    void ProcessInput();
    void UpdateCamera();

    void Draw();

  public:
    GameplayScene();
    ~GameplayScene();

    void Load();
    void Run();
    void Quit();

    int GetID() { return SceneType::GAMEPLAY; }
    GameplayCanvas &GetCanvas() { return canvas; }
};