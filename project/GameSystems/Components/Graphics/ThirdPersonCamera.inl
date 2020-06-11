#ifndef _THIRDPERSONCAMERA_INL
#define _THIRDPERSONCAMERA_INL

#include "Component.hpp"
#include "Transform.inl"

struct ThirdPersonCamera : public Component
{
    ThirdPersonCamera() = default
    virtual ~ThirdPersonCamera() = default;

    Transform* player;

    float distanceFromPlayer = 50.0f;
    float angleAroundPlayer = 0.0f;
};


#endif // _THIRDPERSONCAMERA_INL
