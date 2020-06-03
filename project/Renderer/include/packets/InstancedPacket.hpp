#ifndef _INSTANCEDPACKET_INL
#define _INSTANCEDPACKET_INL

#include <glad/glad.h>
#include <vector>

#include "RenderPacket.inl"

// TODO Documentation
struct InstancedPacket : RenderPacket
{
    static constexpr unsigned int INSTANCE_ALLOCATION = 32;

    InstancedPacket(Mesh* mesh, Material* material);
    InstancedPacket() = default;
    virtual ~InstancedPacket() = default;

    void render(glm::mat4& VP);
    void renderWithShader(Shader* shader, glm::mat4& VP);

    std::vector<glm::mat4> instanceMatricesUnculled;
    std::vector<glm::mat4*> instanceMatrices;
    std::vector<bool> instanceOccluded;
};

#endif // _INSTANCEDPACKET_INL
