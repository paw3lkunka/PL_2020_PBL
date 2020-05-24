#ifndef COLLISION_DETECTION_SYSTEM_IPP
#define COLLISION_DETECTION_SYSTEM_IPP

#include "CollisionSystemHelperFunctions.hpp"
#include "CollisionDataStructures.inl"
#include "Message.inl"
#include "Core.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vec_swizzle.hpp> 

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
        std::cout << "Collision detected between " << Name(collider1) << " and " << Name(collider2) << " - type: " << (int)collider2->type << std::endl;
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
    glm::vec3 momentum1 = body1->mass * body1->velocity;
    glm::vec3 momentum2 = body2->mass * body2->velocity;

    glm::vec3 reaction = ( momentum1 + momentum2 + body2->mass * glm::e<float>() * ( body2->velocity - body1->velocity) ) / ( body1->mass + body2->mass );
    reaction = glm::sign(testResult.collisionNormal) * glm::abs(reaction);
    glm::vec3 penetration = testResult.collisionNormal * testResult.penetration;

    body1->velocity =  reaction;// glm::max(reaction, penetration);
    body1->angularVelocity = glm::cross( -testResult.collisionNormal, glm::cross( -testResult.collisionNormal, body1->angularVelocity) + body2->velocity );
}

template<class T>
void CollisionSystem::resolveCollsion(T* collider1, BoxCollider* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2)
{
    glm::vec3 r1 = testResult.collisionCentre - glm::xyz( transform1->getModelMatrix() * glm::vec4(collider1->center, 1.0f) );
    glm::vec3 r2 = testResult.collisionCentre - glm::xyz( transform2->getModelMatrix() * glm::vec4(collider2->center, 1.0f) );

    glm::vec3 jImpulse = JImpulse(body1, body2, r1, r2, testResult.collisionNormal);

    glm::vec3 reaction = body1->velocity + jImpulse / body1->mass;
    //glm::vec3 penetration = testResult.collisionNormal * testResult.penetration;

    body1->velocity = reaction;
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
            //FIXME: this is wrong
            testResult.penetration = proj1.end - proj2.start;
        }
        // 2.start, 1.start, 2.end, 1.end
        else if (proj2.start < proj1.start && proj2.end > proj1.start)
        {
            centres.push_back(proj2Buffer[END(i)] - proj1Buffer[START(i)]);
            testResult.penetration = proj2.end - proj1.start;
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
    //testResult.penetration

    return true;
    #undef START
    #undef END
}

template<class T>
glm::vec3 CollisionSystem::collisionNormal(T* collider1, SphereCollider* collider2, Transform* transform1, Transform* transform2)
{
    glm::vec3 vector = transform1->getModelMatrix() * glm::vec4(collider1->center, 1.0f)
            - transform2->getModelMatrix() * glm::vec4(collider2->center, 1.0f); 
    return glm::normalize(vector);
}

template<class T>
glm::vec3 CollisionSystem::collisionNormal(T* collider1, BoxCollider* collider2, Transform* transform1, Transform* transform2)
{
    glm::vec3 vector = glm::normalize
    (
        transform1->getModelMatrix() * glm::vec4(collider1->center, 1.0f)
        - transform2->getModelMatrix() * glm::vec4(collider2->center, 1.0f)
    );

    glm::mat4 rotMatrix;
    {
        glm::quat rotation;
        union
        {
            glm::vec3 v3;
            glm::vec4 v4;
        } tmp;
        //TODO OPTIMIZE
        glm::decompose(transform1->getModelMatrix(), tmp.v3, rotation, tmp.v3, tmp.v3, tmp.v4);
        rotMatrix = glm::toMat4(rotation);
    }

    //TODO: counting in model space seems to be more optimal way

    glm::vec3 possibles[6] // Normalized
    {
        rotMatrix * glm::vec4(  1.0f,  0.0f,  0.0f,  0.0f ),
        rotMatrix * glm::vec4( -1.0f,  0.0f,  0.0f,  0.0f ),
        rotMatrix * glm::vec4(  0.0f,  1.0f,  0.0f,  0.0f ),
        rotMatrix * glm::vec4(  0.0f, -1.0f,  0.0f,  0.0f ),
        rotMatrix * glm::vec4(  0.0f,  0.0f,  1.0f,  0.0f ),
        rotMatrix * glm::vec4(  0.0f,  0.0f, -1.0f,  0.0f ),
    };

    float cos = -INFINITY;
    int index = -1;

    for (int i = 0; i < 6; i++)
    {
        float dot = glm::dot(vector, possibles[i]);
        if (dot > cos)
        {
            cos = dot;
            index = i;
        }
    }
    
    return possibles[index];
}

#endif