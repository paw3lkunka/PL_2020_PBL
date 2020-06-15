#include "RendererModule.hpp"

#include "Message.inl"
#include "MeshQuad.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "CubemapHdr.hpp"
#include "Core.hpp"
#include "DistanceComparer.hpp"
#include "UiRenderer.inl"
#include "TextRenderer.inl"
#include "TerrainRenderer.inl"
#include "BuiltInShaders.inl"
#include "BuiltInShapes.inl"

#include <algorithm>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int RendererModule::lastMatID = 0;
unsigned int RendererModule::lastShaderID = 0;

RendererModule::~RendererModule()
{
    delete internalErrorMat;
    delete internalShaderError;
    delete irradianceMap;
    delete directionalDepth;
    delete ssaoMap;
}

void RendererModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::RENDERER_ADD_MESH_TO_QUEUE:
        {
            MeshRenderer* mr = msg.getValue<MeshRenderer*>();
            if (mr->material != nullptr)
            {
                if (mr->material->isInstancingEnabled())
                {
                    // ? +++++ Create new instanced packet or just add an instance matrix +++++
                    auto packet = instancedPackets.insert({key(mr->mesh->getID(), mr->material->getID()), InstancedPacket(mr->mesh, mr->material)});
                    packet.first->second.instanceMatrices.push_back(&mr->modelMatrix);
                    packet.first->second.instanceOccluded.push_back(true);
                }
                else
                {
                    // ? +++++ Create new packet for normal rendering +++++
                    normalPackets.push_back(NormalPacket(mr->mesh, mr->material, mr->modelMatrix));
                }
            }
            else
            {
                // ? +++++ Create new packet for normal rendering +++++
                normalPackets.push_back(NormalPacket(mr->mesh, internalErrorMat, mr->modelMatrix));
            }
            
            break;
        }
        case Event::RENDERER_ADD_TERRAIN_TO_QUEUE:
        {
            TerrainRenderer* tr = msg.getValue<TerrainRenderer*>();
            if (tr->material != nullptr)
            {
                terrainPackets.push_back(TerrainPacket(tr->terrainMesh, tr->material, tr->splatmap, tr->modelMatrix));
            }
            else
            {
                terrainPackets.push_back(TerrainPacket(tr->terrainMesh, internalErrorMat, tr->splatmap, tr->modelMatrix));
            }
            break;
        }
        case Event::RENDERER_ADD_UI_TO_QUEUE:
        {
            UiRenderer* uiElementToAdd = msg.getValue<UiRenderer*>();
            if (uiElementToAdd != nullptr)
            {
                spritePackets.push_back(SpritePacket(&uiElementToAdd->mesh, uiElementToAdd->material, uiElementToAdd->modelMatrix));
                uiQueue.push_back(&spritePackets.back());
            }
            TextRenderer* textElementToAdd = msg.getValue<TextRenderer*>();
            if (textElementToAdd != nullptr)
            {
                textPackets.push_back(TextPacket(&textElementToAdd->mesh, textElementToAdd->material, textElementToAdd->modelMatrix));
                uiQueue.push_back(&textPackets.back());
            }
            break;
        }
        case Event::RENDERER_ADD_LIGHT:
        {
            Light* lightToAdd = msg.getValue<Light*>();
            switch(lightToAdd->lightType)
            {
                case LightType::Directional:
                    directionalLight = lightToAdd;
                    break;
                case LightType::Point:
                    // TODO: Implement point lights
                    std::cerr << "Point lights not yet implemented.\n";
                    break;
                case LightType::Spot:
                    // TODO: Implement spot lights
                    std::cerr << "Spot lights not yet implemented.\n";
                    break;
            }
            break;
        }
        case Event::RENDERER_SET_MAIN_CAMERA:
            cameraMain = msg.getValue<Camera*>();
            break;
        case Event::RENDERER_SET_BONE_TRANSFORMS_PTR:
            bones = msg.getValue<std::map<int, glm::mat4>*>();
            break;
        case Event::KEY_PRESSED:
            if (msg.getValue<int>() == GLFW_KEY_F)
            {
                frustumCullingEnabled = !frustumCullingEnabled;
            }
            break;

        case Event::WINDOW_RESIZED:
            glm::ivec2 size = msg.getValue<glm::ivec2>();
            // Hdr framebuffers
            glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, hdrBrightBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);

            // Bloom framebuffers
            for (size_t i = 0, j = 2; i < 10; i += 2, j *= 2)
            {
                glBindTexture(GL_TEXTURE_2D, blurBuffers[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x / j, size.y / j, 0, GL_RGBA, GL_FLOAT, nullptr);

                glBindTexture(GL_TEXTURE_2D, blurBuffers[i+1]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x / j, size.y / j, 0, GL_RGBA, GL_FLOAT, nullptr);
            }
            // Gbuffers
            glBindTexture(GL_TEXTURE_2D, gbufferPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x / 2, size.y / 2, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, gbufferNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x / 2, size.y / 2, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindRenderbuffer(GL_RENDERBUFFER, gbufferDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x / 2, size.y / 2);

            // SSAO
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x / 2, size.y / 2, 0, GL_RED, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x / 2, size.y / 2, 0, GL_RED, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            break;
    }
}

