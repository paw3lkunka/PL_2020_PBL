#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

class Entity;

/**
 * @brief Base class for all ECS data structures, should not contain any logic
 */
class Component
{
    public:
        virtual ~Component() = default;
        /**
         * @brief points on this components Entity - shouldn't be assinging manually
         */
        Entity* entityPtr;
    protected:
    private:
};

#endif /* !COMPONENT_HPP_ */
