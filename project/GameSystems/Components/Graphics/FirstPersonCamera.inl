#ifndef _FIRSTPERSONCAMERA_INL
#define _FIRSTPERSONCAMERA_INL

#include "Component.hpp"

struct FirstPersonCamera : public Component
{
    FirstPersonCamera() = default;
    virtual ~FirstPersonCamera() = default;

    Transform* player;
    glm::vec3 headOffset = { 0.0f, 20.0f, 0.0f};

    float rotationSensitivity = 1.0f;

    float minPitch;
    float maxPitch;

    float minYaw;
    float maxYaw;
};


#endif // _FIRSTPERSONCAMERA_INL
