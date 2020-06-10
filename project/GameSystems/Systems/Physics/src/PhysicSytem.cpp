#include "PhysicSystem.hpp"

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "Core.hpp"
#include "Utils.hpp"
#include "Entity.hpp"
#include "Components.inc"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vec_swizzle.hpp> 

bool PhysicSystem::assertEntity(Entity* entity)
{
    transformPtr = entity->getComponentPtr<Transform>();
    rBodyPtr = entity->getComponentPtr<Rigidbody>();
    colliderPtr = entity->getComponentPtr<Collider>();
    return transformPtr && rBodyPtr && colliderPtr;
}

void PhysicSystem::start()
{
    
    //Create react RB
    rp3d::Transform reactTrb;
    reactTrb.setPosition(Vec3Cast(transformPtr->getModelMatrix()[3]));
    reactTrb.setOrientation(QuatCast(transformPtr->getWorldRotation()));
    rBodyPtr->reactRB = GetCore().GetPhysicsWorld()->createRigidBody(reactTrb);
    //Should it be method?

    
    if (rBodyPtr->angularDrag > 1.0f || rBodyPtr->angularDrag < 0.0f )
    {
        std::cerr << "ERROR: Angular drag of " << Name(rBodyPtr) << " is out of bounds. Value was clamped.";
        rBodyPtr->angularDrag = std::clamp(rBodyPtr->angularDrag, 0.0f, 1.0f);
    }
    if (rBodyPtr->drag > 1.0f || rBodyPtr->drag < 0.0f )
    {
        std::cerr << "ERROR: Drag of " << Name(rBodyPtr) << " is out of bounds. Value was clamped.";
        rBodyPtr->drag = std::clamp(rBodyPtr->drag, 0.0f, 1.0f);
    }

    rBodyPtr->reactRB->setMass(rBodyPtr->mass);
    rBodyPtr->reactRB->setAngularDamping(rBodyPtr->angularDrag);
    rBodyPtr->reactRB->setLinearDamping(rBodyPtr->drag);           //HACK
    rBodyPtr->reactRB->setLocalInertiaTensor(BoxMomentOfInertia(rBodyPtr->mass, {1.0f, 1.0f, 1.0f}));
    rBodyPtr->reactRB->enableGravity(!rBodyPtr->ignoreGravity);

    colliderPtr->computeReactCS();
    rp3d::Transform reactTc(Vec3Cast(colliderPtr->center), rp3d::Quaternion::identity());
    rBodyPtr->reactRB->addCollider(colliderPtr->reactCS, reactTc);

    //if (Name(rBodyPtr) == "PhysicSurface")
        //rBodyPtr->reactRB->setType(rp3d::BodyType::STATIC);
    //else
    {
    }
    
}

void PhysicSystem::fixedUpdate()
{
    rp3d::Transform t;
    t.setPosition(Vec3Cast(transformPtr->getModelMatrix()[3]));
    t.setOrientation(QuatCast(transformPtr->getWorldRotation()));
    rBodyPtr->reactRB->setTransform(t);

    for (Impulse& i : rBodyPtr->impulses)
    {
        rBodyPtr->reactRB->applyForceAtWorldPosition(Vec3Cast(i.force), Vec3Cast(i.point));
        std::cout << "impulse" << std::endl;
    }
    rBodyPtr->impulses.clear();
}
