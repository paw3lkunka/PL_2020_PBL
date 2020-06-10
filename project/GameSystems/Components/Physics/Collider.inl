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
     * @brief Center of the collider in local space
     */
    glm::vec3 center = {0.0f, 0.0f, 0.0f};

    /**
     * @brief Create CollisionShape depending on type and parameters.
     * pointer will be stored at reactCS, and created object will be deallocated by destructor of final class;
     */
    virtual void computeReactCS() = 0;

    /**
     * @brief Pointer to CollisionShape, created by computeReactCS().
     * If uninitialized - nullptr. 
     */
    reactphysics3d::CollisionShape* reactCS = nullptr;

    virtual ~Collider() = default;
};

#endif /* !COLLIDER_HPP_ */
