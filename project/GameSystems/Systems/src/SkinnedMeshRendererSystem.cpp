#include "SkinnedMeshRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"

bool SkinnedMeshRendererSystem::assertEntity(Entity* entity)
{
    meshRenderer = entity->getComponentPtr<SkinnedMeshRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (meshRenderer != nullptr && transform != nullptr);
}

void SkinnedMeshRendererSystem::frameUpdate()
{
    meshRenderer->modelMatrix = transform->localToWorldMatrix;
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_SKINNED_MESH_TO_QUEUE, meshRenderer));
}