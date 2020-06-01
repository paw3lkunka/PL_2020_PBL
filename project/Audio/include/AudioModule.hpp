#ifndef AUDIOMODULE_HPP_
#define AUDIOMODULE_HPP_

#define alCheckErrors() alCheckErrorsImpl(__FILE__, __LINE__)
#define alcCheckErrors() alcCheckErrorsImpl(device, __FILE__, __LINE__)

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include "AudioFile.hpp"

#include "IModule.inl"

class Message;

class AudioListener;
class AudioSource;


struct AudioDeviceLevelException : public std::exception
{
    const char* message;

    AudioDeviceLevelException(const char* errorMessage)
    {
        message = errorMessage;
    }

    const char* what()
    {
        return message;
    }
};

struct AudioContextLevelException : public AudioDeviceLevelException
{
    AudioContextLevelException(const char* errorMessage) 
    : AudioDeviceLevelException(errorMessage)
    {}
};

/**
 * @brief Module responsible for communication with audio devices and updating audio structures attributes
 * 
 */
class AudioModule : public IModule
{
    public:
        /**
         * @brief Construct a new Audio Module object
         * 
         */
        AudioModule() = default;

        /**
         * @brief Destroy the Audio Module object
         * 
         */
        virtual ~AudioModule() = default;

        /**
         * @brief Handle incoming MessageBus messages
         * Modify listeners, sources and buffers in respond to the messages and received data.
         * 
         * @param msg - Received message with data
         */
        virtual void receiveMessage(Message msg);

        /**
         * @brief Initialize AudioModule
         * 
         * Connect to the default sound device.
         * Generate buffers.
         * Generate and start processing the context.
         */
        void init();

        /**
         * @brief Unloads current scene
         * 
         * Release all the contexts.
         * Release all buffers.
         */
        void unloadScene();

        /**
         * @brief Clean up AudioModule
         * 
         * Stop processing and delete the context.
         * Delete all remaining buffers in the device.
         * Disconnect from the default sound device.
         */
        void cleanup();

    protected:

    private:

        /// @brief Pointer used to operate on sound device 
        ALCdevice* device = nullptr;

        /// @brief Vector of all contexts existing on the device. Used for cleanup.
        std::vector<ALCcontext*> contexts = {};

        /**
         * @brief Pointer to currently processed AudioListener
         * 
         * If it is nullptr no AudioListener (context) is processed thus no sound is processed.
         * 
         * If a diffrent AudioListener's "isCurrent" flag is set it will be replaced and its "isCurrent" will be unset.
         */
        AudioListener* currentListener = nullptr;

        /**
         * @brief Unordered map of AudioClips' paths and their buffers
         * 
         * FIRST: Path to the file from /Resources folder
         * SECOND: Name (id) of the buffer
         */
        std::unordered_map<std::string, ALuint> clips = {};

        /**
         * @brief Unordered map of buffer names and their queue counters
         * 
         * FIRST: Name (id) of the buffer
         * SECOND: Queues it is in counter
         */
        std::unordered_map<ALuint, ALint> queueCounters = {};

        /// @brief Push all accepted changes to device at once
        void alcPushCurrentContextChangesToDevice();

        /// @brief Process current context
        void alcProcessCurrentContext();

        /// @brief Suspend current context
        void alcSuspendCurrentContext();

        #pragma region OpenAL errors checkers

        /**
         * @brief Check if any OpenAL context errors occurred
         * 
         * Implementation of the method that queries OpenAL context for errors from previous operations.
         * 
         * Use it after AL function calls.
         * 
         * @param filename - Name of file where the error occurred
         * @param line - Line of file where the error occurred
         */
        void alCheckErrorsImpl(const std::string& filename, const std::uint_fast32_t line);

        /**
         * @brief Check if any OpenAL device errors occurred
         * 
         * Implementation of the method that queries OpenAL device for errors from previous operations.
         * 
         * Use it after ALC function calls.
         * 
         * @param filename - Name of file where the error occurred
         * @param line - Line of file where the error occurred
         */
        void alcCheckErrorsImpl(ALCdevice* device, const std::string& filename, const std::uint_fast32_t line);

        #pragma endregion

        #pragma region Event Handlers

