#ifndef _SKINNEDMESHRENDERERSYSTEM_HPP
#define _SKINNEDMESHRENDERERSYSTEM_HPP

#include "System.hpp"
#include "SkinnedMeshRenderer.inl"
#include "Transform.inl"
#include "Message.inl"

    //TODO documentation
class SkinnedMeshRendererSystem : public System
{
public:
    SkinnedMeshRendererSystem() = default;
    virtual ~SkinnedMeshRendererSystem() = default;

    //TODO documentation
    virtual bool assertEntity(Entity* entity);
    //TODO documentation
    virtual void frameUpdate();

private:
    Transform* transform;
    SkinnedMeshRenderer* meshRenderer;
};

#endif // _SKINNEDMESHRENDERERSYSTEM_HPP
