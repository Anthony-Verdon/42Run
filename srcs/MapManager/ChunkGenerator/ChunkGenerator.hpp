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
    GATE_SMALL_BLUE,
    GATE_LARGE_BLUE,
    TILE_LOW_RED,
    SLOPE_LOW_MEDIUM_RED,
    SLOPE_MEDIUM_HIGH_RED, 
    GATE_SMALL_RED,
    GATE_LARGE_RED,
    TILE_LOW_YELLOW,
    SLOPE_LOW_MEDIUM_YELLOW,
    SLOPE_MEDIUM_HIGH_YELLOW,
    GATE_SMALL_YELLOW,
    GATE_LARGE_YELLOW,
    TILE_LOW_GREEN,
    SLOPE_LOW_MEDIUM_GREEN,
    SLOPE_MEDIUM_HIGH_GREEN,
    GATE_SMALL_GREEN,
    GATE_LARGE_GREEN,
    SPIKE_ROLLER,
    BARRIER,
};

enum TileFlag
{
    SLOPE = 1 << 0,
    GROUND_TILE = 1 << 1,
    OBSTACLES = 1 << 2,
    UPDATE_COLOR = 1 << 3,
    ROTATE_OVER_TIME = 1 << 4,
};

struct Tile
{
    ml::vec3 position;
    ml::vec3 size;
    int modelIndex;
    ml::mat4 transform;
    JPH::BodyID bodyId;
    int flag;
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
        static void UpdateTerrainColor(Chunk &chunk);
        
        // TerrainGenerator.cpp
        static void GenerateTerrain(Chunk &chunk);
        static bool CanSpawnTurn();
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
        
        // ObstaclesGenerator.cpp
        static void GenerateObstacles(Chunk &chunk);
        static void GenerateSpikeRoller(Chunk &chunk);
        static void GenerateGate(Chunk &chunk, bool highGate);
        static void GenerateBarrier(Chunk &chunk);

    public:
        static void Init();
        static Chunk GenerateChunk(int dirX, int dirZ);

        static int GetChunkSize() { return (chunkSize); }
        static int GetHalfChunkSize() { return (chunkSize / 2); }
        static ChunkType LastChunkGeneratedType() { return (lastChunk.type); }
};