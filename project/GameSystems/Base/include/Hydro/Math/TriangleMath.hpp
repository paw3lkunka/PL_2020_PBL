#ifndef _TRIANGLEMATH_HPP
#define _TRIANGLEMATH_HPP

#include <glm/glm.hpp>

namespace TriangleMath
{
    glm::vec3 getCenter(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    glm::vec3 getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    float getArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    glm::vec3 normalizedProjectOnPlane(glm::vec3 vec, glm::vec3 normal);
};

#endif // _TRIANGLEMATH_HPP
