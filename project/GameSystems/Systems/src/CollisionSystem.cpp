#include "CollisionSystem.hpp"

#include "Core.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/string_cast.hpp>

#include "CollisionDataStructures.inl"

void CollisionSystem::start()
{
    colliders.push_back(colliderPtr);
    transforms.push_back(transformPtr);
    rigidbodies.push_back(rigidbodyPtr);
}     

bool CollisionSystem::assertEntity(Entity* entity)
{
    colliderPtr = entity->getComponentPtr<Collider>();
    transformPtr = entity->getComponentPtr<Transform>();
    rigidbodyPtr = entity->getComponentPtr<Rigidbody>();
    
    return colliderPtr != nullptr && transformPtr != nullptr;
}

void CollisionSystem::fixedUpdate()
{
    if(colliderPtr->type != Collider::Type::DYNAMIC)
    {
        return;
    }

    if( SphereCollider* sphere1 = dynamic_cast<SphereCollider*>(colliderPtr) )
    {
        collisionOf(sphere1);
    }
    else if( BoxCollider* box1 = dynamic_cast<BoxCollider*>(colliderPtr) )
    {
        collisionOf(box1);
    }  
}

Projection1D CollisionSystem::axisProjection(SphereCollider* sphere, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    glm::vec3 centreWS = transform->localToWorldMatrix * glm::vec4(transform->getLocalPosition() + sphere->center, 1.0f);
    glm::vec3 projCentre = axisProjection(centreWS, axisPoint1, axisPoint2);
    float centre1D = toLineSpace1D(projCentre, axisPoint1, axisPoint2);

    return {centre1D - sphere->radius, centre1D + sphere->radius};
}

Projection1D CollisionSystem::axisProjection(BoxCollider* box, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    Projection1D result = {INFINITY, -INFINITY};

    for (glm::vec4& vert : box->verts)
    {
        glm::vec3 vertWS = transform->localToWorldMatrix * vert;

        glm::vec3 projVert = axisProjection(vertWS, axisPoint1, axisPoint2);
        float centre1D = toLineSpace1D(projVert, axisPoint1, axisPoint2);

        if (centre1D < result.start)
        {
            result.start = centre1D;
        }
        
        if (centre1D > result.end)
        {
            result.end = centre1D;
        }
    }

    return result;
}

glm::vec3 CollisionSystem::axisProjection(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    //URL: https://gamedev.stackexchange.com/questions/72528/how-can-i-project-a-3d-point-onto-a-3d-line
    glm::vec3 AB = axisPoint2 - axisPoint1;
    glm::vec3 AP = point - axisPoint1;

    return axisPoint1 + glm::dot(AP,AB) / glm::dot(AB,AB) * AB;
}

float CollisionSystem::toLineSpace1D(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2)
{
    glm::vec3 proj0 = axisProjection(glm::zero<glm::vec3>(), axisPoint1, axisPoint2);
    glm::vec3 diff = point - proj0;
    return glm::length(diff) * glm::sign(glm::dot(diff, axisPoint2 - axisPoint1));
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Annotation for resolveCollsion()             *
 *  Vectors n1,2,3 explanation:                  *
 *      v - velocity of collider                 *
 *      n1 - vector betwen centre of colliders   *
 *      n2 = v x n1                              *
 *      n3 = n1 x n2                             *
 * * * * * * * * * * * * * * * * * * * * * * * * *
 */

template<>
void CollisionSystem::resolveCollsion<SphereCollider,SphereCollider>(SphereCollider* collider1, SphereCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    glm::vec3 n1 = transform2->localToWorldMatrix * glm::vec4(transform2->getLocalPosition() + collider2->center, 1.0f)
    - transform1->localToWorldMatrix * glm::vec4(transform1->getLocalPosition() + collider2->center, 1.0f);

    n1 = glm::normalize(n1);

    body1->velocity =
    (
        body1->mass * body1->velocity 
        + body2->mass * body2->velocity
        + body2->mass * glm::e<float>() * ( body2->velocity - body1->velocity)
    )
    / ( body1->mass + body2->mass );

    body1->angularVelocity = 
    glm::cross
    (
        n1,
        glm::cross(n1, body1->angularVelocity) + glm::vec3(0.0f, body2->velocity.y, body2->velocity.z)
    );

    std::cout << "n1" <<  glm::to_string(n1) << "\n"
            << "AngV1:" <<  glm::to_string(body1->angularVelocity) << "\n"
            << "AngV2: " << glm::to_string(body2->angularVelocity) << std::endl;
}

template<>
void CollisionSystem::resolveCollsion<SphereCollider,BoxCollider>(SphereCollider* collider1, BoxCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}

template<>
void CollisionSystem::resolveCollsion<BoxCollider,SphereCollider>(BoxCollider* collider1, SphereCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}

template<>
void CollisionSystem::resolveCollsion<BoxCollider,BoxCollider>(BoxCollider* collider1, BoxCollider* collider2,  Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}