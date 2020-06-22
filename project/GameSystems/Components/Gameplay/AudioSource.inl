#ifndef AUDIOSOURCE_HPP_
#define AUDIOSOURCE_HPP_

#include <AL/al.h>
#include <glm/vec3.hpp>
#include <vector>
#include <unordered_map>

#include "Component.hpp"

//TODO documentation
struct AudioSource : public Component
{
    AudioSource() = default;
    virtual ~AudioSource() = default;

    /// READONLY: Name (id) of the source in OpenAL
    ALuint name;

    /// @brief Audio clip only to be set in scene file
    std::string audioClip;

    /**
     * @brief Dirty flag indicating which Source's attributes must be updated on device level
     * Dirty flag bits present as follows:
     * 
     * 1 << 0 flag: position
     * 1 << 1 flag: velocity
     * 1 << 2 flag: gain
     * 1 << 3 flag: isRelativeToListener
     * 1 << 4 flag: isLooping
     * 1 << 5 flag: currentBuffer
     * 1 << 6 flag: minGain
     * 1 << 7 flag: maxGain
     * 1 << 8 flag: referenceDistance
     * 1 << 9 flag: rolloffFactor
     * 1 << 10 flag: maxDistance
     * 1 << 11 flag: pitch
     * 1 << 12 flag: direction
     * 1 << 13 flag: coneInnerAngle
     * 1 << 14 flag: coneOuterAngle
     * 1 << 15 flag: coneOuterGain
     * 1 << 16 flag: secOffset
     * 1 << 17 flag: sampleOffset
     * 1 << 18 flag: byteOffset
     * 
     * @returns Value of dirty flag
     */
    ALuint getDirty() { return dirty; }

    /**
     * @brief Dirty flag indicating which Source's attributes must be updated on device level
     * Dirty flag bits present as follows:
     * 
     * 1 << 0 flag: position
     * 1 << 1 flag: velocity
     * 1 << 2 flag: gain
     * 1 << 3 flag: isRelativeToListener
     * 1 << 4 flag: isLooping
     * 1 << 5 flag: currentBuffer
     * 1 << 6 flag: minGain
     * 1 << 7 flag: maxGain
     * 1 << 8 flag: referenceDistance
     * 1 << 9 flag: rolloffFactor
     * 1 << 10 flag: maxDistance
     * 1 << 11 flag: pitch
     * 1 << 12 flag: direction
     * 1 << 13 flag: coneInnerAngle
     * 1 << 14 flag: coneOuterAngle
     * 1 << 15 flag: coneOuterGain
     * 1 << 16 flag: secOffset
     * 1 << 17 flag: sampleOffset
     * 1 << 18 flag: byteOffset
     * 
     * @returns Reference to dirty flag
     */
    ALuint& getDirtyModifiable() { return dirty; }

    /**
     * @brief Source's Position vector
     * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
     * 
     * @returns Value of position vector
     */
    glm::vec3 getPosition() { return position; }
    
    /**
     * @brief Source's Position vector
     * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
     * 
     * @returns Reference to position vector
     */
    glm::vec3& getPositionModifiable() { dirty |= (1 << 0); return position; }

    /**
     * @brief Source's velocity vector
     * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
     * 
     * @returns Value of velocity vector
     */
    glm::vec3 getVelocity() { return velocity; }
    
    /**
     * @brief Source's velocity vector
     * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
     * 
     * @returns Reference to velocity vector
     */
    glm::vec3& getVelocityModifiable() { dirty |= (1 << 1); return velocity; }

    /**
     * @brief Source's gain value
     * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
     * 
     * @returns Value of gain
     */
    ALfloat getGain() { return gain; }

    /**
     * @brief Source's gain value
     * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
     * 
     * @returns Reference to gain
     */
    ALfloat& getGainModifiable() { dirty |= (1 << 2); return gain; }
    
    /**
     * @brief Is source considered Relative to Listener
     * If source is relative to listener, listeners transformations are not taken into accout while calculating gain.
     * 
     * AL_SOURCE_RELATIVE set to AL_TRUE indicates that the position, velocity, cone, and direction properties of a source are to be interpreted relative to the listener position.
     * 
     * @returns Value of isRelativeToListener
     */
    ALboolean getIsRelative() { return isRelativeToListener; }
    
