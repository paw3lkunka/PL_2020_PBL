#include "LightSystem.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "Message.inl"

bool LightSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    light = entity->getComponentPtr<Light>();
    return (transform != nullptr && light != nullptr);
}

void LightSystem::start()
{
    light->modelMatrix = &transform->getModelMatrix();
    GetCore().messageBus.sendMessage(Message(Event::RENDERER_ADD_LIGHT, light));
}