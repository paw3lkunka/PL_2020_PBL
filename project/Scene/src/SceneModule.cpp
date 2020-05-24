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

//TODO: Is this mat 4 parameter necessery?
void SceneModule::process(Transform& transform, bool dirty)
{
    dirty |= transform.dirty;
        
    glm::mat4 local(1); 

    if(dirty)
    {
        local = glm::translate(local, transform.getLocalPosition());
        local = local * glm::toMat4(transform.getLocalRotation());
        local = glm::scale(local, transform.getLocalScale());
        
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