#include "CargoStorageSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "CargoStorage.inl"
#include "Cargo.inl"

#include <algorithm>

void CargoStorageSystem::receiveMessage(Message msg)
{
    if(cargoStoragePtr != nullptr)
    {
        switch(msg.getEvent())
        {
            case Event::ADD_CARGO:
            {
                Cargo* cargoToAdd = msg.getValue<Cargo*>();
                std::list<Cargo*>::iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), cargoToAdd);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    std::cerr << "Cargo already exists!" << std::endl;
                }
                else
                {
                    cargoStoragePtr->cargosStored.push_back(cargoToAdd);
                    cargoStoragePtr->weightSum += cargoToAdd->weight;
                    cargoStoragePtr->incomeSum += cargoToAdd->income;
                    std::cout << "Required: " << std::to_string(cargoStoragePtr->cargosStoredSize) << "now: " << std::to_string(cargoStoragePtr->cargosStored.size()) << "\n";
                    if(cargoStoragePtr->cargosStoredSize == cargoStoragePtr->cargosStored.size())
                    {
                        std::cout << "cargo mass add\n";
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass += cargoStoragePtr->weightSum;
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->updateReactRB(cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass);
                    }
                }
            }
            break;
            case Event::REMOVE_CARGO:
            {
                Cargo* cargoToRemove = msg.getValue<Cargo*>();
                std::list<Cargo*>::iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), cargoToRemove);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    cargoStoragePtr->cargosStored.erase(iter);
                    cargoStoragePtr->weightSum -= cargoToRemove->weight;
                    cargoStoragePtr->incomeSum -= cargoToRemove->income;
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

void CargoStorageSystem::init(CargoStorage* cs)
{
    if(cs != nullptr)
    {
        cargoStoragePtr = cs;
    }
}
