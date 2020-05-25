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

    std::cout << "Centre 1: " << glm::to_string(glm::xyz( transform1->getModelMatrix() * glm::vec4(collider1->center, 1.0f) ) ) << '\n'
              << "Centre 2: " << glm::to_string(glm::xyz( transform2->getModelMatrix() * glm::vec4(collider2->center, 1.0f) ) ) << '\n' 
              << "Centre c: " << glm::to_string(testResult.collisionCentre) << '\n' 
              << "vec1c r1: " << glm::to_string(r1) << '\n' 
              << "vec2c r2: " << glm::to_string(r2) << std::endl; 

    glm::vec3 jImpulse = JImpulse(body1, body2, r1, r2, testResult.collisionNormal);

    body1->velocity = body1->velocity + jImpulse / body1->mass;
    body1->angularVelocity = body1->velocity + body1->invertedMomentOfInertia * ( glm::cross(r1, jImpulse) * testResult.collisionNormal);
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
    std::vector<float> separations;

    for (int i = 0; i < tests; i++)
    {        
        Projection1D proj1 = AxisProjection(collider1, transform1, axes[START(i)], axes[END(i)], proj1Buffer + START(i));
        Projection1D proj2 = AxisProjection(collider2, transform2, axes[START(i)], axes[END(i)], proj2Buffer + START(i));

        // 1.start, 2.start, 1.end, 2.end
        if (proj1.start < proj2.start && proj1.end > proj2.start)
        {
            centres.push_back( (proj1Buffer[END(i)] + proj2Buffer[START(i)]) * 0.5f );
            std::cout << "sc" << i << ": "<<glm::to_string(centres.back())<<std::endl;
        }
        // 2.start, 1.start, 2.end, 1.end
        else if (proj2.start < proj1.start && proj2.end > proj1.start)
        {
            centres.push_back( (proj2Buffer[END(i)] + proj1Buffer[START(i)]) * 0.5f );
            std::cout << "sc" << i << ": "<<glm::to_string(centres.back())<<std::endl;
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
    
    glm::vec3 penetrationBuffer[4];
    Projection1D proj1 = AxisProjection(collider1, transform1, transform2->getModelMatrix()[3], glm::xyz(transform2->getModelMatrix()[3]) + testResult.collisionNormal, penetrationBuffer);
    Projection1D proj2 = AxisProjection(collider2, transform2, transform2->getModelMatrix()[3], glm::xyz(transform2->getModelMatrix()[3]) + testResult.collisionNormal, penetrationBuffer + 2);
    // testResult.penetration = std::abs(std::max(proj1.start, proj2.start) - std::min(proj1.end, proj2.end));
    // std::cout << "POINTS: " << glm::to_string(penetrationBuffer[0]) << "\n"
    //           << "        " << glm::to_string(penetrationBuffer[1]) << "\n"
    //           << "        " << glm::to_string(penetrationBuffer[2]) << "\n"
    //           << "        " << glm::to_string(penetrationBuffer[3]) << "\n"
    //           << "PROJECT: " << (proj1.start) << "\n"
    //           << "         " << (proj1.end) << "\n"
    //           << "         " << (proj2.start) << "\n"
    //           << "         " << (proj2.end) << "\n" << std::endl;

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
    glm::vec3 vector = transform2->getToModelMatrix() * transform1->getModelMatrix() * glm::vec4(collider1->center, 1.0f) - glm::vec4(collider2->center, 1.0f);

    float maximal = -INFINITY;
    int index = -1;

    for (int i = 0; i < 3; i++)
    {
        float abs = std::abs(vector[i]);
        if (abs > maximal)
        {
            maximal = abs;
            index = i;
        }
    }

    glm::vec3 normal(0.0f, 0.0f, 0.0f);
    normal[index] = std::copysign(1.0f, vector[index]);
    std::cout << "model space central " << glm::to_string(vector) << std::endl;
    std::cout << "model space normal " << glm::to_string(normal) << std::endl;
    normal = transform2->getModelMatrix() * glm::vec4(normal, 0.0f);
    std::cout << "world space normal " << glm::to_string(glm::normalize(normal)) << std::endl;
    
    return glm::normalize(normal);
}

#endif