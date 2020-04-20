#ifndef _CAMERACONTROLSYSTEM_HPP
#define _CAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"
#include "Transform.inl"
#include "Camera.inl"

    //TODO documentation
class CameraControlSystem : public System, public IMsgReceiver
{
public:
    CameraControlSystem() = default;
    virtual ~CameraControlSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    //TODO documentation
    virtual void receiveMessage(Message msg);
    //TODO documentation
    virtual void fixedUpdate();

private:
    float speed = 1.0f;
    float sensitivity = 0.1f;
    glm::vec3 movementVector = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;

    Transform* transform;
    Camera* camera;
};

#endif // _CAMERACONTROLSYSTEM_HPP
