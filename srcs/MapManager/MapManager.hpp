#pragma once

#include "Matrix/Matrix.hpp"
#include <unordered_map>
#include <vector>

struct Tile
{
    ml::vec3 position;
    int modelIndex;
    ml::mat4 transform;
};

class MapManager
{
    private:
        MapManager() = delete;
        ~MapManager() = delete;

        static std::vector<Tile> tiles;
        static ml::vec3 lastPlayerPos;

    public:
        static std::vector<Tile> UpdateTerrain(const ml::vec3 &playerPos);
};