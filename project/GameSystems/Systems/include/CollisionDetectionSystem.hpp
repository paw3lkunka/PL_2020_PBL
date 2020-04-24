#ifndef COLLISIONDETECTION_HPP_
#define COLLISIONDETECTION_HPP_

#include "System.hpp"

#include <glm/glm.hpp>
#include <vector>

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

        glm::vec3 collsionBB(BoxCollider* of, BoxCollider* with, Transform* ofT, Transform* withT);
        glm::vec3 collsionBS(BoxCollider* of, SphereCollider* with, Transform* ofT, Transform* withT);

        glm::vec3 collsionSS(SphereCollider* of, SphereCollider* with, Transform* ofT, Transform* withT);
        glm::vec3 collsionSB(SphereCollider* of, BoxCollider* with, Transform* ofT, Transform* withT);
};

#endif /* !COLLISIONDETECTION_HPP_ */
