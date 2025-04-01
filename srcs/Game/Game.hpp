#pragma once

#include "Engine/AProgram/AProgram.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Light/Light.hpp"

class Game: public AProgram
{
    private:
        Camera3D camera;
        Light lights[4];

        void LoadAssets();

        void ProcessInput();
        void UpdateCamera();
        
    public:
        Game();
        ~Game();

        void Run();
};