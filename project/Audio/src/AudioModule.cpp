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
    // try
    // {
    //     switch (msg.getEvent())
    //     {
    //     case Event::AUDIO_SOURCE_INIT:
    //         sources.push_back(msg.getValue<AudioSource*>());
    //     break;
    //     case Event::AUDIO_LISTENER_INIT:
    //     {
    //         auto audioListenerPtr = msg.getValue<AudioListener*>();
    //         audioListenerInitHandler(audioListenerPtr);
    //     }
    //         break;

    //     case Event::AUDIO_LISTENER_UPDATE:
    //     {
    //         auto audioListenerPtr = msg.getValue<AudioListener*>();
    //         audioListenerUpdateHandler(audioListenerPtr);
    //     }
    //         break;

    //     case Event::AUDIO_SOURCE_UPDATE_LISTENERS:
    //     {
    //         auto audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourceUpdateListenersHandler(audioSourcePtr);
    //     }
    //         break;

    //     case Event::AUDIO_SOURCE_UPDATE_ATTRIBUTES:
    //     {
    //         auto audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourceUpdateAttributesHandler(audioSourcePtr);
    //     }
    //         break;

    //     case Event::RECEIVE_AUDIO_DATA:
    //     {
    //         auto audioFile = msg.getValue<AudioFile*>();
    //         receiveAudioDataHandler(audioFile);
    //     }
    //         break;

    //     case Event::AUDIO_SOURCE_PLAY:
    //     {
    //         AudioSource* audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourcePlayHandler(audioSourcePtr);
    //     }
    //         break;

    //     case Event::AUDIO_SOURCE_STOP:
    //     {
    //         auto audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourceStopHandler(audioSourcePtr);
    //     }
    //         break;

    //     case Event::AUDIO_SOURCE_PAUSE:
    //     {
    //         auto audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourcePauseHandler(audioSourcePtr);
    //     }
    //         break;
        
    //     case Event::AUDIO_SOURCE_REWIND:
    //     {
    //         auto audioSourcePtr = msg.getValue<AudioSource*>();
    //         audioSourceRewindHandler(audioSourcePtr);
    //     }
    //         break;
    //     case Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING:
    //         paused = !paused;
    //         if(paused)
    //         {
    //             for(auto iter = playingSources.begin(); iter != playingSources.end(); ++iter)
    //             {
    //                 audioSourcePauseHandler(*iter);
    //             }
    //         }
    //         else
    //         {
    //             for(auto iter = playingSources.begin(); iter != playingSources.end(); ++iter)
    //             {
    //                 audioSourcePlayHandler(*iter);
    //             }
    //         }
    //     break;
    //     }

    //     if(msg.getEvent() >= Event::AUDIO_FIRST && msg.getEvent() <= Event::AUDIO_LAST)
    //     {
    //         alcPushCurrentContextChangesToDevice();
            
    //         for(auto iter = playingSources.begin(); iter != playingSources.end(); ++iter)
    //         {
    //             if((*iter)->names.size() > 0 && !(*iter)->getIsLooping())
    //             {
    //                 ALint state;
    //                 alGetSourcei((*iter)->names.begin()->second, AL_SOURCE_STATE, &state);
    //                 if(state == AL_STOPPED)
    //                 {
    //                     playingSources.erase(iter);
    //                 }
    //             }
    //         }
    //     }
    // }
    // catch(AudioContextLevelException e)
    // {
    //     std::cerr << e.what();
    // }
    // catch(AudioDeviceLevelException e)
    // {
    //     std::cerr << e.what();
    // }
    // catch(const std::exception& e)
    // {}
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

