#ifndef _BONE_INL
#define _BONE_INL

#include <string>
#include <vector>
#include <glm/glm.hpp>

/**
 * @brief Holds bone data used in skinning and animation
 */
struct Bone
{
    static constexpr unsigned int MAX_BONE_COUNT = 128;

    Bone(unsigned int id, std::string name, glm::mat4 offsetMatrix) 
        : boneID(id), boneName(name), offsetMatrix(offsetMatrix) {}
    Bone() = default;
    virtual ~Bone() = default;

    /**
     * @brief Children of selected bone. If vector size equals to zero, the bone is considered a leaf
     */
    std::vector<Bone*> children;
    /**
     * @brief Parent of selected bone. If equals to nullptr then the bone is considered a root
     */
    Bone* parent;

    /// @brief ID used to identify the bone in shader
    unsigned int boneID;
    /// @brief Bone name used to identify bone in the node hierarchy
    std::string boneName;

    /**
     * @brief Matrix used to transform node from model space to bone space
     * Used for offsetting bone to aid in animations and sets the bind pose
     */
    glm::mat4 offsetMatrix;
    /**
     * @brief Matrix used to translate the offset bone into the animation pose
     */
    glm::mat4 localBoneTransform;
    glm::mat4 boneTransform;
};

#endif // _BONE_INL
