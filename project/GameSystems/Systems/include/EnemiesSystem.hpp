#ifndef ENEMIESSYSTEM_HPP_
#define ENEMIESSYSTEM_HPP_

#include "System.hpp"

class Enemy;
class Transform;

/**
 * @brief Determine whole enemy behaviour. 
 */
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

    /**
     * @brief Procedure of detecting kayak
     * 
     * @param kayakPtr Pointer to kayak object - MUST NOT BE NULL!
     * @param enemyPos Position of enemy in world space.
     * @param kayakPos Position of kayak in world space.
     */
    void detection(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos);
    
    /**
     * @brief 
     * 
     * @param kayakPtr Pointer to kayak object - MUST NOT BE NULL!
     * @param enemyPos Position of enemy in world space.
     * @param kayakPos Position of kayak in world space.
     */
    void animation(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos);
};

#endif /* !ENEMIESSYSTEM_HPP_ */
