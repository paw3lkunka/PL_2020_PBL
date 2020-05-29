#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "mesh/Mesh.hpp"

struct MeshRenderer : public Component
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;

    //TODO may this be pointer?
    glm::mat4 modelMatrix;
    
    Material* material;
    Mesh* mesh;
};

#endif // _MESHRENDERER_INL
