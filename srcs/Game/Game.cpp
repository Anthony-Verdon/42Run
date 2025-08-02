#include "Game/Game.hpp"
#include "Game/Layers.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Engine/RessourceManager/RessourceManager.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "MapManager/MapManager.hpp"
#if DRAW_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif
#include <iostream>
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>
#include "Engine/3D/WorldPhysic3D/ObjectAndBroadPhaseLayer/ObjectAndBroadPhaseLayer.hpp"
Game::Game()
{
    srand(time(NULL));

    WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    LineRenderer3D::Init();
    lights.push_back(std::make_unique<DirectionalLight>(ml::vec3(1, 1, 1), 3, ml::vec3(0, -1, 0)));

    ChunkGenerator::Init();

    SetUpLayers();
    WorldPhysic3D::Init();
    player.Init();
    MapManager::Init();

    accumulatedTime = 0;

#if DRAW_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(WindowManager::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
#endif
#pragma region INIT_RAGDOLL
    // Create skeleton
    JPH::Ref<JPH::Skeleton> skeleton = new JPH::Skeleton;
    uint lower_body = skeleton->AddJoint("LowerBody");
    uint mid_body = skeleton->AddJoint("MidBody", lower_body);
    uint upper_body = skeleton->AddJoint("UpperBody", mid_body);
    /*uint head =*/skeleton->AddJoint("Head", upper_body);
    uint upper_arm_l = skeleton->AddJoint("UpperArmL", upper_body);
    uint upper_arm_r = skeleton->AddJoint("UpperArmR", upper_body);
    /*uint lower_arm_l =*/skeleton->AddJoint("LowerArmL", upper_arm_l);
    /*uint lower_arm_r =*/skeleton->AddJoint("LowerArmR", upper_arm_r);
    uint upper_leg_l = skeleton->AddJoint("UpperLegL", lower_body);
    uint upper_leg_r = skeleton->AddJoint("UpperLegR", lower_body);
    /*uint lower_leg_l =*/skeleton->AddJoint("LowerLegL", upper_leg_l);
    /*uint lower_leg_r =*/skeleton->AddJoint("LowerLegR", upper_leg_r);

    // Create shapes for limbs
    JPH::Ref<JPH::Shape> shapes[] = {
        new JPH::CapsuleShape(0.15f, 0.10f),  // Lower Body
        new JPH::CapsuleShape(0.15f, 0.10f),  // Mid Body
        new JPH::CapsuleShape(0.15f, 0.10f),  // Upper Body
        new JPH::CapsuleShape(0.075f, 0.10f), // Head
        new JPH::CapsuleShape(0.15f, 0.06f),  // Upper Arm L
        new JPH::CapsuleShape(0.15f, 0.06f),  // Upper Arm R
        new JPH::CapsuleShape(0.15f, 0.05f),  // Lower Arm L
        new JPH::CapsuleShape(0.15f, 0.05f),  // Lower Arm R
        new JPH::CapsuleShape(0.2f, 0.075f),  // Upper Leg L
        new JPH::CapsuleShape(0.2f, 0.075f),  // Upper Leg R
        new JPH::CapsuleShape(0.2f, 0.06f),   // Lower Leg L
        new JPH::CapsuleShape(0.2f, 0.06f),   // Lower Leg R
    };

    // Positions of body parts in world space
    JPH::RVec3 positions[] = {
        JPH::RVec3(0, 1.15f, 0),       // Lower Body
        JPH::RVec3(0, 1.35f, 0),       // Mid Body
        JPH::RVec3(0, 1.55f, 0),       // Upper Body
        JPH::RVec3(0, 1.825f, 0),      // Head
        JPH::RVec3(-0.425f, 1.55f, 0), // Upper Arm L
        JPH::RVec3(0.425f, 1.55f, 0),  // Upper Arm R
        JPH::RVec3(-0.8f, 1.55f, 0),   // Lower Arm L
        JPH::RVec3(0.8f, 1.55f, 0),    // Lower Arm R
        JPH::RVec3(-0.15f, 0.8f, 0),   // Upper Leg L
        JPH::RVec3(0.15f, 0.8f, 0),    // Upper Leg R
        JPH::RVec3(-0.15f, 0.3f, 0),   // Lower Leg L
        JPH::RVec3(0.15f, 0.3f, 0),    // Lower Leg R
    };

    // Rotations of body parts in world space
    JPH::Quat rotations[] = {
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Lower Body
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Mid Body
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Upper Body
        JPH::Quat::sIdentity(),                                        // Head
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Upper Arm L
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Upper Arm R
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Lower Arm L
        JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI), // Lower Arm R
        JPH::Quat::sIdentity(),                                        // Upper Leg L
        JPH::Quat::sIdentity(),                                        // Upper Leg R
        JPH::Quat::sIdentity(),                                        // Lower Leg L
        JPH::Quat::sIdentity()                                         // Lower Leg R
    };

    // World space constraint positions
    JPH::RVec3 constraint_positions[] = {
        JPH::RVec3::sZero(),           // Lower Body (unused, there's no parent)
        JPH::RVec3(0, 1.25f, 0),       // Mid Body
        JPH::RVec3(0, 1.45f, 0),       // Upper Body
        JPH::RVec3(0, 1.65f, 0),       // Head
        JPH::RVec3(-0.225f, 1.55f, 0), // Upper Arm L
        JPH::RVec3(0.225f, 1.55f, 0),  // Upper Arm R
        JPH::RVec3(-0.65f, 1.55f, 0),  // Lower Arm L
        JPH::RVec3(0.65f, 1.55f, 0),   // Lower Arm R
        JPH::RVec3(-0.15f, 1.05f, 0),  // Upper Leg L
        JPH::RVec3(0.15f, 1.05f, 0),   // Upper Leg R
        JPH::RVec3(-0.15f, 0.55f, 0),  // Lower Leg L
        JPH::RVec3(0.15f, 0.55f, 0),   // Lower Leg R
    };

    // World space twist axis directions
    JPH::Vec3 twist_axis[] = {
        JPH::Vec3::sZero(),   // Lower Body (unused, there's no parent)
        JPH::Vec3::sAxisY(),  // Mid Body
        JPH::Vec3::sAxisY(),  // Upper Body
        JPH::Vec3::sAxisY(),  // Head
        -JPH::Vec3::sAxisX(), // Upper Arm L
        JPH::Vec3::sAxisX(),  // Upper Arm R
        -JPH::Vec3::sAxisX(), // Lower Arm L
        JPH::Vec3::sAxisX(),  // Lower Arm R
        -JPH::Vec3::sAxisY(), // Upper Leg L
        -JPH::Vec3::sAxisY(), // Upper Leg R
        -JPH::Vec3::sAxisY(), // Lower Leg L
        -JPH::Vec3::sAxisY(), // Lower Leg R
    };

    // Constraint limits
    float twist_angle[] = {
        0.0f,  // Lower Body (unused, there's no parent)
        5.0f,  // Mid Body
        5.0f,  // Upper Body
        90.0f, // Head
        45.0f, // Upper Arm L
        45.0f, // Upper Arm R
        45.0f, // Lower Arm L
        45.0f, // Lower Arm R
        45.0f, // Upper Leg L
        45.0f, // Upper Leg R
        45.0f, // Lower Leg L
        45.0f, // Lower Leg R
    };

    float normal_angle[] = {
        0.0f,  // Lower Body (unused, there's no parent)
        10.0f, // Mid Body
        10.0f, // Upper Body
        45.0f, // Head
        90.0f, // Upper Arm L
        90.0f, // Upper Arm R
        0.0f,  // Lower Arm L
        0.0f,  // Lower Arm R
        45.0f, // Upper Leg L
        45.0f, // Upper Leg R
        0.0f,  // Lower Leg L
        0.0f,  // Lower Leg R
    };

    float plane_angle[] = {
        0.0f,  // Lower Body (unused, there's no parent)
        10.0f, // Mid Body
        10.0f, // Upper Body
        45.0f, // Head
        45.0f, // Upper Arm L
        45.0f, // Upper Arm R
        90.0f, // Lower Arm L
        90.0f, // Lower Arm R
        45.0f, // Upper Leg L
        45.0f, // Upper Leg R
        60.0f, // Lower Leg L (cheating here, a knee is not symmetric, we should have rotated the twist axis)
        60.0f, // Lower Leg R
    };

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

        // First part is the root, doesn't have a parent and doesn't have a constraint
        if (p > 0)
        {
            JPH::SwingTwistConstraintSettings *constraint = new JPH::SwingTwistConstraintSettings;
            constraint->mDrawConstraintSize = 0.1f;
            constraint->mPosition1 = constraint->mPosition2 = constraint_positions[p];
            constraint->mTwistAxis1 = constraint->mTwistAxis2 = twist_axis[p];
            constraint->mPlaneAxis1 = constraint->mPlaneAxis2 = JPH::Vec3::sAxisZ();
            constraint->mTwistMinAngle = -JPH::DegreesToRadians(twist_angle[p]);
            constraint->mTwistMaxAngle = JPH::DegreesToRadians(twist_angle[p]);
            constraint->mNormalHalfConeAngle = JPH::DegreesToRadians(normal_angle[p]);
            constraint->mPlaneHalfConeAngle = JPH::DegreesToRadians(plane_angle[p]);
            part.mToParent = constraint;
        }
    }

    // Optional: Stabilize the inertia of the limbs
    settings.Stabilize();

    // Disable parent child collisions so that we don't get collisions between constrained bodies
    settings.DisableParentChildCollisions();

    // Calculate the map needed for GetBodyIndexToConstraintIndex()
    settings.CalculateBodyIndexToConstraintIndex();

