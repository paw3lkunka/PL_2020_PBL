#include "AudioSourceSystem.hpp"

#include "Core.hpp"
#include "Event.hpp"
#include "Message.inl"
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
        auto position = static_cast<glm::vec3>(transform->modelMatrix[3]);
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
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_UPDATE_ATTRIBUTES, audioSource) );
    }
}
