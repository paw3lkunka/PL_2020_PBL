#ifndef _SPRITEPACKET_HPP
#define _SPRITEPACKET_HPP

#include "UiQuad.hpp"
#include "UiPacket.inl"

struct SpritePacket : public UiPacket
{
    SpritePacket(UiQuad* mesh, Material* material, glm::mat3& modelMatrix) : UiPacket(material, modelMatrix), mesh(mesh) {}
    virtual ~SpritePacket() = default;

    void render(glm::mat4& projection);

    UiQuad* mesh;
};

#endif // _SPRITEPACKET_HPP
