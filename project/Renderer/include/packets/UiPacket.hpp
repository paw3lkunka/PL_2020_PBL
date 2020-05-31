#ifndef _UIPACKET_INL
#define _UIPACKET_INL

#include "Material.hpp"
#include "UiQuad.hpp"

struct UiPacket
{
    UiPacket(UiQuad* mesh, Material* material, glm::mat3& modelMatrix) 
        : mesh(mesh), material(material), modelMatrix(modelMatrix) {}
    UiPacket() = default;
    virtual ~UiPacket() = default;

    void render(glm::mat4 projection);

    glm::mat3 modelMatrix;

    UiQuad* mesh;
    Material* material;
};

#endif // _UIPACKET_INL
