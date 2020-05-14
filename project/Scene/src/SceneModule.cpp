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

        local = glm::translate(local, transform.getLocalPosition());
        local = local * glm::toMat4(transform.getLocalRotation());
        local = glm::scale(local, transform.getLocalScale());
        
        transform.localToWorldMatrix = parentsMatrix;
        transform.worldToLocalMatrix = glm::inverse(transform.localToWorldMatrix);

        global = parentsMatrix * local;

/*
TODO RETURN TO THIS IDEA
    #pragma region WTL
    
        glm::vec3 scale = transform.getLocalScale();
        local = glm::mat4(1);
        local = glm::scale(local, glm::vec3(1/scale[0],1/scale[1],1/scale[2]));
        local = local * glm::toMat4(glm::inverse(transform.getLocalRotation()));
        local = glm::translate(local, -transform.getLocalPosition());

    #pragma endregion
*/
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