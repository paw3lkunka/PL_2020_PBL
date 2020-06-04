#ifndef ENEMIESSIGTHSYSTEM_HPP_
#define ENEMIESSIGTHSYSTEM_HPP_

#include "System.hpp"

class Enemy;
class Transform;

class EnemiesSightSystem : public System
{
    public:
        EnemiesSightSystem() = default;
        virtual ~EnemiesSightSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    protected:
    private:
        Enemy* enemyPtr;
        Transform* enemyTransformPtr;
        Transform* kayakTransformPtr;
};

#endif /* !ENEMIESSIGTHSYSTEM_HPP_ */
