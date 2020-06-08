#ifndef _HYDROCONSTANTS_HPP
#define _HYDROCONSTANTS_HPP

namespace HydroConstants
{
    /// @brief Air density in [kg / cm^3]
    constexpr float RHO_AIR = 0.000001f;
    constexpr float AIR_RESISTANCE_COEFFICIENT = 1.0f;

    /// @brief Water density in [kg / cm^3]
    constexpr float RHO_WATER =  0.001f;

    constexpr bool BUOYANCY_UP_ONLY = false;
    constexpr float BUOYANCY_FORCE_MULTIPLIER = 1.0f;//10000.0f;

    /// @brief Water viscosity in [cm^2 / s] for 18 Celsius degree temperature
    constexpr float VISCOSITY_WATER = 0.010533f;
    constexpr float VISCOUS_FORCE_MULTIPLIER = 300.0f;
    
    constexpr float PRESSURE_DRAG_COEFFICIENT = 1200.0f;
    constexpr float PRESSURE_FALL_OF_POWER = 0.1f;
    constexpr float SUCTION_DRAG_COEFFICIENT = 1200.0f;
    constexpr float SUCTION_FALL_OF_POWER = 0.1f;
};

#endif // _HYDROCONSTANTS_HPP
