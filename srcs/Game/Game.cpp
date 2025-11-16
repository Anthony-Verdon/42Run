#include "Game/Game.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include <iostream>
#include "Engine/2D/Renderers/TextRenderer/TextRenderer.hpp"
#include "Engine/2D/Renderers/LineRenderer2D/LineRenderer2D.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Scenes/MenuScene/MenuScene.hpp"
#if DRAW_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif
Game::Game(AProgramState *state)
{
    if (state)
    {
    }
}

void Game::Init()
{
    srand(time(NULL));

    LineRenderer2D::Init();
    TextRenderer::Init();
    TextRenderer::LoadFont("arial", "assets/fonts/arial.ttf", 48);
    LineRenderer3D::Init();

    currentScene = std::make_unique<MenuScene>();
    currentScene->Load();

#if DRAW_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(WindowManager::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
#endif
}

Game::~Game()
{
    currentScene->Quit();

    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Destroy();

    LineRenderer3D::Destroy();

#if DRAW_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
    LineRenderer2D::Destroy();
    TextRenderer::Destroy();
}

void Game::Run()
{
    auto ptr = currentScene->Run();
    if (ptr)
    {
        currentScene->Quit();
        currentScene = std::move(ptr);
        currentScene->Load();
    }
}

#if HOTRELOAD
extern "C" AProgram *create(AProgramState *state)
{
    return new Game(state);
}

extern "C" AProgramState *save()
{
    GameState *ptr = new GameState();
    return ptr;
}

extern "C" void destroy(AProgram *obj)
{
    delete obj;
}
#endif