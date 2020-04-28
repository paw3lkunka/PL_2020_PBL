#include "ObjectMaker.hpp"
#include "Entity.hpp"

template<typename T>
T* ObjectMaker::newEmptyComponent()
{
    objContainer->components.push_back(new T());
    return dynamic_cast<T*>(objContainer->components[objContainer->components.size() - 1]);
}

template<typename T>
T* ObjectMaker::newEmptyComponentForLastEntity()
{
    objContainer->components.push_back(new T());
    objContainer->entities[objContainer->entities.size() - 1].addComponent(objContainer->components[objContainer->components.size() - 1]);
    return dynamic_cast<T*>(objContainer->components[objContainer->components.size() - 1]);
}