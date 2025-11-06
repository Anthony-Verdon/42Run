#pragma once

#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"

class ChunkGenerator::TerrainGenerator
{
    friend ChunkGenerator;

  private:
    static void GenerateTerrain(Chunk &chunk);
    static void SpawnAllGround(Chunk &chunk);
    static void SpawnTurn(Chunk &chunk);
    static void SpawnTopLevel(Chunk &chunk, int laneIndex);
    static void SpawnGroundLevel(Chunk &chunk, int laneIndex);
    static void SpawnBottomLevel(Chunk &chunk, int laneIndex);

    static void SpawnGround(Chunk &chunk, int laneIndex, int y);
    static void SpawnSlopeUp(Chunk &chunk, int laneIndex, int y);
    static void SpawnSlopeDown(Chunk &chunk, int laneIndex, int y);

    static std::shared_ptr<Tile42Run> SpawnGroundTile(const ml::vec3 &position);
    static std::shared_ptr<Tile42Run> SpawnSlopeTile(const ml::vec3 &position, const ml::vec3 &direction, bool isMediumHigh, bool goingUp);
    static std::vector<JPH::Vec3> InitSlopePoints(float angle);

  public:
    static bool CanSpawnTurn(const Chunk &chunk = lastChunk);
    static float CalculateSlopeRotation(const ml::vec3 &direction, bool goingUp);
};