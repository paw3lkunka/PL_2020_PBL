#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionSystemHelperFunctions.hpp"
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


template<class T>
void CollisionSystem::resolveCollsion(T* collider1, SphereCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    glm::vec3& normal = testResult.collisionNormal;
    glm::vec3 momentum1 = body1->mass * body1->velocity;
    glm::vec3 momentum2 = body2->mass * body2->velocity;

    body1->velocity = ( momentum1 + momentum2 + body2->mass * glm::e<float>() * ( body2->velocity - body1->velocity) ) / ( body1->mass + body2->mass );

    body1->angularVelocity = glm::cross(normal, glm::cross(normal, body1->angularVelocity) + glm::vec3(0.0f, body2->velocity.y, body2->velocity.z) );
}

template<class T>
void CollisionSystem::resolveCollsion(T* collider1, BoxCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    glm::vec3 r1 = testResult.collisionCentre - static_cast<glm::vec3>( transform1->localToWorldMatrix * glm::vec4(transform1->getLocalPosition() + collider1->center, 1.0f) );
    glm::vec3 r2 = testResult.collisionCentre - static_cast<glm::vec3>( transform2->localToWorldMatrix * glm::vec4(transform2->getLocalPosition() + collider2->center, 1.0f) );

    glm::vec3 jImpulse = JImpulse(body1, body2, r1, r2, testResult.collisionNormal);

    body1->velocity = body1->velocity + jImpulse * testResult.collisionNormal / body1->mass;
    //TODO I^-1 chould be pre computed
    body1->angularVelocity = body1->velocity + glm::inverse(body1->momentOfInertia) * ( glm::cross(r1, jImpulse) * testResult.collisionNormal);
}

template<class T1, class T2>
bool CollisionSystem::SATTest(T1* collider1, T2* collider2, Transform* transform1, Transform* transform2, std::vector<glm::vec3>& axes)
{
    ///@brief parse number of line projection in vec3 array to valid index of it's start point.
    #define START(i) (2*i)
    ///@brief parse number of line projection in vec3 array to valid index of it's end point.
    #define END(i) (2*i+1)

    int axesSize = axes.size();
    int tests = axesSize / 2;
    
    glm::vec3 proj1Buffer[axesSize];
    glm::vec3 proj2Buffer[axesSize];
    std::vector<glm::vec3> centres;

    for (int i = 0; i < tests; i++)
    {        
        Projection1D proj1 = AxisProjection(collider1, transform1, axes[START(i)], axes[END(i)], proj1Buffer + START(i));
        Projection1D proj2 = AxisProjection(collider2, transform2, axes[START(i)], axes[END(i)], proj2Buffer + START(i));

        // 1.start, 2.start, 1.end, 2.end
        if (proj1.start < proj2.start && proj1.end > proj2.start)
        {
            centres.push_back(proj1Buffer[END(i)] - proj2Buffer[START(i)]);
        }
        // 2.start, 1.start, 2.end, 1.end
        else if (proj2.start < proj1.start && proj2.end > proj1.start)
        {
            centres.push_back(proj2Buffer[END(i)] - proj1Buffer[START(i)]);
        }
        else
        {
            return testResult.collisionDetected = false;
        }
    }

    glm::vec3 sum(0.0f, 0.0f, 0.0f);
    for (glm::vec3& v : centres)
    {
        sum += v;
    }

    testResult.collisionDetected = true;
    testResult.collisionCentre = sum /= centres.size();
    testResult.collisionNormal = collisionNormal( collider1, collider2, transform1, transform2);

    return true;
    #undef START
    #undef END
}

template<class T>
glm::vec3 CollisionSystem::collisionNormal(T* collider1, SphereCollider* collider2, Transform* transform1, Transform* transform2)
{
    glm::vec3 vector = transform2->localToWorldMatrix * glm::vec4(transform2->getLocalPosition() + collider2->center, 1.0f)
            - transform1->localToWorldMatrix * glm::vec4(transform1->getLocalPosition() + collider1->center, 1.0f); 
    return glm::normalize(vector);
}

template<class T>
glm::vec3 CollisionSystem::collisionNormal(T* collider1, BoxCollider* collider2, Transform* transform1, Transform* transform2)
{
    glm::vec3 centralVector = transform2->localToWorldMatrix * glm::vec4(transform2->getLocalPosition() + collider2->center, 1.0f)
        - transform1->localToWorldMatrix * glm::vec4(transform1->getLocalPosition() + collider1->center, 1.0f); 

    glm::vec3 potentialNormals[]
    {
        collider2->verts[1] - collider2->verts[0], // locall x
        collider2->verts[0] - collider2->verts[1], // locall -x
        collider2->verts[4] - collider2->verts[0], // locall y
        collider2->verts[0] - collider2->verts[4], // locall -y
        collider2->verts[3] - collider2->verts[0], // locall z
        collider2->verts[0] - collider2->verts[3]  // locall -z
    };

    float angle = INFINITY;
    int normalIndex = -1;

    for (int i = 0; i < 6; i++)
    {
        float dot = glm::dot(centralVector, potentialNormals[i]);
        if (dot < angle)
        {
            normalIndex = i;
            angle = dot;
        }
    }

    return glm::normalize(potentialNormals[normalIndex]);
}

#endif