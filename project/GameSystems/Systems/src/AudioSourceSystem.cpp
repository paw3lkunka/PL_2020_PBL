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
    if(audioSource->dirty & (1 << 20))
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_LISTENERS, audioSource) );
    }

    if(transform)
    {
        if(audioSource->position != transform->getLocalPosition())
        {
            audioSource->position = transform->getLocalPosition();
            audioSource->dirty |= (1 << 0);
        }
    }

    if(rigidbody)
    {
        if(audioSource->velocity != rigidbody->velocity)
        {
            audioSource->velocity = rigidbody->velocity;
            audioSource->dirty |= (1 << 1);
        }
    }
    
    if(audioSource->dirty)
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_ATTRIBUTES, audioSource) );
    }
}
