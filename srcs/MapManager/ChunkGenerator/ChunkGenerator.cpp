#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <magic_enum.hpp>

int ChunkGenerator::chunkSize = 6;
Chunk ChunkGenerator::lastChunk = {};

Chunk ChunkGenerator::GenerateChunk(int chunkZ)
{
    Chunk chunk;

    chunk.z = chunkZ;
    
    if (lastChunk.tiles.empty())
        SpawnAllGround(chunk);
    else 
    {
        for (int i = 0; i < Lane::COUNT; i++)
        {
            switch (lastChunk.levels[i])
            {
                case Level::TOP:
                {
                    SpawnTopLevel(chunk, i);
                    break;
                }
                case Level::GROUND:
                {
                    SpawnGroundLevel(chunk, i);
                    break;
                }
                case Level::BOTTOM:
                {
                   SpawnBottomLevel(chunk, i); 
                    break;
                }
            }
        }
    }

    lastChunk = chunk;

    return (chunk);
}

void ChunkGenerator::SpawnAllGround(Chunk &chunk)
{
    for (int i = 0; i < Lane::COUNT; i++)
    {
        SpawnGround(chunk, magic_enum::enum_value<Lane>(i), 0);
        chunk.levels[i] = Level::GROUND;
    }
}

void ChunkGenerator::SpawnTopLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 2;
    if (randomValue == 0)
    {
        SpawnGround(chunk, magic_enum::enum_value<Lane>(laneIndex), 2);
        chunk.levels[laneIndex] = Level::TOP;
    }
    else
    {
        SpawnSlopeDown(chunk, magic_enum::enum_value<Lane>(laneIndex), 2);
        chunk.levels[laneIndex] = Level::GROUND;
    }
}

void ChunkGenerator::SpawnGroundLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 3;
    if (randomValue == 0)
    {
        SpawnGround(chunk, magic_enum::enum_value<Lane>(laneIndex), 0);
        chunk.levels[laneIndex] = Level::GROUND;
    }
    else if (randomValue == 1)
    {
        SpawnSlopeUp(chunk, magic_enum::enum_value<Lane>(laneIndex), 2);
        chunk.levels[laneIndex] = Level::TOP;
    }
    else
    {
        SpawnSlopeDown(chunk, magic_enum::enum_value<Lane>(laneIndex), 0);
        chunk.levels[laneIndex] = Level::BOTTOM;
    }
}

void ChunkGenerator::SpawnBottomLevel(Chunk &chunk, int laneIndex)
{
    int randomValue = rand() % 2;
    if (randomValue == 0)
    {
        SpawnGround(chunk, magic_enum::enum_value<Lane>(laneIndex), -2);
        chunk.levels[laneIndex] = Level::BOTTOM;
    }
    else
    {
        SpawnSlopeUp(chunk, magic_enum::enum_value<Lane>(laneIndex), 0);
        chunk.levels[laneIndex] = Level::GROUND;
    }
}

void ChunkGenerator::SpawnGround(Chunk &chunk, int x, int y)
{
    for (int z = 0; z < chunkSize; z++)
        chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, y, chunk.z * chunkSize + z)));
}

Tile ChunkGenerator::SpawnGroundTile(const ml::vec3 &position)
{
    Tile tile;
    tile.position = position;
    tile.modelIndex = 3;
    tile.size = ml::vec3(2, 1, 2);
    ml::vec3 positionTimeSize = tile.position * tile.size;
    ml::vec3 halfSize = tile.size / 2.0f;
    tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(JPH::RVec3(halfSize.x, halfSize.y, halfSize.z)), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    return (tile);
}

void ChunkGenerator::SpawnSlopeDown(Chunk &chunk, int x, int y)
{
    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, y - 0, chunk.z * chunkSize + 0)));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 1, chunk.z * chunkSize + 1), 90, true));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 1, chunk.z * chunkSize + 2), 90, false));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 3), 90, true));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 4), 90, false));
    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 5)));

}

void ChunkGenerator::SpawnSlopeUp(Chunk &chunk, int x, int y)
{
    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 0)));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 1), -90, false));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 2, chunk.z * chunkSize + 2), -90, true));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 1, chunk.z * chunkSize + 3), -90, false));
    chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(x, y - 1, chunk.z * chunkSize + 4), -90, true));
    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, y - 0, chunk.z * chunkSize + 5)));
}
Tile ChunkGenerator::SpawnSlopeTile(const ml::vec3 &position, float orientation, bool isMediumHigh)
{
    Tile tile;
    tile.position = ml::vec3(ml::vec3(position));
    tile.size = ml::vec3(2, 1, 2);
    if (isMediumHigh)
        tile.modelIndex = 5;
    else
        tile.modelIndex = 4;
    ml::vec3 positionTimeSize = tile.position * tile.size;
    ml::vec3 halfSize = tile.size / 2.0f;
    tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), orientation, ml::vec3(0, 1, 0));

    JPH::Vec3 points[8] = {
        {0, 0, 0},
        {0, 0, 2},
        {2, 0, 0},
        {2, 0, 2},
        {0, 1.5, 0},
        {0, 1, 2},
        {2, 1.5, 0},
        {2, 1, 2},
    };
        
    if (orientation == -90.0f)
    {
        points[4].SetZ(2);
        points[5].SetZ(0);
        points[6].SetZ(2);
        points[7].SetZ(0);
    }

    if (isMediumHigh)
    {
        for (int i = 4; i < 8; i++)
            points[i].SetY(points[i].GetY() + 0.5);
    }

    JPH::Shape::ShapeResult outResult;
    JPH::ConvexHullShapeSettings slopSetting(points, 8);
    JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);

    return (tile);
}