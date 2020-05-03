#ifndef BONE_INL_
#define BONE_INL_

#include <map>

#include "Component.inl"
#include "AnimationDataStructures.inl"
#include "IFileSystem.inl"

struct Bone : public Component, public IFileSystem
{
    Bone() = default;
    Bone(std::string filePath) : IFileSystem(filePath) {}
    virtual ~Bone() = default;

    ///@brief position keys of this animation
    std::map<double, glm::vec3> positionKeys;
    ///@brief position keys of this animation
    std::map<double, glm::quat> rotationKeys;
    ///@brief defines how the animation behaves before the first key
    AnimationBehaviour beforeState;
    ///@brief defines how the animation behaves after the last key
    AnimationBehaviour afterState;
    ///@brief bone id in animation hierarchy
    unsigned int boneID;
};

#endif /* !BONE_INL_ */
