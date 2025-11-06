#include "MapManager/ChunkGenerator/TerrainGenerator/TerrainGenerator.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <magic_enum.hpp>
#include "Game/Layers.hpp"

void ChunkGenerator::TerrainGenerator::GenerateTerrain(Chunk &chunk)
{
    if (firstChunk)
    {
        SpawnAllGround(chunk);
        chunk.type = ChunkType::SPAWN;
    }
#if SPAWN_TURN
    else if (CanSpawnTurn())
    {
        SpawnTurn(chunk);
    }
    else if (lastChunk.type == ChunkType::TURN)
    {
        SpawnAllGround(chunk);
        chunk.type = ChunkType::OUT_OF_TURN;
    }
#endif
    else
    {
#if FLAT_TERRAIN
        SpawnAllGround(chunk);
#else
        for (int i = 0; i < LaneType::COUNT; i++)
        {
            switch (lastChunk.lanes[i].level)
            {
            case Level::GOING_TO_TOP:
            case Level::TOP:
                SpawnTopLevel(chunk, i);
                break;
            case Level::GOING_DOWN_TO_GROUND:
            case Level::GOING_UP_TO_GROUND:
            case Level::GROUND:
                SpawnGroundLevel(chunk, i);
                break;
            case Level::GOING_TO_BOTTOM:
            case Level::BOTTOM:
                SpawnBottomLevel(chunk, i);
                break;
            }
        }
#endif
        chunk.type = ChunkType::CLASSIC;
    }
}

void ChunkGenerator::TerrainGenerator::SpawnAllGround(Chunk &chunk)
{
    for (int i = 0; i < LaneType::COUNT; i++)
    {
        SpawnGround(chunk, i, 0);
        chunk.lanes[i].level = Level::GROUND;
    }
}

void ChunkGenerator::TerrainGenerator::SpawnTurn(Chunk &chunk)
{
    if (chunk.dirZ != 0)
    {
        for (int z = -3; z < -1; z++)
            for (int x = 0; x < LaneType::COUNT; x++)
                chunk.lanes[x].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + magic_enum::enum_value<LaneType>(x), 0, chunk.z * chunkSize + chunkSize / 2 + z * chunk.dirZ)));
        for (int x = -3; x < 4; x++)
            for (int z = 0; z < LaneType::COUNT; z++)
                chunk.lanes[z].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + x, 0, chunk.z * chunkSize + chunkSize / 2 + magic_enum::enum_value<LaneType>(z))));
    }
    else
    {
        for (int x = -3; x < -1; x++)
            for (int z = 0; z < LaneType::COUNT; z++)
                chunk.lanes[z].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + x * chunk.dirX, 0, chunk.z * chunkSize + chunkSize / 2 + magic_enum::enum_value<LaneType>(z))));
        for (int z = -3; z < 4; z++)
            for (int x = 0; x < LaneType::COUNT; x++)
                chunk.lanes[x].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + magic_enum::enum_value<LaneType>(x), 0, chunk.z * chunkSize + chunkSize / 2 + z)));
    }

    chunk.type = ChunkType::TURN;
}

void ChunkGenerator::TerrainGenerator::SpawnTopLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 2;
    if (randomValue == 0)
    {
        SpawnGround(chunk, laneIndex, 2);
        chunk.lanes[laneIndex].level = Level::TOP;
    }
    else
    {
        SpawnSlopeDown(chunk, laneIndex, 2);
        chunk.lanes[laneIndex].level = Level::GOING_DOWN_TO_GROUND;
    }
}

void ChunkGenerator::TerrainGenerator::SpawnGroundLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 3;
    if (randomValue == 0)
    {
        SpawnGround(chunk, laneIndex, 0);
        chunk.lanes[laneIndex].level = Level::GROUND;
    }
    else if (randomValue == 1)
    {
        SpawnSlopeUp(chunk, laneIndex, 2);
        chunk.lanes[laneIndex].level = Level::GOING_TO_TOP;
    }
    else
    {
        SpawnSlopeDown(chunk, laneIndex, 0);
        chunk.lanes[laneIndex].level = Level::GOING_TO_BOTTOM;
    }
}

