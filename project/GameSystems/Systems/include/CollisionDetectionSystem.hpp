#ifndef COLLISIONDETECTION_HPP_
#define COLLISIONDETECTION_HPP_

#include "System.hpp"

#include <glm/glm.hpp>
#include <vector>

enum class ComponentType;
struct Projection1D;
class Transform;
class Collider;
class BoxCollider;
class SphereCollider;

class CollisionDetectionSystem : public System
{
    public:
        CollisionDetectionSystem() = default;
        virtual ~CollisionDetectionSystem() = default;

        virtual void start();        
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();
        
        Collider* colliderPtr;
        Transform* transformPtr;
        glm::vec3 separation;
    protected:
    private:
        std::vector<Collider*> colliders;
        std::vector<Transform*> transforms;

        template<class T>
        void collisionOf(T collider);

        template<class T1, class T2>
        void collisionWith(T1 collider1, T2 collider2, Transform* transform2);

        template<class T1, class T2>
        bool collsion(T1* of, T2* with, Transform* ofT, Transform* withT);
        
        Projection1D axisProjection(BoxCollider* box, glm::vec3 axis, glm::mat4& localToWorld);
        Projection1D axisProjection(SphereCollider* sphere, glm::vec3 axis, glm::mat4& localToWorld);
};

#include "CollisionDetectionSystem.ipp"

#endif /* !COLLISIONDETECTION_HPP_ */
