#include "ObjectModule.hpp"
#include "Components.inc"
#include "ObjectMaker.ipp"

template<typename T>
T* ObjectModule::newEmptyComponent()
{
    return objectMaker.newEmptyComponent<T>();
}

template<typename T>
T* ObjectModule::newEmptyComponentForLastEntity()
{
    return objectMaker.newEmptyComponentForLastEntity<T>();
}