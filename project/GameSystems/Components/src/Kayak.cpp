#include "Kayak.hpp"
#include "Transform.hh"
#include "Entity.hpp"

Kayak* Kayak::lastInstance = nullptr;
Transform* Kayak::kayakTransformPtr = nullptr;

Kayak::Kayak()
{
    lastInstance = this;
}

Kayak::~Kayak()
{
    lastInstance = nullptr;
}

Kayak* Kayak::get()
{
    return lastInstance;
} 

Transform* Kayak::getTransform()
{
    if (kayakTransformPtr == nullptr && lastInstance != nullptr)
    {
        kayakTransformPtr = Kayak::lastInstance->entityPtr->getComponentPtr<Transform>();      
    }

    return kayakTransformPtr;
} 