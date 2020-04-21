#include "Entity.hpp"

#include "Component.inl"
#include "System.hpp"

Entity::Entity(int id, int bufferSize)
{
    this->id = id;
    this->components.reserve(bufferSize);
}

#include <iostream>

void Entity::addComponent(Component* ptr)
{
    components.push_back(ptr);
    ptr->entityPtr = this;
}

const int Entity::getId() const
{
    return id;
}

const std::vector<Component*>* Entity::getComponentsPtr() const
{
    return &components;
}