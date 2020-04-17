#include "SceneModule.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Message.inl"

void SceneModule::receiveMessage(Message msg) {}

void SceneModule::updateTransforms()
{
    process(rootNode, glm::mat4(1), false);
}

void SceneModule::process(Transform& transform, glm::mat4 parentsMatrix, bool dirty)
{
    dirty |= transform.dirty;
        
    glm::mat4 local(1), global(1);

    if(dirty)
    {

        local = glm::translate(local, transform.localPosition);
        local = local * glm::toMat4(transform.localRotation);
        local = glm::scale(local, transform.localScale);
        
        global = parentsMatrix * local;

        transform.localToWorldMatrix = global;

        transform.dirty = false;
    }
    else
    {
        global = transform.localToWorldMatrix;
    }
    

    for(Transform* t : transform.children)
    {
        process(*t, global, dirty);
    }
}