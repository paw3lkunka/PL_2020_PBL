#ifndef _RENDERERSYSTEM_HPP
#define _RENDERERSYSTEM_HPP

#include "System.hpp"
#include "MeshRenderer.inl"
#include "Transform.inl"

class RendererSystem : public System
{
public:
    RendererSystem() = default;
    virtual ~RendererSystem() = default;

    virtual bool assertEntity(Entity* entity);
    virtual void start();
    virtual void frameUpdate();

private:
    Transform* transform;
    MeshRenderer* meshRenderer;
};

#endif // _RENDERERSYSTEM_HPP
