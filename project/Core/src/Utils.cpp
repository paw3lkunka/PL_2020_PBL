#include "Utils.hpp"

#include "glm/gtc/quaternion.hpp"

glm::vec3 Vec3Cast(reactphysics3d::Vector3 vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

reactphysics3d::Vector3 Vec3Cast(glm::vec3 vec)
{
    return reactphysics3d::Vector3(vec.x, vec.y, vec.z);
}

glm::quat QuatCast(reactphysics3d::Quaternion quat)
{
    return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

reactphysics3d::Quaternion QuatCast(glm::quat quat)
{
    return reactphysics3d::Quaternion(quat.x, quat.y, quat.z, quat.w);
}