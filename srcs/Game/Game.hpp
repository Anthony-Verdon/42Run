#pragma once

#include "Engine/AProgram/AProgram.hpp"
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/Lights/Lights.hpp"
#include "Player/Player.hpp"
#include <vector>
#include "WorldPhysic/WorldPhysic.hpp"

class Game : public AProgram
{
  private:
    BPLayerInterfaceImpl BPLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl ObjectVsBPLayerFilter;
    ObjectLayerPairFilterImpl OBjectLPFilter;

    Camera3D camera;
    std::vector<std::unique_ptr<ALight>> lights;
    Player player;

    float accumulatedTime;

    void ProcessInput();
    void UpdateCamera();

    void Draw();

  public:
    Game();
    ~Game();

    void Run();
};