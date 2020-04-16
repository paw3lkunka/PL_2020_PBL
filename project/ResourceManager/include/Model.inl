#ifndef _MODEL_INL
#define _MODEL_INL

#include "Mesh.hpp"

struct Model
{
    // Model data
    std::vector< Mesh<Vertex_base> >  meshes;
    std::vector<unsigned char*> textures;
};

#endif // _MODEL_INL
