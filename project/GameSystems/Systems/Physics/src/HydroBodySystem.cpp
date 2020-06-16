#include "HydroBodySystem.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Core.hpp"
#include "Event.inl"
#include "Message.inl"
#include "Entity.hpp"

#include "HydroBody.inl"
#include "HydroAccelerator.inl"
#include "Transform.hh"
#include "Rigidbody.hpp"
#include "MeshRenderer.inl"

#include "PhysicStructures.inl"
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

void HydroBodySystem::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::TRIGGER_ENTER:
            {
                TriggerData data = msg.getValue<TriggerData>();
                HydroBody* body = data.causeBody->entityPtr->getComponentPtr<HydroBody>();
                HydroCurrent* current = data.triggerBody->entityPtr->getComponentPtr<HydroCurrent>();
                if(body != nullptr && current != nullptr)
                {
                    body->targetCurrentVelocity = current->velocity;
                    body->currentVelocityLerp = current->velocityLerp;
                }
            }
            break;
    }
}

void HydroBodySystem::fixedUpdate()
{
    glm::vec3& currentVelocity = hydroBody->currentVelocity;
    currentVelocity.x = std::lerp(currentVelocity.x, hydroBody->targetCurrentVelocity.x, hydroBody->currentVelocityLerp);
    currentVelocity.y = std::lerp(currentVelocity.y, hydroBody->targetCurrentVelocity.y, hydroBody->currentVelocityLerp);
    currentVelocity.z = std::lerp(currentVelocity.z, hydroBody->targetCurrentVelocity.z, hydroBody->currentVelocityLerp);
    
    Rigidbody* rb;
    glm::vec3 velocity;

    std::vector<HydroTriangle> triangles =  HydroMeshParser::parse(meshRenderer->mesh, transform->getModelMatrix());
    HullTriangles hullTriangles = HullMath::cutHull(triangles);

    if(hydroAccelerator == nullptr)
    {
        rb = rigidbody;
        velocity = rigidbody->velocity + currentVelocity;
        float viscousCoefficient = HydroPhysics::viscousResistanceCoefficient( glm::length(velocity) );

        for(HydroTriangle triangle : hullTriangles.underwater)
        {
            Impulse impulse;
            
            impulse.force += HydroPhysics::buoyancyForce(triangle);
            impulse.force += HydroPhysics::viciousResistanceForce(triangle, velocity, viscousCoefficient);
            impulse.force += HydroPhysics::pressureDragForce(triangle, velocity);
            
            impulse.point = triangle.center;
            impulse.type = Impulse::Type::WORLD_SPACE_FORCE;

            rb->impulses.push_back(impulse);
        }

        velocity = rb->velocity;
    }
    else
    {
        rb = hydroAccelerator->rigidbody;
        glm::vec3 centerPos = static_cast<glm::vec3>(transform->getModelMatrix()[3]);

        for(HydroTriangle triangle : hullTriangles.underwater)
        {
            Impulse impulse;

            velocity = TriangleMath::getCenterVelocity
            (
                hydroAccelerator->velocity, 
                hydroAccelerator->angularVelocity, 
                triangle.center, 
                centerPos
            );

            impulse.force = velocity + currentVelocity;
            impulse.force.y = 0.0f;
            if(hydroAccelerator->velocity.y > 0.0f)
            {
                impulse.force *= -1.0f;
            }
            impulse.force *= hydroAccelerator->powerUp;

            impulse.point = triangle.center;
            impulse.type = Impulse::Type::WORLD_SPACE_FORCE;

            rb->impulses.push_back(impulse);
        }

        velocity = hydroAccelerator->velocity;
    }

    // FIXME: This for is causing segfault in rp3d physics for some reason
    // for(HydroTriangle triangle : hullTriangles.abovewater)
    // {
    //     Impulse impulse;

    //     impulse.force += HydroPhysics::airResistanceForce(triangle, velocity);
    //     impulse.point = triangle.center;

    //     rb->impulses.push_back(impulse);
    // }
}
