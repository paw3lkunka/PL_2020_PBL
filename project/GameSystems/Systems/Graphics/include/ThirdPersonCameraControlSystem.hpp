#ifndef _THIRDPERSONCAMERACONTROLSYSTEM_HPP
#define _THIRDPERSONCAMERACONTROLSYSTEM_HPP

#include "System.hpp"
#include "IMsgReceiver.inl"

#include <glm/glm.hpp>

struct Transform;
struct Camera;
struct ThirdPersonCamera;
struct SphereCollider;
struct Rigidbody;

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
    ThirdPersonCamera* tpCamera;
    SphereCollider* collider;

    float angleAroundPlayer = 0.0f;
    float pitch = -70.0f;

    bool usingMouse = false;
    float rotationSensitivity = 1.0f;
    float zoom = 0.0f;

    glm::vec2 gamepadAxis = { 0.0f, 0.0f };
};

#endif // _THIRDPERSONCAMERACONTROLSYSTEM_HPP
