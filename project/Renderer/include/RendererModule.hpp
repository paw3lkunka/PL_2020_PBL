#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "MeshRenderer.inl"
#include "Light.inl"
#include "Camera.inl"

#include "packets/RenderPacket.inl"
#include "packets/NormalPacket.inl"
#include "packets/InstancedPacket.inl"

#include <deque>
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
    /**
     * @brief Cleanup after frame rendering
     */
    void frameCleanup();

private:
    static constexpr unsigned int DRAW_CALL_NORMAL_ALLOCATION = 512;
    static constexpr unsigned int DRAW_CALL_INSTANCED_ALLOCATION = 128;

    GLFWwindow* window;
    RendererModuleCreateInfo createInfo;
    // * Skybox variables
    unsigned int skyboxVao, skyboxVbo;
    Material* skyboxMaterial;
    // * Bone zone
    std::map<int, glm::mat4>* bones;
    // * UBO buffers
    unsigned int cameraBuffer, boneBuffer, directionalLightBuffer;
    
    Camera* cameraMain;

    std::deque<RenderPacket*> opaqueQueue;
    std::deque<RenderPacket*> transparentQueue;

    Light* directionalLight;

    // * Normal render packets collection
    std::vector<NormalPacket> normalPackets;
    // * Instanced render packet collection
    // ? size_t used as a key is actually two unsigned ints encoded to act as a pair
    std::unordered_map<size_t, InstancedPacket> instancedPackets; // ? +++++ size_t = mesh id << 32 | material id +++++

    __attribute__((always_inline)) inline size_t key(unsigned int first, unsigned int second) { return (size_t) first << 32 | second; }
};

#endif // _RENDERERMODULE_HPP
