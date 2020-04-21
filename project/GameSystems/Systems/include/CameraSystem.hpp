#ifndef _CAMERASYSTEM_HPP
#define _CAMERASYSTEM_HPP

#include "System.hpp"

class Transform;
class Camera;

/**
 * @brief 
 */
class CameraSystem : public System
{
public:
    CameraSystem() = default;
    virtual ~CameraSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    // FIXME AAAAAAAAAAAAA
    virtual void start();
    //TODO documentation
    virtual void frameUpdate();

    // FIXME TO JEST BRUDNY HACK - MOŻE TRAFIĆ NA MASTERA TYLKO JAK NIE ZDĄŻYMY TEGO ROZWIĄZAĆ INACZEJ
    static Transform* mainCameraTransform;
    static Camera* mainCamera;

private:
    Transform* transform;
    Camera* camera;
};

#endif // _CAMERASYSTEM_HPP
