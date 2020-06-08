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

void SortingGroupSystem::frameUpdate()
{
    if(glm::abs(sortingGroupPtr->lastFrameTransparency - sortingGroupPtr->groupTransparency) > 0.001f)
    {
        setChildrenTransparency(rectTransformPtr);
        sortingGroupPtr->lastFrameTransparency = sortingGroupPtr->groupTransparency;
    }
}

void SortingGroupSystem::setChildrenTransparency(RectTransform* parent)
{
    if(auto button = parent->entityPtr->getComponentPtr<Button>())
    {
        button->baseColor.a = sortingGroupPtr->groupTransparency;
        button->highlightedColor.a = sortingGroupPtr->groupTransparency;
        button->onClickColor.a = sortingGroupPtr->groupTransparency;
        button->inactiveColor.a = sortingGroupPtr->groupTransparency;
        // * if button is hardly visible
        if(sortingGroupPtr->groupTransparency < 0.01f)
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
        color.a = sortingGroupPtr->groupTransparency;
        image->material->setVec4("color", color);
    }
    else if(auto text = parent->entityPtr->getComponentPtr<TextRenderer>())
    {
        glm::vec4 color = *text->material->getVec4Ptr("color");
        color.a = sortingGroupPtr->groupTransparency;
        text->material->setVec4("color", color);
    }


    for(auto child : parent->children)
    {
        setChildrenTransparency(child);
    }

}