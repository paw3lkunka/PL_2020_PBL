#ifndef RIGIDBODY_INL_
#define RIGIDBODY_INL_

#include "Component.hpp"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>

#include "PhisicStructures.inl"
#include "MomentOfInertia.hpp"

/**
 * @brief Rigidbody struct with all of its data and forces
 */
struct Rigidbody : public Component
{
    friend class PhysicSystem;
    //HACK
    friend class Core;

    Rigidbody() = default;
    virtual ~Rigidbody() = default;

    // ? serialized

    ///@brief Mass of the rigidbody
    float mass = 1;

    ///@brief Drag of the rigidbody
    float drag = 1;

    ///@brief Angular drag of the rigidbody
    float angularDrag = 1;

    ///@brief determines, if gravity affects this body
    bool ignoreGravity = false;

    //TODO documentation, serialization, mental castration
    reactphysics3d::BodyType type;

    // ? unserialized

    ///@brief Velocity of the rigidbody,
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

    ///@brief Angular velocity of the rigidbody
    glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};

    std::vector<Impulse> impulses;

private:

    ///@brief Pointer to reactphysics3d object
    reactphysics3d::RigidBody* reactRB;
};

#endif