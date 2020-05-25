#ifndef _RENDERPACKET_INL
#define _RENDERPACKET_INL

#include <glm/glm.hpp>

#include "Material.hpp"
#include "mesh/Mesh.hpp"

// TODO Documentation
struct RenderPacket
{
    RenderPacket(Mesh* mesh, Material* material) : mesh(mesh), material(material) {}
    RenderPacket() = default;
    virtual ~RenderPacket() = default;

    virtual void render(glm::mat4& VP) = 0;
    virtual void renderWithShader(Shader* shader, glm::mat4& VP) = 0;

    Mesh* mesh;
    Material* material;
};

#endif // _RENDERPACKET_INL
