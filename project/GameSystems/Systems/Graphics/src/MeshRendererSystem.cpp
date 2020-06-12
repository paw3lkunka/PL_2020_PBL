#include "MeshRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "MeshRenderer.inl"
#include "Transform.hh"
#include "Message.inl"

bool MeshRendererSystem::assertEntity(Entity* entity)
{
    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (meshRenderer != nullptr && transform != nullptr);
}

void MeshRendererSystem::frameUpdate()
{
    meshRenderer->modelMatrix = transform->getModelMatrix();
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_MESH_TO_QUEUE, meshRenderer));
}