#include "ObjectModule.hpp"
#include "ComponentType.inl"
#include "Entity.hpp"

template<typename T>
T* ObjectModule::NewComponent()
{
    T* ptr = components.allocate(T());
    entities.back().addComponent(ptr);
    return ptr;
}