    /**
     * @brief Is source considered Relative To Listener
     * If source is relative to listener, listeners transformations are not taken into accout while calculating gain.
     * 
     * AL_SOURCE_RELATIVE set to AL_TRUE indicates that the position, velocity, cone, and direction properties of a source are to be interpreted relative to the listener position.
     * 
     * @returns Reference to isRelativeToListener
     */
    ALboolean& getIsRelativeModifiable() { dirty |= (1 << 3); return isRelativeToListener; }

    /**
     * @brief Is source looping value 
     * AL_LOOPING is a flag that indicates that the source will not be in AL_STOPPED state once it reaches the end of last buffer in the buffer queue. 
     * Instead, the source will immediately promote to AL_INITIAL and AL_PLAYING. 
     * The default value is AL_FALSE.
     * 
     * @returns Value of isLooping
     */
    ALboolean getIsLooping() { return isLooping; }
    
    /**
     * @brief Is source looping value 
     * AL_LOOPING is a flag that indicates that the source will not be in AL_STOPPED state once it reaches the end of last buffer in the buffer queue. 
     * Instead, the source will immediately promote to AL_INITIAL and AL_PLAYING. 
     * The default value is AL_FALSE.
     * 
     * @returns Reference to isLooping
     */
    ALboolean& getIsLoopingModifiable() { dirty |= (1 << 4); return isLooping;}

    /**
     * @brief WRITEONLY: Name (id) of the current buffer, unique in the device 
     * Specifies the current buffer object, making it the head entry in the source's queue. 
     * Using AL_BUFFER on a source in the AL_STOPPED or AL_INITIAL state empties the entire queue, then appends the one buffer specified.
     * 
     * @returns Value of name (id) of currently set buffer
     */
    ALuint getCurrentBuffer() { return currentBuffer; }

    /**
     * @brief WRITEONLY: Name (id) of the current buffer, unique in the device 
     * Specifies the current buffer object, making it the head entry in the source's queue. 
     * Using AL_BUFFER on a source in the AL_STOPPED or AL_INITIAL state empties the entire queue, then appends the one buffer specified.
     * 
     * @returns Reference to name (id) of currently played buffer
     */
    ALuint& getCurrentBufferModifiable() { dirty |= (1 << 5); return currentBuffer; }
    
    /**
     * @brief Minimal AL_GAIN guaranteed for this source 
     * AL_MIN_GAIN is a scalar amplitude threshold. It indicates the minimal AL_GAIN that is always guaranteed for this source. 
     * 
     * If the effective gain is lower than AL_MIN_GAIN, AL_MIN_GAIN is applied. 
     * 
     * This happens before the listener gain is applied.
     * 
     * @returns Value of minGain
     */
    ALfloat getMinGain() { return minGain; }

    /**
     * @brief Minimal AL_GAIN guaranteed for this source 
     * AL_MIN_GAIN is a scalar amplitude threshold. It indicates the minimal AL_GAIN that is always guaranteed for this source. 
     * 
     * If the effective gain is lower than AL_MIN_GAIN, AL_MIN_GAIN is applied. 
     * 
     * This happens before the listener gain is applied.
     * 
     * @returns Reference to minGain
     */
    ALfloat& getMinGainModifiable() { dirty |= (1 << 6); return minGain; }
    
    /**
     * @brief Set the Source Max Gain 
     * AL_MAX_GAIN defines a scalar amplitude threshold. It indicates the maximal AL_GAIN permitted for this source. 
     * 
     * If the effective gain is higher than AL_MAX_GAIN, AL_MAX_GAIN is applied. 
     * This happens before the listener AL_GAIN is applied. 
     * 
     * Setting AL_MAX_GAIN to zero is the proper way to mute a source.
     * 
     * @returns Value of maxGain
     */
    ALfloat getMaxGain() { return maxGain; }

    /**
     * @brief Set the Source Max Gain 
     * AL_MAX_GAIN defines a scalar amplitude threshold. It indicates the maximal AL_GAIN permitted for this source. 
     * 
     * If the effective gain is higher than AL_MAX_GAIN, AL_MAX_GAIN is applied. 
     * This happens before the listener AL_GAIN is applied. 
     * 
     * Setting AL_MAX_GAIN to zero is the proper way to mute a source.
     * 
     * @returns Reference to maxGain
     */
    ALfloat& getMaxGainModifiable() { dirty |= (1 << 7); return maxGain; }
    
