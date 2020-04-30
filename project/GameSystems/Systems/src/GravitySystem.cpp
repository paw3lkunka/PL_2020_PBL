#include "GravitySystem.hpp"

#include "Entity.hpp"
#include "Components.inc"

glm::vec3 GravitySystem::G_CONST = {0.0f, 9.80665f, 0.0f};

bool GravitySystem::assertEntity(Entity* entity)
{
    rBodyPtr = entity->getComponentPtr<Rigidbody>();
    return (rBodyPtr != nullptr && rBodyPtr->ignoreGravity == false) ? true : false;
}

void GravitySystem::fixedUpdate()
{
    rBodyPtr->force -= rBodyPtr->mass * G_CONST;
}