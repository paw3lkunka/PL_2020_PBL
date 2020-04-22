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

/**
 * @brief vertex data with bones; animation purposes
 */
struct SkinnedVertex : Vertex
{
    static constexpr int MAX_WEIGHTS = 4;

    int boneIDs[MAX_WEIGHTS];
    float weights[MAX_WEIGHTS];
};

/**
 * @brief Min max bounds struct for AABB calculation
 */
struct Bounds
{
    glm::vec3 minBound;
    glm::vec3 maxBound;
};

#endif // _MESHDATASTRUCTURES_INL
