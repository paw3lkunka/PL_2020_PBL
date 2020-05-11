#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionDataStructures.inl"
#include "Message.inl"
#include "Core.hpp"


template<class T1, class T2>
bool CollisionDetectionSystem::collsion(T1* coll1, T2* coll2, Transform* trans1, Transform* trans2)
{
    glm::vec4 axis = trans2->localToWorldMatrix * glm::vec4(trans2->getLocalPosition(), 1.0f)
                     - trans1->localToWorldMatrix * glm::vec4(trans1->getLocalPosition(), 1.0f);

    Projection1D proj1 = axisProjection(coll1, static_cast<glm::vec3>(axis), trans1->localToWorldMatrix);
    Projection1D proj2 = axisProjection(coll2, static_cast<glm::vec3>(axis), trans2->localToWorldMatrix);    

    return (proj1.start < proj2.start && proj1.end > proj2.start) || (proj2.start < proj1.start && proj2.end > proj1.start);
}

template<class T>
void CollisionDetectionSystem::collisionOf(T* collider)
{
    for (int i = 0; i < colliders.size(); i++)
    {
        if(collider == colliders[i])
        {
            continue;
        }
        if( SphereCollider* sphere2 = dynamic_cast<SphereCollider*>(colliders[i]) )
        {
            collisionWith(collider, sphere2, transforms[i]);
        }
        else if( BoxCollider* box2 = dynamic_cast<BoxCollider*>(colliders[i]) )
        {        
            collisionWith(collider, box2, transforms[i]); 
        } 
        
    }
}

template<class T1, class T2>
void CollisionDetectionSystem::collisionWith(T1* collider1, T2* collider2, Transform* transform2)
{
    if(collsion(collider1, collider2, transformPtr, transform2))
    {
        std::cout << "Collision detected - type: " << (int)collider2->type << std::endl;
        switch (collider2->type)
        {
        case Collider::Type::DYNAMIC:
            //separation /= 2.0f;
        case Collider::Type::KINEMATIC:
            /*{                        
                transformPtr->getLocalPositionModifiable() += static_cast<glm::vec3>(transformPtr->worldToLocalMatrix * separation);   
                CollisionData data = {collider1, collider2, separation};
                Message msg(Event::COLLSION_DETECT, data);
                GetCore().messageBus.sendMessage(msg);
            }*/
            //TODO implement phisic based collision
            break;
        case Collider::Type::TRIGGER:
            TriggerData data = {collider1, collider2};
            if(activeTriggers.find(data) == activeTriggers.end())
            {
                GetCore().messageBus.sendMessage( Message(Event::TRIGGER_ENTER, data) );
                activeTriggers.insert(data);
            }
            break;
        }   
    }
    else if(collider2->type == Collider::Type::TRIGGER)
    {   
        TriggerData data = {collider1, collider2};
        if(activeTriggers.find(data) != activeTriggers.end())
        {
            GetCore().messageBus.sendMessage( Message(Event::TRIGGER_EXIT, data) );
            activeTriggers.erase(data);
        }
    }
}

#endif