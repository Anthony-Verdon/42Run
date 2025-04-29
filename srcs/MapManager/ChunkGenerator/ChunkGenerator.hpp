#pragma once

#include <vector>
#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Matrix/Matrix.hpp"
#include <map>

enum ChunkElements
{
    TILE_LOW_BLUE = 0,
    SLOPE_LOW_MEDIUM_BLUE,
    SLOPE_MEDIUM_HIGH_BLUE,
    TILE_LOW_RED,
    SLOPE_LOW_MEDIUM_RED,
    SLOPE_MEDIUM_HIGH_RED, 
    TILE_LOW_YELLOW,
    SLOPE_LOW_MEDIUM_YELLOW,
    SLOPE_MEDIUM_HIGH_YELLOW,
    TILE_LOW_GREEN,
    SLOPE_LOW_MEDIUM_GREEN,
    SLOPE_MEDIUM_HIGH_GREEN,
    SPIKE_ROLLER
};

struct Tile
{
    ml::vec3 position;
    ml::vec3 size;
    int modelIndex;
    ml::mat4 transform;
    JPH::BodyID bodyId;
    bool updateColor;
    bool rotateOverTime;
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

enum ChunkType
{
    SPAWN,
    TURN,
    OUT_OF_TURN,
    CLASSIC
};
struct Chunk
{
    int x;
    int z;
    int dirX;
    int dirZ;
    ChunkType type;
    std::vector<Tile> tiles;
    Level levels[3];

};

class ChunkGenerator
{
    private:
        ChunkGenerator() = delete;
        
        static std::map<ChunkElements, int> elements;
        static int chunkSize;
        static Chunk lastChunk;

        // ChunkGenerator.cpp
        static void GenerateTerrain(Chunk &chunk);
        static bool CanSpawnTurn();
        static void UpdateTerrainColor(Chunk &chunk);
        static void GenerateObstacles(Chunk &chunk);

        // TerrainGenerator.cpp
        static void SpawnAllGround(Chunk &chunk);
        static void SpawnTurn(Chunk &chunk);
        static void SpawnTopLevel(Chunk &chunk, int laneIndex);
        static void SpawnGroundLevel(Chunk &chunk, int laneIndex);
        static void SpawnBottomLevel(Chunk &chunk, int laneIndex);

        static void SpawnGround(Chunk &chunk, int lane, int y);
        static Tile SpawnGroundTile(const ml::vec3 &position);

        static void SpawnSlopeUp(Chunk &chunk, int lane, int y);
        static void SpawnSlopeDown(Chunk &chunk, int lane, int y);
        static Tile SpawnSlopeTile(const ml::vec3 &position, const ml::vec3 &direction, bool isMediumHigh, bool goingUp);
        static std::pair<float, std::vector<JPH::Vec3>> CalculateSlopRotation(const ml::vec3 &direction, bool goingUp);
    
    public:
        static void Init();
        static Chunk GenerateChunk(int dirX, int dirZ);

        static int GetChunkSize() { return (chunkSize); }
        static int GetHalfChunkSize() { return (chunkSize / 2); }
        static ChunkType LastChunkGeneratedType() { return (lastChunk.type); }
};