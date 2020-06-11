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

    ///@brief Determines body shape.
    enum Shape
    {
        BOX,
        SOLID_SPHERE,
        HOLLOW_SPHERE
    } shape = BOX;

    // ? unserialized

    ///@brief Velocity of the rigidbody,
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

    ///@brief Angular velocity of the rigidbody
    glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};

    ///@brief 
    std::vector<Impulse> impulses;

    void updateReactRB()    
    {
        if (angularDrag > 1.0f || angularDrag < 0.0f )
        {
            std::cerr << "ERROR: Angular drag of " << Name(this) << " is out of bounds. Value was clamped." << std::endl;
            angularDrag = std::clamp(angularDrag, 0.0f, 1.0f);
        }
        if (drag > 1.0f || drag < 0.0f )
        {
            std::cerr << "ERROR: Drag of " << Name(this) << " is out of bounds. Value was clamped." << std::endl;
            drag = std::clamp(drag, 0.0f, 1.0f);
        }
        
        reactRB->setType(type);
        
        if (type == rp3d::BodyType::DYNAMIC)
        {
            reactRB->setAngularDamping(angularDrag);
            reactRB->setLinearDamping(drag);           //HACK
            reactRB->setLocalInertiaTensor(BoxMomentOfInertia(mass, {1.0f, 1.0f, 1.0f}));
            reactRB->enableGravity(!ignoreGravity);
            reactRB->setMass(mass);
        }
    }


private:

    ///@brief Pointer to reactphysics3d object
    reactphysics3d::RigidBody* reactRB;
};

#endif