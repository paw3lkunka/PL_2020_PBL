#include "TutorialControllerSystem.hpp"
#include "Entity.hpp"
#include "RectTransform.inl"
#include "Tutorial.inl"
#include "UiRenderer.inl"

bool TutorialControllerSystem::assertEntity(Entity* entity)
{
    tutorialPtr = entity->getComponentPtr<Tutorial>();
    rectTransformPtr = entity->getComponentPtr<RectTransform>();
    rendererPtr = entity->getComponentPtr<UiRenderer>();
    return (tutorialPtr != nullptr) && (rectTransformPtr != nullptr) && (rendererPtr != nullptr);
}

void TutorialControllerSystem::start()
{
    tutorialPtr->sceneTime = 0.0f;
    boardIndex = 0;
    if(animationFrame[0] == nullptr)
    {
        animationFrame[0] = animationFrame[6] = animationFrame[12] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_basic.png");
        animationFrame[1] = animationFrame[7] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_up.png");
        animationFrame[2] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_up_right.png");
        animationFrame[3] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_right.png");
        animationFrame[4] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_down_right.png");
        animationFrame[5] = animationFrame[11] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_down.png");
        animationFrame[8] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_up_left.png");
        animationFrame[9] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_left.png");
        animationFrame[10] = GetCore().objectModule.getTexturePtrByFilePath("Resources/Sprites/tutorial/pad_down_left.png");
    }
}

void TutorialControllerSystem::frameUpdate()
{
    tutorialPtr->frameTime += glfwGetTime() - tutorialPtr->sceneTime;
    if(tutorialPtr->frameTime > tutorialPtr->timeOfBoard)
    {
        GetCore().objectModule.getMaterialPtrByName("tutorial_pad")->setTexture("sprite", animationFrame[boardIndex]);
        boardIndex = ++boardIndex % 13;
        tutorialPtr->frameTime -= tutorialPtr->timeOfBoard;
    }
    tutorialPtr->sceneTime = glfwGetTime();
}