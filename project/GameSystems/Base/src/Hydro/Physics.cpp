#include "Hydro/Physics.hpp"

#include "Hydro/Data/HydroTriangle.hpp"
#include "Hydro/Math/TriangleMath.hpp"
#include "Rigidbody.inl"
#include "PhisicStructures.inl"

glm::vec3 HydroPhysics::airResistanceForce(HydroTriangle triangle, glm::vec3 velocity, float resistanceCoefficient, float density)
{
    float speed = glm::length(velocity);
    
    float length = (density * speed * speed * triangle.area * resistanceCoefficient) / 2.0f;
    glm::vec3 direction = -TriangleMath::normalizedProjectOnPlane(velocity, triangle.normal);

    return direction * abs(length) * resistanceCoefficient;
}

glm::vec3 HydroPhysics::buoyancyForce(HydroTriangle triangle, float density, bool upOnly)
{
    //TODO: Proper water height for center
    float height = (triangle.hA + triangle.hB + triangle.hC) / 3.0f; 
    float gravity = glm::length(PhysicStructures::G_CONST);

    float length = density * gravity * height * triangle.area;
    glm::vec3 direction = -triangle.normal;
    if(upOnly)
    {
        direction.x = 0.0f;
        direction.z = 0.0f;
    }

    return direction * abs(length);
}

glm::vec3 HydroPhysics::pressureDragForce(HydroTriangle triangle, glm::vec3 velocity, float pressureDragCoefficient, float suctionDragCoefficient, float pressureFallOfPower, float suctionFallOfPower)
{
    float speed = glm::length(velocity);

    float cosNormalVelocity = glm::dot(triangle.normal, velocity) / ( glm::length(triangle.normal) * glm::length(velocity) );
    float angleNormalVelocity = acosf(cosNormalVelocity) * 180.0f;

    float length;
    glm::vec3 direction;

    if(angleNormalVelocity <= 90.0f)
    {
        direction = -triangle.normal;
        length = pressureDragCoefficient * speed * speed * triangle.area * powf( abs(cosNormalVelocity), pressureFallOfPower );
    }
    else if (angleNormalVelocity >= 90.0f)
    {
        direction = triangle.normal;
        length = suctionDragCoefficient * speed * speed * triangle.area * pow( abs(cosNormalVelocity), suctionFallOfPower );
    }

    return direction * abs(length);
}

float viscousLastSpeedValue = 0.0f;
float viscousResistanceCoefficientValue = 0.0f;

glm::vec3 HydroPhysics::viciousResistanceForce(HydroTriangle triangle, glm::vec3 velocity, float forceMultiply, float density, float viscosity)
{
    float speed = glm::length(velocity);

    if(speed != viscousLastSpeedValue)
    {
        viscousLastSpeedValue = speed;
        viscousResistanceCoefficientValue = viscousResistanceCoefficient(speed, density, viscosity);
    }

    glm::vec3 direction = TriangleMath::normalizedProjectOnPlane(velocity, triangle.normal);
    float length = (density * speed * speed * triangle.area * viscousResistanceCoefficientValue) / 2.0f;
    return direction * abs(length) * forceMultiply;
}

float HydroPhysics::viscousResistanceCoefficient(float speed, float density, float viscosity)
{
    float reynoldsCoefficient = (density * speed) / viscosity;

    float temp = log10f(reynoldsCoefficient) - 2.0f;
    float resistance = 0.075f / (temp * temp);

    return abs(resistance);
}
