#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

void ChunkGenerator::GenerateObstacles(Chunk &chunk)
{
    GenerateSpikeRoller(chunk);
    GenerateGate(chunk, true);
    GenerateBarrier(chunk);
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


void ChunkGenerator::GenerateGate(Chunk &chunk, bool highGate)
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
    newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2) * 2);
    newTile.size = ml::vec3(1, 1, 1);
    if (highGate)
        newTile.modelIndex = elements[ChunkElements::GATE_LARGE_BLUE];
    else
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

    JPH::TriangleList triangles;
    
    // front face
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 2, 0.25), JPH::Float3(-0.75, 0, 0.25), JPH::Float3(-0.25, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 0, 0.25), JPH::Float3(-0.25, 2, 0.25), JPH::Float3(-0.75, 2, 0.25)));

    triangles.push_back(JPH::Triangle(JPH::Float3(1.75, 2, 0.25), JPH::Float3(1.75, 0, 0.25), JPH::Float3(1.25, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 0, 0.25), JPH::Float3(1.25, 2, 0.25), JPH::Float3(1.75, 2, 0.25)));
    
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 2, 0.25), JPH::Float3(-0.25, 2, 0.25), JPH::Float3(1.25, 1.75, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 1.75, 0.25), JPH::Float3(-0.25, 1.75, 0.25), JPH::Float3(-0.25, 2, 0.25)));

    // back face
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 2, 0.75), JPH::Float3(-0.75, 0, 0.75), JPH::Float3(-0.25, 0, 0.75)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 0, 0.75), JPH::Float3(-0.25, 2, 0.75), JPH::Float3(-0.75, 2, 0.75)));

    triangles.push_back(JPH::Triangle(JPH::Float3(1.75, 2, 0.75), JPH::Float3(1.75, 0, 0.75), JPH::Float3(1.25, 0, 0.75)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 0, 0.75), JPH::Float3(1.25, 2, 0.75), JPH::Float3(1.75, 2, 0.75)));
    
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 2, 0.75), JPH::Float3(-0.25, 2, 0.75), JPH::Float3(1.25, 1.75, 0.75)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 1.75, 0.75), JPH::Float3(-0.25, 1.75, 0.75), JPH::Float3(-0.25, 2, 0.75)));

    // top 
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 2, 0.25), JPH::Float3(-0.75, 2, 0.75), JPH::Float3(1.75, 2, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 2, 0.75), JPH::Float3(1.75, 2, 0.75), JPH::Float3(1.75, 2, 0.25)));

    // bottom
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 1.75, 0.25), JPH::Float3(-0.25, 1.75, 0.75), JPH::Float3(1.25, 1.75, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 1.75, 0.75), JPH::Float3(1.25, 1.75, 0.75), JPH::Float3(1.25, 1.75, 0.25)));

    // out sides
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 2, 0.25), JPH::Float3(-0.75, 2, 0.75), JPH::Float3(-0.75, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.75, 0, 0.75), JPH::Float3(-0.75, 2, 0.75), JPH::Float3(-0.75, 0, 0.25)));

    triangles.push_back(JPH::Triangle(JPH::Float3(1.75, 2, 0.25), JPH::Float3(1.75, 2, 0.75), JPH::Float3(1.75, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.75, 0, 0.75), JPH::Float3(1.75, 2, 0.75), JPH::Float3(1.75, 0, 0.25)));

    // in sides
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 1.75, 0.25), JPH::Float3(-0.25, 1.75, 0.75), JPH::Float3(-0.25, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(-0.25, 0, 0.75), JPH::Float3(-0.25, 1.75, 0.75), JPH::Float3(-0.25, 0, 0.25)));

    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 1.75, 0.25), JPH::Float3(1.25, 1.75, 0.75), JPH::Float3(1.25, 0, 0.25)));
    triangles.push_back(JPH::Triangle(JPH::Float3(1.25, 0, 0.75), JPH::Float3(1.25, 1.75, 0.75), JPH::Float3(1.25, 0, 0.25)));

    if (chunk.dirZ == 0)
    {
        for (size_t i = 0; i < triangles.size(); i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                float tmp = triangles[i].mV[j].x;
                triangles[i].mV[j].x = triangles[i].mV[j].z;
                triangles[i].mV[j].z = tmp;
            }
        }
    }

    if (highGate)
    {
        for (size_t i = 0; i < triangles.size(); i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                if (triangles[i].mV[j].y != 0)
                    triangles[i].mV[j].y += 1.1;
            }
        }
    }
    JPH::Shape::ShapeResult outResult;
    JPH::MeshShapeSettings gateSettings(triangles);
    JPH::BodyCreationSettings boxSettings(new JPH::MeshShape(gateSettings, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.updateColor = true;
    newTile.rotateOverTime = false;

    chunk.tiles.push_back(newTile);
}


void ChunkGenerator::GenerateBarrier(Chunk &chunk)
{
    if (chunk.levels[2] != lastChunk.levels[2] ||  chunk.type == ChunkType::TURN)
        return;

    Tile newTile;
    int height = 0;
    switch (chunk.levels[2])
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
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2 + 1) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2) * 2);
    else
        newTile.position = ml::vec3((chunk.x * chunkSize + chunkSize / 2) * 2, height * 2 + 1, (chunk.z * chunkSize + chunkSize / 2 + 1) * 2);
    newTile.size = ml::vec3(1, 1, 1);
    newTile.modelIndex = elements[ChunkElements::BARRIER];
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    JPH::Vec3 boxData(1.0f, 0.5f, 0.25f);
    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    if (chunk.dirZ == 0)
    {
        newTile.transform = newTile.transform * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
        float tmp = boxData.GetX();
        boxData.SetX(boxData.GetZ());
        boxData.SetZ(tmp);
    }

    ml::vec3 halfSize = newTile.size / 2.0f;
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(boxData), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.updateColor = false;
    newTile.rotateOverTime = false;

    chunk.tiles.push_back(newTile);
}