        /// @brief Handles AUDIO_LISTENER_INIT event
        void audioListenerInitHandler(AudioListener* audioListenerPtr);

        /// @brief Handles AUDIO_LISTENER_UPDATE event
        void audioListenerUpdateHandler(AudioListener* audioListenerPtr);

        /// @brief Handles AUDIO_SOURCE_UPDATE_LISTENERS event
        void audioSourceUpdateListenersHandler(AudioSource* audioSourcePtr);

        /// @brief Handles AUDIO_SOURCE_UPDATE_ATTRIBUTES event
        void audioSourceUpdateAttributesHandler(AudioSource* audioSourcePtr);

        /// @brief Handles AUDIO_SOURCE_PLAY event
        void audioSourcePlayHandler(AudioSource* audioSourcePtr);    
        
        /// @brief Handles AUDIO_SOURCE_STOP event
        void audioSourceStopHandler(AudioSource* audioSourcePtr);

        /// @brief Handles AUDIO_SOURCE_PAUSE event
        void audioSourcePauseHandler(AudioSource* audioSourcePtr);

        /// @brief Handles AUDIO_SOURCE_REWIND event
        void audioSourceRewindHandler(AudioSource* audioSourcePtr);

        ///@brief Handles RECEIVE_AUDIO_DATA event
        void receiveAudioDataHandler(AudioFile* audioFilePtr);

        #pragma endregion

        #pragma region Event helpers

        /// @brief Sets chosen AudioListener as current
        void audioListenerSetAsCurrentHelper(AudioListener* audioListenerPtr);

        /** @brief Updates buffers and queues of the source
         * 
         * Firstly it finds or generate all needed buffers.
         * Secondly it empties current queue of the source.
         * Finally new queue is set.
         * 
         */
        void audioSourceUpdateBuffersHelper(AudioSource* audioSourcePtr); 

        #pragma endregion

        #pragma region Buffers methods

        /**
         * @brief Load AudioClip to buffer using standard accessible values of a clip
         * 
         * @param bufferId - Name (id) of buffer to load clip to
         * @param channels - Number of channels (ex. 1 for MONO and 2 for STEREO)
         * @param sampleRate - Sample rate (frequency in Hertz) of the clip
         * @param bitsPerSample - Number of bits per sample (ex. 8bit or 16bit)
         * @param data - Raw data of audio clip 
         * @param size - Actual size read from clip header
         */
        void loadAudioFileDataToBuffer(ALuint bufferId, ALubyte channels, ALint sampleRate, ALubyte bitsPerSample, std::vector<ALchar>* data, ALsizei size);

        /**
         * @brief Set the Buffer Data object
         * Valid formats are: 
         * AL_FORMAT_MONO8 or AL_FORMAT_MONO16 for one channel.
         * AL_FORMAT_STEREO8 or AL_FORMAT_STEREO16 for two channels.
         * An implementation may expose other formats through extensions.
         * 
         * Buffers containing audio data with more than one channel will be played without 3D spatialization features – these formats are normally used for background music.
         * 
         * @param bufferId - Name (id) of buffer to modify
         * @param format - Format specifiing chanels number and sample rate of AudioClip
         * @param data - Data of the AudioClip 
         * @param size - Size of the AudioClip
         * @param frequency - 
         */
        void setBufferData(ALuint bufferId, ALenum format, ALvoid* data, ALsizei size, ALsizei frequency);

        #pragma endregion

        #pragma region Listener Getters
        
        /**
         * @brief Get the Listener Position
         * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
         * 
         * @return glm::vec3 - Listener's world position vector
         */
        glm::vec3 getListenerPosition();

        /**
         * @brief Get the Listener Velocity
         * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
         * 
         * @return glm::vec3 - Listener's velocity vector
         */
        glm::vec3 getListenerVelocity();

        /**
         * @brief Get the Listener Gain
         * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
         * 
         * @return ALfloat - Listener's gain value
         */
        ALfloat getListenerGain();

        /**
         * @brief Get the Listener Orientation
         * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
         * 
         * @param at - vector indicating the direction listener is facing 
         * (normal to listener's nose peak)
         * @param up - vector indicating the direction of what listener recognise as "up" 
         * (normal to listener's head peak)
         */
        void getListenerOrientation(glm::vec3* at, glm::vec3* up);
        
