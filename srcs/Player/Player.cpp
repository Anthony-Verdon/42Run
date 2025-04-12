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
}

void Player::Init()
{
    JPH::BodyCreationSettings capsuleSetting(new JPH::CapsuleShape(0.5, 0.5), JPH::RVec3(0, 2, 0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    capsuleSetting.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ | JPH::EAllowedDOFs::RotationY;
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

#define AUTOMATIC_MOVEMENT

void Player::ProcessInput()
{
    direction = ml::normalize(ml::vec3(-sinf(angle), 0, cosf(angle)));
    
    std::string currentAnim = ModelManager::GetModel(modelIndex).GetCurrentAnimation();
    if (currentAnim == "Run")
    {
        #ifdef AUTOMATIC_MOVEMENT
        JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);
        #else
        JPH::Vec3 velocity = JPH::Vec3(0, 0, 0);
        if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_W))
            velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);
        #endif
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);

        if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) && column != 1)
        {
            #ifdef AUTOMATIC_MOVEMENT
            ModelManager::GetModel(modelIndex).Play("DashRight");
            column++;
            #else
            angle += Time::getDeltaTime();
            #endif
        }
        else if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A) && column != -1)
        {
            #ifdef AUTOMATIC_MOVEMENT
            ModelManager::GetModel(modelIndex).Play("DashLeft");
            column--;
            #else
            angle -= Time::getDeltaTime();
            #endif
        }
        else if (WindowManager::IsInputPressed(GLFW_KEY_SPACE))
            ModelManager::GetModel(modelIndex).Play("Jump");
        else if (WindowManager::IsInputPressed(GLFW_KEY_LEFT_SHIFT))
            ModelManager::GetModel(modelIndex).Play("Roll");
    }
    else if (currentAnim == "DashRight")
    {
        JPH::Vec3 velocity = (JPH::Vec3(direction.x, direction.y, direction.z) * speed + JPH::Vec3(-1, 0, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration();
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "DashLeft")
    {
        JPH::Vec3 velocity = (JPH::Vec3(direction.x, direction.y, direction.z) * speed + JPH::Vec3(1, 0, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration();
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "Jump")
    {
        JPH::Vec3 velocity = (JPH::Vec3(direction.x, direction.y, direction.z) * speed + JPH::Vec3(0, 1, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration();
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Fall", 2.0f);
    }
    else if (currentAnim == "Fall")
    {
        JPH::Vec3 velocity = (JPH::Vec3(direction.x, direction.y, direction.z) * speed + JPH::Vec3(0, -1, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration();
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "Roll")
    {
        JPH::Vec3 velocity = (JPH::Vec3(direction.x, direction.y, direction.z) * speed + JPH::Vec3(0, 0, 1) * 4.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration();
        WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
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