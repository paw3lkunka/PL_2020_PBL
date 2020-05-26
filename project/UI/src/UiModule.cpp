#include "UiModule.hpp"

#include "Message.inl"

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
        process(var, glm::mat3(1.0f), false);
    }
}

void UiModule::process(RectTransform* transform, glm::mat3 parent, bool dirty)
{
    dirty |= transform->dirty;

    glm::mat3 local(1.0f);

    if (dirty)
    {
        local[2][0] = transform->getLocalPosition().x;
        local[2][1] = transform->getLocalPosition().y;
        float sin = glm::sin(transform->getLocalRotation());
        float cos = glm::cos(transform->getLocalRotation());
        local[0][0] = cos;
        local[0][1] = -sin;
        local[1][0] = sin;
        local[1][1] = cos;
        local[0][0] *= transform->getLocalScale().x;
        local[1][1] *= transform->getLocalScale().y;

        transform->modelMatrix = parent * local;
        transform->dirty = false;
    }
    
    for(auto t : transform->children)
    {
        process(t, transform->modelMatrix, dirty);
    }
}