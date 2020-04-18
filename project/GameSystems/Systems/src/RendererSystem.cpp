#include "RendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"

bool RendererSystem::assertEntity(Entity* entity)
{
    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (meshRenderer != nullptr && transform != nullptr);
}

void RendererSystem::start()
{
    meshRenderer->modelMatrix = &transform->localToWorldMatrix;
}

void RendererSystem::frameUpdate()
{
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_TO_QUEUE, meshRenderer));
}