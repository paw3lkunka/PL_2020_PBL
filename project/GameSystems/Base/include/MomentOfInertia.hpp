#ifndef MOMENT_OF_INERTIA_HPP
#define MOMENT_OF_INERTIA_HPP

#include <glm/glm.hpp>

glm::mat3 BoxMomentOfInertia(float mass, float width, float height, float depth);
glm::mat3 SphereMomentOfInertia(float mass, float radius, bool hollow = false);

#endif /*MOMENT_OF_INERTIA_HPP*/