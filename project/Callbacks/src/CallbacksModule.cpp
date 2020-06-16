#include "CallbacksModule.hpp"
#include "MessageBus.hpp"
#include "PhysicStructures.inl"
#include "ECS.inc"


void CallbacksModule::init(const char* hpBarName)
{
    Entity* e = GetCore().objectModule.getEntityPtrByName("Health_Bar");
    if (e)
    {
        healthbarPtr = e->getComponentPtr<ProgressBar>();
    }
}

void CallbacksModule::receiveMessage(Message msg)
{
    switch (msg.getEvent())
    {
        case Event::COLLISION_ENTER:
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
        kayak->hp = std::max(kayak->hp, 0.0f);

        if (healthbarPtr)
        {
            healthbarPtr->percentage = kayak->hp / kayak->maxHp;
        }

        if (kayak->hp <= 0)
        {
            GetCore().messageBus.sendMessage(Message(Event::HP_0));
        }
    }
}