#include "ObjectModule.hpp"
#include "Entity.hpp"

template<typename T>
T* ObjectModule::NewComponent()
{
    //HACK AS FUCK
    T* ptr = new T();
    entities.back().addComponent(ptr);
    components.push_back(ptr);
    return ptr;
}