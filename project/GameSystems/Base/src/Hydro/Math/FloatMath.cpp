#include "Hydro/Math/FloatMath.hpp"

bool HydroFloatMath::approximate(float v1, float v2, float tolerancy)
{
    return glm::abs(v1 - v2) <= tolerancy;
}

glm::vec3 HydroFloatMath::normalizedProjectOnPlane(glm::vec3 vec, glm::vec3 normal)
{
    if(approximate(glm::length(vec), 0.0f) == false)
    {
        vec = glm::normalize(vec);
    }
    normal = glm::normalize(normal);
    
    return vec - normal * glm::dot( vec, normal );
}