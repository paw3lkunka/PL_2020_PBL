#include "CollisionSystemHelperFunctions.hpp"

#include <glm/gtc/constants.hpp>

#include "Components.inc"
#include "CollisionDataStructures.inl"

Projection1D AxisProjection(SphereCollider* sphere, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2, glm::vec3 projBuffer[])
{
    glm::vec3 centreWS = transform->getModelMatrix() * glm::vec4(sphere->center, 1.0f);
    glm::vec3 projCentre = AxisProjection(centreWS, axisPoint1, axisPoint2);
    float centre1D = ToLineSpace1D(projCentre, axisPoint1, axisPoint2);

    glm::vec3 projectionEdgeOffset = glm::normalize(axisPoint2 - axisPoint1) * sphere->radius;

    projBuffer[0] = projCentre - projectionEdgeOffset;
    projBuffer[1] = projCentre + projectionEdgeOffset;

    return {centre1D - sphere->radius, centre1D + sphere->radius};
}

Projection1D AxisProjection(BoxCollider* box, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2, glm::vec3 projBuffer[])
{
    Projection1D result = {INFINITY, -INFINITY};

    for (glm::vec4& vert : box->verts)
    {
        glm::vec3 vertWS = transform->getModelMatrix() * vert;

        glm::vec3 projVert = AxisProjection(vertWS, axisPoint1, axisPoint2);
        float centre1D = ToLineSpace1D(projVert, axisPoint1, axisPoint2);

        if (centre1D < result.start)
        {
            result.start = centre1D;
            projBuffer[0] = vertWS;
        }
        
        if (centre1D > result.end)
        {
            result.end = centre1D;
            projBuffer[1] = vertWS;
        }
    }

    return result;
}

glm::vec3 AxisProjection(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    //URL: https://gamedev.stackexchange.com/questions/72528/how-can-i-project-a-3d-point-onto-a-3d-line
    glm::vec3 AB = axisPoint2 - axisPoint1;
    glm::vec3 AP = point - axisPoint1;

    return axisPoint1 + glm::dot(AP,AB) / glm::dot(AB,AB) * AB;
}

float ToLineSpace1D(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    glm::vec3 proj0 = AxisProjection(glm::zero<glm::vec3>(), axisPoint1, axisPoint2);
    glm::vec3 diff = point - proj0;
    return glm::length(diff) * glm::sign(glm::dot(diff, axisPoint2 - axisPoint1));
}

glm::vec3 RelativeVelocity(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return collisionNormal * ((rBody1->velocity + glm::cross(rBody1->angularVelocity, r1)) - (rBody2->velocity + glm::cross(rBody2->angularVelocity, r2)));
}

glm::vec3 JImpulse(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal)
{
    return -(1 + glm::e<float>()) * RelativeVelocity(rBody1, rBody2, r1, r2, collisionNormal)
    /
    (
    //TODO I^-1 should be pre computed
        (1 / rBody1->mass) + (1 / rBody2->mass)
        + ( collisionNormal * ( glm::inverse(rBody1->momentOfInertia) * glm::cross(glm::cross(r1, collisionNormal), r1) ) )
        + ( collisionNormal * ( glm::inverse(rBody2->momentOfInertia) * glm::cross(glm::cross(r2, collisionNormal), r2) ) )
    );
}