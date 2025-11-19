#include "Engine/WindowManager/WindowManager.hpp"

#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Engine/Time/Time.hpp"
#include "MapManager/MapManager.hpp"
#include "Player/Player.hpp"
#include "Engine/3D/WorldPhysic3D/ContactListener/ContactListener.hpp"
#include "Game/Layers.hpp"
#include "PhysicBodyType.hpp"

#if DRAW_IMGUI
#include "imgui.h"
#include <magic_enum_flags.hpp>
#endif

#define PLAYER_CAPSULE_RADIUS 0.5
#define PLAYER_CAPSULE_HEIGHT_STANDING 0.5
#define PLAYER_CAPSULE_HEIGHT_ROLLING 0.25
#define PLAYER_CAPSULE_HEIGHT_DIFF (PLAYER_CAPSULE_HEIGHT_STANDING - PLAYER_CAPSULE_HEIGHT_ROLLING)

Player::Player()
{
    column = 0;
    lastColumn = 0;
    angle = 0;
    direction = ml::vec3(1, 0, 0);
    speed = 5;
    onGround = true;
    state = PlayerStateFlag::RUNNING;
    timeElapsed = 0;
    physicBodyType = PhysicBodyType::PLAYER;
}

void Player::Init()
{
    ModelManager::AddModels(ModelLoader::LoadModel("assets/duck.glb"));
    modelIndex = ModelManager::GetNbModel() - 1;
    ModelManager::GetModel(modelIndex).Init();
    ModelManager::GetModel(modelIndex).Play("Run");

    standingShape = new JPH::CapsuleShape(PLAYER_CAPSULE_HEIGHT_STANDING, PLAYER_CAPSULE_RADIUS);
    rollingShape = new JPH::CapsuleShape(PLAYER_CAPSULE_HEIGHT_ROLLING, PLAYER_CAPSULE_RADIUS);
    JPH::BodyCreationSettings capsuleSetting(standingShape, JPH::RVec3(6, 2, 6), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, ObjectLayers::MOVING);
    capsuleSetting.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ | JPH::EAllowedDOFs::RotationY;
    capsuleSetting.mFriction = 0;
    WorldPhysic3D::AddBody(this, capsuleSetting, JPH::EActivation::Activate);
}

void Player::Destroy()
{
    WorldPhysic3D::RemoveBody(GetID());
}

Player::~Player()
{
}

void Player::ProcessInput()
{
    if (state & PlayerStateFlag::DEFEATED)
        return;

    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A) && column != -1 && !(state & (PlayerStateFlag::MOVING_RIGHT | PlayerStateFlag::MOVING_LEFT)))
    {
        state |= PlayerStateFlag::MOVING_LEFT;
        timeElapsed = 0;
        lastColumn = column;
        column--;
    }

    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) && column != 1 && !(state & (PlayerStateFlag::MOVING_RIGHT | PlayerStateFlag::MOVING_LEFT)))
    {
        state |= PlayerStateFlag::MOVING_RIGHT;
        timeElapsed = 0;
        lastColumn = column;
        column++;
    }

    if (WindowManager::IsInputPressed(GLFW_KEY_SPACE) && !(state & PlayerStateFlag::JUMPING) && onGround)
    {
        if (state & PlayerStateFlag::ROLLING)
        {
            state &= ~PlayerStateFlag::ROLLING;
            ModelManager::GetModel(modelIndex).Play("Run");
        }
        state |= PlayerStateFlag::JUMPING;
    }

    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT_SHIFT) && !(state & (PlayerStateFlag::JUMPING | PlayerStateFlag::ROLLING)) && onGround)
    {
        state |= PlayerStateFlag::ROLLING;
        ModelManager::GetModel(modelIndex).Play("Roll");
        WorldPhysic3D::SetShape(GetID(), rollingShape, true, JPH::EActivation::Activate);
        WorldPhysic3D::SetPosition(GetID(), WorldPhysic3D::GetPosition(GetID()) - JPH::Vec3::sAxisY() * PLAYER_CAPSULE_HEIGHT_DIFF, JPH::EActivation::Activate);
    }
}

