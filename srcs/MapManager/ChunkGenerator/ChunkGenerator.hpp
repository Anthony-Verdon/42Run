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

struct Chunk
{
    std::vector<Tile> tiles;
};

class ChunkGenerator
{
    private:
        ChunkGenerator() = delete;
        
        static int chunkSize;
        static Tile SpawnGroundTile(const ml::vec3 &position); 
    
    public:
        static Chunk GenerateChunk(int chunkZ);
        static int GetChunkSize() { return (chunkSize); }
};