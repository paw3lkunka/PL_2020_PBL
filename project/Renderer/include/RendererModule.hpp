#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "MeshRenderer.inl"
#include "SkinnedMeshRenderer.inl"
#include "BillboardRenderer.inl"

#include <queue>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * @brief Renderer module create info
 * Setting cullFace to true requires cullFaceMode and cullFrontFace set
 * Setting depthTest to true requires 
 */
struct RendererModuleCreateInfo
{
    glm::vec3 clearColor;
    bool cullFace, depthTest, wireframeMode;
    GLenum cullFaceMode, cullFrontFace;
    GLbitfield clearFlags;
};

class GLFWwindow;

/**
 * @brief Renderer module, handles render queues and processes draw calls
 */
class RendererModule : public IModule
{
public:
    RendererModule() = default;
    virtual ~RendererModule() = default;
    
    /**
     * @brief Handles renderer message bus events
     * 
     * @param msg RENDERER_ADD_X_TO_QUEUE events
     */
    virtual void receiveMessage(Message msg);
    /**
     * @brief Initializes base renderer with values from RendererModuleCreate info and enables base instanced shapes
     * 
     * @param window GLFW window pointer
     * @param createInfo CreateInfoStruct with renderer parameters
     */
    void initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo, Material* skyboxMaterial = nullptr);
    /**
     * @brief Render current render queue
     */
    void render();

private:
    GLFWwindow* window;
    RendererModuleCreateInfo createInfo;
    // * Skybox variables
    unsigned int skyboxVao, skyboxVbo;
    Material* skyboxMaterial;
    // * Bone zone
    std::map<int, glm::mat4>* bones;
    // HACK: Or not? Discuss this.
    unsigned int billboardVao, billboardVbo, instancedVbo;
    unsigned int viewProjectionBuffer;
    bool projectionChanged = false, viewChanged = false;
    glm::mat4* projectionMatrix, * viewMatrix;
    std::queue<MeshRenderer*> renderQueue;
    // HACK PLZ MAKE THIS POLYMORPHIC AS SOON AS YOU CAN
    std::queue<BillboardRenderer*> billboardQueue;
    std::queue<SkinnedMeshRenderer*> skinnedQueue;
    
};

#endif // _RENDERERMODULE_HPP
