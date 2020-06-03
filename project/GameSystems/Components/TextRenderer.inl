#ifndef _TEXTRENDERER_INL
#define _TEXTRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "TextMesh.hpp"

struct TextRenderer : public Component
{
    TextRenderer()
    {
        mesh.setup();
    }
    virtual ~TextRenderer() = default;

    glm::mat3 modelMatrix;

    Material* material;
    TextMesh mesh;
};

#endif // _TEXTRENDERER_INL
