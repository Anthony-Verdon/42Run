#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::vector<Tile> MapManager::tiles;
int MapManager::lastPlayerPos = 0;

std::vector<Tile> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    for (auto it = tiles.begin(); it != tiles.end(); )
    {
        if (playerPos.z - it->position.z * it->size.z > it->size.z / 2)
            it = tiles.erase(it);
        else
            it++;
    }

    int playerPosZ = round(playerPos.z);
    static bool once = true;
    if (playerPosZ != lastPlayerPos && once)
    {
        for (int i = -1; i <= 1; i++)
        {
            Tile tile;
            tile.position = ml::vec3(ml::vec3(i, 0, playerPosZ));
            int modelIndex = 3;
            switch (modelIndex)
            {
                case 1:
                    tile.size = ml::vec3(6, 1, 6);
                    break;
                case 2:
                    tile.size = ml::vec3(2, 2, 2);
                    break;
                case 3:
                    tile.size = ml::vec3(2, 1, 2);
                    break;
            }
            tile.modelIndex = modelIndex;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * tile.size);
            tiles.push_back(tile);
        }

        lastPlayerPos = playerPosZ;
    }

    return (tiles);
}