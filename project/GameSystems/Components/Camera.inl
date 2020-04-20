#ifndef _CAMERA_INL
#define _CAMERA_INL

#include "Component.inl"
#include "ComponentType.inl"


enum class CameraProjection { Perspective, Orthographic };

struct Camera : public Component
{
    inline Camera() { type = ComponentType::CAMERA; }
    virtual ~Camera() = default;
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
