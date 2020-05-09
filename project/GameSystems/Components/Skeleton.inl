#ifndef _SKELETON_INL
#define _SKELETON_INL

#include "Component.inl"
#include <glm/mat4x4.hpp>

class Animation;
struct Bone;


struct Skeleton : public Component
{
    Skeleton() = default;
    virtual ~Skeleton() = default;

    Bone* rootBone;
    Animation* animation;

    glm::mat4 globalInverseTransform;
};


#endif // _SKELETON_INL
