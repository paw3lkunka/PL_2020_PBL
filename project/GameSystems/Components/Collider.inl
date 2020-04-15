#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Component.inl"

/**
 * @brief Colliders base class
 */
class Collider : public Component
{
public:
    virtual ~Collider() = default;

    /**
     * @brief Is collider trigger flag; 
     * true = not solid, collision triggers logic
     * false = solid collider, impassable
     */
    bool isTrigger;

};

#endif /* !COLLIDER_HPP_ */
