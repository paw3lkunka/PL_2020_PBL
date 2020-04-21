#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

class Entity;

/**
 * @brief Base struct for all ECS data structures, should not contain any logic
 */
struct Component
{
    Component() = default;
    virtual ~Component() = default;
    /**
     * @brief points on this components Entity - shouldn't be assinging manually
     */
    Entity* entityPtr;
};

#endif /* !COMPONENT_HPP_ */
