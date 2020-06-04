#ifndef _HYDROACCELERATOR_INL
#define _HYDROACCELERATOR_INL

#include <glm/vec3.hpp>

#include "Component.hpp"

#include "Rigidbody.inl"

struct HydroAccelerator : public Component
{
    HydroAccelerator() = default;
    ~HydroAccelerator() = default;

    Rigidbody* rigidbody;

    glm::vec3 velocity = glm::vec3(0.0f);

    glm::vec3 angularVelocity = glm::vec3(0.0f);
};


#endif // _HYDROACCELERATOR_INL
