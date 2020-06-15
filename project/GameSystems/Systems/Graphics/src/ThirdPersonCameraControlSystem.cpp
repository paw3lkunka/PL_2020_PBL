#include "ThirdPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <cmath>
#include <tgmath.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "GamepadDataStructures.inl"

#include "Transform.hh"
#include "Camera.inl"
#include "ThirdPersonCamera.inl"
#include "SphereCollider.hpp"
#include "Rigidbody.hpp"

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

    tpCamera = entity->getComponentPtr<ThirdPersonCamera>();
    if(tpCamera == nullptr)
    {
        return false;
    }

    collider = entity->getComponentPtr<SphereCollider>();

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
                    angleAroundPlayer += cursorData.xDelta * rotationSensitivity;
                    pitch -= cursorData.yDelta * rotationSensitivity;
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

        case Event::MOUSE_SCROLL_MOVED:
            {
                ScrollData scrollData = msg.getValue<ScrollData>();
                zoom -= scrollData.offset;
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
                            angleAroundPlayer = 0.0f;
                            pitch = -70.0f;
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
                        angleAroundPlayer = 0.0f;
                        pitch = -70.0f;
                }
            }
            break;
    }
}

void ThirdPersonCameraControlSystem::fixedUpdate()
{
    rotationSensitivity = tpCamera->rotationSensitivity;
    angleAroundPlayer += gamepadAxis.x;
    angleAroundPlayer = glm::mod(angleAroundPlayer, 360.0f);
    pitch -= gamepadAxis.y;
    
    float& currentDistance = tpCamera->distance;
    float targetDistance = currentDistance + (zoom * tpCamera->zoomSensitivity);
    targetDistance = glm::clamp(targetDistance, tpCamera->minDistance, tpCamera->maxDistance);
    currentDistance = std::lerp(currentDistance, targetDistance, tpCamera->zoomLerp);
    zoom = 0.0f;

    glm::vec3 playerRotRad = glm::eulerAngles( tpCamera->player->getWorldRotation() );
    if(glm::abs(playerRotRad.z) >= glm::pi<float>() / 2.0f) 
    {
        playerRotRad.x += glm::pi<float>();
        playerRotRad.y = glm::pi<float>() - playerRotRad.y;
        playerRotRad.z += glm::pi<float>();
    }
    
    pitch = glm::clamp(pitch, tpCamera->minPitch, tpCamera->maxPitch);
    float pitchRad = glm::radians(pitch);
    float aroundRad = glm::radians(angleAroundPlayer);
    float thetaRad = aroundRad + playerRotRad.y;

    float verticalDistance = currentDistance * glm::cos(pitchRad);
    float horizontalDistance = currentDistance * glm::sin(pitchRad);

    glm::vec3 playerPos = static_cast<glm::vec3>( tpCamera->player->getModelMatrix()[3] );
    playerPos += tpCamera->playerOffset;

    glm::vec3 camOffset(0.0f);
    camOffset.x = -horizontalDistance * glm::sin(thetaRad);
    camOffset.y = verticalDistance;
    camOffset.z = -horizontalDistance * glm::cos(thetaRad);

    /*
    glm::vec3 playerVelocity = tpCamera->playerRigidbody->velocity;
    float playerSpeed = glm::length(playerVelocity);
    if(playerSpeed > tpCamera->maxIgnoredPlayerSpeed)
    {
        float length = glm::length(camOffset);
        glm::vec3 direction = -glm::normalize(playerVelocity);
        direction.y = 0.0f;

        glm::vec3 targetCamOffset = direction * length;
        float targetPitch = -glm::degrees( glm::asin(direction.y) );
        float targetAngleAroundPlayer = glm::degrees( std::atan2(direction.x, direction.z) );
        //camOffset.x = std::lerp(camOffset.x, targetCamOffset.x, 0.1f);
        //camOffset.y = std::lerp(camOffset.y, targetCamOffset.y, 0.1f);
        //camOffset.z = std::lerp(camOffset.z, targetCamOffset.z, 0.1f);
        pitch = std::lerp(pitch, targetPitch, 0.001f);
        angleAroundPlayer = std::lerp(angleAroundPlayer, targetAngleAroundPlayer, 0.001f);
    }
    */
    
    glm::vec3 targetPosition = playerPos + camOffset;
    glm::vec3& currentPosition = transform->getLocalPositionModifiable();
    currentPosition.x = std::lerp(currentPosition.x, targetPosition.x, tpCamera->moveLerp);
    currentPosition.y = std::lerp(currentPosition.y, targetPosition.y, tpCamera->moveLerp);
    currentPosition.z = std::lerp(currentPosition.z, targetPosition.z, tpCamera->moveLerp);

    glm::quat targetRotation = glm::quatLookAt( -glm::normalize(camOffset), glm::vec3(0.0f, 1.0f, 0.0f) );
    glm::quat& currentRotation = transform->getLocalRotationModifiable();
    currentRotation = glm::slerp(currentRotation, targetRotation, tpCamera->rotationLerp);
}
