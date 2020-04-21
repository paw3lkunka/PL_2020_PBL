#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "MeshRenderer.inl"
#include "BillboardRenderer.inl"

#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//TODO documentation
struct RendererModuleCreateInfo
{
    glm::vec3 clearColor;
    bool cullFace, depthTest, wireframeMode;
    GLenum cullFaceMode, cullFrontFace;
    GLbitfield clearFlags;
};

class GLFWwindow;

    //TODO documentation
class RendererModule : public IModule
{
public:
    RendererModule() = default;
    virtual ~RendererModule() = default;
    //TODO documentation
    virtual void receiveMessage(Message msg);

    //TODO documentation
    void initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo);
    //TODO documentation
    void render();

private:
    GLFWwindow* window;
    RendererModuleCreateInfo createInfo;
    // HACK: Or not? Discuss this.
    unsigned int billboardVao, billboardVbo, instancedVbo;
    unsigned int viewProjectionBuffer;
    bool projectionChanged = false, viewChanged = false;
    glm::mat4* projectionMatrix, * viewMatrix;
    std::queue<MeshRenderer*> renderQueue;
    // HACK PLZ MAKE THIS POLYMORPHIC AS SOON AS YOU CAN
    std::queue<BillboardRenderer*> billboardQueue;
    
};

#endif // _RENDERERMODULE_HPP
