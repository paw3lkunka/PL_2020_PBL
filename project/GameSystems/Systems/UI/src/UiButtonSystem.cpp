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

void UiButtonSystem::fixedUpdate()
{
    if(buttonPtr->isActive) //check if button is active
    {
        glm::ivec2 buttonPos = rectTransformPtr->getScreenPosition();
        glm::ivec2 buttonSize = rectTransformPtr->getSize();
        // * check if mouse pointer is in button
        if( lastCursorData.xPos > (buttonPos.x - (buttonSize.x / 2.0f))
            && lastCursorData.xPos < (buttonPos.x + (buttonSize.x / 2.0f))
            && Core::windowHeight - lastCursorData.yPos > (buttonPos.y - (buttonSize.y / 2.0f))
            && Core::windowHeight - lastCursorData.yPos < (buttonPos.y + (buttonSize.y / 2.0f)))
        {
            if(mouseButtonClicked && !buttonPtr->isClicked) //* if mouse clicked over button, send event and change color for on click
            {
                buttonPtr->lastFrameColor = glm::mix(buttonPtr->lastFrameColor, buttonPtr->onClickColor, lerpFactor);
                for(auto e : buttonPtr->onClickEvents)
                {
                    GetCore().messageBus.sendMessage(Message(e));
                }
                buttonPtr->isClicked = true;
            }
            else //* if is over button, change color to highlited
            {
                buttonPtr->isClicked = false;
                buttonPtr->lastFrameColor = glm::mix(buttonPtr->lastFrameColor, buttonPtr->highlightedColor, lerpFactor);
            }
        }
        else //* if mouse is not over button, change to base color
        {
            buttonPtr->lastFrameColor = glm::mix(buttonPtr->lastFrameColor, buttonPtr->baseColor, lerpFactor);
        }
    }
    else
    {
        buttonPtr->lastFrameColor = buttonPtr->inactiveColor;
    }
    rendererPtr->material->setVec4("color", buttonPtr->lastFrameColor);
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