#ifndef _THIRDPERSONCAMERACONTROLSYSTEM_HPP
#define _THIRDPERSONCAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/glm.hpp>

struct Transform;
struct Camera;

class ThirdPersonCameraControlSystem : public System, public IMsgReceiver
{
public:
    ThirdPersonCameraControlSystem() = default;
    virtual ~ThirdPersonCameraControlSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void receiveMessage(Message msg);
    virtual void fixedUpdate();

private:
    Transform* transform;
    Camera* camera;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 20.0f;
    float roll;

    bool usingMouse = false;
    float zoom = 0.0f;
};

#endif // _THIRDPERSONCAMERACONTROLSYSTEM_HPP
