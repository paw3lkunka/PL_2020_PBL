#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Component.inl"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * @brief Struct that defines local transform of the object and its model matrix
 */
struct Transform : public Component
{
    virtual ~Transform() = default;

    glm::vec3 localPosition;
    glm::quat localRotation;
    glm::vec3 localScale;
    
    /**
     * @brief Model matrix (local to world transform)
     */
    glm::mat4 localToWorldMatrix;
};

#endif /* !TRANSFORM_HPP_ */
