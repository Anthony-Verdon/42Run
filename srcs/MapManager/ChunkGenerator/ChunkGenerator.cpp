#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"

std::map<ChunkElements, int> ChunkGenerator::elements;
int ChunkGenerator::chunkSize = 7;
Chunk ChunkGenerator::lastChunk = {};

void ChunkGenerator::Init()
{
    std::vector<std::string> paths = {
        "assets/tiles/low/tileLow_teamBlue.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamBlue.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamBlue.gltf.glb",
        "assets/tiles/low/tileLow_teamRed.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamRed.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamRed.gltf.glb",
        "assets/tiles/low/tileLow_teamYellow.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamYellow.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamYellow.gltf.glb",
        "assets/tiles/low/tileLow_teamGreen.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamGreen.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamGreen.gltf.glb",
    };
    int nbModel = ModelManager::GetNbModel();
    for (size_t i = 0; i < paths.size(); i++)
    {
        ModelManager::AddModels(ModelLoader::LoadModel(paths[i]));
        elements[(ChunkElements)i] = nbModel + i;
        ModelManager::GetModel(nbModel + i).Init();
    }

}

Chunk ChunkGenerator::GenerateChunk(int dirX, int dirZ)
{
    Chunk chunk;

    if (lastChunk.tiles.empty())
    {
        chunk.x = 0;        
        chunk.z = 0;        
    }
    else
    {
        chunk.x = lastChunk.x + dirX;
        chunk.z = lastChunk.z + dirZ;
    }
    chunk.dirX = dirX;
    chunk.dirZ = dirZ;
    
    GenerateTerrain(chunk);
    UpdateTerrainColor(chunk);

    lastChunk = chunk;
    return (chunk);
}

void ChunkGenerator::GenerateTerrain(Chunk &chunk)
{
    if (lastChunk.tiles.empty())
    {
        SpawnAllGround(chunk);
        chunk.type = ChunkType::SPAWN;
    }
    else if (CanSpawnTurn())
    {
        SpawnTurn(chunk);
    }
    else if (lastChunk.type == ChunkType::TURN)
    {
        SpawnAllGround(chunk);
        chunk.type = ChunkType::OUT_OF_TURN;
    }
    else
    {
        for (int i = 0; i < Lane::COUNT; i++)
        {
            switch (lastChunk.levels[i])
            {
                case Level::TOP:
                {
                    SpawnTopLevel(chunk, i);
                    break;
                }
                case Level::GROUND:
                {
                    SpawnGroundLevel(chunk, i);
                    break;
                }
                case Level::BOTTOM:
                {
                   SpawnBottomLevel(chunk, i); 
                    break;
                }
            }
        }
        chunk.type = ChunkType::CLASSIC;
    }
}

bool ChunkGenerator::CanSpawnTurn()
{
    if (lastChunk.type != ChunkType::CLASSIC)
        return (false);

    for (int i = 0; i < Lane::COUNT; i++)
        if (lastChunk.levels[i] != Level::GROUND)
            return (false);

    return (true);
}

void ChunkGenerator::UpdateTerrainColor(Chunk &chunk)
{
    int tileColorModifier;
    if (chunk.dirX != 0)
    {
        if (chunk.dirX == 1)
            tileColorModifier = 3; // red
        else
            tileColorModifier = 6; // yellow
    }
    else
    {
        if (chunk.dirZ == 1)
            tileColorModifier = 9; // green
        else
            tileColorModifier = 0; // blue
    }
    for (size_t i = 0; i < chunk.tiles.size(); i++)
    {
        chunk.tiles[i].modelIndex += tileColorModifier;
    }

}