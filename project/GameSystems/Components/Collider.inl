#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Component.inl"

/**
 * @brief Colliders base struct
 */
struct Collider : public Component
{
    virtual ~Collider() = default;

    /**
     * @brief Is collider trigger flag; 
     * true = not solid, collision triggers logic
     * false = solid collider, impassable
     */
    bool isTrigger;
};

#endif /* !COLLIDER_HPP_ */
