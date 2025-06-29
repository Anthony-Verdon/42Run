#pragma once

#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"

class ChunkGenerator::ObstaclesGenerator
{
    friend ChunkGenerator;

  private:
    static void GenerateObstacles(Chunk &chunk);
    static Tile GenerateSpikeRoller(const ml::vec3 &position);
    static Tile GenerateGate(const ml::vec3 &position, int chunkDirZ, bool highGate);
    static JPH::TriangleList GetGateHitbox();
    static Tile GenerateBarrier(const ml::vec3 &position, int chunkDirZ);

  public:
    static bool CanGoToLane(const Lane &currentLane, const Lane &futureLane);
    static bool CanGoToLane(Level currentLevel, Level futureLevel);
};