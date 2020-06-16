#include "PaddleControlSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"
#include "Core.hpp"
#include "GamepadDataStructures.inl"

#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>

bool PaddleControlSystem::assertEntity(Entity* entity)
{
    paddlePtr = entity->getComponentPtr<Paddle>();
    transformPtr = entity->getComponentPtr<Transform>();
    hydroAcceleratorPtr = entity->getComponentPtr<HydroAccelerator>();
    return (paddlePtr != nullptr && transformPtr != nullptr);
}

void PaddleControlSystem::fixedUpdate()
{
    glm::vec2 oldPosition2D = paddlePtr->position2D;
    if(keyboardInput)
    {
        inputRaw = glm::mix(inputRaw, interpolationTarget, 0.15f);
    }
    paddlePtr->position2D = glm::mix(paddlePtr->position2D, inputRaw, 0.1f);

    glm::vec3 newPos;
    newPos.x = glm::mix(startPosition.x, paddlePtr->maxPos.x, -paddlePtr->position2D.x);
    newPos.y = glm::mix(startPosition.z, paddlePtr->maxPos.z, paddlePtr->position2D.y);
    newPos.z = copysignf( abs(glm::mix(startPosition.y, paddlePtr->maxPos.y, -paddlePtr->position2D.x)), paddlePtr->maxPos.y);

    glm::vec3 eulerRot = glm::eulerAngles(startRotation) * 180.0f / glm::pi<float>();
    float frontRot = glm::mix(eulerRot.x, paddlePtr->maxFrontRot, copysignf(paddlePtr->position2D.y * paddlePtr->position2D.x, paddlePtr->position2D.y));
    float sideRot = glm::mix(eulerRot.y, paddlePtr->maxSideRot, -paddlePtr->position2D.x);

    if(hydroAcceleratorPtr != nullptr)
    {
        glm::vec2 tempPos = glm::abs(oldPosition2D - paddlePtr->position2D);
        glm::vec2 tempInput = glm::abs(inputRaw);

        if( tempPos.y < 0.001f || glm::abs(tempInput.x - tempInput.y) < 0.001f ) 
        {
            hydroAcceleratorPtr->velocity = glm::vec3(0.0f);
            hydroAcceleratorPtr->angularVelocity = glm::vec3(0.0f);
        }
        else
        {
            hydroAcceleratorPtr->velocity = (newPos - transformPtr->getLocalPosition()) * hydroAcceleratorPtr->powerUp;
            hydroAcceleratorPtr->angularVelocity = (glm::vec3(frontRot, sideRot, eulerRot.z) - eulerRot);
            hydroAcceleratorPtr->angularVelocity.y *= hydroAcceleratorPtr->handling;
        }
    }

    /*
    if(hydroAcceleratorPtr != nullptr)
    {
        hydroAcceleratorPtr->velocity = (newPos - hydroAcceleratorPtr->lastPos);
        hydroAcceleratorPtr->angularVelocity = (glm::vec3(frontRot, sideRot, eulerRot.z) - hydroAcceleratorPtr->lastRot);

        hydroAcceleratorPtr->lastPos = newPos;
        hydroAcceleratorPtr->lastRot = glm::vec3(frontRot, sideRot, eulerRot.z);
    }
    */

    transformPtr->getLocalPositionModifiable() = newPos;
    transformPtr->getLocalRotationModifiable() = eulerToQuaternion(glm::vec3(frontRot, sideRot, eulerRot.z));
}

void PaddleControlSystem::start()
{
    startPosition = transformPtr->getLocalPosition();
    startRotation = transformPtr->getLocalRotation();
}

void PaddleControlSystem::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::KEY_PRESSED:
        {
            int key = msg.getValue<int>();
            keyboardInput = true;
            switch(key)
            {
                case GLFW_KEY_UP:
                    interpolationTarget.y += 1.0f;
                break;
                case GLFW_KEY_DOWN:
                    interpolationTarget.y -= 1.0f;
                break;
                case GLFW_KEY_LEFT:
                    interpolationTarget.x -= 1.0f;
                break;
                case GLFW_KEY_RIGHT:
                    interpolationTarget.x += 1.0f;
                break;
            }
        }
        break;

        case Event::KEY_RELEASED:
        {
            int key = msg.getValue<int>();
            keyboardInput = true;
            switch(key)
            {
                case GLFW_KEY_UP:
                    interpolationTarget.y += -1.0f;
                break;
                case GLFW_KEY_DOWN:
                    interpolationTarget.y -= -1.0f;
                break;
                case GLFW_KEY_LEFT:
                    interpolationTarget.x -= -1.0f;
                break;
                case GLFW_KEY_RIGHT:
                    interpolationTarget.x += -1.0f;
                break;
            }
        }
        break;

        case Event::GAMEPAD_AXIS_CHANGED:
            auto data = msg.getValue<GamepadAxisData>();
            keyboardInput = false;
            switch(data.axisId)
            {
                case GLFW_GAMEPAD_AXIS_RIGHT_X:
                    inputRaw.x = std::clamp(data.axisState, -sqrtf(2)/ 2.0f, sqrtf(2) / 2.0f) / (sqrtf(2) / 2.0f);
                break;
                case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                    inputRaw.y = std::clamp(data.axisState, -sqrtf(2)/ 2.0f, sqrtf(2) / 2.0f) / (sqrtf(2) / 2.0f) * -1.0f;
                break;
            }
        break;
    }
}