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
    // a = F / m
    // V = V0 + a * t
    // s = V * t + a * t^2 / 2
    glm::vec3 dragForce = 0.5f * rBodyPtr->drag * rBodyPtr->velocity * glm::abs(rBodyPtr->velocity);
    glm::vec3 force = rBodyPtr->force + dragForce;

    glm::vec3 acceleration =  force / rBodyPtr->mass;
    rBodyPtr->velocity += acceleration * Core::FIXED_TIME_STEP_F;    

//EXPERIMENT simplification
    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * glm::vec4(rBodyPtr->velocity, 0.0f));

/*
    glm::vec3 offset = rBodyPtr->velocity * Core::FIXED_TIME_STEP_F + acceleration * Core::FIXED_TIME_STEP_F * Core::FIXED_TIME_STEP_F / 2.0f;
    glm::vec4 offsetLocal = transformPtr->worldToLocalMatrix * glm::vec4(offset, 0.0f);
    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(offsetLocal);
*/
    rBodyPtr->force = {0.0f, 0.0f, 0.0f};
}