#ifndef ENEMIESSIGTHSYSTEM_HPP_
#define ENEMIESSIGTHSYSTEM_HPP_

#include "System.hpp"

class Enemy;
class Transform;

class EnemiesSystem : public System
{
    public:
        EnemiesSystem() = default;
        virtual ~EnemiesSystem() = default;

    protected:
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    protected:
    private:
        Enemy* enemyPtr;
        Transform* enemyTransformPtr;
        EnemyAnimation* enemyAnimationPtr;

        void detection(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos);
        void animation(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos);
};

#endif /* !ENEMIESSIGTHSYSTEM_HPP_ */
