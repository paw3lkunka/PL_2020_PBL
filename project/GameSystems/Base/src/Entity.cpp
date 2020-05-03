#include "Entity.hpp"

#include "Component.inl"
#include "System.hpp"

Entity::Entity(int id, int bufferSize, std::string name)
{
    this->id = id;
    this->components.reserve(bufferSize);
    this->name = name;
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