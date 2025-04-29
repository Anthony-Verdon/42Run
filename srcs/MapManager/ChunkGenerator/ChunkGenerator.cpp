#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

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
        "assets/spikeRoller.gltf.glb",
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
    GenerateObstacles(chunk);
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

void ChunkGenerator::GenerateObstacles(Chunk &chunk)
{
    if (chunk.levels[0] != lastChunk.levels[0] ||  chunk.type == ChunkType::TURN)
        return;
    
    Tile newTile;
    int height = 0;
    switch (chunk.levels[0])
    {
        case TOP:
        {
            height = 1;
            break;
        }
        case GROUND:
        {
            height = 0;
            break;
        }
        case BOTTOM:
        {
            height = -1;
            break;
        }
        default:
            break;
    }
    if (chunk.dirZ != 0)
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2 + -1) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2) * 2);
    else
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2 + -1) * 2);
    newTile.size = ml::vec3(1, 1, 1);
    newTile.modelIndex = elements[ChunkElements::SPIKE_ROLLER];
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    ml::vec3 halfSize = newTile.size / 2.0f;
    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::BodyCreationSettings boxSettings(new JPH::CylinderShape(1.5, 0.5), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.updateColor = false;
    newTile.rotateOverTime = true;

    chunk.tiles.push_back(newTile);
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
        if (chunk.tiles[i].updateColor)
            chunk.tiles[i].modelIndex += tileColorModifier;
    }

}