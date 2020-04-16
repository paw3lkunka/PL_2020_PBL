#ifndef _TEXTUREASSET_INL
#define _TEXTUREASSET_INL

struct TextureAsset
{
    const char* path;
    // Data container for storing texture byte by byte
    unsigned char* data;
};

#endif // _TEXTUREASSET_INL
