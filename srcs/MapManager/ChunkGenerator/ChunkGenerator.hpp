#pragma once

#include <vector>
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Matrix/Matrix.hpp"

struct Tile
{
    ml::vec3 position;
    ml::vec3 size;
    int modelIndex;
    ml::mat4 transform;
    JPH::BodyID bodyId;
};

enum Lane
{
    LEFT = 1,
    MIDDLE = 0,
    RIGHT = -1,
    COUNT = 3
};

enum Level
{
    TOP,
    ZERO,
    BOTTOM
};

struct Chunk
{
    std::vector<Tile> tiles;
    Level levels[3];

};

class ChunkGenerator
{
    private:
        ChunkGenerator() = delete;
        
        static int chunkSize;
        static Chunk lastChunk;
        static Tile SpawnGroundTile(const ml::vec3 &position);
        static void SpawnSlope(Chunk &chunk);
        static Tile SpawnSlopeTile(const ml::vec3 &position, float orientation, bool isMediumHigh); 
    
    public:
        static Chunk GenerateChunk(int chunkZ);
        static int GetChunkSize() { return (chunkSize); }
};