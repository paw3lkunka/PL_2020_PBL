#ifndef _RENDERER_INL
#define _RENDERER_INL

#include "Component.inl"
#include "ComponentType.inl"

#include "Material.hpp"

struct Renderer : public Component
{
    inline Renderer() { type = ComponentType::RENDERER; }
    virtual ~Renderer() = default;

    glm::mat4 modelMatrix;
    
    Material* material;
};

#endif // _RENDERER_INL
