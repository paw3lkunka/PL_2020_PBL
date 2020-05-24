#ifndef HYDRODATASTRUCTURES_HPP_
#define HYDRODATASTRUCTURES_HPP_

#include <string>
#include <glm/glm.hpp>

struct Rigidbody;

struct TriangleData
{
    /**
     * @brief Construct a new Triangle Data object
     * 
     * @param p0 - First vertex of the triangle
     * @param p1 - Second vertex of the triangle
     * @param p2 - Third vertex of the triangle
     * @param bodyRB - Rigidbody of the body triangle belongs to
     * @param timeSinceStart - Time since start of current calculations
     */
    TriangleData(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Rigidbody bodyRB, float timeSinceStart);

    /// @brief Default destructor
    virtual ~TriangleData() = default;

    /// @brief The corners of this triangle in global coordinates
    glm::vec3 corners[3];

    /// @brief The center of the triangle
    glm::vec3 center;

    /// @brief The distance to the surface from the center of the triangle
    float distanceToSurface;

    /// @brief The area of the triangle
    float area;

    /// @brief The normal to the triangle
    glm::vec3 normal;

    /// @brief The velocity of the triangle at the center
    glm::vec3 velocity;

    /// @brief The velocity normalized
    glm::vec3 velocityDirection;

    /**
     * @brief The angle between the normal and the velocity
     * 
     * Negative if pointing in the opposite direction
     * Positive if pointing in the same direction
     */
    float cosTheta;
};

/// @brief Data that belongs to one triangle in the original body mesh and is needed to calculate the slamming force
struct SlammingForceData 
{
    /// @brief The area of the original triangles - calculate once in the beginning because always the same
    float originalArea;
    /// @brief How much area of a triangle in the whole body is submerged
    float submergedArea;
    /// @brief Same as above but previous time step
    float previousSubmergedArea;
    /// @brief Need to save the center of the triangle to calculate the velocity
    glm::vec3 triangleCenter;
    /// @brief Velocity in current time step
    glm::vec3 velocity;
    /// @brief Velocity in previous time step
    glm::vec3 previousVelocity;
};

/**
 * @brief Collection of necessary methods made for calculating hydrodynamic and hydrostatic forces
 * 
 * It was based on articles by Jacques Kerner, a senior software engineer at Avalanche Studios.
 * 
 * Part 1 (buoyancy forces):
 * https://www.gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php
 * 
 * Part 2 (resistance forces):
 * http://www.gamasutra.com/view/news/263237/Water_interaction_model_for_boats_in_video_games_Part_2.php
 * 
 */
struct HydroMath
{
#pragma region Densities Constants
    
    /// @brief Clean water density
    static constexpr float RHO_WATER = 1000.0f;
    
    /// @brief Ocean water density
    static constexpr float RHO_OCEAN_WATER = 1027.0f;
    
    /// @brief Sunflower oil density
    static constexpr float RHO_SUNFLOWER_OIL = 920.0f;
    
    /// @brief Milk density
    static constexpr float RHO_MILK = 1035.0f;
    
    /// @brief Air density
    static constexpr float RHO_AIR = 1.225f;
    
    /// @brief Helium density
    static constexpr float RHO_HELIUM = 0.164f;
    
    /// @brief Gold density
    static constexpr float RHO_GOLD = 19300.0f;

#pragma endregion // Densities Constants

#pragma region Neglected Dynamic Coefficients

    /// @brief Drag coefficient for plate perpendicular to fluid flow
    static constexpr float C_D_PERPENDICULAR = 1.28f;

    /// @brief Viscosity depends on the temperature, this is for 20 celsius degrees
    static constexpr float VISCOSITY_FACTOR = 0.000001f; // or 0.00001002f

#pragma endregion // Neglected Dynamic Coefficients

#pragma region Pressure Drag Coefficients

    /// @brief Linear coefficient for Pressure Drag Force
    static constexpr float C_PD1 = 10.0f;

    /// @brief Quadratic coefficient for Pressure Drag Force
    static constexpr float C_PD2 = 10.0f;
    
    /// @brief Falloff power for Pressure Drag Force, should be smaller than (0, 1)
    static constexpr float f_P = 0.5f;

#pragma endregion // Pressure Drag Coefficients

#pragma region Suction Drag

    /// @brief Linear coefficient for Suction Drag Force
    static constexpr float C_SD1 = 10.0f;
    
    /// @brief Quadratic coefficient for Suction Drag Force
    static constexpr float C_SD2 = 10.0f;
    
    /// @brief Falloff power for Pressure Drag Force, should be smaller than (0, 1)
    static constexpr float f_S = 0.5f;

#pragma endregion // Suction Drag

#pragma region Slamming Force

    /// @brief Power-up for a slamming force
    static constexpr float p = 2.0f;
    
    /// @brief Maximal witnessed acceleration during the fall
    static float acc_max;
    
    /// @brief Cheat used to make slamming force prettier
    static constexpr float slammingCheat = 1.0f;

#pragma endregion // Slamming Force

#pragma region Triangles Methods

    /**
     * @brief Calculate the velocity at the center of the triangle
     * 
     * The connection formula for velocities
     * v_A = v_B + omega_B cross r_BA
     * 
     * v_A - velocity in point A
     * v_B - velocity in point B
     * omega_B - angular velocity in point B
     * r_BA - vector between A and B
     * 
     * @param bodyRB - Rigid body of floating body
     * @param triangleCenter - Position of triangle center in world space
     * @return glm::vec3 - Triangle velocity
     */
    static glm::vec3 GetTriangleVelocity(Rigidbody bodyRB, glm::vec3 triangleCenter);

