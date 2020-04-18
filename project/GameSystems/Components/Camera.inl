#ifndef _CAMERA_INL
#define _CAMERA_INL

#include "Component.inl"

enum class CameraProjection { Perspective, Orthographic };

struct Camera : public Component
{
    CameraProjection projectionMode;
    bool isMain;
    float fieldOfView;
    float othographicSize;
    float nearPlane, farPlane;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    bool projectionChanged = true;
};

#endif // _CAMERA_INL
