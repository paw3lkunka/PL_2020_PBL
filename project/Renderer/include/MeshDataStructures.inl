#ifndef _MESHDATASTRUCTURES_INL
#define _MESHDATASTRUCTURES_INL

#include <glm/glm.hpp>
#include <glad/glad.h>


/**
 * @brief Base vertex data structure with only the barebones position, normal and texcoord
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
struct VertexSkinned
{
    static constexpr int MAX_WEIGHTS = 4;

    // IMPORTANT: It's easier to just copy these from Vertex and make offsetof than guess the offset and make mistakes
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;

    unsigned int boneIDs[MAX_WEIGHTS] = {0};
    float weights[MAX_WEIGHTS] = {0.0f};
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
