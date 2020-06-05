#ifndef _HULLMATH_HPP
#define _HULLMATH_HPP

#include <vector>
#include <glm/vec3.hpp>

struct HydroTriangle;
struct HullTriangles;

namespace HullMath
{
    HullTriangles cutHull(std::vector<HydroTriangle> triangles);
    void processTriangle(HydroTriangle triangle);
    void oneVertexAboveSurface();
    void twoVerticesAboveSurface();
}

#endif // _HULLMATH_HPP
