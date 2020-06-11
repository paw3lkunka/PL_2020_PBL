#include "ThirdPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "Transform.inl"
#include "Camera.inl"
#include "Core.hpp"

bool ThirdPersonCameraControlSystem::assertEntity(Entity* entity)
{
    camera = entity->getComponentPtr<Camera>();
    if(camera == nullptr)
    {
        return false;
    }

    transform = entity->getComponentPtr<Transform>();
    if(transform == nullptr)
    {
        return false;
    }

    return (camera->isMain && camera->control == CameraControl::ThirdPerson);
}

void ThirdPersonCameraControlSystem::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::MOUSE_CURSOR_MOVED:
            {
                if(usingMouse)
                {
                    CursorData cursorData = msg.getValue<CursorData>();
                    yaw += cursorData.xDelta * sensitivity;
                    pitch -= cursorData.yDelta * sensitivity;
                    if (pitch > 89.0f)
                    {
                        pitch = 89.0f;
                    }
                    else if (pitch < -89.0f)
                    {
                        pitch = -89.0f;
                    }
                }
            }
            break;

        case Event::MOUSE_BUTTON_PRESSED:
            switch (msg.getValue<int>())
            {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    glfwSetInputMode(GetCore().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    usingMouse = !usingMouse;
                    break;
            }
            break;

        case Event::MOUSE_BUTTON_RELEASED:
            switch (msg.getValue<int>())
            {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    glfwSetInputMode(GetCore().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    usingMouse = !usingMouse;
                    break;
            }
            break;
    }
}

void ThirdPersonCameraControlSystem::fixedUpdate()
{

}
