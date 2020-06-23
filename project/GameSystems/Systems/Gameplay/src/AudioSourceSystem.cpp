#include "AudioSourceSystem.hpp"

#include "Core.hpp"
#include "Event.inl"
#include "Message.inl"
#include "Entity.hpp"

#include <glm/gtx/vec_swizzle.hpp>

#include "AudioSource.inl"
#include "Transform.hh"
#include "Rigidbody.hpp"


bool AudioSourceSystem::assertEntity(Entity* entity)
{
    audioSource = entity->getComponentPtr<AudioSource>();
    if(audioSource == nullptr)
    {
        return false;
    }
    
    transform = entity->getComponentPtr<Transform>();
    rigidbody = entity->getComponentPtr<Rigidbody>();
    return true;
}

void AudioSourceSystem::start()
{
    GetCore().messageBus.sendMessage(Message(Event::AUDIO_SOURCE_INIT, audioSource));
}

void AudioSourceSystem::frameUpdate()
{
    // if(audioSource->getDirty() & (1 << 20))
    // {
    //     GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_LISTENERS, audioSource) );
    // }

    if(transform)
    {
        auto position = glm::xyz(transform->getModelMatrix()[3]);
        if(audioSource->getPosition() != position)
        {
            audioSource->getPositionModifiable() = position;
        }
    }

    if(rigidbody)
    {
        if(audioSource->getVelocity() != rigidbody->velocity)
        {
            audioSource->getVelocityModifiable() = rigidbody->velocity;
        }
    }
    
    if(audioSource->getDirty())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE, audioSource) );
    }
}
