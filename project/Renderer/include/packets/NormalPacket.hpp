#ifndef _NORMALPACKET_HPP
#define _NORMALPACKET_HPP

#include "RenderPacket.inl"

// TODO Documentation
struct NormalPacket : RenderPacket
{
    NormalPacket(Mesh* mesh, Material* material, glm::mat4 model) : RenderPacket(mesh, material), modelMatrix(model) {}
    NormalPacket() = default;
    virtual ~NormalPacket() = default;

    void render(glm::mat4& VP);
    void renderWithShader(Shader* shader, glm::mat4& VP);

    glm::mat4 modelMatrix;
};

#endif // _NORMALPACKET_HPP
