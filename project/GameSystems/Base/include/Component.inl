#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

#include "ComponentType.inl"

class Entity;

/**
 * @brief Base struct for all ECS data structures, should not contain any logic
 */
struct Component
{
    inline Component() : type(ComponentType::COMPONENT) {}
    virtual ~Component() = default;
    /**
     * @brief points on this components Entity - shouldn't be assinging manually
     */
    Entity* entityPtr;
    ComponentType type;
};

#endif /* !COMPONENT_HPP_ */
