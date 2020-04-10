#ifndef _MESHDATASTRUCTURES_INL
#define _MESHDATASTRUCTURES_INL

#include <glm/glm.hpp>

/**
 * @brief Base vertex data structure with only the barebones position, normal and texcoord
 * 
 */
struct Vertex_base
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

/**
 * @brief Base vertex data with additional color sttribute
 * 
 */
struct Vertex_color : Vertex_base
{
    glm::vec3 color;
};

/**
 * @brief Base vertex data with second uv channel
 * 
 */
struct Vertex_uv2 : Vertex_base
{
    glm::vec2 uv2;
};

#endif // _MESHDATASTRUCTURES_INL
