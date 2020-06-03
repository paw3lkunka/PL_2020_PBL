#include "RendererModule.hpp"

#include "Message.inl"
#include "mesh/MeshQuad.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Core.hpp"
#include "DistanceComparer.hpp"
#include "UiRenderer.inl"
#include "TextRenderer.inl"

#include <algorithm>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int RendererModule::lastMatID = std::numeric_limits<unsigned int>::max();
//unsigned int lastShaderID = std::numeric_limits<unsigned int>::max();

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
    }
}

void RendererModule::initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo, Material* skyboxMaterial)
{
    this->window = window;
    this->createInfo = createInfo;
    this->skyboxMaterial = skyboxMaterial;

    normalPackets.reserve(DRAW_CALL_NORMAL_ALLOCATION);
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

    // * ===== Setup buffer for simple gizmo rendering =====
    glGenVertexArrays(1, &gizmoVao);
    glGenBuffers(1, &gizmoVbo);

    glBindVertexArray(gizmoVao);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
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

        // ? +++++ Shadow mapping section +++++
        if (directionalLight != nullptr)
        {
            // I made copy instead of sneaky modification of matrix in transform.
            // That was super unsafe, and now i see, that encapsulating matrices inside transform was a good idea.
            //                                                                                  ~ Andrzej
            glm::mat4 lightMatrix = *directionalLight->modelMatrix;
            lightMatrix[3].x = cameraMain->getFrustum().position.x;
            lightMatrix[3].y = cameraMain->getFrustum().position.y;
            lightMatrix[3].z = cameraMain->getFrustum().position.z;

            // ? ++++++ Send directional light matrices to UBO +++++
            glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

            glm::mat4 directionalProjMat = glm::ortho(-512.0f, 512.0f, -512.0f, 512.0f, 2000.0f, -2000.0f);
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
            // * ===== Normal unordered packets shadow mapping =====
            for(auto& packet : normalPackets)
            {
                // TODO: Transparent shadows
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
        glViewport(0, 0, Core::windowWidth, Core::windowHeight);
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

        //std::cout << "Rendering " << opaqueQueue.size() + transparentQueue.size() << " after culling.\n";

        // ? +++++ Sort the render queue +++++
        std::sort(opaqueQueue.begin(), opaqueQueue.end(), 
            [](RenderPacket* a, RenderPacket* b) { return a->material->getID() > b->material->getID(); });

        // ? +++++ Execute (order 66) opaque rendering loop +++++
        while(!opaqueQueue.empty())
        {
            // TODO: Send directional light shadow map via ubo
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
        std::sort(transparentQueue.begin(), transparentQueue.end(), DistanceComparer(cameraMain->getFrustum().position));

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

    // std::cout << "==================================================================================\n";

    frustumPlanes[NEARP][NORMAL] = frustum.front;
    frustumPlanes[NEARP][POINT] = nearCenter;
    // std::cout << "Wektor((" << -frustum.front.x << ", " << -frustum.front.y << ", " << -frustum.front.z << "))\n(" 
    // << nearCenter.x << ", " << nearCenter.y << ", " << nearCenter.z << ")\n";

    frustumPlanes[FARP][NORMAL] = -frustum.front;
    frustumPlanes[FARP][POINT] = farCenter;
    // std::cout << "Wektor((" << frustum.front.x << ", " << frustum.front.y << ", " << frustum.front.z << "))\n(" 
    // << farCenter.x << ", " << farCenter.y << ", " << farCenter.z << ")\n";

    point = nearCenter + frustum.up * frustum.Hnear / 2.0f;
    //std::cout << "POINT: " << glm::to_string(point) << '\n';
    //std::cout << "POS: " << glm::to_string(frustum.position) << '\n';
    aux = point - frustum.position;
    //std::cout << "POINT - POS: " << glm::to_string(aux) << '\n';
    aux = glm::normalize(aux);
    //std::cout << "NORM(POINT - POS): " << glm::to_string(aux) << '\n';
    normal = glm::normalize(glm::cross(aux, frustum.right));
    frustumPlanes[TOP][NORMAL] = normal;
    frustumPlanes[TOP][POINT] = point;
    // std::cout << "Wektor((" << normal.x << ", " << normal.y << ", " << normal.z << "))\n(" 
    // << point.x << ", " << point.y << ", " << point.z << ")\n";

    point = nearCenter - frustum.up * frustum.Hnear / 2.0f;
    //std::cout << "POINT: " << glm::to_string(point) << '\n';
    //std::cout << "POS: " << glm::to_string(frustum.position) << '\n';
    aux = point - frustum.position;
    //std::cout << "POINT - POS: " << glm::to_string(aux) << '\n';
    aux = glm::normalize(aux);
    //std::cout << "NORM(POINT - POS): " << glm::to_string(aux) << '\n';
    //normal = -glm::cross(frustum.right, aux);
    normal = glm::normalize(glm::cross(frustum.right, aux));
    frustumPlanes[BOTTOM][NORMAL] = normal;
    frustumPlanes[BOTTOM][POINT] = point;
    // std::cout << "Wektor((" << normal.x << ", " << normal.y << ", " << normal.z << "))\n(" 
    // << point.x << ", " << point.y << ", " << point.z << ")\n";

    point = nearCenter - frustum.right * frustum.Wnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    normal = glm::normalize(glm::cross(aux, frustum.up));
    frustumPlanes[LEFT][NORMAL] = normal;
    frustumPlanes[LEFT][POINT] = point;
    // std::cout << "Wektor((" << normal.x << ", " << normal.y << ", " << normal.z << "))\n(" 
    // << point.x << ", " << point.y << ", " << point.z << ")\n";

    point = nearCenter + frustum.right * frustum.Wnear / 2.0f;
    aux = point - frustum.position;
    aux = glm::normalize(aux);
    // normal = -glm::cross(frustum.up, aux);
    normal = glm::normalize(glm::cross(frustum.up, aux));
    frustumPlanes[RIGHT][NORMAL] = normal;
    frustumPlanes[RIGHT][POINT] = point;
    // std::cout << "Wektor((" << normal.x << ", " << normal.y << ", " << normal.z << "))\n(" 
    // << point.x << ", " << point.y << ", " << point.z << ")\n";
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