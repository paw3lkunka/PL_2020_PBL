#include "Components.inc"
#include "Systems.inc"

#include <glm/glm.hpp>

#define CLAMP_PROC enemyPtr->detectionCounter = std::clamp(enemyPtr->detectionCounter, 0, enemyPtr->detectionCounterMaxValue);

bool EnemiesSightSystem::assertEntity(Entity* entity)
{   
    enemyPtr = entity->getComponentPtr<Enemy>();
    enemyTransformPtr = entity->getComponentPtr<Transform>();
    return enemyPtr && enemyTransformPtr;
}

void EnemiesSightSystem::fixedUpdate()
{
    if (auto* kayakPtr = Kayak::get() )
    {
        if (kayakPtr->isHidden)
        {
            enemyPtr->detectionCounter -= enemyPtr->detectionNegativeStep;
            CLAMP_PROC;
        }
        else
        {
            if (kayakTransformPtr == nullptr)
            {
                kayakTransformPtr = kayakPtr->entityPtr->getComponentPtr<Transform>();
            }

            glm::vec3 enemyPos = enemyTransformPtr->getModelMatrix()[3];
            glm::vec3 kayakPos = kayakTransformPtr->getModelMatrix()[3];
            float distance = glm::distance(enemyPos, kayakPos);

            if (distance < enemyPtr->sightDistance)
            {
                enemyPtr->detectionCounter += enemyPtr->detectionPositiveStep;
                if (enemyPtr->detectionCounter >= enemyPtr->detectionCounterMaxValue)
                {
                    kayakPtr->isDetected = true;
                }
                CLAMP_PROC;
            }
        }
    }
}

#undef CLAMP