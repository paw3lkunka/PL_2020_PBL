#ifndef _FIRSTPERSONCAMERACONTROLSYSTEM_HPP
#define _FIRSTPERSONCAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/glm.hpp>

struct Transform;
struct Camera;

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

    bool usingMouse = false;
};

#endif // _FIRSTPERSONCAMERACONTROLSYSTEM_HPP
