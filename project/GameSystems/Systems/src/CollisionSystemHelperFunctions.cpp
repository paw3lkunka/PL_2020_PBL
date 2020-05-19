#include "CollisionSystemHelperFunctions.hpp"
#include <glm/gtc/constants.hpp>

glm::vec3 RelativeVelocity(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return collisionNormal * ((rBody1->velocity + glm::cross(rBody1->angularVelocity, r1)) - (rBody2->velocity + glm::cross(rBody2->angularVelocity, r2)));
}

glm::vec3 JImpulse(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return -(1 + glm::e<float>()) * RelativeVelocity(rBody1, rBody2, r1, r2, collisionNormal)
    /
    (
    //TODO I^-1 should be pre computed
        1 / rBody1->mass + 1 / rBody2->mass
        + collisionNormal * (glm::inverse(rBody1->momentOfInertia) * glm::cross(glm::cross(r1, collisionNormal), r1) )
        + collisionNormal * (glm::inverse(rBody2->momentOfInertia) * glm::cross(glm::cross(r2, collisionNormal), r2) )
    );
}