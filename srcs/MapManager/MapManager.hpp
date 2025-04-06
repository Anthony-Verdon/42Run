#pragma once

#include "Matrix/vec/vec.hpp"
#include <map>

class MapManager
{
    private:
        MapManager() = delete;
        ~MapManager() = delete;

        static std::map<ml::vec3, int> tiles;

    public:
        static std::map<ml::vec3, int> UpdateTerrain(const ml::vec3 &playerPos);
};