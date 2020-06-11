#ifndef RIGIDBODY_INL_
#define RIGIDBODY_INL_

#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>

#include "Core.hpp"
#include "Utils.hpp"
#include "PhysicStructures.inl"

/**
 * @brief Rigidbody struct with all of its data and forces
 */
struct Rigidbody : public Component
{
    friend class PhysicSystem;
    friend class PhysicModule;

    Rigidbody() = default;
    virtual ~Rigidbody() = default;

    // ? serialized

    ///@brief Mass of the rigidbody.
    float mass = 1;

    ///@brief Drag of the rigidbody.
    float drag = 1;

    ///@brief Angular drag of the rigidbody.
    float angularDrag = 1;

    ///@brief Determines, if gravity affects this body.
    bool ignoreGravity = false;

    ///@brief Type of rigidbody (Static / Cinematic / Dynamic).
    rp3d::BodyType type = rp3d::BodyType::DYNAMIC;

    // ? unserialized

    ///@brief Velocity of the rigidbody,
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

    ///@brief Angular velocity of the rigidbody
    glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};

    ///@brief list of all impulses applied in this time step;
    std::vector<Impulse> impulses;

    /**
     * @brief Update state of react RigidBody.
     * 
     * @param updateInnertiaTensor update also inntertia tensor. To do this, this component must be assigned to entity along with Collider component.
     */
    void updateReactRB(bool updateInnertiaTensor);

    /**
     * @brief Update state of react Transform, applying valuse from given Transform.
     * 
     * @param transformPtr transform with values to apply.
     */
    void updateReactTransform(Transform* transformPtr);
    
private:

    ///@brief Pointer to reactphysics3d object
    reactphysics3d::RigidBody* reactRB;
};

#endif