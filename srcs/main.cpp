#include "Game/Game.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "globals.hpp"
#include <cstdlib>

int main()
{
    WindowManager::InitWindow("new game", WINDOW_WIDTH, WINDOW_HEIGHT);
    Game game;
    WindowManager::StartUpdateLoop(&game);
    WindowManager::DestructWindowManager();

    return (EXIT_SUCCESS);
}