#include "KinematicSystem.hpp"

#include <glm/glm.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Components.inc"

bool KinematicSystem::assertEntity(Entity* entity)
{
    transformPtr = entity->getComponentPtr<Transform>();
    rBodyPtr = entity->getComponentPtr<Rigidbody>();
    return transformPtr && rBodyPtr;
}

#include <iostream>

void KinematicSystem::fixedUpdate()
{
    glm::vec3 force = {0,0,0};
    glm::vec3 torque = {0,0,0};

    for(Impulse& impuse : rBodyPtr->impulses)
    {
        force += impuse.force;
        torque += glm::cross(impuse.point, impuse.force);
    }

    glm::vec3 dragForce = 0.5f * rBodyPtr->drag * rBodyPtr->velocity * glm::abs(rBodyPtr->velocity);
    force += dragForce;

    glm::vec3 acceleration = force / rBodyPtr->mass;
    rBodyPtr->velocity += acceleration * Core::FIXED_TIME_STEP_F;    

    glm::vec3 dragTorque = 0.5f * rBodyPtr->angularDrag * rBodyPtr->angularVelocity * glm::abs(rBodyPtr->angularDrag);
    torque += dragTorque;

    glm::vec3 angularAcceleration = rBodyPtr->momentOfInertia * torque;
    rBodyPtr->angularVelocity += angularAcceleration * Core::FIXED_TIME_STEP_F;

    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * glm::vec4(rBodyPtr->velocity, 0.0f));
    transformPtr->getLocalRotationModifiable() = glm::quat(rBodyPtr->angularVelocity) * transformPtr->getLocalRotation();

    std::cout << "Velocity: " << rBodyPtr->velocity.x << ", " << rBodyPtr->velocity.x << ", " << rBodyPtr->velocity.x << "\n" ;
    std::cout << "Position: " << transformPtr->getLocalPosition().x << ", " << transformPtr->getLocalPosition().x << ", " << transformPtr->getLocalPosition().x << "\n" ;
    std::cout << "Velocity: " << rBodyPtr->velocity.x << ", " << rBodyPtr->velocity.x << ", " << rBodyPtr->velocity.x << "\n" ;

    rBodyPtr->impulses.clear();
}