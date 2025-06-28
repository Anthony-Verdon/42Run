#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "WorldPhysic/DebugRenderer.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <doctest/doctest.h>

TEST_CASE("ChunkGenerator::CanGoToLane")
{
    // @TODO: could be great to find a way to avoid having to init that
    BPLayerInterfaceImpl BPLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl ObjectVsBPLayerFilter;
    ObjectLayerPairFilterImpl OBjectLPFilter;
    ContactListener contactListener;
    WorldPhysic3D::Init(BPLayerInterface, ObjectVsBPLayerFilter, OBjectLPFilter);
    WorldPhysic3D::SetContactListener(&contactListener);
    JPH::DebugRenderer::sInstance = new DebugRendererImpl();

    SUBCASE("Current level: top")
    {
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::TOP, Level::BOTTOM));
    }
    SUBCASE("Current level: going down to ground")
    {
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: going to top")
    {
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::BOTTOM));
    }
    SUBCASE("Current level: ground")
    {
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GROUND, Level::TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GROUND, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::GROUND, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GROUND, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: going to bottom")
    {
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::BOTTOM));
    }
    SUBCASE("Current level: going up to ground")
    {
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_TO_TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_UP_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: bottom")
    {
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_DOWN_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_TO_TOP));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::GROUND));
        CHECK(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_UP_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::CanGoToLane(Level::BOTTOM, Level::BOTTOM));
    }
}