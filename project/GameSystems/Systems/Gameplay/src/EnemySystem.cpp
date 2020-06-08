#include "ECS.inc"
#include "Core.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>

bool EnemySystem::assertEntity(Entity* entity)
{
    //obligatory
        enemyPtr = entity->getComponentPtr<Enemy>();
        enemyTransformPtr = entity->getComponentPtr<Transform>();
    //optional
        enemyAnimationPtr = entity->getComponentPtr<EnemyAnimation>();
        enemyAttackPtr = entity->getComponentPtr<EnemyAttack>();

	return enemyPtr && enemyTransformPtr;
}

void EnemySystem::fixedUpdate()
{
    auto* kayakPtr = Kayak::get();
    auto* kayakTransformPtr = Kayak::getTransform();
    
    glm::vec3 enemyPos = enemyTransformPtr->getModelMatrix()[3];
    glm::vec3 kayakPos = kayakTransformPtr->getModelMatrix()[3];
    
    if (kayakPtr && kayakTransformPtr)
    {
        detection(kayakPtr, enemyPos, kayakPos);
    }
}

void EnemySystem::detection(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos)
{
    if (kayakPtr->isHidden)
    {
        enemyPtr->detectionCounter -= enemyPtr->detectionNegativeStep;
    }
    else
    {
        float distance = glm::distance(enemyPos, kayakPos);
        glm::vec3 forward = glm::normalize(glm::xyz(enemyTransformPtr->getModelMatrix()[0]));
        glm::vec3 toKayak = glm::normalize(kayakPos - enemyPos);

        if ( distance < enemyPtr->sightDistance && glm::dot(forward, toKayak) < enemyPtr->sightAngle)
        {
            enemyPtr->detectionCounter += enemyPtr->detectionPositiveStep;

            animation(toKayak);

            if (enemyPtr->detectionCounter >= enemyPtr->detectionCounterMaxValue && enemyPtr->notified == false)
            {
                GetCore().messageBus.sendMessage(Message(Event::PLAYER_DETECTED, enemyPtr));
                enemyPtr->notified = true;
                kayakPtr->isDetected++;                
            }
        }
        else
        {
            enemyPtr->detectionCounter -= enemyPtr->detectionNegativeStep;
        }
    }
    
    if (enemyPtr->detectionCounter <= 0 && enemyPtr->notified == true)
    {
        GetCore().messageBus.sendMessage(Message(Event::PLAYER_ESCAPED, enemyPtr));
        enemyPtr->notified = false;
        kayakPtr->isDetected--;
    }
    enemyPtr->detectionCounter = std::clamp(enemyPtr->detectionCounter, 0, enemyPtr->detectionCounterMaxValue);
}

void EnemySystem::animation(glm::vec3 dir)
{
    if (enemyAnimationPtr)
    {
        glm::quat target = glm::quatLookAt(dir, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat& rotation = enemyTransformPtr->getLocalRotationModifiable();
        rotation = glm::slerp(rotation, target, enemyAnimationPtr->lerpParameter);
    }
}

void EnemySystem::attack(glm::vec3 dir)
{
    if (enemyAttackPtr)
    {
        enemyAttackPtr->attackCounter += enemyAttackPtr->incrementValue;

        if (enemyAttackPtr->attackCounter >= enemyAttackPtr->activationValue)
        {
            enemyAttackPtr->attackCounter -= enemyAttackPtr->activationValue;
        }
    }
}