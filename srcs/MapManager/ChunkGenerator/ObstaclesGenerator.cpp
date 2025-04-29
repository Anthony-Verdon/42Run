#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

void ChunkGenerator::GenerateObstacles(Chunk &chunk)
{
    GenerateSpikeRoller(chunk);
    GenerateSmallGate(chunk);
}

void ChunkGenerator::GenerateSpikeRoller(Chunk &chunk)
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


void ChunkGenerator::GenerateSmallGate(Chunk &chunk)
{
    if (chunk.levels[1] != lastChunk.levels[1] ||  chunk.type == ChunkType::TURN)
        return;

    Tile newTile;
    int height = 0;
    switch (chunk.levels[1])
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
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2) * 2);
    else
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2) * 2);
    newTile.size = ml::vec3(1, 1, 1);
    newTile.modelIndex = elements[ChunkElements::GATE_SMALL_BLUE];
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    ml::vec3 halfSize = newTile.size / 2.0f;
    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::RVec3 hitboxSize;
    if (chunk.dirZ != 0)
    {
        hitboxSize = JPH::RVec3(newTile.size.x, halfSize.y / 2, halfSize.z);
    }
    else
    {
        hitboxSize = JPH::RVec3(halfSize.x, halfSize.y / 2, newTile.size.z);
        newTile.transform = newTile.transform * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
    }
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(hitboxSize), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + newTile.size.y * 2, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.updateColor = false;
    newTile.rotateOverTime = false;

    chunk.tiles.push_back(newTile);
}