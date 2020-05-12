#include "PaddleControlSystem.hpp"
#include "Message.inl"
#include "Entity.hpp"
#include "Components.inc"

bool PaddleControlSystem::assertEntity(Entity* entity)
{
    paddlePtr = entity->getComponentPtr<Paddle>();
    transformPtr = entity->getComponentPtr<Transform>();
    return (paddlePtr != nullptr && transformPtr != nullptr);
}

void PaddleControlSystem::frameUpdate()
{

}

void PaddleControlSystem::receiveMessage(Message msg)
{

}