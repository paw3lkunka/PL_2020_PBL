#include "Hydro/Data/HydroTriangle.hpp"

#include "Hydro/Math/TriangleMath.hpp"

HydroTriangle::HydroTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    A = a;
    B = b; 
    C = c;

    center = TriangleMath::getCenter(a, b, c);
    normal = TriangleMath::getNormal(a, b, c);
    area = TriangleMath::getArea(a, b, c);
}

HydroTriangle::HydroTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, float hA, float hB, float hC)
    : HydroTriangle(a, b, c)
{
    this->hA = hA;
    this->hB = hB;
    this->hC = hC;
}

