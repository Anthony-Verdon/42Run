#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::vector<Tile> MapManager::tiles;
ml::vec3 MapManager::lastPlayerPos = {0, 0, 0};

std::vector<Tile> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    for (auto it = tiles.begin(); it != tiles.end(); )
    {
        if (playerPos.z - it->position.z > 1)
            it = tiles.erase(it);
        else
            it++;
    }

    ml::vec3 playerPosRounded = playerPos;
    for (int i = 0; i < 3; i++)
        playerPosRounded[i] = round(playerPosRounded[i]);

    if (lastPlayerPos != playerPosRounded)
    {
        if (playerPosRounded.z + 4 == 10)
        {
            Tile tile;
            tile.position = playerPosRounded + ml::vec3(ml::vec3(-1, -0.5, 4));
            tile.modelIndex = 2;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            tiles.push_back(tile);
        } 
        
        for (int i = -1; i <= 1; i++)
        {
            Tile tile;
            tile.position = playerPosRounded + ml::vec3(ml::vec3(i, -1, 4));
            tile.modelIndex = 1;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }

        lastPlayerPos = playerPosRounded;
    }

    return (tiles);
}