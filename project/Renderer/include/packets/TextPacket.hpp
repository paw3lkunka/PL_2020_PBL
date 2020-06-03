#ifndef _TEXTPACKET_HPP
#define _TEXTPACKET_HPP

#include "Material.hpp"
#include "TextMesh.hpp"
#include "UiPacket.inl"

struct TextPacket : public UiPacket
{
    TextPacket(TextMesh* mesh, Material* material, glm::mat3& modelMatrix) : UiPacket(material, modelMatrix), mesh(mesh) {}
    virtual ~TextPacket() = default;

    void render(glm::mat4& projection);

    TextMesh* mesh;
};

#endif // _TEXTPACKET_HPP
