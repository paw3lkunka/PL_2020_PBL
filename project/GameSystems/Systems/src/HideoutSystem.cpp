#include "HideoutSystem.hpp"

#include "CollisionDataStructures.inl"
#include "Components.inc"
#include "Message.inl"
#include "Core.hpp"


bool HideoutSystem::assertEntity(Entity* entity)
{
    return hideoutPtr = entity->getComponentPtr<Hideout>();
}

void HideoutSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::TRIGGER_ENTER || msg.getEvent() == Event::TRIGGER_EXIT)
    {
        if (msg.getValue<CollisionData>().cause->entityPtr == playerPtr->entityPtr)
        {
            messages.push_back(msg);
        }
    }
}

void HideoutSystem::init()
{
    for (Entity& e : *GetCore().gameSystemsModule.entities)
    {
        playerPtr = e.getComponentPtr<Kayak>();

        if (playerPtr)
        {
            return;
        }
    }
    std::cerr << "Kayak Component not found!";
    exit(-1);
}

void HideoutSystem::clean()
{
    messages.clear();
}

void HideoutSystem::fixedUpdate()
{
    for (Message& msg : messages)
    {
        if (msg.getValue<CollisionData>().target->entityPtr == hideoutPtr->entityPtr)
        {
            switch (msg.getEvent())
            {
            case Event::TRIGGER_ENTER:
                playerPtr->isHidden++;
                break;
            case Event::TRIGGER_EXIT:
                playerPtr->isHidden--;
                break;
            }
        }
    }
}