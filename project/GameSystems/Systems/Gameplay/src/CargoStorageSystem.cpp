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
                    if(cargoStoragePtr->cargosStoredSize == cargoStoragePtr->cargosStored.size())
                    {
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass += cargoStoragePtr->weightSum;
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->updateReactRB(cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass);
                    }
                }

                // Cargo has not transform - select cargo scene
                if(cargoToAdd->entityPtr->getComponentPtr<CargoButton>() != nullptr) 
                {
                    auto e = GetCore().objectModule.getEntityPtrByName("Play_Button");
                    if(e != nullptr)
                    {
                        if(cargoStoragePtr->cargosStored.size() > 1)
                        {
                            e->getComponentPtr<Button>()->isActive = true;
                        }
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

                // Cargo has not transform - select cargo scene
                if(cargoToRemove->entityPtr->getComponentPtr<CargoButton>() != nullptr) 
                {
                    auto e = GetCore().objectModule.getEntityPtrByName("Play_Button");
                    if(e != nullptr)
                    {
                        if(cargoStoragePtr->cargosStored.size() < 1)
                        {
                            e->getComponentPtr<Button>()->isActive = false;
                        }
                    }
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
        std::cout << "Init" << std::endl;
        cargoStoragePtr = cs;
    }
}
