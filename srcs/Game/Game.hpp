#pragma once

#include "Engine/AProgram/AProgram.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Lights/Lights.hpp"
#include <vector>

class Game: public AProgram
{
    private:
        Camera3D camera;
        std::vector<std::unique_ptr<ALight>> lights;

        void LoadAssets();

        void ProcessInput();
        void UpdateCamera();

        void Draw();

    public:
        Game();
        ~Game();

        void Run();
};