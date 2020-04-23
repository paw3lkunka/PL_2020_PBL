#ifndef COLLISIONDATASTRUCTURES_INL_
#define COLLISIONDATASTRUCTURES_INL_

#include "Collider.inl"
#include <glm/glm.hpp>

/**
 * @brief Data structure for collision event
 * 
 */
struct __attribute__((packed)) CollisionData
{
    /// @brief Collider for which collision was resolved
    Collider* cause;
    /// @brief Collider with which collision was resolved
    Collider* target;
    /// @brief Separation vector for 'cause' collider
    glm::vec3 separation;
};

/**
 * @brief Data structure for trigger events
 * 
 */
struct TriggerData
{
    /// @brief Collider which interacted with trigger
    Collider* cause;
    /// @brief Trigger
    Collider* trigger;
};

#endif /* !COLLISIONDATASTRUCTURES_INL_ */