void RendererModule::initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo)
{
    this->window = window;
    this->createInfo = createInfo;
    this->skyboxMaterial = skyboxMaterial;

    normalPackets.reserve(DRAW_CALL_NORMAL_ALLOCATION);
    terrainPackets.reserve(DRAW_CALL_TERRAIN_ALLOCATION);
    instancedPackets.reserve(DRAW_CALL_INSTANCED_ALLOCATION);
    spritePackets.reserve(DRAW_CALL_UI_ALLOCATION);
    textPackets.reserve(DRAW_CALL_TEXT_ALLOCATION);

    if (createInfo.cullFace)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(createInfo.cullFaceMode);
        glFrontFace(createInfo.cullFrontFace);
    }

    if (createInfo.depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }

    if (createInfo.wireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    internalShaderError = new Shader("internalShaderError", BuiltInShaders::baseVertexCode, BuiltInShaders::internalErrorFragmentCode, nullptr, false);
    internalErrorMat = new Material(internalShaderError, "internalErrorMat", RenderType::Opaque, false, false);
    combineShader = GetCore().objectModule.newShader("", "Resources/Shaders/PostProcessing/Quad.vert", "Resources/Shaders/PostProcessing/AllCombine.frag", nullptr, false);
    blurShader = GetCore().objectModule.newShader("", "Resources/Shaders/PostProcessing/Quad.vert", "Resources/Shaders/PostProcessing/GaussianBlur.frag", nullptr, false);
    gbufferShader = GetCore().objectModule.newShader("", "Resources/Shaders/PostProcessing/ViewSpaceValues.vert", "Resources/Shaders/PostProcessing/PosNormBuffers.frag", nullptr, false);
    ssaoShader = GetCore().objectModule.newShader("", "Resources/Shaders/PostProcessing/Quad.vert", "Resources/Shaders/PostProcessing/SSAO.frag", nullptr, false);
    ssaoBlurShader = GetCore().objectModule.newShader("", "Resources/Shaders/PostProcessing/Quad.vert", "Resources/Shaders/PostProcessing/SSAOblur.frag", nullptr, false);

    // * ===== Setup Uniform Buffer Object for camera =====
    glGenBuffers(1, &cameraBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, cameraBuffer, 0, 2 * sizeof(glm::mat4) + sizeof(glm::vec4));

    // * ===== Setup Uniform Buffer Object for bone info =====
    glGenBuffers(1, &boneBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
    glBufferData(GL_UNIFORM_BUFFER, VertexSkinned::MAX_BONES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 1, boneBuffer, 0, VertexSkinned::MAX_BONES * sizeof(glm::mat4));

    // * ===== Setup Uniform Buffer Object for directional light =====
    glGenBuffers(1, &directionalLightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, directionalLightBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 3, directionalLightBuffer, 0, 3 * sizeof(glm::vec4));

    // * ===== Setup Uniform Buffer Object for shadow mapping =====
    glGenBuffers(1, &shadowMappingBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, shadowMappingBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 4, shadowMappingBuffer, 0, sizeof(glm::mat4));

    // * ===== Setup Uniform Buffer Object for time =====
    glGenBuffers(1, &timeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, timeBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 5, timeBuffer, 0, sizeof(float));

    // * ===== Setup buffer for simple gizmo rendering =====
    glGenVertexArrays(1, &gizmoVao);
    glGenBuffers(1, &gizmoVbo);

    glBindVertexArray(gizmoVao);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);

    // * ===== Generate mesh for skybox rendering =====
    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &skyboxVbo);

    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
    
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), BuiltInShapes::cubeTri, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // * ===== Create framebuffer for depth map =====
    glGenFramebuffers(1, &depthMapFBO);

    TextureCreateInfo depthCreateInfo = {};
    depthCreateInfo.format = GL_DEPTH_COMPONENT;
    depthCreateInfo.generateMipmaps = false;
    depthCreateInfo.height = SHADOW_HEIGHT;
    depthCreateInfo.width = SHADOW_WIDTH;
    depthCreateInfo.type = GL_FLOAT;
    depthCreateInfo.minFilter = GL_LINEAR;
    depthCreateInfo.magFilter = GL_LINEAR;
    depthCreateInfo.wrapMode = GL_CLAMP_TO_BORDER;
    directionalDepth = new Texture(nullptr, depthCreateInfo, "");

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepth->getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // * ===== Create framebuffer for hdr rendering =====
    glGenFramebuffers(1, &hdrFBO);

    // Generate color attachment
    glGenTextures(1, &hdrColorBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth, GetCore().windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate bright color attachment
    glGenTextures(1, &hdrBrightBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrBrightBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth, GetCore().windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate depth renderbuffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetCore().windowWidth, GetCore().windowHeight);

    // Attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdrBrightBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Hdr framebuffer not complete!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // * ===== Framebuffers for ping pong gaussian blur =====
    // TODO: Framebuffer for every color attachment should be more performant, maybe also use MRT to speed things up
    glGenFramebuffers(10, blurFBO);
    glGenTextures(10, blurBuffers);

    for (size_t i = 0, j = 2; i < 10; i += 2, j *= 2)
    {
        glBindTexture(GL_TEXTURE_2D, blurBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth / j, GetCore().windowHeight / j, 0, GL_RGBA, GL_FLOAT, nullptr);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurBuffers[i], 0);

        glBindTexture(GL_TEXTURE_2D, blurBuffers[i+1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth / j, GetCore().windowHeight / j, 0, GL_RGBA, GL_FLOAT, nullptr);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i+1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurBuffers[i+1], 0);
    }

    // * ===== Gbuffer framebuffer =====
    glGenFramebuffers(1, &gbufferFBO);
    glGenTextures(1, &gbufferPosition);
    glGenTextures(1, &gbufferNormal);

    glBindTexture(GL_TEXTURE_2D, gbufferPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth / 2, GetCore().windowHeight / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, gbufferNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetCore().windowWidth / 2, GetCore().windowHeight / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbufferPosition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbufferNormal, 0);

    unsigned int gbufferAttachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, gbufferAttachments);

    glGenRenderbuffers(1, &gbufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gbufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetCore().windowWidth / 2, GetCore().windowHeight / 2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gbufferDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Gbuffer framebuffer not complete!\n";
    }

    // * ===== SSAO kernel randomization =====
    ssaoKernel.reserve(32);
    for (size_t i = 0; i < 32; i++)
    {
        glm::vec3 sample = {
            GetCore().randomFloat01R() * 2.0f - 1.0f,
            GetCore().randomFloat01R() * 2.0f - 1.0f,
            GetCore().randomFloat01R()
        };
        sample = glm::normalize(sample);
        sample *= GetCore().randomFloat01R();

        float scale = static_cast<float>(i) / 32.0f;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoKernel.push_back(sample);
    }
    
    ssaoNoise.reserve(16);
    for (size_t i = 0; i < 16; i++)
    {
        glm::vec3 noise = {
            GetCore().randomFloat01R() * 2.0 - 1.0,
            GetCore().randomFloat01R() * 2.0 - 1.0,
            0.0f
        };
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &ssaoNoiseTex);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // * ===== SSAO framebuffer init =====
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, GetCore().windowWidth / 2, GetCore().windowHeight / 2, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    // Ssao blur
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, GetCore().windowWidth / 2, GetCore().windowHeight / 2, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    ssaoMap = new Texture(ssaoColorBufferBlur);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RendererModule::render()
{
    if (window != nullptr)
    {
        //TODO Remove if it became useless
        // !IMGUI RENDER
        ImGui::Render();

        glm::mat4 VP = glm::mat4(1.0f);

        // ? +++++ Send time for use to shaders +++++
        glBindBuffer(GL_UNIFORM_BUFFER, timeBuffer);
        float time = (float)GetCore().getCurrentFrameStart();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &time);

        // ? +++++ Shadow mapping section +++++
        if (directionalLight != nullptr)
        {
            glm::mat4 lightMatrix = *directionalLight->modelMatrix;
            lightMatrix[3].x = cameraMain->getFrustum().position.x;
            lightMatrix[3].y = cameraMain->getFrustum().position.y;
            lightMatrix[3].z = cameraMain->getFrustum().position.z;

            // ? ++++++ Send directional light matrices to UBO +++++
            glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

            glm::mat4 directionalProjMat = glm::ortho(-256.0f, 256.0f, -256.0f, 256.0f, 1000.0f, -1000.0f);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &directionalProjMat);
            glm::mat4 directionalViewMat = glm::inverse(lightMatrix);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &directionalViewMat);

            // ? +++++ Calculate the View-Projection matrix +++++
            VP = directionalProjMat * directionalViewMat;

            // ? +++++ Send light view matrix to shadow mapping buffer +++++
            glBindBuffer(GL_UNIFORM_BUFFER, shadowMappingBuffer);

            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &VP);

            // ? +++++ Render depth buffer for shadow mapping +++++
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

            //glCullFace(GL_FRONT);
            glDisable(GL_CULL_FACE);
            glClear(GL_DEPTH_BUFFER_BIT);

            // TODO: Culling for directional light shadow mapping
            // * ===== Terrain packets shadow mapping ====
            for(auto& packet : terrainPackets)
            {
                packet.render(VP);
            }

            // * ===== Normal unordered packets shadow mapping =====
            for(auto& packet : normalPackets)
            {
                if (packet.material->getRenderType() == RenderType::Opaque)
                {
                    packet.render(VP);
                }
            }

            // * ===== Instantiated packets shadow mapping =====
            for(auto ipacket : instancedPackets)
            {
                ipacket.second.render(VP);
            }

            glEnable(GL_CULL_FACE);
            //glCullFace(GL_BACK);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // ? +++++ Send skinning data to ubo +++++
        if (bones != nullptr)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
            for(auto &bone : *bones)
            {
                glBufferSubData(GL_UNIFORM_BUFFER, bone.first * sizeof(glm::mat4), sizeof(glm::mat4), &bone.second);
            }
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        // ? +++++ Calculate the View-Projection matrix +++++
        VP = cameraMain->projectionMatrix * cameraMain->viewMatrix;

        // ? ++++++ Send camera matrices to UBO +++++
        glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &cameraMain->projectionMatrix);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &cameraMain->viewMatrix);

        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec4), &cameraMain->getFrustum().position);

        // ? +++++ Send directional light data to UBO +++++
        if (directionalLight != nullptr)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, directionalLightBuffer);

            glm::vec3 direction = glm::vec3((*directionalLight->modelMatrix)[2]);
            glm::vec4 color = glm::vec4(directionalLight->color, directionalLight->intensity);
            glm::vec4 ambient = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);

            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &direction);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), &color);
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), &ambient);
        }

        // ? +++++ Perform frustum culling +++++
        glViewport(0, 0, Core::windowWidth / 2, Core::windowHeight / 2);
        glClear(createInfo.clearFlags);

        if (frustumCullingEnabled)
        {
            calculateFrustumPlanes();
            calculateFrustumPoints();
        }
        else
        {
            drawFrustum(VP);
        }

        // * ===== Normal packets =====
        for(auto& packet : normalPackets)
        {
            //drawBounds(packet.mesh->bounds, *internalErrorMat, packet.modelMatrix, VP);
            if (objectInFrustum(packet.mesh->bounds, packet.modelMatrix))
            {
                switch(packet.material->getRenderType())
                {
                    case RenderType::Opaque:
                        opaqueQueue.push_back(&packet);
                        break;
                    case RenderType::Transparent:
                        transparentQueue.push_back(&packet);
                        break;
                }
            }
        }

        // * ===== Terrain packets =====
        for(auto& packet : terrainPackets)
        {
            if (objectInFrustum(packet.mesh->bounds, packet.modelMatrix))
            {
                terrainQueue.push_back(&packet);
            }
        }

        // * ===== Instanced packets =====
        for(auto& packet : instancedPackets)
        {
            int i = 0;
            bool wholePacketOccluded = true;
            for(auto matrix : packet.second.instanceMatrices)
            {
                //drawBounds(packet.second.mesh->bounds, *internalErrorMat, *matrix, VP);
                if (objectInFrustum(packet.second.mesh->bounds, *matrix))
                {
                    packet.second.instanceOccluded[i] = false;
                    wholePacketOccluded = false;
                }
                i++;
            }

            if (!wholePacketOccluded)
            {
                opaqueQueue.push_back(&(packet.second));
            }
        }

        // ? +++++ Sort the render queue +++++
        std::sort(opaqueQueue.begin(), opaqueQueue.end(), 
            [](RenderPacket* a, RenderPacket* b) { return a->material->getID() > b->material->getID(); });

        // ? +++++ Gbuffer rendering pass ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto packet : terrainQueue)
        {
            packet->material->getTexturePtr("diffuse0")->bind(0);
            gbufferShader->setInt("diffuse", 0);
            packet->renderWithShader(gbufferShader, VP);
        }

        for(auto packet : opaqueQueue)
        {
            packet->material->getTexturePtr("diffuse")->bind(0);
            gbufferShader->setInt("diffuse", 0);
            packet->renderWithShader(gbufferShader, VP);
        }

        RendererModule::lastMatID = 0;

        // ? +++++ SSAO rendering ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbufferPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbufferNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);

        ssaoShader->use();
        for (size_t i = 0; i < 32; i++)
        {
            ssaoShader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        }
        ssaoShader->setMat4("projection", cameraMain->projectionMatrix);
        glm::vec3 noiseScale = {GetCore().windowWidth / 4.0f, GetCore().windowHeight / 4.0f, 0.0f};
        ssaoShader->setVec3("noiseScale", noiseScale);
        ssaoShader->setFloat("radius", 10.5f);
        ssaoShader->setFloat("bias", 0.025f);
        ssaoShader->setInt("gPosition", 0);
        ssaoShader->setInt("gNormal", 1);
        ssaoShader->setInt("texNoise", 2);
        drawQuad();

        // Ssao blur
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

        ssaoBlurShader->use();
        ssaoBlurShader->setInt("ssaoInput", 0);

        drawQuad();

        glBindTexture(GL_TEXTURE_2D, 0);

        // ? +++++ Bind hdr framebuffer for color pass +++++++++++++++++++++++++++++++++++++++++++++++++++
        glViewport(0, 0, Core::windowWidth, Core::windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // ! +++++ Terrain +++++
        while(!terrainQueue.empty())
        {
            terrainQueue.front()->material->setTexture("directionalShadowMap", directionalDepth);
            terrainQueue.front()->material->setTexture("irradianceMap", irradianceMap);
            terrainQueue.front()->render(VP);
            terrainQueue.pop_front();
        }

        // ! +++++ Opaque +++++
        while(!opaqueQueue.empty())
        {
            // HACK: Only one of the materials should be double sided
            std::string matName = std::string(opaqueQueue.front()->material->getName());
            bool doubleSided = matName == "Conifer Leaves BODT" ||
                                matName == "Grass_rushes";
            if (doubleSided)
            {
                glDisable(GL_CULL_FACE);
            }
            opaqueQueue.front()->material->setTexture("directionalShadowMap", directionalDepth);
            opaqueQueue.front()->material->setTexture("irradianceMap", irradianceMap);
            opaqueQueue.front()->render(VP);
            opaqueQueue.pop_front();
            if (doubleSided)
            {
                glEnable(GL_CULL_FACE);
            }
        }

        // ! +++++ Skybox +++++

        if (skyboxMaterial != nullptr)
        {
            glDepthFunc(GL_LEQUAL);
            glm::mat4 viewStatic = glm::mat4(glm::mat3(cameraMain->viewMatrix));
            glBindVertexArray(skyboxVao);
            skyboxMaterial->setMat4("viewStatic", viewStatic);
            skyboxMaterial->use();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }

        // ! +++++ Transparent +++++
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::sort(transparentQueue.begin(), transparentQueue.end(), DistanceComparer(cameraMain->getFrustum().position));

        while(!transparentQueue.empty())
        {
            transparentQueue.front()->material->setTexture("directionalShadowMap", directionalDepth);
            transparentQueue.front()->material->setTexture("irradianceMap", irradianceMap);
            transparentQueue.front()->render(VP);
            transparentQueue.pop_front();
        }

        // ? +++++ Rendering bloom ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        bool horizontal = true, firstIteration = true, firstBloom = true;
        int amount = 9;

        glActiveTexture(GL_TEXTURE0);

        blurShader->use();
        blurShader->setInt("image", 0);

        for (size_t i = 0, j = 2; i < 10; i += 2, j *= 2)
        {
            glViewport(0, 0, Core::windowWidth / j, Core::windowHeight / j);

            for (size_t k = 0; k < amount; k++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i + horizontal]);
                blurShader->setInt("horizontal", horizontal);
                if (firstIteration)
                {
                    glBindTexture(GL_TEXTURE_2D, firstBloom ? hdrBrightBuffer : blurBuffers[i + !horizontal]);
                    firstIteration = false;
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, firstBloom ? blurBuffers[i - 2] : blurBuffers[i + !horizontal]);
                }

                drawQuad();

                horizontal = !horizontal;
                if (firstBloom)
                {
                    firstBloom = false;
                }
            }

            firstBloom = true;
            horizontal = true;
        }

        // ? +++++ Render to main frame ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        glViewport(0, 0, Core::windowWidth, Core::windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ! ----- Combine -----
        combineShader->use();
        combineShader->setInt("scene", 0);
        combineShader->setInt("bloom0", 1);
        combineShader->setInt("bloom1", 2);
        combineShader->setInt("bloom2", 3);
        combineShader->setInt("bloom3", 4);
        combineShader->setInt("bloom4", 5);
        combineShader->setInt("ssao", 6);
        combineShader->setFloat("exposure", cameraMain->exposure);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurBuffers[0]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, blurBuffers[2]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blurBuffers[4]);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, blurBuffers[6]);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, blurBuffers[8]);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        drawQuad();

        // ? +++++ Overlay UI rendering loop +++++
        glm::mat4 orthoScreen = glm::ortho(0.0f, (float)Core::windowWidth, 0.0f, (float)Core::windowHeight);

        glDisable(GL_DEPTH_TEST);
        while(!uiQueue.empty())
        {
            uiQueue.front()->render(orthoScreen);
            uiQueue.pop_front();
        }
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        //TODO Remove if it became useless
        // !IMGUI RENDER
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ? +++++ Swap buffers for double-buffering +++++
        glfwSwapBuffers(window);

        // ? +++++ Clear the render packets +++++
        RendererModule::lastMatID = 0;
        RendererModule::lastShaderID = 0;
        normalPackets.clear();
        terrainPackets.clear();
        instancedPackets.clear();
        spritePackets.clear();
        textPackets.clear();
    }
}

