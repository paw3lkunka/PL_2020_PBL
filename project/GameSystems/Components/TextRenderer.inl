#ifndef _TEXTRENDERER_INL
#define _TEXTRENDERER_INL

#include "Component.hpp"
#include "Material.hpp"
#include "TextMesh.hpp"

//TODO documentation
struct TextRenderer : public Component
{
    TextRenderer()
    {
        mesh.setup();
    }
    virtual ~TextRenderer() = default;

//TODO documentation
    glm::mat3 modelMatrix;

//TODO documentation
    Material* material;
//TODO documentation
    TextMesh mesh;
};

#endif // _TEXTRENDERER_INL
