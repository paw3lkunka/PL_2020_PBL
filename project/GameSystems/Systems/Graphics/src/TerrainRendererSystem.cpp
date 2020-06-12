#include "TerrainRendererSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "TerrainRenderer.inl"
#include "Transform.hh"
#include "Message.inl"

bool TerrainRendererSystem::assertEntity(Entity* entity)
{
    terrainRenderer = entity->getComponentPtr<TerrainRenderer>();
    transform = entity->getComponentPtr<Transform>();
    return (terrainRenderer != nullptr && transform != nullptr);
}

void TerrainRendererSystem::frameUpdate()
{
    terrainRenderer->modelMatrix = transform->getModelMatrix();
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_TERRAIN_TO_QUEUE, terrainRenderer));
}