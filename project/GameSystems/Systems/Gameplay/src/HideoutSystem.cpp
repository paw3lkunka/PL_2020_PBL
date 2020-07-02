#include "HideoutSystem.hpp"

#include "Components.inc"
#include "Message.inl"
#include "Core.hpp"


bool HideoutSystem::assertEntity(Entity* entity)
{
    audioSource = entity->getComponentPtr<AudioSource>();
    return Kayak::get() && (hideoutPtr = entity->getComponentPtr<Hideout>());
}

void HideoutSystem::receiveMessage(Message msg)
{
    if (msg.getEvent() == Event::TRIGGER_ENTER || msg.getEvent() == Event::TRIGGER_EXIT)
    {
        if (Kayak::get() && msg.getValue<TriggerData>().causeBody->entityPtr == Kayak::get()->entityPtr)
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
        if (msg.getValue<TriggerData>().triggerBody->entityPtr == hideoutPtr->entityPtr)
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

    if(audioSource)
    {
        auto kayakRb = Kayak::get()->entityPtr->getComponentPtr<Rigidbody>();
        audioSource->getGainModifiable() = glm::length(kayakRb->velocity) / 25.0f;
    }

}