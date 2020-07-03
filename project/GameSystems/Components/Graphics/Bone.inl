#ifndef _BONE_INL
#define _BONE_INL

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Component.hpp"

/**
 * @brief Holds bone data used in skinning and animation
 */
struct Bone : public Component
{
//TODO documentation
    static constexpr unsigned int MAX_BONE_COUNT = 128;
    
    Bone() = default;
    virtual ~Bone() = default;

    /// @brief ID used to identify the bone in shader
    unsigned int boneID;

    /// @brief Bone name used to identify bone in the node hierarchy
    std::string boneName;

    /// @brief How much in the local space the bone is allowed to move
    glm::vec3 minConstraints = {-360.0f, -360.0f, -360.0f};
    glm::vec3 maxConstraints = { 360.0f,  360.0f,  360.0f};

    /**
     * @brief Matrix used to transform node from model space to bone space
     * Used for offsetting bone to aid in animations and sets the bind pose
     */
    glm::mat4 offsetMatrix;
};

#endif // _BONE_INL