void AudioModule::unloadScene()
{
    try
    {
        for(auto src : sources)
        {
            for(auto name : src->names)
            {
                alcMakeContextCurrent(name.first->context);
                alSourcePlay(name.second);
                alSourceStop(name.second);
                alSourcei(name.second, AL_LOOPING, 0);
                ALuint* buffers = new ALuint;
                alSourceUnqueueBuffers(name.second, 1, buffers);
                alCheckErrors();
                alSourcei(name.second, AL_BUFFER, 0);
                alCheckErrors();
                alDeleteSources(1, &name.second);
                alCheckErrors();
                delete buffers;
                alcPushCurrentContextChangesToDevice();
            }
            for(auto clip : src->getClips())
            {
                auto iter = clips.find(clip);
                if(iter != clips.end())
                {
                    alDeleteBuffers(1, &iter->second);
                    alCheckErrors();
                }
            }
        }
        // Release and destroy the context
        auto releaseContextStatus = alcMakeContextCurrent(nullptr);
        if(releaseContextStatus == ALC_FALSE)
        {
            alcCheckErrors();
        }

        for(auto it = contexts.begin(); it != contexts.end(); it++)
        {
            alcDestroyContext(*it);
            alcCheckErrors();
        }
    }
    catch(AudioDeviceLevelException& e)
    {
        std::cerr << e.what();
    }
    catch(const std::exception& e)
    {}
    contexts.clear();
    clips.clear();
    sources.clear();
    queueCounters.clear();
    playingSources.clear();
}

void AudioModule::cleanup()
{
    try
    {
        unloadScene();

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
    if(currentListener)
    {
        alcProcessCurrentContext();
        alcSuspendCurrentContext();
    }
}

void AudioModule::alcProcessCurrentContext()
{
    alcProcessContext(currentListener->context);
    alcCheckErrors();
}

void AudioModule::alcSuspendCurrentContext()
{
    alcSuspendContext(currentListener->context);
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
    /*
    To create list of context's attributes you must prepare an array of pair<int, int>
    Each pair shall consist of valid attribute id as a key and its value.

    ALC_FREQUENCY; - Frequency for mixing output buffer in units of Hz
    ALC_REFRESH; - Refresh intervals in units of Hz
    ALC_SYNC; - Flag indicating a synchronous context
    ALC_MONO_SOURCES; - A hint indicating how many sources should be capable of supporting mono data
    ALC_STEREO_SOURCES; - A hint indicating how many sources should be capable of supporting stereo data 
    */
    auto contextAttrList = nullptr;
    
    // Create context for selected device with attributes
    audioListenerPtr->context = alcCreateContext(device, contextAttrList);
    if(audioListenerPtr->context == nullptr)
    {
        alcCheckErrors();
    }

    contexts.push_back(audioListenerPtr->context);

    if(audioListenerPtr->getIsCurrent())
    {
        audioListenerSetAsCurrentHelper(audioListenerPtr);
    }
}

void AudioModule::audioListenerUpdateHandler(AudioListener* audioListenerPtr)
{
    alcProcessCurrentContext();

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

    if(audioListenerPtr->getDirty() & (1 << 4))
    {
        alcPushCurrentContextChangesToDevice();

        if(audioListenerPtr->getIsCurrent())
        {
            audioListenerSetAsCurrentHelper(audioListenerPtr);
        }
        else
        {
            auto releseContextStatus = alcMakeContextCurrent(nullptr);
            if(releseContextStatus == ALC_FALSE)
            {
                alcCheckErrors();
            }

            currentListener->getIsCurrentModifiable() = false;
            currentListener = nullptr;
        }

        audioListenerPtr->getDirtyModifiable() &= ~(1 << 4);
    }

    alcSuspendCurrentContext();
}

void AudioModule::audioSourceUpdateListenersHandler(AudioSource* audioSourcePtr)
{
    auto tmpCurrent = currentListener;
    
    // Generte names for all AudioListeners that requires that
    for(auto it = audioSourcePtr->getListeners().begin(); it != audioSourcePtr->getListeners().end(); it++)
    {
        auto name = audioSourcePtr->names.find(*it);
        if(name == audioSourcePtr->names.end())
        {
            audioListenerSetAsCurrentHelper(*it);
            ALuint newName;
            alGenSources(1, &newName);
            
            audioSourcePtr->names.insert( std::pair<AudioListener*, ALuint>(*it, newName) );
            alcPushCurrentContextChangesToDevice();
        }
    }

    // Erase pairs of listeners and names which are no longer needed
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        auto listener = std::find(audioSourcePtr->getListeners().begin(), audioSourcePtr->getListeners().end(), it->first);
        if(listener == audioSourcePtr->getListeners().end())
        {
            audioListenerSetAsCurrentHelper(it->first);
            alDeleteSources(1, &(it->second));
            alCheckErrors();
            audioSourcePtr->names.erase(it);
            alcPushCurrentContextChangesToDevice();
        }
    }

    audioSourcePtr->getClipsModifiable();
    audioSourcePtr->getDirtyModifiable() &= ~(1 << 20);
    
    if(tmpCurrent)
    {
        audioListenerSetAsCurrentHelper(tmpCurrent);
    }
}

