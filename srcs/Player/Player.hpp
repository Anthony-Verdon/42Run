#pragma once

#include "Matrix/Matrix.hpp"
#include <memory>
#include "Engine/3D/Lights/Lights.hpp"
#include <vector>
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

enum PlayerStateFlag
{
    RUNNING = 1 << 0,
    MOVING_LEFT = 1 << 1,
    MOVING_RIGHT = 1 << 2,
    JUMPING = 1 << 3,
    ROLLING = 1 << 4,
    DEFEATED = 1 << 5,
};

class Player
{
    private:
        int column;
        int lastColumn;
        float angle;
        ml::vec3 direction;
        float speed;
        bool onGround;
        int state;
        
        int modelIndex;
        float timeElapsed;
        
        JPH::BodyID bodyId;
        JPH::RefConst<JPH::Shape> standingShape;
        JPH::RefConst<JPH::Shape> rollingShape;

    public:
        Player();
        ~Player();

        void Init();
        void Destroy();
        
        void ProcessInput();
        void Update();
        void Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view);

        int GetModelIndex() const { return (modelIndex); }
        void SetModelIndex(int modelIndex) { this->modelIndex = modelIndex; }
        ml::vec3 GetPosition() const;
        ml::vec3 GetDirection() const { return direction; }
        void SetDirection(const ml::vec3 &direction) { this->direction = direction; }
        float GetAngle() const { return angle; }
        void AddToAngle(float angle) { this->angle += angle; }
        bool IsDefeated() const { return (state & PlayerStateFlag::DEFEATED); }
};