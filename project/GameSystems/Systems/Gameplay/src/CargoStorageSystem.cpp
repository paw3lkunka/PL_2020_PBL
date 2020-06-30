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
        Cargo* editedCargo = nullptr;
        switch(msg.getEvent())
        {
            case Event::ADD_CARGO:
            {
                editedCargo = msg.getValue<Cargo*>();
                Iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), editedCargo);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    std::cerr << "Cargo already exists!" << std::endl;
                }
                else
                {
                    addCargo(editedCargo);
                    updateButtonsState(editedCargo);
                }
            }
            break;
            case Event::REMOVE_CARGO:
            {
                editedCargo = msg.getValue<Cargo*>();
                Iterator iter = std::find(cargoStoragePtr->cargosStored.begin(), cargoStoragePtr->cargosStored.end(), editedCargo);
                if(iter != cargoStoragePtr->cargosStored.end())
                {
                    removeCargo(iter);
                    updateButtonsState(editedCargo);
                }
                else
                {
                    std::cerr << "Cargo don't exist!" << std::endl;
                }
            }
            break;
            case Event::CARGO_LOST:
            {
                if (int size = cargoStoragePtr->cargosStored.size())
                {
                    Iterator iter = cargoStoragePtr->cargosStored.begin() + GetCore().randomInt(size - 1);
                    LooseCargo(iter);
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

void CargoStorageSystem::removeCargo(Iterator cargoIter)
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

void CargoStorageSystem::LooseCargo(Iterator cargoIter)
{
    Cargo* cargoPtr = *cargoIter;

    cargoStoragePtr->weightSum -= cargoPtr->weight;
    cargoStoragePtr->incomeSum -= cargoPtr->income;

    cargoStoragePtr->cargosLost.push_back(cargoPtr);
    cargoStoragePtr->cargosStored.erase(cargoIter);
}

void CargoStorageSystem::updateButtonsState(Cargo* cargoPtr)
{
    if(cargoPtr->entityPtr->getComponentPtr<CargoButton>() != nullptr) 
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