    /**
     * @brief Set the Source Reference Distance 
     * This is used for distance attenuation calculations based on inverse distance with rolloff. 
     * 
     * Depending on the distance model it will also act as a distance threshold below which gain is clamped.
     * 
     * @return Value of reference distance
     */
    ALfloat getReferenceDistance() { return referenceDistance; }
    
    /**
     * @brief Set the Source Reference Distance 
     * This is used for distance attenuation calculations based on inverse distance with rolloff. 
     * 
     * Depending on the distance model it will also act as a distance threshold below which gain is clamped.
     * 
     * @return Reference to reference distance
     */
    ALfloat& getReferenceDistanceModifiable() { dirty |= (1 << 8); return referenceDistance; }

    /**
     * @brief Source's rolloff factor value 
     * This is used for distance attenuation calculations based on inverse distance with rolloff. 
     * 
     * For distances smaller than AL_MAX_DISTANCE (and, depending on the distance model, larger than AL_REFERENCE_DISTANCE), this will scale the distance attenuation over the applicable range. 
     * 
     * In particular, AL_ROLLOFF_FACTOR can be set to zero for those sources that are supposed to be exempt from distance attenuation. 
     * 
     * @returns Value of rolloffFactor
     */
    ALfloat getRolloffFactor() { return rolloffFactor; }

    /**
     * @brief Source's rolloff factor value 
     * This is used for distance attenuation calculations based on inverse distance with rolloff. 
     * 
     * For distances smaller than AL_MAX_DISTANCE (and, depending on the distance model, larger than AL_REFERENCE_DISTANCE), this will scale the distance attenuation over the applicable range. 
     * 
     * In particular, AL_ROLLOFF_FACTOR can be set to zero for those sources that are supposed to be exempt from distance attenuation. 
     * 
     * @returns Reference to rolloffFactor
     */
    ALfloat& getRolloffFactorModifiable() { dirty |= (1 << 9); return rolloffFactor; }
    
    /**
     * @brief Source's max distance value 
     * This is used for distance attenuation calculations based on inverse distance with rolloff, if the Inverse Clamped Distance Model is used. 
     * 
     * In this case, distances greater than AL_MAX_DISTANCE will be clamped to AL_MAX_DISTANCE. 
     * 
     * AL_MAX_DISTANCE based clamping is applied before AL_MIN_GAIN clamping, so if the effective gain at AL_MAX_DISTANCE is larger than AL_MIN_GAIN, AL_MIN_GAIN will have no effect. No culling is supported.
     * 
     * @returns Value of maxDistance
     */
    ALfloat getMaxDistance() { return maxDistance; }

    /**
     * @brief Source's max distance value 
     * This is used for distance attenuation calculations based on inverse distance with rolloff, if the Inverse Clamped Distance Model is used. 
     * 
     * In this case, distances greater than AL_MAX_DISTANCE will be clamped to AL_MAX_DISTANCE. 
     * 
     * AL_MAX_DISTANCE based clamping is applied before AL_MIN_GAIN clamping, so if the effective gain at AL_MAX_DISTANCE is larger than AL_MIN_GAIN, AL_MIN_GAIN will have no effect. No culling is supported.
     * 
     * @returns Reference to maxDistance
     */
    ALfloat& getMaxDistanceModifiable() { dirty |= (1 << 10); return maxDistance; }
    
    /**
     * @brief Source's pitch value 
     * Desired pitch shift, where 1.0 equals identity. 
     * 
     * Each reduction by 50 percent equals a pitch shift of -12 semitones (one octave reduction). 
     * 
     * Each doubling equals a pitch shift of 12 semitones (one octave increase). 
     * 
     * Zero is not a legal value.
     * 
     * @returns Value of the pitch
     */
    ALfloat getPitch() { return pitch; }

