#ifndef _INSTANCEDPACKET_INL
#define _INSTANCEDPACKET_INL

#include <glad/glad.h>
#include <vector>

#include "RenderPacket.inl"

// TODO Documentation
struct InstancedPacket : RenderPacket
{
    static constexpr unsigned int INSTANCE_ALLOCATION = 32;

    InstancedPacket(Mesh* mesh, Material* material) : RenderPacket(mesh, material) 
    {
        instanceMatrices.reserve(INSTANCE_ALLOCATION);
    }
    InstancedPacket() = default;
    virtual ~InstancedPacket() = default;

    void render(glm::mat4& VP, unsigned int instanceBuffer)
    {
        material->use();
        // TODO: sending MVP instead of model matrices

        // ? +++++ Setting instance transformation array +++++
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size(), instanceMatrices.data(), GL_DYNAMIC_DRAW);

        mesh->renderInstanced(instanceMatrices.size());
    }

    std::vector<glm::mat4> instanceMatrices;
};

#endif // _INSTANCEDPACKET_INL
