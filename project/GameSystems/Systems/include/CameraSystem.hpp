#ifndef _CAMERASYSTEM_HPP
#define _CAMERASYSTEM_HPP

#include "System.hpp"

class Transform;
class Camera;

/**
 * @brief Handles perspective and view matrices. Updates main camera reference.
 */
class CameraSystem : public System
{
public:
    CameraSystem() = default;
    virtual ~CameraSystem() = default;

    /**
     * @brief Set the camera as main function. Sets global static mainCamera and mainCameraTransform.
     * 
     * @param entity Entity to set as main camera (only if contains transform and camera components)
     * @return true if entity is valid and main camera was switched
     * @return false if entity was invalid
     */
    static bool setAsMain(Entity* entity);
    /**
     * @brief Transform of a camera with which we're rendering
     */
    static Transform* mainCameraTransform;
    /**
     * @brief Camera with which we're rendering
     */
    static Camera* mainCamera;

    /**
     * @brief Looks for entities with camera and transform components
     * 
     * @param entity Entity proveided by system
     * @return true if transform and camera components are found
     * @return false if above condition tests false
     */
    virtual bool assertEntity(Entity* entity);
    /**
     * @brief Updates camera projection and view matrices
     */
    virtual void frameUpdate();

private:
    Transform* transform;
    Camera* camera;
};

#endif // _CAMERASYSTEM_HPP
