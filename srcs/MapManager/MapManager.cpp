#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::queue<Chunk> MapManager::chunks;
int MapManager::playerPosX = 0;
int MapManager::playerPosZ = 0;
int MapManager::playerDirX = 0;
int MapManager::playerDirZ = 1;
int MapManager::nbChunk = 5;

void MapManager::Init()
{
    for (int i = 0; i <= nbChunk; i++)
    {
        if (ChunkGenerator::LastChunkGeneratedType() == ChunkType::TURN)
            break;
        chunks.push(ChunkGenerator::GenerateChunk(playerDirX, playerDirZ));
    }
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
        if (ChunkGenerator::LastChunkGeneratedType() != ChunkType::TURN)
        {
            chunks.push(ChunkGenerator::GenerateChunk(playerDirX, playerDirZ));
        }
        else if (playerDirX != dirX || playerDirZ != dirZ)
        {
            playerDirX = dirX;
            playerDirZ = dirZ;
            for (int i = 0; i <= nbChunk; i++)
                chunks.push(ChunkGenerator::GenerateChunk(playerDirX, playerDirZ));
        }
        
        for (auto it = chunks.front().tiles.begin(); it != chunks.front().tiles.end(); it++)
        {
            WorldPhysic3D::GetBodyInterface().RemoveBody(it->bodyId);
            WorldPhysic3D::GetBodyInterface().DestroyBody(it->bodyId);
        }
        chunks.pop();
    }
    return (chunks);
}
