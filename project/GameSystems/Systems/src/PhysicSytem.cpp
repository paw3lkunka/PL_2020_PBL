#include "PhysicSystem.hpp"

#include <glm/glm.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Components.inc"

#include <glm/gtx/string_cast.hpp>

glm::vec3 PhysicSystem::G_CONST = {0.0f, 9.80665f, 0.0f};

bool PhysicSystem::assertEntity(Entity* entity)
{
    transformPtr = entity->getComponentPtr<Transform>();
    rBodyPtr = entity->getComponentPtr<Rigidbody>();
    return transformPtr && rBodyPtr;
}

void PhysicSystem::fixedUpdate()
{
    glm::vec3 force = {0,0,0};
    glm::vec3 torque = {0,0,0};

    for (Impulse& impuse : rBodyPtr->impulses)
    {
        force += impuse.force;
        torque += glm::cross(impuse.point, impuse.force);
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

    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * glm::vec4(rBodyPtr->velocity, 0.0f));
    transformPtr->getLocalRotationModifiable() = glm::quat(rBodyPtr->angularVelocity) * transformPtr->getLocalRotation();

    rBodyPtr->impulses.clear();
}