void AudioModule::audioSourceUpdateAttributesHandler(AudioSource* audioSourcePtr)
{
    if(audioSourcePtr->getDirty() & (1 << 19))
    {
        audioSourceUpdateBuffersHelper(audioSourcePtr);
    }
    
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        if(audioSourcePtr->getDirty() & (1 << 0))
        {
            setSourcePosition(it->second, audioSourcePtr->getPosition());
        }

        if(audioSourcePtr->getDirty() & (1 << 1))
        {
            setSourceVelocity(it->second, audioSourcePtr->getVelocity());
        }

        if(audioSourcePtr->getDirty() & (1 << 2))
        {
            setSourceGain(it->second, audioSourcePtr->getGain());
        }

        if(audioSourcePtr->getDirty() & (1 << 3))
        {
            setSourceIsRelative(it->second, audioSourcePtr->getIsRelative());
        }

        if(audioSourcePtr->getDirty() & (1 << 4))
        {
            setSourceIsLooping(it->second, audioSourcePtr->getIsLooping());
        }

        if(audioSourcePtr->getDirty() & (1 << 5))
        {
            setSourceCurrentBuffer(it->second, audioSourcePtr->getCurrentBuffer());
        }

        if(audioSourcePtr->getDirty() & (1 << 6))
        {
            setSourceMinGain(it->second, audioSourcePtr->getMinGain());
        }

        if(audioSourcePtr->getDirty() & (1 << 7))
        {
            setSourceMaxGain(it->second, audioSourcePtr->getMaxGain());
        }

        if(audioSourcePtr->getDirty() & (1 << 8))
        {
            setSourceReferenceDistance(it->second, audioSourcePtr->getReferenceDistance());
        }

        if(audioSourcePtr->getDirty() & (1 << 9))
        {
            setSourceRolloffFactor(it->second, audioSourcePtr->getRolloffFactor());
        }

        if(audioSourcePtr->getDirty() & (1 << 10))
        {
            setSourceMaxDistance(it->second, audioSourcePtr->getMaxDistance());
        }

        if(audioSourcePtr->getDirty() & (1 << 11))
        {
            setSourcePitch(it->second, audioSourcePtr->getPitch());
        }

        if(audioSourcePtr->getDirty() & (1 << 12))
        {
            setSourceDirection(it->second, audioSourcePtr->getDirection());
        }

        if(audioSourcePtr->getDirty() & (1 << 13))
        {
            setSourceConeInnerAngle(it->second, audioSourcePtr->getConeInnerAngle());
        }

        if(audioSourcePtr->getDirty() & (1 << 14))
        {
            setSourceConeOuterAngle(it->second, audioSourcePtr->getConeOuterAngle());
        }

        if(audioSourcePtr->getDirty() & (1 << 15))
        {
            setSourceConeOuterGain(it->second, audioSourcePtr->getConeOuterGain());
        }

        if(audioSourcePtr->getDirty() & (1 << 16))
        {
            setSourceSecOffset(it->second, audioSourcePtr->getSecOffset());
        }

        if(audioSourcePtr->getDirty() & (1 << 17))
        {
            setSourceSampleOffset(it->second, audioSourcePtr->getSampleOffset());
        }

        if(audioSourcePtr->getDirty() & (1 << 18))
        {
            setSourceByteOffset(it->second, audioSourcePtr->getByteOffset());
        }

        // (1 << 19) stands for updating buffers queues

        // (1 << 20) stands for AUDIO_SOURCE_UPDATE_LISTENERS event trigger
    }

    audioSourcePtr->getDirtyModifiable() &= ~(0b001111111111111111111);
}

