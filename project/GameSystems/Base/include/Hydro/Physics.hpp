#ifndef _HYDROPHYSICS_HPP
#define _HYDROPHYSICS_HPP

#include <glm/glm.hpp>

#include "Hydro/Constants.hpp"

struct HydroTriangle;
struct Rigidbody;

namespace HydroPhysics
{
    glm::vec3 airResistanceForce
    (
        HydroTriangle triangle, 
        glm::vec3 velocity, 
        float resistanceCoefficient = HydroConstants::AIR_RESISTANCE_COEFFICIENT, 
        float density = HydroConstants::RHO_AIR
    );
    
    glm::vec3 buoyancyForce
    (
        HydroTriangle triangle, 
        float density = HydroConstants::RHO_WATER, 
        bool upOnly = HydroConstants::BUOYANCY_UP_ONLY, 
        float buoyancyForceMultiplier = HydroConstants::BUOYANCY_FORCE_MULTIPLIER
    );
    
    glm::vec3 pressureDragForce
    (
        HydroTriangle triangle, 
        glm::vec3 velocity, 
        float pressureDragCoefficient = HydroConstants::PRESSURE_DRAG_COEFFICIENT, 
        float suctionDragCoefficient = HydroConstants::SUCTION_DRAG_COEFFICIENT, 
        float pressureFallOfPower = HydroConstants::PRESSURE_FALL_OF_POWER, 
        float suctionFallOfPower = HydroConstants::SUCTION_FALL_OF_POWER
    );
    
    glm::vec3 viciousResistanceForce
    (
        HydroTriangle triangle, 
        glm::vec3 velocity, 
        float forceMultiply = HydroConstants::VISCOUS_FORCE_MULTIPLIER, 
        float density = HydroConstants::RHO_WATER, 
        float viscosity = HydroConstants::VISCOSITY_WATER
    );
    
    float viscousResistanceCoefficient(float speed, float density, float viscosity);
};

#endif // _HYDROPHYSICS_HPP
