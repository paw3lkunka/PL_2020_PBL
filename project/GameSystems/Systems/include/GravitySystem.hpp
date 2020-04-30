#ifndef GRAVITYSYSTEM_HPP_
#define GRAVITYSYSTEM_HPP_

#include <glm/glm.hpp>

#include "System.hpp"

class Rigidbody;

/**
 * @brief Calculate gravity force for rigidbody 
 */
class GravitySystem : public System
{
    public:
        static glm::vec3 G_CONST;
        GravitySystem() = default;
        virtual ~GravitySystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        Rigidbody* rBodyPtr;
};

#endif /* !GRAVITYSYSTEM_HPP_ */