void Player::Update()
{
    if (state & PlayerStateFlag::DEFEATED)
        return;

    // direction
    if (MapManager::GetCurrentChunkType() == ChunkType::TURN && column != 0)
    {
        bool updateAngle = false;
        if (direction.x != 0)
        {
            int posX = GetPosition().x / 2;
            if (direction.x == -1)
                posX++;
            updateAngle = (abs(posX) % ChunkGenerator::GetChunkSize() == ChunkGenerator::GetHalfChunkSize());
        }
        else if (direction.z != 0)
        {
            int posZ = GetPosition().z / 2;
            if (direction.z == -1)
                posZ++;
            updateAngle = (abs(posZ) % ChunkGenerator::GetChunkSize() == ChunkGenerator::GetHalfChunkSize());
        }

        if (updateAngle)
        {
            angle += ml::radians(90) * column;
            column = 0;
        }
    }

    direction = ml::normalize(ml::vec3(-sinf(angle), 0, cosf(angle)));
    direction.x = round(direction.x);
    direction.z = round(direction.z);

    // velocity
    timeElapsed += WorldPhysic3D::GetDeltaTime();
    JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetLinearVelocity(GetID()).GetY(), direction.z * speed);

    if (state & PlayerStateFlag::ROLLING)
    {
        velocity.SetX(velocity.GetX() * 2);
        velocity.SetZ(velocity.GetZ() * 2);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
        {
            state &= ~PlayerStateFlag::ROLLING;
            ModelManager::GetModel(modelIndex).Play("Run");
            WorldPhysic3D::SetShape(GetID(), standingShape, true, JPH::EActivation::Activate);
            WorldPhysic3D::SetPosition(GetID(), WorldPhysic3D::GetPosition(GetID()) + JPH::Vec3::sAxisY() * PLAYER_CAPSULE_HEIGHT_DIFF, JPH::EActivation::Activate);
        }
    }

    if (state & (PlayerStateFlag::MOVING_RIGHT | PlayerStateFlag::MOVING_LEFT))
    {
        int sign = (state & PlayerStateFlag::MOVING_RIGHT) ? -1 : 1;
        float time = 2;
        JPH::Vec3 horizontalMovement;
        if (direction.x != 0)
            horizontalMovement = JPH::Vec3(0, 0, sign * -direction.x * 2 * speed / time);
        else
            horizontalMovement = JPH::Vec3(sign * direction.z * 2 * speed / time, 0, 0);

        velocity += horizontalMovement;
        if (timeElapsed >= time / speed)
        {
            (state & PlayerStateFlag::MOVING_RIGHT) ? state = state - PlayerStateFlag::MOVING_RIGHT : state = state - PlayerStateFlag::MOVING_LEFT;
            timeElapsed = 0;
            lastColumn = column;
        }
    }

    if (state & PlayerStateFlag::JUMPING)
    {
        velocity.SetY(6);
        state &= ~PlayerStateFlag::JUMPING;
    }

    WorldPhysic3D::SetLinearVelocity(GetID(), velocity);
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform = ml::translate(ml::mat4(1.0f), GetPosition()) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    if (state & PlayerStateFlag::DEFEATED)
        transform = transform * ml::rotate(ml::mat4(1.0f), 30 * (column - lastColumn), ml::vec3(0, 1, 0));
    bool enableRootMotion = !(state & PlayerStateFlag::ROLLING);
    ModelManager::Draw(modelIndex, camPos, lights, projection, view, transform, enableRootMotion);

#if DRAW_IMGUI
    ImGui::Begin("Player");
    ImGui::Text("State: %s", magic_enum::enum_flags_name(static_cast<PlayerStateFlag>(state)).c_str());
    ImGui::Text("On ground: %s", (onGround ? "true" : "false"));
    ImGui::End();
#endif
}

ml::vec3 Player::GetPosition() const
{
    JPH::RVec3 position = WorldPhysic3D::GetPosition(GetID());
    return (ml::vec3(position.GetX(), position.GetY() - 1, position.GetZ()));
}

void Player::OnWorldPhysicUpdated()
{
    onGround = false;
}

void Player::OnContactAdded([[maybe_unused]] const JPH::ContactManifold &inManifold, [[maybe_unused]] const PhysicBody3D *collisionedBody)
{
    bool playerLost = false;

    if (collisionedBody->GetPhysicBodyType() == PhysicBodyType::OBSTACLE)
        playerLost = true;
    else if (collisionedBody->GetPhysicBodyType() == PhysicBodyType::TILE)
    {
        if (ml::dotProduct(ml::vec3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()), ml::vec3(0, 1, 0)) <= 0.5)
            playerLost = true;
        else
            onGround = true;
    }

    if (playerLost)
    {
        state |= PlayerStateFlag::DEFEATED;
        ModelManager::GetModel(modelIndex).Play("Defeat", false);
        std::unique_ptr<DeactivateBodyAction> action = std::make_unique<DeactivateBodyAction>();
        action->id = GetID();
        WorldPhysic3D::SaveActionDelayed(std::move(action));
    }
}

void Player::OnContactPersisted([[maybe_unused]] const JPH::ContactManifold &inManifold, [[maybe_unused]] const PhysicBody3D *collisionedBody)
{
    onGround = true;
}