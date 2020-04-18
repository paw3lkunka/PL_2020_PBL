#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.inl"
#include "Mesh.hpp"
#include "Material.hpp"

struct MeshRenderer : public Component
{
    glm::mat4* modelMatrix;
    Mesh* mesh;
    Material* material;
};

#endif // _MESHRENDERER_INL
