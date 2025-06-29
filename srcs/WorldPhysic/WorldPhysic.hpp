#pragma once

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <map>
#include <vector>

namespace Layers
{
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}; // namespace Layers

namespace BroadPhaseLayers
{
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr uint NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

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
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
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

struct Contact
{
    JPH::BodyID id;
    JPH::Vec3 normal;
};

class ContactListener : public JPH::ContactListener
{
  private:
    std::map<JPH::BodyID, std::vector<Contact>> contacts;

  public:
    virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
    {
        (void)inBody1;
        (void)inBody2;
        (void)inBaseOffset;
        (void)inCollisionResult;

        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
    {
        (void)ioSettings;

        {
            Contact newContact;
            newContact.id = inBody2.GetID();
            newContact.normal = inManifold.mWorldSpaceNormal;
            contacts[inBody1.GetID()].push_back(newContact);
        }
        {
            Contact newContact;
            newContact.id = inBody1.GetID();
            newContact.normal = -inManifold.mWorldSpaceNormal;
            contacts[inBody2.GetID()].push_back(newContact);
        }
    }

    virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
    {
        (void)inBody1;
        (void)inBody2;
        (void)inManifold;
        (void)ioSettings;
    }

    virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
    {
        {
            auto contactMapIt = contacts.find(inSubShapePair.GetBody1ID());
            if (contactMapIt != contacts.end())
            {
                for (auto contactVectorIt = contactMapIt->second.begin(); contactVectorIt != contactMapIt->second.end(); contactVectorIt++)
                {
                    if (contactVectorIt->id == inSubShapePair.GetBody2ID())
                    {
                        contactMapIt->second.erase(contactVectorIt);
                        break;
                    }
                }
            }
        }
        {
            auto contactMapIt = contacts.find(inSubShapePair.GetBody2ID());
            if (contactMapIt != contacts.end())
            {
                for (auto contactVectorIt = contactMapIt->second.begin(); contactVectorIt != contactMapIt->second.end(); contactVectorIt++)
                {
                    if (contactVectorIt->id == inSubShapePair.GetBody1ID())
                    {
                        contactMapIt->second.erase(contactVectorIt);
                        break;
                    }
                }
            }
        }
    }

    std::vector<Contact> GetContacts(const JPH::BodyID &id)
    {
        auto contactMapIt = contacts.find(id);
        if (contactMapIt != contacts.end())
            return (contactMapIt->second);
        else
            return (std::vector<Contact>{});
    }
};