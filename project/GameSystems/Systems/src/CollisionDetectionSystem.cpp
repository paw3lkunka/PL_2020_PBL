#include "CollisionDetectionSystem.hpp"

#include "Core.hpp"
#include "Entity.hpp"
#include "Components.inc"

void CollisionDetectionSystem::start()
{
    colliders.push_back(colliderPtr);
    transforms.push_back(transformPtr);
}     

bool CollisionDetectionSystem::assertEntity(Entity* entity)
{
    colliderPtr = entity->getComponentPtr<Collider>();
    transformPtr = entity->getComponentPtr<Transform>();
    
    return colliderPtr != nullptr && transformPtr != nullptr;
}

//HACK quick debug output
#include <iostream>

void CollisionDetectionSystem::fixedUpdate()
{
    if(colliderPtr->type != Collider::Type::DYNAMIC)
    {
        return;
    }

    if( auto sphere1 = dynamic_cast<SphereCollider*>(colliderPtr) )
    {
        //TODO finally it can be more human-redable, if this loop will be a function template, cause it might not be replied for each collider type
        for (int i = 0; i < colliders.size(); i++)
        {
            if(sphere1 == colliders[i])
            {
                continue;
            }

            if( auto sphere2 = dynamic_cast<SphereCollider*>(colliders[i]) )
            {
                glm::vec4 separation(collsionSS(sphere1, sphere2, transformPtr, transforms[i]), 0);
                switch (sphere2->type)
                {
                case Collider::Type::KINEMATIC:
                    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * separation);
                    break;
                case Collider::Type::DYNAMIC:
                    transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * (separation * 0.5f));
                    break;
                }
                //std::cout << "SEPARATION_" << i << ": {" << separation.x << ", " << separation.y << ", " << separation.z << "}" << std::endl;
            }
            else
            {
                //TODO Implement other collisions than spheres
            }    
            
        }
    }
    else
    {
        //TODO Implement other collisions than spheres
    }    
}

//-----[ PRIVATES ]--------------------------------

//TODO implement this.
/*
glm::vec3 CollisionDetectionSystem::collsionBB(BoxCollider* of, BoxCollider* with, Transform* ofT, Transform* withT)
{
#pragma region new of AABB in with space   
    glm::mat4 ofToWithT = withT->worldToLocalMatrix * ofT->localToWorldMatrix;
    glm::vec4 vert[8];

    for (int i = 0; i < 8; i++)
    {
        vert[i] = ofToWithT * of->vert[i];
    }

    struct
    {        
        glm::vec3 min;
        glm::vec3 max;
    } a, b;

    b.min = with->center - with->halfSize;
    b.max = with->center + with->halfSize;

    a.min = vert[0];
    a.max = vert[0];

    for (int i = 1; i < 8; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if( a.min[j] > vert[i][j] )
            {
                a.min[j] = vert[i][j];
            }

            if( a.max[j] < vert[i][j] )
            {
                a.max[j] = vert[i][j];
            }
        }            
    }

#pragma endregion
    int condition = 0;
    glm::vec3 separation[6]{};

    if(a.min.x < b.max.x)
    {
        separation[0] = glm::vec3(1.0f,0.0f,0.0f) * (b.min.x - a.max.x);
        condition++;
    }    
    if(a.max.x > b.min.x)
    {
        separation[1] = glm::vec3(-1.0f,0.0f,0.0f) * (a.max.x - b.min.x);
        condition++;
    }    
    if(a.min.x < b.max.y)
    {
        separation[2] = glm::vec3(0.0f,1.0f,0.0f) * (b.min.y - a.max.y);
        condition++;
    }    
    if(a.max.y > b.min.y)
    {
        separation[3] = glm::vec3(0.0f,-1.0f,0.0f) * (a.max.y - b.min.y);
        condition++;
    }    
    if(a.min.z < b.max.z)
    {
        separation[4] = glm::vec3(0.0f,0.0f,1.0f) * (b.min.z - a.max.z);
        condition++;
    }    
    if(a.max.z > b.min.z)
    {
        separation[5] = glm::vec3(0.0f,0.0f,-1.0f) * (a.max.z - b.min.z);
        condition++;
    }
    

}

glm::vec3 CollisionDetectionSystem::collsionBS(BoxCollider* of, SphereCollider* with, Transform* ofT, Transform* withT)
{

}
*/

glm::vec3 CollisionDetectionSystem::collsionSS(SphereCollider* of, SphereCollider* with, Transform* ofT, Transform* withT)
{
    glm::vec3 centre1 = ofT->localToWorldMatrix * glm::vec4(of->center,1);
    glm::vec3 centre2 = withT->localToWorldMatrix * glm::vec4(with->center,1);

    float distance = glm::distance(centre1, centre2);
    float radiiSum = of->radius + with->radius; 
    float difference = radiiSum - distance;

    if(difference > 0)
    {
        return glm::normalize(centre2 - centre1) * -difference;
    }
    else
    {
        return glm::zero<glm::vec3>();
    }
    
}

//TODO IMPLEMENTATION
/*
glm::vec3 CollisionDetectionSystem::collsionSB(SphereCollider* of, BoxCollider* with, Transform* ofT, Transform* withT)
{

}
*/