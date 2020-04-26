#include "ObjectModule.hpp"
#include "Entity.hpp"

template<typename T>
T* ObjectModule::NewComponent()
{
    components.push_back(new T());
    entities.back().addComponent(components[components.size() - 1]);
    return dynamic_cast<T*>(components[components.size() - 1]);
}