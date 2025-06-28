#include "MapManager/ChunkGenerator/ChunkGenerator.hpp"
#include "WorldPhysic/WorldPhysic.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

void ChunkGenerator::GenerateObstacles(Chunk &chunk)
{
    for (int i = 0; i < LaneType::COUNT; i++)
        chunk.lanes[i].hasSpikeRoller = false;
    for (int i = 0; i < LaneType::COUNT; i++)
    {
        Lane &lane = chunk.lanes[i];

        // determine position
        int pos;
        switch (lane.level)
        {
        case TOP:
        case GROUND:
        case BOTTOM:
            pos = rand() % (GetChunkSize() - 2) + 1;
            break;
        case GOING_DOWN_TO_GROUND:
        case GOING_TO_TOP:
        case GOING_TO_BOTTOM:
        case GOING_UP_TO_GROUND:
            pos = GetHalfChunkSize();
            break;
        }
        ml::vec3 obstaclePos = ml::vec3(lane.tiles[pos].position.x * 2, lane.tiles[pos].position.y + 1, lane.tiles[pos].position.z * 2);

        // check if we can spawn or not a spike roller
        bool spikeRollerCanSpawn;
        switch (i)
        {
        case 0:
            spikeRollerCanSpawn = (CanGoToLane(lane, chunk.lanes[i + 1]) && !chunk.lanes[i + 1].hasSpikeRoller);
            break;
        case LaneType::COUNT - 1:
            spikeRollerCanSpawn = (CanGoToLane(lane, chunk.lanes[i - 1]) && !chunk.lanes[i - 1].hasSpikeRoller);
            break;
        default:
            spikeRollerCanSpawn = ((CanGoToLane(lane, chunk.lanes[i - 1]) && !chunk.lanes[i - 1].hasSpikeRoller) || (CanGoToLane(lane, chunk.lanes[i + 1]) && !chunk.lanes[i + 1].hasSpikeRoller));
            break;
        }

        // spawn obstacle
        int obstacle = (spikeRollerCanSpawn ? rand() % 3 : rand() % 2 + 1);
        switch (obstacle)
        {
        case 0:
            lane.tiles.push_back(GenerateSpikeRoller(obstaclePos));
            lane.hasSpikeRoller = true;
            break;
        case 1:
            lane.tiles.push_back(GenerateGate(obstaclePos, chunk.dirZ, rand() % 2));
            break;
        case 2:
            lane.tiles.push_back(GenerateBarrier(obstaclePos, chunk.dirZ));
            break;
        }
    }
}

bool ChunkGenerator::CanGoToLane(const Lane &currentLane, const Lane &futureLane)
{
    return (CanGoToLane(currentLane.level, futureLane.level));
}

// return the possibility to go to another lane depending of it level at the start of the chunk
// without taking into account potential obstacles
bool ChunkGenerator::CanGoToLane(Level currentLevel, Level futureLevel)
{
    switch (currentLevel)
    {
    case Level::TOP:
    case Level::GOING_DOWN_TO_GROUND:
        return true; // futureLevel >= Level::TOP always true
    case Level::GOING_TO_TOP:
    case Level::GROUND:
    case Level::GOING_TO_BOTTOM:
        return (futureLevel >= Level::GOING_TO_TOP);
    case Level::GOING_UP_TO_GROUND:
    case Level::BOTTOM:
        return (futureLevel >= Level::GOING_UP_TO_GROUND);
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