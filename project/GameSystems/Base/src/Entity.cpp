#include "Entity.hpp"

#include "Component.hpp"
#include "System.hpp"

Entity::Entity(int id, int bufferSize, std::string name, bool serializable) : ISerializable(serializable)
{
    this->id = id;
    this->name = name;
    this->components.reserve(bufferSize);
}

void Entity::addComponent(Component* ptr)
{
    components.push_back(ptr);
    ptr->entityPtr = this;
}

const int Entity::getId() const
{
    return id;
}

const std::string Entity::getName() const
{
    return name;
}

const std::vector<Component*>* Entity::getComponentsPtr() const
{
    return &components;
}