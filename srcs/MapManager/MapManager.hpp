#pragma once

#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include <queue>

class MapManager
{
    private:
        MapManager() = delete;
        ~MapManager() = delete;

        static int playerPosX;
        static int playerPosZ;
        static int playerDirX;
        static int playerDirZ;
        static int nbChunk;
        static bool tryDeleteChunk;
        static std::queue<Chunk> chunks;

        static void GenerateChunks(const ml::vec3 &playerPos, const ml::vec3 &playerDir);
        static void DeleteChunk(const ml::vec3 &playerPos);
    public:
        static void Init();
        static void UpdateTerrain(const ml::vec3 &playerPos, const ml::vec3 &playerDir);
        static std::queue<Chunk> GetChunks() { return (chunks); }
        static ChunkType GetCurrentChunkType() { return (chunks.front().type); }
};