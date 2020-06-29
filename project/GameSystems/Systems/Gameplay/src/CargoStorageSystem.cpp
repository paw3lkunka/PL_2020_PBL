#include "CargoStorageSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "CargoStorage.inl"
#include "Cargo.inl"

#include <algorithm>

void CargoStorageSystem::addCargo(Cargo* cargoPtr)
{
    cargoStoragePtr->cargosStored.push_back(cargoPtr);
    cargoStoragePtr->weightSum += cargoPtr->weight;
    cargoStoragePtr->incomeSum += cargoPtr->income;

    if(auto rb = cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>())
    {
        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass += cargoPtr->weight;
        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->updateReactRB(cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass);
    }
}

void CargoStorageSystem::removeCargo(std::list<Cargo*>::iterator cargoIter)
{
    Cargo* cargoPtr = *cargoIter;

    cargoStoragePtr->weightSum -= cargoPtr->weight;
    cargoStoragePtr->incomeSum -= cargoPtr->income;
    cargoStoragePtr->cargosStored.erase(cargoIter);

    if(auto rb = cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>())
    {
        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass -= cargoPtr->weight;
        cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->updateReactRB(cargoStoragePtr->entityPtr->getComponentPtr<Rigidbody>()->mass);
    }
}

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
                    addCargo(editedCargo);
                }
            }
            break;
            case Event::REMOVE_CARGO:
            {
                editedCargo = msg.getValue<Cargo*>();
                std::list<Cargo*>::iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), editedCargo);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    removeCargo(iter);
                }
                else
                {
                    std::cerr << "Cargo don't exist!" << std::endl;
                }
            }
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
                weightText->mesh.text = getFloatWithPrecision(cargoStoragePtr->weightSum, 2) + "kg";
                incomeText->mesh.text = getFloatWithPrecision(cargoStoragePtr->incomeSum, 2) + "$";
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