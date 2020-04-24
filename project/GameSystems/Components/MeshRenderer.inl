#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.inl"

#include "Renderer.inl"

struct MeshRenderer : public Renderer
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;
};

#endif // _MESHRENDERER_INL
