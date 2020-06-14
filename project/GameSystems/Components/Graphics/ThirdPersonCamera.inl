#ifndef _THIRDPERSONCAMERA_INL
#define _THIRDPERSONCAMERA_INL

#include "Component.hpp"
#include "Transform.hh"

struct ThirdPersonCamera : public Component
{
    ThirdPersonCamera() = default;
    virtual ~ThirdPersonCamera() = default;

    Transform* player;
    glm::vec3 playerOffset = { 0.0f, 20.0f, 0.0f };

    float distance = 150.0f;
    float minDistance = 50.0f;
    float maxDistance = 300.0f;

    float angleAroundPlayer = 0.0f;

    float minPitch = -100.0f;
    float maxPitch = -30.0f; 

    float moveLerp = 0.1f;

    float zoomSensitivity = 150.0f;
    float zoomLerp = 0.1f;

    float rotationSensitivity = 0.2f;
    float rotationLerp = 0.1f;
};


#endif // _THIRDPERSONCAMERA_INL
