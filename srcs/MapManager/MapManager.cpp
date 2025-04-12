#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::queue<Chunk> MapManager::chunks;
int MapManager::playerPosZ = 0;
int MapManager::nbChunk = 5;

void MapManager::Init()
{
    for (int i = 0; i <= nbChunk; i++)
        chunks.push(ChunkGenerator::GenerateChunk(i));
}

std::queue<Chunk> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    int newPlayerPosZ = (int)round(playerPos.z / 2) / ChunkGenerator::GetChunkSize();
    if (playerPosZ != newPlayerPosZ)
    {
        playerPosZ = newPlayerPosZ;
        chunks.push(ChunkGenerator::GenerateChunk(playerPosZ + nbChunk));
        for (auto it = chunks.front().tiles.begin(); it != chunks.front().tiles.end(); it++)
        {
            WorldPhysic3D::GetBodyInterface().RemoveBody(it->bodyId);
            WorldPhysic3D::GetBodyInterface().DestroyBody(it->bodyId);
        }
        chunks.pop();
    }
    return (chunks);
}
