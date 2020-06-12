#ifndef _LIGHTSYSTEM_HPP
#define _LIGHTSYSTEM_HPP

#include "System.hpp"
#include "Transform.hh"
#include "Light.inl"

//TODO documentation
class LightSystem : public System
{
public:
    LightSystem() = default;
    virtual ~LightSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void start();

private:
    Transform* transform;
    Light* light;
};

#endif // _LIGHTSYSTEM_HPP
