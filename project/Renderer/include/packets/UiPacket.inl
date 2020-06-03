#ifndef _UIPACKET_INL
#define _UIPACKET_INL

#include "Material.hpp"

struct UiPacket
{
    UiPacket(Material* material, glm::mat3& modelMatrix) : material(material), modelMatrix(modelMatrix) {}
    virtual ~UiPacket() = default;

    virtual void render(glm::mat4& projection) = 0;

    glm::mat3 modelMatrix;
    Material* material;
};

#endif // _UIPACKET_INL
