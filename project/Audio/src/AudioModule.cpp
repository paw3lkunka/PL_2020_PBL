#include "AudioModule.hpp"

#include "Core.hpp"

#include "Event.inl"
#include "Message.inl"
#include "MessageBus.hpp"

#include "FileStructures.inl"

#include "AudioSource.inl"
#include "AudioListener.inl"


void AudioModule::receiveMessage(Message msg)
{
    try
    {
        switch (msg.getEvent())
        {
            case Event::AUDIO_LISTENER_INIT:
            {
                auto audioListenerPtr = msg.getValue<AudioListener*>();
                audioListenerInitHandler(audioListenerPtr);
            }
            break;

            case Event::AUDIO_LISTENER_UPDATE:
            {
                auto audioListenerPtr = msg.getValue<AudioListener*>();
                audioListenerUpdateHandler(audioListenerPtr);
            }
            break;

            case Event::AUDIO_SOURCE_INIT:
            {
                auto audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourceInitHandler(audioSourcePtr);
            }
            break;

            case Event::AUDIO_SOURCE_UPDATE:
            {
                auto audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourceUpdateHandler(audioSourcePtr);
            }
            break;

            case Event::AUDIO_SOURCE_PLAY:
            {
                AudioSource* audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourcePlayHandler(audioSourcePtr);
            }
            break;

            case Event::AUDIO_SOURCE_STOP:
            {
                auto audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourceStopHandler(audioSourcePtr);
            }
            break;

            case Event::AUDIO_SOURCE_PAUSE:
            {
                auto audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourcePauseHandler(audioSourcePtr);
            }
            break;
            
            case Event::AUDIO_SOURCE_REWIND:
            {
                auto audioSourcePtr = msg.getValue<AudioSource*>();
                audioSourceRewindHandler(audioSourcePtr);
            }
            break;
            
            case Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING:
            {
                paused = !paused;
                if(paused)
                {
                    for(auto it = playingSources.begin(); it != playingSources.end(); it++)
                    {
                        audioSourcePauseHandler(*it);
                    }
                }
                else
                {
                    for(auto it = playingSources.begin(); it != playingSources.end(); it++)
                    {
                        audioSourcePlayHandler(*it);
                    }
                }
            }
            break;

            case Event::RECEIVE_AUDIO_DATA:
            {
                auto audioFile = msg.getValue<AudioFile*>();
                receiveAudioDataHandler(audioFile);
            }
            break;
        }

        if(msg.getEvent() >= Event::AUDIO_FIRST && msg.getEvent() <= Event::AUDIO_LAST)
        {
            alcPushCurrentContextChangesToDevice();
            
            for(auto it = playingSources.begin(); it != playingSources.end(); it++)
            {
                ALint state;
                alGetSourcei( (*it)->name, AL_SOURCE_STATE, &state );
                if(state == AL_STOPPED)
                {
                    playingSources.erase(it);
                }
            }
        }
    }
    catch(AudioContextLevelException e)
    {
        std::cerr << e.what();
    }
    catch(AudioDeviceLevelException e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
}

void AudioModule::init()
{
    try
    {
        // Connect to default audio device
        device = alcOpenDevice(NULL);
        if(device == nullptr)
        {
            alcCheckErrors();
        }
    }
    catch(AudioDeviceLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
}

void AudioModule::sceneInit()
{
    try
    {        
        listener->context = alcCreateContext(device, nullptr);
        if(listener->context == nullptr)
        {
            alcCheckErrors();
        }
        alcMakeContextCurrent(listener->context);
        alcCheckErrors();

        //Create all needed buffers for AudioSources
        for(auto it = audioBuffers.begin(); it != audioBuffers.end(); it++)
        {
            alGenBuffers(1, &it->second);
            alCheckErrors();
            GetCore().getMessageBus().sendMessage( Message(Event::QUERY_AUDIO_DATA, &it->first) );
        }

        // Send QUERY_AUDIO_DATA
        GetCore().getMessageBus().notify();
        // Receive RECEIVE_AUDIO_DATA
        GetCore().getMessageBus().notify();

        alcPushCurrentContextChangesToDevice();

        for(auto source : sources)
        {
            alGenSources(1, &source->name);
            alCheckErrors();

            ALuint* buffer = &( audioBuffers.find(source->audioClip)->second );
            alSourceQueueBuffers(source->name, 1, buffer);
            alCheckErrors();
        }

        alcPushCurrentContextChangesToDevice();
    }
    catch(AudioContextLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(AudioDeviceLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
}

void AudioModule::sceneUnload()
{
    try
    {
        for(auto src : sources)
        {
            alSourcePlay(src->name);
            alSourceStop(src->name);
            setSourceIsLooping(src->name, false);
            
            ALuint buffers;
            alSourceUnqueueBuffers(src->name, 1, &buffers);
            alCheckErrors();
            
            alSourcei(src->name, AL_BUFFER, 0);
            alCheckErrors();
            
            alDeleteSources(1, &src->name);
            alCheckErrors();

            alcPushCurrentContextChangesToDevice();
        }
        sources.clear();
        playingSources.clear();

        for(auto buffer : audioBuffers)
        {
            alDeleteBuffers(1, &buffer.second);
            alCheckErrors();
        }
        audioBuffers.clear();

        auto releaseContextStatus = alcMakeContextCurrent(nullptr);
        if(releaseContextStatus == ALC_FALSE)
        {
            alcCheckErrors();
        }
        alcDestroyContext(listener->context);
        alcCheckErrors();
    }
    catch(AudioContextLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(AudioDeviceLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
}

void AudioModule::cleanup()
{
    try
    {
        sceneUnload();

        // Disconnect from connected device
        auto closeDeviceStatus = alcCloseDevice(device);
        if(closeDeviceStatus == ALC_FALSE)
        {
            alcCheckErrors();
        }
    }
    catch(AudioDeviceLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
}

void AudioModule::alcPushCurrentContextChangesToDevice()
{
    alcProcessCurrentContext();
    alcSuspendCurrentContext();
}

void AudioModule::alcProcessCurrentContext()
{
    alcProcessContext(listener->context);
    alcCheckErrors();
}

void AudioModule::alcSuspendCurrentContext()
{
    alcSuspendContext(listener->context);
    alcCheckErrors();
}

#pragma region OpenAL errors checkers

void AudioModule::alCheckErrorsImpl(const std::string& filename, const std::uint_fast32_t line)
{
    ALenum error = alGetError();
    if(error == AL_NO_ERROR)
    {
        return;
    }
    
    std::stringstream message;
    message << "[Audio]" << filename << ":" << line << ": error: ";

    switch(error)
    {
    case AL_INVALID_NAME:
        message << "AL_INVALID_NAME: a bad name (id) was passed to an OpenAL function";
        break;
    case AL_INVALID_ENUM:
        message << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
        break;
    case AL_INVALID_VALUE:
        message << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
        break;
    case AL_INVALID_OPERATION:
        message << "AL_INVALID_OPERATION: the requested operation is not valid";
        break;
    case AL_OUT_OF_MEMORY:
        message << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
        break;
    default:
        message << "UNKNOWN AL ERROR: " << error;
    }

    message << "\n";
    throw AudioContextLevelException(message.str().c_str());
}

void AudioModule::alcCheckErrorsImpl(ALCdevice* device, const std::string& filename, const std::uint_fast32_t line)
{
    ALenum error = alcGetError(device);
    if(error == ALC_NO_ERROR)
    {
        return;
    }
    
    std::stringstream message;
    message << "[Audio]" << filename << ":" << line << ": error: ";

    switch(error)
    {
    case ALC_INVALID_DEVICE:
        message << "ALC_INVALID_DEVICE: a bad DEVICE name (id) was passed to an OpenAL function";
        break;
    case ALC_INVALID_CONTEXT:
        message << "ALC_INVALID_NAME: a bad CONTEXT name (id) was passed to an OpenAL function";
        break;
    case ALC_INVALID_ENUM:
        message << "ALC_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
        break;
    case ALC_INVALID_VALUE:
        message << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
        break;
    case ALC_OUT_OF_MEMORY:
        message << "ALC_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
        break;
    default:
        message << "UNKNOWN ALC ERROR: " << error;
    }

    message << "\n";
    throw AudioDeviceLevelException(message.str().c_str());
}


#pragma endregion

#pragma region Event handlers

void AudioModule::audioListenerInitHandler(AudioListener* audioListenerPtr)
{
    if(listener != nullptr)
    {
        std::cout << "WARNING: Only one AudioListener for a scene is valid! Ignoring duplicate..." << std::endl;
        return;
    }

    listener = audioListenerPtr;
}

void AudioModule::audioListenerUpdateHandler(AudioListener* audioListenerPtr)
{
    if(audioListenerPtr->getDirty() & (1 << 0))
    {
        setListenerPosition(audioListenerPtr->getPosition());
        audioListenerPtr->getDirtyModifiable() &= ~(1 << 0);
    }

    if(audioListenerPtr->getDirty() & (1 << 1))
    {
        setListenerVelocity(audioListenerPtr->getVelocity());
        audioListenerPtr->getDirtyModifiable() &= ~(1 << 1);
    }

    if(audioListenerPtr->getDirty() & (1 << 2))
    {
        setListenerGain(audioListenerPtr->getGain());
        audioListenerPtr->getDirtyModifiable() &= ~(1 << 2);
    }

    if(audioListenerPtr->getDirty() & (1 << 3))
    {
        setListenerOrientation(audioListenerPtr->getAt(), audioListenerPtr->getUp());
        audioListenerPtr->getDirtyModifiable() &= ~(1 << 3);
    }
}

void AudioModule::audioSourceInitHandler(AudioSource* audioSourcePtr)
{
    auto buffer = audioBuffers.find(audioSourcePtr->audioClip);
    if(buffer == audioBuffers.end())
    {
        audioBuffers.insert( std::pair(audioSourcePtr->audioClip, 0) );
    }

    sources.push_back(audioSourcePtr);
}

void AudioModule::audioSourceUpdateHandler(AudioSource* audioSourcePtr)
{
    if(audioSourcePtr->getDirty() & (1 << 0))
    {
        setSourcePosition(audioSourcePtr->name, audioSourcePtr->getPosition());
    }

    if(audioSourcePtr->getDirty() & (1 << 1))
    {
        setSourceVelocity(audioSourcePtr->name, audioSourcePtr->getVelocity());
    }

    if(audioSourcePtr->getDirty() & (1 << 2))
    {
        setSourceGain(audioSourcePtr->name, audioSourcePtr->getGain());
    }

    if(audioSourcePtr->getDirty() & (1 << 3))
    {
        setSourceIsRelative(audioSourcePtr->name, audioSourcePtr->getIsRelative());
    }

    if(audioSourcePtr->getDirty() & (1 << 4))
    {
        setSourceIsLooping(audioSourcePtr->name, audioSourcePtr->getIsLooping());
    }

    if(audioSourcePtr->getDirty() & (1 << 5))
    {
        setSourceCurrentBuffer(audioSourcePtr->name, audioSourcePtr->getCurrentBuffer());
    }

    if(audioSourcePtr->getDirty() & (1 << 6))
    {
        setSourceMinGain(audioSourcePtr->name, audioSourcePtr->getMinGain());
    }

    if(audioSourcePtr->getDirty() & (1 << 7))
    {
        setSourceMaxGain(audioSourcePtr->name, audioSourcePtr->getMaxGain());
    }

    if(audioSourcePtr->getDirty() & (1 << 8))
    {
        setSourceReferenceDistance(audioSourcePtr->name, audioSourcePtr->getReferenceDistance());
    }

    if(audioSourcePtr->getDirty() & (1 << 9))
    {
        setSourceRolloffFactor(audioSourcePtr->name, audioSourcePtr->getRolloffFactor());
    }

    if(audioSourcePtr->getDirty() & (1 << 10))
    {
        setSourceMaxDistance(audioSourcePtr->name, audioSourcePtr->getMaxDistance());
    }

    if(audioSourcePtr->getDirty() & (1 << 11))
    {
        setSourcePitch(audioSourcePtr->name, audioSourcePtr->getPitch());
    }

    if(audioSourcePtr->getDirty() & (1 << 12))
    {
        setSourceDirection(audioSourcePtr->name, audioSourcePtr->getDirection());
    }

    if(audioSourcePtr->getDirty() & (1 << 13))
    {
        setSourceConeInnerAngle(audioSourcePtr->name, audioSourcePtr->getConeInnerAngle());
    }

    if(audioSourcePtr->getDirty() & (1 << 14))
    {
        setSourceConeOuterAngle(audioSourcePtr->name, audioSourcePtr->getConeOuterAngle());
    }

    if(audioSourcePtr->getDirty() & (1 << 15))
    {
        setSourceConeOuterGain(audioSourcePtr->name, audioSourcePtr->getConeOuterGain());
    }

    if(audioSourcePtr->getDirty() & (1 << 16))
    {
        setSourceSecOffset(audioSourcePtr->name, audioSourcePtr->getSecOffset());
    }

    if(audioSourcePtr->getDirty() & (1 << 17))
    {
        setSourceSampleOffset(audioSourcePtr->name, audioSourcePtr->getSampleOffset());
    }

    if(audioSourcePtr->getDirty() & (1 << 18))
    {
        setSourceByteOffset(audioSourcePtr->name, audioSourcePtr->getByteOffset());
    }

    audioSourcePtr->getDirtyModifiable() = 0;
}

void AudioModule::audioSourcePlayHandler(AudioSource* audioSourcePtr)   
{
    // if(std::find(playingSources.begin(), playingSources.end(), audioSourcePtr) == playingSources.end())
    // {
    //     playingSources.push_back(audioSourcePtr);
    // }
    
    alSourcePlay(audioSourcePtr->name);
    alCheckErrors();
}

void AudioModule::audioSourceStopHandler(AudioSource* audioSourcePtr)
{
    alSourceStop(audioSourcePtr->name);
    alCheckErrors();
}

void AudioModule::audioSourcePauseHandler(AudioSource* audioSourcePtr)
{
    alSourcePause(audioSourcePtr->name);
    alCheckErrors();
}

void AudioModule::audioSourceRewindHandler(AudioSource* audioSourcePtr)
{
    alSourceRewind(audioSourcePtr->name);
    alCheckErrors();
}

void AudioModule::receiveAudioDataHandler(AudioFile* audioFilePtr)
{
    auto buffer = audioBuffers.find(audioFilePtr->getFilePath());
    if(buffer == audioBuffers.end())
    {
        std::cout << "WARNING: Unknown audio data received! File: " << audioFilePtr->getFilePath() << std::endl;
        return;
    }

    loadAudioFileDataToBuffer(buffer->second, audioFilePtr->getChannelsCount(), audioFilePtr->getSampleRate(), audioFilePtr->getBitsPerSample(), &audioFilePtr->data, audioFilePtr->getSize());
}

#pragma endregion

#pragma region Event helpers

void AudioModule::audioSourceUpdateBufferHelper(AudioSource* audioSourcePtr)
{
    auto buffer = audioBuffers.find(audioSourcePtr->audioClip);
    alSourceQueueBuffers(audioSourcePtr->name, 1, &buffer->second);
    alCheckErrors();
    
    if(audioSourcePtr->autoPlay)
    {
        GetCore().messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PLAY, audioSourcePtr));
    }
}

#pragma endregion

#pragma region Buffers methods

void AudioModule::loadAudioFileDataToBuffer(ALuint bufferId, ALubyte channels, ALint sampleRate, ALubyte bitsPerSample, std::vector<ALchar>* data, ALsizei size)
{
    ALenum format;
    switch(channels)
    {
        case 1: // MONO
        {
            switch(bitsPerSample)
            {
            case 8:
                format = AL_FORMAT_MONO8;
                break;
            
            case 16:
                format = AL_FORMAT_MONO16;
                break;
            }
        }
        break;

        case 2: // STEREO
        {
            switch(bitsPerSample)
            {
            case 8:
                format = AL_FORMAT_STEREO8;
                break;
            
            case 16:
                format = AL_FORMAT_STEREO16;
                break;
            }
        }
        break;
    }

    setBufferData(bufferId, format, data->data(), size, sampleRate);
}

void AudioModule::setBufferData(ALuint bufferId, ALenum format, ALvoid* data, ALsizei size, ALsizei frequency)
{
    alBufferData(bufferId, format, data, size, frequency);
    alCheckErrors();
}

#pragma endregion

#pragma region Listener Getters

glm::vec3 AudioModule::getListenerPosition()
{
    glm::vec3 position;
    alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
    alCheckErrors();

    return position;
}

glm::vec3 AudioModule::getListenerVelocity()
{
    glm::vec3 velocity;
    alGetListener3f(AL_POSITION, &velocity.x, &velocity.y, &velocity.z);
    alCheckErrors();

    return velocity;
}

ALfloat AudioModule::getListenerGain()
{
    ALfloat gain;
    alGetListenerf(AL_GAIN, &gain);
    alCheckErrors();

    return gain;
}

void AudioModule::getListenerOrientation(glm::vec3* at, glm::vec3* up)
{
    ALfloat values[6];
    alGetListenerfv(AL_ORIENTATION, values);
    alCheckErrors();

    *at = { values[0], values[1], values[2] };
    *up = { values[3], values[4], values[5] };
}

#pragma endregion

#pragma region Source Getters

glm::vec3 AudioModule::getSourcePosition(ALuint sourceId)
{
    glm::vec3 position;
    alGetSource3f(sourceId, AL_POSITION, &position.x, &position.y, &position.z);
    alCheckErrors();
    return position;
}

glm::vec3 AudioModule::getSourceVelocity(ALuint sourceId)
{
    glm::vec3 velocity;
    alGetSource3f(sourceId, AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);
    alCheckErrors();
    return velocity;
}

ALfloat AudioModule::getSourceGain(ALuint sourceId)
{
    ALfloat gain;
    alGetSourcef(sourceId, AL_GAIN, &gain);
    alCheckErrors();
    return gain;
}

ALboolean AudioModule::getSourceIsRelative(ALuint sourceId)
{
    ALint isRelativeToListener;
    alGetSourcei(sourceId, AL_SOURCE_RELATIVE, &isRelativeToListener);
    alCheckErrors();
    return isRelativeToListener;
}

ALenum AudioModule::getSourceType(ALuint sourceId)
{
    ALenum sourceType;
    alGetSourcei(sourceId, AL_SOURCE_TYPE, &sourceType);
    alCheckErrors();
    return sourceType;
}

ALboolean AudioModule::getSourceIsLooping(ALuint sourceId)
{
    ALint isLooping;
    alGetSourcei(sourceId, AL_LOOPING, &isLooping);
    alCheckErrors();
    return isLooping;
}

ALuint AudioModule::getSourceCurrentBuffer(ALuint sourceId)
{
    ALint currentBuffer;
    alGetSourcei(sourceId, AL_BUFFER, &currentBuffer);
    alCheckErrors();
    return currentBuffer;
}

ALfloat AudioModule::getSourceMinGain(ALuint sourceId)
{
    ALfloat minGain;
    alGetSourcef(sourceId, AL_MIN_GAIN, &minGain);
    alCheckErrors();
    return minGain;
}

ALfloat AudioModule::getSourceMaxGain(ALuint sourceId)
{
    ALfloat maxGain;
    alGetSourcef(sourceId, AL_MAX_GAIN, &maxGain);
    alCheckErrors();
    return maxGain;
}

ALfloat AudioModule::getSourceReferenceDistance(ALuint sourceId)
{
    ALfloat referenceDistance;
    alGetSourcef(sourceId, AL_REFERENCE_DISTANCE, &referenceDistance);
    alCheckErrors();
    return referenceDistance;
}

ALfloat AudioModule::getSourceRolloffFactor(ALuint sourceId)
{
    ALfloat rolloffFactor;
    alGetSourcef(sourceId, AL_ROLLOFF_FACTOR, &rolloffFactor);
    alCheckErrors();
    return rolloffFactor;
}

ALfloat AudioModule::getSourceMaxDistance(ALuint sourceId)
{
    ALfloat maxDistance;
    alGetSourcef(sourceId, AL_MAX_DISTANCE, &maxDistance);
    alCheckErrors();
    return maxDistance;
}

ALfloat AudioModule::getSourcePitch(ALuint sourceId)
{
    ALfloat pitch;
    alGetSourcef(sourceId, AL_PITCH, &pitch);
    alCheckErrors();
    return pitch;
}

glm::vec3 AudioModule::getSourceDirection(ALuint sourceId)
{
    glm::vec3 direction;
    alGetSource3f(sourceId, AL_DIRECTION, &direction.x, &direction.y, &direction.z);
    alCheckErrors();
    return direction;
}

ALfloat AudioModule::getSourceConeInnerAngle(ALuint sourceId)
{
    ALfloat coneInnerAngle;
    alGetSourcef(sourceId, AL_CONE_INNER_ANGLE, &coneInnerAngle);
    alCheckErrors();
    return coneInnerAngle;
}

ALfloat AudioModule::getSourceConeOuterAngle(ALuint sourceId)
{
    ALfloat coneOuterAngle;
    alGetSourcef(sourceId, AL_CONE_OUTER_ANGLE, &coneOuterAngle);
    alCheckErrors();
    return coneOuterAngle;
}

ALfloat AudioModule::getSourceConeOuterGain(ALuint sourceId)
{
    ALfloat coneOuterGain;
    alGetSourcef(sourceId, AL_CONE_OUTER_GAIN, &coneOuterGain);
    alCheckErrors();
    return coneOuterGain;
}

ALfloat AudioModule::getSourceSecOffset(ALuint sourceId)
{
    ALfloat secOffset;
    alGetSourcef(sourceId, AL_SEC_OFFSET, &secOffset);
    alCheckErrors();
    return secOffset;
}

ALfloat AudioModule::getSourceSampleOffset(ALuint sourceId)
{
    ALfloat sampleOffset;
    alGetSourcef(sourceId, AL_SAMPLE_OFFSET, &sampleOffset);
    alCheckErrors();
    return sampleOffset;
}

ALfloat AudioModule::getSourceByteOffset(ALuint sourceId)
{
    ALfloat byteOffset;
    alGetSourcef(sourceId, AL_BYTE_OFFSET, &byteOffset);
    alCheckErrors();
    return byteOffset;
}

ALuint AudioModule::getSourceBuffersQueuedCount(ALuint sourceId)
{
    ALint buffersQueuedCount;
    alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &buffersQueuedCount);
    alCheckErrors();
    return buffersQueuedCount;
}

ALuint AudioModule::getSourceBuffersProcessedCount(ALuint sourceId)
{
    ALint buffersProcessedCount;
    alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessedCount);
    alCheckErrors();
    return buffersProcessedCount;
}

#pragma endregion

#pragma region Listener Setters

void AudioModule::setListenerPosition(glm::vec3 position)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);
    alCheckErrors();
}

void AudioModule::setListenerVelocity(glm::vec3 velocity)
{
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    alCheckErrors();
}

void AudioModule::setListenerGain(ALfloat gain)
{
    alListenerf(AL_GAIN, gain);
    alCheckErrors();
}

void AudioModule::setListenerOrientation(glm::vec3 at, glm::vec3 up)
{
    ALfloat values[] =
    {
        at.x, at.y, at.z,
        up.x, up.y, up.z
    };

    alListenerfv(AL_ORIENTATION, values);
    alCheckErrors();
}

#pragma endregion

#pragma region Source Setters

void AudioModule::setSourcePosition(ALuint sourceId, glm::vec3 position)
{
    alSource3f(sourceId, AL_POSITION, position.x, position.y, position.z);
    alCheckErrors();
}

void AudioModule::setSourceVelocity(ALuint sourceId, glm::vec3 velocity)
{
    alSource3f(sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    alCheckErrors();
}

void AudioModule::setSourceGain(ALuint sourceId, ALfloat gain)
{
    alSourcef(sourceId, AL_GAIN, gain);
    alCheckErrors();
}

void AudioModule::setSourceIsRelative(ALuint sourceId, ALboolean isRelativeToListener)
{
    alSourcei(sourceId, AL_SOURCE_RELATIVE, isRelativeToListener);
    alCheckErrors();
}

void AudioModule::setSourceIsLooping(ALuint sourceId, ALboolean isLooping)
{
    alSourcei(sourceId, AL_LOOPING, isLooping);
    alCheckErrors();
}

void AudioModule::setSourceCurrentBuffer(ALuint sourceId, ALuint bufferId)
{
    alSourcei(sourceId, AL_BUFFER, bufferId);
    alCheckErrors();
}

void AudioModule::setSourceMinGain(ALuint sourceId, ALfloat minGain)
{
    alSourcef(sourceId, AL_MIN_GAIN, minGain);
    alCheckErrors();
}

void AudioModule::setSourceMaxGain(ALuint sourceId, ALfloat maxGain)
{
    alSourcef(sourceId, AL_MAX_GAIN, maxGain);
    alCheckErrors();
}

void AudioModule::setSourceReferenceDistance(ALuint sourceId, ALfloat referenceDistance)
{
    alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
    alCheckErrors();
}

void AudioModule::setSourceRolloffFactor(ALuint sourceId, ALfloat rolloffFactor)
{
    alSourcef(sourceId, AL_ROLLOFF_FACTOR, rolloffFactor);
    alCheckErrors();
}

void AudioModule::setSourceMaxDistance(ALuint sourceId, ALfloat maxDistance)
{
    alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
    alCheckErrors();
}

void AudioModule::setSourcePitch(ALuint sourceId, ALfloat pitch)
{
    alSourcef(sourceId, AL_PITCH, pitch);
    alCheckErrors();
}

void AudioModule::setSourceDirection(ALuint sourceId, glm::vec3 direction)
{
    alSource3f(sourceId, AL_DIRECTION, direction.x, direction.y, direction.z);
    alCheckErrors();
}

void AudioModule::setSourceConeInnerAngle(ALuint sourceId, ALfloat coneInnerAngle)
{
    alSourcef(sourceId, AL_CONE_INNER_ANGLE, coneInnerAngle);
    alCheckErrors();
}

void AudioModule::setSourceConeOuterAngle(ALuint sourceId, ALfloat coneOuterAngle)
{
    alSourcef(sourceId, AL_CONE_OUTER_ANGLE, coneOuterAngle);
    alCheckErrors();
}

void AudioModule::setSourceConeOuterGain(ALuint sourceId, ALfloat coneOuterGain)
{
    alSourcef(sourceId, AL_CONE_OUTER_GAIN, coneOuterGain);
    alCheckErrors();
}

void AudioModule::setSourceSecOffset(ALuint sourceId, ALfloat secOffset)
{
    alSourcef(sourceId, AL_SEC_OFFSET, secOffset);
    alCheckErrors();
}

void AudioModule::setSourceSampleOffset(ALuint sourceId, ALfloat sampleOffset)
{
    alSourcef(sourceId, AL_SAMPLE_OFFSET, sampleOffset);
    alCheckErrors();
}

void AudioModule::setSourceByteOffset(ALuint sourceId, ALfloat byteOffset)
{
    alSourcef(sourceId, AL_BYTE_OFFSET, byteOffset);
    alCheckErrors();
}

#pragma endregion
