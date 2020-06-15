#ifndef _HYDROBODY_INL
#define _HYDROBODY_INL

#include "Component.hpp"

//TODO documentation
struct HydroBody : public Component
{
    HydroBody() = default;
    virtual ~HydroBody() = default;

    glm::vec3 currentVelocity = glm::vec3(0.0f);

    glm::vec3 targetCurrentVelocity = glm::vec3(0.0f);
    float currentVelocityLerp = 1.0f;
};

#endif // _HYDROBODY_INL