        #pragma endregion

        #pragma region Source Getters

        /**
         * @brief Get the Source Position 
         * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return glm::vec3 - Sources's world position vector
         */
        glm::vec3 getSourcePosition(ALuint sourceId);

        /**
         * @brief Get the Source Velocity 
         * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return glm::vec3 - Source's velocity vector
         */
        glm::vec3 getSourceVelocity(ALuint sourceId);

        /**
         * @brief Get the Source Gain 
         * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's gain value
         */
        ALfloat getSourceGain(ALuint sourceId);

        /**
         * @brief Get the Source Is Relative To Listener
         * If source is relative to listener, listeners transformations are not taken into accout while calculating gain.
         * 
         * AL_SOURCE_RELATIVE set to AL_TRUE indicates that the position, velocity, cone, and direction properties of a source are to be interpreted relative to the listener position.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALboolean - Is source considered relative value
         */
        ALboolean getSourceIsRelative(ALuint sourceId);

        /**
         * @brief Get the Source Type 
         * AL_SOURCE_TYPE is a read-only property indicating whether a source is ready to queue buffers, ready to use a static buffer, or is in an undetermined state where it can be used for either streaming or static playback.
         * 
         * When first created, a source will be in the AL_UNDETERMINED state. 
         * 
         * If a buffer is then attached using alSourcei(sid, AL_BUFFER, bid), then the source will enter the AL_STATIC state. 
         * 
         * If the first buffer attached to a source is attached using alSourceQueueBuffers, then the source will enter the AL_STREAMING state. 
         * 
         * Attaching the NULL buffer using alSourcei(sid, AL_BUFFER, NULL) to a source of either type will reset the state to AL_UNDETERMINED, and attaching any buffer to a streaming source will change the state to AL_STATIC.
         * 
         * Attempting to queue a buffer on a static source will result in an AL_INVALID_OPERATION error.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALenum - Enum value of source's state
         */
        ALenum getSourceType(ALuint sourceId);

        /**
         * @brief Get the Source Is Looping 
         * AL_LOOPING is a flag that indicates that the source will not be in AL_STOPPED state once it reaches the end of last buffer in the buffer queue. 
         * Instead, the source will immediately promote to AL_INITIAL and AL_PLAYING. 
         * The default value is AL_FALSE.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALboolean - Is source looping value
         */
        ALboolean getSourceIsLooping(ALuint sourceId);

        /**
         * @brief Get the Source Current Buffer 
         * Specifies the current buffer object, making it the head entry in the source's queue. 
         * Using AL_BUFFER on a source in the AL_STOPPED or AL_INITIAL state empties the entire queue, then appends the one buffer specified.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALuint - Name (id) of the buffer, unique in the device
         */
        ALuint getSourceCurrentBuffer(ALuint sourceId);

        /**
         * @brief Get the Source Min Gain
         * AL_MIN_GAIN is a scalar amplitude threshold. It indicates the minimal AL_GAIN that is always guaranteed for this source. 
         * 
         * If the effective gain is lower than AL_MIN_GAIN, AL_MIN_GAIN is applied. 
         * 
         * This happens before the listener gain is applied.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Minimal AL_GAIN guaranteed for this source
         */
        ALfloat getSourceMinGain(ALuint sourceId);

        /**
         * @brief Get the Source Max Gain 
         * AL_MAX_GAIN defines a scalar amplitude threshold. It indicates the maximal AL_GAIN permitted for this source. 
         * 
         * If the effective gain is higher than AL_MAX_GAIN, AL_MAX_GAIN is applied. 
         * This happens before the listener AL_GAIN is applied. 
         * 
         * Setting AL_MAX_GAIN to zero is the proper way to mute a source.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Maximal AL_GAIN permitted for this source
         */
        ALfloat getSourceMaxGain(ALuint sourceId);

        /**
         * @brief Get the Source Reference Distance 
         * This is used for distance attenuation calculations based on inverse distance with rolloff. 
         * 
         * Depending on the distance model it will also act as a distance threshold below which gain is clamped.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's reference distance value
         */
        ALfloat getSourceReferenceDistance(ALuint sourceId);

