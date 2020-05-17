#include "PaddleControlSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"
#include "Core.hpp"
#include "GamepadDataStructures.inl"

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>

bool PaddleControlSystem::assertEntity(Entity* entity)
{
    paddlePtr = entity->getComponentPtr<Paddle>();
    transformPtr = entity->getComponentPtr<Transform>();
    return (paddlePtr != nullptr && transformPtr != nullptr);
}

void PaddleControlSystem::frameUpdate()
{
    paddlePtr->position2D = glm::mix(paddlePtr->position2D, inputRaw, 0.1f);

    glm::vec3 newPos;
    newPos.x = glm::mix(startPosition.x, paddlePtr->maxPos.x, -paddlePtr->position2D.x);
    newPos.y = glm::mix(startPosition.z, paddlePtr->maxPos.z, paddlePtr->position2D.y);
    newPos.z = copysignf( abs(glm::mix(startPosition.y, paddlePtr->maxPos.y, -paddlePtr->position2D.x)), paddlePtr->maxPos.y);

    glm::vec3 eulerRot = glm::eulerAngles(startRotation) * 180.0f / glm::pi<float>();
    float frontRot = glm::mix(eulerRot.x, paddlePtr->maxFrontRot, copysignf(paddlePtr->position2D.y * paddlePtr->position2D.x, paddlePtr->position2D.y));
    float sideRot = glm::mix(eulerRot.y, paddlePtr->maxSideRot, -paddlePtr->position2D.x);

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
            switch(key)
            {
                case GLFW_KEY_UP:
                    inputRaw.y += 1.0f;
                break;
                case GLFW_KEY_DOWN:
                    inputRaw.y -= 1.0f;
                break;
                case GLFW_KEY_LEFT:
                    inputRaw.x -= 1.0f;
                break;
                case GLFW_KEY_RIGHT:
                    inputRaw.x += 1.0f;
                break;
            }
        }
        break;

        case Event::KEY_RELEASED:
        {
            int key = msg.getValue<int>();
            switch(key)
            {
                case GLFW_KEY_UP:
                    inputRaw.y += -1.0f;
                break;
                case GLFW_KEY_DOWN:
                    inputRaw.y -= -1.0f;
                break;
                case GLFW_KEY_LEFT:
                    inputRaw.x -= -1.0f;
                break;
                case GLFW_KEY_RIGHT:
                    inputRaw.x += -1.0f;
                break;
            }
        }
        break;

        case Event::GAMEPAD_AXIS_CHANGED:
        auto data = msg.getValue<GamepadAxisData>();
        switch(data.axisId)
        {
            case GLFW_GAMEPAD_AXIS_RIGHT_X:
                inputRaw.x = data.axisState;
            break;
            case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                inputRaw.y = data.axisState * -1.0f;
            break;
        }
        break;
    }
}