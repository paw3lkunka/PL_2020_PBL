#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Component.inl"
#include "ComponentType.inl"


/**
 * @brief Colliders base struct
 */
struct Collider : public Component
{
    inline Collider() { type = ComponentType::COLLIDER; }
    virtual ~Collider() = default;

    enum class Type
    {
        DYNAMIC;
        KINEMATIC;
        TRIGGER;
    } type;

};

#endif /* !COLLIDER_HPP_ */
