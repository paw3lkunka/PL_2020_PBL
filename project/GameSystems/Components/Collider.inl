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

    /**
     * @brief Is collider trigger flag; 
     * true = not solid, collision triggers logic
     * false = solid collider, impassable
     */
    bool isTrigger;
};

#endif /* !COLLIDER_HPP_ */