/*
WorldPhysic3D::AddRagdoll(&ragdoll, settings, JPH::EActivation::Activate);
for (JPH::BodyID id : ragdoll.GetBodyIDs())
    WorldPhysic3D::SetPosition(id, JPH::Vec3(6, 2, 10), JPH::EActivation::Activate);
*/
#pragma endregion
}

Game::~Game()
{
    for (size_t i = 0; i < ModelManager::GetNbModel(); i++)
        ModelManager::GetModel(i).Destroy();

    player.Destroy();
    LineRenderer3D::Destroy();
    WorldPhysic3D::Destroy();

#if DRAW_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
}

void Game::SetUpLayers()
{
    WorldPhysic3D::broadPhaseLayerInterface.AddObjectToBroadPhaseLayerRelation(ObjectLayers::NON_MOVING, BroadPhaseLayers::NON_MOVING, "NON_MOVING");
    WorldPhysic3D::broadPhaseLayerInterface.AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::MOVING, "MOVING");
    WorldPhysic3D::objectVsBroadPhaseLayerFilter.AddObjectToBroadPhaseLayerRelation(ObjectLayers::NON_MOVING, BroadPhaseLayers::MOVING);
    WorldPhysic3D::objectVsBroadPhaseLayerFilter.AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::MOVING);
    WorldPhysic3D::objectVsBroadPhaseLayerFilter.AddObjectToBroadPhaseLayerRelation(ObjectLayers::MOVING, BroadPhaseLayers::NON_MOVING);
    WorldPhysic3D::objectLayerPairFilter.AddObjectToObjectRelation(ObjectLayers::NON_MOVING, ObjectLayers::MOVING);
    WorldPhysic3D::objectLayerPairFilter.AddObjectToObjectRelation(ObjectLayers::MOVING, ObjectLayers::MOVING);
    WorldPhysic3D::objectLayerPairFilter.AddObjectToObjectRelation(ObjectLayers::MOVING, ObjectLayers::NON_MOVING);
}

