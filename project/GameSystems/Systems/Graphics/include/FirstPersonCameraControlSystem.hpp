#ifndef _FIRSTPERSONCAMERACONTROLSYSTEM_HPP
#define _FIRSTPERSONCAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/glm.hpp>

struct Transform;
struct Camera;
struct FirstPersonCamera;

class FirstPersonCameraControlSystem : public System, public IMsgReceiver
{
public:
    FirstPersonCameraControlSystem() = default;
    virtual ~FirstPersonCameraControlSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void receiveMessage(Message msg);
    virtual void fixedUpdate();

private:
    Transform* transform;
    Camera* camera;
    FirstPersonCamera* fpCamera;

    float yaw = 0.0f;
    float pitch = 0.0f;

    bool usingMouse = false;
    float inputSensitivity = 1.0f;
};

#endif // _FIRSTPERSONCAMERACONTROLSYSTEM_HPP
