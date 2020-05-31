#ifndef _UIRENDERER_INL
#define _UIRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "mesh/UiQuad.hpp"

struct UiRenderer : public Component
{
    UiRenderer()
    {
        mesh = UiQuad(false);
    }
    virtual ~UiRenderer() = default;

    glm::mat3 modelMatrix;

    Material* material;
    UiQuad mesh;
};

#endif // _UIRENDERER_INL
