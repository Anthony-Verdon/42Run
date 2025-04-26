#include "Player/Player.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include "MapManager/MapManager.hpp"

Player::Player()
{
    direction = ml::vec3(1, 0, 0);
    modelIndex = 0;
    angle = 0;
    speed = 5;
    column = 0;
    state = PlayerStateFlag::RUNNING;
    timeElapsed = 0;
    onGround = true;
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
    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A) && column != -1 && !(state & (PlayerStateFlag::MOVING_RIGHT | PlayerStateFlag::MOVING_LEFT)))
    {
        state += PlayerStateFlag::MOVING_LEFT;
        timeElapsed = 0;
        column--;
    }

    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) && column != 1 && !(state & (PlayerStateFlag::MOVING_RIGHT | PlayerStateFlag::MOVING_LEFT)))
    {
        state += PlayerStateFlag::MOVING_RIGHT;
        timeElapsed = 0;
        column++;
    }

    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_SPACE) && !(state & PlayerStateFlag::JUMPING) && onGround)
    {
        state += PlayerStateFlag::JUMPING;
    }

}

void Player::Update()
{
    if (state & PlayerStateFlag::DEFEATED)
        return;
    // collisions
    auto contactListener = dynamic_cast<ContactListener*>(WorldPhysic3D::GetContactListener());
    auto contacts = contactListener->GetContacts(bodyId);
    for (size_t i = 0; i < contacts.size(); i++)
    {
        if (ml::dotProduct(ml::vec3(contacts[0].normal.GetX(), contacts[0].normal.GetY(), contacts[0].normal.GetZ()), ml::vec3(0, 1, 0)) > -0.5)
        {
            state = PlayerStateFlag::DEFEATED;
            ModelManager::GetModel(modelIndex).Play("Defeat", false);
            WorldPhysic3D::GetBodyInterface().DeactivateBody(bodyId);
            return;
        }
    }
    onGround = (contacts.size() != 0);

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
    timeElapsed += 1.0f / 60.0f;
    JPH::Vec3 velocity = JPH::Vec3(direction.x * speed, WorldPhysic3D::GetBodyInterface().GetLinearVelocity(bodyId).GetY(), direction.z * speed);

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
        }
    }

    if (state & PlayerStateFlag::JUMPING)
    {
        velocity += JPH::Vec3(0, 2, 0);
        state = state - PlayerStateFlag::JUMPING;
    }

    WorldPhysic3D::GetBodyInterface().SetLinearVelocity(bodyId, velocity);
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