        /**
         * @brief Get the Source Rolloff Factor 
         * This is used for distance attenuation calculations based on inverse distance with rolloff. 
         * 
         * For distances smaller than AL_MAX_DISTANCE (and, depending on the distance model, larger than AL_REFERENCE_DISTANCE), this will scale the distance attenuation over the applicable range. 
         * 
         * In particular, AL_ROLLOFF_FACTOR can be set to zero for those sources that are supposed to be exempt from distance attenuation. 
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's rolloff factor value
         */
        ALfloat getSourceRolloffFactor(ALuint sourceId);

        /**
         * @brief Get the Source Max Distance 
         * This is used for distance attenuation calculations based on inverse distance with rolloff, if the Inverse Clamped Distance Model is used. 
         * 
         * In this case, distances greater than AL_MAX_DISTANCE will be clamped to AL_MAX_DISTANCE. 
         * 
         * AL_MAX_DISTANCE based clamping is applied before AL_MIN_GAIN clamping, so if the effective gain at AL_MAX_DISTANCE is larger than AL_MIN_GAIN, AL_MIN_GAIN will have no effect. No culling is supported.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's max distance value
         */
        ALfloat getSourceMaxDistance(ALuint sourceId);

        /**
         * @brief Get the Source Pitch 
         * Desired pitch shift, where 1.0 equals identity. 
         * 
         * Each reduction by 50 percent equals a pitch shift of -12 semitones (one octave reduction). 
         * 
         * Each doubling equals a pitch shift of 12 semitones (one octave increase). 
         * 
         * Zero is not a legal value.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's pitch value
         */
        ALfloat getSourcePitch(ALuint sourceId);

        /**
         * @brief Get the Source Direction 
         * If AL_DIRECTION does not equal the zero vector, the source is directional. 
         * 
         * The sound emission is presumed to be symmetric around the direction vector (cylinder symmetry). Sources are not oriented in full 3 degrees of freedom, only two angles are effectively needed. 
         * 
         * The zero vector is default, indicating that a source is not directional. Specifying a non- zero vector will make the source directional. Specifying a zero vector for a directional source will effectively mark it as non-directional.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return glm::vec3 - Source's direction vector
         */
        glm::vec3 getSourceDirection(ALuint sourceId);

        /**
         * @brief Get the Source Cone Inner Angle 
         * Inside angle of the sound cone, in degrees. 
         * 
         * The default of 360 means that the inner angle covers the entire world, which is equivalent to an omni-directional source.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's cone inner angle value
         */
        ALfloat getSourceConeInnerAngle(ALuint sourceId);

        /**
         * @brief Get the Source Cone Outer Angle 
         * Outer angle of the sound cone, in degrees. 
         * 
         * The default of 360 means that the outer angle covers the entire world. 
         * If the inner angle is also 360, then the zone for angle- dependent attenuation is zero.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source cone outer angle value
         */
        ALfloat getSourceConeOuterAngle(ALuint sourceId);

        /**
         * @brief Get the Source Cone Outer Gain 
         * The factor with which AL_GAIN is multiplied to determine the effective gain outside the cone defined by the outer angle. 
         * 
         * The effective gain applied outside the outer cone is AL_GAIN times AL_CONE_OUTER_GAIN. Changing AL_GAIN affects all directions, i.e. the source is attenuated in all directions, for any position of the listener. The application has to change AL_CONE_OUTER_GAIN as well if a different behavior is desired.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Cone outer gain factor value
         */
        ALfloat getSourceConeOuterGain(ALuint sourceId);

        /**
         * @brief Get the Source Sec Offset 
         * The playback position, expressed in seconds (the value will loop back to zero for looping sources). 
         * 
         * When setting AL_SEC_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
         * 
         * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed. 
         * 
         * This value is based on byte position, so a pitch-shifted source will have an exaggerated playback speed. For example, you can be 0.500 seconds into a buffer having taken only 0.250 seconds to get there if the pitch is set to 2.0.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's seconds offset value
         */
        ALfloat getSourceSecOffset(ALuint sourceId);

        /**
         * @brief Get the Source Sample Offset 
         * The playback position, expressed in samples (the value will loop back to zero for looping sources). 
         * 
         * For a compressed format, this value will represent an exact offset within the uncompressed data.
         * 
         * When setting AL_SAMPLE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
         * An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
         * 
         * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's sample offset value
         */
        ALfloat getSourceSampleOffset(ALuint sourceId);

