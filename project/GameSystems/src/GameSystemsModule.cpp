#include "GameSystemsModule.hpp"

#include "Entity.hpp"
#include "System.hpp"
#include "MessageBus.hpp"
#include "Message.inl"

GameSystemsModule::GameSystemsModule()
{
}

void GameSystemsModule::receiveMessage(Message msg)
{

}

void GameSystemsModule::addEntity(Entity* entity)
{
    entities.push_back(entity);
}

void GameSystemsModule::addSystem(System* system)
{
    systems.push_back(system);
}

void GameSystemsModule::run()
{
    for( auto sys : systems )
    {
        for( auto ent : entities )
        {
            sys->process(ent);
        }
    }
}