void AudioModule::receiveAudioDataHandler(AudioFile* audioFilePtr)
{
    auto clip = clips.find(audioFilePtr->getFilePath());
    if(clip == clips.end())
    {
        return;
    }

    auto counter = queueCounters.find(clip->second);
    if(counter == queueCounters.end() || counter->second == 0)
    {
        loadAudioFileDataToBuffer(clip->second, audioFilePtr->getChannelsCount(), audioFilePtr->getSampleRate(), audioFilePtr->getBitsPerSample(), &audioFilePtr->data, audioFilePtr->getSize());
    }
    else
    {
        std::stringstream message;
        message << "Buffer " << clip->second << " is still queued. It cannot be filled with new data.\n";
        throw AudioDeviceLevelException(message.str());
    }
}

void AudioModule::audioSourcePlayHandler(AudioSource* audioSourcePtr)   
{
    if(audioSourcePtr->getDirty() & (1 << 19))
    {
        GetCore().getMessageBus().sendMessage( Message(Event::AUDIO_SOURCE_PLAY, audioSourcePtr) );
        return;
    }
    if(std::find(playingSources.begin(), playingSources.end(), audioSourcePtr) == playingSources.end())
    {
        playingSources.push_back(audioSourcePtr);
    }
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        alSourcePlay(it->second);
        alCheckErrors();
    }
}

void AudioModule::audioSourceStopHandler(AudioSource* audioSourcePtr)
{
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        alSourceStop(it->second);
        alCheckErrors();
    }
}

void AudioModule::audioSourcePauseHandler(AudioSource* audioSourcePtr)
{
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        alSourcePause(it->second);
        alCheckErrors();
    }
}

void AudioModule::audioSourceRewindHandler(AudioSource* audioSourcePtr)
{
    for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
    {
        alSourceRewind(it->second);
        alCheckErrors();
    }
}

#pragma endregion

#pragma region Event helpers

void AudioModule::audioListenerSetAsCurrentHelper(AudioListener* audioListenerPtr)
{
    if(currentListener != audioListenerPtr)
    {
        auto switchContextStatus = alcMakeContextCurrent(audioListenerPtr->context);
        if(switchContextStatus == ALC_FALSE)
        {
            alcCheckErrors();
        }
        
        if(currentListener)
        {
            currentListener->getIsCurrentModifiable() = false;
            currentListener->getDirtyModifiable() &= ~(1 << 4);
        }

        currentListener = audioListenerPtr;
    }
}

void AudioModule::audioSourceUpdateBuffersHelper(AudioSource* audioSourcePtr)
{
    ALboolean buffersReady = true;
    std::vector<ALuint> buffers = {};
    
    for(auto it = audioSourcePtr->getClips().begin(); it != audioSourcePtr->getClips().end() && buffersReady; it++)
    {
        auto buffer = clips.find(*it);
        if(buffer == clips.end())
        {
            ALuint name;
            alGenBuffers(1, &name);
            alCheckErrors();

            clips.insert(std::pair(*it, name));
            GetCore().getMessageBus().sendMessage( Message( Event::QUERY_AUDIO_DATA, it->c_str() ) );
        
            buffersReady = false;
        }
        else
        {
            buffers.push_back(buffer->second);
        }
    }

    if(buffersReady)
    {
        for(auto it = audioSourcePtr->names.begin(); it != audioSourcePtr->names.end(); it++)
        {
            // Uncomment if Rewind is not automatic
            //audioSourceRewindHandler(audioSourcePtr);
            alSourceUnqueueBuffers(it->second, audioSourcePtr->currentQueue.size(), audioSourcePtr->currentQueue.data());
            alCheckErrors();
            alSourceQueueBuffers(it->second, buffers.size(), buffers.data());
            alCheckErrors();
        }

        for(auto it = audioSourcePtr->currentQueue.begin(); it != audioSourcePtr->currentQueue.end(); it++)
        {
            auto counter = queueCounters.find(*it);
            counter->second--;
        }

        audioSourcePtr->currentQueue.clear();

        for(auto it = buffers.begin(); it != buffers.end(); it++)
        {
            auto counter = queueCounters.find(*it);
            if(counter != queueCounters.end())
            {
                counter->second++;
            }
            else
            {
                queueCounters.insert(std::pair(*it, 1));
            }
        }

        audioSourcePtr->currentQueue = buffers;
        audioSourcePtr->getDirtyModifiable() &= ~(1 << 19);
        if(audioSourcePtr->autoPlay)
        {
            GetCore().messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PLAY, audioSourcePtr));
        }
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