        /**
         * @brief Get the Source Byte Offset 
         * The playback position, expressed in bytes (the value will loop back to zero for looping sources). 
         * For a compressed format, this value may represent an approximate offset within the compressed data buffer. 
         * 
         * When setting AL_BYTE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
         * 
         * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALfloat - Source's byte offset value
         */
        ALfloat getSourceByteOffset(ALuint sourceId);

        /**
         * @brief Get the Source Buffers Queued Count 
         * Query only. Query the number of buffers in the queue of a given source. This includes those not yet played, the one currently playing, and the ones that have been played already. 
         * This will return 0 if the current and only buffer name is 0.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALuint - Number of buffers in the queue
         */
        ALuint getSourceBuffersQueuedCount(ALuint sourceId);

        /**
         * @brief Get the Source Buffers Processed Count object
         * Query only. Query the number of buffers that have been played by a given source. Indirectly, this gives the index of the buffer currently playing. 
         * 
         * Used to determine how many slots are needed for dequeuing them. 
         * 
         * On a source in the AL_STOPPED state, all buffers are processed. 
         * On a source in the AL_INITIAL state, no buffers are processed, all buffers are pending. This will return 0 if the current and only buffer name is 0.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @return ALuint - Number of buffers that have been played by a given source
         */
        ALuint getSourceBuffersProcessedCount(ALuint sourceId);

        #pragma endregion

        #pragma region Listener Setters

        /**
         * @brief Set the Listener Position 
         * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
         * 
         * @param position - Listener's world position vector
         */
        void setListenerPosition(glm::vec3 position);

        /**
         * @brief Set the Listener Velocity 
         * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
         * 
         * @param velocity - Listener's velocity vector
         */
        void setListenerVelocity(glm::vec3 velocity);

        /**
         * @brief Set the Listener Gain 
         * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
         * 
         * @param gain - Listener's gain value
         */
        void setListenerGain(ALfloat gain);

        /**
         * @brief Set the Listener Orientation 
         * AL_ORIENTATION is a pair of 3-tuples consisting of an 'at' vector and an 'up' vector, where the 'at' vector represents the 'forward' direction of the listener and the orthogonal projection of the 'up' vector into the subspace perpendicular to the 'at' vector represents the 'up' direction for the listener. OpenAL expects two vectors that are linearly independent.
         * 
         * @param at - vector indicating the direction listener is facing 
         * (normal to listener's nose peak)
         * @param up - vector indicating the direction of what listener recognise as "up" 
         * (normal to listener's head peak)
         */
        void setListenerOrientation(glm::vec3 at, glm::vec3 up);

        #pragma endregion

        #pragma region Source Setters

        /**
         * @brief Set the Source Position 
         * AL_POSITION specifies the current location of the object in the world coordinate system. Any 3-tuple of valid float values is allowed. Implementation behavior on encountering NaN and infinity is not defined. The object position is always defined in the world coordinate system.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param position - Sources's world position vector
         */
        void setSourcePosition(ALuint sourceId, glm::vec3 position);
        
        /**
         * @brief Set the Source Velocity 
         * AL_VELOCITY specifies the current velocity (speed and direction) of the object, in the world coordinate system. Any 3-tuple of valid float/double values is allowed. The object AL_VELOCITY does not affect the source's position and vice-versa.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param velocity - Source's velocity vector
         */
        void setSourceVelocity(ALuint sourceId, glm::vec3 velocity);
        
        /**
         * @brief Set the Source Gain 
         * AL_GAIN defines a scalar amplitude multiplier. As a listener attribute, it effectively applies to all sources in the current context. The default 1.0 means that the sound is unattenuated. An AL_GAIN value of 0.5 is equivalent to an attenuation of 6 dB. The value zero equals silence (no contribution to the output mix).
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param gain - Source's gain value
         */
        void setSourceGain(ALuint sourceId, ALfloat gain);
        
