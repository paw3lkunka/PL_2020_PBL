#include "MeshRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"

bool MeshRendererSystem::assertEntity(Entity* entity)
{
    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (meshRenderer != nullptr && transform != nullptr);
}

void MeshRendererSystem::frameUpdate()
{
    meshRenderer->modelMatrix = transform->localToWorldMatrix;
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_MESH_TO_QUEUE, meshRenderer));
}