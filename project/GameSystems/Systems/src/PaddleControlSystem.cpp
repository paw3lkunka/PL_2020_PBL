#include "PaddleControlSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool PaddleControlSystem::assertEntity(Entity* entity)
{
    paddlePtr = entity->getComponentPtr<Paddle>();
    transformPtr = entity->getComponentPtr<Transform>();
    return (paddlePtr != nullptr && transformPtr != nullptr);
}

void PaddleControlSystem::frameUpdate()
{
    paddlePtr->position2D.x = glm::mix<float, float>(paddlePtr->position2D.x, inputRaw.x, 0.1f);
    paddlePtr->position2D.y = glm::mix<float, float>(paddlePtr->position2D.y, inputRaw.y, 0.1f);
    glm::vec3 newPos;
    newPos.x = glm::mix<float, float>(startPosition.x, paddlePtr->maxPos.x, -paddlePtr->position2D.x);
    newPos.y = copysignf( abs(glm::mix<float, float>(startPosition.y, paddlePtr->maxPos.y, -paddlePtr->position2D.x)), paddlePtr->maxPos.y);
    newPos.z = glm::mix<float, float>(startPosition.z, paddlePtr->maxPos.z, paddlePtr->position2D.y);

    transformPtr->getLocalPositionModifiable() = newPos;
}

void PaddleControlSystem::start()
{
    startPosition = transformPtr->getLocalPosition();
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
        break;

    }
}