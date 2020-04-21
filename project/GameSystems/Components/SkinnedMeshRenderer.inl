#ifndef _SKINNEDMESHRENDERER_INL
#define _SKINNEDMESHRENDERER_INL

#include "MeshRenderer.inl"

struct SkinnedMeshRenderer : public MeshRenderer
{
    inline MeshRenderer() { type = ComponentType::SKINNED_MESH_RENDERER; }
    virtual ~MeshRenderer() = default;

    // TODO: Add animation handling here
};

#endif // _SKINNEDMESHRENDERER_INL
