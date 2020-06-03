#include "HydroBodySystem.hpp"

#include "Core.hpp"
#include "Event.inl"
#include "Message.inl"
#include "Entity.hpp"

//#include "HydroBody.inl"
#include "HydroAccelerator.inl"
#include "Transform.inl"
#include "Rigidbody.inl"
#include "MeshRenderer.inl"
#include "HydroDataStructures.hpp"
#include "PhisicStructures.inl"


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
    modelTranslation = static_cast<glm::vec3>(transform->getModelMatrix()[3]);
    modelVelocity = hydroAccelerator != nullptr ? hydroAccelerator->velocity : rigidbody->velocity;
    modelAngularVelocity = hydroAccelerator != nullptr ? hydroAccelerator->angularVelocity : rigidbody->angularVelocity;
    
    generateUnderSurfaceMesh();

    //Add forces to the part of the boat that's below the water
    if (underSurfaceTriangleData.size() > 0)
    {
        addUnderSurfaceForces();
    }

    //Add forces to the part of the boat that's above the water
    /*
    if (aboveSurfaceTriangleData.size() > 0)
    {
        addAboveSurfaceForces();
    }
    */
}

void HydroBodySystem::addUnderSurfaceForces()
{
    float lazyDensity = 5.0f;
    
    float Cf = HydroForces::resistanceCoefficient
    (
        lazyDensity,
        glm::length(hydroAccelerator != nullptr ? hydroAccelerator->velocity : rigidbody->velocity),
        fabs(maxSubmergedZ - minSubmergedZ)
    );


    // TODO: Bring slamming force back to life
    //calculateSlammingVelocities();
    //float bodyArea = 0.0f;
    //float bodyMass = rigidbody->mass;

    for (auto it = underSurfaceTriangleData.begin(); it != underSurfaceTriangleData.end(); it++)
    {
        //Calculate the forces
        glm::vec3 forceToAdd(0.0f);

        //Force 1 - The hydrostatic force (buoyancy)
        forceToAdd += HydroForces::buoyancyForce(lazyDensity, *it); // HydroForces::RHO_WATER

        //Force 2 - Viscous Water Resistance
        //forceToAdd += HydroForces::viscousWaterResistanceForce(lazyDensity, *it, Cf);

        //Force 3 - Pressure drag
        //forceToAdd += HydroForces::pressureDragForce(*it);

        //Force 4 - Slamming force

        //Which of the original triangles is this triangle a part of
        // TODO: Bring slamming force back to life
        /*
        int originalTriangleIndex = indexOfOriginalTriangle[it - underSurfaceTriangleData.begin()];
        HydroSlammingForceData slammingData = hydroBody->slammingForceData[originalTriangleIndex];
        
        forceToAdd += HydroForces::slammingForce(slammingData, *it, bodyArea, bodyMass, &slam_acc_max);
        */

        //Force 5 - Wave drifting force
        //forceToAdd += HydroForces::waveDriftingForce(lazyDensity, it->area, it->normal);

        //Add the forces to the boat
        Impulse impulse;
        impulse.force = forceToAdd;
        impulse.point = it->center;

        if(hydroAccelerator == nullptr)
        {
            rigidbody->impulses.push_back(impulse);
        }
        else
        {
            hydroAccelerator->rigidbody->impulses.push_back(impulse);
        }
    }
}

void HydroBodySystem::addAboveSurfaceForces()
{
    //Loop through all triangles
    for (auto it = aboveSurfaceTriangleData.begin(); it != aboveSurfaceTriangleData.end(); it++)
    {
        //Calculate the forces
        glm::vec3 forceToAdd = glm::vec3(0.0f);

        //Force 1 - Air resistance 
        forceToAdd += HydroForces::airResistanceForce(HydroForces::RHO_AIR, *it, HydroForces::C_r);

        //Add the forces to the boat
        Impulse impulse;
        impulse.force = forceToAdd;
        impulse.point = it->center;

        if(hydroAccelerator == nullptr)
        {
            rigidbody->impulses.push_back(impulse);
        }
        else
        {
            hydroAccelerator->rigidbody->impulses.push_back(impulse);
        }
    }
}

