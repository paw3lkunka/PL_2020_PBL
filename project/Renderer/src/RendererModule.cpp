#include "RendererModule.hpp"

#include "Message.inl"
#include "mesh/MeshQuad.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Core.hpp"

#include <algorithm>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"

//TODO: TEMP STRING STREAM
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

void RendererModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::RENDERER_ADD_MESH_TO_QUEUE:
        {
            MeshRenderer* mr = msg.getValue<MeshRenderer*>();
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
                            transparentQueue.push_back(&packet.first->second);
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
}

void RendererModule::render()
{
    if (window != nullptr)
    {
        //TODO Remove if it became useless
        // !IMGUI RENDER
        ImGui::Render();

        // ? +++++ Clear the buffers selected in options (createInfo) +++++
        glClear(createInfo.clearFlags);

        // ? ++++++ Send projection matrices to UBO if needed +++++
        glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &cameraMain->projectionMatrix);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &cameraMain->viewMatrix);

        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), &cameraMain->position);

        // ? +++++ Send directional light data to UBO +++++
        // if (directionalLight != nullptr)
        // {
        //     glBindBuffer(GL_UNIFORM_BUFFER, directionalLightBuffer);

        //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), directionalLight->modelMatrix)

        // }

        if(bones != nullptr)
        {
            // ? +++++ Send skinning data to ubo +++++
            glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
            for(auto &bone : *bones)
            {
                glBufferSubData(GL_UNIFORM_BUFFER, bone.first * sizeof(glm::mat4), sizeof(glm::mat4), &bone.second);
            }
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        

        // ? +++++ Calculate the View-Projection matrix +++++
        glm::mat4 VP = cameraMain->projectionMatrix * cameraMain->viewMatrix;

        // ? +++++ Sort the render queue +++++
        std::sort(opaqueQueue.begin(), opaqueQueue.end(), 
            [](RenderPacket* a, RenderPacket* b) { return a->material->getID() > b->material->getID(); });

        // ? +++++ Execute (order 66) opaque rendering loop +++++
        // * Setup
        glDisable(GL_BLEND);
        while(!opaqueQueue.empty())
        {
            opaqueQueue.front()->render(VP);
            opaqueQueue.pop_front();
        }

        // ? +++++ Render skybox with appropriate depth test function +++++

        if (skyboxMaterial != nullptr)
        {
            glDepthFunc(GL_LEQUAL);
            glm::mat4 viewStatic = glm::mat4(glm::mat3(cameraMain->viewMatrix));
            skyboxMaterial->setMat4("viewStatic", viewStatic);
            skyboxMaterial->use();
            glBindVertexArray(skyboxVao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
            glBindVertexArray(0);
        }

        // ? +++++ Transparent rendering loop +++++
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        while(!transparentQueue.empty())
        {
            transparentQueue.front()->render(VP);
            transparentQueue.pop_front();
        }
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