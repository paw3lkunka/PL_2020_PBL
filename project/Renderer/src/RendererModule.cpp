#include "RendererModule.hpp"

#include "Message.inl"
#include "MeshQuad.hpp"
#include <glm/gtc/type_ptr.hpp>

//TODO: TEMP STRING STREAM
#include <sstream>

void RendererModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::RENDERER_ADD_MESH_TO_QUEUE:
            renderQueue.push(msg.getValue<MeshRenderer*>());
            break;
        case Event::RENDERER_ADD_BILLBOARD_TO_QUEUE:
            billboardQueue.push(msg.getValue<BillboardRenderer*>());
            break;
        case Event::RENDERER_SET_PROJECTION_MATRIX:
            projectionChanged = true;
            projectionMatrix = msg.getValue<glm::mat4*>(); // TODO: No need to set pointer every time
            break;
        case Event::RENDERER_SET_VIEW_MATRIX:
            viewChanged = true;
            viewMatrix = msg.getValue<glm::mat4*>(); // TODO: No need to set pointer every time
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

    // * ===== Setup Uniform Buffer Object for vp matrices =====
    glGenBuffers(1, &viewProjectionBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, viewProjectionBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, viewProjectionBuffer, 0, 2 * sizeof(glm::mat4));

    // * ===== Setup Uniform Buffer Object for bone info =====
    glGenBuffers(1, &boneBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
    glBufferData(GL_UNIFORM_BUFFER, VertexSkinned::MAX_BONES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 1, boneBuffer, 0, VertexSkinned::MAX_BONES * sizeof(glm::mat4));

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

    // ! ----- Make buffer for billboard quad rendering -----
    glGenVertexArrays(1, &billboardVao);
    glGenBuffers(1, &billboardVbo);
    glGenBuffers(1, &instancedVbo);

    glBindVertexArray(billboardVao);
    glBindBuffer(GL_ARRAY_BUFFER, billboardVbo);
    float vertices[20] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, instancedVbo);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

#include "Entity.hpp"

void RendererModule::render()
{
    if (window != nullptr)
    {
        // ? +++++ Clear the buffers selected in options (createInfo) +++++
        glClearColor(createInfo.clearColor.x, createInfo.clearColor.y, createInfo.clearColor.z, 1.0f);
        glClear(createInfo.clearFlags);

        // ? ++++++ Send projection matrices to UBO if needed +++++
        glBindBuffer(GL_UNIFORM_BUFFER, viewProjectionBuffer);
        if (projectionChanged)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), projectionMatrix);
            projectionChanged = false;
        }
        if (viewChanged)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), viewMatrix);
            viewChanged = false;
        }

        // ? +++++ Send skinning data to ubo +++++
        glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
        for(auto &bone : *bones)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, bone.first * sizeof(glm::mat4), sizeof(glm::mat4), &bone.second);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // ? +++++ Execute (order 66) rendering loop +++++
        while(!renderQueue.empty())
        {
            renderQueue.front()->material->use();
            renderQueue.front()->material->getShaderPtr()->setMat4("model", renderQueue.front()->modelMatrix);
            renderQueue.front()->mesh->render();
            renderQueue.pop();
        }

        // TODO Proper instanced rendering
        if (!billboardQueue.empty())
        {
            billboardQueue.front()->material->use();
            int i = 0, count = billboardQueue.size();
            glBindBuffer(GL_ARRAY_BUFFER, instancedVbo);
            glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
            while(!billboardQueue.empty())
            {
                glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), &billboardQueue.front()->modelMatrix);
                billboardQueue.pop();
                ++i;
            }
            glBindVertexArray(billboardVao);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
            glBindVertexArray(0);
        }

        // ? +++++ Render skybox with appropriate depth test function +++++

        if (skyboxMaterial != nullptr)
        {
            glDepthFunc(GL_LEQUAL);
            glm::mat4 viewStatic = glm::mat4(glm::mat3(*viewMatrix));
            skyboxMaterial->setMat4("viewStatic", viewStatic);
            skyboxMaterial->use();
            glBindVertexArray(skyboxVao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
            glBindVertexArray(0);
        }

        // ? +++++ Swap buffers for double-buffering +++++
        glfwSwapBuffers(window);
    }
}