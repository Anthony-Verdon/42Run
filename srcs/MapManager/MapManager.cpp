#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::map<ml::vec3, int> MapManager::tiles;

std::map<ml::vec3, int> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    for (auto it = tiles.begin(); it != tiles.end(); )
    {
        if (playerPos.z - it->first[2] > 1)
            it = tiles.erase(it);
        else
            it++;
    }

    ml::vec3 playerPosRounded = playerPos;
    for (int i = 0; i < 3; i++)
        playerPosRounded[i] = round(playerPosRounded[i]);
    tiles[playerPosRounded + ml::vec3(-1, -1, 4)] = 1;
    tiles[playerPosRounded + ml::vec3(0, -1, 4)] = 1;
    tiles[playerPosRounded + ml::vec3(1, -1, 4)] = 1;

    return (tiles);
}