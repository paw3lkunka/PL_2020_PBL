#ifndef _UIRENDERER_INL
#define _UIRENDERER_INL

#include "Component.inl"
#include "Material.hpp"
#include "mesh/UiQuad.hpp"

struct UiRenderer : public Component
{
    UiRenderer() = default;
    virtual ~UiRenderer() = default;

    glm::mat3 modelMatrix;

    Material* material;
    Mesh* mesh;
};

#endif // _UIRENDERER_INL
