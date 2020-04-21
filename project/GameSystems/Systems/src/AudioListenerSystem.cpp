#include "AudioListenerSystem.hpp"

#include <glm/gtx/quaternion.hpp>

#include "Core.hpp"
#include "Event.hpp"

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
        if(audioListener->getPosition() != transform->getLocalPosition())
        {
            audioListener->getPositionModifiable() = transform->getLocalPosition();
        }

        auto at = static_cast<glm::vec3>(glm::toMat4(transform->getLocalRotation()) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
        if(audioListener->getAt() != at)
        {
            audioListener->getAtModifiable() = at;
        }

        auto up = static_cast<glm::vec3>(glm::toMat4(transform->getLocalRotation()) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
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
