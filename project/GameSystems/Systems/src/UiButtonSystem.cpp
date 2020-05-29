#include "UiButtonSystem.hpp"
#include "Components.inc"
#include "Entity.hpp"
#include "Message.inl"
#include "Core.hpp"

bool UiButtonSystem::assertEntity(Entity* entity)
{
    buttonPtr = entity->getComponentPtr<Button>();
    rendererPtr = entity->getComponentPtr<UiRenderer>();
    rectTransformPtr = entity->getComponentPtr<RectTransform>();
    return (buttonPtr != nullptr) && (rendererPtr != nullptr) && (rectTransformPtr != nullptr);
}

void UiButtonSystem::start()
{
    rendererPtr->material->setVec4("color", buttonPtr->baseColor);
}

void UiButtonSystem::frameUpdate()
{
    rendererPtr->material->setVec4("color", buttonPtr->baseColor);
    glm::ivec2 buttonPos = rectTransformPtr->getScreenPosition();
    glm::ivec2 buttonSize = rectTransformPtr->getSize();
    if( mouseButtonClicked   
        && lastCursorData.xPos > (buttonPos.x - (buttonSize.x / 2.0f))
        && lastCursorData.xPos < (buttonPos.x + (buttonSize.x / 2.0f))
        && Core::windowHeight - lastCursorData.yPos > (buttonPos.y - (buttonSize.y / 2.0f))
        && Core::windowHeight - lastCursorData.yPos < (buttonPos.y + (buttonSize.y / 2.0f)))
    {
        rendererPtr->material->setVec4("color", buttonPtr->highlightedColor);
    }
}

void UiButtonSystem::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::MOUSE_CURSOR_MOVED:
            lastCursorData = CursorData(msg.getValue<CursorData>());
        break;
        
        case Event::MOUSE_BUTTON_PRESSED:
            if(msg.getValue<int>() == 0)
            {
                mouseButtonClicked = true;
            }
        break;

        case Event::MOUSE_BUTTON_RELEASED:
            if(msg.getValue<int>() == 0)
            {
                mouseButtonClicked = false;
            }
        break;
    }
}