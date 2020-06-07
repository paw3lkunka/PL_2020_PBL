#ifndef _HYDROTRIANGLEMATH_HPP
#define _HYDROTRIANGLEMATH_HPP

#include <glm/glm.hpp>

namespace TriangleMath
{
    glm::vec3 getCenter(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    glm::vec3 getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    float getArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    glm::vec3 getCenterVelocity(glm::vec3 velocity, glm::vec3 angularVelocity, glm::vec3 triangleCenter, glm::vec3 centerOfMass);
};

#endif // _HYDROTRIANGLEMATH_HPP
