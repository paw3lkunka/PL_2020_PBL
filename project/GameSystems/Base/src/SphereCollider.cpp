#include "SphereCollider.hpp"
#include "Core.hpp"

SphereCollider::~SphereCollider()
{    
    GetCore().physicsCommon.destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(reactCS));
}

void SphereCollider::computeReactCS()
{
    //TODO Scale?
    reactCS = GetCore().physicsCommon.createSphereShape(radius);
}