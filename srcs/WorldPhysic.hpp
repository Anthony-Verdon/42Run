#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
    public:
        BPLayerInterfaceImpl()
        {
            // Create a mapping table from object to broad phase layer
            mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
        }

        virtual uint GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
            return mObjectToBroadPhase[inLayer];
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch ((JPH::BroadPhaseLayer::Type)inLayer)
            {
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
                default:													JPH_ASSERT(false); return "INVALID";
            }
        }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
};

class MyContactListener : public JPH::ContactListener
{
    public:
        // See: ContactListener
        virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
        {
            (void)inBody1;
            (void)inBody2;
            (void)inBaseOffset;
            (void)inCollisionResult;
            std::cout << "Contact validate callback" << std::endl;

            // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            (void)inBody1;
            (void)inBody2;
            (void)inManifold;
            (void)ioSettings;
            std::cout << "A contact was added" << std::endl;
        }

        virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            (void)inBody1;
            (void)inBody2;
            (void)inManifold;
            (void)ioSettings;
            std::cout << "A contact was persisted" << std::endl;
        }

        virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
        {
            (void)inSubShapePair;
            std::cout << "A contact was removed" << std::endl;
        }
};

class MyBodyActivationListener : public JPH::BodyActivationListener
{
    public:
        virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
        {
            (void)inBodyID;
            (void)inBodyUserData;
            std::cout << "A body got activated" << std::endl;
        }

        virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
        {
            (void)inBodyID;
            (void)inBodyUserData;
            std::cout << "A body went to sleep" << std::endl;
        }
};

#include <Jolt/Renderer/DebugRenderer.h>

class DebugRendererImpl : public JPH::DebugRenderer
{
    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
    {
        std::cout << "DrawLine" << std::endl;
        (void)inFrom;
        (void)inTo;
        (void)inColor;
    }

    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off)
    {
        std::cout << "DrawTriangle" << std::endl;
        (void)inV1;
        (void)inV2;
        (void)inV3;
        (void)inColor;
        (void)inCastShadow;
    }

    virtual Batch CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount)
    {
        std::cout << "CreateTriangleBatch1" << std::endl;
        (void)inTriangles;
        (void)inTriangleCount;

        return (Batch());
    }

    virtual Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount)
    {
        std::cout << "CreateTriangleBatch2" << std::endl;
        (void)inVertices;
        (void)inVertexCount;
        (void)inIndices;
        (void)inIndexCount;

        return (Batch());
    }
    virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode = ECullMode::CullBackFace, ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid)
    {
        std::cout << "DrawGeometry" << std::endl;
        (void)inModelMatrix;
        (void)inWorldSpaceBounds;
        (void)inLODScaleSq;
        (void)inModelColor;
        (void)inGeometry;
        (void)inCullMode;
        (void)inCastShadow;
        (void)inDrawMode;
    }

    virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f)
    {
        std::cout << "DrawText3D" << std::endl;
        (void)inPosition;
        (void)inString;
        (void)inColor;
        (void)inHeight;
    }
};