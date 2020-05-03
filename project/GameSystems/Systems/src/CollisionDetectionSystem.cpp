#include "CollisionDetectionSystem.hpp"

#include "Core.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"

#include "CollisionDataStructures.inl"

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

void CollisionDetectionSystem::fixedUpdate()
{
    if(colliderPtr->type != Collider::Type::DYNAMIC)
    {
        return;
    }

    if( auto sphere1 = dynamic_cast<SphereCollider*>(colliderPtr) )
    {
        collisionOf(sphere1);
    }
    else if( auto box1 = dynamic_cast<BoxCollider*>(colliderPtr) )
    {
        collisionOf(box1);
    }  
}


template<>
glm::vec3 CollisionDetectionSystem::collsion<SphereCollider,SphereCollider>(SphereCollider* sphere1, SphereCollider* sphere2, Transform* transform1, Transform* transform2)
{
    glm::vec3 centre1 = transform1->localToWorldMatrix * glm::vec4(sphere1->center,1);
    glm::vec3 centre2 = transform2->localToWorldMatrix * glm::vec4(sphere2->center,1);

    float distance = glm::distance(centre1, centre2);
    float radiiSum = sphere1->radius + sphere2->radius; 
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

template<>
glm::vec3 CollisionDetectionSystem::collsion<BoxCollider, BoxCollider>(BoxCollider* coll1, BoxCollider* coll2, Transform* trans1, Transform* trans2)
{
    glm::vec4 faceNormals[6]
    {
        trans1->localToWorldMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        trans1->localToWorldMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        trans1->localToWorldMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),

        trans2->localToWorldMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        trans2->localToWorldMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        trans2->localToWorldMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    };

    glm::vec3 separation = {0.0f, 0.0f, 0.0f};
    
    //TODO unfinished

    for (auto& line: faceNormals)
    {
        Projection1D proj1 = axisProjection(coll1, static_cast<glm::vec3>(line), trans1->localToWorldMatrix);
        Projection1D proj2 = axisProjection(coll2, static_cast<glm::vec3>(line), trans2->localToWorldMatrix);

        if (proj1.end < proj2.start || proj1.end < proj2.start)
        {
            
        }
    }
    return separation;
}

//TODO not implemented
template<>
glm::vec3 CollisionDetectionSystem::collsion<BoxCollider,SphereCollider>(BoxCollider* box1, SphereCollider* sphere2, Transform* transform1, Transform* transform2)
{
    return {1,2,3};
}

//TODO not implemented
template<>
glm::vec3 CollisionDetectionSystem::collsion<SphereCollider,BoxCollider>(SphereCollider* sphere1, BoxCollider* box2, Transform* transform1, Transform* transform2)
{
    return {1,2,3};
}


Projection1D CollisionDetectionSystem::axisProjection(SphereCollider* sphere, glm::vec3 axis, glm::mat4& localToWorld)
{
    glm::vec3 centreWS = static_cast<glm::vec3>(localToWorld * glm::vec4(sphere->center, 1.0f));
    float centre1D = glm::dot(centreWS, axis);
    return {centre1D - sphere->radius, centre1D + sphere->radius};
}

Projection1D CollisionDetectionSystem::axisProjection(BoxCollider* box, glm::vec3 axis, glm::mat4& localToWorld)
{
    Projection1D result = {INFINITY, -INFINITY};

    for (auto point : box->vert)
    {
        float point1D = glm::dot(static_cast<glm::vec3>(localToWorld * point), axis);

        result.start = glm::min(result.start, point1D);
        result.end = glm::min(result.end, point1D);
    }

    return result;
}