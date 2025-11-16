#include "Scenes/MenuScene/MenuScene.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include <iostream>

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Load()
{
    std::cout << "press space to switch scene" << std::endl;
}

std::unique_ptr<AScene> MenuScene::Run()
{
    canvas.Update();
    canvas.Draw();
    if (WindowManager::IsInputPressed(GLFW_KEY_SPACE))
        return std::make_unique<GameplayScene>();

    return (NULL);
}

void MenuScene::Quit()
{
}