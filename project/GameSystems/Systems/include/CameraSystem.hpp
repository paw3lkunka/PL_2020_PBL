#ifndef _CAMERASYSTEM_HPP
#define _CAMERASYSTEM_HPP

#include "System.hpp"

class Transform;
class Camera;

class CameraSystem : public System
{
public:
    CameraSystem() = default;
    virtual ~CameraSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    Transform* transform;
    Camera* camera;
};

#endif // _CAMERASYSTEM_HPP
