#include "MeshRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "MeshRenderer.inl"
#include "Transform.inl"
#include "Message.inl"

bool MeshRendererSystem::assertEntity(Entity* entity)
{
    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (meshRenderer != nullptr && transform != nullptr);
}

void MeshRendererSystem::frameUpdate()
{
    if (meshRenderer->material->instancingEnabled())
    {

    }
    else
    {
        meshRenderer->modelMatrix = transform->localToWorldMatrix;
        GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_MESH_TO_QUEUE, meshRenderer));
    }
}