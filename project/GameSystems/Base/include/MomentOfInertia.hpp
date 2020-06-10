#ifndef MOMENT_OF_INERTIA_HPP
#define MOMENT_OF_INERTIA_HPP

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

rp3d::Vector3 BoxMomentOfInertia(float mass, float width, float height, float depth);
rp3d::Vector3 BoxMomentOfInertia(float mass, glm::vec3 size);
rp3d::Vector3 SphereMomentOfInertia(float mass, float radius, bool hollow = false);

#endif /*MOMENT_OF_INERTIA_HPP*/