#include "SkinnedMeshRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "SkinnedMeshRenderer.inl"
#include "Transform.inl"
#include "Message.inl"

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