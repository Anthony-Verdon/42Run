#include "Engine/WindowManager/WindowManager.hpp"
#include "Game/Game.hpp"
#include <cstdlib>

int main()
{
    WindowManager::InitWindow("new game", WINDOW_WIDTH, WINDOW_HEIGHT);
    Game game;
    game.Init();
    WindowManager::StartUpdateLoop(&game);
    WindowManager::DestructWindowManager();

    return (EXIT_SUCCESS);
}