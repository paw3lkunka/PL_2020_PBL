#ifndef AUDIOLISTENER_HPP_
#define AUDIOLISTENER_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/vec3.hpp>

#include "Component.inl"

struct AudioListener : public Component
{
    AudioListener() = default;
    
    virtual ~AudioListener() = default;

    /**
     * @brief Pointer to Listener's context
     * 
     * READONLY: Listener is unique among its context, it should only be set by AudioModule itself while responding to AUDIO_LISTENER_INIT event.
     */
    ALCcontext* context = nullptr;

    /**
     * @brief READ: Dirty flag indicating which Listener's attributes must be updated on device level
     * Dirty flag bits present as follows:
     * 
     * 1 << 0 flag: position
     * 1 << 1 flag: velocity
     * 1 << 2 flag: gain
     * 1 << 3 flag: orientation ("at" and "up" vectors)
     * 1 << 4 flag: isCurrent (currently processed context)
     * 
     * @returns Value of dirty flag
     */
    inline ALubyte getDirty() { return dirty; }

    /**
     * @brief WRITE: Dirty flag indicating which Listener's attributes must be updated on device level
     * Dirty flag bits present as follows:
     * 
     * 1 << 0 flag: position
     * 1 << 1 flag: velocity
     * 1 << 2 flag: gain
     * 1 << 3 flag: orientation ("at" and "up" vectors)
     * 1 << 4 flag: isCurrent (currently processed context)
     * 
     * @returns Reference to dirty flag
     */
    inline ALubyte& getDirtyModifiable() { return dirty; }

    /**
     * @brief READ: Listener's Position vector in world space
     * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
     * 
     * @returns Value of position vector
     */
    inline glm::vec3 getPosition() { return position; }

    /**
     * @brief WRITE: Listener's Position vector in world space
     * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
     * 
     * @returns Reference to position vector
     */
    inline glm::vec3& getPositionModifiable() { dirty |= (1 << 0); return position; }

    /**
     * @brief READ: Listener's Velocity vector
     * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
     * 
     * @returns Value of velocity vector
     */
    inline glm::vec3 getVelocity() { return velocity; }
    
    /**
     * @brief WRITE: Listener's Velocity vector
     * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
     * 
     * @returns Reference to velocity vector
     */
    inline glm::vec3& getVelocityModifiable() { dirty |= (1 << 1); return velocity; }

    /**
     * @brief READ: Listener's Gain value
     * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
     * 
     * @returns Value of gain value
     */
    inline ALfloat getGain() { return gain; }

    /**
     * @brief WRITE: Listener's Gain value
     * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
     * 
     * @returns Reference to gain value
     */
    inline ALfloat& getGainModifiable() { dirty |= (1 << 2); return gain; }

    /**
     * @brief READ: Listener's Orientation "at" vector
     * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
     * 
     * at - vector indicating the direction listener is facing 
     * (normal to listener's nose peak)
     * 
     * up - vector indicating the direction of what listener recognise as "up" 
     * (normal to listener's head peak)
     * 
     * @returns Value of at vector
     */
    inline glm::vec3 getAt() { return at; }

    /**
     * @brief WRITE: Listener's Orientation "at" vector
     * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
     * 
     * at - vector indicating the direction listener is facing 
     * (normal to listener's nose peak)
     * 
     * up - vector indicating the direction of what listener recognise as "up" 
     * (normal to listener's head peak)
     * 
     * @returns Reference to at vector
     */
    inline glm::vec3& getAtModifiable() { dirty |= (1 << 3); return at; }

    /**
     * @brief READ: Listener's Orientation "up" vector
     * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
     * 
     * at - vector indicating the direction listener is facing 
     * (normal to listener's nose peak)
     * 
     * up - vector indicating the direction of what listener recognise as "up" 
     * (normal to listener's head peak)
     * 
     * @returns Value of up vector
     */
    inline glm::vec3 getUp() { return up; }

    /**
     * @brief WRITE: Listener's Orientation "up" vector
     * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
     * 
     * at - vector indicating the direction listener is facing 
     * (normal to listener's nose peak)
     * 
     * up - vector indicating the direction of what listener recognise as "up" 
     * (normal to listener's head peak)
     * 
     * @returns Reference to up vector
     */
    inline glm::vec3& getUpModifiable() { dirty |= (1 << 3); return up; }

    /**
     * @brief READ: Flag saying if AudioListener's context is currently processed one.
     * 
     * Set it to TRUE if you want to switch currently processed context.
     * 
     * If it is currently processed and you set it to FALSE, none context will be processed.
     * 
     * @returns Value of isCurrent flag
     */
    inline ALboolean getIsCurrent() { return isCurrent; }

    /**
     * @brief WRITE: Flag saying if AudioListener's context is currently processed one.
     * 
     * Set it to TRUE if you want to switch currently processed context.
     * 
     * If it is currently processed and you set it to FALSE, none context will be processed.
     * 
     * @returns Reference to isCurrent flag
     */
    inline ALboolean& getIsCurrentModifiable() { dirty |= (1 << 4); return isCurrent; }

    private:

        ALubyte dirty = 0;

        glm::vec3 position = {0.0f, 0.0f, 0.0f};

        glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
        
        ALfloat gain = 1.0f;
        
        glm::vec3 at  = {0.0f, 0.0f, -1.0f};
        glm::vec3 up = {0.0f, 1.0f, 0.0f};
        
        ALboolean isCurrent = false;
};

#endif /* !AUDIOLISTENER_HPP_ */