void RendererModule::calculateFrustumPlanes()
{
    const ViewFrustum& frustum = cameraMain->getFrustum();

    glm::vec3 nearCenter = frustum.position + frustum.front * frustum.nearDist;
    glm::vec3 farCenter = frustum.position + frustum.front * frustum.farDist;

    glm::vec3 aux, point, normal;

    frustumPlanes[NEARP][NORMAL] = frustum.front;
    frustumPlanes[NEARP][POINT] = nearCenter;

    frustumPlanes[FARP][NORMAL] = -frustum.front;
    frustumPlanes[FARP][POINT] = farCenter;

    point = nearCenter + frustum.up * frustum.Hnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    normal = glm::normalize(glm::cross(aux, frustum.right));
    frustumPlanes[TOP][NORMAL] = normal;
    frustumPlanes[TOP][POINT] = point;

    point = nearCenter - frustum.up * frustum.Hnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    normal = glm::normalize(glm::cross(frustum.right, aux));
    frustumPlanes[BOTTOM][NORMAL] = normal;
    frustumPlanes[BOTTOM][POINT] = point;

    point = nearCenter - frustum.right * frustum.Wnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    normal = glm::normalize(glm::cross(aux, frustum.up));
    frustumPlanes[LEFT][NORMAL] = normal;
    frustumPlanes[LEFT][POINT] = point;

    point = nearCenter + frustum.right * frustum.Wnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    normal = glm::normalize(glm::cross(frustum.up, aux));
    frustumPlanes[RIGHT][NORMAL] = normal;
    frustumPlanes[RIGHT][POINT] = point;
}

