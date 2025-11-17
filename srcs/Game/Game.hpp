#pragma once

#include "Engine/AProgram/AProgram.hpp"
#include <memory>

class GameState : public AProgramState
{
};

class Game : public AProgram
{
  public:
    Game(AProgramState *state = nullptr);
    ~Game();

    void Init();
    void Run();
};