    /**
     * @brief Source's pitch value 
     * Desired pitch shift, where 1.0 equals identity. 
     * 
     * Each reduction by 50 percent equals a pitch shift of -12 semitones (one octave reduction). 
     * 
     * Each doubling equals a pitch shift of 12 semitones (one octave increase). 
     * 
     * Zero is not a legal value.
     * 
     * @returns Reference to the pitch
     */
    ALfloat& getPitchModifiable() { dirty |= (1 << 11); return pitch; }
    
    /**
     * @brief Source's direction vector
     * If AL_DIRECTION does not equal the zero vector, the source is directional. 
     * 
     * The sound emission is presumed to be symmetric around the direction vector (cylinder symmetry). Sources are not oriented in full 3 degrees of freedom, only two angles are effectively needed. 
     * 
     * The zero vector is default, indicating that a source is not directional. Specifying a non- zero vector will make the source directional. Specifying a zero vector for a directional source will effectively mark it as non-directional.
     * 
     * @returns Value of direction vector
     */
    glm::vec3 getDirection() { return direction; }

    /**
     * @brief Source's direction vector
     * If AL_DIRECTION does not equal the zero vector, the source is directional. 
     * 
     * The sound emission is presumed to be symmetric around the direction vector (cylinder symmetry). Sources are not oriented in full 3 degrees of freedom, only two angles are effectively needed. 
     * 
     * The zero vector is default, indicating that a source is not directional. Specifying a non- zero vector will make the source directional. Specifying a zero vector for a directional source will effectively mark it as non-directional.
     * 
     * @returns Reference to direction vector
     */
    glm::vec3& getDirectionModifiable() { dirty |= (1 << 12); return direction; }
    
    /**
     * @brief Source's cone inner angle value 
     * Inside angle of the sound cone, in degrees. 
     * 
     * The default of 360 means that the inner angle covers the entire world, which is equivalent to an omni-directional source.
     * 
     * @returns Value of cone inner angle
     */
    ALfloat getConeInnerAngle() { return coneInnerAngle; }

    /**
     * @brief Source's cone inner angle value 
     * Inside angle of the sound cone, in degrees. 
     * 
     * The default of 360 means that the inner angle covers the entire world, which is equivalent to an omni-directional source.
     * 
     * @returns Reference to cone inner angle
     */
    ALfloat& getConeInnerAngleModifiable() { dirty |= (1 << 13); return coneInnerAngle; }
    
    /**
     * @brief Source cone outer angle value
     * Outer angle of the sound cone, in degrees. 
     * 
     * The default of 360 means that the outer angle covers the entire world. 
     * If the inner angle is also 360, then the zone for angle- dependent attenuation is zero.
     * 
     * @returns Value of cone outer angle
     */
    ALfloat getConeOuterAngle() { return coneOuterAngle; }

    /**
     * @brief Source cone outer angle value
     * Outer angle of the sound cone, in degrees. 
     * 
     * The default of 360 means that the outer angle covers the entire world. 
     * If the inner angle is also 360, then the zone for angle- dependent attenuation is zero.
     * 
     * @returns Reference to cone outer angle
     */
    ALfloat& getConeOuterAngleModifiable() { dirty |= (1 << 14); return coneOuterAngle; }
    
    /**
     * @brief Cone outer gain factor value 
     * The factor with which AL_GAIN is multiplied to determine the effective gain outside the cone defined by the outer angle. 
     * 
     * The effective gain applied outside the outer cone is AL_GAIN times AL_CONE_OUTER_GAIN. Changing AL_GAIN affects all directions, i.e. the source is attenuated in all directions, for any position of the listener. The application has to change AL_CONE_OUTER_GAIN as well if a different behavior is desired.
     * 
     * @returns Value of cone outer gain
     */
    ALfloat getConeOuterGain() { return coneOuterGain; }
    
    /**
     * @brief Cone outer gain factor value 
     * The factor with which AL_GAIN is multiplied to determine the effective gain outside the cone defined by the outer angle. 
     * 
     * The effective gain applied outside the outer cone is AL_GAIN times AL_CONE_OUTER_GAIN. Changing AL_GAIN affects all directions, i.e. the source is attenuated in all directions, for any position of the listener. The application has to change AL_CONE_OUTER_GAIN as well if a different behavior is desired.
     * 
     * @returns Reference to cone outer gain
     */
    ALfloat& getConeOuterGainModifiable() { dirty |= (1 << 15); return coneOuterGain; }

