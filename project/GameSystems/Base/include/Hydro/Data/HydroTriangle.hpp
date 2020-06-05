#ifndef _HYDROTRIANGLE_HPP
#define _HYDROTRIANGLE_HPP

#include <glm/vec3.hpp>

struct HydroTriangle
{
    HydroTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    HydroTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, float hA, float hB, float hC);
    virtual HydroTriangle() = default;
    
    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;

    glm::vec3 center;
    glm::vec3 normal;
    float area;

    float hA;
    float hB;
    float hC;
};


#endif // _HYDROTRIANGLE_HPP
