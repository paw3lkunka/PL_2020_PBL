#include "HideoutSystem.hpp"

#include "CollisionDataStructures.inl"
#include "Components.inc"
#include "Message.inl"
#include "Core.hpp"


bool HideoutSystem::assertEntity(Entity* entity)
{
    return kayakPtr && (hideoutPtr = entity->getComponentPtr<Hideout>());
}

void HideoutSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::TRIGGER_ENTER || msg.getEvent() == Event::TRIGGER_EXIT)
    {
        if (kayakPtr && msg.getValue<CollisionData>().cause->entityPtr == kayakPtr->entityPtr)
        {
            messages.push_back(msg);
        }
    }
}

void HideoutSystem::init()
{
    for (Entity& e : *GetCore().gameSystemsModule.entities)
    {
        kayakPtr = e.getComponentPtr<Kayak>();

        if (kayakPtr)
        {
            return;
        }
    }
    std::cerr << "Kayak Component not found! Hideout system wouldn't be working!\n";
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
                kayakPtr->isHidden++;
                break;
            case Event::TRIGGER_EXIT:
                kayakPtr->isHidden--;
                break;
            }
        }
    }
}