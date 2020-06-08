#ifndef _UIRENDERER_INL
#define _UIRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "mesh/UiQuad.hpp"

//TODO documentation
struct UiRenderer : public Component
{
    UiRenderer()
    {
        mesh = UiQuad(false);
    }
    virtual ~UiRenderer() = default;

//TODO documentation
    glm::mat3 modelMatrix;

//TODO documentation
    Material* material;
//TODO documentation
    UiQuad mesh;
};

#endif // _UIRENDERER_INL
