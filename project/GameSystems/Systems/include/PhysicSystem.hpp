#ifndef KINEMATICSYSTEM_HPP_
#define KINEMATICSYSTEM_HPP_

#include "System.hpp"
#include "ComponentsPreDeclarations.hxx"

#include <glm/glm.hpp>

class Impulse;

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
    //TODO DOCUMENTATION
        Transform* transformPtr;
    //TODO DOCUMENTATION
        Rigidbody* rBodyPtr;
    //TODO DOCUMENTATION
        Collider* colliderPtr;

    //TODO DOCUMENTATION
        glm::vec3 force;
    //TODO DOCUMENTATION
        glm::vec3 torque;

    //TODO DOCUMENTATION
        void applyImpulse(Impulse impulse, SphereCollider* collider);
    //TODO DOCUMENTATION
        void applyImpulse(Impulse impulse, BoxCollider* collider, Transform* transform);
    //TODO DOCUMENTATION
        void applyImpulse(Impulse impulse);
};

#endif /* !KINEMATICSYSTEM_HPP_ */
