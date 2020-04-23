#ifndef COLLISIONDATASTRUCTURES_INL_
#define COLLISIONDATASTRUCTURES_INL_

#include "Collider.inl"
#include <glm/glm.hpp>

struct __attribute__((packed)) CollisionData
{
    Collider* cause;
    Collider* target;
    glm::vec3 separation;
};

struct TriggerData
{
    Collider* cause;
    Collider* trigger;
};

#endif /* !COLLISIONDATASTRUCTURES_INL_ */