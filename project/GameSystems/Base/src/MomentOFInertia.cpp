#include "MomentOfInertia.hpp"

rp3d::Vector3 BoxMomentOfInertia(float mass, float width, float height, float depth)
{
    rp3d::Vector3 I = {};
    I.x = (height * height + depth * depth) * mass / 12.0f;
    I.y = (width  * width  + depth * depth) * mass / 12.0f;
    I.z = (height * height + width * width) * mass / 12.0f;
    return I;
}

rp3d::Vector3 BoxMomentOfInertia(float mass, glm::vec3 size)
{
    rp3d::Vector3 I = {};
    I.x = (size.y * size.y + size.z * size.z) * mass / 12.0f;
    I.y = (size.x * size.x + size.z * size.z) * mass / 12.0f;
    I.z = (size.y * size.y + size.x * size.x) * mass / 12.0f;
    return I;
}

rp3d::Vector3 SphereMomentOfInertia(float mass, float radius, bool hollow)
{
    float factor = hollow ? (2.0f / 3.0f) : 0.4f;
    rp3d::Vector3 I = {1.0f, 1.0f, 1.0f};
    I *= mass * radius * radius * factor;
    return I;
}