#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionDataStructures.inl"
#include "Message.inl"
#include "Core.hpp"

#include <glm/gtx/string_cast.hpp>

template<class T1, class T2>    // for T1 == T2 == Sphere collider specialization was defined.
bool CollisionSystem::detectCollsion(T1* coll1, T2* coll2, Transform* trans1, Transform* trans2)
{
    std::vector<glm::vec3> axes;

    findSATAxes(coll1, trans1, axes);
    findSATAxes(coll2, trans2, axes);

    return SATTest(coll1, coll2, trans1, trans2, axes);
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
    if(detectCollsion(collider1, collider2, transformPtr, transform2))
    {
        //TODO TEMP DEBUG
        std::cout << "Collision detected - type: " << (int)collider2->type << std::endl;
        CollisionData data = {collider1, collider2};

        switch (collider2->type)
        {
            case Collider::Type::DYNAMIC:
            case Collider::Type::KINEMATIC:
            {
                resolveCollsion(collider1, collider2, rigidbodyPtr, rigidbody2, transformPtr, transform2);

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

template<class T1, class T2>
bool CollisionSystem::SATTest(T1* collider1, T2* collider2, Transform* transform1, Transform* transform2, std::vector<glm::vec3>& axes)
{
    int tests = axes.size() / 2;
    for (int i = 0; i < tests; i += 2)
    {        
        Projection1D proj1 = axisProjection(collider1, transform1, axes[i], axes[i+1]);
        Projection1D proj2 = axisProjection(collider2, transform2, axes[i], axes[i+1]);

        if( !(proj1.start < proj2.start && proj1.end > proj2.start) && !(proj2.start < proj1.start && proj2.end > proj1.start) )
        {
            return false;
        }
    }
    return true;    
}

#endif