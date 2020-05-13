#include "CollisionSystem.hpp"

#include "Core.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"

#include <glm/gtc/epsilon.hpp>

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

/*
//TODO probaby wont be used
template<>
glm::vec3 CollisionSystem::collsion<SphereCollider,SphereCollider>(SphereCollider* sphere1, SphereCollider* sphere2, Transform* transform1, Transform* transform2)
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
*/

Projection1D CollisionSystem::axisProjection(SphereCollider* sphere, glm::vec3 axis, glm::mat4& localToWorld)
{
    glm::vec3 centreWS = static_cast<glm::vec3>(localToWorld * glm::vec4(sphere->center, 1.0f));
    float centre1D = glm::dot(centreWS, axis);
    return {centre1D - sphere->radius, centre1D + sphere->radius};
}


Projection1D CollisionSystem::axisProjection(BoxCollider* box, glm::vec3 axis, glm::mat4& localToWorld)
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

/*
Vectors n1,2,3 explanation:
    v - velocity of collider
    n1 - vector betwen centre of colliders
    n2 = v x n1 
    n3 = n1 x n2
*/

template<>
void CollisionSystem::resolveCollsion<SphereCollider,SphereCollider>(Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //FIXME ignores collider centre
    glm::vec3 n1 = transform2->localToWorldMatrix * glm::vec4(transform2->getLocalPosition(), 1.0f) - transform1->localToWorldMatrix * glm::vec4(transform1->getLocalPosition(), 1.0f);

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
}

template<>
void CollisionSystem::resolveCollsion<SphereCollider,BoxCollider>(Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}

template<>
void CollisionSystem::resolveCollsion<BoxCollider,SphereCollider>(Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}

template<>
void CollisionSystem::resolveCollsion<BoxCollider,BoxCollider>(Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    //TODO implementation
}