#include "Component.hpp"
#include "Entity.hpp"

const std::string Name(Component& c) { return c.entityPtr->getName(); }
const std::string Name(Component* c) { return c->entityPtr->getName(); }
