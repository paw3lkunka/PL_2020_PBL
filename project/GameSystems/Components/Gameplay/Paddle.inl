#ifndef PADDLE_INL_
#define PADDLE_INL_

#include "Component.hpp"
#include <glm/vec2.hpp>

/**
 * @brief Paddle component.
 *  ! There must be only one entity with this component
 */
struct Paddle : public Component
{
    Paddle() = default;
    virtual ~Paddle() = default;

    ///@brief minimum animation interpolation speed
    float minSpeed;
    ///@brief maximum animation interpolation speed
    float maxSpeed;

    ///@brief maximum animation gizmo position (in all axes)
    glm::vec3 maxPos;

    ///@brief maximun range of rotation in front (Z?) axis
    float maxFrontRot;
    ///@brief maximun range of rotation in side (X?) axis
    float maxSideRot;

    ///@brief position in 2d space
    glm::vec2 position2D = {0.0f, 0.0f};
};

#endif /* !PADDLE_INL_ */