void Game::Run()
{
    accumulatedTime += Time::getDeltaTime();
    while (accumulatedTime >= WorldPhysic3D::GetDeltaTime())
    {
        WorldPhysic3D::Update();
        accumulatedTime -= WorldPhysic3D::GetDeltaTime();
    }

    MapManager::UpdateTerrain(player.GetPosition(), player.GetDirection());
    ProcessInput();

    UpdateCamera();
    Draw();
#if DEBUG_DRAW_PHYSIC_3D
    WorldPhysic3D::DebugDraw();
#endif
    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    LineRenderer3D::Draw(projection, view);
}

void Game::ProcessInput()
{
    if (WindowManager::IsInputPressed(GLFW_KEY_ESCAPE))
#if DRAW_IMGUI
    {
        if (WindowManager::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            WindowManager::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else if (WindowManager::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
            WindowManager::StopUpdateLoop();
    }
#else
        WindowManager::StopUpdateLoop();
#endif

    player.ProcessInput();
    player.Update();
}

void Game::UpdateCamera()
{
#if CAMERA_DETACH
    // position
    camera.setSpeed(20);
    const float speed = camera.getSpeed() * Time::getDeltaTime();

    int front = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_W) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_S);
    camera.addToPosition((float)front * camera.getFrontDirection() * speed);

    int right = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_D) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_A);
    camera.addToPosition((float)right * camera.getRightDirection() * speed);

    int up = WindowManager::IsInputPressedOrMaintain(GLFW_KEY_SPACE) - WindowManager::IsInputPressedOrMaintain(GLFW_KEY_LEFT_SHIFT);
    camera.addToPosition((float)up * camera.getUpDirection() * speed);

    // orientation
    const float sensitivity = 0.1f;

    ml::vec2 mousePos = WindowManager::GetMousePosition();
    static float lastX = mousePos.x;
    static float lastY = mousePos.y;

    float xOffset;
    float yOffset;
    xOffset = (mousePos.x - lastX) * sensitivity;
    yOffset = (lastY - mousePos.y) * sensitivity;
    lastX = mousePos.x;
    lastY = mousePos.y;
    camera.addToYaw(xOffset);
    camera.addToPitch(yOffset);
    if (camera.getPitch() > 89.0f)
        camera.setPitch(89.0f);
    else if (camera.getPitch() < -89.0f)
        camera.setPitch(-89.0f);

    ml::vec3 direction(cosf(ml::radians(camera.getYaw())) * cosf(ml::radians(camera.getPitch())),
                       sinf(ml::radians(camera.getPitch())),
                       sinf(ml::radians(camera.getYaw())) * cosf(ml::radians(camera.getPitch())));
    camera.setFrontDirection(ml::normalize(direction));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
