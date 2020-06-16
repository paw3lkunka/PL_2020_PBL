#ifndef _HYDROCONSTANTS_HPP
#define _HYDROCONSTANTS_HPP

namespace HydroConstants
{
    /// @brief Air density in [kg / m^3]
    constexpr float RHO_AIR = 1.0f; // [kg / cm^3] -> 0.000001f;
    constexpr float AIR_RESISTANCE_COEFFICIENT = 1.0f;

    /// @brief Water density in [kg / m^3]
    constexpr float RHO_WATER =  1000.0f; // [kg / cm^3] -> 0.001f;

    constexpr bool BUOYANCY_UP_ONLY = false;
    constexpr float BUOYANCY_FORCE_MULTIPLIER = 1.0f; //10000.0f;

    /// @brief Water viscosity for 18 Celsius degree temperature
    constexpr float VISCOSITY_WATER = 0.00001002f; // 0.010533f in [cm^2 / s]
    constexpr float VISCOUS_FORCE_MULTIPLIER = 3.0f;
    
    constexpr float PRESSURE_DRAG_COEFFICIENT = 1.0f;
    constexpr float PRESSURE_FALL_OF_POWER = 0.5f;
    constexpr float SUCTION_DRAG_COEFFICIENT = 1.0f;
    constexpr float SUCTION_FALL_OF_POWER = 0.5f;
};

#endif // _HYDROCONSTANTS_HPP
