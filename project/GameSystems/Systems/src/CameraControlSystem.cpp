#include "CameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "Transform.inl"
#include "Camera.inl"

bool CameraControlSystem::assertEntity(Entity* entity)
{
    transform = entity->getComponentPtr<Transform>();
    camera = entity->getComponentPtr<Camera>();
    return (camera != nullptr && transform != nullptr && camera->isMain);
}

void CameraControlSystem::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::KEY_PRESSED:

            switch (msg.getValue<int>())
            {
                case GLFW_KEY_W:
                    movementVector.z += -1.0f;
                    break;
                case GLFW_KEY_S:
                    movementVector.z += 1.0f;
                    break;
                case GLFW_KEY_A:
                    movementVector.x += -1.0f;
                    break;
                case GLFW_KEY_D:
                    movementVector.x += 1.0f;
                    break;
                case GLFW_KEY_Q:
                    movementVector.y += -1.0f;
                    break;
                case GLFW_KEY_E:
                    movementVector.y += 1.0f;
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    speed *= 5.0f;
                    break;
                case GLFW_KEY_C:
                    usingMouse = !usingMouse;
                    break;
            }

            break;
        case Event::KEY_RELEASED:
            
            switch (msg.getValue<int>())
            {
                case GLFW_KEY_W:
                    movementVector.z -= -1.0f;
                    break;
                case GLFW_KEY_S:
                    movementVector.z -= 1.0f;
                    break;
                case GLFW_KEY_A:
                    movementVector.x -= -1.0f;
                    break;
                case GLFW_KEY_D:
                    movementVector.x -= 1.0f;
                    break;
                case GLFW_KEY_Q:
                    movementVector.y -= -1.0f;
                    break;
                case GLFW_KEY_E:
                    movementVector.y -= 1.0f;
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    speed /= 5.0f;
                    break;
            }

            break;

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
    }
}

void CameraControlSystem::fixedUpdate()
{
    glm::vec3 direction = glm::vec3(0.0f);
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);
    glm::quat rotation = glm::quatLookAt(direction, up);
    transform->getLocalRotationModifiable() = rotation;

    glm::vec3 move = 
        glm::normalize(glm::cross(direction, up)) * movementVector.x +
        up * movementVector.y +
        direction * -movementVector.z;

    transform->getLocalPositionModifiable() += move * speed;
}