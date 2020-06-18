#ifndef _CAMERASYSTEM_HPP
#define _CAMERASYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

class Transform;
class Camera;

/**
 * @brief Handles perspective and view matrices. Updates main camera reference.
 */
class CameraSystem : public System, public IMsgReceiver
{
public:
    CameraSystem() : System(false) {}
    virtual ~CameraSystem() = default;

    static Camera* mainCamera;

    /**
     * @brief Recieve message callback from message bus
     * 
     * @param msg message to process
     */
    virtual void receiveMessage(Message msg);

    /**
     * @brief Set the camera as main function. Sets global static mainCamera and mainCameraTransform.
     * 
     * @param entity Entity to set as main camera (only if contains transform and camera components)
     * @return true if entity is valid and main camera was switched
     * @return false if entity was invalid
     */
    static bool setAsMain(Entity* entity);

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
    bool cameraChanged = true;

    bool switchDebugCameraControl = false;
    bool switchGameplayCameraControl = false;

    Transform* transform;
    Camera* camera;
};

#endif // _CAMERASYSTEM_HPP
