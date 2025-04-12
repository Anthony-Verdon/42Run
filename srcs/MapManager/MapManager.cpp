#include "MapManager/MapManager.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include <iostream>
#include <cmath>

std::vector<Tile> MapManager::tiles;
int MapManager::lastPlayerPos = 0;
int MapManager::nbRowDisplay = 5;

void MapManager::Init()
{
    for (int i = -1; i <= 1; i++)
    {
        for (int j = 0; j <= nbRowDisplay; j++)
            SpawnTile(i, 0, j);
    }

}

std::vector<Tile> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    for (auto it = tiles.begin(); it != tiles.end(); )
    {
        if (playerPos.z - it->position.z * it->size.z > it->size.z / 2)
        {
            WorldPhysic3D::GetBodyInterface().RemoveBody(it->bodyId);
            WorldPhysic3D::GetBodyInterface().DestroyBody(it->bodyId);
            it = tiles.erase(it);
        }
        else
            it++;
    }

    int playerPosZ = round(playerPos.z) / 2; // divide by 2 because we spawn tile of size 2, tmp
    if (playerPosZ != lastPlayerPos)
    {
        for (int i = -1; i <= 1; i++)
        {
            SpawnTile(i, 0, playerPosZ + nbRowDisplay);
        }

        if (playerPosZ == 5)
        {
            {
                Tile tile;
                tile.position = ml::vec3(ml::vec3(-1, 1, playerPosZ + nbRowDisplay));
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
                tiles.push_back(tile);
            }
            {
                Tile tile;
                tile.position = ml::vec3(ml::vec3(-1, 1, playerPosZ + nbRowDisplay - 1));
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
                tiles.push_back(tile);
            }
            {
                Tile tile;
                tile.position = ml::vec3(ml::vec3(-1, 0, playerPosZ + nbRowDisplay - 2));
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
                tiles.push_back(tile);
            }
            {
                Tile tile;
                tile.position = ml::vec3(ml::vec3(-1, 0, playerPosZ + nbRowDisplay - 3));
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
                tiles.push_back(tile);
            }
        }
        if (playerPosZ > 5)
        {
            for (int i = 1; i <= 2; i++)
            {
                SpawnTile(-1, i, playerPosZ + nbRowDisplay);
                SpawnTile(1, i, playerPosZ + nbRowDisplay);
            }
        }


        lastPlayerPos = playerPosZ;
    }

    return (tiles);
}

void MapManager::SpawnTile(int x, int y, int z)
{
    Tile tile;
    tile.position = ml::vec3(ml::vec3(x, y, z));
    int modelIndex = 3;
    switch (modelIndex)
    {
        case 1:
            tile.size = ml::vec3(6, 1, 6);
            break;
        case 2:
            tile.size = ml::vec3(2, 2, 2);
            break;
        case 3:
            tile.size = ml::vec3(2, 1, 2);
            break;
    }
    tile.modelIndex = modelIndex;
    ml::vec3 positionTimeSize = tile.position * tile.size;
    ml::vec3 halfSize = tile.size / 2.0f;
    tile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(JPH::RVec3(halfSize.x, halfSize.y, halfSize.z)), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    tile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    tiles.push_back(tile);
}