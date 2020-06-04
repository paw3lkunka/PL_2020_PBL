#include "ECS.inc"

#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>

bool EnemiesSystem::assertEntity(Entity* entity)
{
	enemyPtr = entity->getComponentPtr<Enemy>();
	enemyTransformPtr = entity->getComponentPtr<Transform>();
	enemyAnimationPtr = entity->getComponentPtr<EnemyAnimation>();

	return enemyPtr && enemyTransformPtr;
}

void EnemiesSystem::fixedUpdate()
{
    auto* kayakPtr = Kayak::get();
    auto* kayakTransformPtr = Kayak::getTransform();
    
    glm::vec3 enemyPos = enemyTransformPtr->getModelMatrix()[3];
    glm::vec3 kayakPos = kayakTransformPtr->getModelMatrix()[3];
    
    if (kayakPtr && kayakTransformPtr)
    {
        detection(kayakPtr, enemyPos, kayakPos);

        if (enemyAnimationPtr)
        {
            animation(kayakPtr, enemyPos, kayakPos);
        }
    }
}

void EnemiesSystem::detection(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos)
{
    if (kayakPtr->isHidden)
    {
        enemyPtr->detectionCounter -= enemyPtr->detectionNegativeStep;
    }
    else
    {
        float distance = glm::distance(enemyPos, kayakPos);
        if (distance < enemyPtr->sightDistance)
        {
            enemyPtr->detectionCounter += enemyPtr->detectionPositiveStep;
            if (enemyPtr->detectionCounter >= enemyPtr->detectionCounterMaxValue && enemyPtr->notified == false)
            {
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
        enemyPtr->notified = false;
        kayakPtr->isDetected--;
    }
    enemyPtr->detectionCounter = std::clamp(enemyPtr->detectionCounter, 0, enemyPtr->detectionCounterMaxValue);
}

void EnemiesSystem::animation(Kayak* kayakPtr, glm::vec3 enemyPos, glm::vec3 kayakPos)
{
    glm::quat target = glm::quatLookAt(glm::normalize(kayakPos - enemyPos), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat& rotation = enemyTransformPtr->getLocalRotationModifiable();
    rotation = glm::slerp(rotation, target, enemyAnimationPtr->lerpParameter);
}