#pragma once

#include "Engine/Scenes/AScene/AScene.hpp"
#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/Scenes.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Lights/Lights.hpp"

enum CharacterSkin
{
    DUCK = 0,
    BEAR,
    DOG
};

struct Character
{
    std::string path;
    int modelIndex;
    bool unlock;
    int prize;
    CharacterSkin skin;
};

class MenuScene : public AScene
{
  private:
    MenuCanvas canvas;

    Camera3D camera;
    std::vector<std::unique_ptr<ALight>> lights;
    std::vector<Character> characters;
    int characterSelect;
    float angle;
    float angleOffset;
    int nbStars;

#if DRAW_IMGUI
    float soundValue;
#endif

  public:
    MenuScene();
    ~MenuScene();

    void Load();
    void Run();
    void Quit();

    int GetID() { return SceneType::MENU; }

    void UpdateCharacterSelect(int nbOffset);
    Character &GetCharacterSelect() { return (characters[characterSelect]); }
    int &GetNbStars() { return (nbStars); }
};