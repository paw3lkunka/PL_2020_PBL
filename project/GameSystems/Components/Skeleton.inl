#ifndef _SKELETON_INL
#define _SKELETON_INL

#include "Component.hpp"
#include <glm/mat4x4.hpp>

class Animation;
class Entity;


struct Skeleton : public Component
{
    Skeleton() = default;
    virtual ~Skeleton() = default;

    Entity* rootBone;
    Animation* animation;

    glm::mat4 globalInverseTransform;
};


#endif // _SKELETON_INL
