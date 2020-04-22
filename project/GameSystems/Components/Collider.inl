#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Component.inl"


/**
 * @brief Colliders base struct
 */
struct Collider : public Component
{
    Collider() = default;
    virtual ~Collider() = default;

    enum class Type
    {
        DYNAMIC,
        KINEMATIC,
        TRIGGER,
    } type;

};

#endif /* !COLLIDER_HPP_ */
