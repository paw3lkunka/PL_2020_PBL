#include "Component.hpp"
#include "Entity.hpp"

const std::string Name(Component& c) { return c.entityPtr ? c.entityPtr->getName() : "Unassigned component"; }
const std::string Name(Component* c) { return c && c->entityPtr ? c->entityPtr->getName() : "Unassigned component"; }
