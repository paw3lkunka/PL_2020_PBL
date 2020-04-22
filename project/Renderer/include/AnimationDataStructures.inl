#ifndef ANIMATIONDATASTRUCTURES_INL_
#define ANIMATIONDATASTRUCTURES_INL_

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * @brief Defines how an animation behaves outside the defined time range
 */
enum class AnimationBehaviour
{
    ///@brief Value from the default node transformation
    DEFAULT = 0,
    ///@brief Nearest key value without interpolation
    CONSTANT = 1,
    ///@brief Nearest two keys are linearly extrapolated
    LINEAR,
    ///@brief The animation is repeated
    REPEAT,
};

/**
 * @brief Structure with animation key data
 */
struct AnimationKey
{
    /// @brief position of specified animation key
    glm::vec3 position;
    /// @brief rotation of specified animation key
    glm::vec3 rotation;
};


#endif /* !ANIMATIONDATASTRUCTURES_INL_ */
