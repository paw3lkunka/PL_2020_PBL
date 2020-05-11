#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.inl"
#include "Material.hpp"
#include "mesh/Mesh.hpp"

struct MeshRenderer : public Component
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;

    glm::mat4 modelMatrix;
    
    Material* material;
    Mesh* mesh;
};

#endif // _MESHRENDERER_INL
