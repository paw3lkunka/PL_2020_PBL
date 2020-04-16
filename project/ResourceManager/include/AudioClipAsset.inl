#ifndef _AUDIOCLIPASSET_INL
#define _AUDIOCLIPASSET_INL

#include "AudioFile.h"

struct AudioClipAsset
{
    const char* path;
    // Audio file data
    AudioFile<float> data;
};

#endif // _AUDIOCLIPASSET_INL
