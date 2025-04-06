#include "MapManager/MapManager.hpp"
#include <iostream>
#include <cmath>

std::vector<Tile> MapManager::tiles;
int MapManager::lastPlayerPos = 0;

std::vector<Tile> MapManager::UpdateTerrain(const ml::vec3 &playerPos)
{
    for (auto it = tiles.begin(); it != tiles.end(); )
    {
        if (playerPos.z - it->position.z > 1)
            it = tiles.erase(it);
        else
            it++;
    }

    int playerPosZ =  round(playerPos.z);

    if (playerPosZ != lastPlayerPos)
    {
        if (playerPosZ + 4 == 10)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(-1, 0.5, 4));
            tile.modelIndex = 2;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f) * ml::rotate(ml::mat4(1.0f), -90, ml::vec3(0, 1, 0));
            tiles.push_back(tile);
        }
        else if (playerPosZ + 4 == 15)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(0, 0.5, 4));
            tile.modelIndex = 3;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }
        else if (playerPosZ + 4 == 20)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(0, 0.5, 4));
            tile.modelIndex = 4;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }
        else if (playerPosZ + 4 == 25)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(0, 0.5, 4));
            tile.modelIndex = 5;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }
        else if (playerPosZ + 4 == 30)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(0, 0.5, 4));
            tile.modelIndex = 6;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }
        else if (playerPosZ + 4 == 35)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(0, 0.5, 4));
            tile.modelIndex = 7;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        } 
        
        for (int i = -1; i <= 1; i++)
        {
            Tile tile;
            tile.position = ml::vec3(0, 0, playerPosZ) + ml::vec3(ml::vec3(i, 0, 4));
            tile.modelIndex = 1;
            tile.transform = ml::translate(ml::mat4(1.0f), tile.position * 2.0f);
            tiles.push_back(tile);
        }

        lastPlayerPos = playerPosZ;
    }

    return (tiles);
}