#include "Game/Game.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include <iostream>
#include "Engine/2D/Renderers/TextRenderer/TextRenderer.hpp"
#include "Engine/2D/Renderers/LineRenderer2D/LineRenderer2D.hpp"
#include "Engine/2D/Renderers/SpriteRenderer/SpriteRenderer.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"
#include "Scenes/MenuScene/MenuScene.hpp"
#include "Json/Json.hpp"
#include "SaveDefines.hpp"

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
    TextRenderer::LoadFont("arial", "assets/UI/fonts/arial.ttf", 48);
    LineRenderer3D::Init();
    SpriteRenderer::Init();
    RessourceManager::AddTexture("star", "assets/UI/star.png");
    RessourceManager::AddTexture("rectangle_button_flat", "assets/UI/buttons/button_rectangle_flat.png");
    RessourceManager::AddTexture("rectangle_button_depth_flat", "assets/UI/buttons//button_rectangle_depth_flat.png");

    if (!std::filesystem::exists(SCORE_FILE))
    {
        Json::Node file;
        file[NB_STARS] = 0;
        file[BEAR_UNLOCK] = false;
        file[DOG_UNLOCK] = false;
        if (!std::filesystem::exists(SAVE_DIRECTORY))
            std::filesystem::create_directory(SAVE_DIRECTORY);

        Json::WriteFile(SCORE_FILE, file);
    }

    SceneManager::LoadScene(std::make_unique<MenuScene>());

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
    SceneManager::QuitScene();

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
    SpriteRenderer::Destroy();
}

void Game::Run()
{
    SceneManager::UpdateCurrentScene();
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