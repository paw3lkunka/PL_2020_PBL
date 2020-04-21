#ifndef BONE_INL_
#define BONE_INL_

#include <vector>

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

};

#endif /* !BONE_INL_ */