// TODO: Bring slamming force back to life
/*
void HydroBodySystem::calculateSlammingVelocities()
{
    for(auto it = hydroBody->slammingForceData.begin(); it != hydroBody->slammingForceData.end(); it++)
    {
        it->previousVelocity = it->velocity;
        glm::vec3 center = modelTranslation + it->triangleCenter;
        it->velocity = HydroForces::getTriangleVelocity(*rigidbody, center);
    }
}
*/

void HydroBodySystem::generateUnderSurfaceMesh()
{
    //Reset
    aboveSurfaceTriangleData.clear();
    underSurfaceTriangleData.clear();
    intersectionVertices.clear();

    bodyVerticesGlobal.clear();
    bodyDistancesToSurface.clear();
    indexOfOriginalTriangle.clear();

    minSubmergedZ = INFINITY;
    maxSubmergedZ = -INFINITY;

    bodyArea = 0.0f;
    slam_acc_max = 0.0f;


    // TODO: Bring slamming force back to life
    //Switch the submerged triangle area with the one in the previous time step
    /*
    for (auto it = hydroBody->slammingForceData.begin(); it != hydroBody->slammingForceData.end(); it++)
    {
        it->previousSubmergedArea = it->submergedArea;
    }
    */

    //Make sure we find the distance to water with the same time
    timeSinceStart = static_cast<float>( GetCore().getCurrentFrameStart() );

    glm::mat4 modelMatrix = transform->getModelMatrix();
    //Find all the distances to water once because some triangles share vertices, so reuse
    for(auto it = meshRenderer->mesh->getVertices()->begin(); it != meshRenderer->mesh->getVertices()->end(); it++)
    {
        //The coordinate should be in global position
        glm::vec3 globalPos = static_cast<glm::vec3>( modelMatrix * glm::vec4(it->position, 1.0f) );
        bodyVerticesGlobal.push_back(globalPos);
        // TODO: Make HydroSurface great again
        bodyDistancesToSurface.push_back( HydroWaves::getDistanceToWave(HydroSurface(), globalPos, timeSinceStart) );
    }

    //Add the triangles
    addTriangles();
}

void HydroBodySystem::addTriangles()
{
    std::vector<HydroVertexData> vertexData(3);

    //Loop through all the triangles (3 vertices at a time = 1 triangle)
    int triangleCounter = 0;
    for(auto it = meshRenderer->mesh->getIndices()->begin(); it != meshRenderer->mesh->getIndices()->end(); it += 3, triangleCounter += 1)
    {
        //Loop through the 3 vertices
        for (int x = 0; x < 3; x++)
        {
            //Save the data we need
            vertexData[x].distanceToSurface = bodyDistancesToSurface[*(it + x)];
            vertexData[x].index = x;
            vertexData[x].modelPosition = bodyVerticesGlobal[*(it + x)];
        }

        //All vertices are above the water
        if (vertexData[0].distanceToSurface > 0.0f && vertexData[1].distanceToSurface > 0.0f && vertexData[2].distanceToSurface > 0.0f)
        {
            //Save the triangle
            aboveSurfaceTriangleData.push_back
            (
                HydroTriangleData
                (
                    vertexData[0].modelPosition, 
                    vertexData[1].modelPosition, 
                    vertexData[2].modelPosition,
                    modelTranslation, 
                    modelVelocity,
                    modelAngularVelocity, 
                    timeSinceStart
                )
            );

            // TODO: Bring slamming force back to life
            //hydroBody->slammingForceData[triangleCounter].submergedArea = 0.0f;

            continue;
        }


        //Create the triangles that are below the waterline

        //All vertices are underwater
        if (vertexData[0].distanceToSurface < 0.0f && vertexData[1].distanceToSurface < 0.0f && vertexData[2].distanceToSurface < 0.0f)
        {
            HydroTriangleData submergedTriangle
            (
                vertexData[0].modelPosition,
                vertexData[1].modelPosition,
                vertexData[2].modelPosition,
                modelTranslation, 
                modelVelocity,
                modelAngularVelocity, 
                timeSinceStart
            );

            checkSubmergedMinMaxZ(submergedTriangle);

            //Save the triangle
            underSurfaceTriangleData.push_back(submergedTriangle);

            //We have already calculated the area of this triangle
            // TODO: Bring slamming force back to life
            //hydroBody->slammingForceData[triangleCounter].submergedArea = hydroBody->slammingForceData[triangleCounter].originalArea;

            indexOfOriginalTriangle.push_back(triangleCounter);
        }
        //1 or 2 vertices are below the water
        else
        {
            //Sort the vertices
            std::sort
            (
                vertexData.begin(), 
                vertexData.end(), 
                [](const HydroVertexData& d1, const HydroVertexData& d2)
                {
                    return d1.distanceToSurface > d2.distanceToSurface;
                }
            );    
            //.Sort((x, y) => x.distance.CompareTo(y.distance));

            //One vertice is above the water, the rest is below
            if (vertexData[0].distanceToSurface > 0.0f && vertexData[1].distanceToSurface < 0.0f && vertexData[2].distanceToSurface < 0.0f)
            {
                addTrianglesOneVertexAboveSurface(vertexData, triangleCounter);
            }
            //Two vertices are above the water, the other is below
            else if (vertexData[0].distanceToSurface > 0.0f && vertexData[1].distanceToSurface > 0.0f && vertexData[2].distanceToSurface < 0.0f)
            {
                addTrianglesTwoVerticesAboveSurface(vertexData, triangleCounter);
            }
        }
    }
}

