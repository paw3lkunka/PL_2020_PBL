#include "PhysicSystem.hpp"

#include <glm/glm.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Components.inc"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vec_swizzle.hpp> 

glm::vec3 PhysicSystem::G_CONST = {0.0f, 9.80665f, 0.0f};

bool PhysicSystem::assertEntity(Entity* entity)
{
    transformPtr = entity->getComponentPtr<Transform>();
    rBodyPtr = entity->getComponentPtr<Rigidbody>();
    colliderPtr = entity->getComponentPtr<Collider>();
    return transformPtr && rBodyPtr;
}

void PhysicSystem::fixedUpdate()
{
    force = {0,0,0};
    torque = {0,0,0};

    if( auto* sphere = dynamic_cast<SphereCollider*>(colliderPtr) )
    {
        for (Impulse& impulse : rBodyPtr->impulses)
            applyImpulse(impulse, sphere);
    }
    else if( auto* box = dynamic_cast<BoxCollider*>(colliderPtr) )
    {
        for (Impulse& impulse : rBodyPtr->impulses)
            applyImpulse(impulse, box, transformPtr);
    }
    else
    {
        for (Impulse& impulse : rBodyPtr->impulses)
            applyImpulse(impulse);
    }

    if (!rBodyPtr->ignoreGravity)
    {
        force += -rBodyPtr->mass * G_CONST;
    }

    glm::vec3 acceleration = force / rBodyPtr->mass;
    rBodyPtr->velocity += acceleration * Core::FIXED_TIME_STEP_F;

    glm::vec3 angularAcceleration = rBodyPtr->momentOfInertia * torque;
    rBodyPtr->angularVelocity += angularAcceleration * Core::FIXED_TIME_STEP_F;

    rBodyPtr->velocity *= glm::exp(-rBodyPtr->drag * Core::FIXED_TIME_STEP_F);
    rBodyPtr->angularVelocity *= glm::exp(-rBodyPtr->angularDrag * Core::FIXED_TIME_STEP_F);

    //TODO VELOCITY W AUDIO

    //TODO czy skala nie zniszczy efektu?
    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->toParentMatrix * glm::vec4(rBodyPtr->velocity, 0.0f));
    transformPtr->getLocalRotationModifiable() = glm::quat(rBodyPtr->angularVelocity) * transformPtr->getLocalRotation();

    rBodyPtr->impulses.clear();
}

void PhysicSystem::applyImpulse(Impulse impulse, SphereCollider* collider)
{
    //TODO check this
    force += impulse.force * collider->radius / (collider->radius + glm::length(impulse.point));
    torque += glm::cross(impulse.point, impulse.force);
}

void PhysicSystem::applyImpulse(Impulse impulse, BoxCollider* collider, Transform* transform)
{
    glm::vec3 msForce = transform->toModelMatrix * glm::vec4(impulse.force, 0.0f);
    msForce = msForce * collider->halfSize / (collider->halfSize + impulse.point);
    //TODO check this
    force += glm::xyz(transform->modelMatrix * glm::vec4(msForce, 0.0f));
    torque += glm::cross(impulse.point, impulse.force);
}

void PhysicSystem::applyImpulse(Impulse impulse)
{
    force += impulse.force;
}