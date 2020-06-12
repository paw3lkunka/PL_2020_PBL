#ifndef KINEMATICSYSTEM_HPP_
#define KINEMATICSYSTEM_HPP_

#include "System.hpp"
#include "ComponentsPreDeclarations.hxx"

#include <glm/glm.hpp>

class Impulse;

/**
 * @brief Calculate speed and position of rigidbody depending on forces, and current speed
 */
class PhysicSystem : public System
{
    public:
        PhysicSystem() = default;
        virtual ~PhysicSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();
        virtual void start();

    private:
        /// @brief Pointer to Transform component - REQUIRED.
        Transform* transformPtr;
        /// @brief Pointer to Rigidbody component - REQUIRED.
        Rigidbody* rBodyPtr;
        /// @brief Pointer to Collider component - OPTIONAL.
        Collider* colliderPtr;
};

#endif /* !KINEMATICSYSTEM_HPP_ */