void HydroBodySystem::addTrianglesOneVertexAboveSurface(std::vector<HydroVertexData> vertexData, int triangleCounter)
{
    //H is always at position 0
    glm::vec3 H = vertexData[0].modelPosition;

    //Left of H is M
    //Right of H is L

    //Find the index of M
    int M_index = vertexData[0].index - 1;
    if (M_index < 0)
    {
        M_index = 2;
    }

    //We also need the heights to water
    float h_H = vertexData[0].distanceToSurface;
    float h_M = 0.0f;
    float h_L = 0.0f;

    glm::vec3 M(0.0f);
    glm::vec3 L(0.0f);

    //This means M is at position 1 in the List
    if (vertexData[1].index == M_index)
    {
        M = vertexData[1].modelPosition;
        L = vertexData[2].modelPosition;

        h_M = vertexData[1].distanceToSurface;
        h_L = vertexData[2].distanceToSurface;
    }
    else
    {
        M = vertexData[2].modelPosition;
        L = vertexData[1].modelPosition;

        h_M = vertexData[2].distanceToSurface;
        h_L = vertexData[1].distanceToSurface;
    }


    //Now we can calculate where we should cut the triangle to form 2 new triangles
    //because the resulting area will always form a square

    //Point I_M
    glm::vec3 MH = H - M;
    float t_M = -h_M / (h_H - h_M);
    glm::vec3 MI_M = t_M * MH;
    glm::vec3 I_M = MI_M + M;

    //Point I_L
    glm::vec3 LH = H - L;
    float t_L = -h_L / (h_H - h_L);
    glm::vec3 LI_L = t_L * LH;
    glm::vec3 I_L = LI_L + L;

    //Save the data, such as normal, area, etc      
    //2 triangles below the water  
    HydroTriangleData triangle1(M, I_M, I_L, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart);
    HydroTriangleData triangle2(M, I_L, L, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart);

    checkSubmergedMinMaxZ(triangle1);
    checkSubmergedMinMaxZ(triangle2);

    underSurfaceTriangleData.push_back(triangle1);
    underSurfaceTriangleData.push_back(triangle2);
    //1 triangle above the water
    aboveSurfaceTriangleData.push_back(HydroTriangleData(I_M, H, I_L, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart));

    //Calculate the total submerged area
    // FIXME: Triangle area for M, I_L, L vertices is nan always...
    float totalArea = HydroForces::getTriangleArea(M, I_M, I_L) + HydroForces::getTriangleArea(M, I_L, L);

    // TODO: Bring slamming force back to life
    //hydroBody->slammingForceData[triangleCounter].submergedArea = totalArea;

    indexOfOriginalTriangle.push_back(triangleCounter);
    //Add 2 times because 2 submerged triangles need to connect to the same original triangle
    indexOfOriginalTriangle.push_back(triangleCounter);

    //Save the vertices that are intersecting between water/air
    intersectionVertices.push_back(I_M);
    intersectionVertices.push_back(I_L);
}

