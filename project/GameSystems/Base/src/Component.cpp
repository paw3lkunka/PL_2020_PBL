#include "Component.inl"
#include "Entity.hpp"

const std::string __attribute__((always_inline)) Name(Component& c) { return c.entityPtr->getName(); }
const std::string __attribute__((always_inline)) Name(Component* c) { return c->entityPtr->getName(); }
