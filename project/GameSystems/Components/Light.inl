#ifndef _LIGHT_INL
#define _LIGHT_INL

#include "Component.inl"

enum class LightType { Directional, Point, Spot };

struct Light : public Component
{
    Light() = default;
    virtual ~Light() = default;

    LightType lightType;
    float intensity;
    glm::vec3 color;
    // For point light
    float range;
    // For spot light
    float angle;
};

#endif // _LIGHT_INL
