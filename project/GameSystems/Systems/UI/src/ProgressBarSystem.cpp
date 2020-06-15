#include "ProgressBarSystem.hpp"

#include "Entity.hpp"
#include "ProgressBar.inl"
#include "UiRenderer.inl"

bool ProgressBarSystem::assertEntity(Entity* entity)
{
    progressPtr = entity->getComponentPtr<ProgressBar>();
    rendererPtr = entity->getComponentPtr<UiRenderer>();
    return (progressPtr != nullptr) && (rendererPtr != nullptr);
}

void ProgressBarSystem::start()
{
    rendererPtr->material->setFloat("fadingPercentage", progressPtr->percentage);
}

void ProgressBarSystem::frameUpdate()
{
    if(fabs(progressPtr->lastFramePercentage - progressPtr->percentage) > 0.001f)
    {
        rendererPtr->material->setFloat("fadingPercentage", progressPtr->percentage);
        progressPtr->lastFramePercentage = progressPtr->percentage;
    }
}