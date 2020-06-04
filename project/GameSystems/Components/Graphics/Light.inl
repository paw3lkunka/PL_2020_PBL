#ifndef _LIGHT_INL
#define _LIGHT_INL

#include "Component.hpp"

#include <glm/glm.hpp>

enum class LightType { Directional, Point, Spot };

//TODO documentation
struct Light : public Component
{
    Light() = default;
    virtual ~Light() = default;

    // HACK: This is potentially unsafe
    glm::mat4 const* modelMatrix;

//TODO documentation
    LightType lightType;
//TODO documentation
    float intensity;
//TODO documentation
    glm::vec3 color;
    // For point light
//TODO documentation
    float range;
    // For spot light
//TODO documentation
    float angle;
};

#endif // _LIGHT_INL
