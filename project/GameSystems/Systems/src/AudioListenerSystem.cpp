#include "AudioListenerSystem.hpp"

#include <glm/gtx/quaternion.hpp>

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
        glm::vec3 scale; // unused
        glm::quat orientation; // unused
        glm::vec3 translation;
        glm::vec3 skew; // unused
        glm::vec4 perspective; // unused
        glm::decompose(transform->localToWorldMatrix, scale, orientation, translation, skew, perspective);

        if(audioListener->getPosition() != translation)
        {
            audioListener->getPositionModifiable() = translation;
        }

        auto at = glm::normalize( static_cast<glm::vec3>( glm::toMat4(orientation) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f) ) );
        if(audioListener->getAt() != at)
        {
            audioListener->getAtModifiable() = at;
        }

        auto up = glm::normalize( static_cast<glm::vec3>( glm::toMat4(orientation) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) ) );
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
