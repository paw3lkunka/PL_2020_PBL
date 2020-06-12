#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

glm::vec3 Vec3Cast(reactphysics3d::Vector3 vec);
reactphysics3d::Vector3 Vec3Cast(glm::vec3 vec);

glm::quat QuatCast(reactphysics3d::Quaternion quat);
reactphysics3d::Quaternion QuatCast(glm::quat quat);

#endif /* !UTILS_HPP_ */
