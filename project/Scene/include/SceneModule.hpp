#ifndef SCENEMODULE_HPP_
#define SCENEMODULE_HPP_

#include "IModule.inl"
#include "Transform.inl"

#include <glm/glm.hpp>

class Message;

class SceneModule : IModule
{
    public:
        Transform rootNode;
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

    private:
        void process(Transform& transform, glm::mat4 parent, bool dirty);
};

#endif /* !SCENEMODULE_HPP_ */
