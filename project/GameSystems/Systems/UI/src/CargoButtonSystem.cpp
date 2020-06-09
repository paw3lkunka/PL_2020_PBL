#include "CargoButtonSystem.hpp"

#include "Entity.hpp"
#include "CargoButton.inl"
#include "CargoDataStructures.inl"
#include "Core.hpp"

#include <sstream>
#include <iomanip>
#include <ios>

bool CargoButtonSystem::assertEntity(Entity* entity)
{
    cargoButtonPtr = entity->getComponentPtr<CargoButton>();
    return cargoButtonPtr != nullptr;
}

void CargoButtonSystem::start()
{
    CargoType data = Cargos::baseCargos[GetCore().randomInt(Cargos::baseCargosSize - 1)];
    Cargo* cargo = GetCore().objectModule.newEmptyComponent<Cargo>();
    cargo->name = data.name;
    cargo->isRisky = data.isRisky;
    cargo->income = GetCore().randomFloatR(data.incomeRange.x, data.incomeRange.y);
    cargo->weight = GetCore().randomFloatR(data.weightRange.x, data.weightRange.y);
    cargoButtonPtr->entityPtr->addComponent(cargo);
    cargoButtonPtr->onActivateEvents.push_back(Message(Event::ADD_CARGO, cargo));
    cargoButtonPtr->onDeactivateEvents.push_back(Message(Event::REMOVE_CARGO, cargo));
    

    cargoButtonPtr->nameText->mesh.text = cargo->name;
    if(cargo->isRisky)
    {
        cargoButtonPtr->nameText->material->setVec4("color", {0.6f, 0.078f, 0.078f, 1.0f});
    }
    else
    {
        cargoButtonPtr->nameText->material->setVec4("color", {0.0f, 0.0f, 0.0f, 1.0f});
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << cargo->income;
    cargoButtonPtr->incomeText->mesh.text = "Income:" + ss.str() + "$";
    ss.str("");

    ss << std::fixed << std::setprecision(2) << cargo->weight;
    cargoButtonPtr->weightText->mesh.text = "Weight:" + ss.str() + "kg";
    ss.str("");
}