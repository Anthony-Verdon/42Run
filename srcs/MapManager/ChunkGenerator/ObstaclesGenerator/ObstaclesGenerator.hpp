#pragma once

#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"

class ChunkGenerator::ObstaclesGenerator
{
    friend ChunkGenerator;

  private:
    static void GenerateObstacles(Chunk &chunk);
    static std::shared_ptr<Tile42Run> GenerateSpikeRoller(const ml::vec3 &position);
    static std::shared_ptr<Tile42Run> GenerateGate(const ml::vec3 &position, int chunkDirZ, bool highGate);
    static JPH::TriangleList GetGateHitbox();
    static std::shared_ptr<Tile42Run> GenerateBarrier(const ml::vec3 &position, int chunkDirZ);

  public:
    static bool CanGoToLane(const Lane &currentLane, const Lane &futureLane);
    static bool CanGoToLane(Level currentLevel, Level futureLevel);
};