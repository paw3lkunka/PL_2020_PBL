#include "CargoStorageSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "CargoStorage.inl"
#include "Cargo.inl"

#include <algorithm>
#include <sstream>
#include <iomanip>

void CargoStorageSystem::receiveMessage(Message msg)
{
    if(cargoStoragePtr != nullptr)
    {
        Cargo* editedCargo = nullptr;
        switch(msg.getEvent())
        {
            case Event::ADD_CARGO:
            {
                editedCargo = msg.getValue<Cargo*>();
                std::list<Cargo*>::iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), editedCargo);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    std::cerr << "Cargo already exists!" << std::endl;
                }
                else
                {
                    cargoStoragePtr->cargosStored.push_back(editedCargo);
                    cargoStoragePtr->weightSum += editedCargo->weight;
                    cargoStoragePtr->incomeSum += editedCargo->income;
                    if(cargoStoragePtr->cargosStoredSize == cargoStoragePtr->cargosStored.size())
                    {
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass += cargoStoragePtr->weightSum;
                        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->updateReactRB(cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass);
                    }
                }
            }
            break;
            case Event::REMOVE_CARGO:
            {
                editedCargo = msg.getValue<Cargo*>();
                std::list<Cargo*>::iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), editedCargo);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    cargoStoragePtr->cargosStored.erase(iter);
                    cargoStoragePtr->weightSum -= editedCargo->weight;
                    cargoStoragePtr->incomeSum -= editedCargo->income;
                }
                else
                {
                    std::cerr << "Cargo don't exist!" << std::endl;
                }
            }
            break;

            case Event::COLLISION_ENTER:
                //TODO: ANDRZEJ ZRUP TÓ ŻECZY
                //TODO: ANDRZEJ ZRUP TÓ ŻECZY
                //TODO: ANDRZEJ ZRUP TÓ ŻECZY
                //TODO: ANDRZEJ ZRUP TÓ ŻECZY
                //TODO: ANDRZEJ ZRUP TÓ ŻECZY
            break;
        }
        if(msg.getEvent() >= Event::CARGO_FIRST && msg.getEvent() <= Event::CARGO_LAST)
        {
            // Cargo has not transform - select cargo scene
            if(editedCargo->entityPtr->getComponentPtr<CargoButton>() != nullptr) 
            {
                auto e = GetCore().objectModule.getEntityPtrByName("Play_Button");
                if(e != nullptr)
                {
                    if(cargoStoragePtr->cargosStored.size() < 1)
                    {
                        e->getComponentPtr<Button>()->isActive = false;
                    }
                    else
                    {
                        e->getComponentPtr<Button>()->isActive = true;
                    }
                    
                }
            }

            if((weightText != nullptr) && (incomeText != nullptr))
            {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << cargoStoragePtr->weightSum;
                weightText->mesh.text = ss.str() + "kg";
                ss.str("");

                ss << std::fixed << std::setprecision(2) << cargoStoragePtr->incomeSum;
                incomeText->mesh.text = ss.str() + "$";
                ss.str("");
                
            }

            if(warningGroup != nullptr)
            {
                if(cargoStoragePtr->weightSum > cargoStoragePtr->weightLimit)
                {
                    warningGroup->groupTransparency = 0.9f;
                }
                else
                {
                    warningGroup->groupTransparency = 0.0f;
                }
            }
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

void CargoStorageSystem::initTexts(TextRenderer* weightText, TextRenderer* incomeText, UiSortingGroup* warningGroup)
{
    this->weightText = weightText;
    this->incomeText = incomeText;
    this->warningGroup = warningGroup;
}