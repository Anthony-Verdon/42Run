#pragma once

#include "Engine/AProgram/AProgram.hpp"
#include "Engine/AScene/AScene.hpp"
#include <memory>

class GameState : public AProgramState
{
};

class Game : public AProgram
{
  private:
    std::unique_ptr<AScene> currentScene;

  public:
    Game(AProgramState *state = nullptr);
    ~Game();

    void Init();
    void Run();
};