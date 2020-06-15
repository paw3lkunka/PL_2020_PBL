#include "ECS.inc"

bool DetectionBarSystem::init(const char* barEntityName)
{
	this->barEntityName = barEntityName;

	Entity* entity;
	if (entity = GetCore().objectModule.getEntityPtrByName(barEntityName))
	{
		progressBarPtr = entity->getComponentPtr<ProgressBar>();
	}
	else
	{
		progressBarPtr = nullptr;
	}
	
	return progressBarPtr;
}

void DetectionBarSystem::prepare()
{
	if (progressBarPtr)
	{
		progressBarPtr->percentage = 0.0f;
	}
}


bool DetectionBarSystem::assertEntity(Entity* entity)
{
	
	enemyPtr = entity->getComponentPtr<Enemy>();
	return progressBarPtr && enemyPtr;
}

void DetectionBarSystem::frameUpdate()
{
	progressBarPtr->percentage = std::max(progressBarPtr->percentage, (float)enemyPtr->detectionCounter / (float)enemyPtr->detectionCounterMaxValue);
}
