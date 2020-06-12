#ifndef SPHERECOLLIDER_INL_
#define SPHERECOLLIDER_INL_

#include "Collider.inl"

/**
 * @brief Sphere-shaped collider.
 * IMPORTANT collider size is defined in world scale!
 */
struct SphereCollider : public Collider
{
    SphereCollider() = default;
    virtual ~SphereCollider();

    /**
     * @brief Create react phisic box shape, basing on valuses stored in fields - used in physic simulation.
     */
    virtual void computeReactCS();

    /**
     * @brief Compute value of innertia tensor for given mass, basing on valuses stored in fields.
     * 
     * @param mass Mass of body
     * @return rp3d::Vector3 innertia tensor (diagonal).
     */
    virtual rp3d::Vector3 computeInnertiaTensor(float mass);

    /**
     * @brief Radius of the sphere collider
     */
    float radius = 1.0f;

    /**
     * @brief Is sphere hollow or solid.
     * NOTE: Necessery to compute innertia tensor, do not affects collisions detection. 
     */
    float isHollow = false;
};

#endif /* !SPHERECOLLIDER_INL_ */
