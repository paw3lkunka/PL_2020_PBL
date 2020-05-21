#ifndef _CAMERA_INL
#define _CAMERA_INL

#include <glm/glm.hpp>

#include "Component.hpp"

enum class CameraProjection { Perspective, Orthographic };

struct Camera : public Component
{
    Camera() = default;
    virtual ~Camera() = default;

    CameraProjection projectionMode;
    bool isMain;
    float fieldOfView, orthographicSize, nearPlane, farPlane;
    
    /// @brief Dirty flags
    bool projectionChanged = true;

    glm::vec3 position;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

#endif // _CAMERA_INL
