#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Component.hpp"


/**
 * @brief Colliders base struct
 */
struct Collider : public Component
{
    Collider() = default;
    virtual ~Collider() = default;

    /**
     * @brief Define type of collider depending on its behaviour. 
     */
    enum class Type
    {
        // Resolve all collisions with DYNAMICs and KINEMATICs
        DYNAMIC,
        // Don't resolve own collisions, but DYNAMICs can collide with it.
        KINEMATIC,
        // Never resolve collisions, generate trigger events.
        TRIGGER,
    } type = Type::DYNAMIC;
};

#endif /* !COLLIDER_HPP_ */
