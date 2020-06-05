#include "Hydro/Math/TriangleMath.hpp"

#include "Hydro/Data/HydroTriangle.hpp"

glm::vec3 TriangleMath::getCenter(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    return (a + b + c) / 3.0f;
}

glm::vec3 TriangleMath::getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    return glm::normalize( glm::cross(b - a, c - a) );
}

float TriangleMath::getArea(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 AB = b - a;
    glm::vec3 AC = c - a;
    glm::vec3 cross = glm::cross(AC, AB);
    
    return abs( glm::length(cross) ) / 2.0f;
}

glm::vec3 normalizedProjectOnPlane(glm::vec3 vec, glm::vec3 normal)
{
    vec = glm::normalize(vec);
    normal = glm::normalize(normal);
    
    return vec - normal * glm::dot( vec, normal );
}
