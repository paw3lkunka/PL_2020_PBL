#ifndef _HYDROCURRENT_INL
#define _HYDROCURRENT_INL

#include "Component.hpp"

struct HydroCurrent : public Component
{
    glm::vec3 velocity = glm::vec3(0.0f);
};

#endif // _HYDROCURRENT_INL
