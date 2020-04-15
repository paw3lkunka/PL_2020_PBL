#ifndef BOXCOLLIDER_HPP_
#define BOXCOLLIDER_HPP_

#include "Collider.inl"

#include <glm/glm.hpp>

/**
 * @brief Box-shaped collider (cuboid)
 */
struct BoxCollider : public Collider
{
    virtual ~BoxCollider() = default;

    /**
     * @brief Center of the collider
     */
    glm::vec3 center;

    /**
     * @brief Size of the collider, where:
     * x - width, 
     * y - height, 
     * z - depth
     */
    glm::vec3 size;
};

#endif /* !BOXCOLLIDER_HPP_ */
