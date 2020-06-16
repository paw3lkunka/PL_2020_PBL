#include "CallbacksModule.hpp"
#include "MessageBus.hpp"
#include "PhysicStructures.inl"
#include "Rigidbody.hpp"
#include "Kayak.hpp"

void CallbacksModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::COLLISION_ENTER:
        {
            if (Kayak::get() != nullptr)
            {
                auto data = msg.getValue<CollisionData>();
                bool valid = false;
                valid |= data.body1->entityPtr == Kayak::get()->entityPtr;
                valid |= data.body2->entityPtr == Kayak::get()->entityPtr;
                if (valid)
                {
                    rocksHit();
                }
            }
        }
            return;
    }
}

void CallbacksModule::rocksHit()
{
    auto kayak = Kayak::get();
    float velocity = glm::length(kayak->entityPtr->getComponentPtr<Rigidbody>()->velocity);
    float base = velocity - kayak->hitDamageTreshold;

    if (base > 0)
    {
        kayak->hp -= kayak->hitDamagefactor * base;

        if (kayak->hp <= 0)
        {
            GetCore().messageBus.sendMessage(Message(Event::HP_0));
        }
    }
}