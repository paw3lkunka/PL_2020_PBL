#ifndef _MESHDATASTRUCTURES_INL
#define _MESHDATASTRUCTURES_INL

#include <glm/glm.hpp>
#include <glad/glad.h>

/**
 * @brief Base vertex data structure with only the barebones position, normal and texcoord
 * 
 */
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

#endif // _MESHDATASTRUCTURES_INL
