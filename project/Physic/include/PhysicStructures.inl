#ifndef PHYSIC_STRUCTURES_INL
#define PHYSIC_STRUCTURES_INL

#include <glm/glm.hpp>

class Rigidbody;
class Collider;

/**
 * @brief Data structure for trigger events
 * 
 */
struct TriggerData
{
    /// @brief Rigidbody component of object interacted with trigger.
    Rigidbody* causeBody;
    /// @brief Rigidbody component of trigger object.
    Rigidbody* triggerBody;
};

/**
 * @brief Data structure for collision events
 * 
 */
struct CollisionData
{
    /// @brief Rigidbody component of first colliding object.
    Rigidbody* body1;
    /// @brief Rigidbody component of second colliding object.
    Rigidbody* body2;
};

/**
 * @brief Structure encapsulating force it's point of application;
 * Forces are applied by kinematic system in only in one frame 
 */
struct Impulse
{
    ///@brief Determines type of impulse.
    enum Type
    {
        // Force is always applied to body's center of mass. Point parameter is ignored.
        CENTER_OF_MASS_FORCE,
        // Force is always applied to point defined in world space.
        WORLD_SPACE_FORCE,
        // Force is always applied to point defined in body's local space.
        LOCAL_SPACE_FORCE,
        // Force is interpreted as torque in x,y,z axes. Point parameter is ignored.
        TORQUE
    } type = CENTER_OF_MASS_FORCE;

    ///@brief Force in world space.
    glm::vec3 force = {0.0f, 0.0f, 0.0f};

    ///@brief Application point of force. Ignored for CENTER_OF_MASS_FORCE and TORQUE.
    glm::vec3 point = {0.0f, 0.0f, 0.0f};
};

#endif /*PHYSIC_STRUCTURES_INL*/