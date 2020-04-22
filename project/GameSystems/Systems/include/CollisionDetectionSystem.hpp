#ifndef COLLISIONDETECTION_HPP_
#define COLLISIONDETECTION_HPP_

#include "System.hpp"

#include <vector>

enum class ComponentType;
class Collider;

class CollisionDetectionSystem : public System
{
    public:
        CollisionDetectionSystem();
        virtual ~CollisionDetectionSystem() = default;

        virtual void start();     
        virtual bool assertEntity(Entity* entity) = 0;
        virtual void fixedUpdate();
        
        Collider* colliderPtr;
        std::vector<Collider*> colliders;
    protected:
    private:
};

#endif /* !COLLISIONDETECTION_HPP_ */