bool RendererModule::objectInFrustum(Bounds& meshBounds, glm::mat4& modelMatrix)
{
    bool result = false;

    int out, in;
    // ? +++++ Check for each frustum plane +++++
    for (size_t i = 0; i < 6; i++)
    {
        // Reset counters
        out = 0;
        in = 0;
        for (size_t j = 0; j < 8 && (in == 0 || out == 0); j++)
        {
            // ? +++++ Check if corner is outside or inside +++++
            if (pointToPlaneDistance2(frustumPlanes[i][POINT], frustumPlanes[i][NORMAL], modelMatrix * glm::vec4(meshBounds.getPoint(j), 1.0f)) < 0.0f)
            {
                out++;
            }
            else
            {
                in++;
            }
        }
        
        // ? +++++ If all corners are out +++++
        if (!in)
        {
            return false;
        }
        // ? +++++ If some corners are in +++++
        else
        {
            result = true;
        }
    }
    return result;
}

float RendererModule::pointToPlaneDistance2(glm::vec3& pointOnPlane, glm::vec3& planeNormal, glm::vec3 point)
{
    return glm::dot(point - pointOnPlane, planeNormal);
}

float RendererModule::pointToPlaneDistance(glm::vec3& pointOnPlane, glm::vec3& planeNormal, glm::vec3 point)
{
    float D = planeNormal.x * (-pointOnPlane.x) + planeNormal.y * (-pointOnPlane.y) + planeNormal.z * (-pointOnPlane.z);
    return planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z + D;
}

