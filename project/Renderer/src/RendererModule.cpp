#include "RendererModule.hpp"

#include "Message.inl"
#include <glm/gtc/type_ptr.hpp>

void RendererModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::RENDERER_ADD_TO_QUEUE:
            renderQueue.push(msg.getValue<MeshRenderer*>());
            break;
        case Event::RENDERER_SET_PROJECTION_MATRIX:
            projectionChanged = true;
            projectionMatrix = msg.getValue<glm::mat4*>(); // TODO: No need to set pointer every time
            break;
        case Event::RENDERER_SET_VIEW_MATRIX:
            viewChanged = true;
            viewMatrix = msg.getValue<glm::mat4*>(); // TODO: No need to set pointer every time
            break;
    }
}

void RendererModule::initialize(GLFWwindow* window, RendererModuleCreateInfo createInfo)
{
    this->window = window;
    this->createInfo = createInfo;

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
}

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
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // ? +++++ Execute (order 66) rendering loop +++++
        while(!renderQueue.empty())
        {
            renderQueue.front()->material->use();
            renderQueue.front()->material->getShaderPtr()->setMat4("model", *renderQueue.front()->modelMatrix);
            renderQueue.front()->mesh->render();
            renderQueue.pop();
        }

        // ? +++++ Swap buffers for double-buffering +++++
        glfwSwapBuffers(window);
    }
}