#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "Engine/3D/ModelLoader/ModelLoader.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "MapManager/MapManager.hpp"
#include "MapManager/ChunkGenerator/ObstaclesGenerator/ObstaclesGenerator.hpp"
#include "MapManager/ChunkGenerator/TerrainGenerator/TerrainGenerator.hpp"
#include "MapManager/ChunkGenerator/CollectiblesGenerator/CollectiblesGenerator.hpp"

std::map<ChunkElements, int> ChunkGenerator::elements = {};
const int ChunkGenerator::chunkSize = 7;
const int ChunkGenerator::halfChunkSize = chunkSize / 2;
Chunk ChunkGenerator::lastChunk = {};
bool ChunkGenerator::firstChunk = true;

void ChunkGenerator::Init()
{
    std::vector<std::string> paths = {
        "assets/tiles/low/tileLow_teamBlue.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamBlue.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamBlue.gltf.glb",
        "assets/gates/small/gateSmall_teamBlue.gltf.glb",
        "assets/gates/large/gateLarge_teamBlue.gltf.glb",
        "assets/tiles/low/tileLow_teamRed.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamRed.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamRed.gltf.glb",
        "assets/gates/small/gateSmall_teamRed.gltf.glb",
        "assets/gates/large/gateLarge_teamRed.gltf.glb",
        "assets/tiles/low/tileLow_teamYellow.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamYellow.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamYellow.gltf.glb",
        "assets/gates/small/gateSmall_teamYellow.gltf.glb",
        "assets/gates/large/gateLarge_teamYellow.gltf.glb",
        "assets/tiles/low/tileLow_teamGreen.gltf.glb",
        "assets/slopes/lowMedium/tileSlopeLowMedium_teamGreen.gltf.glb",
        "assets/slopes/mediumHigh/tileSlopeMediumHigh_teamGreen.gltf.glb",
        "assets/gates/small/gateSmall_teamGreen.gltf.glb",
        "assets/gates/large/gateLarge_teamGreen.gltf.glb",
        "assets/spikeRoller.gltf.glb",
        "assets/barriers/barrierSmall.gltf.glb",
        "assets/star.gltf.glb",
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

    if (firstChunk)
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

    TerrainGenerator::GenerateTerrain(chunk);
#if SPAWN_OBSTACLES
    if (chunk.type == ChunkType::CLASSIC)
        ObstaclesGenerator::GenerateObstacles(chunk);
#endif
#if SPAWN_COLLECTIBLES
    CollectiblesGenerator::GenerateCollectibles(chunk);
#endif
    UpdateTerrainColor(chunk, DetermineTerrainColor(chunk));

    lastChunk = chunk;
    firstChunk = false;
    return (chunk);
}

TerrainColor ChunkGenerator::DetermineTerrainColor(const Chunk &chunk)
{
    if (chunk.dirX != 0)
    {
        if (chunk.dirX == 1)
            return (TerrainColor::RED);
        else
            return (TerrainColor::YELLOW);
    }
    else
    {
        if (chunk.dirZ == 1)
            return (TerrainColor::GREEN);
        else
            return (TerrainColor::BLUE);
    }
}

void ChunkGenerator::UpdateTerrainColor(Chunk &chunk, TerrainColor color)
{
    for (int i = 0; i < LaneType::COUNT; i++)
    {
        Lane &lane = chunk.lanes[i];
        for (size_t i = 0; i < lane.tiles.size(); i++)
        {
            if (lane.tiles[i]->flag & TileFlag::UPDATE_COLOR)
                lane.tiles[i]->modelIndex += color;
        }
    }
}

Tile42Run::Tile42Run(PhysicBodyType physicBodyType)
{
    this->physicBodyType = physicBodyType;
}

void Tile42Run::OnContactAdded([[maybe_unused]] const JPH::ContactManifold &inManifold, [[maybe_unused]] const PhysicBody3D *collisionedBody)
{
    if (physicBodyType == COLLECTIBLE && collisionedBody->GetPhysicBodyType() == PLAYER)
    {
        std::unique_ptr<RemoveBodyAction> action = std::make_unique<RemoveBodyAction>();
        action->id = GetID();
        WorldPhysic3D::SaveActionDelayed(std::move(action));
        flag |= TileFlag::DONT_DRAW;
    }
}

Lane::Lane()
{
    obstacleType = ChunkElements::NONE;
}

void Lane::AddTile(const std::shared_ptr<Tile42Run> &newTile, int laneIndex)
{
    newTile->laneIndex = laneIndex;
    newTile->tileIndex = tiles.size();
    tiles.push_back(newTile);
}