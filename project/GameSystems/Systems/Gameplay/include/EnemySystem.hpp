#ifndef ENEMIESSYSTEM_HPP_
#define ENEMIESSYSTEM_HPP_

#include "System.hpp"

class Enemy;
class Transform;
class EnemyAnimation;
class EnemyAttack;

/**
 * @brief Determine whole enemy behaviour. 
 */
class EnemySystem : public System
{
public:
    EnemySystem() = default;
    virtual ~EnemySystem() = default;
protected:
    virtual bool assertEntity(Entity* entity);
    virtual void fixedUpdate();
protected:
private:
    //obligatory
        Enemy* enemyPtr;
        Transform* enemyTransformPtr;
    //optional
        EnemyAnimation* enemyAnimationPtr;
        EnemyAttack* enemyAttackPtr;

    /**
     * @brief Routine of detecting kayak.
     * 
     * @param kayakPtr Pointer to kayak object - MUST NOT BE NULL!
     * @param enemyPos Position of enemy in world space.
     * @param kayakPos Position of kayak in world space.
     */
    void detection(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos);
    
    /**
     * @brief Animation of enemy - look at with lerp.
     * 
     * @param dir Direction to look at (normalized).
     */
    void animation(glm::vec3 dir);

    /**
     * @brief Attack routine.
     * 
     * @param dir Direction from enemy to player.
     */
    void attack(glm::vec3 dir);
};

#endif /* !ENEMIESSYSTEM_HPP_ */
