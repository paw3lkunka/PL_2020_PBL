#ifndef SPHERECOLLIDER_INL_
#define SPHERECOLLIDER_INL_

#include "Collider.inl"

#include <glm/glm.hpp>

/**
 * @brief Sphere-shaped collider
 */
struct SphereCollider : public Collider
{
    virtual ~SphereCollider() = default;

    /**
     * @brief Collider local center
     */
    glm::vec3 center;

    /**
     * @brief Radius of the sphere collider
     */
    float radius;
};

#endif /* !SPHERECOLLIDER_INL_ */
