#ifndef SPHERECOLLIDER_INL_
#define SPHERECOLLIDER_INL_

#include "Collider.inl"

/**
 * @brief Sphere-shaped collider
 */
struct SphereCollider : public Collider
{
    SphereCollider() = default;
    virtual ~SphereCollider();

    //TODO documentation
    virtual void computeReactCS();

    /**
     * @brief Radius of the sphere collider
     */
    float radius = 1.0f;
};

#endif /* !SPHERECOLLIDER_INL_ */
