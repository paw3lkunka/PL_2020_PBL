#include "CollisionSystem.hpp"

#include "Core.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"
#include "CollisionSystemHelperFunctions.hpp"

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

template<>  // Specialization only for this case
bool CollisionSystem::detectCollsion<SphereCollider, SphereCollider>(SphereCollider* coll1, SphereCollider* coll2, Transform* trans1, Transform* trans2)
{
    std::vector<glm::vec3> axes;

    axes.push_back(trans1->getModelMatrix() * glm::vec4(coll1->center, 1.0f));
    axes.push_back(trans2->getModelMatrix() * glm::vec4(coll2->center, 1.0f));

    return SATTest(coll1, coll2, trans1, trans2, axes);
}

void CollisionSystem::findSATAxes(SphereCollider* collider, Transform* transform, std::vector<glm::vec3>& axes)
{
    return; // Yes, this function should be empty.
}

void CollisionSystem::findSATAxes(BoxCollider* collider, Transform* transform, std::vector<glm::vec3>& axes)
{
    // Local X
    axes.push_back(transform->getModelMatrix() * collider->verts[0]);
    axes.push_back(transform->getModelMatrix() * collider->verts[1]);

    // Local Y
    axes.push_back(transform->getModelMatrix() * collider->verts[0]);
    axes.push_back(transform->getModelMatrix() * collider->verts[4]);

    // Local Z
    axes.push_back(transform->getModelMatrix() * collider->verts[0]);
    axes.push_back(transform->getModelMatrix() * collider->verts[3]);
}