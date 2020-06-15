#include "ToggleButtonSystem.hpp"

#include "ToggleButton.inl"
#include "RectTransform.inl"
#include "UiRenderer.inl"
#include "Entity.hpp"
#include "Core.hpp"

bool ToggleButtonSystem::assertEntity(Entity* entity)
{
    toggleButtonPtr = entity->getComponentPtr<ToggleButton>();
    rectTransformPtr = entity->getComponentPtr<RectTransform>();
    rendererPtr = entity->getComponentPtr<UiRenderer>();
    return (toggleButtonPtr != nullptr) && (rectTransformPtr != nullptr) && (rendererPtr != nullptr);
}

void ToggleButtonSystem::receiveMessage(Message msg)
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

void ToggleButtonSystem::frameUpdate()
{
    static glm::vec4 targetColor = toggleButtonPtr->lastFrameColor; 
    if(toggleButtonPtr->isActive) //check if button is active
    {
        glm::ivec2 buttonPos = rectTransformPtr->getScreenPosition();
        glm::ivec2 buttonSize = rectTransformPtr->getSize();
        // * check if mouse pointer is in button
        if( lastCursorData.xPos > (buttonPos.x - (buttonSize.x / 2.0f))
            && lastCursorData.xPos < (buttonPos.x + (buttonSize.x / 2.0f))
            && Core::windowHeight - lastCursorData.yPos > (buttonPos.y - (buttonSize.y / 2.0f))
            && Core::windowHeight - lastCursorData.yPos < (buttonPos.y + (buttonSize.y / 2.0f)))
        {
            //* if mouse clicked over button, change toggle flag and send appriopriate events
            if(mouseButtonClicked) 
            {
                targetColor = toggleButtonPtr->state ? toggleButtonPtr->onClickColorOn : toggleButtonPtr->onClickColorOff;
                if(!toggleButtonPtr->isClicked)
                {
                    // * Send on activation messages if button switches to ON
                    if(toggleButtonPtr->state) 
                    {
                        for(auto m : toggleButtonPtr->onDeactivateEvents)
                        {
                            GetCore().messageBus.sendMessage(m);
                        }
                    }
                    // * Send on deactivation messages if button switches to OFF
                    else
                    {
                        for(auto m : toggleButtonPtr->onActivateEvents)
                        {
                            GetCore().messageBus.sendMessage(m);
                        }
                    }
                    toggleButtonPtr->isClicked = true;
                    toggleButtonPtr->state = !toggleButtonPtr->state;
                }
            }
            //* if is over button, change color to highlighted
            else
            {
                toggleButtonPtr->isClicked = false;
                targetColor = toggleButtonPtr->state ? toggleButtonPtr->highlightedColorOn : toggleButtonPtr->highlightedColorOff;
            }
        }
        else //* if mouse is not over button, change to base color
        {
            targetColor = toggleButtonPtr->state ? toggleButtonPtr->baseColorOn : toggleButtonPtr->baseColorOff;
        }
    }
    else // * if button is inactive
    {
        targetColor = toggleButtonPtr->inactiveColor;
    }
    toggleButtonPtr->lastFrameColor = glm::mix(toggleButtonPtr->lastFrameColor, targetColor, lerpFactor);
    rendererPtr->material->setVec4("color", toggleButtonPtr->lastFrameColor);
}