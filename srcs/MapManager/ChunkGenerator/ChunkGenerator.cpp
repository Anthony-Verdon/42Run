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

        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 0, chunkZ * chunkSize + 1));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 4;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 1, 0},
                {0, 1.5, 2},
                {2, 1, 0},
                {2, 1.5, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }
        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 0, chunkZ * chunkSize + 2));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 5;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 1.5, 0},
                {0, 2, 2},
                {2, 1.5, 0},
                {2, 2, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }
        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 1, chunkZ * chunkSize + 3));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 4;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 1, 0},
                {0, 1.5, 2},
                {2, 1, 0},
                {2, 1.5, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }
        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 1, chunkZ * chunkSize + 4));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 5;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 1.5, 0},
                {0, 2, 2},
                {2, 1.5, 0},
                {2, 2, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }

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

        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 1, chunkZ * chunkSize + 10));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 5;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 2, 0},
                {0, 1.5, 2},
                {2, 2, 0},
                {2, 1.5, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }

        
        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 1, chunkZ * chunkSize + 11));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 4;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
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
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }

        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 0, chunkZ * chunkSize + 12));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 5;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
            JPH::Vec3 points[8] = {
                {0, 0, 0},
                {0, 0, 2},
                {2, 0, 0},
                {2, 0, 2},
                {0, 2, 0},
                {0, 1.5, 2},
                {2, 2, 0},
                {2, 1.5, 2},
            };
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }

        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(-1, 0, chunkZ * chunkSize + 13));
            tile.size = ml::vec3(2, 1, 2);
            tile.modelIndex = 4;
            ml::vec3 positionTimeSize = tile.position * tile.size;
            ml::vec3 halfSize = tile.size / 2.0f;
            tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize) * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
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
            JPH::Shape::ShapeResult outResult;
            JPH::ConvexHullShapeSettings slopSetting(points, 8);
            JPH::BodyCreationSettings slopCreationSetting(new JPH::ConvexHullShape(slopSetting, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
            tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(slopCreationSetting, JPH::EActivation::DontActivate);
            chunk.tiles.push_back(tile);
        }

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