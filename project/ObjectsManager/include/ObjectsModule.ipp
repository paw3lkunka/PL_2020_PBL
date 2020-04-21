#include "ObjectModule.hpp"

template<typename T>
T* ObjectModule::NewComponent()
{
    //HACK AS FUCK
    T* ptr = new T();
    entities.back().addComponent(ptr);
    return ptr;
}