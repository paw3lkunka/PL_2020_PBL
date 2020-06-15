#ifndef _HYDROHULLTRIANGLES_HPP
#define _HYDROHULLTRIANGLES_HPP

#include <vector>

#include "Hydro/Data/HydroTriangle.hpp"

/// @brief Structure that stores triangles which are above and under water
struct HullTriangles
{
    HullTriangles() = default;
    virtual ~HullTriangles() = default;

    /// @brief Vector of submerged triangles for a body
    std::vector<HydroTriangle> underwater;

    /// @brief Vector of body triangles which are above the water
    std::vector<HydroTriangle> abovewater;
};


#endif // _HYDROHULLTRIANGLES_HPP
