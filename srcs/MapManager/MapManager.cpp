#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::queue<Chunk> MapManager::chunks;
int MapManager::playerPosZ = 0;
int MapManager::playerPosX = 0;
int MapManager::nbChunk = 1;

void MapManager::Init()
{
    for (int z = 0; z < nbChunk; z++)
        chunks.push(ChunkGenerator::GenerateChunk(0, z, 0, 1));
}

std::queue<Chunk> MapManager::UpdateTerrain(const ml::vec3 &playerPos, const ml::vec3 &playerDir)
{
    int newPlayerPosX = (int)round(playerPos.x / 2) / ChunkGenerator::GetChunkSize();
    if (playerPos.x < 0)
        newPlayerPosX--;
    int newPlayerPosZ = (int)round(playerPos.z / 2) / ChunkGenerator::GetChunkSize();
    if (playerPos.z < 0)
        newPlayerPosZ--;

    if (playerPosZ != newPlayerPosZ || playerPosX != newPlayerPosX)
    {
        playerPosX = newPlayerPosX;
        playerPosZ = newPlayerPosZ;
        int dirX = (int)round(playerDir.x);
        int dirZ = (int)round(playerDir.z);
        chunks.push(ChunkGenerator::GenerateChunk(newPlayerPosX, playerPosZ, dirX, dirZ));
        for (auto it = chunks.front().tiles.begin(); it != chunks.front().tiles.end(); it++)
        {
            WorldPhysic3D::GetBodyInterface().RemoveBody(it->bodyId);
            WorldPhysic3D::GetBodyInterface().DestroyBody(it->bodyId);
            
        }
        chunks.pop();
    }
    return (chunks);
}
