#ifndef RIGIDBODY_INL_
#define RIGIDBODY_INL_

#include "Component.inl"

#include <vector>
#include <glm/glm.hpp>

#include "PhisicStructures.inl"
#include "MomentOfInertia.hpp"

/**
 * @brief Rigidbody struct with all of its data and forces
 */
struct Rigidbody : public Component
{
    Rigidbody() = default;
    virtual ~Rigidbody() = default;

    // ! serialized

    ///@brief Mass of the rigidbody
    float mass;

    ///@brief Moment of interia of Rigidbody
    glm::mat3 momentOfInertia;

    ///@brief Drag of the rigidbody
    float drag;

    ///@brief Angular drag of the rigidbody
    float angularDrag;

    ///@brief determines, if gravity affects this body
    bool ignoreGravity = false;

    // ! unserialized

    ///@brief Stores all impulses - cleared on the end of frame
    std::vector<Impulse> impulses;

    ///@brief Velocity of the rigidbody,
    glm::vec3 velocity;

    ///@brief Angular velocity of the rigidbody
    glm::vec3 angularVelocity;
};

#endif