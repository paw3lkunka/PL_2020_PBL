#include "FirstPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"

#include "Transform.hh"
#include "Camera.inl"
#include "FirstPersonCamera.inl"

bool FirstPersonCameraControlSystem::assertEntity(Entity* entity)
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

    fpCamera = entity->getComponentPtr<FirstPersonCamera>();
    if(fpCamera == nullptr)
    {
        return false;
    }

    return (camera->isMain && camera->control == CameraControl::FirstPerson);
}

void FirstPersonCameraControlSystem::receiveMessage(Message msg)
{
switch (msg.getEvent())
    {
        case Event::MOUSE_CURSOR_MOVED:
            {
                if(usingMouse)
                {
                    CursorData cursorData = msg.getValue<CursorData>();
                    yaw += cursorData.xDelta * inputSensitivity;
                    pitch -= cursorData.yDelta * inputSensitivity;
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

        case Event::KEY_PRESSED:
            {
                switch (msg.getValue<int>())
                {
                    case GLFW_KEY_F1:
                    case GLFW_KEY_C:
                    case GLFW_KEY_V:
                        {
                            yaw = -90.0f;
                            pitch = 0.0f;
                        }
                        break;
                }
            }
            break;
    }
}

void FirstPersonCameraControlSystem::fixedUpdate()
{
    inputSensitivity = fpCamera->rotationSensitivity;
    yaw = glm::clamp(yaw, fpCamera->minYaw, fpCamera-> maxYaw);
    pitch = glm::clamp(pitch, fpCamera->minPitch, fpCamera->maxPitch);

    glm::vec3 targetPosition = static_cast<glm::vec3>( fpCamera->player->getModelMatrix()[3] ) + fpCamera->headOffset;
    glm::vec3& currentPosition = transform->getLocalPositionModifiable();
    currentPosition.x = std::lerp(currentPosition.x, targetPosition.x, fpCamera->moveLerp);
    currentPosition.y = std::lerp(currentPosition.y, targetPosition.y, fpCamera->moveLerp);
    currentPosition.z = std::lerp(currentPosition.z, targetPosition.z, fpCamera->moveLerp);

    glm::vec3 direction = glm::vec3(0.0f);
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);
    glm::quat targetRotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat& currentRotation = transform->getLocalRotationModifiable();
    currentRotation = glm::slerp(currentRotation, targetRotation, fpCamera->rotationLerp);
}
