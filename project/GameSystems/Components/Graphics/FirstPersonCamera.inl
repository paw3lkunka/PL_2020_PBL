#ifndef _FIRSTPERSONCAMERA_INL
#define _FIRSTPERSONCAMERA_INL

#include "Component.hpp"

struct FirstPersonCamera : public Component
{
    FirstPersonCamera() = default;
    virtual ~FirstPersonCamera() = default;

    Transform* player;
    glm::vec3 headOffset = { 0.0f, -0.22f, 0.67f};

    float rotationSensitivity = 1.0f;

    float minPitch = -60.0f;
    float maxPitch = 70.0f;

    float minYaw = -200.0f;
    float maxYaw = 20.0f;

    float moveLerp = 0.75f;
    float rotationLerp = 0.1f;
};


#endif // _FIRSTPERSONCAMERA_INL
