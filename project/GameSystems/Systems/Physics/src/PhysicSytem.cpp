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
    

    colliderPtr->computeReactCS();
    rp3d::Transform reactTc(Vec3Cast(colliderPtr->center), rp3d::Quaternion::identity());
    rBodyPtr->reactRB->addCollider(colliderPtr->reactCS, reactTc);
    rBodyPtr->updateReactRB();
}

void PhysicSystem::fixedUpdate()
{
    rp3d::Transform t;
    t.setPosition(Vec3Cast(transformPtr->getModelMatrix()[3]));
    t.setOrientation(QuatCast(transformPtr->getWorldRotation()));
    rBodyPtr->reactRB->setTransform(t);

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
        std::cout << "impulse" << std::endl;
    }
    rBodyPtr->impulses.clear();
}
