#include "UiModule.hpp"

#include "Message.inl"

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
        
        glm::mat2 rotation = glm::mat2(1.0f);
        float sin = glm::sin(transform->getLocalRotation());
        float cos = glm::cos(transform->getLocalRotation());
        rotation[0][0] = cos;
        rotation[0][1] = -sin;
        rotation[1][0] = sin;
        rotation[1][1] = cos;

        glm::mat2 scale = glm::mat2(1.0f);
        scale[0][0] = transform->getLocalScale().x;
        scale[1][1] = transform->getLocalScale().y;

        glm::mat2 rotScl = rotation * scale;

        local[0][0] = rotScl[0][0];
        local[0][1] = rotScl[0][1];
        local[1][0] = rotScl[1][0];
        local[1][1] = rotScl[1][1];

        transform->modelMatrix = parent * local;
        transform->dirty = false;
    }
    
    for(auto t : transform->children)
    {
        process(t, transform->modelMatrix, dirty);
    }
}