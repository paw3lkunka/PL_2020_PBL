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
    // Wide phase
    if (!quickDetectCollision(coll1, coll2, trans1, trans2))
    {
        std::cout << "WidePhase: NO." << std::endl;
        return false;
    }
        std::cout << "WidePhase: YES." << std::endl;
    
    //NArrow phase
    //TODO Could be optimized
    std::vector<glm::vec3> axes;

    findSATAxes(coll1, coll2, trans1, trans2, axes);

    return SATTest(coll1, coll2, trans1, trans2, axes);
}
