#include "RendererModule.hpp"

#include "Message.inl"
#include "mesh/MeshQuad.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Core.hpp"
#include "DistanceComparer.hpp"
#include "UiRenderer.inl"

#include <algorithm>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"

#include <glm/gtc/type_ptr.hpp>

RendererModule::~RendererModule()
{
    delete internalErrorMat;
    delete internalShaderError;
    delete simpleDepth;
    delete directionalDepth;
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
                    packet.first->second.instanceMatrices.push_back(mr->modelMatrix);
                    // ? +++++ If packet was created, add it to the queue +++++
                    if (packet.second)
                    {
                        switch(packet.first->second.material->getRenderType())
                        {
                            case RenderType::Opaque:
                                opaqueQueue.push_back(&packet.first->second);
                                break;
                            case RenderType::Transparent:
                                std::cerr << "Instancing is not allowed for transparent materials!" << std::endl;
                                break;
                        }
                    }
                }
                else
                {
                    // ? +++++ Create new packet for normal rendering +++++
                    normalPackets.push_back(NormalPacket(mr->mesh, mr->material, mr->modelMatrix));
                    // ? +++++ Add packet to render queue +++++
                    switch(normalPackets.back().material->getRenderType())
                    {
                        case RenderType::Opaque:
                            opaqueQueue.push_back(&normalPackets.back());
                            break;
                        case RenderType::Transparent:
                            transparentQueue.push_back(&normalPackets.back());
                            break;
                    }
                }
            }
            else
            {
                // ? +++++ Create new packet for normal rendering +++++
                normalPackets.push_back(NormalPacket(mr->mesh, internalErrorMat, mr->modelMatrix));
                opaqueQueue.push_back(&normalPackets.back());
            }
            
            break;
        }
        case Event::RENDERER_ADD_UI_TO_QUEUE:
        {
            UiRenderer* uiElementToAdd = msg.getValue<UiRenderer*>();
            uiPackets.push_back(UiPacket(&uiElementToAdd->mesh, uiElementToAdd->material, uiElementToAdd->modelMatrix));
            uiQueue.push_back(&uiPackets.back());
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
    }
}

void RendererModule::initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo, Material* skyboxMaterial)
{
    this->window = window;
    this->createInfo = createInfo;
    this->skyboxMaterial = skyboxMaterial;

    normalPackets.reserve(DRAW_CALL_NORMAL_ALLOCATION);
    instancedPackets.reserve(DRAW_CALL_INSTANCED_ALLOCATION);
    uiPackets.reserve(DRAW_CALL_UI_ALLOCATION);

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

    simpleDepth = new Shader(depthVertexCode, depthFragmentCode, nullptr, false);
    internalShaderError = new Shader(depthVertexCode, internalErrorFragmentCode, nullptr, false);
    internalErrorMat = new Material(internalShaderError, "internalErrorMat", RenderType::Opaque, false, false);

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

    // * ===== Generate mesh for skybox rendering =====

    if (skyboxMaterial != nullptr)
    {
        glGenVertexArrays(1, &skyboxVao);
        glGenBuffers(1, &skyboxVbo);

        glBindVertexArray(skyboxVao);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
        float skyboxVertices[] = 
        {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
        glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }


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

    // glGenTextures(1, &depthMap);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, , , 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepth->getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
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

        // ? +++++ Sort the render queue +++++
        std::sort(opaqueQueue.begin(), opaqueQueue.end(), 
            [](RenderPacket* a, RenderPacket* b) { return a->material->getID() > b->material->getID(); });

        // ? +++++ Shadow mapping section +++++
        if (directionalLight != nullptr)
        {
            directionalLight->modelMatrix->operator[](3).x = cameraMain->position.x;
            directionalLight->modelMatrix->operator[](3).y = cameraMain->position.y;
            directionalLight->modelMatrix->operator[](3).z = cameraMain->position.z;

            // ? ++++++ Send directional light matrices to UBO +++++
            glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

            glm::mat4 directionalProjMat = glm::ortho(-512.0f, 512.0f, -512.0f, 512.0f, 2000.0f, -2000.0f);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &directionalProjMat);
            glm::mat4 directionalViewMat = glm::inverse(*directionalLight->modelMatrix);
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
            for(auto packet : opaqueQueue)
            {
                packet->renderWithShader(simpleDepth, VP);
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

        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec4), &cameraMain->position);

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

        // ? +++++ Execute (order 66) opaque rendering loop +++++
        glViewport(0, 0, Core::windowWidth, Core::windowHeight);
        glClear(createInfo.clearFlags);
        while(!opaqueQueue.empty())
        {
            opaqueQueue.front()->material->setTexture("directionalShadowMap", directionalDepth);
            opaqueQueue.front()->render(VP);
            opaqueQueue.pop_front();
        }

        // ? +++++ Render skybox with appropriate depth test function +++++

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

        // ? +++++ Transparent rendering loop +++++
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::sort(transparentQueue.begin(), transparentQueue.end(), DistanceComparer(cameraMain->position));

        while(!transparentQueue.empty())
        {
            transparentQueue.front()->material->setTexture("directionalShadowMap", directionalDepth);
            transparentQueue.front()->render(VP);
            transparentQueue.pop_front();
        }

        // ? +++++ Overlay UI rendering loop +++++
        // Get screen ortho projection
        glm::mat4 orthoScreen = glm::ortho(0.0f, (float)Core::windowWidth, 0.0f, (float)Core::windowHeight);

        glDisable(GL_DEPTH_TEST);
        while(!uiQueue.empty())
        {
            //static int temp = 0;
            //temp %= 127;
            //int character = GetCore().objectModule.getFontPtrByName("KosugiMaru-Regular")->getCharTex((char)temp++);
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
        normalPackets.clear();
        instancedPackets.clear();
    }
}