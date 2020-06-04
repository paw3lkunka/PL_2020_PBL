#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "mesh/Mesh.hpp"

//TODO documentation
struct MeshRenderer : public Component
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;

//TODO documentation
    //TODO may this be pointer?
    glm::mat4 modelMatrix;
    
//TODO documentation
    Material* material;
//TODO documentation
    Mesh* mesh;
};

#endif // _MESHRENDERER_INL
