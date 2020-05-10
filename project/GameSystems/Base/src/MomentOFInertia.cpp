#include "MomentOfInertia.hpp"

glm::mat3 BoxMomentOfInertia(float mass, float width, float height, float depth)
{
    glm::mat3 I = {};
    I[0][0] = (height * height + depth * depth) * mass / 12.0f;
    I[1][1] = (width  * width  + depth * depth) * mass / 12.0f;
    I[2][2] = (height * height + width * width) * mass / 12.0f;
    return I;
}

glm::mat3 SphereMomentOfInertia(float mass, float radius, bool hollow)
{
    float factor = hollow ? (2.0f / 3.0f) : 0.4f;
    glm::mat3 I = {};
    I[0][0] = I[1][1] = I[2][2] = mass * radius * radius * factor;
    return I;
}