#else
    ml::vec3 cameraPosition;
    if (player.IsDefeated())
        cameraPosition = (ml::vec3(0, 1, 0) * 3 - ml::normalize(player.GetDirection()) * 5);
    else
        cameraPosition = (ml::vec3(0, 1, 0) - ml::normalize(player.GetDirection())) * 3;
    camera.setPosition(player.GetPosition() + cameraPosition);

    ml::vec3 cameraOrientation = ml::vec3(0, -0.25, 0);
    camera.setFrontDirection(ml::normalize(player.GetDirection() + cameraOrientation));
    camera.setRightDirection(ml::normalize(ml::crossProduct(camera.getFrontDirection(), camera.getUpDirection())));
#endif
}

void Game::Draw()
{
#if DRAW_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif

    ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());
    player.Draw(camera.getPosition(), lights, projection, view);

    auto chunks = MapManager::GetChunks();
    int rotationValue = (int)ml::degrees(Time::getTime() * 2) % 360;
    for (; !chunks.empty(); chunks.pop())
    {
        Chunk &chunk = chunks.front();
        for (int i = 0; i < LaneType::COUNT; i++)
        {
            Lane &lane = chunk.lanes[i];
            for (auto it = lane.tiles.begin(); it != lane.tiles.end(); it++)
            {
                const std::shared_ptr<Tile> &tile = *it;
                ml::mat4 rotation = ml::mat4(1.0f);
                if (tile->flag & TileFlag::ROTATE_OVER_TIME)
                    rotation = ml::rotate(rotation, rotationValue, ml::vec3(0, 1, 0));
                ModelManager::GetModel(tile->modelIndex).Draw(camera.getPosition(), lights, projection, view, tile->transform * rotation);
            }
        }
    }

#if DRAW_IMGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}