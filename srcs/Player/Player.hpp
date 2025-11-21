#pragma once

#include "Engine/3D/Lights/Lights.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Matrix/Matrix.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <memory>
#include <vector>
#include "Engine/3D/WorldPhysic3D/PhysicBody3D/PhysicBody3D.hpp"

enum PlayerStateFlag
{
    RUNNING = 1 << 0,
    MOVING_LEFT = 1 << 1,
    MOVING_RIGHT = 1 << 2,
    JUMPING = 1 << 3,
    ROLLING = 1 << 4,
    DEFEATED = 1 << 5,
};

#if DRAW_IMGUI
#include <magic_enum.hpp>
template <>
struct magic_enum::customize::enum_range<PlayerStateFlag>
{
    static constexpr bool is_flags = true;
};
#endif

class Player : public PhysicBody3D
{
  private:
    int column;
    int lastColumn;
    float angle;
    ml::vec3 direction;
    float speed;
    bool onGround;
    int state;
    int nbCollectible;

    int modelIndex;
    float timeElapsed;

    JPH::RefConst<JPH::Shape> standingShape;
    JPH::RefConst<JPH::Shape> rollingShape;

    void OnWorldPhysicUpdated();
    void OnContactAdded(const JPH::ContactManifold &inManifold, const PhysicBody3D *collisionedBody);
    void OnContactPersisted(const JPH::ContactManifold &inManifold, const PhysicBody3D *collisionedBody);

  public:
    Player();
    ~Player();

    void Init(const std::string &modelPath);
    void Destroy();

    void ProcessInput();
    void Update();
    void Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view);

    int GetModelIndex() const
    {
        return (modelIndex);
    }
    void SetModelIndex(int modelIndex)
    {
        this->modelIndex = modelIndex;
    }
    ml::vec3 GetPosition() const;
    ml::vec3 GetDirection() const
    {
        return direction;
    }
    void SetDirection(const ml::vec3 &direction)
    {
        this->direction = direction;
    }
    float GetAngle() const
    {
        return angle;
    }
    void AddToAngle(float angle)
    {
        this->angle += angle;
    }
    bool IsDefeated() const
    {
        return (state & PlayerStateFlag::DEFEATED);
    }
};