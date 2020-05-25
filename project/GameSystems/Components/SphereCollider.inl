#ifndef SPHERECOLLIDER_INL_
#define SPHERECOLLIDER_INL_

#include "Collider.inl"


#include <glm/glm.hpp>

/**
 * @brief Sphere-shaped collider
 */
struct SphereCollider : public Collider
{
    SphereCollider() = default;
    virtual ~SphereCollider() = default;

    /**
     * @brief Collider local center
     */
    glm::vec3 center = {0.0f, 0.0f, 0.0f};

    /**
     * @brief Radius of the sphere collider
     */
    float radius = 1.0f;
};

#endif /* !SPHERECOLLIDER_INL_ */
