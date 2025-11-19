#include "MapManager/ChunkGenerator/CollectiblesGenerator/CollectiblesGenerator.hpp"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "Game/Layers.hpp"

void ChunkGenerator::CollectiblesGenerator::GenerateCollectibles(Chunk &chunk)
{
    // faut check le type de chunk
    // choisir une ligne
    // si elle a un obstacle, check le type d'obstacle
    // si on a spike roller, mettre les stars avant ou après
    // sinon, mettre dedans
    // check le level de la ligne aussi
    if (chunk.type == CLASSIC)
        SpawnStars(chunk, rand() % LaneType::COUNT);
}

void ChunkGenerator::CollectiblesGenerator::SpawnStars(Chunk &chunk, int laneNum)
{
    Lane &lane = chunk.lanes[laneNum];

    if (lane.level != Level::BOTTOM && lane.level != Level::TOP && lane.level != Level::GROUND)
        return;

    switch (lane.obstacleType)
    {
    case ChunkElements::NONE:
        break;
    case ChunkElements::SPIKE_ROLLER:
        break;
    case ChunkElements::GATE_LARGE_BLUE:
    case ChunkElements::GATE_LARGE_RED:
    case ChunkElements::GATE_LARGE_YELLOW:
    case ChunkElements::GATE_LARGE_GREEN:
    case ChunkElements::GATE_SMALL_BLUE:
    case ChunkElements::GATE_SMALL_RED:
    case ChunkElements::GATE_SMALL_YELLOW:
    case ChunkElements::GATE_SMALL_GREEN:
    case ChunkElements::BARRIER:
        for (int i = -1; i <= 1; i++)
        {
            ml::vec3 starPos = lane.obstaclePos + ml::vec3(0, 1, 0);
            if (chunk.dirZ == 0)
                starPos.x += i * 2;
            else
                starPos.z += i * 2;
            if (lane.obstacleType == ChunkElements::BARRIER && i == 0)
                starPos.y += 1;
            lane.AddTile(SpawnStar(starPos, chunk.dirZ), laneNum);
        }
        break;
    default:
        break;
    }
}

std::shared_ptr<Tile42Run> ChunkGenerator::CollectiblesGenerator::SpawnStar(const ml::vec3 &position, int chunkDirZ)
{
    std::shared_ptr<Tile42Run> tile = std::make_shared<Tile42Run>(PhysicBodyType::COLLECTIBLE);
    tile->position = position;
    tile->modelIndex = elements[ChunkElements::STAR];
    tile->size = ml::vec3(1, 1, 1);
    ml::vec3 halfSize = tile->size / 2.0f;
    ml::vec3 positionTimeSize = tile->position * tile->size;
    tile->transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    if (chunkDirZ == 0)
        tile->transform = tile->transform * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(JPH::RVec3(halfSize.x, halfSize.y, halfSize.z)), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ObjectLayers::NON_MOVING);
    WorldPhysic3D::AddBody(tile.get(), boxSettings, JPH::EActivation::DontActivate);
    WorldPhysic3D::DeactivateBody(tile->GetID());
    tile->flag = 0;
    return (tile);
}