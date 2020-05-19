#ifndef COLLISION_SYSTEM_HELPER_FUNCTIONS
#define COLLISION_SYSTEM_HELPER_FUNCTIONS

#include <glm/glm.hpp>

#include "Rigidbody.inl"

//http://jonathanbosson.github.io/reports/TNM085_group5.pdf

/**
 * @brief Return relative velocity between the two colliding points of the object.
 * 
 * @param rBody1 first colliding object
 * @param rBody2 second colliding object
 * @param r1 vector from first object centre to collision point
 * @param r2 vector from first object centre to collision point
 * @param collisionNormal normal vector of collision
 * @return glm::vec3 relative velocity between the two colliding points
 */
glm::vec3 RelativeVelocity(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return collisionNormal * ((rBody1->velocity + glm::cross(rBody1->angularVelocity, r1)) - (rBody2->velocity + glm::cross(rBody2->angularVelocity, r2)));
}
/**
 * @brief Calculate j Impuls, details here: http://jonathanbosson.github.io/reports/TNM085_group5.pdf
 * 
 * @param rBody1 first colliding object
 * @param rBody2 second colliding object
 * @param r1 vector from first object centre to collision point
 * @param r2 vector from first object centre to collision point
 * @param collisionNormal normal vector of collision
 * @return glm::vec3 relative velocity between the two colliding points
 */
glm::vec3 JImpulse(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return -(1 + glm::e<float>()) * RelativeVelocity(rBody1, rBody2, r1, r2, collisionNormal)
    /
    (
    //TODO I^-1 chould be pre computed
        1 / rBody1->mass + 1 / rBody2->mass
        + collisionNormal * (glm::inverse(rBody1->momentOfInertia) * glm::cross(glm::cross(r1, collisionNormal), r1) )
        + collisionNormal * (glm::inverse(rBody2->momentOfInertia) * glm::cross(glm::cross(r2, collisionNormal), r2) )
    );
}

#endif