    /**
     * @brief WRITEONLY: Source's seconds offset value 
     * The playback position, expressed in seconds (the value will loop back to zero for looping sources). 
     * 
     * When setting AL_SEC_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
     * 
     * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed. 
     * 
     * This value is based on byte position, so a pitch-shifted source will have an exaggerated playback speed. For example, you can be 0.500 seconds into a buffer having taken only 0.250 seconds to get there if the pitch is set to 2.0.
     * 
     * @returns Value of seconds offset that was set
     */
    ALfloat getSecOffset() { return secOffset; }

    /**
     * @brief WRITEONLY: Source's seconds offset value 
     * The playback position, expressed in seconds (the value will loop back to zero for looping sources). 
     * 
     * When setting AL_SEC_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
     * 
     * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed. 
     * 
     * This value is based on byte position, so a pitch-shifted source will have an exaggerated playback speed. For example, you can be 0.500 seconds into a buffer having taken only 0.250 seconds to get there if the pitch is set to 2.0.
     * 
     * @returns Reference to seconds offset to set
     */
    ALfloat& getSecOffsetModifiable() { dirty |= (1 << 16); return secOffset; }
    
    /**
     * @brief WRITEONLY: Source's sample offset value 
     * The playback position, expressed in samples (the value will loop back to zero for looping sources). 
     * 
     * For a compressed format, this value will represent an exact offset within the uncompressed data.
     * 
     * When setting AL_SAMPLE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
     * An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
     * 
     * @returns Value of sample offset that was set
     */
    ALfloat getSampleOffset() { return sampleOffset; }

    /**
     * @brief WRITEONLY: Source's sample offset value 
     * The playback position, expressed in samples (the value will loop back to zero for looping sources). 
     * 
     * For a compressed format, this value will represent an exact offset within the uncompressed data.
     * 
     * When setting AL_SAMPLE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
     * An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
     * 
     * @returns Reference to sample offset to set
     */
    ALfloat& getSampleOffsetModifiable() { dirty |= (1 << 17); return sampleOffset; }
    
    /**
     * @brief WRITEONLY: Source's byte offset value 
     * The playback position, expressed in bytes (the value will loop back to zero for looping sources). 
     * For a compressed format, this value may represent an approximate offset within the compressed data buffer. 
     * 
     * When setting AL_BYTE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
     * 
     * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed.
     * 
     * @returns Value of byte offset that was set
     */
    ALfloat getByteOffset() { return byteOffset; }

    /**
     * @brief WRITEONLY: Source's byte offset value 
     * The playback position, expressed in bytes (the value will loop back to zero for looping sources). 
     * For a compressed format, this value may represent an approximate offset within the compressed data buffer. 
     * 
     * When setting AL_BYTE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
     * If the source is not playing, then the offset will be applied on the next alSourcePlay call. An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
     * 
     * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed.
     * 
     * @returns Reference to byte offset to set
     */
    ALfloat& getByteOffsetModifiable() { dirty |= (1 << 18); return byteOffset; }

    bool autoPlay = false;

    private:
        ALint dirty = 0;

        glm::vec3 position = {0.0f, 0.0f, 0.0f};

        glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
        
        ALfloat gain = 0;
        
        ALboolean isRelativeToListener = AL_FALSE;
        
        ALboolean isLooping = AL_FALSE;
        
        ALuint currentBuffer = AL_NONE;
        
        ALfloat minGain = 0.0f;
        
        ALfloat maxGain = 1.0f;
        
        ALfloat referenceDistance = 1.0f;
        
        ALfloat rolloffFactor = 1.0f;
        
        ALfloat maxDistance = std::numeric_limits<float>::infinity();
        
        ALfloat pitch = 1.0f;
        
        glm::vec3 direction = {0.0f, 0.0f, 0.0f};
        
        ALfloat coneInnerAngle = 360.0f;
        
        ALfloat coneOuterAngle = 360.0f;
        
        ALfloat coneOuterGain = 0.0f;
        
        ALfloat secOffset;
        
        ALfloat sampleOffset;
        
        ALfloat byteOffset;
};

#endif /* !AUDIOSOURCE_HPP_ */
