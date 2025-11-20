#include "MapManager/MapManager.hpp"
#include <cmath>
#include <iostream>

std::queue<Chunk> MapManager::chunks;
int MapManager::playerPosX = 0;
int MapManager::playerPosZ = 0;
int MapManager::playerDirX = 0;
int MapManager::playerDirZ = 1;
const int MapManager::nbChunk = 5;
bool MapManager::tryDeleteChunk = false;

void MapManager::Init()
{
    playerPosX = 0;
    playerPosZ = 0;
    playerDirX = 0;
    playerDirZ = 1;
    tryDeleteChunk = false;
    for (; !chunks.empty(); chunks.pop())
        ;

    for (int i = 0; i <= nbChunk; i++)
    {
        if (ChunkGenerator::LastChunkGeneratedType() == ChunkType::TURN)
            break;
        chunks.push(ChunkGenerator::GenerateChunk(playerDirX, playerDirZ));
    }
}

void MapManager::UpdateTerrain(const ml::vec3 &playerPos, const ml::vec3 &playerDir)
{
    GenerateChunks(playerPos, playerDir);
    if (tryDeleteChunk)
        DeleteChunk(playerPos);
}

void MapManager::GenerateChunks(const ml::vec3 &playerPos, const ml::vec3 &playerDir)
{
    int newPlayerPosX = playerPos.x / 2 / ChunkGenerator::GetChunkSize();
    if (playerPos.x < 0)
        newPlayerPosX--;
    int newPlayerPosZ = playerPos.z / 2 / ChunkGenerator::GetChunkSize();
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
            {
                chunks.push(ChunkGenerator::GenerateChunk(playerDirX, playerDirZ));
                if (ChunkGenerator::LastChunkGeneratedType() == ChunkType::TURN)
                    break;
            }
        }
        tryDeleteChunk = true;
    }
}

void MapManager::DeleteChunk(const ml::vec3 &playerPos)
{
    bool deleteChunk = false;
    if (playerDirX != 0)
    {
        int posX = playerPos.x / 2;
        if (playerDirX == -1)
            posX++;
        deleteChunk = (abs(posX) % ChunkGenerator::GetChunkSize() == ChunkGenerator::GetHalfChunkSize());
    }
    else if (playerDirZ != 0)
    {
        int posZ = playerPos.z / 2;
        if (playerDirZ == -1)
            posZ++;
        deleteChunk = (abs(posZ) % ChunkGenerator::GetChunkSize() == ChunkGenerator::GetHalfChunkSize());
    }

    if (deleteChunk)
    {
        Chunk &chunk = chunks.front();
        for (int i = 0; i < LaneType::COUNT; i++)
        {
            Lane &lane = chunk.lanes[i];
            for (auto it = lane.tiles.begin(); it != lane.tiles.end(); it++)
            {
                const std::shared_ptr<Tile42Run> &tile = *it;
                if (!tile->BodyRemoved())
                    WorldPhysic3D::RemoveBody(tile->GetID());
            }
        }

        chunks.pop();

        tryDeleteChunk = false;
    }
}