void RendererModule::drawBounds(Bounds& bounds, Material& material, glm::mat4& model, glm::mat4& VP)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec3 vertices[] = {
        bounds.getPoint(7),
        bounds.getPoint(4),
        bounds.getPoint(1),
        bounds.getPoint(6),
        bounds.getPoint(3),
        bounds.getPoint(4),
        bounds.getPoint(5),
        bounds.getPoint(7),
        bounds.getPoint(2),
        bounds.getPoint(1),
        bounds.getPoint(0),
        bounds.getPoint(3),
        bounds.getPoint(2),
        bounds.getPoint(5),
    };

    glBindVertexArray(gizmoVao);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVbo);
    glBufferData(GL_ARRAY_BUFFER, 14 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

    if (material.getID() != RendererModule::lastMatID)
    {
        material.use();
        RendererModule::lastMatID = material.getID();
    }
    material.setMVP(model, VP);
    material.setModel(model);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RendererModule::drawFrustum(glm::mat4& VP)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    glm::vec3 vertices[] = {
        frustumPoints[7],
        frustumPoints[4],
        frustumPoints[1],
        frustumPoints[6],
        frustumPoints[3],
        frustumPoints[4],
        frustumPoints[5],
        frustumPoints[7],
        frustumPoints[2],
        frustumPoints[1],
        frustumPoints[0],
        frustumPoints[3],
        frustumPoints[2],
        frustumPoints[5],
    };

    glBindVertexArray(gizmoVao);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVbo);
    glBufferData(GL_ARRAY_BUFFER, 14 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

    if (internalErrorMat->getID() != RendererModule::lastMatID)
    {
        internalErrorMat->use();
        RendererModule::lastMatID = internalErrorMat->getID();
    }
    glm::mat4 identity = glm::mat4(1.0f);
    internalErrorMat->setMVP(identity, VP);
    internalErrorMat->setModel(identity);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RendererModule::calculateFrustumPoints()
{
    const ViewFrustum& frustum = cameraMain->getFrustum();

    glm::vec3 nearCenter = frustum.position + frustum.front * frustum.nearDist;
    glm::vec3 farCenter = frustum.position + frustum.front * frustum.farDist;

    // =============== Z axis =========== Y axis ============================= X axis =========================
    frustumPoints[0] = nearCenter - (frustum.up * frustum.Hnear/2.0f) - (frustum.right * frustum.Wnear/2.0f); // -1, -1, -1
    frustumPoints[1] = farCenter - (frustum.up * frustum.Hfar/2.0f) - (frustum.right * frustum.Wfar/2.0f); // -1, -1, 1
    frustumPoints[2] = nearCenter + (frustum.up * frustum.Hnear/2.0f) - (frustum.right * frustum.Wnear/2.0f); // -1, 1, -1
    frustumPoints[3] = nearCenter - (frustum.up * frustum.Hnear/2.0f) + (frustum.right * frustum.Wnear/2.0f); // 1, -1, -1
    frustumPoints[4] = farCenter + (frustum.up * frustum.Hfar/2.0f) + (frustum.right * frustum.Wfar/2.0f); // 1, 1, 1
    frustumPoints[5] = nearCenter + (frustum.up * frustum.Hnear/2.0f) + (frustum.right * frustum.Wnear/2.0f); // 1, 1, -1
    frustumPoints[6] = farCenter - (frustum.up * frustum.Hfar/2.0f) + (frustum.right * frustum.Wfar/2.0f); // 1, -1, 1
    frustumPoints[7] = farCenter + (frustum.up * frustum.Hfar/2.0f) - (frustum.right * frustum.Wfar/2.0f); // -1, 1, 1
}

void RendererModule::generateCubemapConvolution(const Texture* cubemap, unsigned int dimensions)
{
    // ? +++++ Generate renderbuffers +++++
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions, dimensions);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ? +++++ Generate textures for convoluted environment map +++++
    unsigned int irrID;
    glGenTextures(1, &irrID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irrID);
    for (size_t i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, dimensions, dimensions, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    irradianceMap = new CubemapHdr(irrID);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions, dimensions);

    Shader irradianceShader("", BuiltInShaders::simpleCubemapVertex, BuiltInShaders::cubemapConvolution);
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    cubemap->bind(0);

    glViewport(0, 0, dimensions, dimensions);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    for (size_t i = 0; i < 6; i++)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irrID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawCube();
    }
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RendererModule::drawCube()
{
    static unsigned int cubeVao = 0;
    static unsigned int cubeVbo = 0;

    if (cubeVao == 0)
    {
        glGenVertexArrays(1, &cubeVao);
        glGenBuffers(1, &cubeVbo);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BuiltInShapes::cubeNormUvTri), BuiltInShapes::cubeNormUvTri, GL_STATIC_DRAW);

        glBindVertexArray(cubeVao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(cubeVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void RendererModule::drawQuad()
{
    static unsigned int quadVao = 0;
    static unsigned int quadVbo = 0;
    
    if (quadVao == 0)
    {
        glGenVertexArrays(1, &quadVao);
        glGenBuffers(1, &quadVbo);

        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BuiltInShapes::quadVertices), BuiltInShapes::quadVertices, GL_STATIC_DRAW);

        glBindVertexArray(quadVao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RendererModule::setSkybox(Material* skyboxMaterial)
{
    if (this->skyboxMaterial != nullptr)
    {
        this->skyboxMaterial->setAsSkybox(false);
    }
    skyboxMaterial->setAsSkybox(true);
    this->skyboxMaterial = skyboxMaterial;
    generateCubemapConvolution(skyboxMaterial->getTexturePtr("cubemap"), 32);
}