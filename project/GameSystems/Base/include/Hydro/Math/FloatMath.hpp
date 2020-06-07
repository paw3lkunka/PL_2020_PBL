#ifndef _HYDROFLOATMATH_HPP
#define _HYDROFLOATMATH_HPP

#include <glm/glm.hpp>

namespace HydroFloatMath
{
    bool approximate(float v1, float v2, float tolerancy = 0.000001f);
    glm::vec3 normalizedProjectOnPlane(glm::vec3 vec, glm::vec3 normal);
} // namespace HydroFloatMath


#endif // _HYDROFLOATMATH_HPP
