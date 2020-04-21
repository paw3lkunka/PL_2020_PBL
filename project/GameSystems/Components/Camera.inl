#ifndef _CAMERA_INL
#define _CAMERA_INL

#include <glm/glm.hpp>

#include "Component.inl"

enum class CameraProjection { Perspective, Orthographic };

struct Camera : public Component
{
    Camera() = default;
    virtual ~Camera() = default;

    CameraProjection projectionMode;
    bool isMain;
    float fieldOfView, othographicSize, nearPlane, farPlane;
    
    /// @brief Dirty flags
    bool projectionChanged = true;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

#endif // _CAMERA_INL
