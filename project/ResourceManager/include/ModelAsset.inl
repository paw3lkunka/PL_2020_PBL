#ifndef _MODELASSET_INL
#define _MODELASSET_INL

#include "Mesh.hpp"
#include "TextureAsset.inl"

struct ModelAsset
{
    // Model data
    std::vector< Mesh<Vertex_base> >  meshes;
    std::vector<TextureAsset> textures;
};

#endif // _MODELASSET_INL
