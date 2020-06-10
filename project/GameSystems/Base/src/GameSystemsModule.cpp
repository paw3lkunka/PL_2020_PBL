#include "GameSystemsModule.hpp"

#include "Core.hpp"

#include "Entity.hpp"
#include "MessageBus.hpp"
#include "Message.inl"

GameSystemsModule::GameSystemsModule()
{
}

void GameSystemsModule::receiveMessage(Message msg)
{
    for (IMsgReceiver* receiver : msgReceivers)
    {
        receiver->receiveMessage(msg);
    }
    
}

void GameSystemsModule::addSystem(System* system)
{
    systems.push_back(system);

    auto receiver = dynamic_cast<IMsgReceiver*>(system);

    if(receiver)
    {
        msgReceivers.push_back(receiver);
    }
}

void GameSystemsModule::run(System::UpdateType updateType)
{
    for( auto sys : systems )
    {
        if(GetCore().isGamePaused() && sys->isPausable())
        {
            continue;
        }
        for( Entity& ent : *entities )
        {
            sys->process(&ent, updateType);
        }
    }
}