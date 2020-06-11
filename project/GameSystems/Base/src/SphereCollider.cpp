#include "SphereCollider.hpp"
#include "Core.hpp"
#include "MomentOfInertia.hpp"

SphereCollider::~SphereCollider()
{    
    GetCore().physicsCommon.destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(reactCS));
}

void SphereCollider::computeReactCS()
{
    reactCS = GetCore().physicsCommon.createSphereShape(radius);
}

rp3d::Vector3 SphereCollider::computeInnertiaTensor(float mass)
{
    return SphereMomentOfInertia(mass, radius, isHollow);
}