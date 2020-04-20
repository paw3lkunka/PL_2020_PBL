#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.inl"
#include "ComponentType.inl"

#include "Mesh.hpp"
#include "Material.hpp"

struct MeshRenderer : public Component
{
    inline MeshRenderer() { type = ComponentType::MESH_RENDERER; }
    virtual ~MeshRenderer() = default;
    glm::mat4* modelMatrix;
    Mesh* mesh;
    Material* material;
};

#endif // _MESHRENDERER_INL
