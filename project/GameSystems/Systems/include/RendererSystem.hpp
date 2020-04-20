#ifndef _RENDERERSYSTEM_HPP
#define _RENDERERSYSTEM_HPP

#include "System.hpp"
#include "MeshRenderer.inl"
#include "Transform.inl"

    //TODO documentation
class RendererSystem : public System
{
public:
    RendererSystem() = default;
    virtual ~RendererSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    //TODO documentation
    virtual void start();
    //TODO documentation
    virtual void frameUpdate();

private:
    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _RENDERERSYSTEM_HPP
