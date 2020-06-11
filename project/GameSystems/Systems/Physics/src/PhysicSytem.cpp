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
    rp3d::Transform reactTrb
    (
        Vec3Cast(transformPtr->getModelMatrix()[3]),
        QuatCast(transformPtr->getWorldRotation())
    );
    rBodyPtr->reactRB = GetCore().physicModule.GetWorld().createRigidBody(reactTrb);
    
    colliderPtr->computeReactCS();

    rp3d::Transform reactTc
    (
        Vec3Cast(colliderPtr->center),
        rp3d::Quaternion::identity()
    );

    colliderPtr->reactCollider = rBodyPtr->reactRB->addCollider(colliderPtr->reactShape, reactTc);
    colliderPtr->reactCollider->setIsTrigger(colliderPtr->isTrigger);

    rBodyPtr->updateReactRB(true);
}

void PhysicSystem::fixedUpdate()
{
    rBodyPtr->updateReactTransform(transformPtr);

    for (Impulse& i : rBodyPtr->impulses)
    {
        switch (i.type)
        {
        case Impulse::CENTER_OF_MASS_FORCE:
            rBodyPtr->reactRB->applyForceToCenterOfMass(Vec3Cast(i.force));
            break;

        case Impulse::WORLD_SPACE_FORCE:
            rBodyPtr->reactRB->applyForceAtWorldPosition(Vec3Cast(i.force), Vec3Cast(i.point));
            break;

        case Impulse::LOCAL_SPACE_FORCE:
            rBodyPtr->reactRB->applyForceAtLocalPosition(Vec3Cast(i.force), Vec3Cast(i.point));
            break;

        case Impulse::TORQUE:
            rBodyPtr->reactRB->applyTorque(Vec3Cast(i.force));
            break;
        }
    }
    rBodyPtr->impulses.clear();
}
