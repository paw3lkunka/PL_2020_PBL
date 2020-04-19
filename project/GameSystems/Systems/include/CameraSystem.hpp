#ifndef _CAMERASYSTEM_HPP
#define _CAMERASYSTEM_HPP

#include "System.hpp"

class Transform;
class Camera;

    //TODO documentation
class CameraSystem : public System
{
public:
    CameraSystem() = default;
    virtual ~CameraSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    //TODO documentation
    virtual void frameUpdate();

private:
    Transform* transform;
    Camera* camera;
};

#endif // _CAMERASYSTEM_HPP
