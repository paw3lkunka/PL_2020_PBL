#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

#include "ISerializable.inl"

#include <string>

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

//inline const std::string __attribute__((always_inline)) Name(Component& c) { return c.entityPtr->getName(); }
//inline const std::string __attribute__((always_inline)) Name(Component* c) { return c->entityPtr->getName(); }


#endif /* !COMPONENT_HPP_ */
