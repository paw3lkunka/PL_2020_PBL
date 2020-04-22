#include "AudioSourceSystem.hpp"

#include "Core.hpp"
#include "Event.hpp"

#include "Entity.hpp"

#include "AudioSource.inl"
#include "Transform.inl"
#include "Rigidbody.inl"


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

void AudioSourceSystem::fixedUpdate()
{
    if(audioSource->getDirty() & (1 << 20))
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_LISTENERS, audioSource) );
    }

    if(transform)
    {
        if(audioSource->getPosition() != transform->getLocalPosition())
        {
            audioSource->getPositionModifiable() = transform->getLocalPosition();
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
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_ATTRIBUTES, audioSource) );
    }
}
