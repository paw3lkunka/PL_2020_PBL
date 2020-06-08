#include "CargoStorageSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "CargoStorage.inl"
#include "Cargo.inl"

void CargoStorageSystem::receiveMessage(Message msg)
{
    if(cargoStoragePtr != nullptr)
    {
        switch(msg.getEvent())
        {
            case Event::ADD_CARGO:
            {
                Cargo* cargoToAdd = msg.getValue<Cargo*>();
                if(findCargo(cargoStoragePtr->cargosStored, cargoToAdd) != cargoStoragePtr->cargosStored.end())
                {
                    std::cerr << "Cargo already exists!" << std::endl;
                }
                else
                {
                    cargoStoragePtr->cargosStored.push_back(msg.getValue<Cargo*>());
                }
            }
            break;
            case Event::REMOVE_CARGO:
            {
                Cargo* cargoToRemove = msg.getValue<Cargo*>();
                auto iter = findCargo(cargoStoragePtr->cargosStored, cargoToRemove);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    cargoStoragePtr->cargosStored.erase(iter);
                }
                else
                {
                    std::cerr << "Cargo don't exist!" << std::endl;
                }
            }
            break;
        }
    }
}

bool CargoStorageSystem::assertEntity(Entity* entity)
{
    cargoStoragePtr = entity->getComponentPtr<CargoStorage>();
    return cargoStoragePtr != nullptr;
}

void CargoStorageSystem::frameUpdate()
{

}

std::list<Cargo*>::iterator CargoStorageSystem::findCargo(std::list<Cargo*> vec, Cargo* toFind)
{
    for(std::list<Cargo*>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        if(*(*iter) == *toFind)
        {
            return iter;
        }
    }
    return vec.end();
}