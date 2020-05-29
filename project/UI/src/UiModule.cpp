#include "UiModule.hpp"

#include "Message.inl"
#include "Core.hpp"

#include <glm/gtc/matrix_transform.hpp>

void UiModule::receiveMessage(Message msg)
{

}

void UiModule::init()
{
    
}

void UiModule::updateRectTransforms()
{
    for(auto var : rootNodes)
    {
        process(var, nullptr, false);
    }
}

void UiModule::process(RectTransform* transform, RectTransform* parent, bool dirty)
{
    dirty |= transform->dirty;

    glm::mat3 local(1.0f);

    if (dirty)
    {
        glm::vec2 parentSize = glm::vec2(1.0f);
        glm::mat3 parentMatrix = glm::mat3(1.0f);
        if (parent == nullptr)
        {
            parentSize.x = GetCore().windowWidth;
            parentSize.y = GetCore().windowHeight;
        }
        else
        {
            parentMatrix = parent->modelMatrix;
            parentSize = parent->getSize();
        }
        local[2][0] = transform->getAnchor().x * parentSize.x + transform->getLocalPosition().x - transform->getOrigin().x * transform->getSize().x;
        local[2][1] = transform->getAnchor().y * parentSize.y + transform->getLocalPosition().y - transform->getOrigin().y * transform->getSize().y;

        transform->screenPosition.x = local[2][0];
        transform->screenPosition.y = local[2][1];
        
        glm::mat2 rotation = glm::mat2(1.0f);
        float sin = glm::sin(transform->getLocalRotation());
        float cos = glm::cos(transform->getLocalRotation());
        rotation[0][0] = cos;
        rotation[0][1] = -sin;
        rotation[1][0] = sin;
        rotation[1][1] = cos;

        glm::mat2 scale = glm::mat2(1.0f);
        scale[0][0] = transform->getSize().x;
        scale[1][1] = transform->getSize().y;

        glm::mat2 rotScl = rotation * scale;

        local[0][0] = rotScl[0][0];
        local[0][1] = rotScl[0][1];
        local[1][0] = rotScl[1][0];
        local[1][1] = rotScl[1][1];

        transform->modelMatrix = parentMatrix * local;
        transform->dirty = false;
    }
    
    for(auto t : transform->children)
    {
        process(t, transform, dirty);
    }
}