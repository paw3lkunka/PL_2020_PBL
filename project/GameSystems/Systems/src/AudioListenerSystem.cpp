#include "AudioListenerSystem.hpp"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vec_swizzle.hpp>

#include "Core.hpp"
#include "Event.hpp"
#include "Message.inl"

#include "Entity.hpp"

#include "Transform.inl"
#include "Rigidbody.inl"
#include "AudioListener.inl"


bool AudioListenerSystem::assertEntity(Entity* entity)
{
    audioListener = entity->getComponentPtr<AudioListener>();
    if(audioListener == nullptr)
    {
        return false;
    }
    
    transform = entity->getComponentPtr<Transform>();
    rigidbody = entity->getComponentPtr<Rigidbody>();
    return true;
}

void AudioListenerSystem::fixedUpdate()
{
    if(audioListener->context == nullptr)
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_LISTENER_INIT, audioListener) );
    }

    if(transform)
    {
        auto position = glm::xyz(transform->getModelMatrix()[3]);
        if(audioListener->getPosition() != position)
        {
            audioListener->getPositionModifiable() = position;
        }

        auto at = (-1.0f) * glm::normalize( glm::xyz(transform->getModelMatrix()[2]) );
        if(audioListener->getAt() != at)
        {
            audioListener->getAtModifiable() = at;
        }

        auto up = glm::normalize( glm::xyz(transform->getModelMatrix()[1]) );
        if(audioListener->getUp() != up)
        {
            audioListener->getUpModifiable() = up;
        }
    }

    if(rigidbody)
    {
        if(audioListener->getVelocity() != rigidbody->velocity)
        {
            audioListener->getVelocityModifiable() = rigidbody->velocity;
        }
    }
    
    if(audioListener->getDirty())
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_LISTENER_UPDATE, audioListener) );
    }
}
