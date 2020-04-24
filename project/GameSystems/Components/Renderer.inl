#ifndef _RENDERER_INL
#define _RENDERER_INL

#include "Component.inl"

#include "Material.hpp"
#include "Mesh.inl"

struct Renderer : public Component
{
    inline Renderer() = default;
    virtual ~Renderer() = default;

    glm::mat4 modelMatrix;
    
    Material* material;
    Mesh* mesh;
};

#endif // _RENDERER_INL
