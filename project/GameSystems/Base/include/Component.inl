#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

#include "ISerializable.inl"

class Entity;

/**
 * @brief Base struct for all ECS data structures, should not contain any logic
 */
struct Component : public ISerializable
{
    Component() = default;
    virtual ~Component() = default;
    /**
     * @brief points on this components Entity - shouldn't be assinging manually
     */
    Entity* entityPtr;
};

#endif /* !COMPONENT_HPP_ */
