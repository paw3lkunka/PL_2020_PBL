#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include <reactphysics3d/reactphysics3d.h>
#include <glm/glm.hpp>

#include "Component.hpp"


/**
 * @brief Colliders base struct
 */

struct Collider : public Component
{    
    /**
     * @brief Center of the collider in local space.
     */
    glm::vec3 center = {0.0f, 0.0f, 0.0f};

    /**
     * @brief Determine if collider is trigger.
     */
    bool isTrigger = false;

    /**
     * @brief Create CollisionShape depending on type and parameters.
     * pointer will be stored at reactShape, and created object will be deallocated by destructor of final class;
     */
    virtual void computeReactCS() = 0;

    /**
     * @brief Compute innertia tensor basing on collider's shape and body's mass.
     * 
     * @param mass of body.
     * @return rp3d::Vector3 innertia tensor.
     */
    virtual rp3d::Vector3 computeInnertiaTensor(float mass) = 0;

    /**
     * @brief Pointer to CollisionShape, created by computeReactCS().
     * If uninitialized - nullptr. 
     */
    rp3d::CollisionShape* reactShape = nullptr;

    /**
     * @brief Pointer to Collider, setted externally by system;
     * 
     */
    rp3d::Collider* reactCollider = nullptr;

    virtual ~Collider() = default;
};

#endif /* !COLLIDER_HPP_ */