void ChunkGenerator::TerrainGenerator::SpawnBottomLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 2;
    if (randomValue == 0)
    {
        SpawnGround(chunk, laneIndex, -2);
        chunk.lanes[laneIndex].level = Level::BOTTOM;
    }
    else
    {
        SpawnSlopeUp(chunk, laneIndex, 0);
        chunk.lanes[laneIndex].level = Level::GOING_UP_TO_GROUND;
    }
}

void ChunkGenerator::TerrainGenerator::SpawnGround(Chunk &chunk, int laneIndex, int y)
{
    int lane = magic_enum::enum_value<LaneType>(laneIndex);
    if (chunk.dirZ != 0)
    {
        for (int z = 0; z < chunkSize; z++)
            chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y, chunk.z * chunkSize + z)));
    }
    else
    {
        for (int x = 0; x < chunkSize; x++)
            chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + x, y, chunk.z * chunkSize + chunkSize / 2 + lane)));
    }
}

void ChunkGenerator::TerrainGenerator::SpawnSlopeDown(Chunk &chunk, int laneIndex, int y)
{
    int lane = magic_enum::enum_value<LaneType>(laneIndex);
    if (chunk.dirZ == 1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 0, chunk.z * chunkSize + 0)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 1), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 2), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 3)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 4), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 5), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 6)));
    }
    else if (chunk.dirZ == -1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 0)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 2), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 3), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 4)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 5), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 6), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 0, chunk.z * chunkSize + 7)));
    }
    else if (chunk.dirX == 1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 0, y - 0, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 1, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 2, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 3, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 4, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 5, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 6, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane)));
    }
    else if (chunk.dirX == -1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 0, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 1, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 2, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 3, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 4, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 5, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, false));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 6, y - 0, chunk.z * chunkSize + chunkSize / 2 + lane)));
    }
}

void ChunkGenerator::TerrainGenerator::SpawnSlopeUp(Chunk &chunk, int laneIndex, int y)
{
    int lane = magic_enum::enum_value<LaneType>(laneIndex);
    if (chunk.dirZ == 1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 0)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 1), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 2), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 3)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 4), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 5), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 0, chunk.z * chunkSize + 6)));
    }
    else if (chunk.dirZ == -1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 0, chunk.z * chunkSize + 0)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 1), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 2), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 1, chunk.z * chunkSize + 3)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 4), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 5), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + chunkSize / 2 + lane, y - 2, chunk.z * chunkSize + 6)));
    }
    else if (chunk.dirX == 1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 0, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 1, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 2, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 3, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 4, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 5, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 6, y - 0, chunk.z * chunkSize + chunkSize / 2 + lane)));
    }
    else if (chunk.dirX == -1)
    {
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 0, y - 0, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 1, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 2, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 3, y - 1, chunk.z * chunkSize + chunkSize / 2 + lane)));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 4, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), true, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnSlopeTile(ml::vec3(chunk.x * chunkSize + 5, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane), ml::vec3(chunk.dirX, 0, chunk.dirZ), false, true));
        chunk.lanes[laneIndex].tiles.push_back(SpawnGroundTile(ml::vec3(chunk.x * chunkSize + 6, y - 2, chunk.z * chunkSize + chunkSize / 2 + lane)));
    }
}

std::shared_ptr<Tile42Run> ChunkGenerator::TerrainGenerator::SpawnGroundTile(const ml::vec3 &position)
{
    std::shared_ptr<Tile42Run> tile = std::make_shared<Tile42Run>();
    tile->position = position;
    tile->modelIndex = elements[ChunkElements::TILE_LOW_BLUE];
    tile->size = ml::vec3(2, 1, 2);
    ml::vec3 halfSize = tile->size / 2.0f;
    ml::vec3 positionTimeSize = tile->position * tile->size;
    tile->transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(JPH::RVec3(halfSize.x, halfSize.y, halfSize.z)), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ObjectLayers::NON_MOVING);
    WorldPhysic3D::AddBody(tile.get(), boxSettings, JPH::EActivation::DontActivate);
    tile->flag = TileFlag::GROUND_TILE | TileFlag::UPDATE_COLOR;
    return (tile);
}

