#include "PhysicEventListener.hpp"
#include "Rigidbody.hpp"

#include "Core.hpp"
#include "MessageBus.hpp"
#include "PhysicStructures.inl"

void PhysicEventListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
{
    int size = callbackData.getNbContactPairs();
    for (int i = 0; i < size; i++)
    {
        auto contactPair = callbackData.getContactPair(i);

        //Addresses of our components is stored in rp3d components as user data pointer.
        Rigidbody* rb1 = reinterpret_cast<Rigidbody*>(contactPair.getBody1()->getUserData());
        Rigidbody* rb2 = reinterpret_cast<Rigidbody*>(contactPair.getBody2()->getUserData());

        CollisionData data = {rb1, rb2};

        switch (contactPair.getEventType())
        {
            case rp3d::CollisionCallback::ContactPair::EventType::ContactStart:
                GetCore().messageBus.sendMessage(Message(Event::COLLSION_ENTER, data));
                break;
            case rp3d::CollisionCallback::ContactPair::EventType::ContactExit:
                GetCore().messageBus.sendMessage(Message(Event::COLLSION_EXIT, data));
                break;
        }
        


    }
}

void PhysicEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
{
    int size = callbackData.getNbOverlappingPairs();
    for (int i = 0; i < size; i++)
    {
        auto overlappingPair = callbackData.getOverlappingPair(i);

        //Addresses of our components is stored in rp3d components as user data pointer.
        Rigidbody* rb1 = reinterpret_cast<Rigidbody*>(overlappingPair.getBody1()->getUserData());
        Rigidbody* rb2 = reinterpret_cast<Rigidbody*>(overlappingPair.getBody2()->getUserData());

        CollisionData data;

        if (overlappingPair.getCollider2()->getIsTrigger())
        {
            data = {rb1, rb2};
        }
        else
        {
            data = {rb2, rb1};
        }

        switch (overlappingPair.getEventType())
        {
            case rp3d::OverlapCallback::OverlapPair::EventType::OverlapStart:
                GetCore().messageBus.sendMessage(Message(Event::TRIGGER_ENTER, data));
                break;
            case rp3d::OverlapCallback::OverlapPair::EventType::OverlapExit:
                GetCore().messageBus.sendMessage(Message(Event::TRIGGER_EXIT, data));
                break;
        }
    }
}