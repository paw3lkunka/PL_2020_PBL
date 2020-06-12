#ifndef _CAMERA_INL
#define _CAMERA_INL

#include <glm/glm.hpp>

#include "Component.hpp"

enum class CameraProjection { Perspective, Orthographic };

/**
 * @brief Struct with camera's view frustum info
 * Elements which must be initialized:
 * aspectRatio, nearPlane/nearDist, farPlane/farDist, fov/fieldOfView/orthographicSize
 */
//TODO BRIEF
struct ViewFrustum
{
    // Current view position - updated every frame for main camera
    glm::vec3 position;
    // Camera's front, up and right vectors
    glm::vec3 front, up, right;
    // Clipping planes dimensions
    float Hnear, Wnear;
    float Hfar, Wfar;
    // Camera's aspect ratio
    float aspectRatio;
    // Field of view or orthographic size
    union 
    {
        float fov;
        float fieldOfView;
        float orthographicSize;
    };
    // Near plane distance
    union
    {
        float nearDist; // Alias for near plane
        float nearPlane; // Alias for near dist
    };
    // Far plane distance
    union 
    {
        float farDist; // Alias for far plane
        float farPlane; // Alias for far dist
    };
};

struct Camera : public Component
{
    Camera() = default;
    virtual ~Camera() = default;

    /**
     * @brief Get the camera frustum
     * @return const ViewFrustum& reference to camera's frustum
     */
    const ViewFrustum& getFrustum() { return frustum; }

    /**
     * @brief Modify the camera's frustum and set the dirty flag to recalculate the projection
     * @return ViewFrustum& reference to camera's frustum
     */
    ViewFrustum& getFrustumModifiable() { projectionChanged = true; return frustum; }

    /**
     * @brief Get the camera's projection mode 
     * @return const CameraProjection& reference to camera's projection mode
     */
    const CameraProjection& getProjectionMode() { return projectionMode; }

    /**
     * @brief Modeify the camera's projection mode
     * @return CameraProjection& reference to camera's projection mode
     */
    CameraProjection& getProjectionModeModifiable() { projectionChanged = true; return projectionMode; }

    /// @brief Projection dirty flag
    bool projectionChanged = true;
    /// @brief Main camera flag - only main camera is getting processed
    bool isMain;

    /// @brief Camera's view matrix, calculated every frame for main camera
    glm::mat4 viewMatrix;
    /// @brief Camera's projection matrix, calculated for main camera only if dirty flag == true
    glm::mat4 projectionMatrix;

    /**
     * @brief Camera exposure used for hdr rendering
     */
    float exposure = 1.0f;

private:
    CameraProjection projectionMode;
    ViewFrustum frustum;
};

#endif // _CAMERA_INL
