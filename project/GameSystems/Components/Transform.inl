#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Component.inl"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * @brief Struct that defines local transform of the object and its model matrix
 */
struct Transform : public Component
{
    virtual ~Transform() = default;

    glm::vec3 localPosition = {0,0,0};
    glm::quat localRotation = {1,0,0,0};
    glm::vec3 localScale = {1,1,1};
    
    /**
     * @brief Model matrix (local to world transform)
     */
    glm::mat4 localToWorldMatrix = glm::mat4(1);

    /**
     * @brief Signalize, if local parameters was changed.
     * system chaning localPosition, localRotation or localScale should also set iy true.
     */
    bool dirty = true;

    /**
     * @brief parent node. 
     */
    Transform* parent = nullptr;

    /**
     * @brief child nodes.
     */
    std::vector<Transform*> children;
};

#endif /* !TRANSFORM_HPP_ */
