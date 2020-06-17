#include "CallbacksModule.hpp"
#include "MessageBus.hpp"
#include "PhysicStructures.inl"
#include "EnemyDataStructures.inl"
#include "ECS.inc"


void CallbacksModule::init(const char* hpBarName, float bulletDamage)
{
    Entity* e = GetCore().objectModule.getEntityPtrByName(hpBarName);
    if (e)
    {
        healthbarPtr = e->getComponentPtr<ProgressBar>();
    }

    this->bulletDamage = bulletDamage;
}

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

        case Event::PLAYER_ATTACKED:
        {
            if (msg.getValue<AttackData>().success)
            {
                bulletHit();
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

void CallbacksModule::bulletHit()
{
    auto kayak = Kayak::get();
    
    kayak->hp -= bulletDamage;
    
    if (healthbarPtr)
    {
        healthbarPtr->percentage = kayak->hp / kayak->maxHp;
    }

    if (kayak->hp <= 0)
    {
        GetCore().messageBus.sendMessage(Message(Event::HP_0));
    }
}