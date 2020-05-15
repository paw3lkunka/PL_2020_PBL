#ifndef COLLISIONDETECTION_HPP_
#define COLLISIONDETECTION_HPP_

#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>

#include "CollisionDataStructures.inl"
#include "System.hpp"

enum class ComponentType;
struct Projection1D;
class Transform;
class Collider;
class BoxCollider;
class SphereCollider;
class Rigidbody;

//TODO DOCUMENTATION !!!
class CollisionSystem : public System
{
    public:
        CollisionSystem() = default;
        virtual ~CollisionSystem() = default;

//TODO DOCUMENTATION !!!
        virtual void start();        
//TODO DOCUMENTATION !!!
        virtual bool assertEntity(Entity* entity);
//TODO DOCUMENTATION !!!
        virtual void fixedUpdate();
        
//TODO DOCUMENTATION !!!
        Collider* colliderPtr;
//TODO DOCUMENTATION !!!
        Transform* transformPtr;
//TODO DOCUMENTATION !!!
        Rigidbody* rigidbodyPtr;
//TODO DOCUMENTATION !!!
        glm::vec3 separation;
    protected:
    private:
//TODO DOCUMENTATION !!!
        std::vector<Collider*> colliders;
//TODO DOCUMENTATION !!!
        std::vector<Transform*> transforms;
//TODO DOCUMENTATION !!!
        std::vector<Rigidbody*> rigidbodies;
//TODO DOCUMENTATION !!!
        std::unordered_set<CollisionData,CollisionDataHasher,CollisionDataEquals> activeTriggers;

//TODO DOCUMENTATION !!!
        template<class T>
        void collisionOf(T* collider);

//TODO DOCUMENTATION !!!
        template<class T1, class T2>
        void collisionWith(T1* collider1, T2* collider2, Transform* transform2, Rigidbody* rigidbody2);

//TODO DOCUMENTATION !!!
        template<class T1, class T2>
        bool collsion(T1* of, T2* with, Transform* ofT, Transform* withT);

//TODO DOCUMENTATION !!!
        template<class T1, class T2>
        void resolveCollsion(Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2);

//TODO should it be here?

//TODO DOCUMENTATION !!!
        static Projection1D axisProjection(BoxCollider* box, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2);
//TODO DOCUMENTATION !!!
        static Projection1D axisProjection(SphereCollider* sphere, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2);
//TODO DOCUMENTATION !!!
        static glm::vec3 axisProjection(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2);
};

#include "CollisionSystem.ipp"

#endif /* !COLLISIONDETECTION_HPP_ */
