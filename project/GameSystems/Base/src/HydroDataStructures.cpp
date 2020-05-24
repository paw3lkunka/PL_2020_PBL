#include "HydroDataStructures.hpp"

#include "Core.hpp"
#include "Entity.hpp"
#include "Rigidbody.inl"
#include "PhysicSystem.hpp"

TriangleData::TriangleData(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Rigidbody bodyRB, float timeSinceStart)
{
    corners[0] = p0;
    corners[1] = p1;
    corners[2] = p2;

    center = (p0 + p1 + p2) / 3.0f;

    distanceToSurface = abs(1.0f); // TODO: proper function

    normal = glm::normalize( glm::cross(p1 - p0, p2 - p0) );

    area = HydroMath::GetTriangleArea(p0, p1, p2);

    velocity = HydroMath::GetTriangleVelocity(bodyRB, center);

    velocityDirection = glm::normalize(velocity);

    cosTheta = glm::dot(velocityDirection, normal);
}

#pragma region Slamming Force

float HydroMath::acc_max = 0.0f;

#pragma endregion // Slamming Force

#pragma region Triangles Methods

glm::vec3 HydroMath::GetTriangleVelocity(Rigidbody bodyRB, glm::vec3 triangleCenter)
{
    glm::vec3 r_BA = triangleCenter - static_cast<glm::vec3>( bodyRB.entityPtr->getComponentPtr<Transform>()->modelMatrix[3] );
    glm::vec3 v_A = bodyRB.velocity + glm::cross(bodyRB.angularVelocity, r_BA);

    return v_A;
}

float HydroMath::GetTriangleArea(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
    float a = glm::distance(p0, p1);
    float c = glm::distance(p2, p0);
    float area = ( a * c * sinf( acosf( glm::dot(p1 - p0, p2 - p0) ) ) ) / 2.0f;
    
    return area;
}

#pragma endregion // Triangles Methods

glm::vec3 HydroMath::BuoyancyForce(float rho, TriangleData triangleData)
{
    glm::vec3 buoyancyForce = rho * PhysicSystem::G_CONST.y * triangleData.distanceToSurface * triangleData.area * triangleData.normal;
    buoyancyForce.x = 0.0f;
    buoyancyForce.z = 0.0f;
    
    buoyancyForce = CheckForceIsValid(buoyancyForce, "Buoyancy");

    return buoyancyForce;
}

#pragma region Main Resistance Forces

float HydroMath::ResistanceCoefficient(float rho, float velocity, float length)
{
    float Rn = (velocity * length) / VISCOSITY_FACTOR;
    float Cf = 0.075f / powf( ( logf(Rn) - 2.0f ), 2.0f );

    return Cf;
}

glm::vec3 HydroMath::ViscousWaterResistanceForce(float rho, TriangleData triangleData, float Cf)
{
    glm::vec3 A = triangleData.velocity;
    glm::vec3 B = triangleData.normal;

    glm::vec3 velocityTangent = glm::cross( B, glm::cross(A, B) / glm::length(B) ) / glm::length(B);

    // Direction of the tangential velocity (-1 to get the flow which is in the opposite direction)
    glm::vec3 tangentialDirection = glm::normalize(velocityTangent) * (-1.0f);
    
    // Speed center of the triangle but in the direction of the flow
    glm::vec3 v_f_vec = glm::length(A) * tangentialDirection;
    
    glm::vec3 viscousWaterResistanceForce = 0.5f * rho * glm::length(v_f_vec) * v_f_vec * triangleData.area * Cf;
    viscousWaterResistanceForce = CheckForceIsValid(viscousWaterResistanceForce, "Viscous Water Resistance");
    
    return viscousWaterResistanceForce;
}

glm::vec3 HydroMath::PressureDragForce(TriangleData triangleData)
{
    glm::vec3 pressureDragForce(0.0f);
    
    if(triangleData.cosTheta > 0.0f)
    {
        pressureDragForce = -(C_PD1 + C_PD2) * triangleData.area * powf(triangleData.cosTheta, f_P) * triangleData.normal;
    }
    else
    {
        pressureDragForce = (C_SD1 + C_SD2) * triangleData.area * powf( abs(triangleData.cosTheta), f_S ) * triangleData.normal;
    }

    pressureDragForce = CheckForceIsValid(pressureDragForce, "Pressure drag");
    return pressureDragForce;
}

glm::vec3 HydroMath::SlammingForce(SlammingForceData slammingData, TriangleData triangleData, float bodyArea, float bodyMass)
{
    //Add slamming if the normal is in the same direction as the velocity (the triangle is not receding from the water)
    //Also make sure thea area is not 0, which it sometimes is for some reason
    if (triangleData.cosTheta < 0.0f || slammingData.originalArea <= 0.0f)
    {
        return glm::vec3(0.0f);
    }
    
    //Step 1 - Calculate acceleration
    //Volume of water swept per second
    glm::vec3 dV = slammingData.submergedArea * slammingData.velocity;
    glm::vec3 dV_previous = slammingData.previousSubmergedArea * slammingData.previousVelocity;
    
    //Calculate the acceleration of the center point of the original triangle (not the current underwater triangle)
    //But the triangle the underwater triangle is a part of
    glm::vec3 accVec = (dV - dV_previous) / (slammingData.originalArea * Core::FIXED_TIME_STEP_F);
    
    //The magnitude of the acceleration
    float acc = glm::length(accVec);

    //Step 2 - Calculate slamming force
    // F = clamp(acc / acc_max, 0, 1)^p * cos(theta) * F_stop
    // p - power to ramp up slamming force - should be 2 or more
    // F_stop = m * v * (2A / S)
    // m - mass of the entire body
    // v - velocity
    // A - this triangle's area
    // S - total surface area of the entire body
    glm::vec3 F_stop = bodyMass * triangleData.velocity * ((2.0f * triangleData.area) / bodyArea);
    
    if(acc_max < acc)
    {
        acc_max = acc;
    }
    
    glm::vec3 slammingForce = powf( glm::clamp(acc / acc_max, 0.0f, 1.0f), p ) * triangleData.cosTheta * F_stop * slammingCheat;
    
    //The force acts in the opposite direction
    slammingForce *= -1.0f;
    
    slammingForce = CheckForceIsValid(slammingForce, "Slamming");
    return slammingForce;
}

#pragma endregion // Main Resistance Forces

#pragma region Additional Forces

glm::vec3 HydroMath::AirResistanceForce(float rho, TriangleData triangleData, float C_air)
{
    //Only add air resistance if normal is pointing in the same direction as the velocity
    if (triangleData.cosTheta < 0.0f)
    {
        return glm::vec3(0.0f);
    }
    
    //Calculate air resistance force
    glm::vec3 airResistanceForce = 0.5f * rho * glm::length(triangleData.velocity) * triangleData.velocity * triangleData.area * C_air;
    
    //Act in the opposite side of the velocity
    airResistanceForce *= -1.0f;
    airResistanceForce = CheckForceIsValid(airResistanceForce, "Air resistance");
    
    return airResistanceForce;
}

glm::vec3 HydroMath::CalculateWaveDriftingForce(float rho, float area, glm::vec3 normal)
{
    glm::vec3 waveDriftingForce = 0.5f * rho * PhysicSystem::G_CONST.y * area * area * normal;
    waveDriftingForce.y = 0.0f;
    
    return waveDriftingForce;
}

#pragma endregion // Additional Forces

glm::vec3 HydroMath::CheckForceIsValid(glm::vec3 force, std::string forceName)
{
    if (glm::isnan(force.x + force.y + force.z))
    {
        return glm::vec3(0.0f);
    }

    return force;
}
