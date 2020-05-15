#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionDataStructures.inl"
#include "Message.inl"
#include "Core.hpp"


template<class T1, class T2>
bool CollisionSystem::collsion(T1* coll1, T2* coll2, Transform* trans1, Transform* trans2)
{
    //TODO get collider centre
    //FIXME works wrong - this is not a correct SAT test implementation - this works only for sphere
    glm::vec4 glob1 = trans1->localToWorldMatrix * glm::vec4(trans1->getLocalPosition() + coll1->center, 1.0f);
    glm::vec4 glob2 = trans2->localToWorldMatrix * glm::vec4(trans2->getLocalPosition() + coll2->center, 1.0f);

    Projection1D proj1 = axisProjection(coll1, trans1, glob1, glob2);
    Projection1D proj2 = axisProjection(coll2, trans2, glob1, glob2);    

    std::cout << '\n'
            << "SPHERE 1 ---------------------- " << '\n'
            << "local position: " << trans1->getLocalPosition().x << ", " << trans1->getLocalPosition().y << ", " << trans1->getLocalPosition().z << '\n'
            << "Collider centre (local): " << coll1->center.x << ", " << coll1->center.y << ", " << coll1->center.z << '\n'
            << "Collider centre (global): " << glob1.x << ", " << glob1.y << ", " << glob1.z << '\n'
            << "Axis projection: " << proj1.start << ", " << proj1.end << '\n'
            << '\n'
            << "SPHERE 2 ---------------------- " << '\n'
            << "local position: " << trans2->getLocalPosition().x << ", " << trans2->getLocalPosition().y << ", " << trans2->getLocalPosition().z << '\n'
            << "Collider centre (local): " << coll2->center.x << ", " << coll2->center.y << ", " << coll2->center.z << '\n'
            << "Collider centre (global): " << glob2.x << ", " << glob2.y << ", " << glob2.z << '\n'
            << "Axis projection: " << proj2.start << ", " << proj2.end << '\n'
            << std::endl;

    return (proj1.start < proj2.start && proj1.end > proj2.start) || (proj2.start < proj1.start && proj2.end > proj1.start);
}

template<class T>
void CollisionSystem::collisionOf(T* collider)
{
    for (int i = 0; i < colliders.size(); i++)
    {
        if(collider == colliders[i])
        {
            continue;
        }
        if( SphereCollider* sphere2 = dynamic_cast<SphereCollider*>(colliders[i]) )
        {
            collisionWith(collider, sphere2, transforms[i], rigidbodies[i]);
        }
        else if( BoxCollider* box2 = dynamic_cast<BoxCollider*>(colliders[i]) )
        {        
            collisionWith(collider, box2, transforms[i], rigidbodies[i]); 
        } 
        
    }
}

template<class T1, class T2>
void CollisionSystem::collisionWith(T1* collider1, T2* collider2, Transform* transform2, Rigidbody* rigidbody2)
{
    if(collsion(collider1, collider2, transformPtr, transform2))
    {
        //TODO TEMP DEBUG
        std::cout << "Collision detected - type: " << (int)collider2->type << std::endl;
        CollisionData data = {collider1, collider2};

        switch (collider2->type)
        {
            case Collider::Type::DYNAMIC:
            case Collider::Type::KINEMATIC:
            {
                resolveCollsion<T1,T2>(rigidbodyPtr, rigidbody2, transformPtr, transform2);

                Message msg(Event::COLLSION_DETECT, data);
                GetCore().messageBus.sendMessage(msg);
                break;
            }

            case Collider::Type::TRIGGER:
            {
                if(activeTriggers.find(data) == activeTriggers.end())
                {
                    GetCore().messageBus.sendMessage( Message(Event::TRIGGER_ENTER, data) );
                    activeTriggers.insert(data);
                }
                break;
            }
        }   
    }
    else if(collider2->type == Collider::Type::TRIGGER)
    {   
        CollisionData data = {collider1, collider2};
        if(activeTriggers.find(data) != activeTriggers.end())
        {
            GetCore().messageBus.sendMessage( Message(Event::TRIGGER_EXIT, data) );
            activeTriggers.erase(data);
        }
    }
}

#endif