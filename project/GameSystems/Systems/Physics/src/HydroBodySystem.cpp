#include "HydroBodySystem.hpp"

#include "Core.hpp"
#include "Event.inl"
#include "Message.inl"
#include "Entity.hpp"

#include "HydroBody.inl"
#include "HydroAccelerator.inl"
#include "Transform.hh"
#include "Rigidbody.hpp"
#include "MeshRenderer.inl"

#include "PhisicStructures.inl"
#include "Hydro/Data/HullTriangles.hpp"
#include "Hydro/Data/HydroTriangle.hpp"
#include "Hydro/Math/HullMath.hpp"
#include "Hydro/Math/TriangleMath.hpp"
#include "Hydro/MeshParser.hpp"
#include "Hydro/Physics.hpp"


bool HydroBodySystem::assertEntity(Entity* entity)
{
    hydroBody = entity->getComponentPtr<HydroBody>();
    if(hydroBody == nullptr)
    {
        return false;
    }

    hydroAccelerator = entity->getComponentPtr<HydroAccelerator>();

    transform = entity->getComponentPtr<Transform>();
    if(transform == nullptr)
    {
        return false;
    }

    rigidbody = entity->getComponentPtr<Rigidbody>();
    if(rigidbody == nullptr && hydroAccelerator == nullptr)
    {
        return false;
    }

    meshRenderer = entity->getComponentPtr<MeshRenderer>();
    if(meshRenderer == nullptr)
    {
        return false;
    }

    return true;
}

void HydroBodySystem::fixedUpdate()
{
    Rigidbody* rb;
    glm::vec3 velocity;

    if(hydroAccelerator == nullptr)
    {
        rb = rigidbody;
        velocity = rigidbody->velocity;
    }
    else
    {
        rb = hydroAccelerator->rigidbody;
    }

    std::vector<HydroTriangle> triangles =  HydroMeshParser::parse(meshRenderer->mesh, transform->getModelMatrix());
    HullTriangles hullTriangles = HullMath::cutHull(triangles);

    for(HydroTriangle triangle : hullTriangles.underwater)
    {
        Impulse impulse;

        if(hydroAccelerator == nullptr)
        {
            impulse.force += HydroPhysics::buoyancyForce(triangle);
            impulse.force += HydroPhysics::viciousResistanceForce(triangle, velocity);
            //impulse.force += HydroPhysics::pressureDragForce(triangle, velocity);
        }
        else
        {
            velocity = TriangleMath::getCenterVelocity
            (
                hydroAccelerator->velocity, 
                hydroAccelerator->angularVelocity, 
                triangle.center, 
                static_cast<glm::vec3>(transform->getModelMatrix()[3])
            );

            glm::vec3 force = 0.001f * HydroPhysics::viciousResistanceForce(triangle, velocity);
            force.y = 0.0f;

            impulse.force -= force;
            
            //impulse.force += HydroPhysics::pressureDragForce(triangle, velocity);
        }

        impulse.point = triangle.center; // - static_cast<glm::vec3>(transform->getModelMatrix()[3]);

        rb->impulses.push_back(impulse);
    }

    /*
    for(HydroTriangle triangle : hullTriangles.abovewater)
    {
        Impulse impulse;

        impulse.force += HydroPhysics::airResistanceForce(triangle, velocity);

        impulse.point = triangle.center;

        rb->impulses.push_back(impulse);
    }
    */
}
