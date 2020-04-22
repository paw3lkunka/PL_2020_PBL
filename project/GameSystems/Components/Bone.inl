#ifndef BONE_INL_
#define BONE_INL_

#include <vector>
#include <glm/mat4x4.hpp>

#include "Component.inl"
#include "AnimationDataStructures.inl"

struct Bone : Component
{
    Bone() = default;
    virtual ~Bone() = default;

    ///@brief position keys of this animation
    std::vector<KeyVector> positionKeys;
    ///@brief rotation keys of this animation
    std::vector<KeyQuaternion> rotationKeys;
    ///@brief scale keys of this animation
    std::vector<KeyVector> scaleKeys;
    ///@brief defines how the animation behaves before the first key
    AnimationBehaviour beforeState;
    ///@brief defines how the animation behaves after the last key
    AnimationBehaviour afterState;
    ///@brief skinning transform (Inverse Bind Pose)
    glm::mat4 offsetMatrix;
    ///@brief bone id in animation hierarchy
    unsigned int boneID;
};

#endif /* !BONE_INL_ */
