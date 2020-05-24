#include "SceneModule.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Message.inl"

void SceneModule::receiveMessage(Message msg) {}

SceneModule::SceneModule()
{
    rootNode.setParent(&preRootNode);
}

void SceneModule::updateTransforms()
{
    process(rootNode, false);
}

void SceneModule::process(Transform& transform, bool dirty)
{
    dirty |= transform.dirty;
        
    glm::mat4 local(1); 

    if(dirty)
    {
        local = glm::translate(local, transform.localPosition);
        local = local * glm::toMat4(transform.localRotation);
        local = glm::scale(local, transform.localScale);
        
        transform.worldRotation = transform.localRotation * transform.parent->worldRotation;
        transform.modelMatrix = transform.getParentMatrix() * local;
        //TODO may be computed in more optimal way
        transform.toModelMatrix = glm::inverse(transform.modelMatrix);
        transform.dirty = false;
    }
    

    for(Transform* t : transform.children)
    {
        process(*t, dirty);
    }
}