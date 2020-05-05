#ifndef _SKELETON_INL
#define _SKELETON_INL

#include "Component.inl"

#include "Animation.hpp"
#include "Bone.inl"

struct Skeleton : public Component
{
    Skeleton() = default;
    virtual ~Skeleton() = default;

    Bone* rootBone;
    Animation* animation;

    glm::mat4 globalInverseTransform;
};


#endif // _SKELETON_INL
