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
        static glm::vec3 G_CONST;

        PhysicSystem() = default;
        virtual ~PhysicSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:
        /// @brief Pointer to Transform component - REQUIRED.
        Transform* transformPtr;
        /// @brief Pointer to Rigidbody component - REQUIRED.
        Rigidbody* rBodyPtr;
        /// @brief Pointer to Collider component - OPTIONAL.
        Collider* colliderPtr;

        /// @brief Force acting on object in this iteration.
        glm::vec3 force;
        /// @brief Torque acting on object in this iteration.
        glm::vec3 torque;

        /**
         * @brief Apply impulse in appropiate way to spherical object.
         *
         * @param impulse - Impulse to apply.
         * @param collider - Collider object.
         */
        void applyImpulse(Impulse impulse, SphereCollider* collider);

        /**
         * @brief Apply impulse in appropiate way to cuboidal object.
         *
         * @param impulse - Impulse to apply.
         * @param collider - Collider object.
         * @param transform - Transformation of object.
         */
        void applyImpulse(Impulse impulse, BoxCollider* collider, Transform* transform);

        /**
         * @brief Apply impulse in appropiate way to material point.
         *
         * @param impulse - Impulse to apply.
         */
        void applyImpulse(Impulse impulse);
};

#endif /* !KINEMATICSYSTEM_HPP_ */
