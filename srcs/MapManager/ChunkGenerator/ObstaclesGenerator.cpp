#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

void ChunkGenerator::GenerateObstacles(Chunk &chunk)
{
    int nbGates = 1;
    int nbSpikeRollers = 1;
    int nbBarriers = 1;

    size_t nbTiles = chunk.tiles.size();
    for (size_t i = 0; i < nbTiles; i++)
    {
        if (!(chunk.tiles[i].flag & TileFlag::GROUND_TILE))
            continue;

        if (nbGates > 0 && rand() % 100 > 70)
        {
            chunk.tiles.push_back(GenerateGate(ml::vec3(chunk.tiles[i].position.x * 2, chunk.tiles[i].position.y + 1, chunk.tiles[i].position.z * 2), chunk.dirZ, rand() % 2));
            nbGates--;
        }
        else if (nbSpikeRollers > 0 && rand() % 100 > 70)
        {
            chunk.tiles.push_back(GenerateSpikeRoller(ml::vec3(chunk.tiles[i].position.x * 2, chunk.tiles[i].position.y + 1, chunk.tiles[i].position.z * 2)));
            nbSpikeRollers--;
        }
        else if (nbBarriers > 0 && rand() % 100 > 70)
        {
            chunk.tiles.push_back(GenerateBarrier(ml::vec3(chunk.tiles[i].position.x * 2, chunk.tiles[i].position.y + 1, chunk.tiles[i].position.z * 2), chunk.dirZ));
            nbBarriers--;
        }
    }
}

Tile ChunkGenerator::GenerateSpikeRoller(const ml::vec3 &position)
{
    Tile newTile;
    newTile.position = position;
    newTile.size = ml::vec3(1, 1, 1);
    newTile.modelIndex = elements[ChunkElements::SPIKE_ROLLER];
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    ml::vec3 halfSize = newTile.size / 2.0f;
    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::BodyCreationSettings boxSettings(new JPH::CylinderShape(1.5, 0.5), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.flag = TileFlag::OBSTACLES + TileFlag::ROTATE_OVER_TIME;

    return (newTile);
}


Tile ChunkGenerator::GenerateGate(const ml::vec3 &position, int chunkDirZ, bool highGate)
{
    Tile newTile;
    newTile.position = position;
    newTile.size = ml::vec3(1, 1, 1);
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    ml::vec3 halfSize = newTile.size / 2.0f;

    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    JPH::TriangleList triangles = GetGateHitbox();
    if (chunkDirZ == 0)
    {
        newTile.transform = newTile.transform * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
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
        newTile.modelIndex = elements[ChunkElements::GATE_LARGE_BLUE];
        for (size_t i = 0; i < triangles.size(); i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                if (triangles[i].mV[j].y != 0)
                    triangles[i].mV[j].y += 1.1;
            }
        }
    }
    else
    {
        newTile.modelIndex = elements[ChunkElements::GATE_SMALL_BLUE];
    }

    JPH::Shape::ShapeResult outResult;
    JPH::MeshShapeSettings gateSettings(triangles);
    JPH::BodyCreationSettings boxSettings(new JPH::MeshShape(gateSettings, outResult), JPH::RVec3(positionTimeSize.x - halfSize.x, positionTimeSize.y, positionTimeSize.z - halfSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.flag = TileFlag::OBSTACLES + TileFlag::UPDATE_COLOR;

    return (newTile);
}

JPH::TriangleList ChunkGenerator::GetGateHitbox()
{

    static JPH::TriangleList triangles;
    
    if (triangles.size() == 0) // first time
    {
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
    }

    return (triangles);
}

Tile ChunkGenerator::GenerateBarrier(const ml::vec3 &position, int chunkDirZ)
{
    Tile newTile;
    newTile.position = position; 
    newTile.size = ml::vec3(1, 1, 1);
    newTile.modelIndex = elements[ChunkElements::BARRIER];
    ml::vec3 positionTimeSize = newTile.position * newTile.size;
    JPH::Vec3 boxData(1.0f, 0.5f, 0.25f);
    newTile.transform = ml::translate(ml::mat4(1.0f), positionTimeSize);
    if (chunkDirZ == 0)
    {
        newTile.transform = newTile.transform * ml::rotate(ml::mat4(1.0f), 90, ml::vec3(0, 1, 0));
        float tmp = boxData.GetX();
        boxData.SetX(boxData.GetZ());
        boxData.SetZ(tmp);
    }

    ml::vec3 halfSize = newTile.size / 2.0f;
    JPH::BodyCreationSettings boxSettings(new JPH::BoxShape(boxData), JPH::RVec3(positionTimeSize.x, positionTimeSize.y + halfSize.y, positionTimeSize.z), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    newTile.bodyId = WorldPhysic3D::GetBodyInterface().CreateAndAddBody(boxSettings, JPH::EActivation::DontActivate);
    newTile.flag = TileFlag::OBSTACLES;

    return (newTile);
}