#ifndef _HYDROACCELERATOR_INL
#define _HYDROACCELERATOR_INL

#include <glm/vec3.hpp>

#include "Component.hpp"

#include "Rigidbody.hpp"

//TODO documentation
struct HydroAccelerator : public Component
{
    HydroAccelerator() = default;
    virtual ~HydroAccelerator() = default;

//TODO documentation
    Rigidbody* rigidbody;

//TODO documentation
    glm::vec3 velocity = glm::vec3(0.0f);

//TODO documentation
    glm::vec3 angularVelocity = glm::vec3(0.0f);

    float handlingMultiplier = 0.08f;
    float acceleratorionMultiplier = 1.2f;
};

#endif // _HYDROACCELERATOR_INL
