#include "FirstPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "GamepadDataStructures.inl"

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

        case Event::GAMEPAD_AXIS_CHANGED:
            {
                GamepadAxisData data = msg.getValue<GamepadAxisData>();
                switch(data.axisId)
                {
                    case GLFW_GAMEPAD_AXIS_LEFT_X:
                        if(glm::abs(data.axisState) > 0.25f)
                        {
                            gamepadAxis.x = data.axisState;
                        }
                        else
                        {
                            gamepadAxis.x = 0.0f;
                        }
                        break;

                    case GLFW_GAMEPAD_AXIS_LEFT_Y:
                        if(glm::abs(data.axisState) > 0.25f)
                        {
                            gamepadAxis.y = data.axisState;
                        }
                        else
                        {
                            gamepadAxis.y = 0.0f;
                        }
                        break;
                }
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

        case Event::GAMEPAD_BUTTON_PRESSED:
            {
                GamepadButtonData data = msg.getValue<GamepadButtonData>();
                switch(data.buttonId)
                {
                    case GLFW_GAMEPAD_BUTTON_BACK:
                        yaw = -90.0f;
                        pitch = 0.0f;
                }
            }
            break;
    }
}

void FirstPersonCameraControlSystem::fixedUpdate()
{
    yaw += gamepadAxis.x;
    pitch -= gamepadAxis.y;

    inputSensitivity = fpCamera->rotationSensitivity;
    yaw = glm::clamp(yaw, fpCamera->minYaw, fpCamera-> maxYaw);
    pitch = glm::clamp(pitch, fpCamera->minPitch, fpCamera->maxPitch);

    glm::vec3 targetPosition = static_cast<glm::vec3>( glm::translate(fpCamera->player->getModelMatrix(), fpCamera->headOffset)[3] );
    glm::vec3& currentPosition = transform->getLocalPositionModifiable();
    currentPosition.x = std::lerp(currentPosition.x, targetPosition.x, fpCamera->moveLerp);
    currentPosition.y = std::lerp(currentPosition.y, targetPosition.y, fpCamera->moveLerp);
    currentPosition.z = std::lerp(currentPosition.z, targetPosition.z, fpCamera->moveLerp);

    glm::vec3 playerRotRad = glm::eulerAngles( fpCamera->player->getWorldRotation() );
    if(glm::abs(playerRotRad.z) >= glm::pi<float>() / 2.0f) 
    {
        playerRotRad.x += glm::pi<float>();
        playerRotRad.y = glm::pi<float>() - playerRotRad.y;
        playerRotRad.z += glm::pi<float>();
    }

    float yawRad = glm::radians(yaw) - playerRotRad.y;
    float pitchRad = glm::radians(pitch);

    glm::vec3 direction = glm::vec3(0.0f);
    direction.x = cos(yawRad) * cos(pitchRad);
    direction.y = sin(pitchRad);
    direction.z = sin(yawRad) * cos(pitchRad);
    direction = glm::normalize(direction);
    glm::quat targetRotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat& currentRotation = transform->getLocalRotationModifiable();
    currentRotation = glm::slerp(currentRotation, targetRotation, fpCamera->rotationLerp);
}
