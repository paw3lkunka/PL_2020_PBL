#include "SphereCollider.hpp"
#include "Core.hpp"
#include "MomentOfInertia.hpp"

SphereCollider::~SphereCollider()
{    
    GetCore().physicModule.GetCommon().destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(reactShape));
}

void SphereCollider::computeReactCS()
{
    reactShape = GetCore().physicModule.GetCommon().createSphereShape(radius);
}

rp3d::Vector3 SphereCollider::computeInnertiaTensor(float mass)
{
    return SphereMomentOfInertia(mass, radius, isHollow);
}