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

    float handling = 10.0f;
    float acceleratorionMultiplier = 2.0f;
};

#endif // _HYDROACCELERATOR_INL