    /**
     * @brief Calculate the area of a triangle with three coordinates
     * 
     * Uses Sinus formula
     * 
     * @param p0 - First triangle vertex position in world space
     * @param p1 - Second triangle vertex position in world space
     * @param p2 - Third triangle vertex position in world space
     * @return float - Triangle area
     */
    static float GetTriangleArea(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

#pragma endregion // Triangles Methods

    /**
     * @brief Calculate buoyancy force for single triangle
     * 
     * Buoyancy is a hydrostatic force - it's there even if the water isn't flowing or if the body stays still
     * 
     * F_buoyancy = rho * g * V
     * rho - density of the medium you are in
     * g - gravity
     * V - volume of fluid directly above the curved surface 
     * 
     * V = z * S * n 
     * z - distance to surface
     * S - surface area
     * n - normal to the surface
     * 
     * @param rho - Density of the medium body is in
     * @param triangleData - Triangle of the body to calculate force for
     * @return glm::vec3 - Buoyancy force for a triangle
     */
    static glm::vec3 BuoyancyForce(float rho, TriangleData triangleData);

#pragma region Main Resistance Forces

    /**
     * @brief The Coefficient of frictional resistance - belongs to Viscous Water Resistance but is same for all so calculate once
     * 
     * Reynolds number
     * 
     * Rn = (V * L) / nu
     * V - speed of the body
     * L - length of the submerged body
     * nu - viscosity of the fluid [m^2 / s]
     * 
     * @param rho - Density of the medium body is in
     * @param velocity - Velocity of the body
     * @param length - Length of the submerged body
     * @return float 
     */
    static float ResistanceCoefficient(float rho, float velocity, float length);

    /**
     * @brief Viscous Water Resistance (Frictional Drag) for triangle
     * 
     * Viscous resistance occurs when water sticks to the body's surface and the body has to drag that water with it
     * 
     * F = 0.5 * rho * v^2 * S * Cf
     * rho - density of the medium body is in
     * v - speed
     * S - surface area
     * Cf - Coefficient of frictional resistance
     * 
     * @param rho - Density of the medium body is in
     * @param triangleData - Data of the triangle to calculate resistance for
     * @param Cf - Coefficient of frictional resistance
     * @return glm::vec3 - Viscous fluid resistance force
     */
    static glm::vec3 ViscousWaterResistanceForce(float rho, TriangleData triangleData, float Cf);

    /**
     * @brief Pressure Drag Force for triangle
     * 
     * Modify for different turning behavior and planing forces
     * f_p and f_S - falloff power, should be smaller than 1
     * C - coefficients to modify 
     * 
     * @param triangleData Data of the triangle to calculate resistance for
     * @return glm::vec3 - Pressure Drag Force
     */
    static glm::vec3 PressureDragForce(TriangleData triangleData);

    /**
     * @brief Slamming Force (Water Entry Force)
     * 
     * Force destined to capture the response of the fluid to sudden accelerations or penetrations.
     * 
     * Further details available in definition itsefl.
     * 
     * @param slammingData - Data for slamming force
     * @param triangleData - Data for triangle we calculate force for
     * @param bodyArea - Area of the floating body
     * @param bodyMass - Mass of the floating body
     * @return glm::vec3 - Slamming Force
     */
    static glm::vec3 SlammingForce(SlammingForceData slammingData, TriangleData triangleData, float bodyArea, float bodyMass);

#pragma endregion // Main Resistance Forces

#pragma region Additional Forces

    /**
     * @brief Air resistance on the part of the body above the water
     * 
     * It is typically 4 to 8 percent of total resistance
     * 
     * R_air = 0.5 * rho * v^2 * A_p * C_air
     * rho - air density
     * v - speed of ship
     * A_p - projected transverse profile area of ship
     * C_air - coefficient of air resistance (drag coefficient)
     * 
     * @param rho - Density of the air
     * @param triangleData - Data of the triangle resistance acts on
     * @param C_air - Coefficient of air resistance (drag coefficient)
     * @return glm::vec3 - Air resistance force
     */
    static glm::vec3 AirResistanceForce(float rho, TriangleData triangleData, float C_air);

    /**
     * @brief Calculate the wave drifting force so the body can float with the waves
     * 
     * F = rho * g * A^2 * n 
     * rho - density of the medium
     * g - gravity
     * A - area
     * n - normal
     * 
     * Based on:
     * http://ocw.mit.edu/courses/mechanical-engineering/2-019-design-of-ocean-systems-spring-2011/lecture-notes/MIT2_019S11_DVL1.pdf
     * 
     * @param rho - Density of the medium
     * @param area - Area of the triangle force acts on
     * @param normal - Normal to the triangle surface 
     * @return glm::vec3 - Wave drifting force
     */
    static glm::vec3 CalculateWaveDriftingForce(float rho, float area, glm::vec3 normal);

#pragma endregion // Additional Forces

    /**
     * @brief Check that a force is not NaN
     * 
     * @param force - Force to check
     * @param forceName - Force name in case of a problem
     * 
     * @returns glm::vec3 - If force is valid
     * @returns glm::vec3(0,0,0) - If force invalid  
     */
    static glm::vec3 CheckForceIsValid(glm::vec3 force, std::string forceName);
};

#endif /* !HYDRODATASTRUCTURES_INL_ */