#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "MeshRenderer.inl"

#include <queue>
#include <GLFW/glfw3.h>

struct RendererModuleCreateInfo
{
    glm::vec3 clearColor;
    bool cullFace, depthTest, wireframeMode;
    GLenum cullFaceMode, cullFrontFace;
    GLbitfield clearFlags;
};

class GLFWwindow;

class RendererModule : public IModule
{
public:
    RendererModule() = default;
    virtual ~RendererModule() = default;
    virtual void receiveMessage(Message msg);

    void initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo);
    void render();

private:
    GLFWwindow* window;
    RendererModuleCreateInfo createInfo;
    unsigned int viewProjectionBuffer;
    bool projectionChanged = false, viewChanged = false;
    glm::mat4* projectionMatrix, * viewMatrix;
    std::queue<MeshRenderer*> renderQueue;
};

#endif // _RENDERERMODULE_HPP
