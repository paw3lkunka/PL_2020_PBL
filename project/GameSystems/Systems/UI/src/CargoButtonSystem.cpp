#include "CargoButtonSystem.hpp"

#include "Entity.hpp"
#include "CargoButton.inl"
#include "CargoDataStructures.inl"

bool CargoButtonSystem::assertEntity(Entity* entity)
{
    cargoButtonPtr = entity->getComponentPtr<CargoButton>();
    return cargoButtonPtr != nullptr;
}

void CargoButtonSystem::start()
{
    
}