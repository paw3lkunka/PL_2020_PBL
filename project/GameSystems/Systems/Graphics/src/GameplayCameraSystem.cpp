#include "GameplayCameraSystem.hpp"

#include "Core.hpp"
#include "Entity.hpp"
#include "Message.inl"
#include "MouseDataStructures.inl"

#include "GameplayCamera.inl"
#include "GamepadDataStructures.inl"
#include "Transform.inl"

bool GameplayCameraSystem::assertEntity(Entity* entity)
{
    gameplayCamera = entity->getComponentPtr<GameplayCamera>();
    if(gameplayCamera == nullptr)
    {
        return false;
    }

    transform = entity->getComponentPtr<Transform>();
    if(transform == nullptr)
    {
        return false;
    }

    return true;
}

void GameplayCameraSystem::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::MOUSE_CURSOR_MOVED:
            {
                if(usingMouse)
                {
                    CursorData cursorData = msg.getValue<CursorData>();
                    mouseInput.x += cursorData.xDelta;
                    mouseInput.y -= cursorData.yDelta;
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
                scrollInput -= scrollData.offset;
            }
            break;

        case Event::GAMEPAD_AXIS_CHANGED:
            GamepadAxisData data = msg.getValue<GamepadAxisData>();
            switch(data.axisId)
            {
                case GLFW_GAMEPAD_AXIS_RIGHT_X:
                    mouseInput.x += data.axisState;
                    break;

                case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                    mouseInput.y -= data.axisState;
                    break;
            }
            break;
    }
}

void GameplayCameraSystem::fixedUpdate()
{
    // Move third person camera "ARM"
    //glm::vec4 targetWorldPos = gameplayCamera->playerTransform->getModelMatrix()[3] + glm::vec4(gameplayCamera->targetOffset, 0.0f);
    //transform->getLocalPositionModifiable() = transform->getToModelMatrix() * targetWorldPos;

    // Apply "ARM" rotation on its transform
    //glm::mat4 temp = transform->getModelMatrix();
    //armRotation.z += mouseInput.x * gameplayCamera->inputSensitivity * Core::FIXED_TIME_STEP_F;
    //armRotation.z = glm::clamp(armRotation.x, gameplayCamera->lowerClampAngle, gameplayCamera->upperClampAngle);
    //armRotation.y += mouseInput.y * gameplayCamera->inputSensitivity * Core::FIXED_TIME_STEP_F;
    //temp = glm::rotate( temp, glm::radians(mouseInput.x * gameplayCamera->inputSensitivity * Core::FIXED_TIME_STEP_F), glm::vec3(0.0f, 1.0f, 0.0f) );
    //temp = glm::rotate( temp, glm::radians(mouseInput.y * gameplayCamera->inputSensitivity * Core::FIXED_TIME_STEP_F), glm::vec3(1.0f, 0.0f, 0.0f) );
    
    //transform->getLocalRotationModifiable() = glm::quat(temp);
    //mouseInput = { 0.0f, 0.0f };

    // Move the CAMERA on arm itself
    //gameplayCamera->distanceToPlayer += scrollInput * gameplayCamera->inputSensitivity * Core::FIXED_TIME_STEP_F;
    //gameplayCamera->distanceToPlayer = glm::clamp(gameplayCamera->distanceToPlayer, gameplayCamera->lowerClampDistance, gameplayCamera->upperClampDistance);
    //glm::vec3 cameraOffset(0.0f, 0.0f, gameplayCamera->distanceToPlayer);
    //gameplayCamera->cameraTransform->getLocalPositionModifiable() = cameraOffset;
    //scrollInput = 0.0f;
    
    // Rotate the CAMERA in player direction
    //glm::quat cameraTarget = glm::quatLookAt( static_cast<glm::vec3>(targetWorldPos), glm::vec3(0.0f, 1.0f, 0.0f) );
    //glm::quat& cameraRotation = gameplayCamera->cameraTransform->getLocalRotationModifiable();
    //cameraRotation = glm::slerp(cameraRotation, cameraTarget, gameplayCamera->lerpParameter);
}
