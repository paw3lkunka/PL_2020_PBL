#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "MeshRenderer.inl"
#include "Light.inl"
#include "Camera.inl"

#include "NormalPacket.hpp"
#include "InstancedPacket.hpp"
#include "UiPacket.hpp"

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
    virtual ~RendererModule();
    
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

    // * Material switching optimization
    static unsigned int lastMatID;
    //static unsigned int lastShaderID;

private:
    static constexpr unsigned int DRAW_CALL_NORMAL_ALLOCATION = 512;
    static constexpr unsigned int DRAW_CALL_UI_ALLOCATION = 256;
    static constexpr unsigned int DRAW_CALL_INSTANCED_ALLOCATION = 128;

    // ? +++++ Built in shaders +++++
    const char* depthVertexCode = 
    "#version 430 core\nlayout(location=0) in vec3 position;\nuniform mat4 MVP;\nvoid main() {gl_Position = MVP * vec4(position, 1.0);}";
    const char* depthFragmentCode = 
    "#version 430 core\nvoid main() {}";
    const char* internalErrorFragmentCode = 
    "#version 430 core\nout vec4 FragColor;\nvoid main() { FragColor = vec4(1.0, 0.0, 1.0, 1.0); }";

    void calculateFrustumPlanes();
    void calculateFrustumPoints();
    bool objectInFrustum(Bounds& meshBounds, glm::mat4& modelMatrix);
    float pointToPlaneDistance2(glm::vec3& pointOnPlane, glm::vec3& planeNormal, glm::vec3 point);
    float pointToPlaneDistance(glm::vec3& pointOnPlane, glm::vec3& planeNormal, glm::vec3 point);
    void drawBounds(Bounds& bounds, Material& material, glm::mat4& model, glm::mat4& VP);
    void drawFrustum(glm::mat4& VP);

    GLFWwindow* window = nullptr;
    RendererModuleCreateInfo createInfo;
    // * Skybox variables
    unsigned int skyboxVao, skyboxVbo;
    Material* skyboxMaterial = nullptr;
    // * Bone zone
    std::map<int, glm::mat4>* bones = nullptr;
    // * UBO buffers
    unsigned int cameraBuffer, boneBuffer, directionalLightBuffer, shadowMappingBuffer;
    
    Camera* cameraMain = nullptr;
    enum { TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP };
    enum { NORMAL = 0, POINT };
    glm::vec3 frustumPlanes[6][2];
    glm::vec3 frustumPoints[8];
    bool frustumCullingEnabled = true;

    unsigned int gizmoVao, gizmoVbo;

    std::deque<RenderPacket*> opaqueQueue;
    std::deque<NormalPacket*> transparentQueue;
    std::deque<UiPacket*> uiQueue;

    Light* directionalLight = nullptr;
    unsigned int depthMapFBO = 0;
    unsigned int depthMap = 0;
    Shader* simpleDepth,* internalShaderError;
    Texture* directionalDepth;
    Material* internalErrorMat;
    // TODO: Move this to light properties
    static constexpr int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

    // * Normal render packets collection
    std::vector<NormalPacket> normalPackets;
    // * Ui render packets collection
    std::vector<UiPacket> uiPackets;
    // * Instanced render packet collection
    // ? size_t used as a key is actually two unsigned ints encoded to act as a pair
    std::unordered_map<size_t, InstancedPacket> instancedPackets; // ? +++++ size_t = mesh id << 32 | material id +++++

    __attribute__((always_inline)) inline size_t key(unsigned int first, unsigned int second) { return (size_t) first << 32 | second; }
};

#endif // _RENDERERMODULE_HPP
