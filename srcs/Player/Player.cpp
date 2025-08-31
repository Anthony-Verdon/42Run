#include "Engine/WindowManager/WindowManager.hpp"

#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Engine/Time/Time.hpp"
#include "MapManager/MapManager.hpp"
#include "Player/Player.hpp"
#include "Engine/3D/WorldPhysic3D/ContactListener/ContactListener.hpp"
#include "Game/Layers.hpp"
#if DRAW_IMGUI
#include "imgui.h"
#include <magic_enum_flags.hpp>
#endif
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/Collision/Shape/EmptyShape.h>

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

    ModelManager::AddModels(ModelLoader::LoadModel("assets/duck.glb"));
    modelIndex = ModelManager::GetNbModel() - 1;
    ModelManager::GetModel(modelIndex).Init();
    ModelManager::GetModel(modelIndex).Play("Run");
    timeElapsed = 0;
}

void Player::Init()
{
    /*
    standingShape = new JPH::CapsuleShape(PLAYER_CAPSULE_HEIGHT_STANDING, PLAYER_CAPSULE_RADIUS);
    rollingShape = new JPH::CapsuleShape(PLAYER_CAPSULE_HEIGHT_ROLLING, PLAYER_CAPSULE_RADIUS);
    JPH::BodyCreationSettings capsuleSetting(standingShape, JPH::RVec3(6, 2, 6), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, ObjectLayers::MOVING);
    capsuleSetting.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ | JPH::EAllowedDOFs::RotationY;
    capsuleSetting.mFriction = 0;
    WorldPhysic3D::AddRigidBody(this, capsuleSetting, JPH::EActivation::Activate);
    */

    /*
    Body -1
Head 0
character_duckHead 1
build shape 0.0166229 0.577487
armLeft 0
handSlotLeft 3
character_duckArmLeft 3
build shape 0.0953015 0.190118
armRight 0
handSlotRight 6
character_duckArmRight 6
build shape 0.0953014 0.190118
character_duck 0
build shape -0.0226334 0.385414
    */
    // Model &model = ModelManager::GetModel(modelIndex);
    JPH::Ref<JPH::Skeleton> skeleton = new JPH::Skeleton;
    uint body = skeleton->AddJoint("Body");
    uint head = skeleton->AddJoint("Head", body);
    /*uint character_duckHead = */ skeleton->AddJoint("character_duckHead", head);
    uint armLeft = skeleton->AddJoint("armLeft", body);
    /*uint handSlotLeft = */ skeleton->AddJoint("handSlotLeft", armLeft);
    /*uint character_duckArmLeft = */ skeleton->AddJoint("character_duckArmLeft", armLeft);
    uint armRight = skeleton->AddJoint("armRight", body);
    /*uint handSlotRight = */ skeleton->AddJoint("handSlotRight", armRight);
    /*uint character_duckArmRight = */ skeleton->AddJoint("character_duckArmRight", armRight);
    /*uint character_duck = */ skeleton->AddJoint("character_duck", body);

    std::vector<JPH::Ref<JPH::Shape>> shapes = {
        new JPH::EmptyShape(),
        new JPH::EmptyShape(),
        new JPH::CapsuleShape(0.0166229, 0.577487), // head
        new JPH::EmptyShape(),
        new JPH::EmptyShape(),
        new JPH::CapsuleShape(0.0953015, 0.190118), // armleft
        new JPH::EmptyShape(),
        new JPH::EmptyShape(),
        new JPH::CapsuleShape(0.0953015, 0.190118), // armright
        new JPH::CapsuleShape(-0.0226334, 0.385414) // body
    };
    std::vector<JPH::RVec3> positions = {
        JPH::RVec3(0, 0, 0),
        JPH::RVec3(2.985e-15, 0.711598, 1.45584e-22),
        JPH::RVec3(2.985e-15, 0.711598, 1.45584e-22) + JPH::RVec3(-8.24463e-16, -0.00788677, 2.42144e-08),
        JPH::RVec3(0.14, 0.691598, 1.98491e-09),
        JPH::RVec3(0.14, 0.691598, 1.98491e-09) + JPH::RVec3(5.32907e-14, 0.550922, -7.45062e-09),
        JPH::RVec3(0.14, 0.691598, 1.98491e-09) + JPH::RVec3(0.00104323, 0.0819037, -0.0279617),
        JPH::RVec3(-0.14, 0.691598, -1.98491e-09),
        JPH::RVec3(-0.14, 0.691598, -1.98491e-09) + JPH::RVec3(-7.4607e-14, 0.550922, 3.72528e-08),
        JPH::RVec3(-0.14, 0.691598, -1.98491e-09) + JPH::RVec3(-0.0341072, 0.0791443, 0.0192915),
        JPH::RVec3(0, 0, 0),
    };
    /*
    std::vector<JPH::Quat> rotations = {
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(-1.54703e-15, 1.41783e-23, -6.7853e-21, 1),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(-1.54703e-15, 1.41783e-23, -6.7853e-21, 1) * JPH::Quat(-6.71794e-08, -1.71143e-08, -1.49477e-22, 1),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023) * JPH::Quat(0.606617, -0.606617, 0.36334, 0.36334),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023) * JPH::Quat(-0.696652, -0.167293, 0.672156, 0.186804),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023) * JPH::Quat(0.36334, -0.36334, 0.606617, 0.606617),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023) * JPH::Quat(0.698441, -0.179085, -0.660011, 0.210936),
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1) * JPH::Quat(6.71794e-08, -1.71143e-08, -1.85288e-22, 1),
    };
    */
    std::vector<JPH::Quat> rotations = {
        JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(-1.54703e-15, 1.41783e-23, -6.7853e-21, 1) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(-6.71794e-08, -1.71143e-08, -1.49477e-22, 1) * JPH::Quat(-1.54703e-15, 1.41783e-23, -6.7853e-21, 1) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(0.606617, -0.606617, 0.36334, 0.36334) * JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(-0.696652, -0.167293, 0.672156, 0.186804) * JPH::Quat(0.685863, 0.172023, -0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(0.36334, -0.36334, 0.606617, 0.606617) * JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(0.698441, -0.179085, -0.660011, 0.210936) * JPH::Quat(-0.685863, 0.172023, 0.685863, 0.172023) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
        JPH::Quat(6.71794e-08, -1.71143e-08, -1.85288e-22, 1) * JPH::Quat(-8.55714e-09, 2.42032e-08, -4.14221e-16, 1),
    };
    // DetermineRagdollData(skeleton, -1, shapes, positions, rotations, model.GetRootIndex(), model.GetNodes(), model.GetMeshes(), ml::mat4(1.0f));

    // Create ragdoll settings
    JPH::RagdollSettings settings;
    settings.mSkeleton = skeleton;
    settings.mParts.resize(skeleton->GetJointCount());
    for (int p = 0; p < skeleton->GetJointCount(); ++p)
    {
        JPH::RagdollSettings::Part &part = settings.mParts[p];
        part.SetShape(shapes[p]);
        part.mPosition = positions[p];
        part.mRotation = rotations[p];
        part.mMotionType = JPH::EMotionType::Dynamic;
        part.mObjectLayer = ObjectLayers::MOVING;

        /*
        if (p > 0)
        {
            // pour le point il faudra plutôt voir où les shapes sont link avec les emptyshape
            // regarder dans Blender comment sont liés les points pour savoir où mettre le point
            JPH::PointConstraintSettings *constraint = new JPH::PointConstraintSettings;
            // constraint->mPoint1 = positions[p];
            part.mToParent = constraint;
        }
            */
    }

    // Optional: Stabilize the inertia of the limbs
    settings.Stabilize();

    // Disable parent child collisions so that we don't get collisions between constrained bodies
    // settings.DisableParentChildCollisions();

    // Calculate the map needed for GetBodyIndexToConstraintIndex()
    settings.CalculateBodyIndexToConstraintIndex();

    WorldPhysic3D::AddRagdoll(this, settings, JPH::EActivation::DontActivate);
    // for (JPH::BodyID id : GetBodyIDs())
    //     WorldPhysic3D::SetPosition(id, JPH::Vec3(6, 20, 6), JPH::EActivation::Activate);

#pragma endregion
}

void Player::DetermineRagdollData(JPH::Ref<JPH::Skeleton> skeleton, int parentIndex, std::vector<JPH::Ref<JPH::Shape>> &shapes, std::vector<JPH::RVec3> &positions, std::vector<JPH::Quat> &rotations, size_t nodeIndex, std::map<int, NodeModel> &nodes, std::vector<Mesh> &meshes, const ml::mat4 &parentTransform)
{
    NodeModel node = nodes[nodeIndex];
    // skeleton joint
    uint jointIndex = skeleton->AddJoint(node.name, parentIndex);
    std::cout << node.name << " " << parentIndex << std::endl;
    // shape
    int meshIndex = -1;
    for (size_t i = 0; i < meshes.size(); i++)
    {
        if (meshes[i].GetNodeIndex() == nodeIndex)
        {
            meshIndex = i;
            break;
        }
    }
    if (meshIndex >= 0)
    {
        float minXZ, maxXZ, minY, maxY;
        minXZ = minY = std::numeric_limits<float>::max();
        maxXZ = maxY = std::numeric_limits<float>::min();
        std::vector<Glb::Primitive> primitives = meshes[meshIndex].GetPrimitives();
        for (size_t i = 0; i < primitives.size(); i++)
        {
            std::vector<Glb::Vertex> vertices = primitives[i].vertices;
            for (size_t j = 0; j < vertices.size(); j++)
            {
                if (vertices[j].x < minXZ)
                    minXZ = vertices[j].x;
                if (vertices[j].x > maxXZ)
                    maxXZ = vertices[j].x;

                if (vertices[j].z < minXZ)
                    minXZ = vertices[j].z;
                if (vertices[j].z > maxXZ)
                    maxXZ = vertices[j].z;

                if (vertices[j].y < minY)
                    minY = vertices[j].y;
                if (vertices[j].y > maxY)
                    maxY = vertices[j].y;
            }
        }

        float diameter = maxXZ - minXZ;
        shapes.push_back(new JPH::CapsuleShape((maxY - minY - diameter) / 2, diameter / 2));
        std::cout << "build shape " << (maxY - minY - diameter) / 2 << " " << diameter / 2 << std::endl;
        shapes.push_back(new JPH::CapsuleShape(0.025, 0.025));
    }
    else
        shapes.push_back(new JPH::EmptyShape());

    auto transform = parentTransform * node.transform;
    // position
    positions.push_back(JPH::RVec3(transform[0][3], transform[1][3], transform[2][3]));

    // rotation
    // ml::vec4 quat = toQuat(node.transform);
    // rotations.push_back(JPH::Quat(quat.x, quat.y, quat.z, quat.w));
    rotations.push_back(JPH::Quat::sIdentity());

    for (size_t i = 0; i < node.children.size(); i++)
        DetermineRagdollData(skeleton, jointIndex, shapes, positions, rotations, node.children[i], nodes, meshes, transform);
}

void Player::Destroy()
{
    WorldPhysic3D::RemoveRagdoll(this);
}

Player::~Player()
{
}

void Player::ProcessInput()
{
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

    /*
    if (WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT_SHIFT) && !(state & (PlayerStateFlag::JUMPING | PlayerStateFlag::ROLLING)) && onGround)
    {
        state |= PlayerStateFlag::ROLLING;
        ModelManager::GetModel(modelIndex).Play("Roll");
        WorldPhysic3D::SetShape(GetID(), rollingShape, true, JPH::EActivation::Activate);
        WorldPhysic3D::SetPosition(GetID(), WorldPhysic3D::GetPosition(GetID()) - JPH::Vec3::sAxisY() * PLAYER_CAPSULE_HEIGHT_DIFF, JPH::EActivation::Activate);
    }
    */
}

void Player::Update()
{
    if (state & PlayerStateFlag::DEFEATED)
        return;

    if (WorldPhysic3D::GetPosition(GetBodyID(0)).GetY() < 10)
    {
        for (JPH::BodyID id : GetBodyIDs())
            WorldPhysic3D::DeactivateBody(id);
    }
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

    /*
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
    */
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform = ml::translate(ml::mat4(1.0f), GetPosition()) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    if (state & PlayerStateFlag::DEFEATED)
        transform = transform * ml::rotate(ml::mat4(1.0f), 30 * (column - lastColumn), ml::vec3(0, 1, 0));
    bool enableRootMotion = !(state & PlayerStateFlag::ROLLING);
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, transform, enableRootMotion);
    return;
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, dynamic_cast<Ragdoll *>(this));

#if DRAW_IMGUI
    ImGui::Begin("Player");
    ImGui::Text("State: %s", magic_enum::enum_flags_name(static_cast<PlayerStateFlag>(state)).c_str());
    ImGui::Text("On ground: %s", (onGround ? "true" : "false"));
    ImGui::End();
#endif
}

ml::vec3 Player::GetPosition() const
{
    // JPH::RVec3 position = WorldPhysic3D::GetPosition(GetID());
    // return (ml::vec3(position.GetX(), position.GetY() - 1, position.GetZ()));
    return (ml::vec3(6, 2, 6));
}

void Player::OnWorldPhysicUpdated()
{
    onGround = false;
}

void Player::OnContactAdded([[maybe_unused]] const JPH::ContactManifold &inManifold)
{
    onGround = true;
}

void Player::OnContactPersisted([[maybe_unused]] const JPH::ContactManifold &inManifold)
{
    onGround = true;
}