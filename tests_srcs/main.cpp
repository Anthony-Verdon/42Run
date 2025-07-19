#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "MapManager/ChunkGenerator/ObstaclesGenerator/ObstaclesGenerator.hpp"
#include "MapManager/ChunkGenerator/TerrainGenerator/TerrainGenerator.hpp"
#include <doctest/doctest.h>
#include <magic_enum.hpp>

TEST_CASE("ChunkGenerator::TerrainGenerator::CanSpawnTurn")
{
    SUBCASE("ChunkType::SPAWN")
    {
        Chunk chunk;
        chunk.type = ChunkType::SPAWN;
        CHECK_FALSE(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
    }
    SUBCASE("ChunkType::TURN")
    {
        Chunk chunk;
        chunk.type = ChunkType::TURN;
        CHECK_FALSE(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
    }
    SUBCASE("ChunkType::OUT_OF_TURN")
    {
        Chunk chunk;
        chunk.type = ChunkType::OUT_OF_TURN;
        CHECK_FALSE(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
    }
    SUBCASE("ChunkType::CLASSIC")
    {
        Chunk chunk;
        chunk.type = ChunkType::CLASSIC;
        for (int i = 0; i < LaneType::COUNT; i++)
            chunk.lanes[i].level = Level::GROUND;
        SUBCASE("all lanes to ground")
        {
            CHECK(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
        }

        // generate all combinations
        size_t nbLevel = magic_enum::enum_count<Level>();
        std::vector<size_t> indices(LaneType::COUNT, 0);
        while (true)
        {
            for (size_t i = 0; i < LaneType::COUNT; i++)
                chunk.lanes[i].level = magic_enum::enum_value<Level>(indices[i]);

            bool checkAllGround = true;
            std::string subcaseName = "chunk lanes level:";
            for (size_t i = 0; i < LaneType::COUNT; i++)
            {
                subcaseName += " " + std::string(magic_enum::enum_name(chunk.lanes[i].level));
                if (chunk.lanes[i].level != Level::GROUND)
                    checkAllGround = false;
            }

            SUBCASE(subcaseName.c_str())
            {
                if (checkAllGround)
                    CHECK(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
                else
                    CHECK_FALSE(ChunkGenerator::TerrainGenerator::CanSpawnTurn(chunk));
            }

            bool done = false;
            for (int i = 0; i < LaneType::COUNT; i++)
            {
                indices[i]++;
                if (indices[i] < nbLevel)
                    break;

                indices[i] = 0;

                if (i == LaneType::COUNT - 1)
                    done = true;
            }
            if (done)
                break;
        }
    }
}

TEST_CASE("ChunkGenerator::TerrainGenerator::CalculateSlopeRotation")
{
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(-1, 0, 0), true) == 0);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(1, 0, 0), true) == 180);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(-1, 0, 0), false) == 180);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(1, 0, 0), false) == 0);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(0, 0, -1), true) == 90);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(0, 0, 1), true) == 270);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(0, 0, -1), false) == 270);
    CHECK(ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(ml::vec3(0, 0, 1), false) == 90);
}

TEST_CASE("ChunkGenerator::ObstaclesGenerator::CanGoToLane")
{
    // @TODO: could be great to find a way to avoid having to init that
    WorldPhysic3D::Init();

    SUBCASE("Current level: top")
    {
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::TOP, Level::BOTTOM));
    }
    SUBCASE("Current level: going down to ground")
    {
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_DOWN_TO_GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: going to top")
    {
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_TOP, Level::BOTTOM));
    }
    SUBCASE("Current level: ground")
    {
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: going to bottom")
    {
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_DOWN_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_TO_TOP));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_UP_TO_GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_TO_BOTTOM, Level::BOTTOM));
    }
    SUBCASE("Current level: going up to ground")
    {
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_DOWN_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_TO_TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_UP_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::GOING_UP_TO_GROUND, Level::BOTTOM));
    }
    SUBCASE("Current level: bottom")
    {
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_DOWN_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_TO_TOP));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::GROUND));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_UP_TO_GROUND));
        CHECK_FALSE(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::GOING_TO_BOTTOM));
        CHECK(ChunkGenerator::ObstaclesGenerator::CanGoToLane(Level::BOTTOM, Level::BOTTOM));
    }
}

TEST_CASE("ChunkGenerator::DetermineTerrainColor")
{
    Chunk chunk;
    chunk.dirX = 1;
    chunk.dirZ = 0;
    CHECK(ChunkGenerator::DetermineTerrainColor(chunk) == TerrainColor::RED);
    chunk.dirX = -1;
    chunk.dirZ = 0;
    CHECK(ChunkGenerator::DetermineTerrainColor(chunk) == TerrainColor::YELLOW);
    chunk.dirX = 0;
    chunk.dirZ = 1;
    CHECK(ChunkGenerator::DetermineTerrainColor(chunk) == TerrainColor::GREEN);
    chunk.dirX = 0;
    chunk.dirZ = -1;
    CHECK(ChunkGenerator::DetermineTerrainColor(chunk) == TerrainColor::BLUE);
}