std::shared_ptr<Tile42Run> ChunkGenerator::TerrainGenerator::SpawnSlopeTile(const ml::vec3 &position, const ml::vec3 &direction, bool isMediumHigh, bool goingUp)
{
    std::shared_ptr<Tile42Run> tile = std::make_shared<Tile42Run>();

    tile->position = position;
    tile->size = ml::vec3(2, 1, 2);
    if (isMediumHigh)
        tile->modelIndex = elements[ChunkElements::SLOPE_MEDIUM_HIGH_BLUE];
    else
        tile->modelIndex = elements[ChunkElements::SLOPE_LOW_MEDIUM_BLUE];
    ml::vec3 positionTimeSize = tile->position * tile->size;
    ml::vec3 halfSize = tile->size / 2.0f;
    float angle = CalculateSlopeRotation(direction, goingUp);
    std::vector<JPH::Vec3> points = InitSlopePoints(angle);
    tile->transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), angle, ml::vec3(0, 1, 0));

    if (isMediumHigh)
    {
        for (int i = 4; i < 8; i++)
            points[i].SetY(points[i].GetY() + 0.5);
    }

    JPH::Shape::ShapeResult outResult;
    JPH::ConvexHullShapeSettings slopSetting(points.data(), points.size());
    JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ObjectLayers::NON_MOVING);
    WorldPhysic3D::AddBody(tile.get(), slopCreationSetting, JPH::EActivation::DontActivate);
    tile->flag = TileFlag::SLOPE | TileFlag::UPDATE_COLOR;

    return (tile);
}

std::vector<JPH::Vec3> ChunkGenerator::TerrainGenerator::InitSlopePoints(float angle)
{
    std::vector<JPH::Vec3> points;

    if (angle == 0) // slope going up on x axis
    {
        points.push_back({0, 0, 0});
        points.push_back({0, 0, 2});
        points.push_back({2, 0, 0});
        points.push_back({2, 0, 2});
        points.push_back({2, 1, 0});
        points.push_back({2, 1, 2});
        points.push_back({0, 1.5, 0});
        points.push_back({0, 1.5, 2});
    }
    else if (angle == 90) // slope going down on z axis
    {
        points.push_back({0, 0, 0});
        points.push_back({0, 0, 2});
        points.push_back({2, 0, 0});
        points.push_back({2, 0, 2});
        points.push_back({0, 1.5, 0});
        points.push_back({0, 1, 2});
        points.push_back({2, 1.5, 0});
        points.push_back({2, 1, 2});
    }
    else if (angle == 180) // slope going down on x axis
    {
        points.push_back({0, 0, 0});
        points.push_back({0, 0, 2});
        points.push_back({2, 0, 0});
        points.push_back({2, 0, 2});
        points.push_back({0, 1, 0});
        points.push_back({0, 1, 2});
        points.push_back({2, 1.5, 0});
        points.push_back({2, 1.5, 2});
    }
    else if (angle == 270) // slope going up on z axis
    {
        points.push_back({0, 0, 0});
        points.push_back({0, 0, 2});
        points.push_back({2, 0, 0});
        points.push_back({2, 0, 2});
        points.push_back({0, 1.5, 2});
        points.push_back({0, 1, 0});
        points.push_back({2, 1.5, 2});
        points.push_back({2, 1, 0});
    }

    return (points);
}

bool ChunkGenerator::TerrainGenerator::CanSpawnTurn(const Chunk &chunk)
{
    if (chunk.type != ChunkType::CLASSIC)
        return (false);

    for (int i = 0; i < LaneType::COUNT; i++)
        if (chunk.lanes[i].level != Level::GROUND)
            return (false);

    return (true);
}

float ChunkGenerator::TerrainGenerator::CalculateSlopeRotation(const ml::vec3 &direction, bool goingUp)
{
    const ml::vec3 originalDirection = ml::vec3(-1, 0, 0);
    float angle = acos((direction.x * originalDirection.x + direction.y * originalDirection.y + direction.z * originalDirection.z)) / (sqrt(pow(originalDirection.x, 2) + pow(originalDirection.y, 2) + pow(originalDirection.z, 2)) * sqrt(pow(direction.x, 2) + pow(direction.y, 2) + pow(direction.z, 2)));
    angle = ml::degrees(angle);
    if ((goingUp && direction.z == 1) || (!goingUp && direction.z == -1) || (!goingUp && direction.x == 1) || (!goingUp && direction.x == -1))
        angle += 180;

    return ((int)angle % 360);
}