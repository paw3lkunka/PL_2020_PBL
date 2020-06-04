#ifndef _SKELETON_INL
#define _SKELETON_INL

#include "Component.hpp"
#include <glm/mat4x4.hpp>

class Animation;
class Entity;


//TODO documentation
struct Skeleton : public Component
{
    Skeleton() = default;
    virtual ~Skeleton() = default;

//TODO documentation
    Entity* rootBone;
//TODO documentation
    Animation* animation;

//TODO documentation
    glm::mat4 globalInverseTransform;
};


#endif // _SKELETON_INL
