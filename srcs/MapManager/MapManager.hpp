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
        static std::queue<Chunk> chunks;
    public:
        static void Init();
        static std::queue<Chunk> UpdateTerrain(const ml::vec3 &playerPos, const ml::vec3 &playerDir);
};