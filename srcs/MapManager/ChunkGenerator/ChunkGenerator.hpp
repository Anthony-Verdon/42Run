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
    GROUND,
    BOTTOM
};

struct Chunk
{
    int z;
    std::vector<Tile> tiles;
    Level levels[3];

};

class ChunkGenerator
{
    private:
        ChunkGenerator() = delete;
        
        static int chunkSize;
        static Chunk lastChunk;

        static void SpawnAllGround(Chunk &chunk);
        static void SpawnTopLevel(Chunk &chunk, int laneIndex);
        static void SpawnGroundLevel(Chunk &chunk, int laneIndex);
        static void SpawnBottomLevel(Chunk &chunk, int laneIndex);

        static void SpawnGround(Chunk &chunk, int x, int y);
        static Tile SpawnGroundTile(const ml::vec3 &position);

        static void SpawnSlopeUp(Chunk &chunk, int x, int y);
        static void SpawnSlopeDown(Chunk &chunk, int x, int y);
        static Tile SpawnSlopeTile(const ml::vec3 &position, float orientation, bool isMediumHigh); 
    
    public:
        static Chunk GenerateChunk(int chunkZ);
        static int GetChunkSize() { return (chunkSize); }
};