        /**
         * @brief Set the Source Is Relative To Listener
         * If source is relative to listener, listeners transformations are not taken into accout while calculating gain.
         * 
         * AL_SOURCE_RELATIVE set to AL_TRUE indicates that the position, velocity, cone, and direction properties of a source are to be interpreted relative to the listener position.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param isRelativeToListener - Is source considered relative to listener
         */
        void setSourceIsRelative(ALuint sourceId, ALboolean isRelativeToListener);
        
        /**
         * @brief Set the Source Is Looping 
         * AL_LOOPING is a flag that indicates that the source will not be in AL_STOPPED state once it reaches the end of last buffer in the buffer queue. 
         * Instead, the source will immediately promote to AL_INITIAL and AL_PLAYING. 
         * The default value is AL_FALSE.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param isLooping - Is source looping value
         */
        void setSourceIsLooping(ALuint sourceId, ALboolean isLooping);
        
        /**
         * @brief Set the Source Current Buffer 
         * Specifies the current buffer object, making it the head entry in the source's queue. 
         * Using AL_BUFFER on a source in the AL_STOPPED or AL_INITIAL state empties the entire queue, then appends the one buffer specified.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param bufferId - Name (id) of the buffer, unique in the device
         */
        void setSourceCurrentBuffer(ALuint sourceId, ALuint bufferId);
        
        /**
         * @brief Set the Source Min Gain 
         * AL_MIN_GAIN is a scalar amplitude threshold. It indicates the minimal AL_GAIN that is always guaranteed for this source. 
         * 
         * If the effective gain is lower than AL_MIN_GAIN, AL_MIN_GAIN is applied. 
         * 
         * This happens before the listener gain is applied.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param minGain - Minimal AL_GAIN guaranteed for this source
         */
        void setSourceMinGain(ALuint sourceId, ALfloat minGain);
        
        /**
         * @brief Set the Source Max Gain 
         * AL_MAX_GAIN defines a scalar amplitude threshold. It indicates the maximal AL_GAIN permitted for this source. 
         * 
         * If the effective gain is higher than AL_MAX_GAIN, AL_MAX_GAIN is applied. 
         * This happens before the listener AL_GAIN is applied. 
         * 
         * Setting AL_MAX_GAIN to zero is the proper way to mute a source.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param maxGain - Maximal AL_GAIN permitted for this source
         */
        void setSourceMaxGain(ALuint sourceId, ALfloat maxGain);
        
        /**
         * @brief Set the Source Reference Distance 
         * This is used for distance attenuation calculations based on inverse distance with rolloff. 
         * 
         * Depending on the distance model it will also act as a distance threshold below which gain is clamped.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param referenceDistance - Source's reference distance value
         */
        void setSourceReferenceDistance(ALuint sourceId, ALfloat referenceDistance);
        
        /**
         * @brief Set the Source Rolloff Factor 
         * This is used for distance attenuation calculations based on inverse distance with rolloff. 
         * 
         * For distances smaller than AL_MAX_DISTANCE (and, depending on the distance model, larger than AL_REFERENCE_DISTANCE), this will scale the distance attenuation over the applicable range. 
         * 
         * In particular, AL_ROLLOFF_FACTOR can be set to zero for those sources that are supposed to be exempt from distance attenuation. 
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param rolloffFactor - Source's rolloff factor value
         */
        void setSourceRolloffFactor(ALuint sourceId, ALfloat rolloffFactor);
        
        /**
         * @brief Set the Source Max Distance 
         * This is used for distance attenuation calculations based on inverse distance with rolloff, if the Inverse Clamped Distance Model is used. 
         * 
         * In this case, distances greater than AL_MAX_DISTANCE will be clamped to AL_MAX_DISTANCE. 
         * 
         * AL_MAX_DISTANCE based clamping is applied before AL_MIN_GAIN clamping, so if the effective gain at AL_MAX_DISTANCE is larger than AL_MIN_GAIN, AL_MIN_GAIN will have no effect. No culling is supported.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param maxDistance - Source's max distance value
         */
        void setSourceMaxDistance(ALuint sourceId, ALfloat maxDistance);
        
        /**
         * @brief Set the Source Pitch 
         * Desired pitch shift, where 1.0 equals identity. 
         * 
         * Each reduction by 50 percent equals a pitch shift of -12 semitones (one octave reduction). 
         * 
         * Each doubling equals a pitch shift of 12 semitones (one octave increase). 
         * 
         * Zero is not a legal value.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param pitch - Source's pitch value
         */
        void setSourcePitch(ALuint sourceId, ALfloat pitch);
        
