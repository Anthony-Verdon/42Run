#pragma once

#include "Matrix/Matrix.hpp"
#include <unordered_map>
#include <vector>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
struct Tile
{
    ml::vec3 position;
    ml::vec3 size;
    int modelIndex;
    ml::mat4 transform;
    JPH::BodyID bodyId;

};

class MapManager
{
    private:
        MapManager() = delete;
        ~MapManager() = delete;

        static std::vector<Tile> tiles;
        static int lastPlayerPos;
        static int nbRowDisplay;

        static void SpawnTile(int x, int z);
    public:
        static void Init();
        static std::vector<Tile> UpdateTerrain(const ml::vec3 &playerPos);
};