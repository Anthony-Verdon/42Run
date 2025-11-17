#include "Scenes/MenuScene/MenuScene.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/WindowManager/WindowManager.hpp"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Load()
{
}

void MenuScene::Run()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    canvas.Update();
}

void MenuScene::Quit()
{
}