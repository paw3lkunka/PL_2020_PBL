#include "HydroSurfaceSystem.hpp"

#include "Core.hpp"
#include "Event.inl"
#include "Message.inl"
#include "Entity.hpp"

#include "HydroSurface.inl"
#include "Transform.hh"
#include "MeshRenderer.inl"


bool HydroSurfaceSystem::assertEntity(Entity* entity)
{
    hydroSurface = entity->getComponentPtr<HydroSurface>();
    if(hydroSurface == nullptr)
    {
        return false;
    }

    transform = entity->getComponentPtr<Transform>();
    if(transform == nullptr)
    {
        return false;
    }

    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    if(meshRenderer == nullptr)
    {
        return false;
    }

    return true;
}

void HydroSurfaceSystem::fixedUpdate()
{

}
