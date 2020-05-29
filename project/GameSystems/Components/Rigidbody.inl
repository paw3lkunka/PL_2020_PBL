#ifndef RIGIDBODY_INL_
#define RIGIDBODY_INL_

#include "Component.hpp"

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

    // ? serialized

    ///@brief Mass of the rigidbody
    float mass = 1;

    ///@brief Moment of interia of Rigidbody
    glm::mat3 momentOfInertia = glm::mat3(0);

    ///@brief Drag of the rigidbody
    float drag = 1;

    ///@brief Angular drag of the rigidbody
    float angularDrag = 1;

    ///@brief determines, if gravity affects this body
    bool ignoreGravity = false;

    // ? unserialized

    ///@brief Inverted moment of interia of Rigidbody
    glm::mat3 invertedMomentOfInertia = glm::mat3(0);

    ///@brief Stores all impulses - cleared on the end of frame
    std::vector<Impulse> impulses;

    ///@brief Velocity of the rigidbody,
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

    ///@brief Angular velocity of the rigidbody
    glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};
};

#endif