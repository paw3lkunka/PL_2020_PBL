#ifndef BONE_INL_
#define BONE_INL_

#include <map>

#include "Component.inl"
#include "AnimationDataStructures.inl"

struct Bone : Component
{
    Bone() = default;
    virtual ~Bone() = default;
    /**
     * @brief Children of bones are stored as children of transform to simplify hierarchy calculations
     * The localToWorldMatrix should be interpreted as the finalTransformation 
     */

    ///@brief bone id in animation hierarchy
    unsigned int boneIndex;
    /// @brief the name of the bone
    std::string boneName;
    /**
     * @brief Transforms from the mesh space to the bone space
     */
    glm::mat4 offsetMatrix;
    /**
     * @brief original transform of the bone in relation to its parent (in bone space) (Aka ToParent matrix, aka mTransformation)
     * note: Bind refers to original unanimated pose - the bind pose
     * note2: Bone's local space is often called bone space and it's always in relation to its parent
     */
    // ! As its part of the bone structure it should probably be stored as transform component
    // glm::mat4 localBindTransform; 
    /**
     * @brief inverted transform of the bone in model space, not local (bone) space
     * note: Inverted it means it converts model space to bone space and not the other way around
     * note2: It may be not perfectly clear at the moment but trust me that this matrix is necessary
     */
    glm::mat4 inverseBindTransform;
};

#endif /* !BONE_INL_ */

// ///@brief position keys of this animation
//     std::map<double, glm::vec3> positionKeys;
//     ///@brief position keys of this animation
//     std::map<double, glm::quat> rotationKeys;
// ///@brief defines how the animation behaves before the first key
//     AnimationBehaviour beforeState;
//     ///@brief defines how the animation behaves after the last key
//     AnimationBehaviour afterState;