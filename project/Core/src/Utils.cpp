#include "Utils.hpp"

glm::vec3 vec3cast(reactphysics3d::Vector3 vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}