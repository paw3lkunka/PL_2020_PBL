#ifndef _UIPACKET_INL
#define _UIPACKET_INL

#include "Material.hpp"
#include "UiQuad.hpp"

struct UiPacket
{
    UiPacket(UiQuad* mesh, Material* material) : 
    UiPacket() = default;
    virtual ~UiPacket() = default;

    void render(glm::mat4 projection);

    UiQuad* mesh;
    Material* material;
};

#endif // _UIPACKET_INL
