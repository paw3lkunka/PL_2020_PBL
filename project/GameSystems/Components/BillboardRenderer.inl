#ifndef _BILLBOARDRENDERER_INL
#define _BILLBOARDRENDERER_INL

#include "Component.inl"

#include "Material.hpp"

struct BillboardRenderer : public Component
{
    BillboardRenderer() = default;
    virtual ~BillboardRenderer() = default;

    glm::mat4 modelMatrix;
    
    Material* material;
};

#endif // _BILLBOARDRENDERER_INL
