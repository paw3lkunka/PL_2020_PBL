#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionDataStructures.inl"
#include "Core.hpp"

template<class T>
void CollisionDetectionSystem::collisionOf(T collider)
{
    for (int i = 0; i < colliders.size(); i++)
    {
        if(collider == colliders[i])
        {
            continue;
        }
        if( auto sphere2 = dynamic_cast<SphereCollider*>(colliders[i]) )
        {        
            collisionWith(collider, sphere2, transforms[i]);
        }
        else if( auto box2 = dynamic_cast<BoxCollider*>(colliders[i]) )
        {        
            collisionWith(collider, box2, transforms[i]); 
        } 
        
    }
}

template<class T1, class T2>
void CollisionDetectionSystem::collisionWith(T1 collider1, T2 collider2, Transform* transform2)
{
    glm::vec4 separation(collsion(collider1, collider2, transformPtr, transform2), 0.0f);
    switch (collider2->type)
    {
    case Collider::Type::DYNAMIC:
        separation /= 2.0f;
    case Collider::Type::KINEMATIC:
        {                        
            transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * separation);   
            CollisionData data = {collider1, collider2, separation};
            Message msg(Event::COLLSION_DETECT, data);
            GetCore().messageBus.sendMessage(msg);
        }
        break;
    case Collider::Type::TRIGGER:
        //TODO implement
        break;
    }   
}

#endif