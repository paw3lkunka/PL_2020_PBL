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
 * @brief Structure specifying glm::vec3 for given time
 */
struct KeyVector
{
    ///@brief time of this key
    double time;
    ///@brief value of this key
    glm::vec3 value;
};

/**
 * @brief Structure specifying rotation for given time
 */
struct KeyQuaternion
{
    ///@brief time of this key
    double time;
    ///@brief value of this key
    glm::quat value;
};

#endif /* !ANIMATIONDATASTRUCTURES_INL_ */