void HydroBodySystem::addTrianglesTwoVerticesAboveSurface(std::vector<HydroVertexData> vertexData, int triangleCounter)
{
    //H and M are above the water
    //H is after the vertice that's below water, which is L
    //So we know which one is L because it is last in the sorted list
    glm::vec3 L = vertexData[2].modelPosition;

    //Find the index of H
    int H_index = vertexData[2].index + 1;
    if (H_index > 2)
    {
        H_index = 0;
    }


    //We also need the heights to water
    float h_L = vertexData[2].distanceToSurface;
    float h_H = 0.0f;
    float h_M = 0.0f;

    glm::vec3 H(0.0f);
    glm::vec3 M(0.0f);

    //This means that H is at position 1 in the list
    if (vertexData[1].index == H_index)
    {
        H = vertexData[1].modelPosition;
        M = vertexData[0].modelPosition;

        h_H = vertexData[1].distanceToSurface;
        h_M = vertexData[0].distanceToSurface;
    }
    else
    {
        H = vertexData[0].modelPosition;
        M = vertexData[1].modelPosition;

        h_H = vertexData[0].distanceToSurface;
        h_M = vertexData[1].distanceToSurface;
    }


    //Now we can find where to cut the triangle

    //Point J_M
    glm::vec3 LM = M - L;

    float t_M = -h_L / (h_M - h_L);

    glm::vec3 LJ_M = t_M * LM;

    glm::vec3 J_M = LJ_M + L;


    //Point J_H
    glm::vec3 LH = H - L;

    float t_H = -h_L / (h_H - h_L);

    glm::vec3 LJ_H = t_H * LH;

    glm::vec3 J_H = LJ_H + L;


    //Save the data, such as normal, area, etc
    //1 triangle above the water
    HydroTriangleData triangle(L, J_H, J_M, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart);

    checkSubmergedMinMaxZ(triangle);

    underSurfaceTriangleData.push_back(triangle);
    //2 triangles below the Surface
    aboveSurfaceTriangleData.push_back(HydroTriangleData(J_H, H, J_M, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart));
    aboveSurfaceTriangleData.push_back(HydroTriangleData(J_M, H, M, modelTranslation, modelVelocity, modelAngularVelocity, timeSinceStart));

    //Calculate the submerged area
    // TODO: Bring slamming force back to life
    // hydroBody->slammingForceData[triangleCounter].submergedArea = HydroForces::getTriangleArea(L, J_H, J_M);

    indexOfOriginalTriangle.push_back(triangleCounter);

    //Save the vertices that are intersecting between water/air
    intersectionVertices.push_back(J_H);
    intersectionVertices.push_back(J_M);
}


// TODO: Bring slamming force back to life
/*
void HydroBodySystem::calculateSubmergedTrianglesArea()
{
    int triangleCounter = 0;

    for(auto it = meshRenderer->mesh->getIndices().begin(); it != meshRenderer->mesh->getIndices().end(); it += 3, triangleCounter++)
    {
        glm::vec3 p1 = meshRenderer->mesh->getVertices()[*it].position;
        glm::vec3 p2 = meshRenderer->mesh->getVertices()[*it + 1].position;
        glm::vec3 p3 = meshRenderer->mesh->getVertices()[*it + 2].position;

        //Calculate the area of the triangle
        float triangleArea = HydroForces::getTriangleArea(p1, p2, p3);
    
        //Store the area in a list
        hydroBody->slammingForceData[triangleCounter].originalArea = triangleArea;
        //The total area
        bodyArea += triangleArea;
    }
}
*/

void HydroBodySystem::checkSubmergedMinMaxZ(HydroTriangleData triangle)
{
    if(triangle.center.z < minSubmergedZ)
    {
        minSubmergedZ = triangle.center.z;
    }

    if(triangle.center.z > maxSubmergedZ)
    {
        maxSubmergedZ = triangle.center.z;
    }
}
