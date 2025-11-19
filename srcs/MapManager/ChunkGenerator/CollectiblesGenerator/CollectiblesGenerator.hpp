#pragma once

#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"

class ChunkGenerator::CollectiblesGenerator
{
    friend ChunkGenerator;

  private:
    static void GenerateCollectibles(Chunk &chunk);
    static void SpawnStars(Chunk &chunk, int laneNum);
    static std::shared_ptr<Tile42Run> SpawnStar(const ml::vec3 &pos, int chunkDirZ);
};