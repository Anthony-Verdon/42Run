#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

int ChunkGenerator::chunkSize = 15;

Chunk ChunkGenerator::GenerateChunk(int chunkZ)
{
    Chunk chunk;

    int value = rand() % 2;
    if (value)
    {
        for (int x = -1; x <= 1; x++)
        {
            for (int z = 0; z < chunkSize; z++)
            {
                chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + z)));
            }
        }
    }
    else
    {
        for (int x = -1; x <= 1; x++)
            chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + 0)));

        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 0, chunkZ * chunkSize + 1), -90, false));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 0, chunkZ * chunkSize + 2), -90, true));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 1, chunkZ * chunkSize + 3), -90, false));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 1, chunkZ * chunkSize + 4), -90, true));

        for (int x = 0; x <= 1; x++)
        {
            for (int z = 1; z < 5; z++)
            {
                chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + z)));
            }
        }

        for (int x = -1; x <= 1; x++)
        {
            for (int z = 5; z < 10; z++)
            {
                if (x == -1)
                    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 2, chunkZ * chunkSize + z)));
                else
                    chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + z)));
            }
        }

        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 1, chunkZ * chunkSize + 10), 90, true));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 1, chunkZ * chunkSize + 11), 90, false));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 0, chunkZ * chunkSize + 12), 90, true));
        chunk.tiles.push_back(SpawnSlopeTile(ml::vec3(-1, 0, chunkZ * chunkSize + 13), 90, false));

        for (int x = 0; x <= 1; x++)
        {
            for (int z = 10; z < 14; z++)
            {
                chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + z)));
            }
        }

        for (int x = -1; x <= 1; x++)
            chunk.tiles.push_back(SpawnGroundTile(ml::vec3(x, 0, chunkZ * chunkSize + 14)));
    }

    return (chunk);
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