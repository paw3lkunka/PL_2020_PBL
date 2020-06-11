#include "ECS.inc"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>

#include "Utils.hpp"
#include "PhisicStructures.inl"

void Rigidbody::updateReactRB(bool updateInnertiaTensor)    
{
    if (angularDrag > 1.0f || angularDrag < 0.0f )
    {
        std::cerr << "ERROR: Angular drag of " << Name(this) << " is out of bounds. Value was clamped." << std::endl;
        angularDrag = std::clamp(angularDrag, 0.0f, 1.0f);
    }
    if (drag > 1.0f || drag < 0.0f )
    {
        std::cerr << "ERROR: Drag of " << Name(this) << " is out of bounds. Value was clamped." << std::endl;
        drag = std::clamp(drag, 0.0f, 1.0f);
    }
    
    reactRB->setType(type);
    
    if (type == rp3d::BodyType::DYNAMIC)
    {
        reactRB->setAngularDamping(angularDrag);
        reactRB->setLinearDamping(drag);
        reactRB->enableGravity(!ignoreGravity);
        reactRB->setMass(mass);
        
        if (auto* collider = (entityPtr ? entityPtr->getComponentPtr<Collider>() : nullptr))
        {
            reactRB->setLocalInertiaTensor(collider->computeInnertiaTensor(mass));
        }
    }
}

void Rigidbody::updateReactTransform(Transform* transformPtr)
{
    rp3d::Transform rp3dTransform;
    rp3dTransform.setPosition(Vec3Cast(transformPtr->getModelMatrix()[3]));
    rp3dTransform.setOrientation(QuatCast(transformPtr->getWorldRotation()));
    reactRB->setTransform(rp3dTransform);
}