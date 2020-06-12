#ifndef SCENEMODULE_HPP_
#define SCENEMODULE_HPP_

#include "IModule.inl"
#include "Transform.hh"

#include <glm/glm.hpp>

class Message;

class SceneModule : IModule
{
public:
    Transform rootNode;
    SceneModule();
    virtual ~SceneModule() = default;

    /**
     * @brief IModule implementation, by ow does nothing.
     * 
     * @param msg 
     */
    virtual void receiveMessage(Message msg);

    /**
     * @brief Update transforms in whole scene graph. 
     */
    void updateTransforms();

    /**
     * @brief remove all children for root scene - clear the scene
     */
    void unloadScene();

private:
    void process(Transform& transform, bool dirty);        
    Transform preRootNode;
};

#endif /* !SCENEMODULE_HPP_ */
