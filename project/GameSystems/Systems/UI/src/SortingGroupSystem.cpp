#include "SortingGroupSystem.hpp"

#include "Entity.hpp"
#include "Components.inc"
#include "Material.hpp"

bool SortingGroupSystem::assertEntity(Entity* entity)
{
    sortingGroupPtr = entity->getComponentPtr<UiSortingGroup>();
    rectTransformPtr = entity->getComponentPtr<RectTransform>();
    return (sortingGroupPtr != nullptr) && (rectTransformPtr != nullptr);
}

void SortingGroupSystem::start()
{
    sortingGroupPtr->lastFrameTransparency = sortingGroupPtr->groupTransparency;
    setChildrenTransparency(rectTransformPtr);
}

void SortingGroupSystem::frameUpdate()
{
    // ? If they are not equal
    if(glm::abs(sortingGroupPtr->lastFrameTransparency - sortingGroupPtr->groupTransparency) > 0.001f) 
    {
        setChildrenTransparency(rectTransformPtr);
        sortingGroupPtr->lastFrameTransparency = glm::mix(sortingGroupPtr->lastFrameTransparency, sortingGroupPtr->groupTransparency, sortingGroupPtr->disappearanceFactor);
    }
    // ? If they are equal 
    else
    {
        if(sortingGroupPtr->groupTransparency > 0.001f && sortingGroupPtr->disappearanceTime > 0.001f)
        {
            if(sortingGroupPtr->startTime < 0.01f)
            {
                sortingGroupPtr->startTime = glfwGetTime();
            }
            else if(glfwGetTime() - sortingGroupPtr->startTime > sortingGroupPtr->disappearanceTime)
            {
                sortingGroupPtr->groupTransparency = 0.0f;
            }
        }
        else
        {
            sortingGroupPtr->startTime = 0.0f;
        }
    }
    
}

void SortingGroupSystem::setChildrenTransparency(RectTransform* parent)
{
    if(auto button = parent->entityPtr->getComponentPtr<Button>())
    {
        button->baseColor.a = sortingGroupPtr->lastFrameTransparency;
        button->highlightedColor.a = sortingGroupPtr->lastFrameTransparency;
        button->onClickColor.a = sortingGroupPtr->lastFrameTransparency;
        button->inactiveColor.a = sortingGroupPtr->lastFrameTransparency;
        // * if button is hardly visible
        if(sortingGroupPtr->lastFrameTransparency < 0.01f && sortingGroupPtr->groupTransparency < 0.01f)
        { 
            button->isActive = false;
        }
        //* if the button becomes visible
        else if( sortingGroupPtr->lastFrameTransparency < 0.01f && sortingGroupPtr->groupTransparency > 0.01f)
        {
            button->isActive = true;
        }
    }
    else if(auto image = parent->entityPtr->getComponentPtr<UiRenderer>())
    {
        glm::vec4 color = *image->material->getVec4Ptr("color");
        color.a = sortingGroupPtr->lastFrameTransparency;
        image->material->setVec4("color", color);
    }
    else if(auto text = parent->entityPtr->getComponentPtr<TextRenderer>())
    {
        glm::vec4 color = *text->material->getVec4Ptr("color");
        color.a = sortingGroupPtr->lastFrameTransparency;
        text->material->setVec4("color", color);
    }


    for(auto child : parent->children)
    {
        setChildrenTransparency(child);
    }

}