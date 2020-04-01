#include "GameSystemsModule.hpp"

#include "Entity.hpp"
#include "System.hpp"
#include "MessageBus.hpp"
#include "Message.inl"

GameSystemsModule::GameSystemsModule(MessageBus* messageBus)
{
    this->messageBus = messageBus;
}

void GameSystemsModule::ReceiveMessage(Message msg)
{

}

void GameSystemsModule::AddEntity(Entity* entity)
{
    entities.push_back(entity);
}

void GameSystemsModule::AddSystem(System* system)
{
    systems.push_back(system);
}

void GameSystemsModule::Run()
{
    for( auto sys : systems )
    {
        for( auto ent : entities )
        {
            sys->Process(ent);
        }
    }
}