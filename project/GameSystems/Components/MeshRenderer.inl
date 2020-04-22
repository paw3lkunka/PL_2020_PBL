#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.inl"

#include "Material.hpp"
#include "MeshCustom.hpp"

struct MeshRenderer : public Component
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;
    
    glm::mat4 modelMatrix;
    
    Material* material;
    MeshCustom* mesh;
};

#endif // _MESHRENDERER_INL
