#include "HideoutSystem.hpp"

#include "CollisionDataStructures.inl"
#include "Components.inc"
#include "Message.inl"
#include "Core.hpp"


bool HideoutSystem::assertEntity(Entity* entity)
{
    return Kayak::get() && (hideoutPtr = entity->getComponentPtr<Hideout>());
}

void HideoutSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::TRIGGER_ENTER || msg.getEvent() == Event::TRIGGER_EXIT)
    {
        if (Kayak::get() && msg.getValue<CollisionData>().cause->entityPtr == Kayak::get()->entityPtr)
        {
            messages.push_back(msg);
        }
    }
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
                Kayak::get()->isHidden++;
                break;
            case Event::TRIGGER_EXIT:
                Kayak::get()->isHidden--;
                break;
            }
        }
    }
}