        /**
         * @brief Set the Source Direction 
         * If AL_DIRECTION does not equal the zero vector, the source is directional. 
         * 
         * The sound emission is presumed to be symmetric around the direction vector (cylinder symmetry). Sources are not oriented in full 3 degrees of freedom, only two angles are effectively needed. 
         * 
         * The zero vector is default, indicating that a source is not directional. Specifying a non- zero vector will make the source directional. Specifying a zero vector for a directional source will effectively mark it as non-directional.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param direction - Source's direction vector
         */
        void setSourceDirection(ALuint sourceId, glm::vec3 direction);
        
        /**
         * @brief Set the Source Cone Inner Angle 
         * Inside angle of the sound cone, in degrees. 
         * 
         * The default of 360 means that the inner angle covers the entire world, which is equivalent to an omni-directional source.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param coneInnerAngle - Source's cone inner angle value
         */
        void setSourceConeInnerAngle(ALuint sourceId, ALfloat coneInnerAngle);
        
        /**
         * @brief Set the Source Cone Outer Angle 
         * Outer angle of the sound cone, in degrees. 
         * 
         * The default of 360 means that the outer angle covers the entire world. 
         * If the inner angle is also 360, then the zone for angle- dependent attenuation is zero.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param coneOuterAngle - Source cone outer angle value
         */
        void setSourceConeOuterAngle(ALuint sourceId, ALfloat coneOuterAngle);
        
        /**
         * @brief Set the Source Cone Outer Gain 
         * The factor with which AL_GAIN is multiplied to determine the effective gain outside the cone defined by the outer angle. 
         * 
         * The effective gain applied outside the outer cone is AL_GAIN times AL_CONE_OUTER_GAIN. Changing AL_GAIN affects all directions, i.e. the source is attenuated in all directions, for any position of the listener. The application has to change AL_CONE_OUTER_GAIN as well if a different behavior is desired.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param coneOuterGain - Cone outer gain factor value
         */
        void setSourceConeOuterGain(ALuint sourceId, ALfloat coneOuterGain);
        
        /**
         * @brief Set the Source Sec Offset 
         * The playback position, expressed in seconds (the value will loop back to zero for looping sources). 
         * 
         * When setting AL_SEC_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
         * 
         * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed. 
         * 
         * This value is based on byte position, so a pitch-shifted source will have an exaggerated playback speed. For example, you can be 0.500 seconds into a buffer having taken only 0.250 seconds to get there if the pitch is set to 2.0.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param secOffset - Source's seconds offset value
         */
        void setSourceSecOffset(ALuint sourceId, ALfloat secOffset);

        /**
         * @brief Set the Source Sample Offset 
         * The playback position, expressed in samples (the value will loop back to zero for looping sources). 
         * 
         * For a compressed format, this value will represent an exact offset within the uncompressed data.
         * 
         * When setting AL_SAMPLE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. 
         * An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param sampleOffset - Source's sample offset value
         */
        void setSourceSampleOffset(ALuint sourceId, ALfloat sampleOffset);

        /**
         * @brief Set the Source Byte Offset 
         * The playback position, expressed in bytes (the value will loop back to zero for looping sources). 
         * For a compressed format, this value may represent an approximate offset within the compressed data buffer. 
         * 
         * When setting AL_BYTE_OFFSET on a source which is already playing, the playback will jump to the new offset unless the new offset is out of range, in which case an AL_INVALID_VALUE error is set. 
         * If the source is not playing, then the offset will be applied on the next alSourcePlay call. An alSourceStop, alSourceRewind, or a second alSourcePlay call will reset the offset to the beginning of the buffer. 
         * 
         * The position is relative to the beginning of all the queued buffers for the source, and any queued buffers traversed by a set call will be marked as processed.
         * 
         * @param sourceId - Name (id) of the source, unique in the context
         * @param byteOffset - Source's byte offset value
         */
        void setSourceByteOffset(ALuint sourceId, ALfloat byteOffset);

        #pragma endregion
};

#endif /* !AUDIOMODULE_HPP_ */
