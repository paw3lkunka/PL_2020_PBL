#ifndef _LIGHTSYSTEM_HPP
#define _LIGHTSYSTEM_HPP

#include "System.hpp"
#include "Transform.inl"
#include "Light.inl"

class LightSystem : public System
{
public:
    LightSystem() = default;
    virtual ~LightSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void frameUpdate();

private:
    Transform* transform;
    Light* light;
};

#endif // _LIGHTSYSTEM_HPP
