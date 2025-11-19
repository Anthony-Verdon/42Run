#pragma once

#include "Engine/3D/WorldPhysic3D/WorldPhysic3D.hpp"
#include "Matrix/Matrix.hpp"
#include <map>
#include <vector>
#include "PhysicBodyType.hpp"

enum class ChunkElements
{
    NONE = -1,
    TILE_LOW_BLUE,
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
    STAR
};

enum TileFlag
{
    SLOPE = 1 << 0,
    GROUND_TILE = 1 << 1,
    OBSTACLES = 1 << 2,
    UPDATE_COLOR = 1 << 3,
    ROTATE_OVER_TIME = 1 << 4,
    DONT_DRAW = 1 << 5
};

// temporary rename because of Tile struct in the Engine
struct Tile42Run : public PhysicBody3D
{
    ml::vec3 position;
    ml::vec3 size;
    int modelIndex;
    ml::mat4 transform;
    int flag;

    int tileIndex;
    int laneIndex;

    Tile42Run(PhysicBodyType physicBodyType);
    void OnContactAdded([[maybe_unused]] const JPH::ContactManifold &inManifold, [[maybe_unused]] const PhysicBody3D *collisionedBody);
};

enum LaneType
{
    LEFT = 1,
    MIDDLE = 0,
    RIGHT = -1,
    COUNT = 3
};

// order matters
enum Level
{
    TOP,
    GOING_DOWN_TO_GROUND,
    GOING_TO_TOP,
    GROUND,
    GOING_TO_BOTTOM,
    GOING_UP_TO_GROUND,
    BOTTOM
};

enum ChunkType
{
    SPAWN,
    TURN,
    OUT_OF_TURN,
    CLASSIC
};

struct Lane
{
    std::vector<std::shared_ptr<Tile42Run>> tiles;
    Level level;

    ChunkElements obstacleType;
    ml::vec3 obstaclePos;
    bool hasSpikeRoller;

    Lane();
    void AddTile(const std::shared_ptr<Tile42Run> &newTile, int laneIndex);
};

struct Chunk
{
    int x;
    int z;
    int dirX;
    int dirZ;
    ChunkType type;
    Lane lanes[LaneType::COUNT];
};

enum TerrainColor
{
    BLUE = 0,
    RED = 5,
    YELLOW = 10,
    GREEN = 15
};

class ChunkGenerator
{
  private:
    ChunkGenerator() = delete;

    static std::map<ChunkElements, int> elements;
    static const int chunkSize;
    static const int halfChunkSize;
    static Chunk lastChunk;
    static bool firstChunk;

    static void UpdateTerrainColor(Chunk &chunk, TerrainColor color);

  public:
    class TerrainGenerator;
    class ObstaclesGenerator;
    class CollectiblesGenerator;

    static void Init();
    static Chunk GenerateChunk(int dirX, int dirZ);

    static TerrainColor DetermineTerrainColor(const Chunk &chunk);

    static int GetChunkSize() { return (chunkSize); }
    static int GetHalfChunkSize() { return (halfChunkSize); }
    static ChunkType LastChunkGeneratedType() { return (lastChunk.type); }
};