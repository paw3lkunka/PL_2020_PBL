#ifndef KINEMATICSYSTEM_HPP_
#define KINEMATICSYSTEM_HPP_

#include "System.hpp"

#include <glm/glm.hpp>

class Transform;
class Rigidbody;

/**
 * @brief Calculate speed and position of rigidbody depending on forces, and current speed
 * 
 */
class PhysicSystem : public System
{
    public:
        static glm::vec3 G_CONST;

        PhysicSystem() = default;
        virtual ~PhysicSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        Transform* transformPtr;
        Rigidbody* rBodyPtr;
};

#endif /* !KINEMATICSYSTEM_HPP_ */