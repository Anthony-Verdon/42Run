#include "Player/Player.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "WorldPhysic/WorldPhysic.hpp"

Player::Player()
{
    direction = ml::vec3(1, 0, 0);
    modelIndex = 0;
    angle = 0;
    speed = 5;
    column = 0;
    state = PlayerState::RUNNING;
    timeElapsed = 0;
}

void Player::Init()
{
    JPH::BodyCreationSettings capsuleSetting(new JPH::CapsuleShape(0.5, 0.5), JPH::RVec3(6, 2, 6), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    capsuleSetting.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ | JPH::EAllowedDOFs::RotationY;
    capsuleSetting.mFriction = 0;
    bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(capsuleSetting, JPH::EActivation::Activate);
}

void Player::Destroy()
{
    WorldPhysic3D::GetBodyInterface().RemoveBody(bodyId);
    WorldPhysic3D::GetBodyInterface().DestroyBody(bodyId);
}

Player::~Player()
{
}

void Player::ProcessInput()
{
    timeElapsed += 1.0f / 60.0f;
    direction = ml::normalize(ml::vec3(-sinf(angle), 0, cosf(angle)));

    switch (state)
    {
        case PlayerState::RUNNING:
        {
            JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);
            WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);

            if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) && column != 1)
            {
                state = PlayerState::MOVING_RIGHT;
                timeElapsed = 0;
                column++;
            }
            else if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A) && column != -1)
            {
                state = PlayerState::MOVING_LEFT;
                timeElapsed = 0;
                column--;
            }
            break;
        }
        case PlayerState::MOVING_LEFT: //todo: factorize with moving right
        {
            float time = 2;
            JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);
            JPH::Vec3 horizontalMovement;
            if (direction.x != 0)
                horizontalMovement = JPH::Vec3(0, 0, direction.x * 2 * speed / time);
            else
                horizontalMovement = JPH::Vec3(direction.z * 2 * speed / time, 0, 0);
            
            velocity += horizontalMovement;
            WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
            if (timeElapsed >= time / speed)
            {
                state = PlayerState::RUNNING;
                timeElapsed = 0;
            }
            break;
        }
        case PlayerState::MOVING_RIGHT:
        {
            float time = 2;
            JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);
            JPH::Vec3 horizontalMovement;
            if (direction.x != 0)
                horizontalMovement = JPH::Vec3(0, 0, -direction.x * 2 * speed / time);
            else
                horizontalMovement = JPH::Vec3(-direction.z * 2 * speed / time, 0, 0);
            
            velocity += horizontalMovement;
            WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
            if (timeElapsed >= time / speed)
            {
                state = PlayerState::RUNNING;
                timeElapsed = 0;
            }
            break;
        }
    }
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform;
    if (ModelManager::GetModel(modelIndex).GetCurrentAnimation() == "Roll")
        transform = ml::translate(ml::mat4(1.0f), GetPosition() - ml::vec3(0, 0, 2.0f)) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    else
        transform = ml::translate(ml::mat4(1.0f), GetPosition()) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, transform);
}

ml::vec3 Player::GetPosition() const
{
    JPH::RVec3 position = WorldPhysic3D::GetBodyInterface().GetPosition(bodyId);
    return (ml::vec3(position.GetX(), position.GetY() - 1, position.GetZ()));
}