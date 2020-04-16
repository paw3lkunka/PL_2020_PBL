#ifndef _MODELASSET_INL
#define _MODELASSET_INL

#include "Mesh.h"
#include ""

struct ModelAsset
{
    const char* path;
    // Model data
    std::vector<MeshAsset> meshes;
    std::vector<TextureAsset> textures;
};

#endif // _MODELASSET_INL
