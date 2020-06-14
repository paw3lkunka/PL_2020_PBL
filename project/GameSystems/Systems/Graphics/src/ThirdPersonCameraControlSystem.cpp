#include "ThirdPersonCameraControlSystem.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"
#include "GamepadDataStructures.inl"

#include "Transform.hh"
#include "Camera.inl"
#include "ThirdPersonCamera.inl"
#include "SphereCollider.hpp"

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
                    angleAroundPlayer = glm::mod(angleAroundPlayer, 360.0f);
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
            GamepadAxisData data = msg.getValue<GamepadAxisData>();
            switch(data.axisId)
            {
                case GLFW_GAMEPAD_AXIS_RIGHT_X:
                    angleAroundPlayer += data.axisState * rotationSensitivity;
                    angleAroundPlayer = glm::mod(angleAroundPlayer, 360.0f);
                    break;

                case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                    pitch -= data.axisState * rotationSensitivity;
                    break;
            }
            break;
    }
}

void ThirdPersonCameraControlSystem::fixedUpdate()
{
    rotationSensitivity = tpCamera->rotationSensitivity;
    
    float& currentDistance = tpCamera->distance;
    float targetDistance = currentDistance + (zoom * tpCamera->zoomSensitivity);
    targetDistance = glm::clamp(targetDistance, tpCamera->minDistance, tpCamera->maxDistance);
    currentDistance = std::lerp(currentDistance, targetDistance, tpCamera->zoomLerp);
    zoom = 0.0f;

    glm::vec3 playerRotDeg = glm::eulerAngles( tpCamera->player->getLocalRotation() );

    pitch = glm::clamp(pitch, tpCamera->minPitch, tpCamera->maxPitch);
    float pitchRad = glm::radians(pitch);
    float aroundRad = glm::radians(angleAroundPlayer);
    float thetaRad = aroundRad + glm::radians(playerRotDeg.y);

    float verticalDistance = currentDistance * glm::cos(pitchRad);
    float horizontalDistance = currentDistance * glm::sin(pitchRad);

    glm::vec3 playerPos = static_cast<glm::vec3>( tpCamera->player->getModelMatrix()[3] );
    playerPos += tpCamera->playerOffset;

    glm::vec3 camOffset(0.0f);
    camOffset.x = -horizontalDistance * glm::sin(thetaRad);
    camOffset.y = verticalDistance;
    camOffset.z = -horizontalDistance * glm::cos(thetaRad);
    
    glm::vec3& currentPosition = transform->getLocalPositionModifiable();
    glm::vec3 targetPosition = playerPos + camOffset;
    currentPosition.x = std::lerp(currentPosition.x, targetPosition.x, tpCamera->moveLerp);
    currentPosition.y = std::lerp(currentPosition.y, targetPosition.y, tpCamera->moveLerp);
    currentPosition.z = std::lerp(currentPosition.z, targetPosition.z, tpCamera->moveLerp);

    //yaw = 180.0f - (playerRotDeg.y + angleAroundPlayer);

    glm::quat& currentRotation = transform->getLocalRotationModifiable();
    glm::quat rotation = glm::quatLookAt( -glm::normalize(camOffset), glm::vec3(0.0f, 1.0f, 0.0f) );
    currentRotation = glm::slerp(currentRotation, rotation, tpCamera->rotationLerp);
}
