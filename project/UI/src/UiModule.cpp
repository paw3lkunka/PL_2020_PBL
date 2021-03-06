#include "UiModule.hpp"

#include "Message.inl"
#include "Core.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void UiModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::WINDOW_RESIZED:
            std::cout << "WINDOW RESIZED, RECALCULATING UI\n";
            updateRectTransforms(true);
            break;
    }
}

void UiModule::init()
{
    updateRectTransforms(true);
}

void UiModule::updateRectTransforms(bool windowResized)
{
    for(auto var : rootNodes)
    {
        process(var, nullptr, windowResized);
    }
}

void UiModule::process(RectTransform* transform, RectTransform* parent, bool dirty)
{
    dirty |= transform->dirty;

    glm::mat3 local(1.0f);
    glm::mat3 localNoScale(1.0f);

    if (dirty)
    {
        glm::vec2 parentSize = glm::vec2(1.0f);
        glm::mat3 parentMatrix = glm::mat3(1.0f);
        glm::mat3 parentMatrixNoScale = glm::mat3(1.0f);
        if (parent == nullptr)
        {
            parentSize.x = Core::windowWidth;
            parentSize.y = Core::windowHeight;
        }
        else
        {
            parentMatrix = parent->modelMatrix;
            parentMatrixNoScale = parent->noScaleModelMatrix;
            parentSize = parent->getSize();
        }
        //TODO  origin 
        local[2][0] = transform->getAnchor().x * parentSize.x + transform->getLocalPosition().x ;//- transform->getOrigin().x * transform->getSize().x;
        local[2][1] = transform->getAnchor().y * parentSize.y + transform->getLocalPosition().y ;//- transform->getOrigin().y * transform->getSize().y;

        
        glm::mat2 rotation = glm::mat2(1.0f);
        float sin = glm::sin(transform->getLocalRotation());
        float cos = glm::cos(transform->getLocalRotation());
        rotation[0][0] = cos;
        rotation[0][1] = -sin;
        rotation[1][0] = sin;
        rotation[1][1] = cos;

        localNoScale = local;
        localNoScale[0][0] = rotation[0][0];
        localNoScale[0][1] = rotation[0][1];
        localNoScale[1][0] = rotation[1][0];
        localNoScale[1][1] = rotation[1][1];
        transform->noScaleModelMatrix = parentMatrixNoScale * localNoScale;

        glm::mat2 scale = glm::mat2(1.0f);
        scale[0][0] = transform->getSize().x;
        scale[1][1] = transform->getSize().y;

        glm::mat2 rotScl = rotation * scale;

        local[0][0] = rotScl[0][0];
        local[0][1] = rotScl[0][1];
        local[1][0] = rotScl[1][0];
        local[1][1] = rotScl[1][1];

        transform->modelMatrix = parentMatrixNoScale * local;

        transform->screenPosition.x = transform->modelMatrix[2][0];
        transform->screenPosition.y = transform->modelMatrix[2][1];
        
        transform->dirty = false;
    }
    
    for(auto t : transform->children)
    {
        process(t, transform, dirty);
    }
}

void UiModule::unloadScene()
{
    for(auto node : rootNodes)
    {
        node = nullptr;
    }
    rootNodes.clear();
}