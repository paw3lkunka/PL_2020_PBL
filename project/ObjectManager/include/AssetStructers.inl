#ifndef ASSETSTRUCTERS_INL_
#define ASSETSTRUCTERS_INL_

struct TextureData
{
    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* data;
};

struct TextureHdrData
{
    int width = 0;
    int height = 0;
    int nrComponents = 0;
    float* data;
};

#endif /* !ASSETSTRUCTERS_INL_ */
