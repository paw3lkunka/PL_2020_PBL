#ifndef RIGIDBODY_INL_
#define RIGIDBODY_INL_

#include "Component.inl"

#include <glm/glm.hpp>
/**
 * @brief Rigidbody struct with all of its data and forces
 */
struct Rigidbody : public Component
{
    Rigidbody() = default;
    virtual ~Rigidbody() = default;

    /**
     * @brief Mass of the rigidbody
     */
    float mass;

    /**
     * @brief Drag of the rigidbody
     */
    float drag;

    /**
     * @brief Angular drag of the rigidbody
     */
    float angularDrag;

    bool ignoreGravity = false;

    /**
     * @brief Force of the rigidbody
     */
    glm::vec3 force;

    /**
     * @brief Torque of the rigidbody
     */
    glm::vec3 torque;

    /**
     * @brief Velocity of the rigidbody,
     */
    glm::vec3 velocity;

    /**
     * @brief Angular velocity of the rigidbody
     */
    glm::vec3 angularVelocity;
};

#endif