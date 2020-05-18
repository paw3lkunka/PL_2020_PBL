#include "LightSystem.hpp"

#include "Entity.hpp"

bool LightSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    light = entity->getComponentPtr<Light>();
    return (transform != nullptr && light != nullptr);
}

void LightSystem::frameUpdate()
{

}