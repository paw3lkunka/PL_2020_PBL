#include "CargoSystem.hpp"
#include "Cargo.inl"
#include "Transform.hh"
#include "Entity.hpp"
#include "Core.hpp"

bool CargoSystem::assertEntity(Entity* entity)
{
    cargoPtr = entity->getComponentPtr<Cargo>();
    transformPtr = entity->getComponentPtr<Transform>();
    return (cargoPtr != nullptr) && (transformPtr != nullptr);
}

void CargoSystem::start()
{
    GetCore().messageBus.sendMessage(Message(Event::ADD_CARGO, cargoPtr));
}